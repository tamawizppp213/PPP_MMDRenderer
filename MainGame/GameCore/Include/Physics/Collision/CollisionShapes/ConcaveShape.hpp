//////////////////////////////////////////////////////////////////////////////////
///             @file   ConcaveShape.hpp
///             @brief  ConcaveShape
///             @author 
///             @date   2021_10_05 (first checked) first checked -> no debug.
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
#ifndef CONCAVE_SHAPE_HPP
#define CONCAVE_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "CollisionShape.hpp"
#include "TriangleCallback.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class PhyScalarType
{
	Float       , 
	Double      ,
	Integer     ,
	Short       , 
	FixedPoint88,
	Uchar    
};

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////


/****************************************************************************
*				  			ConcaveShape
*************************************************************************//**
*  @class     ConcaveShape
*  @brief     Concave shape
*****************************************************************************/
__declspec(align(16)) 
class ConcaveShape : public ICollisionShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void ProcessAllTriangles(TriangleCallback* callback, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax) const = 0;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	float GetMargin() const override{ return _collisionMargin; }
	void  SetMargin(float collisionMargin) override { _collisionMargin = collisionMargin; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	ConcaveShape() { _collisionMargin = 0.0f; };
	virtual ~ConcaveShape() {};

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	float _collisionMargin;
};
#endif