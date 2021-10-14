//////////////////////////////////////////////////////////////////////////////////
///             @file   CylinderShape.hpp
///             @brief  CylinderShape
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
#ifndef CYLINDER_MINKOWSKI_HPP
#define CYLINDER_MINKOWSKI_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#include "BoxShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			CylinderShape
*************************************************************************//**
*  @class     CylinderShape
*  @brief     CylinderShape
*****************************************************************************/
__declspec(align(16))
class CylinderShape : public ConvexInternalShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;
	gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const override;
	void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;
	gm::Vector3 LocalGetSupportingVertex(const gm::Vector3& vector) const override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const;
	int  GetUpAxis() const { return _upAxis; }

	gm::Vector3         GetHalfExtentsWithMargin() const;
	const gm::Vector3   GetHalfExtentsWithoutMargin() const { return _implicitShapeDimensions; }
	gm::Vector3 GetAnisotropicRollingFrictionDirection() const override;
	float       GetRadius() const          { return GetHalfExtentsWithMargin().GetX(); };
	const char* GetName  () const override { return "CylinderY"; }

	void SetMargin(float collisionMargin) override;
	void SetLocalScaling(const gm::Vector3& scaling) override;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	CylinderShape(const gm::Vector3& halfExtents);
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	int _upAxis;
};

class CylinderShapeX : public CylinderShape
{
public:
	DECLARE_ALIGNED_ALLOCATOR;
	CylinderShapeX(const gm::Vector3& halfExtents);

	virtual gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const override;
	virtual void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;
	virtual const char* GetName() const { return "CylinderX"; }
	virtual float     GetRadius() const { return GetHalfExtentsWithMargin().GetY(); }
};

class CylinderShapeZ : public CylinderShape
{
public:
	DECLARE_ALIGNED_ALLOCATOR;

	CylinderShapeZ(const gm::Vector3& halfExtents);

	virtual gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const override;
	virtual void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;
	virtual const char* GetName() const { return "CylinderZ"; }
	virtual float     GetRadius() const { return GetHalfExtentsWithMargin().GetX(); }
};
#endif
