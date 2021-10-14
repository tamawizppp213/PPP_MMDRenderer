//////////////////////////////////////////////////////////////////////////////////
///             @file   OverlappingPair.cpp
///             @brief  OverlappingPair // Hash SpaceとNull は未実装
///             @author Partially Edit by Toide
///             @date  2021_09_15
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/OverlappingPair.hpp"
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/CollisionAlgorithm.hpp"
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/Dispatcher.hpp"
using namespace gm;
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define INVALID_ID (-1)

struct MyPairIndex
{
	int OrgIndex;
	int UniqueIDA0;
	int UniqueIDA1;
};

class MyPairIndeSortPredicate
{
public:
	bool operator()(const MyPairIndex& a, const MyPairIndex& b) const
	{
		const int uidA0 = a.UniqueIDA0;
		const int uidB0 = b.UniqueIDA0;
		const int uidA1 = a.UniqueIDA1;
		const int uidB1 = b.UniqueIDA1;
		return uidA0 > uidB0 || (uidA0 == uidB0 && uidA1 > uidB1);
	}
};
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region HashedOverlappingPairCache
HashedOverlappingPairCache::HashedOverlappingPairCache()
{
	int initialAllocatedSize = 2;
	_overlappingPairArray.reserve(initialAllocatedSize);
	GrowTables();
}
HashedOverlappingPairCache::~HashedOverlappingPairCache()
{

}
/****************************************************************************
*						RemoveOverlappingPairsContainingProxy
*************************************************************************//**
*  @fn        void  HashedOverlappingPairCache::RemoveOverlappingPairsContainingProxy(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
*  @brief     --
*  @param[in, out] BroadPhaseProxy* proxy, 
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void  HashedOverlappingPairCache::RemoveOverlappingPairsContainingProxy(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
{
	class RemovePairCallback : public OverlapCallback
	{
		BroadPhaseProxy* m_obsoleteProxy;

	public:
		RemovePairCallback(BroadPhaseProxy* obsoleteProxy)
			: m_obsoleteProxy(obsoleteProxy)
		{
		}
		virtual bool ProcessOverlap(BroadPhasePair& pair)
		{
			return ((pair.Proxy0 == m_obsoleteProxy) ||
				(pair.Proxy1 == m_obsoleteProxy));
		}
	};

	RemovePairCallback removeCallback(proxy);

	ProcessAllOverlappingPairs(&removeCallback, dispatcher);
}
/****************************************************************************
*						RemoveOverlappingPair
*************************************************************************//**
*  @fn        void* HashedOverlappingPairCache::RemoveOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, IDispatcher* dispatcher)
*  @brief     --
*  @param[in, out] BroadPhaseProxy* proxy0, 
*  @param[in, out] BroadPhaseProxy* proxy1, 
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void*
*****************************************************************************/
void* HashedOverlappingPairCache::RemoveOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, IDispatcher* dispatcher)
{
	if (proxy0->UniqueID > proxy1->UniqueID) { std::swap(proxy0, proxy1); };
	int proxyId1 = proxy0->GetUniqueID();
	int proxyId2 = proxy1->GetUniqueID();

	/*if (proxyId1 > proxyId2)
		btSwap(proxyId1, proxyId2);*/

	int hash = static_cast<int>(GetHash(static_cast<unsigned int>(proxyId1), static_cast<unsigned int>(proxyId2)) & (_overlappingPairArray.capacity() - 1));

	BroadPhasePair* pair = InternalFindPair(proxy0, proxy1, hash);
	if (pair == NULL) { return 0; }

	CleanOverlappingPair(*pair, dispatcher);

	void* userData = pair->InternalInfo1;

	assert(pair->Proxy0->GetUniqueID() == proxyId1);
	assert(pair->Proxy1->GetUniqueID() == proxyId2);

	int pairIndex = int(pair - &_overlappingPairArray[0]);
	assert(pairIndex < _overlappingPairArray.size());

	// Remove the pair from the hash table.
	int index = _hashTable[hash];
	assert(index != INVALID_ID);

	int previous = INVALID_ID;
	while (index != pairIndex)
	{
		previous = index;
		index = _next[index];
	}

	if (previous != INVALID_ID)
	{
		assert(_next[previous] == pairIndex);
		_next[previous] = _next[pairIndex];
	}
	else
	{
		_hashTable[hash] = _next[pairIndex];
	}

	// We now move the last pair into spot of the
	// pair being removed. We need to fix the hash
	// table indices to support the move.

	int lastPairIndex = _overlappingPairArray.size() - 1;

	if (_ghostPairCallback)
		_ghostPairCallback->RemoveOverlappingPair(proxy0, proxy1, dispatcher);

	// If the removed pair is the last pair, we are done.
	if (lastPairIndex == pairIndex)
	{
		_overlappingPairArray.pop_back();
		return userData;
	}

	// Remove the last pair from the hash table.
	const BroadPhasePair* last = &_overlappingPairArray[lastPairIndex];
	/* missing swap here too, Nat. */
	int lastHash = static_cast<int>(GetHash(static_cast<unsigned int>(last->Proxy0->GetUniqueID()), static_cast<unsigned int>(last->Proxy1->GetUniqueID())) & (_overlappingPairArray.capacity() - 1));

	index = _hashTable[lastHash];
	assert(index != INVALID_ID);

	previous = INVALID_ID;
	while (index != lastPairIndex)
	{
		previous = index;
		index = _next[index];
	}

	if (previous != INVALID_ID)
	{
		assert(_next[previous] == lastPairIndex);
		_next[previous] = _next[lastPairIndex];
	}
	else
	{
		_hashTable[lastHash] = _next[lastPairIndex];
	}

	// Copy the last pair into the remove pair's spot.
	_overlappingPairArray[pairIndex] = _overlappingPairArray[lastPairIndex];

	// Insert the last pair into the hash table
	_next[pairIndex] = _hashTable[lastHash];
	_hashTable[lastHash] = pairIndex;

	_overlappingPairArray.pop_back();

	return userData;
}
/****************************************************************************
*						NeedsBroadPhaseCollision
*************************************************************************//**
*  @fn        bool  HashedOverlappingPairCache::NeedsBroadPhaseCollision(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) const
*  @brief     --
*  @param[in, out] BroadPhaseProxy* proxy0,
*  @param[in, out] BroadPhaseProxy* proxy1,
*  @return 　　void
*****************************************************************************/
bool  HashedOverlappingPairCache::NeedsBroadPhaseCollision(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) const
{
	if (_overlapFilterCallback)
		return _overlapFilterCallback->NeedBroadPhaseCollision(proxy0, proxy1);

	bool collides = (proxy0->CollisionFilterGroup & proxy1->CollisionFilterMask) != 0;
	collides = collides && (proxy1->CollisionFilterGroup & proxy0->CollisionFilterMask);

	return collides;
}
/****************************************************************************
*						CleanProxyFromPairs
*************************************************************************//**
*  @fn        void  HashedOverlappingPairCache::CleanProxyFromPairs(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
*  @brief     --
*  @param[in, out] BroadPhaseProxy* proxy,
*  @param[in, out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void  HashedOverlappingPairCache::CleanProxyFromPairs(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
{
	class CleanPairCallback : public OverlapCallback
	{
		BroadPhaseProxy* _cleanProxy;
		IOverlappingPairCache* _pairCache;
		IDispatcher* _dispatcher;

	public:
		CleanPairCallback(BroadPhaseProxy* cleanProxy, IOverlappingPairCache* pairCache, IDispatcher* dispatcher)
			: _cleanProxy(cleanProxy),
			_pairCache(pairCache),
			_dispatcher(dispatcher)
		{
		}
		virtual bool ProcessOverlap(BroadPhasePair& pair)
		{
			if ((pair.Proxy0 == _cleanProxy) ||
				(pair.Proxy1 == _cleanProxy))
			{
				_pairCache->CleanOverlappingPair(pair, _dispatcher);
			}
			return false;
		}
	};

	CleanPairCallback cleanPairs(proxy, this, dispatcher);

	ProcessAllOverlappingPairs(&cleanPairs, dispatcher);
}
/****************************************************************************
*						ProcessAllOverlappingPairs
*************************************************************************//**
*  @fn        void  HashedOverlappingPairCache::ProcessAllOverlappingPairs(OverlapCallback*, IDispatcher* dispatcher)
*  @brief     --
*  @param[in] OverlapCallback*
*  @param[in, out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void  HashedOverlappingPairCache::ProcessAllOverlappingPairs(OverlapCallback* callback, IDispatcher* dispatcher)
{
	for (int i = 0; i < _overlappingPairArray.size();)
	{
		BroadPhasePair* pair = &_overlappingPairArray[i];
		if (callback->ProcessOverlap(*pair))
		{
			RemoveOverlappingPair(pair->Proxy0, pair->Proxy1, dispatcher);
		}
		else
		{
			++i;
		}
	}
}
/****************************************************************************
*						ProcessAllOverlappingPairs
*************************************************************************//**
*  @fn        void  HashedOverlappingPairCache::ProcessAllOverlappingPairs(OverlapCallback* callback, IDispatcher* dispatcher, const struct DispatcherInfo&)
*  @brief     --
*  @param[in, out] OverlapCallback* callback, 
*  @param[in, out] IDispatcher* dispatcher, 
*  @param[in]      struct DispatcherInfo&
*  @return 　　void
*****************************************************************************/
void  HashedOverlappingPairCache::ProcessAllOverlappingPairs(OverlapCallback* callback, IDispatcher* dispatcher, const struct DispatcherInfo& dispatchInfo)
{
	if (dispatchInfo.DeterministicOverlappingPairs)
	{
		BroadPhasePairArray& pa = GetOverlappingPairArray();
		gm::AlignedObjectArray<MyPairIndex> indices;
		{
			indices.resize(pa.size());
			for (int i = 0; i < indices.size(); i++)
			{
				const BroadPhasePair& p = pa[i];
				const int uidA0 = p.Proxy0 ? p.Proxy0->UniqueID : -1;
				const int uidA1 = p.Proxy1 ? p.Proxy1->UniqueID : -1;

				indices[i].UniqueIDA0 = uidA0;
				indices[i].UniqueIDA1 = uidA1;
				indices[i].OrgIndex = i;
			}
			indices.quickSort(MyPairIndeSortPredicate());
		}
		{
			int i;
			for (i = 0; i < indices.size();)
			{
				BroadPhasePair* pair = &pa[indices[i].OrgIndex];
				if (callback->ProcessOverlap(*pair))
				{
					RemoveOverlappingPair(pair->Proxy0, pair->Proxy1, dispatcher);
				}
				else
				{
					i++;
				}
			}
		}
	}
	else
	{
		ProcessAllOverlappingPairs(callback, dispatcher);
	}
}
/****************************************************************************
*						CleanOverlappingPair
*************************************************************************//**
*  @fn        void  HashedOverlappingPairCache::CleanOverlappingPair(BroadPhasePair& pair, IDispatcher* dispatcher)
*  @brief     --
*  @param[in,out] BroadPhasePair& pair
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void HashedOverlappingPairCache::CleanOverlappingPair(BroadPhasePair& pair, IDispatcher* dispatcher)
{
	if (pair.Algorithm && dispatcher)
	{
		{
			pair.Algorithm->~ICollisionAlgorithm();
			dispatcher->FreeCollisionAlgorithm(pair.Algorithm);
			pair.Algorithm = 0;
		}
	}
}
/****************************************************************************
*						      FindPair
*************************************************************************//**
*  @fn        BroadPhasePair* HashedOverlappingPairCache::FindPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
*  @brief     --
*  @param[in, out] BroadPhaseProxy* proxy0,
*  @param[in, out] BroadPhaseProxy* proxy1,
*  @return 　　BroadPhasePair*
*****************************************************************************/
BroadPhasePair* HashedOverlappingPairCache::FindPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
{
	if (proxy0->UniqueID > proxy1->UniqueID) { std::swap(proxy0, proxy1); }
	int proxyID1 = proxy0->GetUniqueID();
	int proxyID2 = proxy1->GetUniqueID();
	int hash = static_cast<int>(GetHash(static_cast<unsigned int>(proxyID1), static_cast<unsigned int>(proxyID2)) & (_overlappingPairArray.capacity() - 1));

	if (hash >= _hashTable.size()){ return NULL; }

	int index = _hashTable[hash];
	while (index != INVALID_ID && equalsPair(_overlappingPairArray[index], proxyID1, proxyID2) == false)
	{
		index = _next[index];
	}

	if (index == INVALID_ID)
	{
		return NULL;
	}

	assert(index < _overlappingPairArray.size());

	return &_overlappingPairArray[index];
}
/****************************************************************************
*						AddOverlappingPair
*************************************************************************//**
*  @fn        BroadPhasePair* HashedOverlappingPairCache::AddOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
*  @brief     --
*  @param[in, out] BroadPhaseProxy* proxy0,
*  @param[in, out] BroadPhaseProxy* proxy1,
*  @return 　　BroadPhasePair*
*****************************************************************************/
BroadPhasePair* HashedOverlappingPairCache::AddOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
{
	if (!NeedsBroadPhaseCollision(proxy0, proxy1))
		return 0;

	return InternalAddPair(proxy0, proxy1);
}
/****************************************************************************
*						InternalAddPair
*************************************************************************//**
*  @fn        BroadPhasePair* HashedOverlappingPairCache::InternalAddPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
*  @brief     --
*  @param[in, out] BroadPhaseProxy* proxy0,
*  @param[in,out] BroadPhaseProxy* proxy1
*  @return 　　BroadPhasePair*
*****************************************************************************/
BroadPhasePair* HashedOverlappingPairCache::InternalAddPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
{
	if (proxy0->UniqueID > proxy1->UniqueID) { std::swap(proxy0, proxy1); }
	int proxyId1 = proxy0->GetUniqueID();
	int proxyId2 = proxy1->GetUniqueID();

	int hash = static_cast<int>(GetHash(static_cast<unsigned int>(proxyId1), static_cast<unsigned int>(proxyId2)) & (_overlappingPairArray.capacity() - 1));  // New hash value with new mask

	BroadPhasePair* pair = InternalFindPair(proxy0, proxy1, hash);
	if (pair != NULL) { return pair; }

	int count       = _overlappingPairArray.size();
	int oldCapacity = _overlappingPairArray.capacity();
	void* memory    = &_overlappingPairArray.expandNonInitializing();

	if (_ghostPairCallback) { _ghostPairCallback->AddOverlappingPair(proxy0, proxy1); }
	int newCapacity = _overlappingPairArray.capacity();

	if (oldCapacity < newCapacity)
	{
		GrowTables();
		hash = static_cast<int>(GetHash(static_cast<unsigned int>(proxyId1), static_cast<unsigned int>(proxyId2)) & (_overlappingPairArray.capacity() - 1));
	}

	pair = new(memory)BroadPhasePair(*proxy0, *proxy1);
	pair->Algorithm         = 0;
	pair->InternalTempValue = 0;
	_next[count]            = _hashTable[hash];
	_hashTable[hash]        = count;
	return pair;
}
/****************************************************************************
*						InternalFindPair
*************************************************************************//**
*  @fn        BroadPhasePair* HashedOverlappingPairCache::InternalFindPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, int hash)
*  @brief     --
*  @param[in, out] BroadPhaseProxy* proxy0, 
*  @param[in, out] BroadPhaseProxy* proxy1, 
*  @param[in]      int hash
*  @return 　　BroadPhasePair*
*****************************************************************************/
BroadPhasePair* HashedOverlappingPairCache::InternalFindPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, int hash)
{
	int proxyId1 = proxy0->GetUniqueID();
	int proxyId2 = proxy1->GetUniqueID();
#if 0  // wrong, 'equalsPair' use unsorted uids, copy-past devil striked again. Nat.
	if (proxyId1 > proxyId2)
		btSwap(proxyId1, proxyId2);
#endif

	int index = _hashTable[hash];

	while (index != INVALID_ID && equalsPair(_overlappingPairArray[index], proxyId1, proxyId2) == false)
	{
		index = _next[index];
	}

	if (index == INVALID_ID)
	{
		return NULL;
	}

	assert(index < _overlappingPairArray.size());

	return &_overlappingPairArray[index];
}
/****************************************************************************
*						GrowTables
*************************************************************************//**
*  @fn        void HashedOverlappingPairCache::GrowTables()
*  @brief     --
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void HashedOverlappingPairCache::GrowTables()
{
	int newCapacity = _overlappingPairArray.capacity();

	if (_hashTable.size() < newCapacity)
	{
		//grow hashtable and next table
		int curHashtableSize = _hashTable.size();

		_hashTable.resize(newCapacity);
		_next.resize(newCapacity);

		int i;

		for (i = 0; i < newCapacity; ++i)
		{
			_hashTable[i] = INVALID_ID;
		}
		for (i = 0; i < newCapacity; ++i)
		{
			_next[i] = INVALID_ID;
		}

		for (i = 0; i < curHashtableSize; i++)
		{
			const BroadPhasePair& pair = _overlappingPairArray[i];
			int proxyId1 = pair.Proxy0->GetUniqueID();
			int proxyId2 = pair.Proxy1->GetUniqueID();
			/*if (proxyId1 > proxyId2)
				btSwap(proxyId1, proxyId2);*/
			int hashValue = static_cast<int>(GetHash(static_cast<unsigned int>(proxyId1), static_cast<unsigned int>(proxyId2)) & (_overlappingPairArray.capacity() - 1));  // New hash value with new mask
			_next[i] = _hashTable[hashValue];
			_hashTable[hashValue] = i;
		}
	}
}
/****************************************************************************
*						SortOverlappingPairs
*************************************************************************//**
*  @fn        void HashedOverlappingPairCache::SortOverlappingPairs(IDispatcher* dispatcher)
*  @brief     --
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void HashedOverlappingPairCache::SortOverlappingPairs(IDispatcher* dispatcher)
{
	BroadPhasePairArray tempPairs;
	for (int i = 0; i < _overlappingPairArray.size(); ++i) { tempPairs.push_back(_overlappingPairArray[i]); }
	for (int i = 0; i < tempPairs.size(); ++i) { RemoveOverlappingPair(tempPairs[i].Proxy0, tempPairs[i].Proxy1, dispatcher); }
	for (int i = 0; i < _next.size()    ; ++i) { _next[i] = INVALID_ID; }

	tempPairs.quickSort(BroadPhasePairSortPredicate());
	for (int i = 0; i < tempPairs.size(); ++i) { AddOverlappingPair(tempPairs[i].Proxy0, tempPairs[i].Proxy1); }
}
#pragma endregion HashedOVerlappingPairCache
#pragma region SortedOverlappingPairCache
SortedOverlappingPairCache::SortedOverlappingPairCache()
{
	_blockedForChanges     = false;
	_hasDefferedRemoval    = true;
	_overlapFilterCallback = nullptr;
	_ghostPairCallback     = nullptr;
	_overlappingPairArray.reserve(2);
}

SortedOverlappingPairCache::~SortedOverlappingPairCache(){}
/****************************************************************************
*                       RemoveOverlappingPair
*************************************************************************//**
*  @fn        void* SortedOverlappingPairCache::RemoveOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, Dispatcher* dispatcher)
*  @brief     Remove Overlapping Pair
*  @param[in] BroadPhaseProxy* proxy0, 
*  @param[in] BroadPhaseProxy* proxy1, 
*  @param[in] Dispatcher* dispatcher
*  @return 　　void*
*****************************************************************************/
void* SortedOverlappingPairCache::RemoveOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, IDispatcher* dispatcher)
{
	if (!HasDeferredRemoval())
	{
		BroadPhasePair findPair(*proxy0, *proxy1);

		int findIndex = _overlappingPairArray.findLinearSearch(findPair);
		if (findIndex < _overlappingPairArray.size())
		{
			BroadPhasePair& pair = _overlappingPairArray[findIndex];
			void* userData       = pair.InternalInfo1;
			CleanOverlappingPair(pair, dispatcher);
			
			if (_ghostPairCallback)
			{
				//_ghostPairCallback->RemoveOverlappingPair(proxy0, proxy1, dispatcher);
			}

			_overlappingPairArray.swap(findIndex, _overlappingPairArray.capacity() - 1);
			_overlappingPairArray.pop_back();
			return userData;
		}
	}

	return 0;
}
/****************************************************************************
*                       AddOverlappingPair
*************************************************************************//**
*  @fn        BroadPhasePair* SortedOverlappingPairCache::AddOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
*  @brief     Add Overlapping Pair
*  @param[in] BroadPhaseProxy* proxy0,
*  @param[in] BroadPhaseProxy* proxy1,
*  @return 　　void*
*****************************************************************************/
BroadPhasePair* SortedOverlappingPairCache::AddOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
{
	assert(proxy0 != proxy1);

	if (!NeedsBroadPhaseCollision(proxy0, proxy1)) { return 0; }
	
	void* memory = &_overlappingPairArray.expandNonInitializing();
	BroadPhasePair* pair = new (memory) BroadPhasePair(*proxy0, *proxy1);
	
	if (_ghostPairCallback)
	{
		_ghostPairCallback->AddOverlappingPair(proxy0, proxy1);
	}
	return pair;
}

