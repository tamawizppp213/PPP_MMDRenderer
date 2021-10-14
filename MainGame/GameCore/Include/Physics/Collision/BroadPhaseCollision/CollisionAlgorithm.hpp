//////////////////////////////////////////////////////////////////////////////////
///             @file   CollisionAlgorithm.hpp
///             @brief  CollisionAlgorith,
///             @author Partially edit by Toide.
///             @date   2021_09_17
///                     2021_10_19 second checked
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
#ifndef COLLISION_ALGORITHM_HPP
#define COLLISION_ALGORITHM_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"
#include "Dispatcher.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class  CollisionObject;
class  PersistentManifold;
class  ManifoldResult;
struct CollisionObjectWrapper;
struct BroadPhaseProxy;

using ManifoldArray = gm::AlignedObjectArray<PersistentManifold*>;
//////////////////////////////////////////////////////////////////////////////////
//                       ICollisionAlgorithm
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  		CollisionAlgorithmConstructionInfo
*************************************************************************//**
*  @class    CollisionAlgorithmConstructionInfo
*  @brief    CollisionAlgorithmConstructionInfo
*****************************************************************************/
struct CollisionAlgorithmConstructionInfo
{
	IDispatcher* Dispatcher1;
	PersistentManifold* Manifold;

	CollisionAlgorithmConstructionInfo() : Dispatcher1(0), Manifold(0) {};
	CollisionAlgorithmConstructionInfo(IDispatcher* dispatcher, int temp) : Dispatcher1(dispatcher) { (void)temp; Manifold = NULL; }
};

/****************************************************************************
*				  			 ICollisionAlgorithm
*************************************************************************//**
*  @class     Dispatcher
*  @brief    CollisionAlgorithm is an collision interface that is compatible 
*            with the Broadphase and btDispatcher. It is persistent over frames
*****************************************************************************/
class ICollisionAlgorithm
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) = 0;
	virtual float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) = 0;
	virtual void GetAllContactManifolds(ManifoldArray& manifoldArray) = 0;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ICollisionAlgorithm() {};
	ICollisionAlgorithm(const CollisionAlgorithmConstructionInfo& constructionInfo) { _dispatcher = constructionInfo.Dispatcher1; };
	virtual ~ICollisionAlgorithm() {};
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	IDispatcher* _dispatcher = nullptr;
};
class ICollisionAlgorithm;
class IActivatingCollisionAlgorithm : public ICollisionAlgorithm
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	IActivatingCollisionAlgorithm(const CollisionAlgorithmConstructionInfo& ci) : ICollisionAlgorithm(ci) {};
	IActivatingCollisionAlgorithm(const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) : ICollisionAlgorithm(ci) {};
	~IActivatingCollisionAlgorithm() {};
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/

};
#endif
