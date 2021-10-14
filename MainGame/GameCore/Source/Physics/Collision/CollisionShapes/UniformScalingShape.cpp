//////////////////////////////////////////////////////////////////////////////////
///             @file   UniformScalingShape.hpp
///             @brief  UniformScalingShape
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
#include "GameCore/Include/Physics/Collision/CollisionShapes/UniformScalingShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
UniformScalingShape::UniformScalingShape(ConvexShape* convexChildShape, float uniformScalingFactor) : ConvexShape()
{
	_childConvexShape     = convexChildShape;
	_uniformScalingFactor = uniformScalingFactor;
	_shapeType            = (int)BroadPhaseNativeTypes::Uniform_Scaling_Shape;
}
gm::Vector3 UniformScalingShape::LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const 
{
	Vector3 tmpVertex;
	tmpVertex = _childConvexShape->LocalGetSupportingVertexWithoutMargin(vector);
	return tmpVertex * _uniformScalingFactor;
}

gm::Vector3 UniformScalingShape::LocalGetSupportingVertex(const gm::Vector3& vector) const 
{
	Vector3 tmpVertex;
	tmpVertex = _childConvexShape->LocalGetSupportingVertex(vector);
	return tmpVertex * _uniformScalingFactor;
}
void UniformScalingShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const 
{
	_childConvexShape->BatchedUnitVectorGetSupportingVertexWithoutMargin(vectors, supportVerticesOut, numVectors);
	int i;
	for (i = 0; i < numVectors; i++)
	{
		supportVerticesOut[i] = supportVerticesOut[i] * _uniformScalingFactor;
	}
}

void UniformScalingShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
{
	Vector3 tmpInertia;
	_childConvexShape->CalculateLocalInertia(mass, tmpInertia);
	inertia = tmpInertia * _uniformScalingFactor;
}

void UniformScalingShape::GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
{
	GetAABBSlow(transform, aabbMin, aabbMax);
}

void  UniformScalingShape::GetAABBSlow(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
{
	Vector3 _directions[] =
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

	for (int i = 0; i < 6; i++)
	{
		_directions[i] = transform.GetBasis() * _directions[i];
	}

	BatchedUnitVectorGetSupportingVertexWithoutMargin(_directions, _supporting, 6);

	Vector3 aabbMin1(0, 0, 0), aabbMax1(0, 0, 0);

	for (int i = 0; i < 3; ++i)
	{
		aabbMax1.SetElement(i, (transform.GetBasis() * _supporting[i    ]).GetElement(i));
		aabbMin1.SetElement(i, (transform.GetBasis() * _supporting[i + 3]).GetElement(i));
	}
	Vector3 marginVec(GetMargin(), GetMargin(), GetMargin());
	aabbMin = aabbMin1 - marginVec;
	aabbMax = aabbMax1 + marginVec;

}

const gm::Vector3& UniformScalingShape::GetLocalScaling() const
{
	return _childConvexShape->GetLocalScaling();
}

float UniformScalingShape::GetMargin()const
{
	return _childConvexShape->GetMargin() * _uniformScalingFactor;
}

int  UniformScalingShape::GetPreferredPenetrationDirectionsCount() const 
{
	return _childConvexShape->GetPreferredPenetrationDirectionsCount();
}

void UniformScalingShape::GetPreferredPenetrationDirection(int index, gm::Vector3& penetrationVector) const
{
	_childConvexShape->GetPreferredPenetrationDirection(index, penetrationVector);
}
void UniformScalingShape::SetLocalScaling(const gm::Vector3& scaling)
{
	_childConvexShape->SetLocalScaling(scaling);
}
void UniformScalingShape::SetMargin(float margin)
{
	_childConvexShape->SetMargin(margin);
}