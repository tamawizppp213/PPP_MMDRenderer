//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexPointCloudShape.hpp
///             @brief  ConvexPointCloudShape
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
#pragma once
#ifndef CONVEX_POINT_CLOUD_SHAPE_HPP
#define CONVEX_POINT_CLOUD_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConvexPolyhedralShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         ConvexPointCloudSha@e
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			ConvexPointCloudShape
*************************************************************************//**
*  @class     ConvexPointCloudShape
*  @brief     ConvexPointCloud
*****************************************************************************/
__declspec(align(16))
class ConvexPointCloudShape : public PolyhedralConvexAABBCachingShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual gm::Vector3 LocalGetSupportingVertex(const gm::Vector3& vector) const override;
	virtual gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const override;
	virtual void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetPoints(gm::Vector3* points, int numPoints, bool computeAABB = true, const gm::Vector3& localScaling = gm::Vector3(1.0f, 1.0f, 1.0f));

	virtual const char* GetName() const { return "ConvexPointCloud"; }
	virtual int GetVerticesCount()                                      const override;
	virtual int GetEdgesCount   ()                                      const override;
	virtual void GetEdge  (int index, gm::Vector3& pa, gm::Vector3& pb) const override;
	virtual void GetVertex(int index, gm::Vector3& vertex)              const override;
	virtual int  GetPlanesCount()                                        const override;
	virtual void GetPlane(gm::Vector3& planeNormal, gm::Vector3& planeSupport, int index) const override;
	virtual bool IsInside(const gm::Vector3& pt, float tolerance) const override;
	virtual void SetLocalScaling(const gm::Vector3& scaling) override;

	INLINE        gm::Vector3* GetUnscaledPoints()              { return _unScaledPoints; }
	INLINE  const gm::Vector3* GetUnscaledPoints()        const { return _unScaledPoints; }
	INLINE int                 GetPointsCount   ()        const { return _numPoints; }
	INLINE gm::Vector3         GetScaledPoints(int index) const { return _unScaledPoints[index] * _localScaling; }
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	ConvexPointCloudShape();
	ConvexPointCloudShape(gm::Vector3* points, int numPoints, const gm::Vector3& localScaling, bool computeAABB = true);

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::Vector3* _unScaledPoints;
	int _numPoints;
};
#endif