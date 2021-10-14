//////////////////////////////////////////////////////////////////////////////////
///             @file   CollisionShape.hpp
///             @brief  CollisionShape
///             @author Partically edit bullet library by Toide
///             @date   2021_09_13, 
///                     2021_10_05 (first checked) first checked -> no debug.
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
#ifndef COLLISION_SHAPE_HPP
#define COLLISION_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/BroadPhaseProxy.hpp"
#include "GameMath/Include/GMTransform.hpp"
#include "GameMath/Include/GMAlighedAllocator.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         ICollisionShape
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			ICollisionShape
*************************************************************************//**
*  @class     ICollisionShape
*  @brief     The btCollisionShape class provides an interface for collision shapes 
*             that can be shared among CollisionObjects.
*             that can be shared among CollisionObjects.
*****************************************************************************/
__declspec(align(16)) class ICollisionShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	virtual void  GetAABB(const gm::AffineTransform & transform, gm::Vector3 & aabbMin, gm::Vector3 & aabbMax) const = 0;
	virtual void  GetBoundingSphere(gm::Vector3 & center, float& radius) const;
	virtual float GetMargin()                                                      const = 0;
	virtual float GetAngularMotionDisc()                                           const;
	virtual float GetContactBreakingThreshold(float defaultContactThresholdFactor) const;
	void*         GetUserPointer() const { return _userPointer; }
	int           GetUserIndex  () const { return _userIndex; }
	int           GetUserIndex2 () const { return _userIndex2; }
	int           GetShapeType  () const { return _shapeType; }
	void          CalculateTemporalAABB(const gm::AffineTransform & currentTrans, const gm::Vector3 & linearVelocity, const gm::Vector3 & angularVelocity, float timeStep, gm::Vector3 & temporalAABBMin, gm::Vector3 & temporalAABBMax) const;
	virtual const gm::Vector3& GetLocalScaling() const = 0;
	virtual void               SetLocalScaling(const gm::Vector3& scale) = 0;
	virtual void               CalculateLocalInertia(float mass, gm::Vector3& inertia) const = 0;
	virtual const char* GetName() const = 0;
	virtual gm::Vector3 GetAnisotropicRollingFrictionDirection() const { return gm::Vector3(1, 1, 1); }

	virtual void SetMargin(float margin) = 0;
	void SetUserPointer(void* userPtr) { _userPointer = userPtr; }
	void SetUserIndex (int index) { _userIndex = index; }
	void SetUserIndex2(int index) { _userIndex2 = index; }

	INLINE bool IsPolyhedral() const { return BroadPhaseProxy::IsPolyhedral(GetShapeType()); }
	INLINE bool IsConvex2D()   const { return BroadPhaseProxy::IsConvex2D(GetShapeType()); }
	INLINE bool IsConvex()     const { return BroadPhaseProxy::IsConvex(GetShapeType()); }
	INLINE bool IsNonMoving()  const { return BroadPhaseProxy::IsNonMoving(GetShapeType()); }
	INLINE bool IsConcave()    const { return BroadPhaseProxy::IsConcave(GetShapeType()); }
	INLINE bool IsCompound()   const { return BroadPhaseProxy::IsCompound(GetShapeType()); }
	INLINE bool IsSoftBody()   const { return BroadPhaseProxy::IsSoftBody(GetShapeType()); }
	INLINE bool IsInfinite()   const { return BroadPhaseProxy::IsInfinite(GetShapeType()); }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ICollisionShape() :_shapeType((int)BroadPhaseNativeTypes::Invalid_Shape), _userPointer(nullptr), _userIndex(-1), _userIndex2(-1){}
	virtual ~ICollisionShape() {};
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	int   _shapeType;
	void* _userPointer;
	int   _userIndex;
	int   _userIndex2;
};

#endif