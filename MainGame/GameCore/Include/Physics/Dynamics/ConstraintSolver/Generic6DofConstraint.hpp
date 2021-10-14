//////////////////////////////////////////////////////////////////////////////////
///             @file   FixedConstraint.hpp
///             @brief  Fixed Constraint
///             @author Partially edit by Toide
///             @date   2021_10_01
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

/// 2009 March: btGeneric6DofConstraint refactored by Roman Ponomarev
/// Added support for generic constraint solver through getInfo1/getInfo2 methods

/*
2007-09-09
btGeneric6DofConstraint Refactored by Francisco Le?n
email: projectileman@yahoo.com
http://gimpact.sf.net
*/

#pragma once
#ifndef GENERIC_6DOF_CONSTRAINT_HPP
#define GENERIC_6DOF_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "JacobianEntry.hpp"
#include "TypedConstraint.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class RigidBody;
enum class Dof6Flags
{
	CFM_NORM = 1,
	CFM_STOP = 2,
	ERP_STOP = 4
};
#define DOF6_FLAGS_AXIS_SHIFT 3  // bits per axis
//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class RotationLimitMotor
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	float SolveAngularLimits(float timeStep, gm::Vector3& axis, float jacobianDiagABInverse, RigidBody* body0, RigidBody* body1);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	float LowJointLimit;
	float HighJointLimit;
	float TargetMotorVelocity;
	float MaxMotorForce;
	float MaxLimitForce;
	float Damping;
	float LimitSoftness; // relaxation factor
	float NormalCFM;     // constraint force mixing factor
	float StopERP;       // Error tolerance factor when joint is at limit
	float StopCFM;       // Constraint force mixing factor
	float Bounce;        // restitution factor
	bool  EnableMotor;

	float CurrentLimitError; // how much is violated this limit
	float CurrentPosition;   // current value of angle
	int   CurrentLimit;      // 0: freem 1 = at low limit , 2=at hi limit
	float AccumulatedImpulse;

	bool IsLimited() const
	{
		if (LowJointLimit > HighJointLimit) { return false; }
		return true;
	}
	bool NeedApplyTorques() const
	{
		if (CurrentLimit == 0 && EnableMotor == false) { return false; }
		return true;
	}
	int TestLimitValue(float testValue);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	RotationLimitMotor()
	{
		AccumulatedImpulse = 0.0f;
		TargetMotorVelocity = 0.0f;
		MaxMotorForce = 6.0f;
		MaxLimitForce = 300.0f;
		LowJointLimit = 1.0f;
		HighJointLimit = -1.0f;
		NormalCFM = 0.0f;
		StopERP = 0.2f;
		StopCFM = 0.0f;
		Bounce = 0.0f;
		Damping = 1.0f;
		LimitSoftness = 0.5f;
		CurrentLimit = 0;
		CurrentLimitError = 0;
		EnableMotor = false;
	}

	RotationLimitMotor(const RotationLimitMotor& motor)
	{
		TargetMotorVelocity = motor.TargetMotorVelocity;
		MaxMotorForce = motor.MaxMotorForce;
		LimitSoftness = motor.LimitSoftness;
		LowJointLimit = motor.LowJointLimit;
		HighJointLimit = motor.HighJointLimit;
		NormalCFM = motor.NormalCFM;
		StopERP = motor.StopERP;
		StopCFM = motor.StopCFM;
		Bounce = motor.Bounce;
		CurrentLimit = motor.CurrentLimit;
		CurrentLimitError = motor.CurrentLimitError;
		EnableMotor = motor.EnableMotor;
	}
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class TranslationalLimitMotor
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	float SolveLinearAxis(
		float timeStep,
		float jacobianDiagABInverse,
		RigidBody& body1, const gm::Vector3& pointInA,
		RigidBody& body2, const gm::Vector3& pointInB,
		int limitIndex, const gm::Vector3& axisNormalOnA,
		const gm::Vector3& anchorPosition
	);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3 LowerLimit;              // the constraint lower limits
	gm::Vector3 UpperLimit;              // the constraint upper limits
	gm::Vector3 AccumulatedImpulse;
	float LimitSoftness;                 // softness for linear limit 
	float Damping;                       // damping for linear limit
	float Restitution;                   // Bounce parameter for linear limit
	gm::Vector3 NormalCFM;                     // Constraint force mixing factor
	gm::Vector3 StopERP;                       // Error tolerance factor when joint is at limit
	gm::Vector3 StopCFM;                       // Constraint force mixing factor when joint is at limit
	bool EnableMotor[3];
	gm::Vector3 TargetMotorVelocity;
	gm::Vector3 MaxMotorForce;
	gm::Vector3 CurrentLimitError;       // How much is violated this limit
	gm::Vector3 CurrentLinearDifference; // Current relatice offset of constraint frames
	int CurrentLimit[3];                 // 0: free, 1 = at lower limit , 2= at upper limit

	inline bool IsLimited(int limitIndex) const { return ((float)UpperLimit.GetElement(limitIndex) >= LowerLimit.GetElement(limitIndex)); }
	inline bool NeedApplyForce(int limitIndex) const
	{
		if (CurrentLimit[limitIndex] == 0 && EnableMotor[limitIndex] == false) { return false; }
		return true;
	}
	int TestLimitValue(int limitIndex, float testValue);
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	TranslationalLimitMotor()
	{
		LowerLimit = gm::Vector3(0.0f, 0.0f, 0.0f);
		UpperLimit = gm::Vector3(0.0f, 0.0f, 0.0f);
		AccumulatedImpulse = gm::Vector3(0.0f, 0.0f, 0.0f);
		NormalCFM = gm::Vector3(0.0f, 0.0f, 0.0f);
		StopERP = gm::Vector3(0.2f, 0.2f, 0.2f);
		StopCFM = gm::Vector3(0.0f, 0.0f, 0.0f);
		LimitSoftness = 0.7f;
		Damping = 1.0f;
		Restitution = 0.5f;
		TargetMotorVelocity = gm::Vector3(0.0f, 0.0f, 0.0f);
		MaxMotorForce = gm::Vector3(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 3; ++i)
		{
			EnableMotor[i] = false;
		}
	}

	TranslationalLimitMotor(const TranslationalLimitMotor& other)
	{
		LowerLimit = other.LowerLimit;
		UpperLimit = other.UpperLimit;
		AccumulatedImpulse = other.AccumulatedImpulse;
		NormalCFM = other.NormalCFM;
		StopERP = other.StopERP;
		StopCFM = other.StopCFM;
		LimitSoftness = other.LimitSoftness;
		Damping = other.Damping;
		Restitution = other.Restitution;
		TargetMotorVelocity = other.TargetMotorVelocity;
		MaxMotorForce = other.MaxMotorForce;

		for (int i = 0; i < 3; ++i)
		{
			EnableMotor[i] = other.EnableMotor[i];
		}
	}
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

/****************************************************************************
*				  			Generic6DofConstraint
*************************************************************************//**
*  @class     Generic6DofConstraint
*  @brief     Generic6DofConstraint
*****************************************************************************/
__declspec(align(16))
class Generic6DofConstraint : public TypedConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void CalculateTransforms(const gm::Transform& transformA, const gm::Transform& transformB);
	void CalculateTransforms(); /* Calcs the global transform for the joint offset for body A an B, and also calcs the agle differences between the bodies. Generic6DofConstraint.getCalculatedTransformA , btGeneric6DofConstraint.getCalculatedTransformB, btGeneric6DofConstraint.calculateAngleInfo*/
	void BuildJacobian() override;
	void UpdateRHS(float timeStep);
	virtual void CalculateAnchorPosition();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool UseSolveConstraintObsolete;  //for backwards compatibility during the transition to 'getInfo/getInfo2'
	const gm::Transform& GetCalculatedTransformA() const { return _calculatedTransformA; }
	const gm::Transform& GetCalculatedTransformB() const { return _calculatedTransformB; }
	const gm::Transform& GetFrameOffsetA() const { return _frameInA; }
	const gm::Transform& GetFrameOffsetB() const { return _frameInB; }
	gm::Transform& GetFrameOffsetA() { return _frameInA; }
	gm::Transform& GetFrameOffsetB() { return _frameInB; }

	gm::Vector3 GetAxis(int axisIndex) const;
	float       GetAngle(int axisIndex) const;
	float GetRelativePivotPosition(int axisIndex) const;
	void  GetInfo1(ConstraintInfo1* infomation) override;
	void  GetInfo1NonVirtual(ConstraintInfo1* infomation);
	void  GetInfo2(ConstraintInfo2* infomation) override;
	void  GetInfo2NonVirtual(ConstraintInfo2* infomation, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& linearVelocityA, const gm::Vector3& linearVelocityB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB);
	void  GetLinearLowerLimit(gm::Vector3& linearLower) const { linearLower = _linearLimits.LowerLimit; }
	void  GetLinearUpperLimit(gm::Vector3& linearUpper) const { linearUpper = _linearLimits.UpperLimit; }
	void  GetAngularLowerLimit(gm::Vector3& angularLower) const { for (int i = 0; i < 3; ++i) { angularLower.SetElement(i, _angularLimits[i].LowJointLimit); } }
	void  GetAngularUpperLimit(gm::Vector3& angularUpper) const { for (int i = 0; i < 3; ++i) { angularUpper.SetElement(i, _angularLimits[i].HighJointLimit); } }
	bool GetUseFrameOffset() const { return _useOffsetForConstraintFrame; }
	bool GetUseLinearReferenceFrameA() const { return _useLinearReferenceFrameA; }
	RotationLimitMotor* GetRotatinalLimitMotor(int index) { return &_angularLimits[index]; }
	TranslationalLimitMotor* GetTranslationalLimitMotor() { return &_linearLimits; }
	int GetLimitMotorInfo2(RotationLimitMotor* motor, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& linearVelocityA, const gm::Vector3& linearVelocityB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB,
		ConstraintInfo2* infomation, int row, gm::Vector3& axis1, int rotational, int rotAllowed = false);

	void SetFrames(const gm::Transform& frameA, const gm::Transform& frameB);
	void SetLinearLowerLimit(const gm::Vector3& linearLower) { _linearLimits.LowerLimit = linearLower; }
	void SetLinearUpperLimit(const gm::Vector3& linearUpper) { _linearLimits.UpperLimit = linearUpper; }
	void SetUseFrameOffset(bool frameOffsetObOff) { _useOffsetForConstraintFrame = frameOffsetObOff; }
	void SetUseLinearReferenceFrameA(bool linearReferenceFrameA) { _useLinearReferenceFrameA = linearReferenceFrameA; }
	void SetAngularLowerLimit(const gm::Vector3& angularLower)
	{
		for (int i = 0; i < 3; ++i) { _angularLimits[i].LowJointLimit = gm::NormalizeAngle(angularLower.GetElement(i)); }
	} 
	void SetAngularUpperLimit(const gm::Vector3& angularUpper)
	{
		for (int i = 0; i < 3; ++i) { _angularLimits[i].HighJointLimit = gm::NormalizeAngle(angularUpper.GetElement(i)); }
	}
	void SetLimit(int axis, float low, float high)
	{
		if (axis < 3)
		{
			_linearLimits.LowerLimit.SetElement(axis, low);
			_linearLimits.UpperLimit.SetElement(axis, high);
		}
		else
		{
			low = gm::NormalizeAngle(low);
			high = gm::NormalizeAngle(high);
			_angularLimits[axis - 3].LowJointLimit = low;
			_angularLimits[axis - 3].HighJointLimit = high;
		}
	}
	bool IsLimited(int limitIndex) const
	{
		if (limitIndex < 3) { return _linearLimits.IsLimited(limitIndex); }
		return _angularLimits[limitIndex - 3].IsLimited();
	}
	void SetParam(int num, float value, int axis = -1) override;
	float GetParam(int num, int axis = -1) const override;
	virtual void SetAxis(const gm::Vector3& axis1, const gm::Vector3& axis2);
	virtual int  GetFlags() const {return _flags;}
	bool TestAngularLimitMotor(int axisIndex);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	Generic6DofConstraint(RigidBody& rigidbodyA, RigidBody& rigidbodyB, const gm::Transform& frameInA, const gm::Transform& frameInB, bool useLinearReferenceFrameA);
	Generic6DofConstraint(RigidBody& rigidbodyB, const gm::Transform& frameInB, bool useLinearReferenceFrameB);

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	void BuildLinearJacobian(JacobianEntry& jacobianLinear, const gm::Vector3& normalWorld, const gm::Vector3& pivotAInWorld, const gm::Vector3& pivotBInWorld);
	void BuildAngularJacobian(JacobianEntry& jacobianAngular, const gm::Vector3& jointAxisWorld);
	void CalculateLinearInfo(); // test linear limits
	void CalculateAngleInfo();  // calculates the euler angles between the two bodies
	Generic6DofConstraint& operator=(Generic6DofConstraint& other)
	{
		assert(0); (void)other; return *this;
	}

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	int SetAngularLimits(ConstraintInfo2* infomation, int rowOffset, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& linearVelocityA, const gm::Vector3& linearVelocityB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB);
	int SetLinearLimits(ConstraintInfo2* infomation, int row, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& linearVelocityA, const gm::Vector3& linearVelocityB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB);

	gm::Transform           _frameInA;                  //relative frame,  the constraint space w.r.t body A
	gm::Transform           _frameInB;
	JacobianEntry           _jacobianLinear[3];
	JacobianEntry           _jacobianAngular[3];
	TranslationalLimitMotor _linearLimits;
	RotationLimitMotor      _angularLimits[3];

	float                   _timeStep;
	gm::Transform           _calculatedTransformA;
	gm::Transform           _calculatedTransformB;
	gm::Vector3             _calculatedAxisAngleDiff;
	gm::Vector3             _calculatedAxis[3];
	gm::Vector3             _calculatedLinearDiff;
	float                   _factA;
	float                   _factB;
	bool                    _hasStaticBody;
	gm::Vector3             _anchorPosition;
	bool                    _useLinearReferenceFrameA;
	bool                    _useOffsetForConstraintFrame;
	int                     _flags;
};

#endif