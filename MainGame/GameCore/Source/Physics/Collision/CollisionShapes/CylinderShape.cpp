//////////////////////////////////////////////////////////////////////////////////
///             @file   CylinderShape.hpp
///             @brief  CylinderShape
///             @author Partially edit by Toide
///             @date   2021_09_19
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
#include "GameCore/Include/Physics/Collision/CollisionShapes/CylinderShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Static Function
/****************************************************************************
*						CylinderLocalSupportX
*************************************************************************//**
*  @fn        INLINE Vector3 CylinderLocalSupportX(const Vector3& halfExtents, const Vector3& v)
*  @brief     Set local orientation X axis;
*  @param[in] Vector3& halfExtents
*  @param[in] Vector3& v
*  @return 　　Vector3
*****************************************************************************/
INLINE Vector3 CylinderLocalSupportX(const Vector3& halfExtents, const Vector3& v)
{
	const int cylinderUpAxis = 0;
	const int XX = 1;
	const int YY = 0;
	const int ZZ = 2;

	/*-------------------------------------------------------------------
	-        Mapping depends on how cylinder local orientation is
	-   extents of the cylinder is: X,Y is for radius, and Z for height
	---------------------------------------------------------------------*/
	float radius = halfExtents.GetElement(XX);
	float halfHeight = halfExtents.GetElement(cylinderUpAxis);

	float tmp[3];
	float d;

	float s = Sqrt(v.GetElement(XX) * v.GetElement(XX) + v.GetElement(ZZ) * v.GetElement(ZZ));
	if (s != 0.0f)
	{
		d = radius / s;
		tmp[XX] = v.GetElement(XX) * d;
		tmp[YY] = v.GetElement(YY) < 0.0f ? -halfHeight : halfHeight;
		tmp[ZZ] = v.GetElement(ZZ) * d;
	}
	else
	{
		tmp[XX] = radius;
		tmp[YY] = v.GetElement(YY) < 0.0f ? -halfHeight : halfHeight;
		tmp[ZZ] = 0.0f;
	}

	return Vector3(tmp[0], tmp[1], tmp[2]);
}
/****************************************************************************
*						CylinderLocalSupportY
*************************************************************************//**
*  @fn        INLINE Vector3 CylinderLocalSupportY(const Vector3& halfExtents, const Vector3& v)
*  @brief     Set local orientation Y axis;
*  @param[in] Vector3& halfExtents
*  @param[in] Vector3& v
*  @return 　　Vector3
*****************************************************************************/
INLINE Vector3 CylinderLocalSupportY(const Vector3& halfExtents, const Vector3& v)
{
	const int cylinderUpAxis = 1;
	const int XX = 0;
	const int YY = 1;
	const int ZZ = 2;

	float radius     = halfExtents.GetElement(XX);
	float halfHeight = halfExtents.GetElement(cylinderUpAxis);

	float tmp[3];
	float   d;

	float s = Sqrt(v.GetElement(XX) * v.GetElement(XX) + v.GetElement(ZZ) * v.GetElement(ZZ));
	if (s != 0.0f)
	{
		d = radius / s;
		tmp[XX] = v.GetElement(XX) * d;
		tmp[YY] = v.GetElement(YY) < 0.0f ? -halfHeight : halfHeight;
		tmp[ZZ] = v.GetElement(ZZ) * d;
	}
	else
	{
		tmp[XX] = radius;
		tmp[YY] = v.GetElement(YY) < 0.0f ? -halfHeight : halfHeight;
		tmp[ZZ] = 0.0f;
	}
	return Vector3(tmp[0], tmp[1], tmp[2]);
}

/****************************************************************************
*						CylinderLocalSupportZ
*************************************************************************//**
*  @fn        INLINE Vector3 CylinderLocalSupportZ(const Vector3& halfExtents, const Vector3& v)
*  @brief     Set local orientation X axis;
*  @param[in] Vector3& halfExtents
*  @param[in] Vector3& v
*  @return 　　Vector3
*****************************************************************************/
INLINE Vector3 CylinderLocalSupportZ(const Vector3& halfExtents, const Vector3& v)
{
	const int cylinderUpAxis = 2;
	const int XX = 0;
	const int YY = 2;
	const int ZZ = 1;

	//mapping depends on how cylinder local orientation is
	// extents of the cylinder is: X,Y is for radius, and Z for height

	float radius     = halfExtents.GetElement(XX);
	float halfHeight = halfExtents.GetElement(cylinderUpAxis);

	float tmp[3];
	float d;

	float s = Sqrt(v.GetElement(XX) * v.GetElement(XX) + v.GetElement(ZZ) * v.GetElement(ZZ));
	if (s != 0.0f)
	{
		d = radius / s;
		tmp[XX] = v.GetElement(XX) * d;
		tmp[YY] = v.GetElement(YY) < 0.0f ? -halfHeight : halfHeight;
		tmp[ZZ] = v.GetElement(ZZ) * d;
	}
	else
	{
		tmp[XX] = radius;
		tmp[YY] = v.GetElement(YY) < 0.0f ? -halfHeight : halfHeight;
		tmp[ZZ] = 0.0f;
	}

	return Vector3(tmp[0], tmp[1], tmp[2]);
}
#pragma endregion Static Function

