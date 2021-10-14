//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexShape.cpp
///             @brief  Convex Shape
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
#include "GameCore/Include/Physics/Collision/CollisionShapes/ConvexShape.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/TriangleShape.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/SphereShape.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/CylinderShape.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/ConeShape.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/CapsuleShape.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/ConvexHullShape.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/ConvexPointCloudShape.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
INLINE float Fsels(float a, float b, float c) { return a >= 0 ? b : c; }
static Vector3 ConvexHullSupport(const Vector3& localDirectionOrg, const Vector3* points, int pointCount, const Vector3& localScaling);
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region ConvexShape
/****************************************************************************
*						Project
*************************************************************************//**
*  @fn        void ConvexShape::Project(const Transform& transform, const Vector3& direction, float& minProj, float& maxProj, Vector3& witnesPtMin, Vector3& witnesPtMax) const
*  @brief     Get convex shape infomation
*  @param[in] Transform&
*  @param[in] Vector3 direction
*  @param[out] float minProj
*  @param[out] float maxProj
*  @param[out] Vector3 witnesPtMax
*  @param[out] Vector3 witnesPtMin
*  @return 　　Vector3
*****************************************************************************/
void ConvexShape::Project(const AffineTransform& transform, const Vector3& direction, float& minProj, float& maxProj, Vector3& witnesPtMin, Vector3& witnesPtMax) const
{
	Vector3 localAxis = transform.GetBasis() * direction;
	Vector3 vertex1 = transform * localAxis;
	Vector3 vertex2 = transform * -localAxis;

	minProj = Dot(vertex1, direction);
	maxProj = Dot(vertex2, direction);
	witnesPtMax = vertex2;
	witnesPtMin = vertex1;

	if (minProj > maxProj)
	{
		float temp = minProj;
		minProj = maxProj;
		maxProj = temp;
		witnesPtMax = vertex1;
		witnesPtMin = vertex2;
	}
}

Vector3  ConvexShape::LocalGetSupportVertexWithoutMarginNonVirtual(const Vector3& localDir) const
{
	switch ((BroadPhaseNativeTypes)_shapeType)
	{
		case BroadPhaseNativeTypes::Sphere_Shape:
		{
			return Vector3(0, 0, 0);
		}
		case BroadPhaseNativeTypes::Box_Shape:
		{
			BoxShape* convexShape = (BoxShape*)this;
			const Vector3& halfExtents = convexShape->GetImplicitShapeDimensions();
			return 
				Vector3(Fsels(localDir.GetX(), halfExtents.GetX(), -halfExtents.GetX()),
				Fsels(localDir.GetY(), halfExtents.GetY(), -halfExtents.GetY()),
				Fsels(localDir.GetZ(), halfExtents.GetZ(), -halfExtents.GetZ()));
		}
		case BroadPhaseNativeTypes::Triangle_Shape:
		{
			TriangleShape* triangleShape = (TriangleShape*)this;
			Vector3 dir(localDir);
			Vector3* vertices = &triangleShape->Vertices1[0];
			Vector3 dots      = Vector3(Dot(dir, vertices[0]), Dot(dir, vertices[1]), Dot(dir, vertices[2]));
			Vector3 sup       = vertices[(int)MaxAxis3(dots)];
			return sup;
		}
		case BroadPhaseNativeTypes::Cylinder_Shape:
		{
			CylinderShape* cylinderShape = (CylinderShape*)this;
			Vector3 halfExtents = cylinderShape->GetImplicitShapeDimensions();
			float v[3];
			v[0] = localDir.GetX(); v[1] = localDir.GetY(); v[2] = localDir.GetZ();

			int cylinderUpAxis = cylinderShape->GetUpAxis();
			int XX(1), YY(0), ZZ(2);

			switch (cylinderUpAxis)
			{
				case 0: { XX = 1; YY = 0; ZZ = 2; break; }
				case 1: { XX = 0; YY = 1; ZZ = 2; break; }
				case 2: { XX = 0; YY = 2; ZZ = 1; break; }
				default:{ assert(0); break; }
			}

			Scalar  radius = halfExtents.GetElement(XX);
			Scalar  halfHeight = halfExtents.GetElement(cylinderUpAxis);
			float temp[3];
			Scalar  d;

			Scalar s = Sqrt(v[XX] * v[XX] + v[ZZ] * v[ZZ]);
			if (s != 0.0f)
			{
				d = radius / s;
				temp[XX] = v[XX] * d;
				temp[YY] = v[YY] < 0.0f ? -halfHeight : halfHeight;
				temp[ZZ] = v[ZZ] * d;
				return Vector3(temp[0], temp[1], temp[2]);
			}
			else
			{
				d = radius;
				temp[XX] = v[XX];
				temp[YY] = v[YY] < 0.0f ? -halfHeight : halfHeight;
				temp[ZZ] = 0.0f;
				return Vector3(temp[0], temp[1], temp[2]);
			}
		}
		case BroadPhaseNativeTypes::Capsule_Shape:
		{
			Vector3 vec0(localDir);

			CapsuleShape* capsuleShape = (CapsuleShape*)this;
			Scalar halfHeight          = capsuleShape->GetHalfHeight();
			int capsuleUpAxis          = capsuleShape->GetUpAxis();

			Vector3 supVec(0, 0, 0);
			Scalar maxDot(-FLT_MAX);
			Vector3 vec = vec0;
			Scalar lengthSqr = NormSquared(vec);
			if (lengthSqr < FLT_EPSILON * FLT_EPSILON) { vec = Vector3(1, 0, 0); }
			else
			{
				Scalar rlen = 1.0f / Sqrt(lengthSqr);
				vec *= rlen;
			}

			Vector3 vertex;
			Scalar newDot;
			{
				Vector3 position(0, 0, 0);
				position.SetElement(capsuleUpAxis, halfHeight);
				vertex = position;
				newDot = Dot(vec, vertex);
				if ((float)newDot > maxDot)
				{
					maxDot = newDot;
					supVec = vertex;
				}
			}
			{
				Vector3 position(0, 0, 0);
				position.SetElement(capsuleUpAxis, -halfHeight);
				vertex = position;
				newDot = Dot(vec, vertex);
				if ((float)newDot > maxDot)
				{
					maxDot = newDot;
					supVec = vertex;
				}
			}
			return supVec;
		}
		case BroadPhaseNativeTypes::Convex_Point_Cloud_Shape:
		{
			ConvexPointCloudShape* convexPointCloudShape = (ConvexPointCloudShape*)this;
			Vector3* points = convexPointCloudShape->GetUnscaledPoints();
			int numPoints   = convexPointCloudShape->GetPointsCount();
			return ConvexHullSupport(localDir, points, numPoints, convexPointCloudShape->GetLocalScalingNV());
		}
		case BroadPhaseNativeTypes::Convex_Hull_Shape:
		{
			ConvexHullShape* convexHullShape = (ConvexHullShape*)this;
			Vector3* points = convexHullShape->GetUnscaledPoints();
			int numPoints = convexHullShape->GetPointCount();
			return ConvexHullSupport(localDir, points, numPoints, convexHullShape->GetLocalScalingNV());
		}
		default:
		{
			return this->LocalGetSupportingVertexWithoutMargin(localDir);
		}
	}
}
Vector3  ConvexShape::LocalGetSupportVertexNonVirtual(const gm::Vector3& vector) const
{
	Vector3 localDirNorm = vector;
	if (NormSquared(localDirNorm) < FLT_EPSILON * FLT_EPSILON)
	{
		localDirNorm = Vector3(-1.0f, -1.0f, -1.0f);
	}
	localDirNorm = Normalize(localDirNorm);
	return LocalGetSupportVertexWithoutMarginNonVirtual(localDirNorm) + GetMarginNonVirtual() * localDirNorm;
}

float ConvexShape::GetMarginNonVirtual() const
{
	switch ((BroadPhaseNativeTypes)_shapeType)
	{
		case BroadPhaseNativeTypes::Sphere_Shape:
		{
			SphereShape* sphereShape = (SphereShape*)this;
			return sphereShape->GetRadius();
		}
		case BroadPhaseNativeTypes::Box_Shape:
		{
			BoxShape* convexShape = (BoxShape*)this;
			return convexShape->GetMarginNV();
		}
		case BroadPhaseNativeTypes::Triangle_Shape:
		{
			TriangleShape* triangleShape = (TriangleShape*)this;
			return triangleShape->GetMarginNV();
		}
		case BroadPhaseNativeTypes::Cylinder_Shape:
		{
			CylinderShape* cylinderShape = (CylinderShape*)this;
			return cylinderShape->GetMarginNV();
		}
		case BroadPhaseNativeTypes::Cone_Shape:
		{
			ConeShape* coneShape = (ConeShape*)this;
			return coneShape->GetMarginNV();
		}
		case BroadPhaseNativeTypes::Capsule_Shape:
		{
			CapsuleShape* capsuleShape = (CapsuleShape*)this;
			return capsuleShape->GetMarginNV();
		}
		case BroadPhaseNativeTypes::Convex_Point_Cloud_Shape:
		case BroadPhaseNativeTypes::Convex_Hull_Shape:
		{
			PolyhedralConvexShape* convexHullShape = (PolyhedralConvexShape*)this;
			return convexHullShape->GetMarginNV();
		}
		default:
		{
			return this->GetMargin();
		}
	}

	assert(0);
	return 0.0f;
}
void ConvexShape::GetAABBNonVirtual(const AffineTransform& transform, Vector3& aabbMin, Vector3& aabbMax) const
{
	switch ((BroadPhaseNativeTypes)_shapeType)
	{
		case BroadPhaseNativeTypes::Sphere_Shape:
		{
			SphereShape* sphereShape = (SphereShape*)this;
			Scalar radius = sphereShape->GetImplicitShapeDimensions().GetX();
			Scalar margin = radius + sphereShape->GetMarginNonVirtual();
			const Vector3& center = transform.GetTranslation();
			Vector3 extent(margin, margin, margin);
			aabbMin = center - extent;
			aabbMax = center + extent;
			break;
		}
		case BroadPhaseNativeTypes::Cylinder_Shape:
		case BroadPhaseNativeTypes::Box_Shape:
		{
			BoxShape* convexShape = (BoxShape*)this;
			Scalar margin         = convexShape->GetMarginNonVirtual();
			Vector3 halfExtents   = convexShape->GetImplicitShapeDimensions();
			halfExtents += Vector3(margin, margin, margin);
			Matrix3 abs_b = Absolute(transform.GetBasis());
			Vector3 center = transform.GetTranslation();
			Vector3 extent = Vector3(Dot(halfExtents, abs_b.GetX()), Dot(halfExtents, abs_b.GetY()), Dot(halfExtents, abs_b.GetZ()));
			aabbMin = center - extent;
			aabbMax = center + extent;
			break;
		}
		case BroadPhaseNativeTypes::Triangle_Shape:
		{
			TriangleShape* triangleShape = (TriangleShape*)this;
			Scalar margin = triangleShape->GetMarginNonVirtual();
			for (int i = 0; i < 3; ++i)
			{
				Vector3 vec(0.0f, 0.0f, 0.0f);
				vec.SetElement(i, 1.0f);
				Vector3 sv = LocalGetSupportVertexWithoutMarginNonVirtual(transform.GetBasis() * vec);

				Vector3 temp = transform * sv;
				aabbMax.SetElement(i, temp.GetElement(i) + margin);
				vec.SetElement(i, -1.0f);
				temp = transform * LocalGetSupportVertexWithoutMarginNonVirtual(transform.GetBasis() * vec);
				aabbMin.SetElement(i, temp.GetElement(i) - margin);
			}
			break;
		}
		case BroadPhaseNativeTypes::Capsule_Shape:
		{
			CapsuleShape* capsuleShape = (CapsuleShape*)this;
			Vector3 halfExtents(capsuleShape->GetRadius(), capsuleShape->GetRadius(), capsuleShape->GetRadius());
			int upAxis = capsuleShape->GetUpAxis();
			halfExtents.SetElement(upAxis, capsuleShape->GetRadius() + capsuleShape->GetHalfHeight());
			Matrix3 abs_b = Absolute(transform.GetBasis());
			Vector3 center = transform.GetTranslation();
			Vector3 extent = Vector3(Dot(halfExtents, abs_b.GetX()), Dot(halfExtents, abs_b.GetY()), Dot(halfExtents, abs_b.GetZ()));
			aabbMin = center - extent;
			aabbMax = center + extent;
			break;
		}
		case BroadPhaseNativeTypes::Convex_Point_Cloud_Shape:
		case BroadPhaseNativeTypes::Convex_Hull_Shape:
		{
			PolyhedralConvexAABBCachingShape* convexHullShape = (PolyhedralConvexAABBCachingShape*)this;
			Scalar margin = convexHullShape->GetMarginNonVirtual();
			convexHullShape->GetNonVirtualAABB(transform, aabbMin, aabbMax, margin);
			break;
		}
		default:
		{
			this->GetAABB(transform, aabbMin, aabbMax);
			break;
		}

	}
	assert(0);
}
/****************************************************************************
*						ConvexHullSupport
*************************************************************************//**
*  @fn        static Vector3 ConvexHullSupport(const Vector3& localDirectionOrg, const Vector3* points, int pointCount, const Vector3& localScaling)
*  @brief     Convex hull support
*  @param[in] Vector3 localDirectionOrg,
*  @param[in] Vector3 points
*  @param[in] int pointCount
*  @param[in] Vector3 localScaling
*  @return 　　Vector3
*****************************************************************************/
static Vector3 ConvexHullSupport(const Vector3& localDirectionOrg, const Vector3* points, int pointCount, const Vector3& localScaling)
{
	Vector3 vector = localDirectionOrg * localScaling;

	Scalar maxDot;
	long ptIndex = MaxDot(vector, points, pointCount, maxDot);
	assert(ptIndex >= 0);
	if (ptIndex < 0) { ptIndex = 0; }
	Vector3 supVec = points[ptIndex] * localScaling;
	return supVec;
	
}
#pragma endregion ConvexShape

