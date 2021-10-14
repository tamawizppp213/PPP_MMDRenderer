//////////////////////////////////////////////////////////////////////////////////
///             @file   OverlappingPair.hpp
///             @brief  OverlappingPair // Hash Space‚ÆNull ‚Í–¢ŽÀ‘•
///             @author Partially Edit by Toide
///             @date   2021_09_15
///                     2021_10_10(second checked)
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
#pragma once
#ifndef OVERLAPPING_PAIR_HPP
#define OVERLAPPING_PAIR_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "BroadPhaseInterface.hpp"
#include "BroadPhaseProxy.hpp"
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
const int NULL_PAIR = 0xffffffff;

class IDispatcher;
using BroadPhasePairArray = gm::AlignedObjectArray<BroadPhasePair>;

//////////////////////////////////////////////////////////////////////////////////
//                             OverlappingPair
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  		OverlappingPairCallback
*************************************************************************//**
*  @class    OverlappingPairCallback
*  @brief    The OverlappingPairCallback class is an additional optional broadphase user callback for adding/removing overlapping pairs, similar interface to OverlappingPairCache.
*****************************************************************************/
class OverlappingPairCallback
{
public:
	virtual ~OverlappingPairCallback() {};
	virtual BroadPhasePair* AddOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) = 0;
	virtual void* RemoveOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, IDispatcher* dispatcher) = 0;
	virtual void  RemoveOverlappingPairsContainingProxy(BroadPhaseProxy* proxy0, IDispatcher* dispatcher) = 0;
protected:
	OverlappingPairCallback() {};
};

struct OverlapCallback
{
	virtual ~OverlapCallback() {};
	virtual bool ProcessOverlap(BroadPhasePair& pair) = 0; // return true for deletion of the pair
};

struct OverlapFilterCallback
{
	virtual ~OverlapFilterCallback() {};
	virtual bool NeedBroadPhaseCollision(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) const = 0;
};

/****************************************************************************
*				  		IOverlappingPairCache
*************************************************************************//**
*  @class    IOverlappingPairCache
*  @brief    The OverlappingPairCallback class is an additional optional broadphase user callback for adding/removing overlapping pairs, similar interface to OverlappingPairCache.
*****************************************************************************/
class IOverlappingPairCache : public OverlappingPairCallback
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual bool NeedsBroadPhaseCollision  (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) const = 0;
	virtual void ProcessAllOverlappingPairs(OverlapCallback* callback, IDispatcher* dispatcher) = 0;
	virtual void ProcessAllOverlappingPairs(OverlapCallback* callback, IDispatcher* dispatcher, const struct DispatcherInfo&) { ProcessAllOverlappingPairs(callback, dispatcher); }
	virtual BroadPhasePair* FindPair       (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) = 0;
	virtual void SortOverlappingPairs(IDispatcher* dispatcher) = 0;


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual BroadPhasePair*        GetOverlappingPairArrayPtr()        = 0;
	virtual const BroadPhasePair*  GetOverlappingPairArrayPtr() const  = 0;
	virtual BroadPhasePairArray&   GetOverlappingPairArray()           = 0;
	virtual OverlapFilterCallback* GetOverlapFilterCallback()          = 0;

	virtual void SetOverlapFilterCallback(OverlapFilterCallback* callback) = 0;

	virtual void CleanOverlappingPair(BroadPhasePair& pair, IDispatcher* dispatcher) = 0;
	virtual void CleanProxyFromPairs(BroadPhaseProxy* proxy,IDispatcher* dispatcher) = 0;
	virtual int  GetNumOverlappingPairs() const = 0;
	virtual bool HasDeferredRemoval    () = 0;
	virtual void SetInternalGhostPairCallback(OverlappingPairCallback* ghostPairCallback) = 0;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	virtual ~IOverlappingPairCache() {};  // this is needed so we can get to the derived class destructor


protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
};

/****************************************************************************
*				  		SortedOverlappingPairCache
*************************************************************************//**
*  @class    SortedOverlappingPairCache
*  @brief    SortedOverlappingPairCache maintains the objects with overlapping AABB
*            Typically managed by the Broadphase, Axis3Sweep or btSimpleBroadphase
*****************************************************************************/
__declspec(align(16))
class HashedOverlappingPairCache : public IOverlappingPairCache
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  RemoveOverlappingPairsContainingProxy(BroadPhaseProxy* proxy, IDispatcher* dispatcher) override;
	void* RemoveOverlappingPair     (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, IDispatcher* dispatcher) override;
	bool  NeedsBroadPhaseCollision  (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)   const override;
	void  CleanProxyFromPairs       (BroadPhaseProxy* proxy , IDispatcher* dispatcher)   override;
	void  ProcessAllOverlappingPairs(OverlapCallback*, IDispatcher* dispatcher)          override;
	void  ProcessAllOverlappingPairs(OverlapCallback* callback, IDispatcher* dispatcher, const struct DispatcherInfo& dispatchInfo)override;
	void  CleanOverlappingPair      (BroadPhasePair& pair, IDispatcher* dispatcher)      override;
	BroadPhasePair* FindPair          (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) override;
	BroadPhasePair* AddOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	BroadPhasePair*       GetOverlappingPairArrayPtr  ()       override { return &_overlappingPairArray[0]; }
	const BroadPhasePair* GetOverlappingPairArrayPtr  () const override { return &_overlappingPairArray[0]; }
	BroadPhasePairArray&  GetOverlappingPairArray     () override       { return _overlappingPairArray; }
	const BroadPhasePairArray& GetOverlappingPairArray() const          { return _overlappingPairArray; }
	OverlapFilterCallback* GetOverlapFilterCallback()          override  { return _overlapFilterCallback; }
	int GetCount()                                       const          { return _overlappingPairArray.size(); }
	int GetNumOverlappingPairs()                         const          { return _overlappingPairArray.size(); }
	void SetOverlapFilterCallback(OverlapFilterCallback* callback) { _overlapFilterCallback = callback; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	HashedOverlappingPairCache();
	virtual ~HashedOverlappingPairCache();

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<int> _hashTable;
	gm::AlignedObjectArray<int> _next;
	OverlappingPairCallback*    _ghostPairCallback;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	BroadPhasePair* InternalAddPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1);
	void GrowTables();

	BroadPhasePair* InternalFindPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, int hash);
	bool HasDeferredRemoval() override { return false; }
	void SetInternalGhostPairCallback(OverlappingPairCallback* ghostPairCallback) override { _ghostPairCallback = ghostPairCallback; }
	void SortOverlappingPairs(IDispatcher* dispatcher);
	INLINE bool equalsPair(const BroadPhasePair& pair, int proxyID1, int proxyID2) { return pair.Proxy0->GetUniqueID() == proxyID1 && pair.Proxy1->GetUniqueID() == proxyID2; }
	INLINE unsigned int GetHash(unsigned int proxyID1, unsigned int proxyID2)
	{
		unsigned int key = proxyID1 | (proxyID2 << 16);
		key += ~(key << 15);
		key ^= (key >> 10);
		key += (key << 3);
		key ^= (key >> 6);
		key += ~(key << 11);
		key ^= (key >> 16);
		return key;
	}
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	BroadPhasePairArray    _overlappingPairArray;
	OverlapFilterCallback* _overlapFilterCallback;
};