CylinderShape::CylinderShape(const gm::Vector3& halfExtents) : ConvexInternalShape()
{
	_upAxis = 1;
	Vector3 margin           = Vector3(GetMargin(), GetMargin(), GetMargin());
	_implicitShapeDimensions = (halfExtents * _localScaling) - margin;

	SetSafeMargin(halfExtents);

	_shapeType = (int)BroadPhaseNativeTypes::Cylinder_Shape;
}

#pragma region Public Function
/****************************************************************************
*						CalculateLocalInertia
*************************************************************************//**
*  @fn        void CylinderShape::CalculateLocalInertia(float mass, Vector3& inertia) const
*  @brief     Local inertia
*  @param[in] float mass
*  @param[in] Vector3& inertia
*  @return 　　void
*****************************************************************************/
void CylinderShape::CalculateLocalInertia(float mass, Vector3& inertia) const
{
	/*-------------------------------------------------------------------
	-        Prepare valiables
	---------------------------------------------------------------------*/
	float radius2;                                    // square of cylinder radius
	float height2;                                    // square of cylinder height
	Vector3 halfExtents = GetHalfExtentsWithMargin();  // get cylinder dimension
	float div12 = mass / 12.f;
	float div4 = mass / 4.f;
	float div2 = mass / 2.f;
	int idxRadius, idxHeight;

	/*-------------------------------------------------------------------
	-        Get indices of radius and height of cylinder
	---------------------------------------------------------------------*/
	switch (_upAxis)
	{
	case 0:  // cylinder is aligned along x
		idxRadius = 1;
		idxHeight = 0;
		break;
	case 2:  // cylinder is aligned along z
		idxRadius = 0;
		idxHeight = 2;
		break;
	default:  // cylinder is aligned along y
		idxRadius = 0;
		idxHeight = 1;
	}

	/*-------------------------------------------------------------------
	-        Calculate squares
	---------------------------------------------------------------------*/
	radius2 = halfExtents.GetElement(idxRadius) * halfExtents.GetElement(idxRadius);
	height2 = 4.0f * halfExtents.GetElement(idxHeight) * halfExtents.GetElement(idxHeight);

	/*-------------------------------------------------------------------
	-        Calculate tensor terms
	---------------------------------------------------------------------*/
	float t1 = div12 * height2 + div4 * radius2;
	float t2 = div2 * radius2;

	/*-------------------------------------------------------------------
	-        Set diagonal elements of inertia tensor
	---------------------------------------------------------------------*/
	switch (_upAxis)  
	{
	case 0:  // cylinder is aligned along x
		inertia = Vector3(t2, t1, t1);
		break;
	case 2:  // cylinder is aligned along z
		inertia = Vector3(t1, t1, t2);
		break;
	default:  // cylinder is aligned along y
		inertia = Vector3(t1, t2, t1);
	}
}

