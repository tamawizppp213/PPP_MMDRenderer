//////////////////////////////////////////////////////////////////////////////////
///             @file   ConeShape.hpp
///             @brief  ConeShape
///             @author Partially edit by Toide
///             @date   2021_09_19
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
#ifndef CONE_MINKOWSKI_HPP
#define CONE_MINKOWSKI_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConvexShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                         Cone Shape
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			ConeShape
*************************************************************************//**
*  @class     ConeShape
*  @brief     Cone shape
*****************************************************************************/
__declspec(align(16))
class ConeShape : public ConvexInternalShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	gm::Vector3 LocalGetSupportingVertex                  (const gm::Vector3& vector) const override;
	gm::Vector3 LocalGetSupportingVertexWithoutMargin     (const gm::Vector3& vector) const override;
	void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;
	void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	float GetRadius     () const { return _radius; }
	float GetHeight     () const { return _height; }
	int   GetConeUpIndex() const { return _coneIndices[1]; }
	virtual gm::Vector3 GetAnisotropicRollingFrictionDirection() const override{ return gm::Vector3(0, 1, 0); }
	virtual const char* GetName() const { return "Cone"; }

	void SetRadius(float radius) { _radius = radius; }
	void SetHeight(float height) { _height = height; }
	void SetConeUpIndex(int upIndex);
	virtual void SetLocalScaling(const gm::Vector3& scaling) override;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	ConeShape(float radius, float height);

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	gm::Vector3 ConeLocalSupport(const gm::Vector3 vertex) const;

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	float _sinAngle;
	float _radius;
	float _height;
	int   _coneIndices[3];
	
	float _padding[2];
};

class ConeShapeX : public ConeShape
{
public:
	ConeShapeX(float radius, float height);
	gm::Vector3 GetAnisotropicRollingFrictionDirection() const override { return gm::Vector3(1, 0, 0); }
	const char* GetName() const override{ return "ConeX"; }
};

class ConeShapeZ : public ConeShape
{
public:
	ConeShapeZ(float radius, float height);
	gm::Vector3 GetAnisotropicRollingFrictionDirection() const override { return gm::Vector3(1, 0, 0); }
	const char* GetName() const override { return "ConeZ"; }
};
#endif