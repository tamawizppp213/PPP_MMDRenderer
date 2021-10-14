//////////////////////////////////////////////////////////////////////////////////
///             @file   Hinge2Constraint.hpp
///             @brief  Hinge2 Constraint
///             @author Partially edit by Toide
///             @date   2021_10_02
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
btConeTwistConstraint is Copyright (c) 2007 Starbreeze Studios

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Written by: Marcus Hennix
*/


#pragma once
#ifndef CONE_TWIST_CONSTRAINT_HPP
#define CONE_TWIST_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "JacobianEntry.hpp"
#include "TypedConstraint.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class RigidBody;

enum class ConeTwistFlags
{
	LIN_CFM = 1,
	LIN_ERP = 2,
	ANG_CFM = 4
};
//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     
* 
	Overview:

	btConeTwistConstraint can be used to simulate ragdoll joints (upper arm, leg etc).
	It is a fixed translation, 3 degree-of-freedom (DOF) rotational "joint".
	It divides the 3 rotational DOFs into swing (movement within a cone) and twist.
	Swing is divided into swing1 and swing2 which can have different limits, giving an elliptical shape.
	(Note: the cone's base isn't flat, so this ellipse is "embedded" on the surface of a sphere.)

	In the contraint's frame of reference:
	twist is along the x-axis,
	and swing 1 and 2 are along the z and y axes respectively.

