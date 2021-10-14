//////////////////////////////////////////////////////////////////////////////////
//            @file　　  BroadPhaseInterface.hpp
//            @brief    Broad phase collision detection object interface  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  ----------
//            @date  :  2021_09_17
//                      2021_10_09 (second checked)
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
#ifndef BROAD_PHASE_INTERFACE_HPP
#define BROAD_PHASE_INTERFACE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "BroadPhaseProxy.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct DispatcherInfo;
class  IDispatcher;
class  IOverlappingPairCache;

struct BroadPhaseAabbCallBack
{
	virtual ~BroadPhaseAabbCallBack() {};
	virtual bool Process(const BroadPhaseProxy* proxy) = 0;
};

struct BroadPhaseRayCallBack : public BroadPhaseAabbCallBack
{
	gm::Vector3 RayDirectionInverse;
	unsigned int Signs[3];
	float LambdaMax;

	virtual ~BroadPhaseRayCallBack() {};
protected:
	BroadPhaseRayCallBack() {};
};
/****************************************************************************
*				  			IBroadPhase
*************************************************************************//**
*  @class     IBroadPhase
*  @brief     The btBroadphaseInterface class provides an interface to detect aabb-overlapping object pairs.
*             Some implementations for this broadphase interface include btAxisSweep3, bt32BitAxisSweep3 and btDbvtBroadphase.
*             The actual overlapping pair management, storage, adding and removing of pairs is dealt by the btOverlappingPairCache class.
*****************************************************************************/
class IBroadPhase
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual BroadPhaseProxy* CreateProxy(
		const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, int shapeType,
		void* userPtr, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher) = 0;
	virtual void DestroyProxy(BroadPhaseProxy* proxy, IDispatcher* dispatcher) = 0;
	virtual void RayTest (const gm::Vector3& rayFrom, const gm::Vector3& rayTo, BroadPhaseRayCallBack rayCallback, const gm::Vector3& aabbMin = gm::Vector3(0, 0, 0), const gm::Vector3& aabbMax = gm::Vector3(0, 0, 0)) = 0;
	virtual void AABBTest(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, BroadPhaseAabbCallBack aabbCallback) = 0;
	virtual void CalculateOverlappingPairs(IDispatcher* dispatcher) = 0;
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual void SetAABB(BroadPhaseProxy* proxy, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher) = 0;
	virtual void GetAABB(BroadPhaseProxy* proxy, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax) const = 0;
	virtual IOverlappingPairCache* GetOverlappingPairCache() = 0;
	virtual const IOverlappingPairCache* GetOverlappingPairCache() const = 0;
	virtual void GetBroadPhaseAABB(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax) const = 0;
	virtual void ResetPool(IDispatcher* dispatcher) { (void)dispatcher; }
	virtual void PrintStats() = 0;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	virtual ~IBroadPhase() {};

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif