//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexHullShape.hpp
///             @brief  ConvexHullShape
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
#ifndef CONVEX_HULL_SHAPE_HPP
#define CONVEX_HULL_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConvexPolyhedralShape.hpp"
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/BroadPhaseProxy.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			ConvexHullShape
*************************************************************************//**
*  @class     ConvexHullShape
*  @brief     ConvexHullShape implements an implicit convex hull pf an array of vertices\
*             provides a general and fast collision detector for convex shapes based on GJK and EPA using localGetSupportingVertex  
*****************************************************************************/
__declspec(align(16))
class ConvexHullShape : public PolyhedralConvexAABBCachingShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void AddPoint(const gm::Vector3& point, bool reCalculateLocalAABB = true);
	void OptimizeConvexHull();

	gm::Vector3 LocalGetSupportingVertex(const gm::Vector3& vector) const override;
	gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const override;
	virtual void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const;
	virtual void Project(const gm::AffineTransform& transform, const gm::Vector3& direction, gm::Scalar& minProj, gm::Scalar& maxProj, gm::Vector3& witnesPtMin, gm::Vector3& witnesPtMax) const;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3*       GetUnscaledPoints()       { return &_unScaledPoints[0]; }
	const gm::Vector3* GetUnscaledPoints() const { return &_unScaledPoints[0]; }

	INLINE gm::Vector3 GetScaledPoint(int index) const { return _unScaledPoints[index] * _localScaling; }
	INLINE int         GetPointCount ()          const { return _unScaledPoints.size(); }

	virtual int  GetVerticesCount()                                                   const override;
	virtual int  GetEdgesCount()                                                      const override;
	virtual void GetEdge(int index, gm::Vector3& pa, gm::Vector3& pb)                 const override;
	virtual void GetVertex(int index, gm::Vector3& vertex)                            const override;
	virtual int  GetPlanesCount()                                                     const override;
	virtual void GetPlane(gm::Vector3& planeNormal, gm::Vector3& planeSupport, int i) const override;
	virtual bool IsInside(const gm::Vector3& pt, float tolerance)                     const override;

	///in case we receive negative scaling
	virtual void SetLocalScaling(const gm::Vector3& scaling) override;

	virtual const char* GetName() const { return "Convex"; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	ConvexHullShape(const float* points = 0, int numPoints = 0, int stride = sizeof(gm::Vector3));
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<gm::Vector3> _unScaledPoints;
};


///btShapeHull implemented by John McCutchan.
/****************************************************************************
*				  			ConvexHullShape
*************************************************************************//**
*  @class     ConvexHullShape
*  @brief     The ShapeHull class takes a ConvexShape, builds a simplified convex hull using btConvexHull and provides triangle indices and vertices.
*             It can be useful for to simplify a complex convex object and for visualization of a non-polyhedral convex object.
*             It approximates the convex hull using the supporting vertex of 42 directions.
*****************************************************************************/
__declspec(align(16))
class HullShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool BuildHull(float margin, int highres = 0);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	int TriangleCount() const;
	int VerticesCount() const;
	int IndicesCount() const;
	const gm::Vector3* GetVertexPointer() const { return &_vertices[0]; }
	const unsigned int* GetIndexPointer() const { return &_indices[0]; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	HullShape(const ConvexShape* shape);
	~HullShape();

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<gm::Vector3> _vertices;
	gm::AlignedObjectArray<unsigned int> _indices;
	unsigned int       _indexCount;
	const ConvexShape* _shape;

	static gm::Vector3* GetUnitSpherePoints(int highres = 0);

private:
	float _padding[2];
};

#endif