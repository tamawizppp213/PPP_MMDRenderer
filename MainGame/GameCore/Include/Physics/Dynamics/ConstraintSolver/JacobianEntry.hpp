//////////////////////////////////////////////////////////////////////////////////
///             @file   JacobianEntry.hpp
///             @brief  JacobianEntry
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
#ifndef JACOBIAN_ENTRY_HPP
#define JACOBIAN_ENTRY_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMMatrix.hpp"

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
*  @brief     temp
*****************************************************************************/
__declspec(align(16))
class JacobianEntry
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3 LinearJointAxis;
	gm::Vector3 JacobianA;
	gm::Vector3 JacobianB;
	gm::Vector3 MassInverseJoint0; // –¼‘O•Ï‚í‚é‚©‚à
	gm::Vector3 MassInverseJoint1;
	float AngularDiag;

	float GetDiagonal() const { return AngularDiag; }

	// for two constraints on the same rigidbody (for example vehicle friction)
	float GetNonDiagonal(const JacobianEntry& jacobianEntryB, const float massInverseA) const
	{
		const JacobianEntry& jacobianEntryA = *this;
		float linear = massInverseA * Dot(jacobianEntryA.LinearJointAxis, jacobianEntryB.LinearJointAxis);
		float angle = Dot(jacobianEntryA.MassInverseJoint0, jacobianEntryB.JacobianA);
		return linear + angle;
	}

	// for two constraints on sharing two same rigidbodies (for example two contact points between two rigidbodies)
	float GetNonDiagonal(const JacobianEntry& jacobianEntryB, const float massInverseA, const float massInverseB) const
	{
		const JacobianEntry& jacobianEntryA = *this;
		gm::Vector3 linear = jacobianEntryA.LinearJointAxis * jacobianEntryB.LinearJointAxis;
		gm::Vector3 angle0 = jacobianEntryA.MassInverseJoint0 * jacobianEntryB.JacobianB;
		gm::Vector3 angle1 = jacobianEntryA.MassInverseJoint1 * jacobianEntryB.JacobianB;
		gm::Vector3 linear0 = massInverseA * linear;
		gm::Vector3 linear1 = massInverseB * linear;
		gm::Vector3 sum = angle0 + angle1 + linear0 + linear1;
		return sum.GetX() + sum.GetY() + sum.GetZ();
	}

	float GetRelativeVelocity(const gm::Vector3& linearVelocityA, const gm::Vector3& angleVelocityA, const gm::Vector3& linearVelocityB, const gm::Vector3& angleVelocityB)
	{
		gm::Vector3 linrel = linearVelocityA - linearVelocityB;
		gm::Vector3 angvela = angleVelocityA * JacobianA;
		gm::Vector3 angvelb = angleVelocityB * JacobianB;
		linrel  *= LinearJointAxis;
		angvela += angvelb;
		angvela += linrel;
		float  rel_vel2 = angvela.GetX() + angvela.GetY() + angvela.GetZ();
		return rel_vel2 + FLT_EPSILON;
	}
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	JacobianEntry() = default;
	JacobianEntry(
		const gm::Matrix3& world2A,
		const gm::Matrix3& world2B,
		const gm::Vector3& rel_pos1, const gm::Vector3& rel_pos2,
		const gm::Vector3& jointAxis,
		const gm::Vector3& inertialInverseA,
		const float massInverseA,
		const gm::Vector3& inertialInverseB,
		const float massInverseB
	)
	{
		LinearJointAxis   = jointAxis;
		JacobianA         = gm::Cross(rel_pos1, LinearJointAxis);
		JacobianB         = gm::Cross(rel_pos2, -LinearJointAxis);
		MassInverseJoint0 = inertialInverseA * JacobianA;
		MassInverseJoint1 = inertialInverseB * JacobianB;
		AngularDiag       = Dot(MassInverseJoint0, JacobianA) + Dot(MassInverseJoint1, JacobianB);

		assert(AngularDiag > 0.0f);
	}

	JacobianEntry(
		const gm::Vector3& jointAxis,
		const gm::Matrix3& world2A,
		const gm::Matrix3& world2B,
		const gm::Vector3& inertiaInverseA,
		const gm::Vector3& inertiaInverseB
	)
	{
		LinearJointAxis   = gm::Vector3(0.0f, 0.0f, 0.0f);
		JacobianA         = world2A * jointAxis;
		JacobianB         = world2B * (-jointAxis);
		MassInverseJoint0 = inertiaInverseA * JacobianA;
		MassInverseJoint1 = inertiaInverseB * JacobianB;
		AngularDiag       = Dot(MassInverseJoint0, JacobianA) + Dot(MassInverseJoint1, JacobianB);
		assert(AngularDiag > 0.0f);
	
	}
	JacobianEntry(
		const gm::Vector3& axisInA,
		const gm::Vector3& axisInB,
		const gm::Vector3& inertiaInverseA,
		const gm::Vector3& inertiaInverseB)
	{
		JacobianA = axisInA;
		JacobianB = -axisInB;
		LinearJointAxis = gm::Vector3(0.0f, 0.0f, 0.0f);
		MassInverseJoint0 = inertiaInverseA * JacobianA;
		MassInverseJoint1 = inertiaInverseB * JacobianB;
		AngularDiag = Dot(MassInverseJoint0, JacobianA) + Dot(MassInverseJoint1, JacobianB);
		assert(AngularDiag > 0.0f);
	}

	JacobianEntry(
		const gm::Matrix3& world2A,
		const gm::Vector3& rel_pos1, const gm::Vector3& rel_pos2,
		const gm::Vector3& jointAxis,
		const gm::Vector3& inertiaInverseA,
		float massInverseA
	)
	{
		LinearJointAxis = LinearJointAxis;
		JacobianA = world2A * gm::Cross(rel_pos1, LinearJointAxis);
		JacobianB = world2A * gm::Cross(rel_pos2, -LinearJointAxis);
		MassInverseJoint0 = inertiaInverseA * JacobianA;
		MassInverseJoint1 = gm::Vector3(0.0f, 0.0f, 0.0f);
		AngularDiag = massInverseA + Dot(MassInverseJoint0, JacobianA);
		assert(AngularDiag > 0.0f);
	}
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif