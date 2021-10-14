//////////////////////////////////////////////////////////////////////////////////
///             @file   CollisionShape.cpp
///             @brief  CollisionShape
///             @author Partially edit by Toide
///             @date   2021_10_05 (first checked) first checked -> no debug.
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


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionShapes/CollisionShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
extern "C"
{
	void BulletCollisionProbe();
	void BulletCollisionProbe() {};
}
/****************************************************************************
*						GetBoundingSphere
*************************************************************************//**
*  @fn        void ICollisionShape::GetBoundingSphere(gm::Vector3& center, float& radius) const
*  @brief     Get bounding sphere
*  @param[out] Vector3& center
*  @param[out] float& radius
*  @return 　　void
*****************************************************************************/
void ICollisionShape::GetBoundingSphere(gm::Vector3& center, float& radius) const
{
	AffineTransform transform;
	Vector3 aabbMin, aabbMax;

	GetAABB(transform, aabbMin, aabbMax);
	radius = Norm(aabbMax - aabbMin) * 0.5f;
	center =     (aabbMin + aabbMax) * 0.5f;
}
/****************************************************************************
*						GetContactBreakingThreshold
*************************************************************************//**
*  @fn        float ICollisionShape::GetContactBreakingThreshold(float defaultContactThreshold) const
*  @brief     Get contact breaking threshold
*  @param[in] float defaultContactThreshold
*  @return 　　float
*****************************************************************************/
float ICollisionShape::GetContactBreakingThreshold(float defaultContactThreshold) const
{
	return GetAngularMotionDisc() * defaultContactThreshold;
}
/****************************************************************************
*						GetAngularMotionDisc
*************************************************************************//**
*  @fn        float ICollisionShape::GetAngularMotionDisc() const
*  @brief     Get motion disc
*  @param[in] void
*  @return 　　float
*****************************************************************************/
float ICollisionShape::GetAngularMotionDisc() const
{
	Vector3 center;
	float   disc;
	GetBoundingSphere(center, disc);
	disc   += Norm(center);
	return disc;
}
/****************************************************************************
*						CalculateTemporalAABB
*************************************************************************//**
*  @fn        void ICollisionShape::CalculateTemporalAABB(const gm::AffineTransform& currentTrans, const gm::Vector3& linearVelocity, const gm::Vector3& angularVelocity, float timeStep, gm::Vector3& temporalAABBMin, gm::Vector3& temporalAABBMax) const
*  @brief     CalculateTemporalAABB
*  @param[in] AffineTransform& currentTrans
*  @param[in] Vector3& linearVelocity
*  @param[in] Vector3& angularVelocity
*  @param[in] float timeStep, 
*  @param[out] Vector3& temporalAABBMin, 
*  @param[out] Vector3& temporalAABBMax
*  @return 　　float
*****************************************************************************/
void ICollisionShape::CalculateTemporalAABB(const gm::AffineTransform& currentTrans, const Vector3& linearVelocity, const Vector3& angularVelocity, float timeStep, gm::Vector3& temporalAABBMin, gm::Vector3& temporalAABBMax) const
{
	GetAABB(currentTrans, temporalAABBMin, temporalAABBMax);

	float  temporalAabbMaxx = temporalAABBMax.GetX();
	float  temporalAabbMaxy = temporalAABBMax.GetY();
	float  temporalAabbMaxz = temporalAABBMax.GetZ();
	float  temporalAabbMinx = temporalAABBMin.GetX();
	float  temporalAabbMiny = temporalAABBMin.GetY();
	float  temporalAabbMinz = temporalAABBMin.GetZ();

	// add linear motion
	Vector3 linMotion = linearVelocity * timeStep;
	///@todo: simd would have a vector max/min operation, instead of per-element access
	if (linMotion.GetX() > 0.0f)
		temporalAabbMaxx += linMotion.GetX();
	else
		temporalAabbMinx += linMotion.GetX();
	if (linMotion.GetY() > 0.0f)
		temporalAabbMaxy += linMotion.GetY();
	else
		temporalAabbMiny += linMotion.GetY();
	if (linMotion.GetZ() > 0.0f)
		temporalAabbMaxz += linMotion.GetZ();
	else
		temporalAabbMinz += linMotion.GetZ();

	//add conservative angular motion
	float angularMotion = Norm(angularVelocity) * GetAngularMotionDisc() * timeStep;
	Vector3 angularMotion3d = Vector3(angularMotion, angularMotion, angularMotion);
	temporalAABBMin = Vector3(temporalAabbMinx, temporalAabbMiny, temporalAabbMinz);
	temporalAABBMax = Vector3(temporalAabbMaxx, temporalAabbMaxy, temporalAabbMaxz);

	temporalAABBMin -= angularMotion3d;
	temporalAABBMax += angularMotion3d;
}