/****************************************************************************
*                       ProcessAllOverlappingPairs
*************************************************************************//**
*  @fn        void SortedOverlappingPairCache::ProcessAllOverlappingPairs(OverlapCallback* callback, Dispatcher* dispatcher)
*  @brief     Processing Overlapping Pair
*  @param[in] OverlapCallback* callback, 
*  @param[in] Dispatcher* dispatcher
*  @return 　　void*
*****************************************************************************/
void SortedOverlappingPairCache::ProcessAllOverlappingPairs(OverlapCallback* callback, IDispatcher* dispatcher)
{
	int i;

	for (i = 0; i < _overlappingPairArray.size();)
	{
		BroadPhasePair* pair = &_overlappingPairArray[i];
		if (callback->ProcessOverlap(*pair))
		{
			CleanOverlappingPair(*pair, dispatcher);
			pair->Proxy0 = nullptr;
			pair->Proxy1 = nullptr;
			_overlappingPairArray.swap(i, _overlappingPairArray.size() - 1);
			_overlappingPairArray.pop_back();
		}
		else
		{
			i++;
		}
	}
}

/****************************************************************************
*                       CleanOverlappingPair
*************************************************************************//**
*  @fn        void SortedOverlappingPairCache::CleanOverlappingPair(BroadPhasePair& pair, IDispatcher* dispatcher)
*  @brief     Clean Overlapping Pair
*  @param[out] BroadPhasePair& pair,
*  @param[in] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void SortedOverlappingPairCache::CleanOverlappingPair(BroadPhasePair& pair, IDispatcher* dispatcher)
{
	if (pair.Algorithm)
	{
		pair.Algorithm->~ICollisionAlgorithm();
		dispatcher->FreeCollisionAlgorithm(pair.Algorithm);
		pair.Algorithm = NULL;
	}
}

/****************************************************************************
*                            FindPair
*************************************************************************//**
*  @fn        BroadPhasePair* SortedOverlappingPairCache::FindPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
*  @brief     Find Pair
*  @param[in] BroadPhaseProxy* proxy0,
*  @param[in] BroadPhaseProxy* proxy1,
*  @return 　　void*
*****************************************************************************/
BroadPhasePair* SortedOverlappingPairCache::FindPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
{
	if (!NeedsBroadPhaseCollision(proxy0, proxy1)) { return 0; }

	BroadPhasePair tempPair(*proxy0, *proxy1);
	int findIndex = _overlappingPairArray.findLinearSearch(tempPair);

	if (findIndex < _overlappingPairArray.size())
	{
		BroadPhasePair* pair = &_overlappingPairArray[findIndex];
		return pair;
	}
	return NULL;
}