*****************************************************************************/
__declspec(align(16))
class ConeTwistConstraint : public TypedConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void BuildJacobian() override;
	void UpdateRHS(float timeStep);
	virtual void SolveConstraintObsolete(SolverBody* bodyA, SolverBody& bodyB, float timeStep);
	void CalculateAngleInfo();
	void CalculateAngleInfo2(const gm::Transform& transformA, const gm::Transform& transformB, const gm::Matrix3& inverseInertiaWorldA, const gm::Matrix3& inverseInertiaWorldB);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void GetInfo1(ConstraintInfo1* infomation) override;
	void GetInfo1NonVirtual(ConstraintInfo1* infomation);
	void GetInfo2(ConstraintInfo2* infomation) override;
	void GetInfo2NonVirtual(ConstraintInfo2* infomation);
	bool GetAngularOnly(bool angularOnly) { _angularOnly = angularOnly; }
	float GetDamping        () const { return _damping; }
	float GetMaxMotorImpulse() const { return _maxMotorImpulse; }
	float GetFixThreshold   ()       { return _fixThresh; }
	int GetFlags            () const { return _flags; }
	inline int GetSolveTwistLimit () { return _solveTwistLimit; }
	inline int GetSolveSwingLimit () { return _solveSwingLimit; }
	inline float GetTwistLimitSign() { return _twistLimitSign; }
	inline float GetSwingSpan1      () const { return _swingSpan1; }
	inline float GetSwingSpan2      () const { return _swingSpan2; }
	inline float GetTwistSpan       () const { return _twistSpan; }
	inline float GetLimitSoftness   () const { return _limitSoftness; }
	inline float GetBiasFactor      () const { return _biasFactor; }
	inline float GetRelaxationFactor() const { return _relaxationFactor; }
	inline float GetTwistAngle      () const { return _twistAngle; }
	bool IsPastSwingLimit           ()       { return _solveSwingLimit; }
	bool IsMotorEnabled() const { return _motorEnabled; }
	bool IsMaxMotorImpulseNormalized() const { return _normalizeMotorStrength; }

	gm::Vector3 GetPointForAngle(float angleInRadians, float length) const ;
	const RigidBody& GetRigidBodyA() const { return _rigidBodyA; }
	const RigidBody& GetRigidBodyB() const { return _rigidBodyB; }
	const gm::Transform& GetAFrame() const { return _rigidBodyAFrame; }
	const gm::Transform& GetBFrame() const { return _rigidBoydBFrame; }
	const gm::Quaternion& GetMotorTarget() const { return _quaternionTarget; }
	const gm::Transform& GetFrameOffsetA() const { return _rigidBodyAFrame; }
	const gm::Transform& GetFrameOffsetB() const { return _rigidBoydBFrame; }
	float GetParam(int num, int axis = -1) const override;

	void SetAngularOnly(bool angularOnly);
	void SetMotorTarget(const gm::Quaternion& quaternion);
	void SetMotorTargetInConstraintSpace(const gm::Quaternion& quaternion);
	void SetFixThreshold(float fixThreshold) { _fixThresh    = fixThreshold; }
	void SetDamping     (float damping)      { _damping      = damping; }
	void EnableMotor   (bool b)              { _motorEnabled = b; }
	void SetParam(int num, float value, int axis = 1) override;
	void SetFrames(const gm::Transform& frameA, const gm::Transform& frameB);
	void SetMaxMotorImpulse(float maxMotorImpulse)
	{
		_maxMotorImpulse = maxMotorImpulse;
		_normalizeMotorStrength = false;
	}
	void SetMaxMotorImpulseNormalized(float maxMotorImpulse)
	{
		_maxMotorImpulse        = maxMotorImpulse;
		_normalizeMotorStrength = true;
	}
	void SetLimit(int limitIndex, float limitValue)
	{
		switch (limitIndex)
		{
			case 3: { _twistSpan = limitValue ; break; }
			case 4: { _swingSpan2 = limitValue; break; }
			case 5: { _swingSpan1 = limitValue; break; }
			default: { break; }
		};
	}
	float GetLimit(int limitIndex) const
	{
		switch (limitIndex)
		{
			case 3: { return _twistSpan; break; }
			case 4: { return _swingSpan2; break; }
			case 5: { return _swingSpan1; break; }
			default:
			{
				assert(0 && "Invalid limitIndex specified for btConeTwistConstraint");
				return 0.0;
			}
		};
	}
	void SetLimit(float swingSpan1, float swingSpan2, float twistSpan, float softness = 1.0f, float biasFactor = 0.3f, float relaxationFactor = 1.0f)
	{
		_swingSpan1       = swingSpan1;
		_swingSpan2       = swingSpan2;
		_twistSpan        = twistSpan;
		_limitSoftness    = softness;
		_biasFactor       = biasFactor;
		_relaxationFactor = relaxationFactor;
	}
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	ConeTwistConstraint(RigidBody& rigidbodyA, RigidBody& rigidbodyB, const gm::Transform& rigidbodyAFrame, const gm::Transform& rigidbodyBFrame);
	ConeTwistConstraint(RigidBody& rigidbodyA, const gm::Transform& rigidbodyAFrame);
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	void Initialize();
	void ComputeConeLimitInfo(const gm::Quaternion& quaternionCone, float& swingAngle, gm::Vector3& swingAxis, float& swingLimit);
	void ComputeTwistLimitInfo(const gm::Quaternion& quaternionTwist, float& twistAngle, gm::Vector3& twistAxis);
	void AdjustSwingAxisToUseEllipseNormal(gm::Vector3& swingAxis);

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	JacobianEntry _jacobian[3];
	gm::Transform _rigidBodyAFrame;
	gm::Transform _rigidBoydBFrame;
	float _limitSoftness;
	float _biasFactor;
	float _relaxationFactor;
	float _damping;
	float _swingSpan1;
	float _swingSpan2;
	float _twistSpan;
	float _fixThresh;
	float _swingAxis;
	float _twistAxis;
	float _kSwing;
	float _kTwist;
	float _twistLimitSign;
	float _swingCorrection;
	float _twistCorrection;
	float _twistAngle;
	float _accSwingLimitImpulse;
	float _accTwistLimitImpulse;
	bool  _angularOnly;
	bool  _solveTwistLimit;
	bool  _solveSwingLimit;
	bool _useSolveConstraintObsolete;
	float _swingLimitRatio;
	float _twistLimitRatio;
	gm::Vector3    _twistAxisA;
	bool           _motorEnabled;
	bool           _normalizeMotorStrength;
	gm::Quaternion _quaternionTarget;
	float          _maxMotorImpulse;
	gm::Vector3    _accMotorImpulse;
	int   _flags;
	float _linearCFM;
	float _linearERP;
	float _angularCFM;


};

#endif