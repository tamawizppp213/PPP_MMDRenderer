//////////////////////////////////////////////////////////////////////////////////
///             @file   SphereShape.hpp
///             @brief  Sphere shape
///             @author Partially edit by Toide
///             @date   2021_09_19
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
#include "GameCore/Include/Physics/Collision/CollisionShapes/SphereShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*						LocalGetSupportingVertex
*************************************************************************//**
*  @fn        Vector3 SphereShape::LocalGetSupportingVertex(const Vector3& vector) const
*  @brief     Set local orientation X axis;
*  @param[in] Vector3 vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 SphereShape::LocalGetSupportingVertex(const Vector3& vector) const
{
	Vector3 supVertex;
	supVertex = LocalGetSupportingVertexWithoutMargin(vector);

	Vector3 vecnorm = vector;
	if (NormSquared(vecnorm) < (FLT_EPSILON * FLT_EPSILON))
	{
		vecnorm = Vector3(-1.0f, -1.0f, -1.0f);
	}
	vecnorm    = Normalize(vecnorm);
	supVertex += GetMargin() * vecnorm;
	return supVertex;
}
/****************************************************************************
*						LocalGetSupportingVertex
*************************************************************************//**
*  @fn        gm::Vector3 SphereShape::LocalGetSupportingVertex(const gm::Vector3& vector) const
*  @brief     Set local orientation X axis;
*  @param[in] Vector3& vector
*  @return 　　Vector3
*****************************************************************************/
Vector3 SphereShape::LocalGetSupportingVertexWithoutMargin(const Vector3& vector) const
{
	(void)vector;
	return Vector3(0.0f, 0.0f, 0.0f);
}
/****************************************************************************
*						BatchedUnitVectorGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn         void SphereShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
*  @brief      BatchedUnitVectorGetSupportingVertexWithoutMargin
*  @param[in]  gm::Vector3* vectors
*  @param[out] gm::Vector3* supportVerticesOut
*  @param[in]  int numVectors
*  @return 　　 void
*****************************************************************************/
void SphereShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const Vector3* vectors, Vector3* supportVerticesOut, int numVectors) const
{
	for (int i = 0; i < numVectors; i++)
	{
		supportVerticesOut[i] = Vector3(0.0f, 0.0f, 0.0f);
	}
}
/****************************************************************************
*						CalculateLocalInertia
*************************************************************************//**
*  @fn        void SphereShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
*  @brief     Output local inertia 
*  @param[in] float mass
*  @param[out] Vector3& inertia
*  @return 　　 void
*****************************************************************************/
void SphereShape::CalculateLocalInertia(float mass, Vector3& inertia) const
{
	float elem = 0.4f * mass * GetMargin() * GetMargin();
	inertia    = Vector3(elem, elem, elem);
}
/****************************************************************************
*						GetAABB
*************************************************************************//**
*  @fn         void SphereShape::GetAABB(const gm::Transform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
*  @brief      GetAABB
*  @param[in]  Transform& transform
*  @param[out] Vector3& aabbMin
*  @param[out] Vector3& aabbMax
*  @return 　　 void
*****************************************************************************/
void SphereShape::GetAABB(const AffineTransform& transform, Vector3& aabbMin, Vector3& aabbMax) const
{
	const Vector3& center = transform.GetTranslation();
	Vector3 extent(GetMargin(), GetMargin(), GetMargin());
	aabbMin = center - extent;
	aabbMax = center + extent;
}
#pragma endregion Public Function
