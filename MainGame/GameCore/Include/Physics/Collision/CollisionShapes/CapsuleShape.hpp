//////////////////////////////////////////////////////////////////////////////////
///             @file   CapsuleShape.hpp
///             @brief  Capsule shape
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
#ifndef CAPSULE_SHAPE_HPP
#define CAPSULE_SHAPE_HPP

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
class CapsuleShape : public ConvexInternalShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void        CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;
	virtual gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const override;
	virtual void        BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void        GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override;
	virtual const char* GetName      () const { return "CapsuleShape"; }
	int                 GetUpAxis    () const { return _upAxis; }
	float               GetRadius    () const { int radiusAxis = (_upAxis + 2) % 3; return _implicitShapeDimensions.GetElement((gm::Vector3Element)radiusAxis); }
	float               GetHalfHeight() const { return  _implicitShapeDimensions.GetElement((gm::Vector3Element)_upAxis);}
	virtual gm::Vector3 GetAnisotropicRollingFrictionDirection() const override;

	virtual void SetMargin(float collisionMargin) override { (void)collisionMargin; }
	virtual void SetLocalScaling(const gm::Vector3& scaling) override;
	

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	CapsuleShape(float radius, float height);

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	CapsuleShape() : ConvexInternalShape() { _shapeType = (int)BroadPhaseNativeTypes::Capsule_Shape; }

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	int _upAxis;
};

class CapsuleShapeX : public CapsuleShape
{
public:
	CapsuleShapeX(float radius, float height);
	virtual const char* GetName() const { return "CapsuleX"; }
};



class CapsuleShapeZ : public CapsuleShape
{
public:
	CapsuleShapeZ(float radius, float height);
	virtual const char* GetName() const { return "CapsuleZ"; }
};
#endif