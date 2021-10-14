//////////////////////////////////////////////////////////////////////////////////
///             @file   StaticPlaneShape.hpp
///             @brief  StaticPlaneShape
///             @author Partially edit by Toide
///             @date   2021_09_20
///                     2021_10_06 (first checked) first checked -> no debug.
//////////////////////////////////////////////////////////////////////////////////

/*Bullet Continuous Collision Detectionand Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.*/

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionShapes/StaticPlaneShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
StaticPlaneShape::StaticPlaneShape(const Vector3& planeNormal, float planeConstant): ConcaveShape()
{
	_shapeType     = (int)BroadPhaseNativeTypes::Static_Plane_Shape;
	_planeNormal   = Normalize(planeNormal);
	_planeConstant = planeConstant;
	_localScaling  = Vector3(1.0f, 1.0f, 1.0f);

}

void StaticPlaneShape::ProcessAllTriangles(TriangleCallback* callback, const Vector3& aabbMin, const Vector3& aabbMax) const
{
	Vector3 halfExtents = (aabbMax - aabbMin) * 0.5f;
	float        radius = Norm(halfExtents);
	Vector3 center      = (aabbMax + aabbMin) * 0.5f;

	//this is where the triangles are generated, given AABB and plane equation (normal/constant)

	Vector3 tangentDir0, tangentDir1;

	//tangentDir0/tangentDir1 can be precalculated
	PlaneSpace(_planeNormal, tangentDir0, tangentDir1);

	Vector3 projectedCenter = center - (Dot(_planeNormal, center) - _planeConstant) * _planeNormal;

	Vector3 triangle[3];
	triangle[0] = projectedCenter + tangentDir0 * radius + tangentDir1 * radius;
	triangle[1] = projectedCenter + tangentDir0 * radius - tangentDir1 * radius;
	triangle[2] = projectedCenter - tangentDir0 * radius - tangentDir1 * radius;

	//callback->ProcessingTriangle(triangle, 0, 0); //(デバッグ時の色付ける用)

	triangle[0] = projectedCenter - tangentDir0 * radius - tangentDir1 * radius;
	triangle[1] = projectedCenter - tangentDir0 * radius + tangentDir1 * radius;
	triangle[2] = projectedCenter + tangentDir0 * radius + tangentDir1 * radius;

	//callback->ProcessingTriangle(triangle, 0, 1);
}

void StaticPlaneShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
{
	inertia = Vector3(0, 0, 0);
}

void StaticPlaneShape::GetAABB(const AffineTransform& transform, Vector3& aabbMin, Vector3& aabbMax) const
{
	aabbMin = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	aabbMax = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
}

const gm::Vector3& StaticPlaneShape::GetLocalScaling() const
{
	return _localScaling;
}

void StaticPlaneShape::SetLocalScaling(const gm::Vector3& scaling)
{
	_localScaling = scaling;
}