#pragma region ConvexInternalShape
ConvexInternalShape::ConvexInternalShape()
{
	_localScaling    = Vector3(1.0f, 1.0f, 1.0f);
	_collisionMargin = CONVEX_DISTANCE_MARGIN;
}
/****************************************************************************
*						LocalGetSupportingVertex
*************************************************************************//**
*  @fn        Vector3 BoxShape::LocalGetSupportingVertex(const Vector3& vector) const
*  @brief     Local get supporting vertex
*  @param[in] Vector3& vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 ConvexInternalShape::LocalGetSupportingVertex(const Vector3& vector) const
{
	Vector3 supVertex = LocalGetSupportingVertexWithoutMargin(vector);
	if (GetMargin() != 0.0f)
	{
		Vector3 vecnorm = vector;
		if (NormSquared(vecnorm) < FLT_EPSILON * FLT_EPSILON)
		{
			vecnorm = Vector3(-1.0f, -1.0f, -1.0f);
		}
		vecnorm    = Normalize(vecnorm);
		supVertex += GetMargin() * vecnorm;
	}
	return supVertex;
}
/****************************************************************************
*						GetAABBSlow
*************************************************************************//**
*  @fn        void ConvexInternalShape::GetAABBSlow(const AffineTransform& transform, Vector3& aabbMin, Vector3& aabbMax) const
*  @brief     Get aabb
*  @param[in] AffineTransform& transform
*  @param[out] Vector3& aabbMin
*  @param[out] Vector3& aabbMax
*  @return 　　void
*****************************************************************************/
void ConvexInternalShape::GetAABBSlow(const AffineTransform& transform, Vector3& aabbMin, Vector3& aabbMax) const
{
	float margin = GetMargin();
	for (int i = 0; i < 3; ++i)
	{
		Vector3 vec = Vector3(0, 0, 0);
		vec.SetElement(i, 1.0f);
		Vector3 sv = LocalGetSupportingVertex(transform.GetBasis() * vec);
		Vector3 temp = transform * sv;
		aabbMax.SetElement(i, temp.GetElement(i) + margin);
		vec.SetElement(i, -1.0f);
		temp = transform * LocalGetSupportingVertex(transform.GetBasis() * vec);
		aabbMin.SetElement(i, temp.GetElement(i) - margin);
	}
}
/****************************************************************************
*						SetLocalScaling
*************************************************************************//**
*  @fn        void BoxShape::SetLocalScaling(const Vector3& scaling)
*  @brief     Set local scaling
*  @param[in] Vector3 scaling
*  @return 　　void
*****************************************************************************/
void ConvexInternalShape::SetLocalScaling(const Vector3& scaling)
{
	_localScaling = Abs(scaling);
}


#pragma endregion ConvexInternalShape

#pragma region ConvexInternalAABBChachingShape
ConvexInternalAabbCachingShape::ConvexInternalAabbCachingShape() : ConvexInternalShape()
{
	_localAabbMin = Vector3(1, 1, 1);
	_localAabbMax = Vector3(-1, -1, -1);
	_isLocalAabbValid = false;
}
void ConvexInternalAabbCachingShape::ReCalculateLocalAABB()
{
	_isLocalAabbValid = true;

	static const Vector3 _directions[] =
	{
		Vector3(1., 0., 0.),
		Vector3(0., 1., 0.),
		Vector3(0., 0., 1.),
		Vector3(-1., 0., 0.),
		Vector3(0., -1., 0.),
		Vector3(0., 0., -1.) };

	Vector3 _supporting[] =
	{
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.) };

	BatchedUnitVectorGetSupportingVertexWithoutMargin(_directions, _supporting, 6);

	for (int i = 0; i < 3; ++i)
	{
		_localAabbMax.SetElement(i, _supporting[i].GetElement(i) + _collisionMargin);
		_localAabbMin.SetElement(i, _supporting[i + 3].GetElement(i) - _collisionMargin);
	}
}
void ConvexInternalAabbCachingShape::GetAABB(const AffineTransform& t, Vector3& aabbMin, Vector3& aabbMax) const
{
	GetNonVirtualAABB(t, aabbMin, aabbMax, GetMargin());
}
void ConvexInternalAabbCachingShape::SetLocalScaling(const Vector3& scaling)
{
	ConvexInternalShape::SetLocalScaling(scaling);
	ReCalculateLocalAABB();
}
#pragma endregion ConvexInternalAABBCachingShape