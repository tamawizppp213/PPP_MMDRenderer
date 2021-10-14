//////////////////////////////////////////////////////////////////////////////////
///             @file   BoxShape.hpp
///             @brief  BoxShape
///             @author Partially edit by Toide
///             @date   2021_09_19
///                     2021_10_06 (first checked) first checked -> no debug.
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
#ifndef OBB_BOX_MINKOWSKI_HPP
#define OBB_BOX_MINKOWSKI_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConvexPolyhedralShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			BoxShape
*************************************************************************//**
*  @class     BoxShape
*  @brief     Box Shape
*****************************************************************************/
__declspec(align(16))
class BoxShape : public PolyhedralConvexShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual gm::Vector3 LocalGetSupportingVertex(const gm::Vector3& vector) const;
	gm::Vector3         LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector);
	virtual void        BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const;
	virtual void        CalculateLocalInertia(float mass, gm::Vector3& inertia) const;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3         GetHalfExtentsWithMargin   () const;
	const gm::Vector3&  GetHalfExtentsWithoutMargin() const;
	virtual void        GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const;
	virtual void        GetPlane(gm::Vector3& planeNormal, gm::Vector3& planeSupport, int i) const;
	virtual int         GetPlanesCount  ()                               const override { return 6; }
	virtual int         GetVerticesCount()                               const override { return 6; }
	virtual int         GetEdgesCount   ()                               const override { return 12; }
	virtual int         GetPreferredPenetrationDirectionsCount()         const override { return 6; }
	virtual void        GetPreferredPenetrationDirection(int index, gm::Vector3& penetrationVector) const override;
	virtual void        GetVertex(int index, gm::Vector3& vertex)        const override;
	virtual void        GetPlaneEquation(gm::Vector4& plane, int i)      const;
	virtual void        GetEdge(int i, gm::Vector3& pa, gm::Vector3& pb) const override;
	virtual bool        IsInside(const gm::Vector3& pt, float tolerance) const override;
	virtual const char* GetName() const override{ return "Box"; }

	virtual void SetMargin(float collisionMargin);
	virtual void SetLocalScaling(const gm::Vector3& scaling);
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	BoxShape(const gm::Vector3& boxHalfExtents);

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

#endif