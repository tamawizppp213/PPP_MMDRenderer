//////////////////////////////////////////////////////////////////////////////////
///             @file   UniversalConstraint.hpp
///             @brief  Universal Constraint
///             @author Partially edit by Toide
///             @date   2021_10_02
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library, http://bulletphysics.org
Copyright (C) 2006, 2007 Sony Computer Entertainment Inc.

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
#ifndef UNIVERSAL_CONSTRAINT_HPP
#define UNIVERSAL_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Generic6DofConstraint.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     Constraint similar to ODE Universal Joint
              has 2 rotatioonal degrees of freedom, similar to Euler rotations around Z (axis 1)
              and Y (axis 2)
              Description from ODE manual :
              "Given axis 1 on body 1, and axis 2 on body 2 that is perpendicular to axis 1, it keeps them perpendicular.
              In other words, rotation of the two bodies about the direction perpendicular to the two axes will be equal."

*****************************************************************************/
__declspec(align(16))
class UniversalConstraint : public Generic6DofConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const gm::Vector3& GetAnchor() { return _calculatedTransformA.LocalPosition; }
	const gm::Vector3& GetAnchor2() { return _calculatedTransformB.LocalPosition; }
	const gm::Vector3& GetAxis1() { return _axis1; }
	const gm::Vector3& GetAxis2() { return _axis2; }
	float GetAngle1() { return GetAngle(2); }
	float GetAngle2() { return GetAngle(1); }
	void SetUpperLimit(float angle1Max, float angle2Max) { SetAngularUpperLimit(gm::Vector3(0.0f, angle1Max, angle2Max)); }
	void SetLowerLimit(float angle1Max, float angle2Max) { SetAngularLowerLimit(gm::Vector3(0.0f, angle1Max, angle2Max)); }
	void SetAxis(const gm::Vector3& axis1, const gm::Vector3& axis2);


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;

	UniversalConstraint(RigidBody& rigidbodyA, RigidBody& rigidbodyB, const gm::Vector3& anchor, const gm::Vector3& axis1, const gm::Vector3& axis2);
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::Vector3 _anchor;
	gm::Vector3 _axis1;
	gm::Vector3 _axis2;
};

#endif