//////////////////////////////////////////////////////////////////////////////////
//            @file　　  CollisionWorld.hpp
//            @brief    Implement Total collision Algorithms
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_12 (first checked)
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2013 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

/**
 * @mainpage Bullet Documentation
 *
 * @section intro_sec Introduction
 * Bullet is a Collision Detection and Rigid Body Dynamics Library. The Library is Open Source and free for commercial use, under the ZLib license ( http://opensource.org/licenses/zlib-license.php ).
 *
 * The main documentation is Bullet_User_Manual.pdf, included in the source code distribution.
 * There is the Physics Forum for feedback and general Collision Detection and Physics discussions.
 * Please visit http://www.bulletphysics.org
 *
 * @section install_sec Installation
 *
 * @subsection step1 Step 1: Download
 * You can download the Bullet Physics Library from the github repository: https://github.com/bulletphysics/bullet3/releases
 *
 * @subsection step2 Step 2: Building
 * Bullet has multiple build systems, including premake, cmake and autotools. Premake and cmake support all platforms.
 * Premake is included in the Bullet/build folder for Windows, Mac OSX and Linux.
 * Under Windows you can click on Bullet/build/vs2010.bat to create Microsoft Visual Studio projects.
 * On Mac OSX and Linux you can open a terminal and generate Makefile, codeblocks or Xcode4 projects:
 * cd Bullet/build
 * ./premake4_osx gmake or ./premake4_linux gmake or ./premake4_linux64 gmake or (for Mac) ./premake4_osx xcode4
 * cd Bullet/build/gmake
 * make
 *
 * An alternative to premake is cmake. You can download cmake from http://www.cmake.org
 * cmake can autogenerate projectfiles for Microsoft Visual Studio, Apple Xcode, KDevelop and Unix Makefiles.
 * The easiest is to run the CMake cmake-gui graphical user interface and choose the options and generate projectfiles.
 * You can also use cmake in the command-line. Here are some examples for various platforms:
 * cmake . -G "Visual Studio 9 2008"
 * cmake . -G Xcode
 * cmake . -G "Unix Makefiles"
 * Although cmake is recommended, you can also use autotools for UNIX: ./autogen.sh ./configure to create a Makefile and then run make.
 *
 * @subsection step3 Step 3: Testing demos
 * Try to run and experiment with BasicDemo executable as a starting point.
 * Bullet can be used in several ways, as Full Rigid Body simulation, as Collision Detector Library or Low Level / Snippets like the GJK Closest Point calculation.
 * The Dependencies can be seen in this documentation under Directories
 *
 * @subsection step4 Step 4: Integrating in your application, full Rigid Body and Soft Body simulation
 * Check out BasicDemo how to create a btDynamicsWorld, btRigidBody and btCollisionShape, Stepping the simulation and synchronizing your graphics object transform.
 * Check out SoftDemo how to use soft body dynamics, using btSoftRigidDynamicsWorld.
 * @subsection step5 Step 5 : Integrate the Collision Detection Library (without Dynamics and other Extras)
 * Bullet Collision Detection can also be used without the Dynamics/Extras.
 * Check out btCollisionWorld and btCollisionObject, and the CollisionInterfaceDemo.
 * @subsection step6 Step 6 : Use Snippets like the GJK Closest Point calculation.
 * Bullet has been designed in a modular way keeping dependencies to a minimum. The ConvexHullDistance demo demonstrates direct use of btGjkPairDetector.
 *
 * @section copyright Copyright
 * For up-to-data information and copyright and contributors list check out the Bullet_User_Manual.pdf
 *
 */

#pragma once
#ifndef COLLISION_WORLD_HPP
#define COLLISION_WORLD_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "CollisionObject.hpp"
#include "CollisionDispatcher.hpp"
#include "../BroadPhaseCollision/OverlappingPair.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class ConvexShape;

