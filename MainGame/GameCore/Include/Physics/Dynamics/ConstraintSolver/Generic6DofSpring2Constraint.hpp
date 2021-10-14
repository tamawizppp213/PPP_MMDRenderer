//////////////////////////////////////////////////////////////////////////////////
///             @file   Generic6DofSpring2Constraint.hpp
///             @brief  Generic6DofSpring2Constraint
///             @author Partially edit by Toide
///             @date   2021_10_02
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

/*
2014 May: btGeneric6DofSpring2Constraint is created from the original (2.82.2712) btGeneric6DofConstraint by Gabor Puhr and Tamas Umenhoffer
Pros:
- Much more accurate and stable in a lot of situation. (Especially when a sleeping chain of RBs connected with 6dof2 is pulled)
- Stable and accurate spring with minimal energy loss that works with all of the solvers. (latter is not true for the original 6dof spring)
- Servo motor functionality
- Much more accurate bouncing. 0 really means zero bouncing (not true for the original 6odf) and there is only a minimal energy loss when the value is 1 (because of the solvers' precision)
- Rotation order for the Euler system can be set. (One axis' freedom is still limited to pi/2)

Cons:
- It is slower than the original 6dof. There is no exact ratio, but half speed is a good estimation.
- At bouncing the correct velocity is calculated, but not the correct position. (it is because of the solver can correct position or velocity, but not both.)
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
#ifndef GENERIC_6DOF_SPRING_2_CONSTRAINT_HPP
#define GENERIC_6DOF_SPRING_2_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "JacobianEntry.hpp"
#include "TypedConstraint.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define DOF6_FLAGS_AXIS_SHIFT2 4  // bits per axis
class RigidBody;

enum class RotateOrder
{
	XYZ = 0,
	XZY,
	YXZ,
	YZX,
	ZXY,
	ZYX
};

enum class Dof6Flags
{
	CFM_STOP2 = 1,
	ERP_STOP2 = 2,
	CFM_MOTO2 = 4,
	ERP_MOTO2 = 8,
	USE_INFINITE_ERROR = (1 << 16)
};
//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class RotationalLimitMotor2
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	float LowJointLimit;
	float HighJointLimit;
	float Bounce;
	float StopERP;
	float StopCFM;
	float MotorERP;
	float MotorCFM;
	bool  EnableMotor;
	float TargetMotorVelocity;
	float MaxMotorForce;
	bool  ServoMotor;
	float ServoTarget;
	bool  EnableSpring;
	float SpringStiffness;
	bool  SpringStiffnessLimited;
	float SpringDamping;
	bool  SpringDampingLimited;
	float EquilibriumPoint;
	float CurrentLimitError;
	float CurrentLimitErrorHi;
	float CurrentPosition;
	int   CurrentLimit;

	bool IsLimited()
	{
		if (LowJointLimit > HighJointLimit) return false;
		return true;
	}

	void TestLimitValue(float testValue);
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	RotationalLimitMotor2()
	{
		LowJointLimit          = 1.0f;
		HighJointLimit         = -1.0f;
		Bounce                 = 0.0f;
		StopERP                = 0.2f;
		StopCFM                = 0.f;
		MotorERP               = 0.9f;
		MotorCFM               = 0.f;
		EnableMotor            = false;
		TargetMotorVelocity    = 0;
		MaxMotorForce          = 6.0f;
		ServoMotor             = false;
		ServoTarget            = 0;
		EnableSpring           = false;
		SpringStiffness        = 0;
		SpringStiffnessLimited = false;
		SpringDamping          = 0;
		SpringDampingLimited   = false;
		EquilibriumPoint       = 0;

		CurrentLimitError      = 0;
		CurrentLimitErrorHi    = 0;
		CurrentPosition        = 0;
		CurrentLimit           = 0;
	}

	RotationalLimitMotor2(const RotationalLimitMotor2& limot)
	{
		LowJointLimit          = limot.LowJointLimit;
		HighJointLimit         = limot.HighJointLimit;
		Bounce                 = limot.Bounce;
		StopERP                = limot.StopERP;
		StopCFM                = limot.StopCFM;
		MotorERP               = limot.MotorERP;
		MotorCFM               = limot.MotorCFM;
		EnableMotor            = limot.EnableMotor;
		TargetMotorVelocity    = limot.TargetMotorVelocity;
		MaxMotorForce          = limot.MaxMotorForce;
		ServoMotor             = limot.ServoMotor;
		ServoTarget            = limot.ServoTarget;
		EnableSpring           = limot.EnableSpring;
		SpringStiffness        = limot.SpringStiffness;
		SpringStiffnessLimited = limot.SpringStiffnessLimited;
		SpringDamping          = limot.SpringDamping;
		SpringDampingLimited   = limot.SpringDampingLimited;
		EquilibriumPoint       = limot.EquilibriumPoint;

		CurrentLimitError      = limot.CurrentLimitError;
		CurrentLimitErrorHi    = limot.CurrentLimitErrorHi;
		CurrentPosition        = limot.CurrentPosition;
		CurrentLimit           = limot.CurrentLimit;
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
class TranslationalLimitMotor2
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	inline bool IsLimited(int limitIndex)
	{
		return ((float)UpperJointLimit.GetElement(limitIndex) >= LowerJointLimit.GetElement(limitIndex));
	}

	void TestLimitValue(int limitIndex, float testValue);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3 LowerJointLimit;
	gm::Vector3 UpperJointLimit;
	gm::Vector3 Bounce;
	gm::Vector3 StopERP;
	gm::Vector3 StopCFM;
	gm::Vector3 MotorERP;
	gm::Vector3 MotorCFM;
	bool EnableMotor[3];
	bool ServoMotor[3];
	bool EnableSpring[3];
	gm::Vector3 ServoTarget;
	gm::Vector3 SpringStiffness;
	bool SpringStiffnessLimited[3];
	gm::Vector3 SpringDamping;
	bool SpringDampingLimited[3];
	gm::Vector3 EquilibriumPoint;
	gm::Vector3 TargetVelocity;
	gm::Vector3 MaxMotorForce;

	gm::Vector3 CurrentLimitError;
	gm::Vector3 CurrentLimitErrorHi;
	gm::Vector3 CurrentLinearDiff;
	int CurrentLimit[3];
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	TranslationalLimitMotor2()
	{
		LowerJointLimit     = gm::Vector3(0.0f, 0.0f, 0.0f);
		UpperJointLimit     = gm::Vector3(0.0f, 0.0f, 0.0f);
		Bounce              = gm::Vector3(0.0f, 0.0f, 0.0f);
		StopERP             = gm::Vector3(0.2f, 0.2f, 0.2f);
		StopCFM             = gm::Vector3(0.0f, 0.0f, 0.0f);
		MotorERP            = gm::Vector3(0.9f, 0.9f, 0.9f);
		MotorCFM            = gm::Vector3(0.0f, 0.0f, 0.0f);
		CurrentLimitError   = gm::Vector3(0.0f, 0.0f, 0.0f);
		CurrentLimitErrorHi = gm::Vector3(0.0f, 0.0f, 0.0f);
		CurrentLinearDiff   = gm::Vector3(0.0f, 0.0f, 0.0f);
		ServoTarget         = gm::Vector3(0.0f, 0.0f, 0.0f);
		SpringStiffness     = gm::Vector3(0.0f, 0.0f, 0.0f);
		SpringDamping       = gm::Vector3(0.0f, 0.0f, 0.0f);
		EquilibriumPoint    = gm::Vector3(0.0f, 0.0f, 0.0f);
		TargetVelocity      = gm::Vector3(0.0f, 0.0f, 0.0f);
		MaxMotorForce       = gm::Vector3(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 3; ++i)
		{
			EnableMotor[i]  = false;
			ServoMotor[i]   = false;
			EnableSpring[i] = false;
			SpringStiffnessLimited[i] = false;
			SpringDampingLimited[i]   = false;
			CurrentLimit[i] = 0;
		}
	}
	TranslationalLimitMotor2(const TranslationalLimitMotor2& other)
	{
		LowerJointLimit     = other.LowerJointLimit;
		UpperJointLimit     = other.UpperJointLimit;
		Bounce              = other.Bounce;
		StopERP             = other.StopERP;
		StopCFM             = other.StopCFM;
		MotorERP            = other.MotorERP;
		MotorCFM            = other.MotorCFM;
		CurrentLimitError   = other.CurrentLimitError;
		CurrentLimitErrorHi = other.CurrentLimitErrorHi;
		CurrentLinearDiff   = other.CurrentLinearDiff;
		ServoTarget         = other.ServoTarget;
		SpringStiffness     = other.SpringStiffness;
		SpringDamping       = other.SpringDamping;
		EquilibriumPoint    = other.EquilibriumPoint;
		TargetVelocity      = other.TargetVelocity;
		MaxMotorForce       = other.MaxMotorForce;

		for (int i = 0; i < 3; ++i)
		{
			EnableMotor[i]  = other.EnableMotor[i];
			ServoMotor[i]   = other.ServoMotor[i];
			EnableSpring[i] = other.EnableSpring[i];
			SpringStiffnessLimited[i] = other.SpringStiffnessLimited[i];
			SpringDampingLimited[i]   = other.SpringDampingLimited[i];
			CurrentLimit[i] = other.CurrentLimit[i];
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
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
__declspec(align(16))
class Generic6DofSpring2Constraint : TypedConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void BuildJacobian() override {};
	void CalculateTransforms(const gm::Transform& transformA, const gm::Transform& transformB);
	void CalculateTransforms();

	static float GetMatrixElement(const gm::Matrix3& matrix, int index);
	static bool MatrixToEulerXYZ(const gm::Matrix3& matrix, gm::Vector3& xyz);
	static bool MatrixToEulerXZY(const gm::Matrix3& matrix, gm::Vector3& xyz);
	static bool MatrixToEulerYXZ(const gm::Matrix3& matrix, gm::Vector3& xyz);
	static bool MatrixToEulerYZX(const gm::Matrix3& matrix, gm::Vector3& xyz);
	static bool MatrixToEulerZXY(const gm::Matrix3& matrix, gm::Vector3& xyz);
	static bool MatrixToEulerZYX(const gm::Matrix3& matrix, gm::Vector3& xyz);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void GetInfo1(ConstraintInfo1* infomation) override;
	void GetInfo2(ConstraintInfo2* infomation) override;
	void GetLinearLowerLimit(gm::Vector3& linearLower) { linearLower = _linearLimits.LowerJointLimit; }
	void GetLinearUpperLimit(gm::Vector3& linearUpper) { linearUpper = _linearLimits.UpperJointLimit; }
	void GetAngularLowerLimit(gm::Vector3& angularLower)
	{
		for (int i = 0; i < 3; ++i)
		{
			angularLower.SetElement(i, _angularLimits[i].LowJointLimit);
		}
	}
	void GetAngularLowerLimitReserved(gm::Vector3& angularLower)
	{
		for (int i = 0; i < 3; ++i)
		{
			angularLower.SetElement(i, -_angularLimits[i].HighJointLimit);
		}
	}
	void GetAngularUpperLimit(gm::Vector3& angularUpper)
	{
		for (int i = 0; i < 3; ++i)
		{
			angularUpper.SetElement(i, _angularLimits[i].HighJointLimit);
		}
	}
	void GetAngularUpperLimitReserved(gm::Vector3& angularUpper)
	{
		for (int i = 0; i < 3; ++i)
		{
			angularUpper.SetElement(i, -_angularLimits[i].LowJointLimit);
		}
	}

	gm::Transform& GetFrameOffsetA() { return _frameInA; }
	gm::Transform& GetFrameOffsetB() { return _frameInB; }
	gm::Vector3 GetAxis(int axisIndex) const { return _calculatedAxis[axisIndex]; }
	float GetAngle(int axisIndex) const { return _calculatedAxisAngleDiff.GetElement(axisIndex); }
	float GetRelativePivotPosition(int axisIndex) const { return _calculatedLinearDiff.GetElement(axisIndex); }
	const gm::Transform&      GetCalculatedTransformA() const { return _calculatedTransformA; }
	const gm::Transform&      GetCalculatedTransformB() const { return _calculatedTransformB; }
	const gm::Transform&      GetFrameOffsetA        () const { return _frameInA; }
	const gm::Transform&      GetFrameOffsetB        () const { return _frameInB; }
	RotationalLimitMotor2*    GetRotationalLimitMotor(int index) { return &_angularLimits[index]; }
	TranslationalLimitMotor2* GetTranslationalLimitMotor()       { return &_linearLimits; }
	RotateOrder GetRotationOrder() { return _rotateOrder; }

	void SetFrames(const gm::Transform& frameA, const gm::Transform& frameB);
	void SetRotationOrder(RotateOrder order) { _rotateOrder = order; }
	void SetAxis(const gm::Vector3& axis1, const gm::Vector3& axis2);
	void SetBounce(int index, float bounce);
	void EnableMotor(int index, bool onOff);
	void SetServo(int index,  bool onOff);
	void SetTargetMotorVelocity(int index, float velocity);
	void SetServoTarget        (int index, float target);
	void SetMaxMotorForce      (int index, float force);
	void EnableSpring(int index, bool onOff);
	void SetStiffness(int index, float stiffness, bool limitIfNeeded = true);
	void SetDamping(int index, float damping, bool limitIfNeeded = true);
	void SetEquilibriumPoint();
	void SetEquilibriumPoint(int index);
	void SetEquilibriumPoint(int index, float value);
	void SetLinearLowerLimit(const gm::Vector3& linearLower) { _linearLimits.LowerJointLimit = linearLower; }
	void SetLinearUpperLimit(const gm::Vector3& linearUpper) { _linearLimits.UpperJointLimit = linearUpper; }
	void SetAngularLowerLimit(const gm::Vector3& angularLower)
	{
		for (int i = 0; i < 3; ++i)
		{
			_angularLimits[i].LowJointLimit = gm::NormalizeAngle(angularLower.GetElement(i));
		}
	}
	void SetAngularLowerLimitReserved(const gm::Vector3& angularLower)
	{
		for (int i = 0; i < 3; ++i)
		{
			_angularLimits[i].HighJointLimit = gm::NormalizeAngle(-angularLower.GetElement(i));
		}
	}
	void SetAngularUpperLimit(const gm::Vector3& angularUpper)
	{
		for (int i = 0; i < 3; ++i)
		{
			_angularLimits[i].HighJointLimit = gm::NormalizeAngle(angularUpper.GetElement(i));
		}
	}
	void SetAngularUpperLimit(const gm::Vector3& angularUpper)
	{
		for (int i = 0; i < 3; ++i)
		{
			_angularLimits[i].LowJointLimit = gm::NormalizeAngle(-angularUpper.GetElement(i));
		}
	}
	void SetLimit(int axis, float low, float high)
	{
		if (axis < 3)
		{
			_linearLimits.LowerJointLimit.SetElement(axis, low);
			_linearLimits.UpperJointLimit.SetElement(axis, high);
		}
		else
		{
			low = gm::NormalizeAngle(low);
			high = gm::NormalizeAngle(high);
			_angularLimits[axis - 3].LowJointLimit  = low;
			_angularLimits[axis - 3].HighJointLimit = high;
		}
	}
	void SetLimitReserved(int axis, float low, float high)
	{
		if (axis < 3)
		{
			_linearLimits.LowerJointLimit.SetElement(axis, low);
			_linearLimits.UpperJointLimit.SetElement(axis, high);
		}
		else
		{
			low = gm::NormalizeAngle(low);
			high = gm::NormalizeAngle(high);
			_angularLimits[axis - 3].HighJointLimit = -low;
			_angularLimits[axis - 3].LowJointLimit = -high;
		}
	}
	bool IsLimited(int limitIndex)
	{
		if (limitIndex < 3)
		{
			return _linearLimits.IsLimited(limitIndex);
		}
		return _angularLimits[limitIndex - 3].IsLimited();
	}
	void SetParam(int num, float value, int axis = -1) override;
	float GetParam(int num, int axis = -1) const override;


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	Generic6DofSpring2Constraint(RigidBody& rigidbodyA, RigidBody& rigidbodyB, const gm::Transform& frameInA, const gm::Transform& frameInB, RotateOrder rotateOrder = RotateOrder::XYZ);
	Generic6DofSpring2Constraint(RigidBody& rigidbodyB, const gm::Transform& frameInB, RotateOrder rotateOrder = RotateOrder::XYZ);

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	Generic6DofSpring2Constraint& operator=(const Generic6DofSpring2Constraint&) { assert(0); return *this; }
	void CalculateLinearInfo();
	void CalculateAngleInfo();
	void TestAngularLimitMotor(int axisIndex);
	void CalculateJacobian(RotationalLimitMotor2* motor, const gm::Transform& transformA, const gm::Transform& transformB, ConstraintInfo2* infomation, int slow, gm::Vector3& axis1, int rotational, int rotAllowed);
	int GetLimitMotorInfo2(RotationalLimitMotor2* motor, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& linearVelocityA, const gm::Vector3& linearVelocityB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB, ConstraintInfo2* infomation, int row, gm::Vector3& axis1, int rotational, int rotAllowed = false);

	
	int SetAngularLimits(ConstraintInfo2* infomation, int rowOffset, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& linearVelocityA, const gm::Vector3& linearVelocityB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB);
	int SetLinearLimits(ConstraintInfo2* infomation, int row, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& linearVelocityA, const gm::Vector3& linearVelocityB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB);


	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::Transform _frameInA;
	gm::Transform _frameInB;
	JacobianEntry _jacobianLinear[3];
	JacobianEntry _jacobianAngular[3];
	TranslationalLimitMotor2 _linearLimits;
	RotationalLimitMotor2    _angularLimits[3];
	RotateOrder              _rotateOrder;

	gm::Transform _calculatedTransformA;
	gm::Transform _calculatedTransformB;
	gm::Vector3 _calculatedAxisAngleDiff;
	gm::Vector3 _calculatedAxis[3];
	gm::Vector3 _calculatedLinearDiff;
	float _factA;
	float _factB;
	bool _hasStaticBody;
	int  _flags;
};

#endif