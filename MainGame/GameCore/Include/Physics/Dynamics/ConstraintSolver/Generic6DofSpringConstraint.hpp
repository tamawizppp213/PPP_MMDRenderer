//////////////////////////////////////////////////////////////////////////////////
///             @file   Generic6DofSpringConstraint.hpp
///             @brief  Generic6DofSpring Constraint
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
#ifndef GENERIC_6DOF_SPRING_CONSTRAINT_HPP
#define GENERIC_6DOF_SPRING_CONSTRAINT_HPP

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
*				  			Generic6DofSpringConstraint
*************************************************************************//**
*  @class     Generic6DofSpringConstraint
*  @brief     DOF index used in enableSpring() and setStiffness() means:
 0 : translation X
 1 : translation Y
 2 : translation Z
 3 : rotation X (3rd Euler rotational around new position of X axis, range [-PI+epsilon, PI-epsilon] )
 4 : rotation Y (2nd Euler rotational around new position of Y axis, range [-PI/2+epsilon, PI/2-epsilon] )
 5 : rotation Z (1st Euler rotational around Z axis, range [-PI+epsilon, PI-epsilon] )

*****************************************************************************/
__declspec(align(16))
class Generic6DofSpringConstraint : Generic6DofConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool IsSpringEnabled     (int index) const { return _springEnabled[index]; }
	float GetStiffness       (int index) const { return _springStiffness[index]; }
	float GetDamping         (int index) const { return _springDamping[index]; }
	float GetEquilibriumPoint(int index) const { return _equilibriumPoint[index]; }

	void EnableSpring(int index, bool onoff);
	void SetStiffness(int index, float stiffness);
	void SetDamping(int index, float damping);
	void SetEquilibriumPoint();                     // set the current constraint position/orientation as an equilibrium point for all DOF
	void SetEquilibriumPoint(int index);            // set the current constraint position/orientation as an equilibrium point for given DOF
	void SetEquilibriumPoint(int index, float value);

	void SetAxis(const gm::Vector3& axis1, const gm::Vector3& axis2) override;
	void GetInfo2(ConstraintInfo2* infomation) override;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	Generic6DofSpringConstraint(RigidBody& rigidbodyA, RigidBody& rigidbodyB, const gm::Transform& frameInA, const gm::Transform& frameInB, bool useLinearReferenceFrameA);
	Generic6DofSpringConstraint(RigidBody& rigidbodyB, const gm::Transform& frameInB, bool useLinearReferenceFrameB);

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	void Initialize();
	void InternalUpdateSprings(ConstraintInfo2* infomation);

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	bool _springEnabled[6];
	float _equilibriumPoint[6];
	float _springStiffness[6];
	float _springDamping[6];  // from 0 to 1 (1 == no damping)

};

#endif