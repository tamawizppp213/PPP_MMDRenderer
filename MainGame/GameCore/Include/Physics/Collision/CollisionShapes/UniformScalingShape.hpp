//////////////////////////////////////////////////////////////////////////////////
///             @file   UniformScalingShape.hpp
///             @brief  UniformScalingShape
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
#ifndef UNIFORM_SCALING_SHAPE_HPP
#define UNIFORM_SCALING_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConvexShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			UniformScalingShape
*************************************************************************//**
*  @class     UniformScalingShape
*  @brief     The UniformScalingShape allows to re-use uniform scaled instances of btConvexShape in a memory efficient way.
              Instead of using UniformScalingShape, it is better to use the non-uniform setLocalScaling method on convex shapes that implement it.
*****************************************************************************/
__declspec(align(16))
class UniformScalingShape : public ConvexShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const override;
	virtual gm::Vector3 LocalGetSupportingVertex(const gm::Vector3& vector) const override;
	virtual void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;
	virtual void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void  GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const;
	float GetUniformScalingFactor    () const { return _uniformScalingFactor; }
	ConvexShape*        GetChildShape()       { return _childConvexShape; }
	const ConvexShape*  GetChildShape() const { return _childConvexShape; }
	virtual const char* GetName      () const { return "UniformScalingShape"; }
	virtual void        GetAABBSlow  (const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override;
	virtual const gm::Vector3& GetLocalScaling() const override;
	virtual float       GetMargin()              const override;

	virtual int  GetPreferredPenetrationDirectionsCount() const override;
	virtual void GetPreferredPenetrationDirection(int index, gm::Vector3& penetrationVector) const override;
	virtual void SetLocalScaling(const gm::Vector3& scaling) override;
	virtual void SetMargin(float margin) override;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	UniformScalingShape(ConvexShape* convexChildShape, float uniformScalingFactor);
	virtual ~UniformScalingShape() {};

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	ConvexShape* _childConvexShape;
	float        _uniformScalingFactor;
	float        _padding[2];
};

#endif