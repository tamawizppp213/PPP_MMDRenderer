//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexPointCloudShape.hpp
///             @brief  ConvexPointCloudShape
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
#include "GameCore/Include/Physics/Collision/CollisionShapes/ConvexPointCloudShape.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
ConvexPointCloudShape::ConvexPointCloudShape()
{
	_localScaling   = Vector3(1.0f, 1.0f, 1.0f);
	_shapeType      = (int)BroadPhaseNativeTypes::Convex_Point_Cloud_Shape;
	_unScaledPoints = 0;
	_numPoints      = 0;
}
ConvexPointCloudShape::ConvexPointCloudShape(Vector3* points, int numPoints, const Vector3& localScaling, bool computeAABB)
{
	_localScaling   = localScaling;
	_shapeType      = (int)BroadPhaseNativeTypes::Convex_Point_Cloud_Shape;
	_unScaledPoints = points;
	_numPoints      = numPoints;

	if (computeAABB)
		ReCalculateLocalAABB();
}
#pragma region Public Function
Vector3 ConvexPointCloudShape::LocalGetSupportingVertex(const Vector3& vector) const
{
	Vector3 supVertex = LocalGetSupportingVertexWithoutMargin(vector);

	if (GetMargin() != Scalar(0.0f))
	{
		Vector3 vecnorm = vector;
		if (NormSquared(vecnorm) < (FLT_EPSILON * FLT_EPSILON))
		{
			vecnorm = Vector3(-1.0f, -1.0f, -1.0f);
		}
		Normalize(vecnorm);
		supVertex += GetMargin() * vecnorm;
	}
	return supVertex;
}
Vector3 ConvexPointCloudShape::LocalGetSupportingVertexWithoutMargin(const Vector3& vector) const
{
	Vector3 supVec = Vector3(0.0f, 0.0f, 0.0f);
	Scalar maxDot  = -FLT_MAX;
	Vector3 vec    = vector;
	float lenSqr   = NormSquared(vec);
	if (lenSqr < 0.0001f)
	{
		vec = Vector3(1, 0, 0);
	}
	else
	{
		float rlen = 1.0f / Sqrt(lenSqr);
		vec = vec * rlen;
	}

	if (_numPoints > 0)
	{
		// Here we take advantage of dot(a*b, c) = dot( a, b*c) to do less work. Note this transformation is true mathematically, not numerically.
		//    btVector3 scaled = vec * m_localScaling;
		int index = (int)MaxDot(vec, &_unScaledPoints[0], _numPoints, maxDot);  //FIXME: may violate encapsulation of m_unscaledPoints
		return GetScaledPoints(index);
	}

	return supVec;
}
/****************************************************************************
*				BatchedUnitVectorGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        void ConvexPointCloudShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
*  @brief     GetSupportingVertexWithoutMargin
*  @param[in, out] Vector3* vectors, 
*  @param[in, out] Vector3* supportVerticesOut, 
*  @param[in] int numVectors
*  @return 　　int
*****************************************************************************/
void ConvexPointCloudShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
{
	std::vector<float> supports(numVectors, -FLT_MAX);
	for (int j = 0; j < numVectors; j++)
	{
		const Vector3& vec = vectors[j] * _localScaling;  // dot( a*c, b) = dot(a, b*c)
		Scalar maxDot;
		int index = (int)MaxDot(vec, &_unScaledPoints[0], _numPoints, maxDot);
		supports[j] = -FLT_MAX;
		if (0 <= index)
		{
			//WARNING: don't swap next lines, the w component would get overwritten!
			supportVerticesOut[j] = GetScaledPoints(index);
			supports[j]           = maxDot;
		}
	}
}
/****************************************************************************
*						SetPoints
*************************************************************************//**
*  @fn        void ConvexPointCloudShape::SetPoints(Vector3* points, int numPoints, bool computeAABB, const Vector3& localScaling)
*  @brief     Set points
*  @param[in,out] Vector3* points
*  @param[in] int numPoints
*  @param[in] bool computeAABB
*  @param[in] Vector3 localScaling
*  @return 　　void
*****************************************************************************/
void ConvexPointCloudShape::SetPoints(Vector3* points, int numPoints, bool computeAABB, const Vector3& localScaling)
{
	_unScaledPoints = points;
	_numPoints      = numPoints;
	_localScaling   = localScaling;

	if (computeAABB)
		ReCalculateLocalAABB();
}
/****************************************************************************
*						GetVerticesCount
*************************************************************************//**
*  @fn        int ConvexPointCloudShape::GetVerticesCount() const
*  @brief     GetE Vertex Count
*  @param[in] void
*  @return 　　int
*****************************************************************************/
int ConvexPointCloudShape::GetVerticesCount() const
{
	return _numPoints;
}
/****************************************************************************
*						GetPlaneCount
*************************************************************************//**
*  @fn        int ConvexPointCloudShape::GetEdgesCount() const
*  @brief     GetEdgeCount (This function is unused)
*  @param[in] void
*  @return 　　int
*****************************************************************************/
int ConvexPointCloudShape::GetEdgesCount() const
{
	return 0;
}
/****************************************************************************
*						GetPlaneCount
*************************************************************************//**
*  @fn        void ConvexPointCloudShape::GetEdge(int index, Vector3& pa, Vector3& pb) const
*  @brief     GetEdge (This function is unused)
*  @param[in] int index
*  @param[in] Vector3 pa
*  @param[in] Vector3 pb
*  @return 　　void
*****************************************************************************/
void ConvexPointCloudShape::GetEdge(int index, Vector3& pa, Vector3& pb) const
{
	assert(0);
}
/****************************************************************************
*						GetVertex
*************************************************************************//**
*  @fn        void ConvexPointCloudShape::GetVertex(int index, gm::Vector3& vertex) const
*  @brief     Get vertex
*  @param[in] int index
*  @param[in] Vector3 vector
*  @return 　　void
*****************************************************************************/
void ConvexPointCloudShape::GetVertex(int index, gm::Vector3& vertex) const
{
	vertex = _unScaledPoints[index] * _localScaling;
}
/****************************************************************************
*						GetPlaneCount
*************************************************************************//**
*  @fn        int  ConvexPointCloudShape::GetPlanesCount() const
*  @brief     GetPlaneCount (This function is unused)
*  @param[in] void
*  @return 　　int
*****************************************************************************/
int  ConvexPointCloudShape::GetPlanesCount() const
{
	return 0;
}
/****************************************************************************
*						GetPlane
*************************************************************************//**
*  @fn        void ConvexPointCloudShape::GetPlane(Vector3& planeNormal, Vector3& planeSupport, int index) const
*  @brief     GetPlane (This function is unused)
*  @param[in] Vector3 planeNormal
*  @param[in] Vector3 planeSupport
*  @param[in] int index
*  @return 　　void
*****************************************************************************/
void ConvexPointCloudShape::GetPlane(Vector3& planeNormal, Vector3& planeSupport, int index) const
{
	assert(0);
}
/****************************************************************************
*						IsInside
*************************************************************************//**
*  @fn        bool ConvexPointCloudShape::IsInside(const Vector3& pt, float tolerance) const
*  @brief     Set local scaling (This function is unused)
*  @param[in] Vector3 pt
*  @param[in] float tolerance
*  @return 　　bool
*****************************************************************************/
bool ConvexPointCloudShape::IsInside(const Vector3& pt, float tolerance) const
{
	assert(0); return false;
}
/****************************************************************************
*						SetLocalScaling
*************************************************************************//**
*  @fn        void ConvexPointCloudShape::SetLocalScaling(const Vector3& scaling)
*  @brief     Set local scaling
*  @param[in] Vector3 scaling
*  @return 　　void
*****************************************************************************/
void ConvexPointCloudShape::SetLocalScaling(const Vector3& scaling)
{
	_localScaling = scaling;
	ReCalculateLocalAABB();
}
#pragma endregion Public Function