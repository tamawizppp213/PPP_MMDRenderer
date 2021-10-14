//////////////////////////////////////////////////////////////////////////////////
//            @file　　  AxisSweepInternal.hpp
//            @brief    Axis sweep internal
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partially edit by Toide
//            @date  :  2021_10_09 (first checked) 
//////////////////////////////////////////////////////////////////////////////////
//
// btAxisSweep3.h
//
// Copyright (c) 2006 Simon Hobbs
//
// This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

#pragma once
#ifndef AXIS_SWEEP_INTERNAL_HPP
#define AXIS_SWEEP_INTERNAL_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "BroadPhaseInterface.hpp"
#include "OverlappingPair.hpp"
#include "DBVTBroadPhase.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define USE_OVERLAP_TEST_ON_REMOVES 1

//////////////////////////////////////////////////////////////////////////////////
//                           Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			AxisSweepInternal
*************************************************************************//**
*  @class     AxisSweepInternal
*  @brief     Axis speep 
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
class AxisSweepInternal : public IBroadPhase
{
public:
	/****************************************************************************
	**                Public Class
	*****************************************************************************/
	class Edge
	{
	public:
		BP_FP_INT_TYPE Position;
		BP_FP_INT_TYPE Handle;

		BP_FP_INT_TYPE IsMax() const { return static_cast<BP_FP_INT_TYPE>Position & 1); }
	};

	class Handle : public BroadPhaseProxy
	{
	public:
		DECLARE_ALIGNED_ALLOCATOR;
		BP_FP_INT_TYPE MinEdges[3];
		BP_FP_INT_TYPE MaxEdges[3];
		BroadPhaseProxy* DbvtProxy;
		INLINE void SetNextFree(BP_FP_INT_TYPE next) { MinEdges[0] = next; }
		INLINE BP_FP_INT_TYPE GetNextFree() const { return MinEdges[0]; }
	};

