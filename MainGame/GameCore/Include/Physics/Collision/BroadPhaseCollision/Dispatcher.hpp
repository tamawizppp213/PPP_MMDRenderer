//////////////////////////////////////////////////////////////////////////////////
///             @file   Dispatcher.hpp
///             @brief  Dispatcher
///             @author Partially Edit by Toide
///             @date   2021_09_16
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
#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMPoolAllocator.hpp"
#include "GameCore/Include/Physics/Util/IDebugDraw.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class  ICollisionAlgorithm;
class  CollisionObject;
struct CollisionObjectWrapper;
class  RigidBody;
class  IOverlappingPairCache;
struct DispatcherInfo;
class  IDispatcher;
class  PersistentManifold;

//////////////////////////////////////////////////////////////////////////////////
//                      Dispatcher
//////////////////////////////////////////////////////////////////////////////////

enum class DispatcherQueryType
{
	ContactPointAlgorithms = 1,
	ClosestPointAlgorithms = 2
};

struct DispatcherInfo
{
	enum class DispatchFunction
	{
		Discrete   = 1,
		Continuous = 2
	};
public:
	float         TimeStep;
	int           StepCount;
	int           DispatchFunction;
	mutable float TimeOfImpact;
	IDebugDraw*   DebugDraw;
	bool          UseContinuous;
	bool          EnableSatConvex;
	bool          EnableSPU;
	bool          UseEPA;
	float         AllowedCCDPenetration;
	bool          UseConvexConservativeDistanceUtil;
	float         ConvexConservativeDistanceThreshold;
	bool          DeterministicOverlappingPairs;

	// class IDebugDraw*
	DispatcherInfo()
	{
		TimeStep                            = 0.0f;
		StepCount                           = 0;
		DispatchFunction                    = (int)DispatchFunction::Discrete;
		TimeOfImpact                        = 1.0f;
		UseContinuous                       = true;
		EnableSatConvex                     = false;
		EnableSPU                           = true;
		UseEPA                              = true;
		AllowedCCDPenetration               = 0.04f;
		UseConvexConservativeDistanceUtil   = false;
		ConvexConservativeDistanceThreshold = 0.0f;
		DeterministicOverlappingPairs       = false;
	}

};

/****************************************************************************
*				  			Dispatcher
*************************************************************************//**
*  @class     Dispatcher
*  @brief     The Dispatcher interface class can be used in combination with broadphase 
*             to dispatch calculations for overlapping pairs.
*             For example for pairwise collision detection, calculating contact points
*             stored in PersistentManifold or user callbacks (game logic).
*****************************************************************************/
class IDispatcher
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual ICollisionAlgorithm* FindAlgorithm(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, PersistentManifold* sharedManifold,DispatcherQueryType queryType ) = 0;
	virtual bool  NeedsCollision(const CollisionObject* body0, const CollisionObject* body1) = 0;
	virtual bool  NeedsResponse (const CollisionObject* body0, const CollisionObject* body1) = 0;
	virtual void  DispatchAllCollisionPairs(IOverlappingPairCache* pairCache, const DispatcherInfo& dispatcherInfo, IDispatcher* dispatcher) = 0;
	virtual void* AllocateCollisionAlgorithm(int size) = 0;
	virtual void  FreeCollisionAlgorithm    (void* ptr) = 0;

	virtual void ReleaseManifold(PersistentManifold* manifold) = 0;
	virtual void ClearManifold(PersistentManifold* manifold) = 0;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual PersistentManifold*  GetNewManifold(const CollisionObject* b0, const CollisionObject* b1) = 0;
	virtual PersistentManifold**  GetInternalManifoldPointer()    = 0;
	virtual        gm::PoolAllocator* GetInternalManifoldPool()       = 0;
	virtual  const gm::PoolAllocator* GetInternalManifoldPool() const = 0;
	virtual int GetManifoldsCount() const = 0;


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	virtual ~IDispatcher() {};
	
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif