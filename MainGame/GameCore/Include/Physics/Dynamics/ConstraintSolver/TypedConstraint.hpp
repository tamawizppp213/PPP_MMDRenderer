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

#pragma once
#ifndef TYPED_CONSTRAINT_HPP
#define TYPED_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "SolverConstraint.hpp"
#include "GameCore/Include/Physics/Dynamics/Dynamics/RigidBody.hpp"
#include "GameMath/Include/GMVectorUtility.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
//Don't change any of the existing enum values, so add enum types at the end for serialization compatibility
enum class ConstraintType
{
	Point2Point = 3,
	Hinge       = 4,
	ConeTwist   = 5,
	Dof6        = 6,
	Slider      = 7,
	Gear        = 8,
	Fixed       = 9,
	Dof6Spring  = 10,
	CountOfConstraintType // ?
};

enum class ConstraintParams
{
	ERP     = 1,
	StopERP = 2,
	CFM     = 3,
	StopCFM = 4,
};
//////////////////////////////////////////////////////////////////////////////////
//                            Struct and Class
//////////////////////////////////////////////////////////////////////////////////
__declspec(align(16))
struct JointFeedback
{
	DECLARE_ALIGNED_ALLOCATOR;
	gm::Vector3 AppliedForceBodyA;
	gm::Vector3 AppliedTorqueBodyA;
	gm::Vector3 AppliedForceBodyB;
	gm::Vector3 AppliedTorqueBodyB;
};

__declspec(align(16))
class TypedConstraint : public gm::utils::TypedObject
{
public:
	struct ConstraintInfo1 { int ConstraintRowsCount; int Nub; };
	struct ConstraintInfo2
	{
		float FPS;             // integrator parameters: frame per second = (1 / stepsize),
		float ERP;             // integrator parameters: default error reduction parameter (from 0 to 1)
		float* J1LinearAxis;  
		float* J1AngularAxis;
		float* J2LinearAxis;
		float* J2AngularAxis;
		int    RowSkip;        // Elements to jump from one row to the next in J's
		float* ConstraintError;// J*v = c + cfm * lambda.  cfm is the constraint force mixing vector c is set set to zero on entry. cfm is set to a constant value(verysmall or zero on entry) 
		float* CFM;            // J*v = c + cfm * lambda.  cfm is the constraint force mixing vector c is set set to zero on entry. cfm is set to a constant value(verysmall or zero on entry) 
		float* LowerLimit;     // lo and hi limits for variables
		float* UpperLimit;     // lo and hi limits for variables
		int    IterationCount; // solver iterations
		float  Damping;        // Damping of velocity
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void BuildJacobian() {};
	virtual void SolveConstraintObsolete(SolverBody&, SolverBody&, float) {};
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	RigidBody& GetFixedBody();
	int   GetOverrideSolverIterationsCount() const { return _overrideSolverIterationsCount; }
	float GetInternalAppliedImpulse       ()       { return _appliedImpulse; }
	float GetBreakingImpulseThreshold     () const { return _breakingImpulseThreshold; }
	const RigidBody& GetRigidBodyA        () const { return _rigidBodyA; }
	const RigidBody& GetRigidBodyB        () const { return _rigidBodyB; }
	RigidBody&       GetRigidBodyA        ()       { return _rigidBodyA; }
	RigidBody&       GetRigidBodyB        ()       { return _rigidBodyB; }
	int   GetUserConstraintType           () const { return _userConstraintType; }
	int   GetUserConstraintID             () const { return _userConstraintID; }
	void* GetUserConstraintPtr            ()       { return _userConstraintPtr; }
	const JointFeedback* GetJointFeedback () const { return _jointFeedback; }
	JointFeedback*       GetJointFeedback ()       { return _jointFeedback; }
	float GetAppliedImpulse               () const { assert(_needsFeedback); return _appliedImpulse; }
	ConstraintType GetConstraintType      () const { return ConstraintType(ObjectType); }
	float GetDebugDrawSize                ()       { return _debugDrawSize; }

	void EnableFeedback(bool needsFeedback) { _needsFeedback = needsFeedback; }
	bool NeedsFeedback() const { return _needsFeedback; }
	bool IsEnabled() const { return _isEnabled; }
	virtual void GetInfo1(ConstraintInfo1* info) = 0;
	virtual void GetInfo2(ConstraintInfo2* info) = 0;
	virtual float GetParam(int num, int axis = -1) const = 0;