/****************************************************************************
*						LocalGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        Vector3 CylinderShape::LocalGetSupportingVertexWithoutMargin(const Vector3& vector) const
*  @brief     Get local orientation without margin
*  @param[in] float mass
*  @param[in] Vector3& inertia
*  @return 　　Vector3
*****************************************************************************/
Vector3 CylinderShape::LocalGetSupportingVertexWithoutMargin(const Vector3& vector) const
{
	return CylinderLocalSupportY(GetHalfExtentsWithoutMargin(), vector);
}
/****************************************************************************
*						BatchedUnitVectorGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        void CylinderShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const Vector3* vectors, Vector3* supportVerticesOut, int numVectors) const
*  @brief     Get supporting vertex without margin (total vertices)
*  @param[in] const Vector3* vectors,
*  @param[out] Vector3* supportVerticesOut,
*  @param[in]  int numVectors
*  @return 　　void
*****************************************************************************/
void CylinderShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const Vector3* vectors, Vector3* supportVerticesOut, int numVectors) const
{
	for (int i = 0; i < numVectors; i++)
	{
		supportVerticesOut[i] = CylinderLocalSupportY(GetHalfExtentsWithoutMargin(), vectors[i]);
	}
}
/****************************************************************************
*						LocalGetSupportingVertex
*************************************************************************//**
*  @fn        Vector3 CylinderShape::LocalGetSupportingVertex(const Vector3& vector) const
*  @brief     LocalGetSupportingVertex
*  @param[in] Vector3& vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 CylinderShape::LocalGetSupportingVertex(const Vector3& vector) const
{
	Vector3 supVertex;
	supVertex = LocalGetSupportingVertexWithoutMargin(vector);

	if (GetMargin() != 0.0f)
	{
		Vector3 vecnorm = vector;
		if (NormSquared(vecnorm) < (FLT_EPSILON * FLT_EPSILON))
		{
			vecnorm = Vector3(-1.0f, -1.0f, -1.0f);
		}
		vecnorm    = Normalize(vecnorm);
		supVertex += GetMargin() * vecnorm;
	}
	return supVertex;
}
/****************************************************************************
*						GetAABB
*************************************************************************//**
*  @fn        void BoxShape::GetAABB(const AffineTransform& transform, Vector3& aabbMin, Vector3& aabbMax) const
*  @brief     Get aabb
*  @param[in] AffineTransform transform
*  @param[out] aabbMin
*  @param[out] aabbMax
*  @return 　　void
*****************************************************************************/
void CylinderShape::GetAABB(const AffineTransform& transform, Vector3& aabbMin, Vector3& aabbMax) const
{
	TransformAABB(GetHalfExtentsWithoutMargin(), GetMargin(), transform, aabbMin, aabbMax);
}
/****************************************************************************
*						GetHalfExtentsWithMargin
*************************************************************************//**
*  @fn        Vector3 BoxShape::GetHalfExtentsWithMargin() const
*  @brief     Get half extents with margin
*  @param[in] void
*  @return 　　Vector3
*****************************************************************************/
Vector3 CylinderShape::GetHalfExtentsWithMargin() const
{
	Vector3 halfExtents = GetHalfExtentsWithoutMargin();
	Vector3 margin(GetMargin(), GetMargin(), GetMargin());
	halfExtents += margin;
	return halfExtents;
}
/****************************************************************************
*						GetAnisotropicRollingFrictionDirectio
*************************************************************************//**
*  @fn        Vector3 CylinderShape::GetAnisotropicRollingFrictionDirection() const
*  @brief     GetAnisotropicRollingFrictionDirectio
*  @param[in] void
*  @return 　　Vector3
*****************************************************************************/
Vector3 CylinderShape::GetAnisotropicRollingFrictionDirection() const
{
	Vector3 aniDir(0, 0, 0);
	aniDir.SetElement(GetUpAxis(), 1);
	return aniDir;
}
/****************************************************************************
*						SetMargin
*************************************************************************//**
*  @fn        void BoxShape::SetMargin(float collisionMargin)
*  @brief     Set margin
*  @param[in] float collisionMargin
*  @return 　　void
*****************************************************************************/
void CylinderShape::SetMargin(float collisionMargin)
{
	Vector3 oldMargin(GetMargin(), GetMargin(), GetMargin());
	Vector3 implicitShapeDimensionsWithMargin = _implicitShapeDimensions + oldMargin;

	ConvexInternalShape::SetMargin(collisionMargin);
	Vector3 newMargin(GetMargin(), GetMargin(), GetMargin());
	_implicitShapeDimensions = implicitShapeDimensionsWithMargin - newMargin;
}
/****************************************************************************
*						SetLocalScaling
*************************************************************************//**
*  @fn        void BoxShape::SetLocalScaling(const Vector3& scaling)
*  @brief     Set local scaling
*  @param[in] Vector3 scaling
*  @return 　　void
*****************************************************************************/
void CylinderShape::SetLocalScaling(const gm::Vector3& scaling)
{
	Vector3 oldMargin(GetMargin(), GetMargin(), GetMargin());
	Vector3 implicitShapeDimensionsWithMargin         = _implicitShapeDimensions + oldMargin;
	Vector3 unScaledImplicitShapeDimensionsWithMargin = implicitShapeDimensionsWithMargin / _localScaling;

	ConvexInternalShape::SetLocalScaling(scaling);

	_implicitShapeDimensions = (unScaledImplicitShapeDimensionsWithMargin * _localScaling) - oldMargin;
}

#pragma endregion Public Function

#pragma region CylinderShapeX-Z
CylinderShapeX::CylinderShapeX(const Vector3& halfExtents) : CylinderShape(halfExtents)
{
	_upAxis = 0;
}

CylinderShapeZ::CylinderShapeZ(const Vector3& halfExtents) : CylinderShape(halfExtents)
{
	_upAxis = 2;
}

gm::Vector3 CylinderShapeX::LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const
{
	return CylinderLocalSupportX(GetHalfExtentsWithoutMargin(), vector);
}

void CylinderShapeX::BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
{
	for (int i = 0; i < numVectors; i++)
	{
		supportVerticesOut[i] = CylinderLocalSupportX(GetHalfExtentsWithoutMargin(), vectors[i]);
	}
}

gm::Vector3 CylinderShapeZ::LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const
{
	return CylinderLocalSupportZ(GetHalfExtentsWithoutMargin(), vector);
}

void CylinderShapeZ::BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
{
	for (int i = 0; i < numVectors; i++)
	{
		supportVerticesOut[i] = CylinderLocalSupportZ(GetHalfExtentsWithoutMargin(), vectors[i]);
	}
}
#pragma endregion CylinderShapeX-Z