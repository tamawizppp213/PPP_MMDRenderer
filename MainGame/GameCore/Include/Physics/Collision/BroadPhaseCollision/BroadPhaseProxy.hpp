//////////////////////////////////////////////////////////////////////////////////
//            @file　　  BroadPhaseProxy.hpp
//            @brief    Broad phase collision detection object interface  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  ----------
//            @date  :  2021_09_13 Edit. (Toide)
//                      2021_10_09 second checked
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
#ifndef BROAD_PHASE_PROXY_HPP
#define BROAD_PHASE_PROXY_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMTransform.hpp"
#include "GameMath/Include/GMAlighedAllocator.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

enum class BroadPhaseNativeTypes
{
	Box_Shape,
	Triangle_Shape,
	Tetrahedral_Shape,
	Convex_Triangle_Mesh_Shape,
	Convex_Hull_Shape,
	Convex_Point_Cloud_Shape,
	Custom_Polyhedral_Shape,
	Implicit_Convex_Shapes_Start_Here,
	Sphere_Shape,
	Multi_Sphere_Shape,
	Capsule_Shape,
	Cone_Shape,
	Convex_Shape,
	Cylinder_Shape,
	Uniform_Scaling_Shape,
	Minkowski_Sum_Shape,
	Minkowski_Difference_Shape,
	Box2D_Shape,
	Convex2D_Shape,
	Custom_Convex_Shape,
	Concave_Shapes_Start_Here,
	Triangle_Mesh_Shape,
	Scaled_Triangle_Mesh_Shape,
	Fast_Concave_Mesh,
	Terrain_Shape,
	GImpact_Shape,
	MultiMaterial_Triangle_Mesh,
	Empty_Shape,
	Static_Plane_Shape,
	Custom_Concave_Shape,
	SDF_Shape = Custom_Concave_Shape,
	Concave_Shapes_End_Here,
	Compound_Shape,
	Softbody_Shape,
	Hffluid_Shape,
	Hffluid_Buoyant_Convex_Shape,
	Invalid_Shape,
	CountOfBroadPhaseCollisionTypes
};


/****************************************************************************
*				  			BroadPhaseProxy
*************************************************************************//**
*  @class     BroadPhaseProxy
*  @brief     BroadPhaseProxy class (note! : 16 bytes (for 16 alignment))
*****************************************************************************/
ALIGNED16(struct) BroadPhaseProxy
{
public:
	enum CollisionFilterGroups
	{
		Default       = 0x0001,
		Static        = 0x0002,
		Kinematic     = 0x0004,
		Debris        = 0x0008,
		SensorTrigger = 0x0010,
		AllFilter     = -1
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	// Usually the client CollisionObject or RigidbodyClass
	void* ClientObject;
	int CollisionFilterGroup;
	int CollisionFilterMask;
	int UniqueID;

	gm::Vector3 AABBMin;
	gm::Vector3 AABBMax;

	INLINE int GetUniqueID() const { return UniqueID; }
	static INLINE bool IsPolyhedral(int proxyType) { return (proxyType < (int)BroadPhaseNativeTypes::Implicit_Convex_Shapes_Start_Here); }
	static INLINE bool IsConvex    (int proxyType) { return (proxyType < (int)BroadPhaseNativeTypes::Concave_Shapes_Start_Here); }
	static INLINE bool IsNonMoving (int proxyType) { return (IsConcave(proxyType) && !(proxyType == (int)BroadPhaseNativeTypes::GImpact_Shape)); }
	static INLINE bool IsConcave   (int proxyType) { return ((proxyType > (int)BroadPhaseNativeTypes::Concave_Shapes_Start_Here) && (proxyType < (int)BroadPhaseNativeTypes::Concave_Shapes_End_Here));}
	static INLINE bool IsCompound  (int proxyType) { return (proxyType == (int)BroadPhaseNativeTypes::Compound_Shape); }
	static INLINE bool IsSoftBody  (int proxyType) { return (proxyType == (int)BroadPhaseNativeTypes::Softbody_Shape); }
	static INLINE bool IsInfinite  (int proxyType) { return (proxyType == (int)BroadPhaseNativeTypes::Static_Plane_Shape); }
	static INLINE bool IsConvex2D  (int proxyType) { return (proxyType == (int)BroadPhaseNativeTypes::Box2D_Shape) || (proxyType == (int)BroadPhaseNativeTypes::Convex2D_Shape); }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	BroadPhaseProxy() : ClientObject(NULL) {};
	BroadPhaseProxy(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, void* userPtr, int collisionFilterGroup, int collisionFilterMask)
	{
		ClientObject         = userPtr;
		CollisionFilterGroup = collisionFilterGroup;
		CollisionFilterMask  = collisionFilterMask;
		AABBMax              = aabbMax;
		AABBMin              = aabbMin;
	}
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

class  ICollisionAlgorithm;
struct BroadPhaseProxy;
/****************************************************************************
*				  			BroadPhasePair
*************************************************************************//**
*  @class     BroadPhasePair
*  @brief     BroadPhase collision pair class (note! : 16 bytes (for 16 alignment))
*****************************************************************************/
__declspec(align(16)) struct BroadPhasePair
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	BroadPhaseProxy* Proxy0;
	BroadPhaseProxy* Proxy1;

	mutable ICollisionAlgorithm* Algorithm = nullptr;
	union
	{
		void* InternalInfo1;
		int InternalTempValue;
	};
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	BroadPhasePair() : Proxy0(nullptr), Proxy1(nullptr), Algorithm(0), InternalInfo1(nullptr) {}
	BroadPhasePair(BroadPhaseProxy& proxy0, BroadPhaseProxy& proxy1)
	{
		if (proxy0.UniqueID < proxy1.UniqueID)
		{
			Proxy0 = &proxy0;
			Proxy1 = &proxy1;
		}
		else
		{
			Proxy0 = &proxy1;
			Proxy1 = &proxy0;
		}

		InternalInfo1 = nullptr;
	}

	DECLARE_ALIGNED_ALLOCATOR;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

class BroadPhasePairSortPredicate
{
public:
	bool operator()(const BroadPhasePair& a, const BroadPhasePair& b) const
	{
		const int uidA0 = a.Proxy0 ? a.Proxy0->UniqueID : -1;
		const int uidB0 = b.Proxy0 ? b.Proxy0->UniqueID : -1;
		const int uidA1 = a.Proxy1 ? a.Proxy1->UniqueID : -1;
		const int uidB1 = b.Proxy1 ? b.Proxy1->UniqueID : -1;

		return uidA0 > uidB0 ||
			(a.Proxy0 == b.Proxy0 && uidA1 > uidB1) ||
			(a.Proxy0 == b.Proxy0 && a.Proxy1 == b.Proxy1
				&& a.Algorithm > b.Algorithm);
	}
};

INLINE bool operator==(const BroadPhasePair& a, const BroadPhasePair& b)
{
	return (a.Proxy0 == b.Proxy0) && (a.Proxy1 == b.Proxy1);
}
#endif