public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void CalculateOverlappingPairs (IDispatcher* dispatcher) override;
	void ProcessAllOverlappingPairs(OverlapCallback* callback);

	BP_FP_INT_TYPE   AddHandle   (const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, void* pOwner, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher);
	void             RemoveHandle(BP_FP_INT_TYPE handle, IDispatcher* dispatcher);
	void             UpdateHandle(BP_FP_INT_TYPE handle, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher);

	void             ResetPool   (IDispatcher* dispatcher) override;

	// BroadPhase Interface
	BroadPhaseProxy* CreateProxy (const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, int shapeType, void* userPtr, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher) override;
	void             DestroyProxy(BroadPhaseProxy* proxy, IDispatcher* dispatcher) override;

	void RayTest (const gm::Vector3& rayFrom, const gm::Vector3& rayTo, BroadPhaseRayCallBack& rayCallback, const gm::Vector3& aabbMin = gm::Vector3(0, 0, 0), const gm::Vector3& aabbMax = gm::Vector3(0, 0, 0)) override;
	void AABBTest(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, BroadPhaseAabbCallBack& callback);

	///unQuantize should be conservative: aabbMin/aabbMax should be larger then 'getAabb' result
	void Quantize(BP_FP_INT_TYPE* out, const gm::Vector3& point, int isMax) const;
	void UnQuantize(BroadPhaseProxy* proxy, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const;

	bool TestAABBOverlap(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	BP_FP_INT_TYPE GetHandleCount()                       const { return _handleCount; }
	INLINE Handle* GetHandle     (BP_FP_INT_TYPE index)   const { return _handles + index; }
	void           GetAABB       (BroadPhaseProxy* proxy, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override;
	IOverlappingPairCache*       GetOverlappingPairCache            () { return _pairCache; }
	const IOverlappingPairCache* GetOverlappingPairCache            () const { return _pairCache; }
	const OverlappingPairCallback* GetOverlappingPairUserCallback   () const { return _userPairCallback;}
	void GetBroadPhaseAABB(gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
	{
		aabbMin = _worldAABBMin;
		aabbMax = _worldAABBMax;
	}
	
	void SetAABB(BroadPhaseProxy* proxy, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher) override;
	void SetOverlappingPairUserCallback(OverlappingPairCallback* pairCallback) override { _userPairCallback = pairCallback; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	AxisSweepInternal(const gm::Vector3& worldAABBMin, const gm::Vector3& worldAABBMax, BP_FP_INT_TYPE handleMask, BP_FP_INT_TYPE handleSentinel, BP_FP_INT_TYPE userMaxHandles = 16384, IOverlappingPairCache* pairCache = NULL, bool disableRaycastAccelerator = false);
	virtual ~AxisSweepInternal();
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	BP_FP_INT_TYPE AllocateHandle();
	void           FreeHandle();
	
	bool           TestOverlap2D(const Handle* handleA, const Handle* handleB, int axis0, int axis1);

	void SortMinDown(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps);
	void SortMinUp  (int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps);
	void SortMaxDown(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps);
	void SortMaxUp  (int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps);


	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	BP_FP_INT_TYPE         _bpHandleMask;
	BP_FP_INT_TYPE         _handleSentinel;
	gm::Vector3            _worldAABBMin;      // overall system bounds
	gm::Vector3            _worldAABBMax;      // overall system bounds
	gm::Vector3            _quantize;          // scaling factor for quantization　(quantize: タイミングを合わせること, Collsionの同期のこと?)
	BP_FP_INT_TYPE         _handleCount;       // number of active handles
	BP_FP_INT_TYPE         _maxHandles;        // max number of handles
	Handle*                _handles;           // handle pool
	BP_FP_INT_TYPE         _firstFreeHandle;   // free handle list
	Edge*                  _edges[3];          // edge arrays for the 3 axes (each array has m_maxHandles * 2 + 2 sentinel entries)
	void*                  _edgesRowPointer[3];
	IOverlappingPairCache* _pairCache;
	OverlappingPairCallback* _userPairCallback; // IOverlappingPairCallback is an additional optional user callback for adding/ removing overlapping pairs, similar interface to IOverlappingPairCache;
	bool                   _ownsPairCache;
	int                    _invalidPair;
	DbvtBroadPhase*        _raycastAccelerator;
	IOverlappingPairCache* _nullPairCache;

};

#pragma region Implement
template <typename BP_FP_INT_TYPE>
AxisSweepInternal<BP_FP_INT_TYPE>::AxisSweepInternal(const gm::Vector3& worldAABBMin, const gm::Vector3& worldAABBMax, BP_FP_INT_TYPE handleMask, BP_FP_INT_TYPE handleSentinel, BP_FP_INT_TYPE userMaxHandles, IOverlappingPairCache* pairCache, bool disableRaycastAccelerator)
{
	_bpHandleMask       = handleMask;
	_handleSentinel     = handleSentinel;
	_pairCache          = pairCache;
	_userPairCallback   = NULL;
	_ownsPairCache      = false;
	_invalidPair        = NULL;
	_raycastAccelerator = NULL;

	BP_FP_INT_TYPE maxHandles = static_cast<BP_FP_INT_TYPE>(userMaxHandles + 1); // need to add one sentinel handle

	if (!_pairCache)
	{
		void* ptr      = gm::AlignedAllocInternal(sizeof(HashedOverlappingPairCache), 16);
		_pairCache     = new (ptr) HashedOverlappingPairCache();
		_ownsPairCache = true;
	}

	if (!disableRaycastAccelerator)
	{
		_nullPairCache = new (gm::AlignedAllocInternal(sizeof(NullPairCache), 16))  NullPairCache();
		_raycastAccelerator = new (gm::AlignedAllocInternal(sizeof(DbvtBroadPhase), 16)) DbvtBroadPhase(_nullPairCache);
		_raycastAccelerator->DeferedCollide = true;
	}

	_worldAABBMin = worldAABBMin;
	_worldAABBMax = worldAABBMax;
	gm::Vector3 aabbSize  = _worldAABBMax - _worldAABBMin;
	BP_FP_INT_TYPE maxInt = _handleSentinel;
	
	_quantize = gm::Vector3(float(maxInt), float(maxInt), float(maxInt)) / aabbSize;

	_handles     = maxHandles;
	_handleCount = NULL;

	_firstFreeHandle = 1;
	{
		for (BP_FP_INT_TYPE i = _firstFreeHandle; i < maxHandles; ++i)
		{
			_handles[i].SetNextFree(static_cast<BP_FP_INT_TYPE>(i + 1));
		}
		_handles[maxHandles - 1].SetNextFree(0);
	}

	{
		for (int i = 0; i < 3; ++i)
		{
			_edgesRowPointer[i] = gm::AlignedAllocInternal(sizeof(Edge) * maxHandles * 2, 16);
			_edges[i] = new (_edgesRowPointer[i])Edge[maxHandles * 2];
		}
	}

	_handles[0].ClientObject = 0;

	for (int axis = 0; axis < 3; ++axis)
	{
		_handles[0].MinEdges[axis] = 0;
		_handles[0].MaxEdges[axis] = 0;
		_edges[axis][0].Position = 0;
		_edges[axis][0].Handle = 0;
		_edges[axis][1].Position = _handleSentinel;
		_edges[axis][1].Handle = 0;
	}
}
template <typename BP_FP_INT_TYPE>
AxisSweepInternal<BP_FP_INT_TYPE>::~AxisSweepInternal()
{
	if (_raycastAccelerator)
	{
		_nullPairCache->~IOverlappingPairCache();
		gm::AlignedFreeInternal(_nullPairCache);
		_raycastAccelerator->~DbvtBroadPhase();
		gm::AlignedFreeInternal(_raycastAccelerator);
	}

	for (int i = 2; i >= 0; i--)
	{
		gm::AlignedFreeInternal(_edgesRowPointer[i]);
	}
	delete[]_handles;

	if (_ownsPairCache)
	{
		_pairCache->~IOverlappingPairCache();
		gm::AlignedFreeInternal(_pairCache);
	}
}
/****************************************************************************
*							CalculateOverlappingPairs
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::CalculateOverlappingPairs(IDispatcher* dispatcher)
*  @brief     CalculateOverlappingPairs
*  @param[in, out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::CalculateOverlappingPairs(IDispatcher* dispatcher)
{
	if (_pairCache->HasDeferredRemoval())
	{
		BroadPhasePairArray& overlappingPairArray = _pairCache->GetOverlappingPairArray();

		//perform a sort, to find duplicates and to sort 'invalid' pairs to the end
		overlappingPairArray.quickSort(BroadPhasePairSortPredicate());

		overlappingPairArray.resize(overlappingPairArray.size() - _invalidPair);
		_invalidPair = 0;

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
				///important to use an AABB test that is consistent with the broadphase
				bool hasOverlap = TestAABBOverlap(pair.Proxy0, pair.Proxy1);

				if (hasOverlap)
				{
					needsRemoval = false;  //callback->processOverlap(pair);
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
				btAssert(!pair.Algorithm);
			}

			if (needsRemoval)
			{
				_pairCache->CleanOverlappingPair(pair, dispatcher);
				pair.Proxy0 = 0;
				pair.Proxy1 = 0;
				_invalidPair++;
			}
		}

		///if you don't like to skip the invalid pairs in the array, execute following code:
#define CLEAN_INVALID_PAIRS 1
#ifdef CLEAN_INVALID_PAIRS

		//perform a sort, to sort 'invalid' pairs to the end
		overlappingPairArray.quickSort(BroadPhasePairSortPredicate());

		overlappingPairArray.resize(overlappingPairArray.size() - _invalidPair);
		_invalidPair = 0;
#endif  //CLEAN_INVALID_PAIRS
	}
}
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::ProcessAllOverlappingPairs(OverlapCallback* callback)
{

}
/****************************************************************************
*							AddHandle
*************************************************************************//**
*  @fn        BP_FP_INT_TYPE AxisSweepInternal<BP_FP_INT_TYPE>::AddHandle(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, void* pOwner, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher)
*  @brief     Add new handle 
*  @param[in] gm::Vector3 aabbMin, 
*  @param[in] gm::Vector3 aabbMax, 
*  @param[in,out] void* pOwner, 
*  @param[in] int collisionFilterGroup, 
*  @param[in] int collisionFilterMask, 
*  @param[in,out]IDispatcher* dispatcher
*  @return 　　BP_FP_INT_TYPE
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
BP_FP_INT_TYPE AxisSweepInternal<BP_FP_INT_TYPE>::AddHandle(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, void* pOwner, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher)
{
	/*-------------------------------------------------------------------
	-           Quantize the bounds
	---------------------------------------------------------------------*/
	BP_FP_INT_TYPE min[3], max[3];
	Quantize(min, aabbMin, 0);
	Quantize(max, aabbMax, 1);

	/*-------------------------------------------------------------------
	-           Allocate a handle
	---------------------------------------------------------------------*/
	BP_FP_INT_TYPE handle         = AllocateHandle();
	Handle*        pHandle        = GetHandle(handle);
	pHandle->UniqueID             = static_cast<int>(handle);
	pHandle->ClientObject         = pOwner;
	pHandle->CollisionFilterGroup = collisionFilterGroup;
	pHandle->CollisionFilterMask  = collisionFilterMask;

	/*-------------------------------------------------------------------
	-           Compute current limit of edge arrays
	---------------------------------------------------------------------*/
	BP_FP_INT_TYPE limit = static_cast<BP_FP_INT_TYPE>(_handleCount * 2);

	/*-------------------------------------------------------------------
	-           Insert new edges just inside the max boundary edge
	---------------------------------------------------------------------*/
	for (BP_FP_INT_TYPE axis = 0; axis < 3; axis++)
	{
		_handles[0].MaxEdges[axis] += 2;
		_edges[axis][limit + 1] = _edges[axis][limit - 1];

		_edges[axis][limit - 1].Position = min[axis];
		_edges[axis][limit - 1].Handle = handle;

		_edges[axis][limit].Position = max[axis];
		_edges[axis][limit].Handle = handle;

		pHandle->MinEdges[axis] = static_cast<BP_FP_INT_TYPE>(limit - 1);
		pHandle->MaxEdges[axis] = limit;
	}

	/*-------------------------------------------------------------------
	-           Sort the new edges to theier correct position
	---------------------------------------------------------------------*/
	SortMinDown(0, pHandle->MinEdges[0], dispatcher, false);
	SortMaxDown(0, pHandle->MaxEdges[0], dispatcher, false);
	SortMinDown(1, pHandle->MinEdges[1], dispatcher, false);
	SortMaxDown(1, pHandle->MaxEdges[1], dispatcher, false);
	SortMinDown(2, pHandle->MinEdges[2], dispatcher, true);
	SortMaxDown(2, pHandle->MaxEdges[2], dispatcher, true);
}
/****************************************************************************
*							RemoveHandle
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::RemoveHandle(BP_FP_INT_TYPE handle, IDispatcher* dispatcher)
*  @brief     Remove handle
*  @param[in] BP_FP_INT_TYPE handle, 
*  @param[in, out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::RemoveHandle(BP_FP_INT_TYPE handle, IDispatcher* dispatcher)
{
	Handle* pHandle = GetHandle(handle);

	if (!_pairCache->HasDeferredRemoval())
	{
		_pairCache->RemoveOverlappingPairsContainingProxy(pHandle, dispatcher);
	}

	// compute current limit of edge arrays
	int limit = static_cast<int>(_handleCount * 2);
	int axis;

	for (axis = 0; axis < 3; axis++)
	{
		_handles[0].MaxEdges[axis] -= 2;
	}

	// remove the edges by sorting them up to the end of the list
	for (axis = 0; axis < 3; axis++)
	{
		Edge* pEdges = _edges[axis];
		BP_FP_INT_TYPE max = pHandle->MaxEdges[axis];
		pEdges[max].Position = _handleSentinel;

		SortMaxUp(axis, max, dispatcher, false);

		BP_FP_INT_TYPE i = pHandle->MinEdges[axis];
		pEdges[i].Position = _handleSentinel;

		SortMinUp(axis, i, dispatcher, false);

		pEdges[limit - 1].Handle = 0;
		pEdges[limit - 1].Position = _handleSentinel;
	}

	// free the handle
	FreeHandle(handle);
}
/****************************************************************************
*							UpdateHandle
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::UpdateHandle(BP_FP_INT_TYPE handle, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher)
*  @brief     Update handle
*  @param[in] BP_FP_INT_TYPE handle, 
*  @param[in] gm::Vector3 aabbMin, 
*  @param[in] gm::Vector3 aabbMax, 
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::UpdateHandle(BP_FP_INT_TYPE handle, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher)
{
	Handle* pHandle = GetHandle(handle);

	/*-------------------------------------------------------------------
	-           Quantize the new bounds
	---------------------------------------------------------------------*/
	BP_FP_INT_TYPE min[3], max[3];
	Quantize(min, aabbMin, 0);
	Quantize(max, aabbMax, 1);

	/*-------------------------------------------------------------------
	-           Update changed edges
	---------------------------------------------------------------------*/
	for (int axis = 0; axis < 3; axis++)
	{
		BP_FP_INT_TYPE eMin = pHandle->MinEdges[axis];
		BP_FP_INT_TYPE eMax = pHandle->MaxEdges[axis];

		int dmin = (int)min[axis] - (int)_edges[axis][emin].Position;
		int dmax = (int)max[axis] - (int)_edges[axis][emax].Position;

		_edges[axis][emin].Position = min[axis];
		_edges[axis][emax].Position = max[axis];

		// expand (only adds overlaps)
		if (dmin < 0)
			SortMinDown(axis, emin, dispatcher, true);

		if (dmax > 0)
			SortMaxUp(axis, emax, dispatcher, true);

		// shrink (only removes overlaps)
		if (dmin > 0)
			SortMinUp(axis, emin, dispatcher, true);

		if (dmax < 0)
			SortMaxDown(axis, emax, dispatcher, true);
	}
}
/****************************************************************************
*							ResetPool
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::ResetPool(IDispatcher* dispatcher)
*  @brief     Reset handle allocator
*  @param[in, out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::ResetPool(IDispatcher* dispatcher)
{
	if (_handleCount == 0)
	{
		_firstFreeHandle = 1;
		{
			for (BP_FP_INT_TYPE i = _firstFreeHandle; i < _maxHandles; i++)
			{
				_handles[i].SetNextFree(static_cast<BP_FP_INT_TYPE>(i + 1));
			}
			_handles[_maxHandles - 1].SetNextFree(0);
		}
	}
}
/****************************************************************************
*							CreateProxy
*************************************************************************//**
*  @fn        BroadPhaseProxy* AxisSweepInternal<BP_FP_INT_TYPE>::CreateProxy(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, int shapeType, void* userPtr, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher)
*  @brief     Create new BroadPhaseProxy
*  @param[in] gm::Vector3 aabbMin,
*  @param[in] gm::Vector3 aabbMax, 
*  @param[in] int shapeType, 
*  @param[in] void* userPtr, 
*  @param[in] int collisionFilterGroup, 
*  @param[in] int collisionFilterMask, 
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　BroadPhaseProxy*
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
BroadPhaseProxy* AxisSweepInternal<BP_FP_INT_TYPE>::CreateProxy(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, int shapeType, void* userPtr, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher)
{
	(void)shapeType;
	BP_FP_INT_TYPE handleID = AddHandle(aabbMin, aabbMax, userPtr, collisionFilterGroup, collisionFilterMask, dispatcher);
	Handle*        handle   = GetHandle(handleID);

	if (_raycastAccelerator)
	{
		BroadPhaseProxy* rayProxy = _raycastAccelerator->CreateProxy(aabbMin, aabbMax, shapeType, userPtr, collisionFilterGroup, collisionFilterMask, dispatcher);
		handle->DbvtProxy = rayProxy;
	}
	return handle;
}
/****************************************************************************
*							DestroyProxy
*************************************************************************//**
*  @fn        BroadPhaseProxy* AxisSweepInternal<BP_FP_INT_TYPE>::CreateProxy(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, int shapeType, void* userPtr, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher)
*  @brief     DestroyProxy
*  @param[in,out] BroadPhaseProxy* proxy, 
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::DestroyProxy(BroadPhaseProxy* proxy, IDispatcher* dispatcher)
{
	AxisSweepInternal::Handle* handle = static_cast<AxisSweepInternal::Handle*>(proxy);
	if (_raycastAccelerator)
	{
		_raycastAccelerator->DestroyProxy(handle->DbvtProxy)
	}
	RemoveHandle(static_cast<BP_FP_INT_TYPE>(handle->UniqueID), dispatcher);
}
/****************************************************************************
*							RayTest
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::SetAABB(BroadPhaseProxy* proxy, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher)
*  @brief     Ray collision test
*  @param[in,out] BroadPhaseProxy* proxy,
*  @param[in]     gm::Vector3 rayFrom
*  @param[in]     gm::Vector3 rayTo
*  @param[in,out] BroadPhaseRayCallBack& rayCallback, 
*  @param[in]     gm::Vector3 aabbMin, 
*  @param[in]     gm::Vector3 aabbMax
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::RayTest(const gm::Vector3& rayFrom, const gm::Vector3& rayTo, BroadPhaseRayCallBack& rayCallback, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax)
{
	if (_raycastAccelerator)
	{
		_raycastAccelerator->RayTest(rayFrom, rayTo, rayCallback, aabbMin, aabbMax);
	}
	else
	{
		BP_FP_INT_TYPE axis = 0;
		for (BP_FP_INT_TYPE i = 1; i < _handleCount * 2 + 1; ++i)
		{
			if (_edges[axis][i].IsMax())
			{
				rayCallback.Process(GetHandle(_edges[axis][i].Handle));
			}
		}
	}
}
/****************************************************************************
*							AABBTest
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::AABBTest(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, BroadPhaseAabbCallBack& callback)
*  @brief     AABB collision test
*  @param[in] gm::Vector3 aabbMin,
*  @param[in] gm::Vector3 aabbMax, 
*  @param[in,out] BroadPhaseAabbCallBack& callback
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::AABBTest(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, BroadPhaseAabbCallBack& callback)
{
	if (_raycastAccelerator)
	{
		_raycastAccelerator->AABBTest(aabbMin, aabbMax, callback);
	}
	else
	{
		BP_FP_INT_TYPE axis = 0;
		for (BP_FP_INT_TYPE i = 1; i < _handleCount * 2 + 1; ++i)
		{
			if (_edges[axis][i].IsMax())
			{
				Handle* handle = GetHandle(_edges[axis][i].Handle);
				if (TestAABBAgainstAABB2(aabbMin, aabbMax, handle->AABBMin, handle->AABBMax))
				{
					callback.Process(handle);
				}
			}
		}
	}
}
/****************************************************************************
*							Quantize
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::Quantize(BP_FP_INT_TYPE* out, const gm::Vector3& point, int isMax)
*  @brief     AABB collision test
*  @param[out] BP_FP_INT_TYPE* out, gm::Vector3& point, 
*  @param[in]  gm::Vector3& point
*  @param[in]  int isMax
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::Quantize(BP_FP_INT_TYPE* out, const gm::Vector3& point, int isMax) const
{
	gm::Vector3 v = (point - _worldAABBMin) * _quantize;
	out[0] = (v[0] <= 0) ? (BP_FP_INT_TYPE)isMax : (v[0] >= _handleSentinel) ? (BP_FP_INT_TYPE)((_handleSentinel & _bpHandleMask) | isMax) : (BP_FP_INT_TYPE)(((BP_FP_INT_TYPE)v[0] & _bpHandleMask) | isMax);
	out[1] = (v[1] <= 0) ? (BP_FP_INT_TYPE)isMax : (v[1] >= _handleSentinel) ? (BP_FP_INT_TYPE)((_handleSentinel & _bpHandleMask) | isMax) : (BP_FP_INT_TYPE)(((BP_FP_INT_TYPE)v[1] & _bpHandleMask) | isMax);
	out[2] = (v[2] <= 0) ? (BP_FP_INT_TYPE)isMax : (v[2] >= _handleSentinel) ? (BP_FP_INT_TYPE)((_handleSentinel & _bpHandleMask) | isMax) : (BP_FP_INT_TYPE)(((BP_FP_INT_TYPE)v[2] & _bpHandleMask) | isMax);
}
/****************************************************************************
*							UnQuantize
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::UnQuantize(BroadPhaseProxy* proxy, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
*  @brief     UnQuantize update aabb infomation
*  @param[in, out] BroadPhaseProxy* proxy
*  @param[out] gm::Vector3 aabbMin,
*  @param[out] gm::Vector3 aabbMax,
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::UnQuantize(BroadPhaseProxy* proxy, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const 
{
	Handle* handle = static_cast<Handle*>(proxy);
	unsigned short vectorInMin[3];
	unsigned short vectorInMax[3];

	vectorInMin[0] = _edges[0][handle->MinEdges[0]].Position;
	vectorInMax[0] = _edges[0][handle->MinEdges[0]].Position + 1;
	vectorInMin[1] = _edges[1][handle->MinEdges[1]].Position;
	vectorInMax[1] = _edges[1][handle->MinEdges[1]].Position + 1;
	vectorInMin[2] = _edges[2][handle->MinEdges[2]].Position;
	vectorInMax[2] = _edges[2][handle->MinEdges[2]].Position + 1;

	aabbMin = Vector3((float)(vectorInMin[0]) / (_quantize.GetX()), (float)(vectorInMin[1]) / (_quantize.GetY()), (float)(vectorInMin[2]) / (_quantize.GetZ()));
	aabbMin = aabbMin + _worldAABBMin;
	aabbMax = Vector3((float)(vectorInMax[0]) / (_quantize.GetX()), (float)(vectorInMax[1]) / (_quantize.GetY()), (float)(vectorInMax[2]) / (_quantize.GetZ()));
	aabbMax = aabbMax + _worldAABBMax;
}
/****************************************************************************
*							TestAABBOverlap
*************************************************************************//**
*  @fn        bool AxisSweepInternal<BP_FP_INT_TYPE>::TestAABBOverlap(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
*  @brief     aabb collision test (overlap function)
*  @param[in, out] BroadPhaseProxy* proxyo
*  @param[in, out] BroadPhaseProxy* proxy1
*  @return 　　bool 
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
bool AxisSweepInternal<BP_FP_INT_TYPE>::TestAABBOverlap(BroadPhaseProxy* proxy0, BroadPhaseProxy* proxy1)
{
	const Handle* pHandleA = static_cast<Handle*>(proxy0);
	const Handle* pHandleB = static_cast<Handle*>(proxy1);

	//optimization 1: check the array index (memory address), instead of the m_pos

	for (int axis = 0; axis < 3; axis++)
	{
		if (pHandleA->MaxEdges[axis] < pHandleB->MinEdges[axis] ||
			pHandleB->MaxEdges[axis] < pHandleA->MinEdges[axis])
		{
			return false;
		}
	}
	return true;
}
#pragma region Property
/****************************************************************************
*							GetAABB
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::SetAABB(BroadPhaseProxy* proxy, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher)
*  @brief     Set aabb to handle
*  @param[in,out] BroadPhaseProxy* proxy,
*  @param[out]   gm::Vector3& aabbMin,
*  @param[out]   gm::Vector3& aabbMax,
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::GetAABB(BroadPhaseProxy* proxy, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
{
	Handle* handle = static_cast<Handle*>(proxy);
	aabbMin = handle->AABBMin;
	aabbMax = handle->AABBMax;
}
/****************************************************************************
*							SetAABB
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::SetAABB(BroadPhaseProxy* proxy, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher)
*  @brief     Set aabb to handle
*  @param[in,out] BroadPhaseProxy* proxy, 
*  @param[in]     gm::Vector3& aabbMin, 
*  @param[in]     gm::Vector3& aabbMax, 
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::SetAABB(BroadPhaseProxy* proxy, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher)
{
	Handle* handle  = static_cast<Handle*>(proxy);
	handle->AABBMin = aabbMin;
	handle->AABBMax = aabbMax;
	UpdateHandle(static_cast<BP_FP_INT_TYPE>(handle->UniqueID), aabbMin, aabbMax, dispatcher);
	if (_raycastAccelerator)
	{
		_raycastAccelerator->SetAABB(handle->DbvtProxy, aabbMin, aabbMax, dispatcher);
	}
}
#pragma endregion Property
#pragma region Protected Function
/****************************************************************************
*							AllocateHandle
*************************************************************************//**
*  @fn        BP_FP_INT_TYPE AxisSweepInternal<BP_FP_INT_TYPE>::AllocateHandle()
*  @brief     Allocate handle
*  @param[in] void
*  @return 　　BP_FP_INT_TYPE
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
BP_FP_INT_TYPE AxisSweepInternal<BP_FP_INT_TYPE>::AllocateHandle()
{
	BP_FP_INT_TYPE handle = _firstFreeHandle;
	_firstFreeHandle = GetHandle(handle)->GetNextFree();
	_handleCount++;
	return handle;
}
/****************************************************************************
*							FreeHandle
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::FreeHandle()
*  @brief     Free handle
*  @param[in] void
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::FreeHandle()
{
	assert(handle > 0 && handle < _maxHandles);
	GetHandle(handle)->SetNextFree(_firstFreeHandle);
	_firstFreeHandle = handle;
	_handleCount--;
}
/****************************************************************************
*							TestOverlap2D
*************************************************************************//**
*  @fn        bool AxisSweepInternal<BP_FP_INT_TYPE>::TestOverlap2D(const Handle* handleA, const Handle* handleB, int axis0, int axis1);
*  @brief     Test overlap2D
*  @param[in] handle* handleA
*  @param[in] handle* handleB
*  @param[in] int axis0
*  @param[in] int axis1
*  @return 　　bool
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
bool AxisSweepInternal<BP_FP_INT_TYPE>::TestOverlap2D(const Handle* handleA, const Handle* handleB, int axis0, int axis1)
{
	if (handleA->MaxEdges[axis0] < handleB->MinEdges[axis0] ||
		handleB->MaxEdges[axis0] < handleA->MinEdges[axis0] ||
		handleA->MaxEdges[axis1] < handleB->MinEdges[axis1] ||
		handleB->MaxEdges[axis1] < handleA->MinEdges[axis1])
	{
		return false;
	}
	return true;
}
/****************************************************************************
*							SortMinDown
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::SortMinDown(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps)
*  @brief     Sorting a min edge downwards can only ever *add* overlaps
*  @param[in] int axis
*  @param[in] BP_FP_INT_TYPE edge
*  @param[in, out] IDispatcher* dispatcher
*  @param[in] bool updateOverlap
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::SortMinDown(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps)
{
	Edge* edge     = _edges[axis] + edge;
	Edge* previous = edge - 1;
	Handle* handleEdge = GetHandle(edge->Handle);

	while (edge->Position < previous->Handle)
	{
		Handle* handlePrevious = GetHandle(previous->Handle);

		if (previous->IsMax())
		{
			/*-------------------------------------------------------------------
			-      If previous edge is a maximum check the bounds and add an overlap if necessary
			---------------------------------------------------------------------*/
			const int axis1 = (1 << axis) & 3;
			const int axis2 = (1 << axis1) & 3;
			if (updateOverlaps && TestOverlap2D(handleEdge, handlePrevious, axis1, axis2))
			{
				_pairCache->AddOverlappingPair(handleEdge, handlePrevious);
				if (_userPairCallback)
				{
					_userPairCallback->AddOverlappingPair(handleEdge, handlePrevious);
				}
			}

			handlePrevious->MaxEdges[axis]++;
		}
		else
		{
			handlePrevious->MinEdges[axis]++;
		}
		handleEdge->MinEdges[axis]--;

		// swap
		Edge swap = *edge;
		*edge     = *previous;
		*previous = swap;

		// decrement
		edge--;
		previous--;
	}
}
/****************************************************************************
*							SortMinUp
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::SortMinDown(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps)
*  @brief     Sorting a min edge downwards can only ever *remove* overlaps
*  @param[in] int axis
*  @param[in] BP_FP_INT_TYPE edge
*  @param[in, out] IDispatcher* dispatcher
*  @param[in] bool updateOverlap
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::SortMinUp(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps)
{
	Edge* edge         = _edges[axis] + edge;
	Edge* next         = edge + 1;
	Handle* handleEdge = GetHandle(edge->Handle);

	while (next->Handle && (edge->Position >= next->Position))
	{
		Handle* handleNext = GetHandle(next->Handle);

		if (next->IsMax())
		{
			Handle* handle0 = GetHandle(edge->Handle);
			Handle* handle1 = GetHandle(next->Handle)
			const int axis1 = (1 << axis) & 3;
			const int axis2 = (1 << axis1) & 3;
			if (updateOverlaps && TestOverlap2D(handle0, handle1, axis1, axis2))
			{
				_pairCache->RemoveOverlappingPair(handle0, handle1, dispatcher);
				if (_userPairCallback)
				{
					_userPairCallback->RemoveOverlappingPair(handle0, handle1, dispatcher);
				}
			}

			handleNext->MaxEdges[axis]--;
		}
		else
		{
			handleNext->MinEdges[axis]--;
		}
		handleEdge->MinEdges[axis]++;

		// swap
		Edge swap = *edge;
		*edge = *next;
		*next= swap;

		// increment
		edge++;
		next++;
	}
}
/****************************************************************************
*							SortMaxDown
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::SortMaxDown(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps)
*  @brief     Sorting a max edge downwards can only ever *remove* overlaps
*  @param[in] int axis
*  @param[in] BP_FP_INT_TYPE edge
*  @param[in, out] IDispatcher* dispatcher
*  @param[in] bool updateOverlap
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::SortMaxDown(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps)
{
	Edge* edge         = _edges[axis] + edge;
	Edge* previous     = edge - 1;
	Handle* handleEdge = GetHandle(edge->Handle);

	while (edge->Position < previous->Handle)
	{
		Handle* handlePrevious = GetHandle(previous->Handle);

		if (!previous->IsMax())
		{
			Handle* handle0 = GetHandle(edge->Handle);
			Handle* handle1 = GetHandle(previous->Handle)
			const int axis1 = (1 << axis) & 3;
			const int axis2 = (1 << axis1) & 3;
			if (updateOverlaps && TestOverlap2D(handle0, handle1, axis1, axis2))
			{
				_pairCache->RemoveOverlappingPair(handle0, handle1, dispatcher);
				if (_userPairCallback)
				{
					_userPairCallback->RemoveOverlappingPair(handle0, handle1, dispatcher);
				}
			}

			handlePrevious->MinEdges[axis]++;
		}
		else
		{
			handlePrevious->MaxEdges[axis]++;
		}
		handleEdge->MaxEdges[axis]--;

		// swap
		Edge swap = *edge;
		*edge = *previous;
		*previous= swap;

		// decrement
		edge--;
		previous--;
	}
}
/****************************************************************************
*							SortMaxUp
*************************************************************************//**
*  @fn        void AxisSweepInternal<BP_FP_INT_TYPE>::SortMaxUp(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps)
*  @brief     Sorting a max edge downwards can only ever *add* overlaps
*  @param[in] int axis
*  @param[in] BP_FP_INT_TYPE edge
*  @param[in, out] IDispatcher* dispatcher
*  @param[in] bool updateOverlap
*  @return 　　void
*****************************************************************************/
template <typename BP_FP_INT_TYPE>
void AxisSweepInternal<BP_FP_INT_TYPE>::SortMaxUp(int axis, BP_FP_INT_TYPE edge, IDispatcher* dispatcher, bool updateOverlaps)
{
	Edge* edge     = _edges[axis] + edge;
	Edge* next     = edge + 1;
	Handle* handleEdge = GetHandle(edge->Handle);

	while (edge->Position < previous->Handle)
	{
		Handle* handleNext = GetHandle(next->Handle);
		const int axis1 = (1 << axis) & 3;
		const int axis2 = (1 << axis1) & 3;
		if (!next->IsMax())
		{
			/*-------------------------------------------------------------------
			-      If previous edge is a maximum check the bounds and add an overlap if necessary
			---------------------------------------------------------------------*/
			if (updateOverlaps && TestOverlap2D(handleEdge, handleNext, axis1, axis2))
			{
				Handle* handle0 = GetHandle(edge->Handle);
				Handle* handle1 = GetHandle(next->Handle);
				_pairCache->AddOverlappingPair(handle0, handle1);
				if (_userPairCallback)
				{
					_userPairCallback->AddOverlappingPair(handle0, handle1);
				}
			}

			handleNext->MinEdges[axis]--;
		}
		else
		{
			handleNect->MaxEdges[axis]--;
		}
		handleEdge->MaxEdges[axis]++;

		// swap
		Edge swap = *edge;
		*edge     = *next;
		*next = swap;

		// decrement
		edge++;
		next++;
	}
}
#pragma endregion Protected Function

#pragma endregion Implement
#endif
