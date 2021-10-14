//////////////////////////////////////////////////////////////////////////////////
//            @file　　  DBVTBroadPhase.hpp
//            @brief    Broad phase collision detection object  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partially edit by Toide
//            @date  :  2021_09_18
//                      2021_10_09 (second checked)
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

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
#ifndef DBVT_BROAD_PHASE_HPP
#define DBVT_BROAD_PHASE_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "BroadPhaseInterface.hpp"
#include "OverlappingPair.hpp"
#include "DBVT.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define DBVT_BP_PROFILE (0)
#define DBVT_BP_PREVENTFALSEUPDATE (0)
#define DBVT_BP_ACCURATESLEEPING   (0)
#define DBVT_BP_ENABLE_BENCHMARK   (0)

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
extern float g_DbvtMargin;

struct DbvtProxy : BroadPhaseProxy
{
	DbvtNode* Leaf;
	DbvtProxy* Links[2];
	int Stage;
	DbvtProxy(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, void* userPtr, int collisionFilterGroup, int collisionFilterMask) : BroadPhaseProxy(aabbMin, aabbMax, userPtr, collisionFilterGroup, collisionFilterMask)
	{
		Links[0] = 0;
		Links[1] = 0;
	}
};

using DbvtProxyArray = gm::AlignedObjectArray<DbvtProxy*>;
struct DbvtBroadPhase : IBroadPhase
{
	enum
	{
		DynamicSet = 0,
		FixedSet   = 1,
		CountOfStage = 2
	};
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Collide(IDispatcher* dispatcher);
	void Optimize();

	BroadPhaseProxy* CreateProxy (const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, int shapeType, void* userPtr, int collisionFilterGroup, int collisionFilterMask, IDispatcher* dispatcher);
	virtual void     DestroyProxy(BroadPhaseProxy* absProxy, IDispatcher* dispatcher);
	virtual void     SetAABB     (BroadPhaseProxy* absProxy, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* dispatcher);
	virtual void     RayTest     (const gm::Vector3& rayFrom, const gm::Vector3& rayTo, BroadPhaseRayCallBack& rayCallback, const gm::Vector3& aabbMin = gm::Vector3(0, 0, 0), const gm::Vector3& aabbMax = gm::Vector3(0, 0, 0));
	virtual void     AABBTest    (const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, BroadPhaseAabbCallBack& aabbCallback);
	virtual void     GetAABB     (BroadPhaseProxy* absProxy, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const;
	virtual void     CalculateOverlappingPairs(IDispatcher* dispatcher);
	virtual IOverlappingPairCache* GetOverlappingPairCache();
	virtual const IOverlappingPairCache* GetOverlappingPairCache() const;
	virtual void GetBroadPhaseAABB(gm::Vector3& aabbMin, gm::Vector3& aabbMax) const;
	virtual void PrintStats();

	virtual void ResetPool(IDispatcher* dispatcher);
	void         PerformDeferredRemoval(IDispatcher* dispatcher);
	void         SetVelocityPrediction(float prediction) { Prediction = prediction; }
	float        GetVelocityPrediction() const           { return Prediction; }

	///this setAabbForceUpdate is similar to setAabb but always forces the aabb update.
	///it is not part of the btBroadphaseInterface but specific to btDbvtBroadphase.
	///it bypasses certain optimizations that prevent aabb updates (when the aabb shrinks), see
	///http://code.google.com/p/bullet/issues/detail?id=223
	void SetAABBForceUpdate(BroadPhaseProxy* absproxy, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, IDispatcher* /*dispatcher*/);

	static void Benchmark(IBroadPhase*);
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	Dbvt Sets[2];                            // Dbvt sets
	DbvtProxy* StageRoots[CountOfStage + 1];   // Stages list
	IOverlappingPairCache* PairCache;         // Pair cache
	float    Prediction;                     // Velocity prediction
	int      StageCurrent;                     // Current stage
	int      FupDates;                       // % of fixed updates per frame 
	int      DupDates;                       // % of dynamic updates per frame
	int      CupDates;                       // % of cleanup updates per frame
	int      NewPairs;                       // Number of pairs created
	int      FixedLeft;                      // Fixed optimization left
	unsigned UpdatesCall;                    // Number of update call
	unsigned UpdatesDone;                    // Number of updates done
	float    UpdatesRatio;                   // Updates_done / updates call
	int      ParseID;                        // Parse ID
	int      CleanUpID;                      // Clean up index
	int      GenID;                          // Gen ID
	bool     ReleasePairCache;               // Release pair cache on delete
	bool     DeferedCollide;                 // Defer dynamic/static collision to collide
	bool     NeedCleanUP;                    // Need to run cleanup
	gm::AlignedObjectArray<gm::AlignedObjectArray<const DbvtNode*>> RayTestStacks;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DbvtBroadPhase(IOverlappingPairCache* pairCache = 0);
	~DbvtBroadPhase();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif