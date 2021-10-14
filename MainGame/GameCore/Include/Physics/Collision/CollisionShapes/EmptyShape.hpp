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
#ifndef EMPTY_SHAPE_HPP
#define EMPTY_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConcaveShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                          Empty Shape
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			EmptyShape
*************************************************************************//**
*  @class     EmptyShape
*  @brief     EmptyShape is a collision shape without actual collision detection shape, so most users should ignore this class
*             It can be replaced by another shape during runtime, but the inertia tensor should be recomputed.
*****************************************************************************/
__declspec(align(16))
class EmptyShape :public ConcaveShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const;
	const gm::Vector3& GetLocalScaling() const override { return _localScaling; }
	void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;
	void ProcessAllTriangles(TriangleCallback*, const gm::Vector3&, const gm::Vector3&)const override;

	virtual const char* GetName() const { return "Empty"; }
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual void SetLocalScaling(const gm::Vector3& scaling) override { _localScaling = scaling; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	EmptyShape();
	~EmptyShape() {}
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::Vector3 _localScaling;
};
#endif