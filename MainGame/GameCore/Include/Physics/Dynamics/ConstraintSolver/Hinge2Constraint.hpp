//////////////////////////////////////////////////////////////////////////////////
///             @file   Hinge2Constraint.hpp
///             @brief  Hinge2 Constraint
///             @author Partially edit by Toide
///             @date   2021_09_30
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
#ifndef HINGE2_CONSTRAINT_HPP
#define HINGE2_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Generic6DofSpring2Constraint.hpp"

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
*  @brief     // Constraint similar to ODE Hinge2 Joint
// has 3 degrees of frredom:
// 2 rotational degrees of freedom, similar to Euler rotations around Z (axis 1) and X (axis 2)
// 1 translational (along axis Z) with suspension spring
*****************************************************************************/
__declspec(align(16))
class Hinge2Constraint : public Generic6DofSpring2Constraint
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
	float GetAngle2() { return GetAngle(0); }

	void SetUpperLimit(float angle1Max) { SetAngularUpperLimit(gm::Vector3(-1.0f, 0.0f, angle1Max)); }
	void SetLowerLimit(float angle1Min) { SetAngularLowerLimit(gm::Vector3(1.0f, 0.0f, angle1Min)); }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	Hinge2Constraint(RigidBody& rigidbodyA, RigidBody& rigidbodyB, gm::Vector3& anchor, gm::Vector3& axis1, gm::Vector3& axis2);

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