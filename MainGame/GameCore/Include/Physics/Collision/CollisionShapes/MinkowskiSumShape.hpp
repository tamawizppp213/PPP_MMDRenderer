//////////////////////////////////////////////////////////////////////////////////
///             @file   MinkowskiSumShape.hpp
///             @brief  MinkowskiSumShape
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
#ifndef MINKOWSKI_SUM_SHAPE_HPP
#define MINKOWSKI_SUM_SHAPE_HPP

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
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
__declspec(align(16))
class MinkowskiSumShape : public ConvexInternalShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const override;
	virtual void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;
	virtual void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetTransformA(const gm::AffineTransform& transformA) { _transformA = transformA; }
	void SetTransformB(const gm::AffineTransform& transformB) { _transformB = transformB; }

	const ConvexShape* GetShapeA() const { return _shapeA; }
	const ConvexShape* GetShapeB() const { return _shapeB; }

	virtual const char* GetName() const { return "MinkowskiSum"; }
	virtual float GetMargin() const override;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	MinkowskiSumShape(const ConvexShape* shapeA, const ConvexShape* shapeB);

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::AffineTransform _transformA;
	gm::AffineTransform _transformB;
	const ConvexShape* _shapeA;
	const ConvexShape* _shapeB;
};

#endif