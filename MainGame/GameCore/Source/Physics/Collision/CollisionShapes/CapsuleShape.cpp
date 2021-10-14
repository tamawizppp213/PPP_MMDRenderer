//////////////////////////////////////////////////////////////////////////////////
///             @file   CapsuleShape.hpp
///             @brief  CapsuleShape
///             @author Partially edit by Toide
///             @date   2021_09_20
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

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionShapes/CapsuleShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
CapsuleShape::CapsuleShape(float radius, float height)
{
	_collisionMargin         = radius;
	_shapeType               = (int)BroadPhaseNativeTypes::Capsule_Shape;
	_upAxis                  = 1;
	_implicitShapeDimensions = Vector3(radius, 0.5f * height, radius);
}

/****************************************************************************
*						CalculateLocalInertia
*************************************************************************//**
*  @fn        void CapsuleShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
*  @brief     Calculate Local inertia 
*  @param[in] float mass
*  @param[out] Vector3& inertia
*  @return 　　void
*****************************************************************************/
void CapsuleShape::CalculateLocalInertia(float mass, Vector3& inertia) const
{
	//as an approximation, take the inertia of the box that bounds the spheres

	AffineTransform ident;

	float radius = GetRadius();

	Vector3 halfExtents(radius, radius, radius);
	halfExtents.SetElement(GetUpAxis(), halfExtents.GetElement(GetUpAxis()) + GetHalfHeight());

	float lx = 2.0f * (halfExtents.GetX());
	float ly = 2.0f * (halfExtents.GetY());
	float lz = 2.0f * (halfExtents.GetZ());
	const float x2 = lx * lx;
	const float y2 = ly * ly;
	const float z2 = lz * lz;
	const float scaledmass = mass * 0.08333333f;

	inertia.GetX() = scaledmass * (y2 + z2);
	inertia.GetY() = scaledmass * (x2 + z2);
	inertia.GetZ() = scaledmass * (x2 + y2);
}
/****************************************************************************
*						LocalGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        Vector3 CapsuleShape::LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const
*  @brief     LocalGetSupportingVertexWithoutMargin
*  @param[in] Vector3& vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 CapsuleShape::LocalGetSupportingVertexWithoutMargin(const Vector3& vector) const
{
	Vector3 supVec(0, 0, 0);

	float maxDot(-FLT_MAX);

	Vector3 vec  = vector;
	float lenSqr = NormSquared(vec);
	if (lenSqr < 0.0001f) { vec = Vector3(1, 0, 0); }
	else
	{
		float rlen = 1.0f / Sqrt(lenSqr);
		vec = rlen * vec;
	}

	Vector3 vertex;
	float newDot;

	{
		Vector3 pos(0, 0, 0);
		pos.SetElement(GetUpAxis(), GetHalfHeight());

		vertex = pos;
		newDot = Dot(vec, vertex);
		if (newDot > maxDot)
		{
			maxDot = newDot;
			supVec = vertex;
		}
	}
	{
		Vector3 pos(0, 0, 0);
		pos.SetElement(GetUpAxis(), -GetHalfHeight());

		vertex = pos;
		newDot = Dot(vec, vertex);
		if (newDot > maxDot)
		{
			maxDot = newDot;
			supVec = vertex ;
		}
	}

	return supVec;
}
/****************************************************************************
*						BatchedUnitVectorGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        void CapsuleShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
*  @brief     BatchedUnitVectorGetSupportingVertexWithoutMargin
*  @param[in] Vector3* vectors
*  @param[out]Vector3* supportVerticesOut, 
*  @param[in] int numVectors
*  @return 　　void
*****************************************************************************/
void CapsuleShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const Vector3* vectors, Vector3* supportVerticesOut, int numVectors) const
{
	for (int j = 0; j < numVectors; j++)
	{
		float maxDot(-FLT_MAX);
		const Vector3& vec = vectors[j];

		Vector3 vertex;
		float newDot;
		{
			Vector3 pos(0, 0, 0);
			pos.SetElement(GetUpAxis(), GetHalfHeight());
			vertex = pos;
			newDot = Dot(vec, vertex);
			if (newDot > maxDot)
			{
				maxDot = newDot;
				supportVerticesOut[j] = vertex;
			}
		}
		{
			Vector3 pos(0, 0, 0);
			pos.SetElement(GetUpAxis(), -GetHalfHeight());
			vertex = pos;
			newDot = Dot(vec, vertex);
			if (newDot > maxDot)
			{
				maxDot = newDot;
				supportVerticesOut[j] = vertex;
			}
		}
	}
}
/****************************************************************************
*						GetAABB
*************************************************************************//**
*  @fn        void CapsuleShape::GetAABB(const gm::Transform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
*  @brief     Get AABB
*  @param[in] const gm::Transform& transform,
*  @param[out]Vector3& aabbMin, 
*  @param[out]Vector3& aabbMax
*  @return 　　void
*****************************************************************************/
void CapsuleShape::GetAABB(const AffineTransform& transform, Vector3& aabbMin, Vector3& aabbMax) const
{
	Vector3 halfExtents(GetRadius(), GetRadius(), GetRadius());
	halfExtents.SetElement(_upAxis, GetRadius() + GetHalfHeight());
	gm::Matrix3 abs_b = Absolute(transform.GetBasis());
	Vector3 center     = transform.GetTranslation();
	gm::Vector3 extent = abs_b * halfExtents;

	aabbMin = center - extent;
	aabbMax = center + extent;
}
/****************************************************************************
*						GetAnisotropicRollingFrictionDirection
*************************************************************************//**
*  @fn        Vector3 CapsuleShape::GetAnisotropicRollingFrictionDirection() const
*  @brief     GetAnisotropicRollingFrictionDirection
*  @param[in] void
*  @return 　　Vector3
*****************************************************************************/
Vector3 CapsuleShape::GetAnisotropicRollingFrictionDirection() const
{
	Vector3 aniDir(0, 0, 0);
	aniDir.SetElement(GetUpAxis(), 1);
	return aniDir;
}

/****************************************************************************
*						SetLocalScaling
*************************************************************************//**
*  @fn        void CapsuleShape::SetLocalScaling(const gm::Vector3& scaling)
*  @brief     SetLocalScaling
*  @param[in] Vector3 scaling
*  @return 　　void
*****************************************************************************/
void CapsuleShape::SetLocalScaling(const gm::Vector3& scaling)
{
	Vector3 unScaledImplicitShapeDimensions = _implicitShapeDimensions / _localScaling;
	ConvexInternalShape::SetLocalScaling(scaling);
	_implicitShapeDimensions = (unScaledImplicitShapeDimensions * scaling);
	//update m_collisionMargin, since entire radius==margin
	int radiusAxis   = (_upAxis + 2) % 3;
	_collisionMargin = _implicitShapeDimensions.GetElement(radiusAxis);
}
#pragma endregion Public Function

CapsuleShapeX::CapsuleShapeX(float radius, float height)
{
	_collisionMargin = radius;
	_upAxis = 0;
	_implicitShapeDimensions = Vector3(0.5f * height, radius, radius);
}

CapsuleShapeZ::CapsuleShapeZ(float radius, float height)
{
	_collisionMargin         = radius;
	_upAxis                  = 2;
	_implicitShapeDimensions = Vector3(radius, radius, 0.5f * height);
}