/****************************************************************************
*				  		SortedOverlappingPairCache
*************************************************************************//**
*  @class    SortedOverlappingPairCache
*  @brief    SortedOverlappingPairCache maintains the objects with overlapping AABB
*            Typically managed by the Broadphase, Axis3Sweep or btSimpleBroadphase
*****************************************************************************/
class SortedOverlappingPairCache : public IOverlappingPairCache
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void    ProcessAllOverlappingPairs (OverlapCallback* callback, IDispatcher* dispatcher) override;
	void*           RemoveOverlappingPair      (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, IDispatcher* dispatcher);
	void            CleanOverlappingPair       (BroadPhasePair& pair   , IDispatcher* dispatcher);
	
	BroadPhasePair* AddOverlappingPair         (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1);
	BroadPhasePair* FindPair                   (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1);
	void            CleanProxyFromPairs        (BroadPhaseProxy* proxy, IDispatcher* dispatcher);
	void RemoveOverlappingPairsContainingProxy (BroadPhaseProxy* proxy, IDispatcher* dispatcher);
	virtual void SortOverlappingPairs(IDispatcher* dispatcher);

	bool NeedsBroadPhaseCollision(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) const;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<BroadPhasePair>      & GetOverlappingPairArray()       { return _overlappingPairArray; }
	const gm::AlignedObjectArray<BroadPhasePair>& GetOverlappingPairArray() const { return _overlappingPairArray; }
	BroadPhasePair*       GetOverlappingPairArrayPtr()       { return &_overlappingPairArray[0]; }
	const BroadPhasePair* GetOverlappingPairArrayPtr() const { return &_overlappingPairArray[0]; }
	int                   GetOverlappingPairsCount  () const { return _overlappingPairArray.size(); }
	OverlapFilterCallback* GetOverlapFilterCallback()        { return _overlapFilterCallback; }
	virtual bool HasDeferredRemoval()  override              { return _hasDefferedRemoval; }
	virtual void SetInternalGhostPairCallback(OverlappingPairCallback* ghostPairCallback) override { _ghostPairCallback = ghostPairCallback; }
	void SetOverlapFilterCallback(OverlapFilterCallback* callBack) { _overlapFilterCallback = callBack; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	SortedOverlappingPairCache();
	virtual ~SortedOverlappingPairCache();

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<BroadPhasePair> _overlappingPairArray; //avoid brute-force finding all the time
	bool                     _blockedForChanges;  //during the dispatch, check that user doesn't destroy/create proxy
	bool                     _hasDefferedRemoval; ///by default, do the removal during the pair traversal
	OverlapFilterCallback  * _overlapFilterCallback;
	OverlappingPairCallback* _ghostPairCallback;
};

class NullPairCache : public IOverlappingPairCache
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	BroadPhasePair* AddOverlappingPair(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) override { return NULL; }
	void* RemoveOverlappingPair      (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1, IDispatcher* dispatcher) override{ return NULL; }
	void CleanOverlappingPair(BroadPhasePair& pair, IDispatcher* dispatcher) override {};
	void CleanProxyFromPairs(BroadPhaseProxy* proxy, IDispatcher* dispatcher) override {};

	bool BeedBroadPhaseCollision(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) const { return true; }

	void ProcessAllOverlappingPairs(OverlapCallback* callback, IDispatcher* dispatcher) override {};
	
	BroadPhasePair* FindPair       (BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1) override { return 0; }
	
	void RemoveOverlappingPairsContainingProxy(BroadPhaseProxy* proxy0, IDispatcher* dispatcher) override {};
	void SortOverlappingPairs(IDispatcher* dispatcher) override { (void)dispatcher; }
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	BroadPhasePair*       GetOverlappingPairArrayPtr() override { return &_overlappingPairArray[0]; }
	const BroadPhasePair* GetOverlappingPairArrayPtr() const    { return &_overlappingPairArray[0]; }
	BroadPhasePairArray&  GetOverlappingPairArray   ()          { return _overlappingPairArray; }
	OverlapFilterCallback* GetOverlapFilterCallback ()          { return 0; }

	bool HasDeferredRemoval()         override { return true; }
	int GetNumOverlappingPairs() const override { return 0; }
	
	void SetOverlapFilterCallback    (OverlapFilterCallback  * callback) override {};
	void SetInternalGhostPairCallback(OverlappingPairCallback* callback) override{};
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/

private:
	BroadPhasePairArray _overlappingPairArray;
};
#endif