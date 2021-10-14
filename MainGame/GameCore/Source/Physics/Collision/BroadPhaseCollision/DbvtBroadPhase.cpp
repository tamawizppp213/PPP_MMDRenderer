//////////////////////////////////////////////////////////////////////////////////
///             @file   DbvtBroadPhase.cpp
///             @brief  DbvtBroadPhase
///             @author ----------------------
///             @date   2021_10_10 checked)
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
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/DBVTBroadPhase.hpp"
#include "GameCore/Include/Physics/Util/BulletThread.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
float g_DbvtMargin = 0.05f;
#pragma warning (disable: 6011)
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Helper Function
template <typename T>
static inline void ListAppend(T* item, T*& list)
{
	item->Links[0] = 0;
	item->Links[1] = list;
	if (list) list->Links[0] = item;
	list = item;
}
template <typename T>
static inline void ListRemove(T* item, T*& list)
{
	if (item->Links[0])
		item->Links[0]->Links[1] = item->Links[1];
	else
		list = item->Links[1];
	if (item->Links[1]) item->Links[1]->Links[0] = item->Links[0];
}
template<typename T>
static inline int ListCount(T* root)
{
	int n = 0;
	while (root)
	{
		++n;
		root = root->links[1];
	}
	return n;
}
template<typename T>
static inline void Clear(T* value)
{
	static const struct ZeroDummy : T
	{

	}zerodummy;
	value = zerodummy;
}
#pragma endregion Helper Function

#pragma region DbvtTreeCollider
struct DbvtTreeCollider : Dbvt::ICollide
{
	DbvtBroadPhase* BroadPhase;
	DbvtProxy*      Proxy;
	DbvtTreeCollider(DbvtBroadPhase* p) : BroadPhase(p) {};
	void Process(const DbvtNode* na, const DbvtNode* nb)
	{
		if (na != nb)
		{
			DbvtProxy* pa = (DbvtProxy*)na->Data;
			DbvtProxy* pb = (DbvtProxy*)nb->Data;
			BroadPhase->PairCache->AddOverlappingPair(pa, pb);
			++BroadPhase->NewPairs;
		}
	}

	void Process(const DbvtNode* node) { Process(node, Proxy->Leaf); }

};
#pragma endregion     DbvtTreeCollider
#pragma region BroadPhaseRayTester
struct BroadPhaseRayTester : Dbvt::ICollide
{
	BroadPhaseAabbCallBack& RayCallback;
	BroadPhaseRayTester(BroadPhaseRayCallBack& orgCallback) : RayCallback(orgCallback) {};
	void Process(const DbvtNode* leaf)
	{
		DbvtProxy* proxy = (DbvtProxy*)leaf->Data;
		RayCallback.Process(proxy);
	}
};
#pragma endregion  BroadPhaseRayTester
#pragma region BroadPhaseAABBTester
struct BroadPhaseAABBTester : Dbvt::ICollide
{
	BroadPhaseAabbCallBack& AABBCallback;
	BroadPhaseAABBTester(BroadPhaseAabbCallBack& callback) : AABBCallback(callback) {};
	void Process(const DbvtNode* leaf)
	{
		DbvtProxy* proxy = (DbvtProxy*)leaf->Data;
		AABBCallback.Process(proxy);
	}
};
#pragma endregion BroadPhaseAABBTester
#pragma region DbvtBroadPhase
DbvtBroadPhase::DbvtBroadPhase(IOverlappingPairCache* pairCache)
{
	DeferedCollide   = false;
	NeedCleanUP      = true;
	ReleasePairCache = (pairCache != 0) ? false : true;
	Prediction       = 0;
	StageCurrent     = 0;
	FixedLeft        = 0;
	FupDates         = 1;
	DupDates         = 0;
	CupDates         = 10;
	NewPairs         = 1;
	UpdatesCall      = 0;
	UpdatesDone      = 0;
	UpdatesRatio     = 0;
	PairCache        = pairCache ? pairCache : new (AlignedAllocInternal(sizeof(HashedOverlappingPairCache), 16)) HashedOverlappingPairCache();
	GenID            = 0;
	ParseID          = 0;
	CleanUpID        = 0;
	for (int i = 0; i <= DbvtBroadPhase::CountOfStage; ++i)
	{
		StageRoots[i] = 0;
	}
#if BT_THREAD_SAFE
	RayTestStacks.resize(BT_MAX_THREAD_COUNT);
#else
	RayTestStacks.resize(1);
#endif
#if DBVT_BP_PROFILE
	clear(m_profiling);
#endif
}