	void SetEnabled(bool enabled) { _isEnabled = enabled; }
	void SetDebugDrawSize               (float debugDrawSize)          { _debugDrawSize = debugDrawSize; }
	void SetJointFeedback               (JointFeedback* jointFeedback) { _jointFeedback = jointFeedback; }
	void SetUserConstraintID             (int uid)                     { _userConstraintID = uid; }
	void SetUserConstraintPtr            (void* ptr)                   { _userConstraintPtr = ptr; }
	void SetUserConstraintType           (int userConstraintType)      { _userConstraintType = userConstraintType; }
	void SetBreakingImpulseThreshold     (float threshold)             { _breakingImpulseThreshold = threshold; }
	void SetInternalAppliedImpulse       (float appliedImpulse)        { _appliedImpulse = appliedImpulse; }
	void SetOverrideSolverIterationsCount(int overrideIterationsCount) { _overrideSolverIterationsCount = overrideIterationsCount; }
	virtual void SetUpSolverConstraint(ConstraintArray& constraintArray, int solverBodyA, int solverBodyB, float timeStep) { (void)constraintArray; (void)solverBodyA; (void)solverBodyB; (void)timeStep; };
	virtual void SetParam(int num, float value, int axis = -1) = 0;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	virtual ~TypedConstraint() {};
	TypedConstraint(ConstraintType type, RigidBody& rigidBodyA);
	TypedConstraint(ConstraintType type, RigidBody& rigidBodyA, RigidBody& rigidBodyB);

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	float GetMotorFactor(float position, float lowLimit, float upperLimit, float velocity, float timeFactor);
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	RigidBody&     _rigidBodyA;
	RigidBody&     _rigidBodyB;
	float          _appliedImpulse;
	float          _debugDrawSize;
	JointFeedback* _jointFeedback;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	TypedConstraint& operator=(TypedConstraint& other) { assert(0); (void)other; return *this;}


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	int _userConstraintType;
	union
	{
		int _userConstraintID;
		void* _userConstraintPtr;
	};
	float _breakingImpulseThreshold;
	bool  _isEnabled;
	bool  _needsFeedback;
	int   _overrideSolverIterationsCount;

};

INLINE float AdjustAngleToLimits(float angleInRadians, float angleLowerLimitInRadians, float angleUpperLimitInRadians)
{
	if (angleLowerLimitInRadians >= angleUpperLimitInRadians)
	{
		return angleInRadians;
	}
	else if (angleInRadians < angleLowerLimitInRadians)
	{
		float diffLo = Abs(gm::NormalizeAngle(angleLowerLimitInRadians - angleInRadians));
		float diffHi = Abs(gm::NormalizeAngle(angleUpperLimitInRadians - angleInRadians));
		return (diffLo < diffHi) ? angleInRadians : (angleInRadians + gm::GM_2PI);
	}
	else if (angleInRadians > angleUpperLimitInRadians)
	{
		float diffHi = Abs(gm::NormalizeAngle(angleInRadians - angleUpperLimitInRadians));
		float diffLo = Abs(gm::NormalizeAngle(angleInRadians - angleLowerLimitInRadians));
		return (diffLo < diffHi) ? (angleInRadians - gm::GM_2PI) : angleInRadians;
	}
	else
	{
		return angleInRadians;
	}
}

class AngularLimit
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Set(float low, float high, float softness = 0.9f, float biasFactor = 0.3f, float relaxationFactor = 1.0f);
	void Test(const float angle);
	void Fit(float& angle);


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	inline float GetSoftness        () const { return _softness; }
	inline float GetBiasFactor      () const { return _biasFactor; }
	inline float GetReraxationFactor() const { return _relaxationFactor; }
	inline float GetCorrection      () const { return _correction; }
	inline float GetSign            () const { return _sign; }
	inline float GetHalfRange       () const { return _halfRange; }
	float GetError() const;
	float GetLow  () const;
	float GetHigh () const;
	inline bool IsLimit() const { return _solveLimit; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	AngularLimit()
	{
		_center           = 0.0f;
		_halfRange        = -1.0f;
		_softness         = 0.9f;
		_biasFactor       = 0.3f;
		_relaxationFactor = 1.0f;
		_correction       = 0.0f;
		_sign             = 0.0f;
		_solveLimit       = false;
	}

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	float _center;
	float _halfRange;
	float _softness;
	float _biasFactor;
	float _relaxationFactor;
	float _correction;
	float _sign;
	bool  _solveLimit;
};


#endif