/****************************************************************************
*                       CleanProxyFromPairs
*************************************************************************//**
*  @fn        void SortedOverlappingPairCache::CleanProxyFromPairs(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
*  @brief     Clean Proxy From Pair
*  @param[in] BroadPhaseProxy* proxy0,
*  @param[in] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void SortedOverlappingPairCache::CleanProxyFromPairs(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
{
	class CleanPairCallback : public OverlapCallback
	{
		BroadPhaseProxy*       _cleanProxy;
		IOverlappingPairCache* _pairCache ;
		IDispatcher*           _dispatcher;

	public:
		CleanPairCallback(BroadPhaseProxy* cleanProxy, IOverlappingPairCache* pairCache, IDispatcher* dispatcher)
			: _cleanProxy(cleanProxy),
			  _pairCache (pairCache),
			  _dispatcher(dispatcher){}
		virtual bool ProcessOverlap(BroadPhasePair& pair)
		{
			if ((pair.Proxy0 == _cleanProxy) ||
				(pair.Proxy1 == _cleanProxy))
			{
				_pairCache->CleanOverlappingPair(pair, _dispatcher);
			}
			return false;
		}
	};

	CleanPairCallback cleanPairs(proxy, this, dispatcher);
	ProcessAllOverlappingPairs(&cleanPairs, dispatcher);
}

/****************************************************************************
*                       RemoveOverlappingPairsContainingProxy
*************************************************************************//**
*  @fn        void SortedOverlappingPairCache::RemoveOverlappingPairsContainingProxy(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
*  @brief     Remove Overlapping Pairs Containing Proxy
*  @param[in] BroadPhaseProxy* proxy,
*  @param[in] IDispatcher*     dispatcher
*  @return 　　void
*****************************************************************************/
void SortedOverlappingPairCache::RemoveOverlappingPairsContainingProxy(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
{
	class RemovePairCallback : public OverlapCallback
	{
		BroadPhaseProxy* m_obsoleteProxy;

	public:
		RemovePairCallback(BroadPhaseProxy* obsoleteProxy)
			: m_obsoleteProxy(obsoleteProxy)
		{
		}
		virtual bool ProcessOverlap(BroadPhasePair& pair)
		{
			return ((pair.Proxy0 == m_obsoleteProxy) ||
				    (pair.Proxy1 == m_obsoleteProxy));
		}
	};

	RemovePairCallback removeCallback(proxy);
	ProcessAllOverlappingPairs(&removeCallback, dispatcher);
}

void SortedOverlappingPairCache::SortOverlappingPairs(IDispatcher* dispatcher)
{

}

/****************************************************************************
*                       NeedsBroadPhaseCollision
*************************************************************************//**
*  @fn        bool SortedOverlappingPairCache::NeedsBroadPhaseCollision(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) const
*  @brief     Needs Broad Phase Collision
*  @param[in] BroadPhaseProxy* proxy0,
*  @param[in] BroadPhaseProxy* proxy1
*  @return 　　void
*****************************************************************************/
bool SortedOverlappingPairCache::NeedsBroadPhaseCollision(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) const
{
	if (_overlapFilterCallback)
	{
		return _overlapFilterCallback->NeedBroadPhaseCollision(proxy0, proxy1);
	}

	bool collides = (proxy0->CollisionFilterGroup & proxy1->CollisionFilterMask) != 0;
	collides = collides && (proxy1->CollisionFilterGroup & proxy0->CollisionFilterMask);
	return collides;
}

#pragma endregion SortedOverlappingPairCache