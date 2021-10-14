//////////////////////////////////////////////////////////////////////////////////
///             @file   MinkowskiSumShape.hpp
///             @brief  MinkowskiSumShape.cpp
///             @author Partially edit by Toide
///             @date   2021_09_26
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
#include "GameCore/Include/Physics/Collision/CollisionShapes/MinkowskiSumShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
MinkowskiSumShape::MinkowskiSumShape(const ConvexShape* shapeA, const ConvexShape* shapeB) : ConvexInternalShape()
{
	_shapeA     = shapeA;
	_shapeB     = shapeB;
	_shapeType  = (int)BroadPhaseNativeTypes::Minkowski_Difference_Shape;
	_transformA = AffineTransform();
	_transformB = AffineTransform();
}
Vector3 MinkowskiSumShape::LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const
{
	Vector3 supVertexA = _transformA * _shapeA->LocalGetSupportingVertexWithoutMargin(_transformA.GetBasis() * vector);
	Vector3 supVertexB = _transformB * _shapeB->LocalGetSupportingVertexWithoutMargin(_transformB.GetBasis() * -vector);
	return supVertexA - supVertexB;
}
void MinkowskiSumShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
{
	for (int i = 0; i < numVectors; i++)
	{
		supportVerticesOut[i] = LocalGetSupportingVertexWithoutMargin(vectors[i]);
	}
}
void MinkowskiSumShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
{
	(void)mass;
	//inertia of the AABB of the Minkowski sum
	AffineTransform identity;
	Vector3 aabbMin, aabbMax;
	GetAABB(identity, aabbMin, aabbMax);

	Vector3 halfExtents = (aabbMax - aabbMin) * 0.5f;

	Scalar margin = GetMargin();

	Scalar lx = Scalar(2.) * (halfExtents.GetX() + margin);
	Scalar ly = Scalar(2.) * (halfExtents.GetY() + margin);
	Scalar lz = Scalar(2.) * (halfExtents.GetZ() + margin);
	const Scalar x2 = lx * lx;
	const Scalar y2 = ly * ly;
	const Scalar z2 = lz * lz;
	const Scalar scaledmass = mass * Scalar(0.08333333);

	inertia = scaledmass * (Vector3(y2 + z2, x2 + z2, x2 + y2));
}
float MinkowskiSumShape::GetMargin() const
{
	return _shapeA->GetMargin() + _shapeB->GetMargin();
}