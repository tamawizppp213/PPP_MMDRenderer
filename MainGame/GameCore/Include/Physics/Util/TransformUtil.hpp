//////////////////////////////////////////////////////////////////////////////////
///             @file   AABBUtil.hpp
///             @brief  AABB functions
///             @author Partially Edit by Toide
///             @date   2021_09_18
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
#ifndef AABBUTIL_HPP
#define AABBUTIL_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMTransform.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Implement
//////////////////////////////////////////////////////////////////////////////////
#define ANGULAR_MOTION_THRESHOLD 0.5f * gm::GM_PI * 0.5f
class AffineTransformUtil
{
public:
	static void IntegrateTransform(const gm::AffineTransform& currentTransform, const gm::Vector3& linearVelocity, const gm::Vector3& angularVelocity, float timeStep, gm::AffineTransform& predictedTransform)
	{
		predictedTransform.SetTranslation(currentTransform.GetTranslation() + linearVelocity * timeStep);

		gm::Vector3 axis;
		gm::Scalar fAngle2 = NormSquared(angularVelocity);
		gm::Scalar fAngle = 0;
		if (fAngle2 > FLT_EPSILON)
		{
			fAngle = Sqrt(fAngle2);
		}

		//limit the angular motion
		if (fAngle * timeStep > ANGULAR_MOTION_THRESHOLD)
		{
			fAngle = ANGULAR_MOTION_THRESHOLD / timeStep;
		}

		if (fAngle < 0.001f)
		{
			// use Taylor's expansions of sync function
			axis = angularVelocity * (0.5f * timeStep - static_cast<float>(((double)timeStep * timeStep * timeStep) * (0.020833333333))) * fAngle * fAngle;
		}
		else
		{
			// sync(fAngle) = sin(c*fAngle)/t
			axis = angularVelocity * (Sin(0.5f * fAngle * timeStep) / fAngle);
		}
		gm::Quaternion dorn(axis, fAngle * timeStep * 0.5f);
		gm::Quaternion orn0 = currentTransform.GetRotation();

		gm::Quaternion predictedOrn = dorn * orn0;
		if (Dot(predictedOrn, predictedOrn) > FLT_EPSILON) { predictedOrn = Normalize(predictedOrn); }

		if (Dot(predictedOrn, predictedOrn) > FLT_EPSILON)
		{
			predictedTransform = gm::RotationQuaternionMatrix3(orn0);
		}
		else
		{
			predictedTransform = currentTransform;
		}
	}
	static void CalculateVelocity(const gm::AffineTransform& transform0, const gm::AffineTransform& transform1, float timeStep, gm::Vector3& linearVelocity, gm::Vector3& angularVelocity)
	{
		linearVelocity = (transform1.GetTranslation() - transform1.GetTranslation()) / timeStep;
		gm::Vector3 axis;
		float       angle;
		CalculateDiffAxisAngle(transform0, transform1, axis, angle);
		angularVelocity = axis * angle / timeStep;
	}
	static void CalculateDiffAxisAngle(const gm::AffineTransform& transform0, const gm::AffineTransform& transform1, gm::Vector3& axis, float& angle)
	{
		gm::Matrix3 matrix = transform1.GetBasis() * gm::InverseMatrix3(transform0.GetBasis());
	}
	static gm::AffineTransform InverseTimes(const gm::AffineTransform& a, const gm::AffineTransform& b)
	{
		gm::Vector3 v = b.GetTranslation() - a.GetTranslation();
		return gm::AffineTransform(gm::Transpose(gm::HadamaldProduct(a.GetBasis(), b.GetBasis())), a.GetBasis() * v);
	}
};

INLINE void TransformAABB(const gm::Vector3& halfExtents, float margin, const gm::AffineTransform& transform, gm::Vector3& aabbMinOut, gm::Vector3& aabbMaxOut)
{

	gm::Vector3 halfExtentsWithMargin = halfExtents + gm::Vector3(margin, margin, margin);
	gm::Matrix3 abs_b = Absolute(transform.GetBasis());

	gm::Vector3 center = transform.GetTranslation();
	gm::Vector3 extent = abs_b * halfExtentsWithMargin;
	aabbMinOut = center - extent;
	aabbMaxOut = center + extent;
}

