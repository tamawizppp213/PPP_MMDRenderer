//////////////////////////////////////////////////////////////////////////////////
///             @file   HingeConstraint.hpp
///             @brief  Hinge Constraint
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
#pragma once
#ifndef HINGE_CONSTRAINT_HPP
#define HINGE_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "JacobianEntry.hpp"
#include "TypedConstraint.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class RigidBody;

enum class HingeFlags
{
	CFM_STOP = 1, // cfm : constraint force mixing
	ERP_STOP = 2, // erp: error reduction parameter
	CFM_NORM = 4,
	ERP_NORM = 8
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
class HingeConstraint : public TypedConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void BuildJacobian() override;
	void UpdateRHS(float timeStep);
	void TestLimit(const gm::Transform& transformA, const gm::Transform& transformB);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void GetInfo1(ConstraintInfo1* infomation) override;
	void GetInfo1NonVirtual(ConstraintInfo1* infomation);
	void GetInfo2(ConstraintInfo2* infomation) override;
	void GetInfo2NonVirtual(ConstraintInfo2* infomation, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB);
	void GetInfo2Internal(ConstraintInfo2* infomation, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB);
	void GetInfo2InternalUsingFrameOffset(ConstraintInfo2* infomation, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& angularVelocityA, const gm::Vector3& angularVelocityB);
	float GetLimitSoftness()         const { return _limit.GetSoftness(); }
	float GetLimitBiasFactor()       const { return _limit.GetBiasFactor(); }
	float GetLimitRelaxationFactor() const { return _limit.GetReraxationFactor(); }
	float GetLowerLimit()            const { return _limit.GetLow(); }
	float GetUpperLimit()            const { return _limit.GetHigh(); }
	float GetHingeAngle();
	float GetHingeAngle(const gm::Transform& transformA, const gm::Transform& transformB);
	bool  GetUseFrameOffset()              { return _useOffsetForConstraintFrame; }
	bool  GetUseReferenceFrameA() const { return _useReferenceFrameA; }
	inline int GetSolveLimit()             { return _limit.IsLimit(); }
	inline float GetLimitSign()            { return _limit.GetSign(); }
	inline bool  GetAngularOnly()          { return _angularOnly; }
	inline bool  GetEnableAngularMotor()   { return _enableAngularMotor; }
	inline float GetMotorTargetVelocity()  { return _motorTargetVelocity; }
	inline float GetMaxMotorImpulse()      { return _maxMotorImpulse; }

	const gm::Transform& GetAFrame() const { return _rigidBodyBFrame; }
	const gm::Transform& GetBFrame() const { return _rigidBodyBFrame; }
	const RigidBody& GetRigidBodyA() const { return _rigidBodyA; }
	const RigidBody& GetRigidBodyB() const { return _rigidBodyB; }
	RigidBody&       GetRigidBodyA()       { return _rigidBodyA; }
	RigidBody&       GetRigidBodyB()       { return _rigidBodyB; }
	gm::Transform&   GetFrameOffsetA()     { return _rigidBodyAFrame; }
	gm::Transform&   GetFrameOffsetB()     { return _rigidBodyBFrame; }


	void SetAxis(gm::Vector3& axisInA);
	void SetFrames(const gm::Transform& frameA, const gm::Transform& frameB);
	void SetAngularOnly(bool angularOnly) { _angularOnly = angularOnly; }
	void SetMaxMotorImpulse   (float maxMotorImpulse) { _maxMotorImpulse = maxMotorImpulse; }
	void SetMotorTargetVelocity(float motorTargetVelocity) { _motorTargetVelocity = motorTargetVelocity; }
	void SetMotorTarget       (const gm::Quaternion& quatenionAInB, float dt);
	void SetMotorTarget       (float targetAngle, float dt);
	void SetLimit(float low, float high, float softness = 0.9f, float biasFactor = 0.3f, float relaxationFactor = 1.0f) { _limit.Set(low, high, softness, biasFactor, relaxationFactor); }
	void SetUseFrameOffset    (bool frameOffsetOnOff) { _useOffsetForConstraintFrame = frameOffsetOnOff; }
	void SetUseReferenceFrameA(bool useReferenceFrameA) { _useReferenceFrameA = useReferenceFrameA; }
	void EnableAngularMotor   (bool enableMotor, float targetVelocity, float maxMotorImpulse)
	{
		_enableAngularMotor  = enableMotor;
		_motorTargetVelocity = targetVelocity;
		_maxMotorImpulse     = maxMotorImpulse;
	}
	void EnableMotor(bool enableMotor) { _enableAngularMotor = enableMotor; }
	
	bool HasLimit() const { return _limit.GetHalfRange() > 0.0f; }
	
	void  SetParam(int num, float value, int axis = -1) override;
	float GetParam(int num, int axis = -1)        const override;
	int   GetFlags() const { return _flags; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	HingeConstraint(RigidBody& rigidBodyA, RigidBody& rigidBodyB, const gm::Vector3& pivotInA, const gm::Vector3& pivotInB, const gm::Vector3& axisInA, const gm::Vector3& axisInB, bool useReferenceFrameA = false);
	HingeConstraint(RigidBody& rigidBodyA, const gm::Vector3& pivotInA, const gm::Vector3& axisInA, bool useReferenceFrameA = false);
	HingeConstraint(RigidBody& rigidBodyA, RigidBody& rigidBodyB, const gm::Transform& rigidBodyAFrame, const gm::Transform& rigidBodyBFrame, bool useReferenceFrameA = false);
	HingeConstraint(RigidBody& rigidBodyA, const gm::Transform& rigidBodyAFrame, bool useReferenceFrameA = false);


private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	JacobianEntry _jacobian[3]; // 3 orthogonal linear constraints
	JacobianEntry _jacobianAngular[3]; // 2 orthogonal angular constraints for limit / motor
	gm::Transform _rigidBodyAFrame;
	gm::Transform _rigidBodyBFrame;
	float         _motorTargetVelocity;
	float         _maxMotorImpulse;
	AngularLimit  _limit;

	float _kHinge;
	float _accLimitImpulse;
	float _hingeAngle;
	float _referenceSign;

	bool _angularOnly;
	bool _enableAngularMotor;
	bool _useSolveConstraintObsolete;
	bool _useOffsetForConstraintFrame;
	bool _useReferenceFrameA;

	float _accMotorImpulse;
	int _flags;
	float _normalCFM;
	float _normalERP;
	float _stopCFM;
	float _stopERP;
};

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
__declspec(align(16))
class HingeAccumulatedAngleConstraint : public HingeConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	float GetAccumulatedHingeAngle();
	void SetAccumulatedHingeAngle(float accAngle);
	void GetInfo1(ConstraintInfo1* infomation);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	HingeAccumulatedAngleConstraint(RigidBody& rigidBodyA, RigidBody& rigidBodyB, const gm::Vector3& pivotInA, const gm::Vector3& pivotInB, const gm::Vector3& axisInA, const gm::Vector3& axisInB, bool useReferenceFrameA = false) :
		HingeConstraint(rigidBodyA, rigidBodyB, pivotInA, pivotInB, axisInA, axisInB, useReferenceFrameA)
	{
		_accumulatedAngle = GetHingeAngle();
	}
	HingeAccumulatedAngleConstraint(RigidBody& rigidBodyA, const gm::Vector3& pivotInA, const gm::Vector3& axisInA, bool userReferenceFrameA = false) :
		HingeConstraint(rigidBodyA, pivotInA, axisInA, userReferenceFrameA)
	{
		_accumulatedAngle = GetHingeAngle();
	}
	HingeAccumulatedAngleConstraint(RigidBody& rigidBodyA, RigidBody& rigidBodyB, const gm::Transform& rigidBodyAFrame, const gm::Transform& rigidBodyBFrame, bool useReferenceFrameA = false)
		: HingeConstraint(rigidBodyA, rigidBodyB, rigidBodyAFrame, rigidBodyBFrame, useReferenceFrameA)
	{
		_accumulatedAngle = GetHingeAngle();
	}
	HingeAccumulatedAngleConstraint(RigidBody& rigidBodyA, const gm::Transform& rigidBodyAFrame, bool useReferenceFrameA = false)
		: HingeConstraint(rigidBodyA, rigidBodyAFrame, useReferenceFrameA)
	{
		_accumulatedAngle = GetHingeAngle();
	}

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	float _accumulatedAngle;
};
#endif