//////////////////////////////////////////////////////////////////////////////////
///             @file   SolverConstraint.hpp
///             @brief  Solver Constraint
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
#ifndef SOLVER_BODY_HPP
#define SOLVER_BODY_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../../../../../GameMath/Include/GMTransform.hpp"
#include "../../../../../GameMath/Include/GMAlighedAllocator.hpp"
#include "../../Util/TransformUtil.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class RigidBody;

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			SolverBody
*************************************************************************//**
*  @class     SolverBody
*  @brief     The SolverBody is an internal datastructure for the constraint solver. 
              Only necessary data is packed to increase cache coherence/performance.
*****************************************************************************/
__declspec(align(16))
struct SolverBody
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void WritebackVelocity()
	{
		if (OriginalBody)
		{
			LinearVelocity += DeltaLinearVelocity;
			AngularVelocity += DeltaAngularVelocity;
		}
	}
	void WritebackVelocityAndTransform(float timeStep, float splitImpulseTurnErp)
	{
		(void)timeStep;
		if (OriginalBody)
		{
			LinearVelocity  += DeltaLinearVelocity;
			AngularVelocity += DeltaAngularVelocity;

			//correct the position/orientation based on push/turn recovery
			gm::AffineTransform newTransform;
			if (PushVelocity.GetX() != 0.f || PushVelocity.GetY() != 0.0f || PushVelocity.GetZ() != 0.0f || TurnVelocity.GetX() != 0.f || TurnVelocity.GetY() != 0.0f || TurnVelocity.GetZ() != 0.0f)
			{
				//	btQuaternion orn = m_worldTransform.getRotation();
				AffineTransformUtil::IntegrateTransform(WorldTransform, PushVelocity, TurnVelocity * splitImpulseTurnErp, timeStep, newTransform);
				WorldTransform = newTransform;
			}
		}
	}
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::AffineTransform WorldTransform;
	gm::Vector3 DeltaLinearVelocity;
	gm::Vector3 DeltaAngularVelocity;
	gm::Vector3 AngularFactor;
	gm::Vector3 LinearFactor;
	gm::Vector3 InverseMass;
	gm::Vector3 PushVelocity;
	gm::Vector3 TurnVelocity;
	gm::Vector3 LinearVelocity;
	gm::Vector3 AngularVelocity;
	gm::Vector3 ExternalForceImpulse;
	gm::Vector3 ExternalTorqueImpulse;
	RigidBody*  OriginalBody;

	const gm::AffineTransform& GetWorldTransform    () const { return WorldTransform; }
	const gm::Vector3& GetDeltaLinearVelocity () const { return DeltaLinearVelocity; }
	const gm::Vector3& GetDeltaAngularVelocity() const { return DeltaAngularVelocity; }
	const gm::Vector3& GetPushVelocity        () const { return PushVelocity; }
	const gm::Vector3& GetTurnVelocity        () const { return TurnVelocity; }
	const gm::Vector3& InternalGetAngularFactor() const { return AngularFactor; }
	const gm::Vector3& InternalGetInverseMass () const { return InverseMass; }
	gm::Vector3& InternalGetDeltaLinearVelocity() { return DeltaLinearVelocity; }
	gm::Vector3& InternalGetDeltaAngularVelocity() { return DeltaAngularVelocity; }
	gm::Vector3& InternalGetPushVelocity() { return PushVelocity; }
	gm::Vector3& InternalGetTurnVelocity() { return TurnVelocity; }


	INLINE void GetVelocityInLocalPointNoDelta(const gm::Vector3& rel_pos, gm::Vector3& velocity) const
	{
		if(OriginalBody){ velocity = Cross(LinearVelocity + ExternalForceImpulse + (AngularVelocity + ExternalTorqueImpulse), rel_pos); }
		else { velocity = gm::Vector3(0, 0, 0); }
	}
	INLINE void GetVelocityInLocalPointObsolete(const gm::Vector3& rel_pos, gm::Vector3& velocity) const
	{
		if (OriginalBody) { velocity = Cross(LinearVelocity + DeltaLinearVelocity + (AngularVelocity + ExternalTorqueImpulse), rel_pos); }
		else { velocity = gm::Vector3(0, 0, 0); }
	}
	INLINE void GetAngularVelocity(gm::Vector3& angleVelocity) const
	{
		if (OriginalBody) { AngularVelocity + DeltaAngularVelocity; }
		else { angleVelocity = gm::Vector3(0, 0, 0); }
	}
	INLINE void ApplyImpulse(const gm::Vector3& linearComponent, const gm::Vector3& angularComponent, const float impulseMagnitude)
	{
		if (OriginalBody)
		{
			DeltaLinearVelocity  += linearComponent * impulseMagnitude * LinearFactor;
			DeltaAngularVelocity += angularComponent * (impulseMagnitude * AngularFactor);
		}
	}
	INLINE void InternalApplyPushImpulse(const gm::Vector3& linearComponent, const gm::Vector3& angularComponent, float impulseMagnitude)
	{
		if (OriginalBody)
		{
			PushVelocity += linearComponent * impulseMagnitude * LinearFactor;
			TurnVelocity += angularComponent * (impulseMagnitude * AngularFactor);
		}
	}
	INLINE void InternalGetVelocityInLocalPointObsolete(const gm::Vector3& rel_pos, gm::Vector3& velocity) const
	{
		velocity = Cross(LinearVelocity + DeltaLinearVelocity + (AngularVelocity + DeltaAngularVelocity),rel_pos);
	}
	INLINE void InternalGetAngularVelocity(gm::Vector3& angularVelocity) const
	{
		angularVelocity = AngularVelocity + DeltaAngularVelocity;
	}
	INLINE void InternalApplyImpulse(const gm::Vector3& linearComponent, const gm::Vector3& angularComponent, const float impulseMagnitude)
	{
		if (OriginalBody)
		{
			DeltaLinearVelocity  += linearComponent * impulseMagnitude * LinearFactor;
			DeltaAngularVelocity += angularComponent * (impulseMagnitude * AngularFactor);
		}
	}

	void SetWorldTransform(const gm::AffineTransform& worldTransform) { WorldTransform = worldTransform; }
	void SetInternalInverseMass(const gm::Vector3& inverseMass) { InverseMass = inverseMass; }


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif