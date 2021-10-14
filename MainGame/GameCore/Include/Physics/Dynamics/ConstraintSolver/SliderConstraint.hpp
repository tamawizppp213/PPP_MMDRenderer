//////////////////////////////////////////////////////////////////////////////////
///             @file   SliderConstraint.hpp
///             @brief  Slider Constraint
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

/*
Added by Roman Ponomarev (rponom@gmail.com)
April 04, 2008

TODO:
 - add clamping od accumulated impulse to improve stability
 - add conversion for ODE constraint solver
*/

#pragma once
#ifndef SLIDER_CONSTRAINT_HPP
#define SLIDER_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "JacobianEntry.hpp"
#include "TypedConstraint.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define SLIDER_CONSTRAINT_DEF_SOFTNESS    (1.0f)
#define SLIDER_CONSTRAINT_DEF_DAMPING     (1.0f)
#define SLIDER_CONSTRAINT_DEF_RESTITUTION (0.7f)
#define SLIDER_CONSTRAINT_DEF_CFM         (0.7f)

// softness, restitution and damping for different cases
// DirLin - moving inside linear limits
// LimLin - hitting linear limit
// DirAng - moving inside angular limits
// LimAng - hitting angular limit
// OrthoLin, OrthoAng - against constraint axis
enum class SliderFlags
{
	CFM_DIRLIN = (1 << 0),
	ERP_DIRLIN = (1 << 1),
	CFM_DIRANG = (1 << 2),
	ERP_DIRANG = (1 << 3),
	CFM_ORTLIN = (1 << 4),
	ERP_ORTLIN = (1 << 5),
	CFM_ORTANG = (1 << 6),
	ERP_ORTANG = (1 << 7),
	CFM_LIMLIN = (1 << 8),
	ERP_LIMLIN = (1 << 9),
	CFM_LIMANG = (1 << 10),
	ERP_LIMANG = (1 << 11)
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
__declspec(align(16))
class SliderConstraint : public TypedConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void GetInfo1(ConstraintInfo1* infomation) override;
	void GetInfo1NonVirtual(ConstraintInfo1* infomation);
	void GetInfo2(ConstraintInfo2* infomation);
	void GetInfo2NonVirtual(ConstraintInfo2* infomation, const gm::Transform& transformA, const gm::Transform& transformB, const gm::Vector3& linearVelocityA, const gm::Vector3& linearVelocityB, float rigidbodyAInverseMass, float rigidbodyBInverseMass);
	const RigidBody& GetRigidBodyA() const { return _rigidBodyA; }
	const RigidBody& GetRigidBodyB() const { return _rigidBodyB; }
	const gm::Transform& GetCalculatedTransformA() const { return _calculatedTransformA; }
	const gm::Transform& GetCalculatedTransformB() const { return _calculatedTransformB; }
	const gm::Transform& GetFrameOffsetA() { return _frameInA; }
	const gm::Transform& GetFrameOffsetB() { return _frameInB; }
	float GetLowerLinLimit() { return _lowerLinearLimit; }
	void SetLowerLinLimit(float lowerLimit) { _lowerLinearLimit = lowerLimit; }
	float GetUpperLinLimit() { return _upperLinearLimit; }
	void SetUpperLinLimit(float upperLimit) { _upperLinearLimit = upperLimit; }
	float GetLowerAngLimit() { return _lowerAngularLimit; }
	void SetLowerAngLimit(float lowerLimit) { _lowerAngularLimit = gm::NormalizeAngle(lowerLimit); }
	float GetUpperAngLimit() { return _upperAngularLimit; }
	void SetUpperAngLimit(float upperLimit) { _upperAngularLimit = gm::NormalizeAngle(upperLimit); }
	bool GetUseLinearReferenceFrameA() { return _useLinearReferenceFrameA; }
	float GetSoftnessDirLin()          { return _softnessDirLinear; }
	float GetRestitutionDirLin()       { return _restitutionDirLinear; }
	float GetDampingDirLin()           { return _dampingDirLinear; }
	float GetSoftnessDirAng()          { return _softnessDirAngular; }
	float GetRestitutionDirAng()       { return _restitutionDirAngular; }
	float GetDampingDirAng()           { return _dampingDirAngular; }
	float GetSoftnessLimLin()          { return _softnessLimLinear; }
	float GetRestitutionLimLin()       { return _restitutionLimLinear; }
	float GetDampingLimLin()           { return _dampingLimLinear; }
	float GetSoftnessLimAng()          { return _softnessLimAngular; }
	float GetRestitutionLimAng()       { return _restitutionLimAngular; }
	float GetDampingLimAng()           { return _dampingLimAngular; }
	float GetSoftnessOrthoLin()        { return _softnessOrthoLinear; }
	float GetRestitutionOrthoLin()     { return _restitutionOrthoLinear; }
	float GetDampingOrthoLin()         { return _dampingOrthoLinear; }
	float GetSoftnessOrthoAng()        { return _softnessOrthoAngular; }
	float GetRestitutionOrthoAng()     { return _restitutionOrthoAngular; }
	float GetDampingOrthoAng()         { return _dampingOrthoAngular; }
	void SetSoftnessDirLin      (float softnessDirLin)      { _softnessDirLinear       = softnessDirLin; }
	void SetRestitutionDirLin   (float restitutionDirLin)   { _restitutionDirLinear    = restitutionDirLin; }
	void SetDampingDirLin       (float dampingDirLin)       { _dampingDirLinear        = dampingDirLin; }
	void SetSoftnessDirAng      (float softnessDirAng)      { _softnessDirAngular      = softnessDirAng; }
	void SetRestitutionDirAng   (float restitutionDirAng)   { _restitutionDirAngular   = restitutionDirAng; }
	void SetDampingDirAng       (float dampingDirAng)       { _dampingDirAngular       = dampingDirAng; }
	void SetSoftnessLimLin      (float softnessLimLin)      { _softnessLimLinear       = softnessLimLin; }
	void SetRestitutionLimLin   (float restitutionLimLin)   { _restitutionLimLinear    = restitutionLimLin; }
	void SetDampingLimLin       (float dampingLimLin)       { _dampingLimLinear        = dampingLimLin; }
	void SetSoftnessLimAng      (float softnessLimAng)      { _softnessLimAngular      = softnessLimAng; }
	void SetRestitutionLimAng   (float restitutionLimAng)   { _restitutionLimAngular   = restitutionLimAng; }
	void SetDampingLimAng       (float dampingLimAng)       { _dampingLimAngular       = dampingLimAng; }
	void SetSoftnessOrthoLin    (float softnessOrthoLin)    { _softnessOrthoLinear     = softnessOrthoLin; }
	void SetRestitutionOrthoLin (float restitutionOrthoLin) { _restitutionOrthoLinear  = restitutionOrthoLin; }
	void SetDampingOrthoLin     (float dampingOrthoLin)     { _dampingOrthoLinear      = dampingOrthoLin; }
	void SetSoftnessOrthoAng    (float softnessOrthoAng)    { _softnessOrthoAngular    = softnessOrthoAng; }
	void SetRestitutionOrthoAng (float restitutionOrthoAng) { _restitutionOrthoAngular = restitutionOrthoAng; }
	void SetDampingOrthoAng     (float dampingOrthoAng)     { _dampingOrthoAngular     = dampingOrthoAng; }
	void SetPoweredLinMotor     (bool onOff) { _poweredLinearMotor = onOff; }
	bool GetPoweredLinMotor     ()           { return _poweredLinearMotor; }
	void SetTargetLinMotorVelocity(float targetLinMotorVelocity) { _targetLinearMotorVelocity = targetLinMotorVelocity; }
	float GetTargetLinMotorVelocity() { return _targetLinearMotorVelocity; }
	void  SetMaxLinMotorForce(float maxLinMotorForce) { _maxLinearMotorForce = maxLinMotorForce; }
	float GetMaxLinMotorForce() { return _maxLinearMotorForce; }
	void  SetPoweredAngMotor(bool onOff) { _poweredAngularMotor = onOff; }
	bool  GetPoweredAngMotor() { return _poweredAngularMotor; }
	void  SetTargetAngMotorVelocity(float targetAngMotorVelocity) { _targetAngularMotorVelocity = targetAngMotorVelocity; }
	float GetTargetAngMotorVelocity() { return _targetAngularMotorVelocity; }
	void  SetMaxAngMotorForce(float maxAngMotorForce) { _maxAngularMotorForce = maxAngMotorForce; }
	float GetMaxAngMotorForce() { return _maxAngularMotorForce; }