//////////////////////////////////////////////////////////////////////////////////
//                               Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class ICollisionWorld
{
public:
	/****************************************************************************
	**                Public Struct
	*****************************************************************************/
	struct LocalShapeInfomation
	{
		int ShapePart;
		int TriangleIndex;
	};
	struct LocalRayResult
	{
		const CollisionObject* CollisionObjectPtr;
		LocalShapeInfomation*  LocalShapeInfo;
		gm::Vector3            HitNormalLocal;
		float                  HitFraction;
		LocalRayResult(const CollisionObject* collisionObject, LocalShapeInfomation* localShapeInfo, const gm::Vector3& hitNormalLocal, float hitFraction)
		{
			CollisionObjectPtr = collisionObject;
			LocalShapeInfo   = localShapeInfo;
			HitNormalLocal   = hitNormalLocal;
			HitFraction      = hitFraction;
		}
	};
	struct RayResultCallback
	{
		float                  ClosestHitFraction;
		const CollisionObject* CollisionObjectPtr;
		int                    CollisionFilterGroup;
		int                    CollisionFilterMask;
		unsigned int           Frags;

		RayResultCallback()
		{
			ClosestHitFraction   = 1.0f;
			CollisionObjectPtr   = NULL;
			CollisionFilterGroup = BroadPhaseProxy::Default;
			CollisionFilterMask  = BroadPhaseProxy::AllFilter;
		}
		virtual ~RayResultCallback() {};
		
		bool HasHit() const { return CollisionObjectPtr != 0; }
		
		virtual bool NeedsCollision(BroadPhaseProxy* proxy) const
		{
			bool collides = (proxy->CollisionFilterGroup & CollisionFilterMask) != 0;
			collides      = collides && (CollisionFilterGroup & proxy->CollisionFilterMask);
			return collides;
		}

		virtual float AddSingleResult(LocalRayResult& rayResult, bool normalInWorldSpace) = 0;
	};
	struct ClosestRayResultCallback : public RayResultCallback
	{
		gm::Vector3 RayFromWorld;
		gm::Vector3 RayToWorld;
		gm::Vector3 HitNormalWorld;
		gm::Vector3 HitPointWorld;

		ClosestRayResultCallback(const gm::Vector3& rayFromWorld, const gm::Vector3& rayToWorld)
		{
			RayFromWorld = rayFromWorld;
			RayToWorld   = rayToWorld;
		}

		float AddSingleResult(LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			assert(rayResult.HitFraction <= ClosestHitFraction);
			ClosestHitFraction = rayResult.HitFraction;
			CollisionObjectPtr = rayResult.CollisionObjectPtr;
			if (normalInWorldSpace)
			{
				HitNormalWorld = rayResult.HitNormalLocal;
			}
			else
			{
				HitNormalWorld = CollisionObjectPtr->GetWorldTransform().GetBasis() * rayResult.HitNormalLocal;
			}
			HitPointWorld = gm::Lerp(RayFromWorld, RayToWorld, rayResult.HitFraction);
			return rayResult.HitFraction;
		}
	};
	struct AllHitsRayResultCallback : public RayResultCallback
	{
		gm::AlignedObjectArray<const CollisionObject* > CollisionObjects;
		gm::Vector3 RayFromWorld;
		gm::Vector3 RayToWorld;
		gm::AlignedObjectArray<gm::Vector3> HitNormalWorld;
		gm::AlignedObjectArray<gm::Vector3> HitPointWorld;
		gm::AlignedObjectArray<float>       HitFractions;

		AllHitsRayResultCallback(const gm::Vector3& rayFromWorld, const gm::Vector3& rayToWorld)
		{
			RayFromWorld = rayFromWorld;
			RayToWorld = rayToWorld;
		}

		float AddSingleResult(LocalRayResult& rayResult, bool normalInWorldSpace) override
		{
			CollisionObjectPtr = rayResult.CollisionObjectPtr;
			CollisionObjects.push_back(rayResult.CollisionObjectPtr);
			gm::Vector3 hitNormalWorld;
			if (normalInWorldSpace)
			{
				hitNormalWorld = rayResult.HitNormalLocal;
			}
			else
			{
				hitNormalWorld = CollisionObjectPtr->GetWorldTransform().GetBasis() * rayResult.HitNormalLocal;
			}
			HitNormalWorld.push_back(hitNormalWorld);

			gm::Vector3 hitPointWorld;
			hitPointWorld = gm::Lerp(RayFromWorld, RayToWorld, rayResult.HitFraction);
			HitPointWorld.push_back(hitPointWorld);
			HitFractions.push_back(rayResult.HitFraction);
			return ClosestHitFraction;
		}
	};
	struct LocalConvexResult
	{
		const CollisionObject* HitCollisionObject;
		LocalShapeInfomation*  LocalShapeInfo;
		gm::Vector3            HitNormalLocal;
		gm::Vector3            HitPointLocal;
		float                  HitFraction;

		LocalConvexResult(const CollisionObject* hitCollisionObject, LocalShapeInfomation* localShapeInfo, const gm::Vector3& hitNormalLocal, const gm::Vector3& hitPointLocal, float hitFraction)
		{
			HitCollisionObject = hitCollisionObject;
			LocalShapeInfo     = localShapeInfo;
			HitNormalLocal     = hitNormalLocal;
			HitPointLocal      = hitPointLocal;
			HitFraction        = hitFraction;
		}
	};
	struct ConvexResultCallback
	{
		float ClosestHitFraction;
		int CollisionFilterGroup;
		int CollisionFilterMask;

		ConvexResultCallback()
		{
			ClosestHitFraction = 1.0f;
			CollisionFilterGroup = BroadPhaseProxy::Default;
			CollisionFilterMask  = BroadPhaseProxy::AllFilter;
		}
		virtual ~ConvexResultCallback() {};

		bool HasHit() const { return ClosestHitFraction < 1.0f; }
		virtual bool NeedsCollision(BroadPhaseProxy* proxy) const
		{
			bool collides = (proxy->CollisionFilterGroup & CollisionFilterMask) != 0;
			collides      = collides && (CollisionFilterGroup & proxy->CollisionFilterMask);
			return collides;
		}

		virtual float AddSingleResult(LocalConvexResult& convexResult, bool normalInWorld) = 0;
	};
	struct ClosestConvexResultCallback : public ConvexResultCallback
	{
		gm::Vector3 ConvexFromWorld;
		gm::Vector3 ConvexToWorld;
		gm::Vector3 HitNormalWorld;
		gm::Vector3 HitPointWorld;
		const CollisionObject* HitCollisionObject;

		ClosestConvexResultCallback(const gm::Vector3& convexFromWorld, const gm::Vector3& convexToWorld)
		{
			ConvexFromWorld    = convexFromWorld;
			ConvexToWorld      = convexToWorld;
			HitCollisionObject = NULL;
		}

		virtual float AddSingleResult(LocalConvexResult& convexResult, bool normalInWorldSpace)
		{
			assert(convexResult.HitFraction <= ClosestHitFraction);

			ClosestHitFraction = convexResult.HitFraction;
			HitCollisionObject = convexResult.HitCollisionObject;
			if (normalInWorldSpace)
			{
				HitNormalWorld = convexResult.HitNormalLocal;
			}
			else
			{
				HitNormalWorld = HitCollisionObject->GetWorldTransform().GetBasis() * convexResult.HitNormalLocal;
			}
			HitPointWorld = convexResult.HitPointLocal;
			return convexResult.HitFraction;
		}
	};
	struct ContactResultCallback
	{
		int CollisionFilterGroup;
		int CollisionFilterMask;
		float ClosestDistanceThreshold;

		ContactResultCallback()
		{
			CollisionFilterGroup     = BroadPhaseProxy::Default;
			CollisionFilterMask      = BroadPhaseProxy::AllFilter;
			ClosestDistanceThreshold = 0;
		}

		~ContactResultCallback() {};

		virtual bool NeedsCollision(BroadPhaseProxy* proxy) const
		{
			bool collides = (proxy->CollisionFilterGroup & CollisionFilterMask) != 0;
			collides      = collides && (CollisionFilterGroup & proxy->CollisionFilterMask);
			return collides;
		}

		virtual float AddSingleResult(ManifoldPoint& cp, const CollisionObjectWrapper* colObj0Wrap, int partID0, int index0, const CollisionObjectWrapper* colObj1Wrap, int partID1, int index1) = 0;

	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void         UpdateSingleAABB(CollisionObject* collisionObject);
	virtual void UpdateAABBs();
	virtual void ComputeOverlappingPairs();

	virtual void DebugDrawWorld();
	virtual void DebugDrawObject(const gm::AffineTransform& worldTransform, const ICollisionShape* shape, const gm::Vector3& color);

	virtual void RayTest        (const gm::Vector3& rayFromWorld, const gm::Vector3& rayToWorld, RayResultCallback& resultCallback);
	void         ConvexSweepTest(const ConvexShape* castShape, const gm::AffineTransform& from, const gm::AffineTransform& to, ConvexResultCallback& resultCallback, float allowedCCDPenetration = 0.0f) const;
	void         ContactTest    (CollisionObject* collisionObject, ContactResultCallback& resultCallback);
	void         ContactPairTest(CollisionObject* collisionObjectA, CollisionObject* collisionObjectB, ContactResultCallback& resultCallback);
	
	static void RayTestSingle            (const gm::AffineTransform& rayFromTransform, const gm::AffineTransform& rayToTransform, CollisionObject* collisionObject, const ICollisionShape* collisionShape, const gm::AffineTransform& collisionObjectWorldTransform, RayResultCallback& resultCallback);
	static void RayTestSingleInternal    (const gm::AffineTransform& rayFromTransform, const gm::AffineTransform& rayToTransform, const CollisionObjectWrapper* collisionObjectWrap, RayResultCallback& resultCallback);
	static void ObjectQuerySingle        (const ConvexShape* castShape, const gm::AffineTransform& rayFromTransform, const gm::AffineTransform& rayToTransform, CollisionObject* collisionObject, const ICollisionShape* collisionShape, const gm::AffineTransform& collisionObjectWorldTransform, ConvexResultCallback& resultCallback, float allowedPenetration);
	static void ObjectQuerySingleInternal(const ConvexShape* castShape, const gm::AffineTransform& convexFromTransform, const gm::AffineTransform& convexToTransform, const CollisionObjectWrapper* collisionObjectWrapper, ConvexResultCallback& resultCallback, float allowedPenetration);

	virtual void AddCollisionObject(CollisionObject* collisionObject, int collisionFilterGroup = BroadPhaseProxy::Default, int collisionFilterMask = BroadPhaseProxy::AllFilter);
	virtual void RemoveCollisionObject(CollisionObject* collisionObject);
	virtual void RefreshBroadPhaseProxy(CollisionObject* collisionObject);
	
	virtual void PerformDiscreteCollisionDetection();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const IBroadPhase*          GetBroadPhase          () const { return _broadPhasePairCache; }
	IBroadPhase*                GetBroadPhase          ()       { return _broadPhasePairCache; }
	IOverlappingPairCache*      GetPairCache           ()       { return _broadPhasePairCache->GetOverlappingPairCache(); }
	IDispatcher*                GetDispatcher          ()       { return _dispatcher; }
	const IDispatcher*          GetDispatcher          () const { return _dispatcher; }
	virtual IDebugDraw*         GetDebugDrawer         ()       { return _debugDrawer; }
	int                         GetCollisionObjectCount() const { return int(_collisionObjects.size()); }
	CollisionObjectArray&       GetCollisionObjectArray()       { return _collisionObjects; }
	const CollisionObjectArray& GetCollisionObjectArray() const { return _collisionObjects; }
	DispatcherInfo&             GetDispatcherInfo      ()       { return _dispatchInfo; }
	const DispatcherInfo&       GetDispatchInfo        () const { return _dispatchInfo; }
	bool                        GetForceUpdateAllAABBs () const { return _forceUpdateAllAABBs; }

	void         SetBroadPhase         (IBroadPhase* pairCache)  { _broadPhasePairCache = pairCache; }
	virtual void SetDebugDrawer        (IDebugDraw* debugDrawer) { _debugDrawer         = debugDrawer; }
	void         SetForceUpdateAllAABBs(bool forceUpdateAABBs)   { _forceUpdateAllAABBs = forceUpdateAABBs; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ICollisionWorld(IDispatcher* dispatcher, IBroadPhase* broadPhasePairCache, CollisionConfiguration* collisionConfiguration);
	virtual ~ICollisionWorld();
protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<CollisionObject*> _collisionObjects;
	IDispatcher*                             _dispatcher;
	DispatcherInfo                           _dispatchInfo;
	IBroadPhase*                             _broadPhasePairCache;
	IDebugDraw*                              _debugDrawer;
	bool                                     _forceUpdateAllAABBs;


};
#endif