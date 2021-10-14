//////////////////////////////////////////////////////////////////////////////////
//            @file　　  CollisionDispatcher.hpp
//            @brief    CollisionDispatcher  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partially edit by toide
//            @date  :  2021_09_29
//                      2021_10_11 second checked
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
#include "GameCore/Include/Physics/Collision/CollisionDispatch/CollisionDispatcher.hpp"
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/CollisionAlgorithm.hpp"
#include "GameCore/Include/Physics/Collision/CollisionDispatch/CollisionObject.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/CollisionShape.hpp"
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/OverlappingPair.hpp"
#include "GameCore/Include/Physics/Util/BulletThread.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region CollisionPairCallback
class CollisionPairCallback : public OverlapCallback
{
	const DispatcherInfo& _dispatchInfo;
	CollisionDispatcher* _dispatcher;

public:
	CollisionPairCallback(const DispatcherInfo& dispatcherInfo, CollisionDispatcher* dispatcher) :_dispatchInfo(dispatcherInfo), _dispatcher(dispatcher){}
	virtual ~CollisionPairCallback() {};
	bool ProcessOverlap(BroadPhasePair& pair) override
	{
		(*_dispatcher->GetNearCallback())(pair, *_dispatcher, _dispatchInfo);
		return false;
	}
};
#pragma endregion CollisionPairCallback
CollisionDispatcher::CollisionDispatcher(CollisionConfiguration* collisionConfiguration)
{
	SetNearCallback(DefaultNearCallback);
	_collisionAlgorithmPoolAllocator = collisionConfiguration->GetCollisionAlgorithmPool();
	_persistentManifoldPoolAllocator = collisionConfiguration->GetPersistentManifoldPool();
	for (int i = 0; i < (int)BroadPhaseNativeTypes::CountOfBroadPhaseCollisionTypes; ++i)
	{
		for (int j = 0; j < (int)BroadPhaseNativeTypes::CountOfBroadPhaseCollisionTypes; ++j)
		{
			_doubleDispatchContactPoints[i][j] = _collisionConfiguration->GetCollisionAlgorithmCreateFunction(i, j);
			assert(_doubleDispatchContactPoints[i][j]);
			_doubleDispatchClosestPoints[i][j] = _collisionConfiguration->GetClosestPointsAlgorithmCreateFunction(i, j);

		}
	}
}
CollisionDispatcher::~CollisionDispatcher()
{

}
/****************************************************************************
*						FindAlgorithm
*************************************************************************//**
*  @fn        ICollisionAlgorithm* CollisionDispatcher::FindAlgorithm(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, PersistentManifold* sharedManifold, DispatcherQueryType queryType)
*  @brief     Find ICollisionAlgorithm*
*  @param[in] CollisionObjectWrapper*  body0Wrap
*  @param[in] CollisionObjectWrapper* body1Wrap
*  @param[in] PersistentManifold* sharedManifold
*  @param[in,out] DispatcherQueryType queryType
*  @return 　　ICollisionAlgorithm*
*****************************************************************************/
ICollisionAlgorithm* CollisionDispatcher::FindAlgorithm(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, PersistentManifold* sharedManifold, DispatcherQueryType queryType)
{
	CollisionAlgorithmConstructionInfo constructionInfo;
	constructionInfo.Dispatcher1 = this;
	constructionInfo.Manifold = sharedManifold;
	ICollisionAlgorithm* algo = 0;
	if (queryType == DispatcherQueryType::ContactPointAlgorithms)
	{
		algo = _doubleDispatchContactPoints[body0Wrap->GetCollisionShape()->GetShapeType()][body1Wrap->GetCollisionShape()->GetShapeType()]->CreateCollsiionAlgorithm(constructionInfo, body0Wrap, body1Wrap);
	}
	else
	{
		algo = _doubleDispatchClosestPoints[body0Wrap->GetCollisionShape()->GetShapeType()][body1Wrap->GetCollisionShape()->GetShapeType()]->CreateCollsiionAlgorithm(constructionInfo, body0Wrap, body1Wrap);
	}
	return algo;
}
/****************************************************************************
*						NeedsCollision
*************************************************************************//**
*  @fn        bool CollisionDispatcher::NeedsCollision(CollisionObject* body0, CollisionObject* body1)
*  @brief     This function finds out if collision detection is required.
*  @param[in,out] CollisionObject*  body0
*  @param[in,out] CollisionObject* body1
*  @return 　　bool
*****************************************************************************/
bool CollisionDispatcher::NeedsCollision(const CollisionObject* body0, const CollisionObject* body1)
{
	assert(body0); assert(body1);
	bool needsCollision = true;

#ifdef _DEBUG
	if (!(_dispatcherFlags & CollisionDispatcher::Static_Static_Reported))
	{
		//broadphase filtering already deals with this
		if (body0->IsStaticOrKinematicObject() && body1->IsStaticOrKinematicObject())
		{
			_dispatcherFlags |= CollisionDispatcher::Static_Static_Reported;
			printf("warning btCollisionDispatcher::needsCollision: static-static collision!\n");
		}
	}
#endif  //BT_DEBUG

	if     ((!body0->IsActive()) && (!body1->IsActive()))                           { needsCollision = false; }
	else if((!body0->CheckCollideWith(body1)) || (!body1->CheckCollideWith(body0))) { needsCollision = false; }

	return needsCollision;
}
/****************************************************************************
*						NeedsResponse
*************************************************************************//**
*  @fn        bool CollisionDispatcher::NeedsResponse(CollisionObject* body0, CollisionObject* body1)
*  @brief     This function finds out if collision response is required.
*  @param[in,out] CollisionObject*  body0
*  @param[in,out] CollisionObject* body1
*  @return 　　bool
*****************************************************************************/
bool CollisionDispatcher::NeedsResponse(const CollisionObject* body0, const CollisionObject* body1)
{
	bool hasResponse = (body0->HasContactResponse() && body1->HasContactResponse());
	hasResponse = hasResponse && ((!body0->IsStaticOrKinematicObject()) || (!body1->IsStaticOrKinematicObject()));
	return hasResponse;
}
/****************************************************************************
*						DispatchAllCollisionPairs
*************************************************************************//**
*  @fn        void CollisionDispatcher::DispatchAllCollisionPairs(OverlappingPairCache* pairCache, const DispatcherInfo& dispatcherInfo, IDispatcher* dispatcher)
*  @brief     All collision pairs is dispatched.
*  @param[in,out] OverlappingPairCache* pairCache
*  @param[in] DispatcherInfo& dispatcherInfo
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void CollisionDispatcher::DispatchAllCollisionPairs(IOverlappingPairCache* pairCache, const DispatcherInfo& dispatcherInfo, IDispatcher* dispatcher)
{
	CollisionPairCallback collisionCallback(dispatcherInfo, this);
	pairCache->ProcessAllOverlappingPairs(&collisionCallback, dispatcher, dispatcherInfo);
}
/****************************************************************************
*						AllocateCollisionAlgorithm
*************************************************************************//**
*  @fn        void* CollisionDispatcher::AllocateCollisionAlgorithm(int size)
*  @brief     Allocate collision algorithm memory
*  @param[in] int size
*  @return 　　void*
*****************************************************************************/
void* CollisionDispatcher::AllocateCollisionAlgorithm(int size)
{
	void* memory = _collisionAlgorithmPoolAllocator->Allocate(size);
	if (memory == NULL) { return gm::AlignedAllocInternal(static_cast<size_t>(size), 16); }
	return memory;
}
/****************************************************************************
*						FreeCollisionAlgorithm
*************************************************************************//**
*  @fn        void CollisionDispatcher::FreeCollisionAlgorithm(void* ptr)
*  @brief     Free collision algorithm memory
*  @param[in] void* ptr
*  @return 　　void
*****************************************************************************/
void CollisionDispatcher::FreeCollisionAlgorithm(void* ptr)
{
	if (_collisionAlgorithmPoolAllocator->ValidPtr(ptr))
	{
		_collisionAlgorithmPoolAllocator->FreeMemory(ptr);
	}
	else
	{
		gm::AlignedFreeInternal(ptr);
	}
}
/****************************************************************************
*						ReleaseManifold
*************************************************************************//**
*  @fn        void CollisionDispatcher::ReleaseManifold(PersistentManifold* manifold)
*  @brief     Release manifold pool allocator
*  @param[in, out] PersistentManifold* manifold
*  @return 　　void
*****************************************************************************/
void CollisionDispatcher::ReleaseManifold(PersistentManifold* manifold)
{
	ClearManifold(manifold);

	int findIndex = manifold->Index1a;
	assert(findIndex < _manifolds.size());

	_manifolds.swap(findIndex, _manifolds.size() - 1);
	_manifolds[findIndex]->Index1a = findIndex;
	_manifolds.pop_back();

	manifold->~PersistentManifold();
	if (_persistentManifoldPoolAllocator->ValidPtr(manifold))
	{
		_persistentManifoldPoolAllocator->FreeMemory(manifold);
	}
	else
	{
		AlignedFreeInternal(manifold);
	}
}
/****************************************************************************
*						ClearManifold
*************************************************************************//**
*  @fn        void CollisionDispatcher::ClearManifold(PersistentManifold* manifold)
*  @brief     --
*  @param[in, out] PersistentManifold* manifold
*  @return 　　void
*****************************************************************************/
void CollisionDispatcher::ClearManifold(PersistentManifold* manifold)
{
	manifold->ClearManifold();
}
/****************************************************************************
*						DefaultNearCallback
*************************************************************************//**
*  @fn        void CollisionDispatcher::DefaultNearCallback(BroadPhasePair& collisionPair, CollisionDispatcher& dispatcher, const DispatcherInfo& dispatchInfo)
*  @brief     --
*  @param[in,out] BroadPhasePair& collisionPair
*  @param[in,out] CollisionDispatcher& dispatcher
*  @param[in] DispatcherInfo& dispatcherInfo
*  @return 　　void
*****************************************************************************/
void CollisionDispatcher::DefaultNearCallback(BroadPhasePair& collisionPair, CollisionDispatcher& dispatcher, const DispatcherInfo& dispatchInfo)
{
	CollisionObject* collisionObject0 = (CollisionObject*)collisionPair.Proxy0->ClientObject;
	CollisionObject* collisionObject1 = (CollisionObject*)collisionPair.Proxy1->ClientObject;

	if (dispatcher.NeedsCollision(collisionObject0, collisionObject1))
	{
		CollisionObjectWrapper objectWrapper0(0, collisionObject0->GetCollisionShape(), collisionObject0, collisionObject0->GetWorldTransform(), -1, -1);
		CollisionObjectWrapper objectWrapper1(0, collisionObject1->GetCollisionShape(), collisionObject1, collisionObject1->GetWorldTransform(), -1, -1);

		if (!collisionPair.Algorithm)
		{
			collisionPair.Algorithm = dispatcher.FindAlgorithm(&objectWrapper0, &objectWrapper1, NULL, DispatcherQueryType::ContactPointAlgorithms);

		}

		if (collisionPair.Algorithm)
		{
			ManifoldResult contactPointResult(&objectWrapper0, &objectWrapper1);
			if (dispatchInfo.DispatchFunction == (int)DispatcherInfo::DispatchFunction::Discrete)
			{
				collisionPair.Algorithm->ProcessCollision(&objectWrapper0, &objectWrapper1, dispatchInfo, &contactPointResult);
			}
			else
			{
				float timeOfImpact = collisionPair.Algorithm->CalculateTimeOfImpact(collisionObject0, collisionObject1, dispatchInfo, &contactPointResult);
				if (dispatchInfo.TimeOfImpact > timeOfImpact)
				{
					dispatchInfo.TimeOfImpact = timeOfImpact;
				}
			}
		}
	}
}
/****************************************************************************
*						GetNewManifold
*************************************************************************//**
*  @fn        PersistentManifold* CollisionDispatcher::GetNewManifold(const CollisionObject* b0, const CollisionObject* b1)
*  @brief     Get ne manifold
*  @param[in] CollisionObject* b0
*  @param[in] CollisionObject* b1
*  @return 　　PersistentManifold*
*****************************************************************************/
PersistentManifold* CollisionDispatcher::GetNewManifold(const CollisionObject* b0, const CollisionObject* b1)
{
	float contactBreakingThreshold = (_dispatcherFlags & CollisionDispatcher::Use_Relative_Contact_Breaking_Threashold) ? Min(b0->GetCollisionShape()->GetContactBreakingThreshold(g_ContactBreakingThreshold), b1->GetCollisionShape()->GetContactBreakingThreshold(g_ContactBreakingThreshold)) 
		: static_cast<gm::Scalar>(g_ContactBreakingThreshold);
	float contactProcessingThreshold = Min(b0->GetContactProcessingThreshold(), b1->GetContactProcessingThreshold());

	void* memory = _persistentManifoldPoolAllocator->Allocate(sizeof(PersistentManifold));
	if (memory == NULL)
	{
		memory = AlignedAllocInternal(sizeof(PersistentManifold), 16);
	}
	else
	{
		assert(0); return 0;
	}

	/*-------------------------------------------------------------------
	-           New Manifold 
	---------------------------------------------------------------------*/
	PersistentManifold* manifold = new(memory)PersistentManifold(b0, b1, 0, static_cast<float>(g_ContactBreakingThreshold), contactProcessingThreshold);
	manifold->Index1a = _manifolds.size();
	_manifolds.push_back(manifold);
	return manifold;
}
/****************************************************************************
*						RegisterCollisionCreateFunction
*************************************************************************//**
*  @fn        void CollisionDispatcher::RegisterCollisionCreateFunction(int proxyType0, int proxyType1, CollisionAlgorithmCreateFunction* createFunction)
*  @brief     RegisterCollisionCreateFunction
*  @param[in] int proxyType0
*  @param[in] int proxyType1
*  @param[in,out] CollisionAlgorithmCreateFunction* createFunction
*  @return 　　void
*****************************************************************************/
void CollisionDispatcher::RegisterCollisionCreateFunction(int proxyType0, int proxyType1, CollisionAlgorithmCreateFunction* createFunction)
{
	_doubleDispatchContactPoints[proxyType0][proxyType1] = createFunction;
}
/****************************************************************************
*						RegisterClosestPointsCreateFunction
*************************************************************************//**
*  @fn        void CollisionDispatcher::RegisterClosestPointsCreateFunction(int proxyType0, int proxyType1, CollisionAlgorithmCreateFunction* createFunction)
*  @brief     Register Closeset point create
*  @param[in] int proxyType0
*  @param[in] int proxyType1
*  @param[in,out] CollisionAlgorithmCreateFunction* createFunction
*  @return 　　void
*****************************************************************************/
void CollisionDispatcher::RegisterClosestPointsCreateFunction(int proxyType0, int proxyType1, CollisionAlgorithmCreateFunction* createFunction)
{
	_doubleDispatchClosestPoints[proxyType0][proxyType1] = createFunction;
}
CollisionDispatcherMt::CollisionDispatcherMt(CollisionConfiguration* config, int grainSize) : CollisionDispatcher(config)
{
	_batchManifoldsPtr.resize(bt::GetTaskScheduler()->GetThreadsCount());
	_batchUpdating = false;
	_grainSize     = grainSize;
}
/****************************************************************************
*						GetNewManifold
*************************************************************************//**
*  @fn        PersistentManifold* CollisionDispatcher::GetNewManifold(const CollisionObject* b0, const CollisionObject* b1)
*  @brief     Get ne manifold
*  @param[in] CollisionObject* b0
*  @param[in] CollisionObject* b1
*  @return 　　PersistentManifold*
*****************************************************************************/
PersistentManifold* CollisionDispatcherMt::GetNewManifold(const CollisionObject* b0, const CollisionObject* b1)
{
	float contactBreakingThreshold   = (_dispatcherFlags & CollisionDispatcher::DispatcherFlags::Use_Relative_Contact_Breaking_Threashold) ? (float)Min((float)b0->GetCollisionShape()->GetContactBreakingThreshold(g_ContactBreakingThreshold), (float)b1->GetCollisionShape()->GetContactBreakingThreshold(g_ContactBreakingThreshold)) : (float)g_ContactBreakingThreshold;
	float contactProcessingThreshold = Min(b0->GetContactProcessingThreshold(), b1->GetContactProcessingThreshold());

	void* memory = _persistentManifoldPoolAllocator->Allocate(sizeof(PersistentManifold));
	if (memory == NULL)
	{
		if ((_dispatcherFlags & CollisionDispatcher::DispatcherFlags::Disable_ContactPool_Dynamic_Allocation) == 0)
		{
			memory = gm::AlignedAllocInternal(sizeof(PersistentManifold), 16);
		}
		else
		{
			assert(0); return 0;
		}
	}
}
/****************************************************************************
*						ReleaseManifold
*************************************************************************//**
*  @fn        void CollisionDispatcher::ReleaseManifold(PersistentManifold* manifold)
*  @brief     Release manifold pool allocator
*  @param[in, out] PersistentManifold* manifold
*  @return 　　void
*****************************************************************************/
void CollisionDispatcherMt::ReleaseManifold(PersistentManifold* manifold)
{
	ClearManifold(manifold);
	if (!_batchUpdating)
	{
		int findIndex = manifold->Index1a;
		assert(findIndex < _manifolds.size());
		_manifolds.swap(findIndex, _manifolds.size() - 1);
		_manifolds[findIndex]->Index1a = findIndex;
		_manifolds.pop_back();
	}

	manifold->~PersistentManifold();
	if (_persistentManifoldPoolAllocator->ValidPtr(manifold))
	{
		_persistentManifoldPoolAllocator->FreeMemory(manifold);
	}
	else
	{
		gm::AlignedFreeInternal(manifold);
	}
}