	float GetLinearPos() const { return _linearPosition; }
	float GetAngularPos() const { return _angularPosition; }

	// access for ODE solver
	bool GetSolveLinLimit() { return _solveLinearLim; }
	float GetLinDepth() { return _depth.GetX(); }
	bool GetSolveAngLimit() { return _solveAngularLim; }
	float GetAngDepth() { return _angularDepth; }
	// shared code used by ODE solver
	void CalculateTransforms(const gm::Transform& transA, const gm::Transform& transB);
	void TestLinLimits();
	void TestAngLimits();
	// access for PE Solver
	gm::Vector3 GetAncorInA();
	gm::Vector3 GetAncorInB();
	// access for UseFrameOffset
	bool GetUseFrameOffset() { return _useOffsetForConstraintFrame; }
	void SetUseFrameOffset(bool frameOffsetOnOff) { _useOffsetForConstraintFrame = frameOffsetOnOff; }
	void SetFrames(const gm::Transform& frameA, const gm::Transform& frameB)
	{
		_frameInA = frameA;
		_frameInB = frameB;
		CalculateTransforms(_rigidBodyA.GetCenterOfMassTransform(), _rigidBodyB.GetCenterOfMassTransform());
		BuildJacobian();
	}
	void SetParam(int num, float value, int axis = -1);
	float GetParam(int num, int axis = -1) const;
	int GetFlags() const { return _flags; }
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	SliderConstraint(RigidBody& rigidbodyA, RigidBody& rigidbodyB, const gm::Transform& frameInA, const gm::Transform& frameInB, bool useLinearReferenceFrameA);
	SliderConstraint(RigidBody& rigidbodyB, const gm::Transform& frameInB, bool useLinearReferenceA);

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	void InitParams();

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	bool _useSolveConstraintObsolete;
	bool _useOffsetForConstraintFrame;
	gm::Transform _frameInA;
	gm::Transform _frameInB;
	bool _useLinearReferenceFrameA;
	float _lowerLinearLimit;
	float _upperLinearLimit;
	float _lowerAngularLimit;
	float _upperAngularLimit;

