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

#pragma once
#ifndef BOX2D_SHAPE_HPP
#define BOX2D_SHAPE_HPP

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
*				  			Box2DClass
*************************************************************************//**
*  @class     Box2DShape
*  @brief     Box2DShape
*****************************************************************************/
__declspec(align(16))
class Box2DShape : public PolyhedralConvexShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	gm::Vector3        GetHalfExtentsWithMargin   () const;
	const gm::Vector3& GetHalfExtentsWithoutMargin() const;
	gm::Vector3        LocalGetSupportingVertex                         (const gm::Vector3& vector) const override;
	gm::Vector3        LocalGetSupportingVertexWithoutMargin            (const gm::Vector3& vector);
	void               BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;

	void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void         GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override;
	int          GetVertexCount  () const          { return 4; }
	int          GetVerticesCount() const override { return 4; }
	int          GetPlanesCount  () const override { return 6; }
	int          GetEdgesCount   () const override { return 12; }
	virtual int  GetPreferredPenetrationDirections() const { return 6; }
	virtual void GetPreferredPenetrationDirection(int index, gm::Vector3& penetrationVector) const;
	void         GetVertex(int i, gm::Vector3& vertex) const override;
	virtual void GetPlaneEquation(gm::Vector4& plane, int i) const;
	void GetPlane(gm::Vector3& planeNormal, gm::Vector3& planeSupport, int i) const override;
	void GetEdge(int i, gm::Vector3& pa, gm::Vector3& pb) const override;

	bool IsInside(const gm::Vector3& point, float tolerance) const override;
	const char*        GetName    () const override { return "Box2d"; }
 	const gm::Vector3* GetVertices() const{ return &_vertices[0]; }
	const gm::Vector3* GetNormals () const { return &_normals[0]; }
	const gm::Vector3& GetCentroid() const { return _centroid; }


	void SetMargin(float collisionMargin) override;
	void SetLocalScaling(const gm::Vector3& scaling) override;


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	Box2DShape(const gm::Vector3& boxHalfExtents);
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::Vector3 _centroid;
	gm::Vector3 _vertices[4];
	gm::Vector3 _normals[4];
};

#endif