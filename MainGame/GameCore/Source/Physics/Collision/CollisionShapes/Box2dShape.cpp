//////////////////////////////////////////////////////////////////////////////////
///             @file   Box2DShape.hpp
///             @brief  Box2DShape
///             @author Partially edit by Toide
///             @date   2021_10_06 (first checked) first checked -> no debug.
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
#include "GameCore/Include/Physics/Collision/CollisionShapes/Box2dShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
INLINE float Fsel(float a, float b, float c) { return a >= 0 ? b : c; }
//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
Box2DShape::Box2DShape(const Vector3& boxHalfExtents) : PolyhedralConvexShape(), _centroid(0, 0, 0)
{
	_vertices[0] = Vector3(-boxHalfExtents.GetX(), -boxHalfExtents.GetY(), 0);
	_vertices[1] = Vector3(boxHalfExtents.GetX(), -boxHalfExtents.GetY(), 0);
	_vertices[2] = Vector3(boxHalfExtents.GetX(), boxHalfExtents.GetY(), 0);
	_vertices[3] = Vector3(-boxHalfExtents.GetX(), boxHalfExtents.GetY(), 0);

	_normals[0] = Vector3(0, -1, 0);
	_normals[1] = Vector3(1, 0, 0);
	_normals[2] = Vector3(0, 1, 0);
	_normals[3] = Vector3(-1, 0, 0);

	float minDimension = boxHalfExtents.GetX();
	if ((float)minDimension > boxHalfExtents.GetY())
		minDimension = boxHalfExtents.GetY();

	_shapeType = (int)BroadPhaseNativeTypes::Box2D_Shape;
	Vector3 margin(GetMargin(), GetMargin(), GetMargin());
	_implicitShapeDimensions = (boxHalfExtents * _localScaling) - margin;

	SetSafeMargin(minDimension);
}

#pragma region Public Function
/****************************************************************************
*						LocalGetSupportingVertex
*************************************************************************//**
*  @fn        Vector3 BoxShape::LocalGetSupportingVertex(const Vector3& vector) const
*  @brief     Local get supporting vertex
*  @param[in] Vector3& vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 Box2DShape::GetHalfExtentsWithMargin() const
{
	Vector3 halfExtents = GetHalfExtentsWithoutMargin();
	Vector3 margin(GetMargin(), GetMargin(), GetMargin());
	halfExtents += margin;
	return halfExtents;
}
/****************************************************************************
*						LocalGetSupportingVertex
*************************************************************************//**
*  @fn        Vector3 BoxShape::LocalGetSupportingVertex(const Vector3& vector) const
*  @brief     Local get supporting vertex
*  @param[in] Vector3& vector
*  @return 　　Vector3
*****************************************************************************/
const Vector3& Box2DShape::GetHalfExtentsWithoutMargin() const
{
	return _implicitShapeDimensions;
}
/****************************************************************************
*						LocalGetSupportingVertex
*************************************************************************//**
*  @fn        Vector3 Box2DShape::LocalGetSupportingVertex(const Vector3& vector) const
*  @brief     Local get supporting vertex
*  @param[in] Vector3& vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 Box2DShape::LocalGetSupportingVertex(const Vector3& vector) const
{
	Vector3 halfExtents = GetHalfExtentsWithoutMargin();
	Vector3 margin = Vector3(GetMargin(), GetMargin(), GetMargin());
	halfExtents += margin;

	return Vector3(
		Fsel(vector.GetX(), halfExtents.GetX(), -halfExtents.GetX()),
		Fsel(vector.GetY(), halfExtents.GetY(), -halfExtents.GetY()),
		Fsel(vector.GetZ(), halfExtents.GetZ(), -halfExtents.GetZ())
	);
}
/****************************************************************************
*						LocalGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        Vector3 BoxShape::LocalGetSupportingVertexWithoutMargin(const Vector3& vector)
*  @brief     Get local supporting vertex
*  @param[in] Vector3 vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 Box2DShape::LocalGetSupportingVertexWithoutMargin(const Vector3& vector)
{
	Vector3 halfExtents = GetHalfExtentsWithoutMargin();

	return Vector3(
		Fsel(vector.GetX(), halfExtents.GetX(), -halfExtents.GetX()),
		Fsel(vector.GetY(), halfExtents.GetY(), -halfExtents.GetY()),
		Fsel(vector.GetZ(), halfExtents.GetZ(), -halfExtents.GetZ())
	);
}
/****************************************************************************
*						BatchedUnitVectorGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        void BoxShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const Vector3* vectors, Vector3* supportVerticesOut, int numVectors) const
*  @brief     Get supporting vertex without margin (total vertices)
*  @param[in] const Vector3* vectors,
*  @param[out] Vector3* supportVerticesOut,
*  @param[in]  int numVectors
*  @return 　　void
*****************************************************************************/
void Box2DShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const Vector3* vectors, Vector3* supportVerticesOut, int numVectors) const

{
	const Vector3& halfExtents = GetHalfExtentsWithoutMargin();

	for (int i = 0; i < numVectors; ++i)
	{
		const Vector3& vector = GetHalfExtentsWithoutMargin();
		supportVerticesOut[i] = Vector3(
			Fsel(vector.GetX(), halfExtents.GetX(), -halfExtents.GetX()),
			Fsel(vector.GetY(), halfExtents.GetY(), -halfExtents.GetY()),
			Fsel(vector.GetZ(), halfExtents.GetZ(), -halfExtents.GetZ())
		);
	}
}
/****************************************************************************
*						CalculateLocalInertia
*************************************************************************//**
*  @fn        void BoxShape::CalculateLocalInertia(float mass, Vector3& inertia) const
*  @brief     Calculate inertia
*  @param[in] float mass
*  @param[out] Vector3 inertia
*  @return 　　void
*****************************************************************************/
void Box2DShape::CalculateLocalInertia(float mass, Vector3& inertia) const
{
	Vector3 halfExtents = GetHalfExtentsWithMargin();

	Scalar lx = Scalar(2.) * (halfExtents.GetX());
	Scalar ly = Scalar(2.) * (halfExtents.GetY());
	Scalar lz = Scalar(2.) * (halfExtents.GetZ());

	inertia = Vector3(
		mass / (Scalar(12.0)) * (ly * ly + lz * lz),
		mass / (Scalar(12.0)) * (lx * lx + lz * lz),
		mass / (Scalar(12.0)) * (lx * lx + ly * ly));
}
/****************************************************************************
*						SetMargin
*************************************************************************//**
*  @fn        void BoxShape::SetMargin(float collisionMargin)
*  @brief     Set margin
*  @param[in] float collisionMargin
*  @return 　　void
*****************************************************************************/
void Box2DShape::SetMargin(float collisionMargin)
{
	Vector3 oldMargin = Vector3(GetMargin(), GetMargin(), GetMargin());
	Vector3 implicitShapeDimensionsWithMargin = _implicitShapeDimensions + oldMargin;

	ConvexInternalShape::SetMargin(collisionMargin);
	Vector3 newMargin = Vector3(GetMargin(), GetMargin(), GetMargin());
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
void Box2DShape::SetLocalScaling(const Vector3& scaling)
{
	Vector3 oldMargin = Vector3(GetMargin(), GetMargin(), GetMargin());
	Vector3 implicitShapeDimensionsWithMargin = _implicitShapeDimensions + oldMargin;
	Vector3 unScaledImplicitShapeDimensionsWithMargin = implicitShapeDimensionsWithMargin / _localScaling;

	ConvexInternalShape::SetLocalScaling(scaling);

	_implicitShapeDimensions = (unScaledImplicitShapeDimensionsWithMargin * _localScaling) - oldMargin;
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
void Box2DShape::GetAABB(const AffineTransform& transform, Vector3& aabbMin, Vector3& aabbMax) const
{
	TransformAABB(GetHalfExtentsWithoutMargin(), GetMargin(), transform, aabbMin, aabbMax);
}
/****************************************************************************
*						GetPlane
*************************************************************************//**
*  @fn        void BoxShape::GetPlane(Vector3& planeNormal, Vector3& planeSupport, int i) const
*  @brief     Get plane infomation
*  @param[out] Vector3 planeNormal
*  @param[out] Vector3 planeSupport
*  @param[in]  int index
*  @return 　　void
*****************************************************************************/
void Box2DShape::GetPlane(Vector3& planeNormal, Vector3& planeSupport, int i) const
{
	Vector4 plane;
	GetPlaneEquation(plane, i);
	planeNormal = Vector3(plane.GetX(), plane.GetY(), plane.GetZ());
	planeSupport = LocalGetSupportingVertex(-planeNormal);
}
/****************************************************************************
*						GetVertex
*************************************************************************//**
*  @fn        void BoxShape::GetVertex(int index, Vector3& vertex) const
*  @brief     Get box shape vertex
*  @param[in] int index
*  @param[out] Vector3 vertex
*  @return 　　void
*****************************************************************************/
void Box2DShape::GetVertex(int index, Vector3& vertex) const
{
	Vector3 halfExtents = GetHalfExtentsWithMargin();

	vertex = Vector3(
		(float)halfExtents.GetX() * (1 - (index & 1)) - (float)halfExtents.GetX() * (index & 1),
		(float)halfExtents.GetY() * (1 - ((index & 2) >> 1)) - (float)halfExtents.GetY() * ((index & 2) >> 1),
		(float)halfExtents.GetZ() * (1 - ((index & 4) >> 2)) - (float)halfExtents.GetZ() * ((index & 4) >> 2));
}
/****************************************************************************
*						GetPlaneEquation
*************************************************************************//**
*  @fn        void BoxShape::GetPlaneEquation(gm::Vector4& plane, int i) const
*  @brief     Get plane equation
*  @param[out] Vector4 plane equation
*  @param[in]  int index
*  @return 　　void
*****************************************************************************/
void Box2DShape::GetPlaneEquation(gm::Vector4& plane, int i) const
{
	Vector3 halfExtents = GetHalfExtentsWithoutMargin();

	switch (i)
	{
	case 0:
		plane = Vector4(Scalar(1.), Scalar(0.), Scalar(0.), -halfExtents.GetX());
		break;
	case 1:
		plane = Vector4(Scalar(-1.), Scalar(0.), Scalar(0.), -halfExtents.GetX());
		break;
	case 2:
		plane = Vector4(Scalar(0.), Scalar(1.), Scalar(0.), -halfExtents.GetY());
		break;
	case 3:
		plane = Vector4(Scalar(0.), Scalar(-1.), Scalar(0.), -halfExtents.GetY());
		break;
	case 4:
		plane = Vector4(Scalar(0.), Scalar(0.), Scalar(1.), -halfExtents.GetZ());
		break;
	case 5:
		plane = Vector4(Scalar(0.), Scalar(0.), Scalar(-1.), -halfExtents.GetZ());
		break;
	default:
		assert(0);
	}
}
/****************************************************************************
*						GetEdge
*************************************************************************//**
*  @fn        void BoxShape::GetEdge(int i, gm::Vector3& pa, gm::Vector3& pb) const
*  @brief     Get vertex edge
*  @param[in] int index
*  @param[out] Vector3 pa
*  @param[out] Vector3 pb
*  @return 　　void
*****************************************************************************/
void Box2DShape::GetEdge(int i, Vector3& pa, Vector3& pb) const
{
	int edgeVert0 = 0;
	int edgeVert1 = 0;

	switch (i)
	{
	case 0:
		edgeVert0 = 0;
		edgeVert1 = 1;
		break;
	case 1:
		edgeVert0 = 0;
		edgeVert1 = 2;
		break;
	case 2:
		edgeVert0 = 1;
		edgeVert1 = 3;

		break;
	case 3:
		edgeVert0 = 2;
		edgeVert1 = 3;
		break;
	case 4:
		edgeVert0 = 0;
		edgeVert1 = 4;
		break;
	case 5:
		edgeVert0 = 1;
		edgeVert1 = 5;

		break;
	case 6:
		edgeVert0 = 2;
		edgeVert1 = 6;
		break;
	case 7:
		edgeVert0 = 3;
		edgeVert1 = 7;
		break;
	case 8:
		edgeVert0 = 4;
		edgeVert1 = 5;
		break;
	case 9:
		edgeVert0 = 4;
		edgeVert1 = 6;
		break;
	case 10:
		edgeVert0 = 5;
		edgeVert1 = 7;
		break;
	case 11:
		edgeVert0 = 6;
		edgeVert1 = 7;
		break;
	default:
		assert(0);
	}

	GetVertex(edgeVert0, pa);
	GetVertex(edgeVert1, pb);
}
/****************************************************************************
*						IsInside
*************************************************************************//**
*  @fn        bool BoxShape::IsInside(const gm::Vector3& pt, float tolerance) const
*  @brief     Is Box inside
*  @param[in] Vector3 point
*  @param[in] float tolerance
*  @return 　　bool
*****************************************************************************/
bool Box2DShape::IsInside(const Vector3& pt, float tolerance) const
{
	Vector3 halfExtents = GetHalfExtentsWithoutMargin();

	//btScalar minDist = 2*tolerance;

	bool result =
		((float)pt.GetX() <= ((float)halfExtents.GetX() + tolerance)) &&
		((float)pt.GetX() >= ((float)-halfExtents.GetX() - tolerance)) &&
		((float)pt.GetY() <= ((float)halfExtents.GetY() + tolerance)) &&
		((float)pt.GetY() >= ((float)-halfExtents.GetY() - tolerance)) &&
		((float)pt.GetZ() <= ((float)halfExtents.GetZ() + tolerance)) &&
		((float)pt.GetZ() >= ((float)-halfExtents.GetZ() - tolerance));

	return result;
}
/****************************************************************************
*						GetPreferredPenetrationDirection
*************************************************************************//**
*  @fn        void BoxShape::GetPreferredPenetrationDirection(int index, Vector3& penetrationVector) const
*  @brief     Get preferred penetration direction
*  @param[in] int index
*  @param[out] Vector3 penetrationVector
*  @return 　　void
*****************************************************************************/
void Box2DShape::GetPreferredPenetrationDirection(int index, Vector3& penetrationVector) const
{
	switch (index)
	{
	case 0:
		penetrationVector = Vector3(Scalar(1.), Scalar(0.), Scalar(0.));
		break;
	case 1:
		penetrationVector = Vector3(Scalar(-1.), Scalar(0.), Scalar(0.));
		break;
	case 2:
		penetrationVector = Vector3(Scalar(0.), Scalar(1.), Scalar(0.));
		break;
	case 3:
		penetrationVector = Vector3(Scalar(0.), Scalar(-1.), Scalar(0.));
		break;
	case 4:
		penetrationVector = Vector3(Scalar(0.), Scalar(0.), Scalar(1.));
		break;
	case 5:
		penetrationVector = Vector3(Scalar(0.), Scalar(0.), Scalar(-1.));
		break;
	default:
		assert(0);
	}
}