INLINE void TransformAABB(const gm::Vector3& localAabbMin, const gm::Vector3& localAabbMax, float margin, const gm::AffineTransform& trans, gm::Vector3& aabbMinOut, gm::Vector3& aabbMaxOut)
{
	assert((float)localAabbMin.GetX() <= localAabbMax.GetX());
	assert((float)localAabbMin.GetY() <= localAabbMax.GetY());
	assert((float)localAabbMin.GetZ() <= localAabbMax.GetZ());
	gm::Vector3 localHalfExtents = 0.5f * (localAabbMax - localAabbMin);
	localHalfExtents += gm::Vector3(margin, margin, margin);

	gm::Vector3 localCenter = 0.5f * (localAabbMax + localAabbMin);
	gm::Matrix3 abs_b  = Absolute(trans.GetBasis());
	gm::Vector3 center = trans * localCenter;
	gm::Vector3 extent = abs_b * localHalfExtents;
	aabbMinOut = center - extent;
	aabbMaxOut = center + extent;
}

INLINE int btSelect(unsigned condition, int valueIfConditionNonZero, int valueIfConditionZero)
{
	unsigned testNz = (unsigned)(((int)condition | -(int)condition) >> 31);
	unsigned testEqz = ~testNz;
	return static_cast<int>((valueIfConditionNonZero & testNz) | (valueIfConditionZero & testEqz));
}

INLINE unsigned TestQuantizedAABBAgainstQuantizedAABB(const unsigned short int* aabbMin1, const unsigned short int* aabbMax1, const unsigned short int* aabbMin2, const unsigned short int* aabbMax2)
{
	return static_cast<unsigned int>(btSelect((unsigned)((aabbMin1[0] <= aabbMax2[0]) & (aabbMax1[0] >= aabbMin2[0]) & (aabbMin1[2] <= aabbMax2[2]) & (aabbMax1[2] >= aabbMin2[2]) & (aabbMin1[1] <= aabbMax2[1]) & (aabbMax1[1] >= aabbMin2[1])),1, 0));
}


INLINE bool RayAABB2(const gm::Vector3& rayFrom,
	const gm::Vector3& rayInvDirection,
	const unsigned int raySign[3],
	const gm::Vector3 bounds[2],
	float& tmin,
	float lambda_min,
	float lambda_max)
{
	float tmax, tymin, tymax, tzmin, tzmax;
	tmin = (bounds[raySign[0]].GetX() - rayFrom.GetX()) * rayInvDirection.GetX();
	tmax = (bounds[1 - raySign[0]].GetX() - rayFrom.GetX()) * rayInvDirection.GetX();
	tymin = (bounds[raySign[1]].GetY() - rayFrom.GetY()) * rayInvDirection.GetY();
	tymax = (bounds[1 - raySign[1]].GetY() - rayFrom.GetY()) * rayInvDirection.GetY();

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[raySign[2]].GetZ() - rayFrom.GetZ()) * rayInvDirection.GetZ();
	tzmax = (bounds[1 - raySign[2]].GetZ() - rayFrom.GetZ()) * rayInvDirection.GetZ();

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	return ((tmin < lambda_max) && (tmax > lambda_min));
}

/// conservative test for overlap between two aabbs
INLINE bool TestAABBAgainstAABB2(const gm::Vector3& aabbMin1, const gm::Vector3& aabbMax1,
	const gm::Vector3& aabbMin2, const gm::Vector3& aabbMax2)
{
	bool overlap = true;
	overlap = ((float)aabbMin1.GetX() > aabbMax2.GetX() || (float)aabbMax1.GetX() < aabbMin2.GetX()) ? false : overlap;
	overlap = ((float)aabbMin1.GetZ() > aabbMax2.GetZ() || (float)aabbMax1.GetZ() < aabbMin2.GetZ()) ? false : overlap;
	overlap = ((float)aabbMin1.GetY() > aabbMax2.GetY() || (float)aabbMax1.GetY() < aabbMin2.GetY()) ? false : overlap;
	return overlap;
}
#endif