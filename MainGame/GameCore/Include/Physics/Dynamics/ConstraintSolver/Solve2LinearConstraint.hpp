//////////////////////////////////////////////////////////////////////////////////
///             @file   Point2PointConstraint.hpp
///             @brief  Point to Point Constraint
///             @author Partially edit by Toide
///             @date   2021_09_30
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

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
#ifndef SOLVE2_CONSTRAINT_HPP
#define SOLVE2_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMTransform.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class RigidBody;

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			Solve2LinearConstraint
*************************************************************************//**
*  @class     Solve2LinearConstraint
*  @brief     Linear constraint
*****************************************************************************/
class Solve2LinearConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void ResolveUniLateralPairConstraint(RigidBody* body, RigidBody* body1,
		const gm::Matrix3& world2A, const gm::Matrix3& world2B,
		const gm::Vector3& inverseInertiaADiag,
		const float inverseMassA,
		const gm::Vector3& linearVelocityA, const gm::Vector3& angularVelocityA,
		const gm::Vector3& relativePositionA1,
		const gm::Vector3& inverseInertiaBDiag,
		const float inverseMassB,
		const gm::Vector3& linearVelocityB, const gm::Vector3& angularVelocityB,
		const gm::Vector3& relativePositionA2,
		float depthA, const gm::Vector3& normalA,
		const gm::Vector3& relativePositionB1, const gm::Vector3& relativePositionB2,
		float depthB, const gm::Vector3& normalB,
		float& imp0, float& imp1);

	void ResolveBiLateralPairConstraint(
		RigidBody* body0, RigidBody* body1,
		const gm::Matrix3& world2A, const gm::Matrix3& world2B,
		const gm::Vector3& inverseInertiaADiag,
		const float inverseMassA,
		const gm::Vector3& linearVelocityA, const gm::Vector3& angularVelocityA,
		const gm::Vector3& relativePositionA1,
		const gm::Vector3& inverseInertiaBDiag,
		const float inverseMassB,
		const gm::Vector3& linearVelocityB, const gm::Vector3& normalB,
		float& imp0, float& imp1
	);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Solve2LinearConstraint(float tau, float damping) { _tau = tau; _damping = damping; }

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	float _tau;
	float _damping;
};
#endif