DbvtBroadPhase::~DbvtBroadPhase()
{
	if (ReleasePairCache)
	{
		PairCache->~IOverlappingPairCache();
		AlignedFreeInternal(PairCache);
	}
}

/****************************************************************************
*                       Collide
*************************************************************************//**
*  @fn        void DbvtBroadPhase::Collide(IDispatcher* dispatcher)
*  @brief     --
*  @param[in] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::Collide(IDispatcher* dispatcher)
{
	/*-------------------------------------------------------------------
	-           Optimize
	---------------------------------------------------------------------*/
	Sets[0].OptimizeIncremental(1 + (Sets[0].Leaves * DupDates) / 100);
	if (FixedLeft)
	{
		const int count = 1 + (Sets[1].Leaves * DupDates) / 100;
		Sets[1].OptimizeIncremental(1 + (Sets[1].Leaves * FupDates) / 100);
		FixedLeft = static_cast<int>(Max(0, FixedLeft - count));
	}

	/*-------------------------------------------------------------------
	-           Dynamic->Fixed set
	---------------------------------------------------------------------*/
	StageCurrent       = (StageCurrent + 1) & CountOfStage;
	DbvtProxy* current = StageRoots[StageCurrent];

	if (current)
	{
#if DBVT_BP_ACCURATESLEEPING
		DbvtTreeCollider collider(this);
#endif
		do
		{
			DbvtProxy* next = current->Links[1];
			ListRemove(current, StageRoots[current->Stage]);
			ListAppend(current, StageRoots[CountOfStage]);

			Sets[0].Remove(current->Leaf);
			ALIGNED16(DbvtVolume) currentAABB = DbvtVolume::FromMM(current->AABBMin, current->AABBMax);
			current->Leaf  = Sets[1].Insert(currentAABB, current);
			current->Stage = CountOfStage;
			current        = next;
		} while (current);

		FixedLeft   = Sets[1].Leaves;
		NeedCleanUP = true;
	}

	/*-------------------------------------------------------------------
	-           Collide dynamics
	---------------------------------------------------------------------*/
	DbvtTreeCollider collider(this);
	if (DeferedCollide)
	{
		Sets[0].CollideTTpersistentStack(Sets[0].Root, Sets[1].Root, collider);
	}
	if (DeferedCollide)
	{
		Sets[0].CollideTTpersistentStack(Sets[0].Root, Sets[0].Root, collider);
	}

	/*-------------------------------------------------------------------
	-           Clean up
	---------------------------------------------------------------------*/
	if (NeedCleanUP)
	{
		BroadPhasePairArray& pairs = PairCache->GetOverlappingPairArray();
		if (pairs.size() > 0)
		{
			int ni = static_cast<int>(Min(pairs.size(), static_cast<int>(Max(NewPairs, (pairs.size() * CupDates)))));
			for (int i = 0; i < ni; ++i)
			{
				BroadPhasePair& p = pairs[(CleanUpID + i) % pairs.size()];
				DbvtProxy* pa = (DbvtProxy*)p.Proxy0;
				DbvtProxy* pb = (DbvtProxy*)p.Proxy1;
				if (!Intersect(pa->Leaf->Volume, pb->Leaf->Volume))
				{
#if DBVT_BP_SORTPAIRS
					if (pa->m_uniqueId > pb->m_uniqueId)
						btSwap(pa, pb);
#endif
					PairCache->RemoveOverlappingPair(pa, pb, dispatcher);
					--ni;
					--i;
				}
			}
			if (pairs.size() > 0)
			{
				CleanUpID = (CleanUpID + ni) % pairs.size();
			}
			else
			{
				CleanUpID = 0;
			}
		}
	}
	++ParseID;
	NewPairs = 1;
	NeedCleanUP = false;
	if (UpdatesCall > 0)
	{
		UpdatesRatio = UpdatesDone / (float)UpdatesCall;
	}
	else
	{
		UpdatesRatio = 0;
	}
	UpdatesDone /= 2;
	UpdatesCall /= 2;
	
}
/****************************************************************************
*                       Optimize
*************************************************************************//**
*  @fn        void DbvtBroadPhase::Optimize()
*  @brief     Sets[0, 1] Optimize top down
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::Optimize()
{
	Sets[0].OptimizeTopDown();
	Sets[1].OptimizeTopDown();
	
}
/****************************************************************************
*                       CreateProxy
*************************************************************************//**
*  @fn        BroadPhaseProxy* DbvtBroadPhase::CreateProxy(const Vector3& aabbMin, const Vector3& aabbMax, int shapeType, void* userPtr, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher)
*  @brief     Create Dbvt proxy
*  @param[in] Vector3 aabbMin
*  @param[in] Vector3 aabbMax
*  @param[in] int shapeType
*  @param[in] void* userPtr, 
*  @param[in] int collisionFilterGroup, 
*  @param[in] int collisionFilterMask, 
*  @param[in] IDispatcher* dispatcher
*  @return 　　BroadPhaseProxy*
*****************************************************************************/
BroadPhaseProxy* DbvtBroadPhase::CreateProxy(const Vector3& aabbMin, const Vector3& aabbMax, int shapeType, void* userPtr, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher)
{
	DbvtProxy* proxy = new(AlignedAllocInternal(sizeof(DbvtProxy), 16)) DbvtProxy(aabbMin, aabbMax, userPtr, collisionFilterGroup, collisionFilterMask);
	DbvtAABBMm aabb  = DbvtVolume::FromMM(aabbMin, aabbMax);
	proxy->Stage     = StageCurrent;
	proxy->UniqueID  = ++GenID;
	proxy->Leaf      = Sets[0].Insert(aabb, proxy);
	ListAppend(proxy, StageRoots[StageCurrent]);

	if (!DeferedCollide)
	{
		DbvtTreeCollider collider(this);
		collider.Proxy = proxy;
		Sets[0].CollideTV(Sets[0].Root, aabb, collider);
		Sets[1].CollideTV(Sets[1].Root, aabb, collider);
	}
	return proxy;
}
/****************************************************************************
*                       DestroyProxy
*************************************************************************//**
*  @fn        void DbvtBroadPhase::DestroyProxy(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
*  @brief     Destroy BroadPhaseProxy
*  @param[in, out] BroadPhaseProxy* proxy
*  @param[in,out] IDispatcher dispatcher
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::DestroyProxy(BroadPhaseProxy* absProxy, IDispatcher* dispatcher)
{
	DbvtProxy* proxy = (DbvtProxy*)absProxy;
	if (proxy->Stage == CountOfStage) { Sets[1].Remove(proxy->Leaf); }
	else                              { Sets[0].Remove(proxy->Leaf); }
	ListRemove(proxy, StageRoots[proxy->Stage]);
	PairCache->RemoveOverlappingPairsContainingProxy(proxy, dispatcher);
	AlignedFreeInternal(proxy);
	NeedCleanUP = true;
}
/****************************************************************************
*                       SetAABB
*************************************************************************//**
*  @fn        void DbvtBroadPhase::SetAABB(BroadPhaseProxy* proxy, const Vector3& aabbMin, const Vector3& aabbMax, IDispatcher* dispatcher)
*  @brief     Set aabb
*  @param[in,out] BroadPhaseProxy* proxy
*  @param[in] Vector3 aabbMin
*  @param[in] Vector3 aabbMax
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::SetAABB(BroadPhaseProxy* absProxy, const Vector3& aabbMin, const Vector3& aabbMax, IDispatcher* dispatcher)
{
	DbvtProxy* proxy           = (DbvtProxy*)absProxy;
	ALIGNED16(DbvtVolume) aabb = DbvtVolume::FromMM(aabbMin, aabbMax);

	bool docollide = false;
	if (proxy->Stage == CountOfStage)
	{ /* fixed -> dynamic set	*/
		Sets[1].Remove(proxy->Leaf);
		proxy->Leaf = Sets[0].Insert(aabb, proxy);
		docollide = true;
	}
	else
	{ /* dynamic set				*/
		++UpdatesCall;
		if (Intersect(proxy->Leaf->Volume, aabb))
		{ /* Moving				*/

			const Vector3 delta = aabbMin - proxy->AABBMin;
			Vector3 velocity(((proxy->AABBMax - proxy->AABBMin) / 2) * Prediction);
			if (delta.GetX() < 0.0f) velocity.SetX(-velocity.GetX());
			if (delta.GetY() < 0.0f) velocity.SetY(-velocity.GetY());
			if (delta.GetZ() < 0.0f) velocity.SetZ(-velocity.GetZ());
			if ( Sets[0].Update(proxy->Leaf, aabb, velocity, g_DbvtMargin))
			{
				++UpdatesDone;
				docollide = true;
			}
		}
		else
		{ /* Teleporting			*/
			Sets[0].Update(proxy->Leaf, aabb);
			++UpdatesDone;
			docollide = true;
		}
	}
	ListRemove(proxy, StageRoots[proxy->Stage]);
	proxy->AABBMin = aabbMin;
	proxy->AABBMax = aabbMax;
	proxy->Stage = StageCurrent;
	ListAppend(proxy, StageRoots[StageCurrent]);
	if (docollide)
	{
		NeedCleanUP = true;
		if (!DeferedCollide)
		{
			DbvtTreeCollider collider(this);
			Sets[1].CollideTTpersistentStack(Sets[1].Root, proxy->Leaf, collider);
			Sets[0].CollideTTpersistentStack(Sets[0].Root, proxy->Leaf, collider);
		}
	}
}
/****************************************************************************
*                       RayTest
*************************************************************************//**
*  @fn        void DbvtBroadPhase::RayTest(const Vector3& rayFrom, const Vector3& rayTo, BroadPhaseRayCallBack& rayCallback, const Vector3& aabbMin, const Vector3& aabbMax)
*  @brief     Cast ray test
*  @param[in] Vector3 rayFrom
*  @param[in] Vector3 ratTo
*  @param[in,out] BroadPhaseRayCallBack callback
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::RayTest(const Vector3& rayFrom, const Vector3& rayTo, BroadPhaseRayCallBack& rayCallback, const Vector3& aabbMin, const Vector3& aabbMax)
{
	BroadPhaseRayTester callback(rayCallback);
	AlignedObjectArray<const DbvtNode*>* stack = &RayTestStacks[0];
#if BT_THREAD_SAFE 
	int threadIndex = bt::GetCurrentThreadIndex();
	AlignedObjectArray<const DbvtNode*> localStack;
	if (0)
	{
		// use per-thread preallocated stack if possible to avoid dynamic allocations
		stack = &RayTestStacks[threadIndex];
	}
	else
	{
		stack = &localStack;
	}
#endif

	Sets[0].RayTestInternal(Sets[0].Root, rayFrom, rayTo, rayCallback.RayDirectionInverse, rayCallback.Signs, rayCallback.LambdaMax, aabbMin, aabbMax, *stack, callback);
	Sets[1].RayTestInternal(Sets[1].Root, rayFrom, rayTo, rayCallback.RayDirectionInverse, rayCallback.Signs, rayCallback.LambdaMax, aabbMin, aabbMax, *stack, callback);
}
/****************************************************************************
*                       AABBTest
*************************************************************************//**
*  @fn        void DbvtBroadPhase::AABBTest(const Vector3& aabbMin, const Vector3& aabbMax, BroadPhaseAabbCallBack& callback)
*  @brief     --
*  @param[in] Vector3 aabbMin
*  @param[in] Vector3 aabbMax
*  @param[in,out] BroadPhaseAabbCallBack& callback
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::AABBTest(const Vector3& aabbMin, const Vector3& aabbMax, BroadPhaseAabbCallBack& aabbCallback)
{
	BroadPhaseAABBTester callback(aabbCallback);

	const ALIGNED16(DbvtVolume) bounds = DbvtVolume::FromMM(aabbMin, aabbMax);
	Sets[0].CollideTV(Sets[0].Root, bounds, callback);
	Sets[1].CollideTV(Sets[1].Root, bounds, callback);
}
/****************************************************************************
*                       GetAABB
*************************************************************************//**
*  @fn        void DbvtBroadPhase::GetAABB(BroadPhaseProxy* proxy, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
*  @brief     Get AABB
*  @param[in,out] BroadPhaseProxy* proxy
*  @param[out] Vector3 aabbMin
*  @param[out] Vector3 aabbMax
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::GetAABB(BroadPhaseProxy* absProxy, Vector3& aabbMin, Vector3& aabbMax) const
{
	DbvtProxy* proxy = (DbvtProxy*)absProxy;
	aabbMin = proxy->AABBMin;
	aabbMax = proxy->AABBMax;
}
/****************************************************************************
*                       CalculateOverlappingPairs
*************************************************************************//**
*  @fn        void DbvtBroadPhase::CalculateOverlappingPairs(IDispatcher* dispatcher)
*  @brief     Calculate overlapping pairs
*  @param[in,out] IDispatcher dispatcher
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::CalculateOverlappingPairs(IDispatcher* dispatcher)
{
	Collide(dispatcher);
	PerformDeferredRemoval(dispatcher);
}
/****************************************************************************
*                       GetOverlappingPairCache
*************************************************************************//**
*  @fn        IOverlappingPairCache* DbvtBroadPhase::GetOverlappingPairCache()
*  @brief     Get OverlappingPairCache
*  @param[in] void
*  @return 　　IOverlappingPairCache*
*****************************************************************************/
IOverlappingPairCache* DbvtBroadPhase::GetOverlappingPairCache()
{
	return PairCache;
}
/****************************************************************************
*                       GetOverlappingPairCache
*************************************************************************//**
*  @fn        const IOverlappingPairCache* DbvtBroadPhase::GetOverlappingPairCache() const
*  @brief     Get overlappingpairCache
*  @param[in] void
*  @return 　　const IOverlappingPairCache*
*****************************************************************************/
const IOverlappingPairCache* DbvtBroadPhase::GetOverlappingPairCache() const
{
	return PairCache;
}
/****************************************************************************
*                       GetBroadPhaseAABB
*************************************************************************//**
*  @fn        void DbvtBroadPhase::GetBroadPhaseAABB(Vector3& aabbMin, Vector3& aabbMax) const
*  @brief     Get broadphase aabb
*  @param[out] Vector3 aabbMin
*  @param[out] Vector3 aabbMax
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::GetBroadPhaseAABB(Vector3& aabbMin, Vector3& aabbMax) const
{
	ALIGNED16(DbvtVolume)bounds;

	if (!Sets[0].Empty())
	{
		if   (!Sets[1].Empty()) { Merge(Sets[0].Root->Volume, Sets[1].Root->Volume, bounds); }	
		else { bounds = Sets[0].Root->Volume; }
	}
	else if (!Sets[1].Empty())
	{
		bounds = Sets[1].Root->Volume;
	}
	else
	{
		bounds = DbvtVolume::FromCR(Vector3(0, 0, 0), 0);
	}
	aabbMin = bounds.Mins();
	aabbMax = bounds.Maxs();
}
/****************************************************************************
*                       PrintStats
*************************************************************************//**
*  @fn        void DbvtBroadPhase::PrintStats()
*  @brief     Not use
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::PrintStats()
{

}
/****************************************************************************
*                        ResetPool
*************************************************************************//**
*  @fn        void DbvtBroadPhase::ResetPool(IDispatcher* dispatcher)
*  @brief     Reset pool allocator. initialize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::ResetPool(IDispatcher* dispatcher)
{
	int totalObjects = Sets[0].Leaves + Sets[1].Leaves;
	if (!totalObjects)
	{
		Sets[0].Clear();
		Sets[1].Clear();

		DeferedCollide = false;
		NeedCleanUP    = true;
		StageCurrent   = 0;
		FixedLeft      = 0;
		FupDates       = 1;
		DupDates       = 0;
		CupDates       = 10;
		NewPairs       = 1;
		UpdatesCall    = 0;
		UpdatesDone    = 0;
		UpdatesRatio   = 0;
		GenID = 0;
		ParseID = 0;
		CleanUpID = 0;
		for (int i = 0; i <= CountOfStage; ++i)
		{
			StageRoots[i] = 0;
		}
	}
}
/****************************************************************************
*                       PerformDeferredRemoval
*************************************************************************//**
*  @fn        void DbvtBroadPhase::PerformDeferredRemoval(IDispatcher* dispatcher)
*  @brief     --
*  @param[in] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::PerformDeferredRemoval(IDispatcher* dispatcher)
{
	if (PairCache->HasDeferredRemoval())
	{
		BroadPhasePairArray& overlappingPairArray = PairCache->GetOverlappingPairArray();

		//perform a sort, to find duplicates and to sort 'invalid' pairs to the end
		overlappingPairArray.quickSort(BroadPhasePairSortPredicate());

		int invalidPair = 0;

		int i;

		BroadPhasePair previousPair;
		previousPair.Proxy0 = 0;
		previousPair.Proxy1 = 0;
		previousPair.Algorithm = 0;

		for (i = 0; i < overlappingPairArray.size(); i++)
		{
			BroadPhasePair& pair = overlappingPairArray[i];

			bool isDuplicate  = (pair == previousPair);
			previousPair      = pair;
			bool needsRemoval = false;

			if (!isDuplicate)
			{
				//important to perform AABB check that is consistent with the broadphase
				DbvtProxy* pa = (DbvtProxy*)pair.Proxy0;
				DbvtProxy* pb = (DbvtProxy*)pair.Proxy1;
				bool hasOverlap = Intersect(pa->Leaf->Volume, pb->Leaf->Volume);

				if (hasOverlap)
				{
					needsRemoval = false;
				}
				else
				{
					needsRemoval = true;
				}
			}
			else
			{
				//remove duplicate
				needsRemoval = true;
				//should have no algorithm
				assert(!pair.Algorithm);
			}

			if (needsRemoval)
			{
				PairCache->CleanOverlappingPair(pair, dispatcher);

				pair.Proxy0 = 0;
				pair.Proxy1 = 0;
				invalidPair++;
			}
		}

		//perform a sort, to sort 'invalid' pairs to the end
		overlappingPairArray.quickSort(BroadPhasePairSortPredicate());
		overlappingPairArray.resize(overlappingPairArray.size() - invalidPair);
	}
}
/****************************************************************************
*                       SetAABBForceUpdate
*************************************************************************//**
*  @fn        void DbvtBroadPhase::SetAABBForceUpdate(BroadPhaseProxy* absproxy, const Vector3& aabbMin, const Vector3& aabbMax, IDispatcher* /*dispatcher
*  @brief     --
*  @param[in] BroadPhaseProxy* absproxy
*  @param[in] Vector3 aabbMin
*  @param[in] Vector3 aabbMax
*  @param[in] IDispatcher* dispatcher (not use)
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::SetAABBForceUpdate(BroadPhaseProxy* absproxy, const Vector3& aabbMin, const Vector3& aabbMax, IDispatcher* /*dispatcher*/)
{
	DbvtProxy* proxy = (DbvtProxy*)absproxy;
	ALIGNED16(DbvtVolume) aabb = DbvtVolume::FromMM(aabbMin, aabbMax);
	bool docollide = false;
	if (proxy->Stage == CountOfStage)
	{ /* fixed -> dynamic set	*/
		Sets[1].Remove(proxy->Leaf);
		proxy->Leaf = Sets[0].Insert(aabb, proxy);
		docollide = true;
	}
	else
	{ /* dynamic set				*/
		++UpdatesCall;
		/* Teleporting			*/
		Sets[0].Update(proxy->Leaf, aabb);
		++UpdatesDone;
		docollide = true;
	}
	ListRemove(proxy, StageRoots[proxy->Stage]);
	proxy->AABBMin = aabbMin;
	proxy->AABBMax = aabbMax;
	proxy->Stage   = StageCurrent;
	ListAppend(proxy, StageRoots[StageCurrent]);
	if (docollide)
	{
		NeedCleanUP = true;
		if (!DeferedCollide)
		{
			DbvtTreeCollider collider(this);
			Sets[1].CollideTTpersistentStack(Sets[1].Root, proxy->Leaf, collider);
			Sets[0].CollideTTpersistentStack(Sets[0].Root, proxy->Leaf, collider);
		}
	}
}
/****************************************************************************
*                       Benchmark
*************************************************************************//**
*  @fn        void DbvtBroadPhase::Benchmark(IBroadPhase*)
*  @brief     Not use.
*  @param[in] IBroadPhase* (not use)
*  @return 　　void
*****************************************************************************/
void DbvtBroadPhase::Benchmark(IBroadPhase*)
{

}
#pragma endregion DbvtBroadPhase
#if DBVT_BP_PROFILE
#undef SPC
#endif