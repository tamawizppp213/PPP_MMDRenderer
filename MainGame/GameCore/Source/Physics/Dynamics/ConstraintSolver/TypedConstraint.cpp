//////////////////////////////////////////////////////////////////////////////////
///             @file   TypedConstraint.hpp
///             @brief  Typed constraint
///             @author Partially edit by Toide
///             @date   2021_09_29
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

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Dynamics/ConstraintSolver/TypedConstraint.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define DEFAULT_DEBUGDRAW_SIZE (0.05f)
static RigidBody g_fixed(0, 0, 0);

//////////////////////////////////////////////////////////////////////////////////
//							Implement
//////////////////////////////////////////////////////////////////////////////////
TypedConstraint::TypedConstraint(ConstraintType type, RigidBody& rbA)
	: gm::utils::TypedObject((int)type),
	_userConstraintType(-1),
	_userConstraintPtr((void*)-1),
	_breakingImpulseThreshold(FLT_MAX),
	_isEnabled(true),
	_needsFeedback(false),
	_overrideSolverIterationsCount(-1),
	_rigidBodyA(rbA),
	_rigidBodyB(TypedConstraint::GetFixedBody()),
	_appliedImpulse(0.0f),
	_debugDrawSize(DEFAULT_DEBUGDRAW_SIZE),
	_jointFeedback(0)
{
}

TypedConstraint::TypedConstraint(ConstraintType type, RigidBody& rbA, RigidBody& rbB)
	: gm::utils::TypedObject((int)type),
	_userConstraintType(-1),
	_userConstraintPtr((void*)-1),
	_breakingImpulseThreshold(FLT_MAX),
	_isEnabled(true),
	_needsFeedback(false),
	_overrideSolverIterationsCount(-1),
	_rigidBodyA(rbA),
	_rigidBodyB(rbB),
	_appliedImpulse(0.0f),
	_debugDrawSize(DEFAULT_DEBUGDRAW_SIZE),
	_jointFeedback(0)
{
}

float TypedConstraint::GetMotorFactor(float position, float lowLimit, float upperLimit, float velocity, float timeFactor)
{
	if (lowLimit > upperLimit)
	{
		return 1.0f;
	}
	else if (lowLimit == upperLimit)
	{
		return 0.0f;
	}
	float lim_fact = 1.0f;
	float delta_max = velocity / timeFactor;
	if (delta_max < 0.0f)
	{
		if ((position >= lowLimit) && (position < (lowLimit - delta_max)))
		{
			lim_fact = (lowLimit - position) / delta_max;
		}
		else if (position < lowLimit)
		{
			lim_fact = 0.0f;
		}
		else
		{
			lim_fact = 1.0f;
		}
	}
	else if (delta_max > 0.0f)
	{
		if ((position <= upperLimit) && (position > (upperLimit - delta_max)))
		{
			lim_fact = (upperLimit - position) / delta_max;
		}
		else if (position > upperLimit)
		{
			lim_fact = 0.0f;
		}
		else
		{
			lim_fact = 1.0f;
		}
	}
	else
	{
		lim_fact = 0.0f;
	}
	return lim_fact;
}


RigidBody& TypedConstraint::GetFixedBody()
{
	g_fixed = RigidBody(0, 0, 0);
	g_fixed.SetMassProperty(0.0f, Vector3(0.0f, 0.0f, 0.0f));
	return g_fixed;
}

void AngularLimit::Set(float low, float high, float softness, float biasFactor, float relaxationFactor)
{
	_halfRange = (high - low) / 2.0f;
	_center           = NormalizeAngle(low + _halfRange);
	_softness         = _softness;
	_biasFactor       = _biasFactor;
	_relaxationFactor = _relaxationFactor;
}

void AngularLimit::Test(const float angle)
{
	_correction = 0.0f;
	_sign = 0.0f;
	_solveLimit = false;

	if (_halfRange >= 0.0f)
	{
		float deviation = NormalizeAngle(angle - _center);
		if (deviation < -_halfRange)
		{
			_solveLimit = true;
			_correction = -(deviation + _halfRange);
			_sign = +1.0f;
		}
		else if (deviation > _halfRange)
		{
			_solveLimit = true;
			_correction = _halfRange - deviation;
			_sign = -1.0f;
		}
	}
}

float AngularLimit::GetError() const
{
	return _correction * _sign;
}

void AngularLimit::Fit(float& angle)
{
	if (_halfRange > 0.0f)
	{
		float relativeAngle = NormalizeAngle(angle - _center);
		if (relativeAngle != _halfRange)
		{
			if (relativeAngle > 0.0f)
			{
				angle = GetHigh();
			}
			else
			{
				angle = GetLow();
			}
		}
	}
}

float AngularLimit::GetLow() const
{
	return NormalizeAngle(_center - _halfRange);
}

float AngularLimit::GetHigh() const
{
	return NormalizeAngle(_center + _halfRange);
}