	float _softnessDirLinear;
	float _restitutionDirLinear;
	float _dampingDirLinear;
	float _cfmDirLinear;

	float _softnessDirAngular;
	float _restitutionDirAngular;
	float _dampingDirAngular;
	float _cfmDirAngular;

	float _softnessLimLinear;
	float _restitutionLimLinear;
	float _dampingLimLinear;
	float _cfmLimLinear;

	float _softnessLimAngular;
	float _restitutionLimAngular;
	float _dampingLimAngular;
	float _cfmLimAngular;

	float _softnessOrthoLinear;
	float _restitutionOrthoLinear;
	float _dampingOrthoLinear;
	float _cfmOrthoLinear;

	float _softnessOrthoAngular;
	float _restitutionOrthoAngular;
	float _dampingOrthoAngular;
	float _cfmOrthoAngular;

	// for internal use
	bool _solveLinearLim[3];
	bool _solveAngularLim[3];
	int  _flags;

	JacobianEntry _jacobianLinear[3];
	float         _jacobianLinearDiagABInverse[3];
	JacobianEntry _jacobianAngular[3];
	float         _timeStep;
	gm::Transform _calculatedTransformA;
	gm::Transform _calculatedTransformB;

	gm::Vector3 _sliderAxis;
	gm::Vector3 _realPivotAInWorld;
	gm::Vector3 _realPivotBInWorld;
	gm::Vector3 _projPivotInWorld;
	gm::Vector3 _delta;
	gm::Vector3 _depth;
	gm::Vector3 _relativePositionA;
	gm::Vector3 _relativePositionB;

	float _linearPosition;
	float _angularPosition;
	float _angularDepth;
	float _kAngle;

	bool _poweredLinearMotor;
	float _targetLinearMotorVelocity;
	float _maxLinearMotorForce;
	float _accumulatedLinearMotorImpulse;

	bool _poweredAngularMotor;
	float _targetAngularMotorVelocity;
	float _maxAngularMotorForce;
	float _accumulatedAngularMotorImpulse;
};
#endif