//////////////////////////////////////////////////////////////////////////////////
///             @file   ConeShape.cpp
///             @brief  Cone Shape
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
#include "GameCore/Include/Physics/Collision/CollisionShapes/ConeShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
ConeShape::ConeShape(float radius, float height)
{
	_shapeType = (int)BroadPhaseNativeTypes::Cone_Shape;
	_radius    = radius;
	_height    = height;
	SetConeUpIndex(1);
	_sinAngle = (_radius / Sqrt(_radius * _radius + _height * _height));
}
/****************************************************************************
*						LocalGetSupportingVertex
*************************************************************************//**
*  @fn        Vector3 ConeShape::LocalGetSupportingVertex(const Vector3& vector) const
*  @brief     Local get supporting vertex
*  @param[in] Vector3& vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 ConeShape::LocalGetSupportingVertex(const Vector3& vector) const
{
	Vector3 supVertex = ConeLocalSupport(vector);
	if (GetMargin() != 0.0f)
	{
		Vector3 vecnorm = vector;
		if (NormSquared(vecnorm) < (FLT_EPSILON * FLT_EPSILON))
		{
			vecnorm = Vector3(-1.0f,-1.0f, -1.0f);
		}
		Vector3 norm = Normalize(vecnorm);
		supVertex += GetMargin() * norm;
	}
	return supVertex;
}
/****************************************************************************
*						LocalGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        Vector3 BoxShape::LocalGetSupportingVertexWithoutMargin(const Vector3& vector)
*  @brief     Get local supporting vertex
*  @param[in] Vector3 vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 ConeShape::LocalGetSupportingVertexWithoutMargin(const Vector3& vector) const
{
	return ConeLocalSupport(vector);
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
void ConeShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const Vector3* vectors, Vector3* supportVerticesOut, int numVectors) const
{
	for (int i = 0; i < numVectors; i++)
	{
		const Vector3& vec = vectors[i];
		supportVerticesOut[i] = ConeLocalSupport(vec);
	}
}
/****************************************************************************
*						CalculateLocalInertia
*************************************************************************//**
*  @fn        void ConeShape::CalculateLocalInertia(float mass, Vector3& inertia) const
*  @brief     Calculate inertia
*  @param[in] float mass
*  @param[out] Vector3 inertia
*  @return 　　void
*****************************************************************************/
void ConeShape::CalculateLocalInertia(float mass, Vector3& inertia) const
{
	AffineTransform identity;

	Vector3 aabbMin, aabbMax;
	GetAABB(identity, aabbMin, aabbMax);

	Vector3 halfExtents = (aabbMax - aabbMin) * 0.5f;

	float margin = GetMargin();

	float lx = (2.0f * (halfExtents.GetX() + margin));
	float ly = 2.0f * (halfExtents.GetY() + margin);
	float lz = 2.0f * (halfExtents.GetZ() + margin);
	const float x2 = lx * lx;
	const float y2 = ly * ly;
	const float z2 = lz * lz;
	const float scaledmass = mass * 0.08333333f;

	inertia = scaledmass * (Vector3(y2 + z2, x2 + z2, x2 + y2));
}
/****************************************************************************
*						SetConeUpIndex
*************************************************************************//**
*  @fn        void ConeShape::SetConeUpIndex(int upIndex)
*  @brief     Set cone up index
*  @param[in] int upIndex
*  @return 　　void
*****************************************************************************/
void ConeShape::SetConeUpIndex(int upIndex)
{
	switch (upIndex)
	{
		case 0:
		{
			_coneIndices[0] = 1;
			_coneIndices[1] = 0;
			_coneIndices[2] = 2;
			break;
		}
		case 1:
		{
			_coneIndices[0] = 0;
			_coneIndices[1] = 1;
			_coneIndices[2] = 2;
			break;
		}
		case 2:
		{
			_coneIndices[0] = 0;
			_coneIndices[1] = 2;
			_coneIndices[2] = 1;
			break;
		}
		default:
			assert(0);
	}

	_implicitShapeDimensions.SetElement(_coneIndices[0], _radius);
	_implicitShapeDimensions.SetElement(_coneIndices[1], _height);
	_implicitShapeDimensions.SetElement(_coneIndices[2], _radius);
}

ConeShapeX::ConeShapeX(float radius, float height) : ConeShape(radius, height)
{
	SetConeUpIndex(0);
}

ConeShapeZ::ConeShapeZ(float radius, float height) : ConeShape(radius, height)
{
	SetConeUpIndex(2);
}

void ConeShape::SetLocalScaling(const gm::Vector3& scaling)
{
	int axis  = _coneIndices[1];
	int r1    = _coneIndices[0];
	int r2    = _coneIndices[2];
	_height  *= scaling.GetElement(axis) / _localScaling.GetElement(axis);
	_radius  *= ((float)scaling.GetElement(r1) / _localScaling.GetElement(r1) + scaling.GetElement(r2) / _localScaling.GetElement(r2)) / 2.0f;
	_sinAngle = (_radius / Sqrt(_radius * _radius + _height * _height));
	ConvexInternalShape::SetLocalScaling(scaling);
}
#pragma endregion Public Function

#pragma region Private Function
Vector3 ConeShape::ConeLocalSupport(const Vector3 vertex) const
{
	float halfHeight = _height * 0.5f;

	if ((float)vertex.GetElement((Vector3Element)_coneIndices[1]) > Norm(vertex) * _sinAngle)
	{
		Vector3 temp;
		temp.SetElement((Vector3Element)_coneIndices[0],  0.0f);
		temp.SetElement((Vector3Element)_coneIndices[1], halfHeight);
		temp.SetElement((Vector3Element)_coneIndices[2], 0.0f);
		return temp;
	}
	else
	{
		float ci0 = vertex.GetElement(_coneIndices[0]);
		float ci1 = vertex.GetElement(_coneIndices[1]);
		float ci2 = vertex.GetElement(_coneIndices[2]);
		float s = Sqrt(ci0 * ci0 + ci2 * ci2);

		if (s > FLT_EPSILON)
		{
			float d = _radius / s;
			Vector3 temp;
			temp.SetElement(_coneIndices[0],  ci0 * d);
			temp.SetElement(_coneIndices[1], -halfHeight);
			temp.SetElement(_coneIndices[2], ci2 * d);
			return temp;
		}
		else
		{
			Vector3 temp;
			temp.SetElement(_coneIndices[0], 0.0f);
			temp.SetElement(_coneIndices[1], -halfHeight);
			temp.SetElement(_coneIndices[2], 0.0f);
			return temp;
		}
	}
}
#pragma endregion Private Function