struct CollisionDispatcherUpdater : public bt::IParallelForBody
{
	BroadPhasePair*       PairArray;
	NearCallback          Callback;
	CollisionDispatcher*  Dispatcher;
	const DispatcherInfo* Infomation;

	CollisionDispatcherUpdater()
	{
		PairArray  = NULL;
		Callback   = NULL;
		Dispatcher = NULL;
		Infomation = NULL;
	}

	void ForLoop(int begin, int end) const
	{
		for (int i = begin; i < end; ++i)
		{
			BroadPhasePair* pair = &PairArray[i];
			Callback(*pair, *Dispatcher, *Infomation);
		}
	}
};
/****************************************************************************
*						DispatchAllCollisionPairs
*************************************************************************//**
*  @fn        void CollisionDispatcher::DispatchAllCollisionPairs(OverlappingPairCache* pairCache, const DispatcherInfo& dispatcherInfo, IDispatcher* dispatcher)
*  @brief     All collision pairs is dispatched.
*  @param[in,out] OverlappingPairCache* pairCache
*  @param[in] DispatcherInfo& dispatcherInfo
*  @param[in,out] IDispatcher* dispatcher
*  @return 　　void
*****************************************************************************/
void CollisionDispatcherMt::DispatchAllCollisionPairs(IOverlappingPairCache* pairCache, const DispatcherInfo& infomation, IDispatcher* dispatcher)
{
	const int pairCount = pairCache->GetNumOverlappingPairs();
	if (pairCount == 0) { return; }

	CollisionDispatcherUpdater updater;
	updater.Callback    = GetNearCallback();
	updater.PairArray   = pairCache->GetOverlappingPairArrayPtr();
	updater.Dispatcher  = this;
	updater.Infomation  = &infomation;

	_batchUpdating = true;
	bt::ParallelFor(0, pairCount, _grainSize, updater);
	_batchUpdating = false;

	for (int i = 0; i < _batchManifoldsPtr.size(); ++i)
	{
		gm::AlignedObjectArray<PersistentManifold*>& batchManifoldsPtr = _batchManifoldsPtr[i];
		for (int j = 0; j < batchManifoldsPtr.size(); ++j)
		{
			_manifolds.push_back(batchManifoldsPtr[j]);
		}
		batchManifoldsPtr.resizeNoInitialize(0);
	}

	for (int i = 0; i < _manifolds.size(); ++i)
	{
		_manifolds[i]->Index1a = i;
	}
}