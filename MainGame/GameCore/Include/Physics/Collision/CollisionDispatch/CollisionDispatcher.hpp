//////////////////////////////////////////////////////////////////////////////////
//            @file　　  CollisionDispatcher.hpp
//            @brief    CollisionDispatcher  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partially edit by toide
//            @date  :  2021_09_29
//                      2021_10_10(second checked)
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
#ifndef COLLISION_DISPATCHER_HPP
#define COLLISION_DISPATCHER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/Dispatcher.hpp"
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/PersistentManifold.hpp"
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/BroadPhaseProxy.hpp"
#include "GameCore/Include/Physics/Collision/CollisionDispatch/ManifoldResult.hpp"
#include "GameCore/Include/Physics/Collision/CollisionDispatch/CollisionCreateFunction.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class IOverlappingPairCache;
class CollisionDispatcher;
typedef void (*NearCallback)(BroadPhasePair& collisionPair, CollisionDispatcher& dispatcher, const DispatcherInfo& dispatchInfo);

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			CollisionConfiguration
*************************************************************************//**
*  @class     CollisionConfiguration
*  @brief     Collision Configuration
*****************************************************************************/
class CollisionConfiguration
{
public:
	virtual ~CollisionConfiguration() {};
	virtual gm::PoolAllocator* GetPersistentManifoldPool() = 0;
	virtual gm::PoolAllocator* GetCollisionAlgorithmPool() = 0;
	virtual CollisionAlgorithmCreateFunction* GetCollisionAlgorithmCreateFunction(int proxyType0, int proxyType1) = 0;
	virtual CollisionAlgorithmCreateFunction* GetClosestPointsAlgorithmCreateFunction(int proxyType0, int proxyType1) = 0;
};

/****************************************************************************
*				  			CollisionDispatcher
*************************************************************************//**
*  @class     CollisionDispatcher
*  @brief     CollisionDispatcher supports algorithms that handle ConvexConvex and ConvexConcave collision pairs.
              Time of Impact, Closest Points and Penetration Depth.
*****************************************************************************/
class CollisionDispatcher : public IDispatcher
{
public:
	enum DispatcherFlags
	{
		Static_Static_Reported = 1,
		Use_Relative_Contact_Breaking_Threashold = 2,
		Disable_ContactPool_Dynamic_Allocation = 4
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	ICollisionAlgorithm* FindAlgorithm(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, PersistentManifold* sharedManifold, DispatcherQueryType queryType) override;
	bool  NeedsCollision(const CollisionObject* body0, const CollisionObject* body1) override;
	bool  NeedsResponse (const CollisionObject* body0, const CollisionObject* body1) override;
	void  DispatchAllCollisionPairs(IOverlappingPairCache* pairCache, const DispatcherInfo& dispatcherInfo, IDispatcher* dispatcher) override;
	void* AllocateCollisionAlgorithm(int size) override;
	void  FreeCollisionAlgorithm(void* ptr) override;

	void  ReleaseManifold(PersistentManifold* manifold) override;
	void  ClearManifold(PersistentManifold* manifold)   override;

	static void DefaultNearCallback(BroadPhasePair& collisionPair, CollisionDispatcher& dispatcher, const DispatcherInfo& dispatchInfo);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	int                           GetDispatcherFlags        () const { return _dispatcherFlags; }
	int                           GetManifoldsCount         () const { return (int)_manifolds.size(); }
	CollisionConfiguration*       GetCollisionConfiguration ()       { return _collisionConfiguration; }
	const CollisionConfiguration* GetCollisionConfiguration () const { return _collisionConfiguration; }
	NearCallback                  GetNearCallback           () const { return _nearCallback; }
	PersistentManifold**          GetInternalManifoldPointer()       { return _manifolds.size() ? &_manifolds[0] : 0; }
	PersistentManifold*           GetManifoldByIndexInternal(int index)
	{
		assert(index >= 0);
		assert(index < _manifolds.size());
		return _manifolds[index];
	}
	const PersistentManifold*     GetManifoldByIndexInternal(int index) const
	{
		assert(index >= 0);
		assert(index < _manifolds.size());
		return _manifolds[index];
	}
	PersistentManifold*      GetNewManifold(const CollisionObject* b0, const CollisionObject* b1) override;
	gm::PoolAllocator*       GetInternalManifoldPool()       override { return _persistentManifoldPoolAllocator; }
	const gm::PoolAllocator* GetInternalManifoldPool() const override { return _persistentManifoldPoolAllocator; }
	
	void SetCollisionConfiguration(CollisionConfiguration* config) { _collisionConfiguration; }
	void SetDispatcherFlags (int flags) { _dispatcherFlags = flags; }
	void SetNearCallback(NearCallback nearCallback) { _nearCallback = nearCallback; }
	void RegisterCollisionCreateFunction    (int proxyType0, int proxyType1, CollisionAlgorithmCreateFunction* createFunction);
	void RegisterClosestPointsCreateFunction(int proxyType0, int proxyType1, CollisionAlgorithmCreateFunction* createFunction);
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	CollisionDispatcher(CollisionConfiguration* collisionConfiguration);
	virtual ~CollisionDispatcher();

protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	int _dispatcherFlags;
	gm::AlignedObjectArray<PersistentManifold*> _manifolds;
	NearCallback _nearCallback;
	gm::PoolAllocator* _collisionAlgorithmPoolAllocator;
	gm::PoolAllocator* _persistentManifoldPoolAllocator;
	CollisionAlgorithmCreateFunction* _doubleDispatchContactPoints[(int)BroadPhaseNativeTypes::CountOfBroadPhaseCollisionTypes][(int)BroadPhaseNativeTypes::CountOfBroadPhaseCollisionTypes];
	CollisionAlgorithmCreateFunction* _doubleDispatchClosestPoints[(int)BroadPhaseNativeTypes::CountOfBroadPhaseCollisionTypes][(int)BroadPhaseNativeTypes::CountOfBroadPhaseCollisionTypes];
	CollisionConfiguration*           _collisionConfiguration;
};

/****************************************************************************
*				  			CollisionDispatcherMt
*************************************************************************//**
*  @class     CollisionDispatcherMt
*  @brief     For multibody collision dispatcher
*****************************************************************************/
class CollisionDispatcherMt : public CollisionDispatcher
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	PersistentManifold* GetNewManifold(const CollisionObject* body0, const CollisionObject* body1) override;
	void ReleaseManifold(PersistentManifold* manifold) override;
	void DispatchAllCollisionPairs(IOverlappingPairCache* pairCache, const DispatcherInfo& infomation, IDispatcher* dispatcher) override;


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	CollisionDispatcherMt(CollisionConfiguration* config, int grainSize = 40);

protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<gm::AlignedObjectArray<PersistentManifold*>> _batchManifoldsPtr;
	bool _batchUpdating;
	int  _grainSize;
};
#endif