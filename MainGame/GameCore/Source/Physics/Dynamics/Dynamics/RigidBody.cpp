//////////////////////////////////////////////////////////////////////////////////
//            @file　　  RigidBody.hpp
//            @brief    RigidBody  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partially edit by Toide
//            @date  :  2021_10_05 (first checked) first checked -> no debug.
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
#include "../../../../Include/Physics/Dynamics/Dynamics/RigidBody.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/ConvexShape.hpp"
#include "../../../../Include/Physics/Dynamics/Dynamics/MotionState.hpp"
#include "../../../../Include/Physics/Util/TransformUtil.hpp"
#include "../../../../Include/Physics/Dynamics/ConstraintSolver/TypedConstraint.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
float g_DeactivationTime    = 2.0f;
bool  g_DisableDeactivation = false;
static int uniqueID         = 0;

//////////////////////////////////////////////////////////////////////////////////
//							Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Inline Function
inline Vector3 EvaluateEulerEquation(const Vector3& w1, const Vector3& w0, const Vector3& T, const float dt, const Matrix3& I)
{
	const Vector3 w2 = I * w1 + Cross(w1, I * w1) * dt - (T * dt + I * w0);
	return w2;
}
inline Matrix3 EvaluateEulerEquationDeriv(const Vector3& w1, const Vector3& w0, const float dt, const Matrix3& I)
{
	Matrix3 w1x, Iw1x;
	const Vector3 Iwi = (I * w1);
	GetSkewSymmetricMatrix3(w1, w1x);
	GetSkewSymmetricMatrix3(Iwi, Iw1x);

	const Matrix3 temp = (w1x * I) - Iw1x;
	const Matrix3 dfw1 = I + (temp - Iw1x) * dt;
	return dfw1;
}
#pragma endregion Inline Function
RigidBody::RigidBody(const RigidBodyConstructionInfo& constructionInfo)
{
	SetUpRigidBody(constructionInfo);
}
RigidBody::RigidBody(float mass, IMotionState* motionState, ICollisionShape* collisionShape, const gm::Vector3& localInertia)
{
	RigidBodyConstructionInfo cinfo(mass, motionState, collisionShape, localInertia);
	SetUpRigidBody(cinfo);
}
#pragma region Public Function
const RigidBody* RigidBody::Upcast(const CollisionObject* collisionObject)
{
	if (collisionObject->GetInternalType() & (int)CollisionObjectTypes::RigidBody)
	{
		return (const RigidBody*)collisionObject;
	}
	return nullptr;
}

RigidBody* RigidBody::Upcast(CollisionObject* collisionObject)
{
	if (collisionObject->GetInternalType() & (int)CollisionObjectTypes::RigidBody)
	{
		return (RigidBody*)collisionObject;
	}
	return nullptr;
}
/****************************************************************************
*						ComputeImpulseDenominator
*************************************************************************//**
*  @fn        float RigidBody::ComputeImpulseDenominator(const Vector3& position, const Vector3& normal) const
*  @brief     Compute impulse denominator
*  @param[in] Vector3& position
*  @param[in] Vector3& normal
*  @return 　　float
*****************************************************************************/
float RigidBody::ComputeImpulseDenominator(const Vector3& position, const Vector3& normal) const
{
	Vector3 r0 = position - GetCenterOfMassPosition();
	Vector3 c0 = Cross(r0, normal);
	Vector3 vec = Cross((GetInverseInertialTensorWorld() * vec), r0);
	return _inverseMass + Dot(normal, vec);
}
/****************************************************************************
*						ComputeAngularImpulseDenominator
*************************************************************************//**
*  @fn        float RigidBody::ComputeAngularImpulseDenominator(const gm::Vector3& axis) const
*  @brief     Compute angular impulse denominator
*  @param[in] Vector3& axis
*  @return 　　flaot
*****************************************************************************/
float RigidBody::ComputeAngularImpulseDenominator(const Vector3& axis) const
{
	Vector3 vector = GetInverseInertialTensorWorld() * axis;
	return Dot(axis, vector);
}
/****************************************************************************
*						UpdateDeactivation
*************************************************************************//**
*  @fn        void  RigidBody::UpdateDeactivation(float timeStep)
*  @brief     Update deactivation
*  @param[in] float timeStep
*  @return 　　void
*****************************************************************************/
void  RigidBody::UpdateDeactivation(float timeStep)
{
	if ((GetActivationState() == ISLAND_SLEEPING) || (GetActivationState() == DISABLE_DEACTIVATION)) { return; }

	if ((NormSquared(GetLinearVelocity()) < _linearSleepingThreshold * _linearSleepingThreshold) &&
		(NormSquared(GetAngularVelocity()) < _angularSleepingThreshold * _angularSleepingThreshold))
	{
		_deactivationTime += timeStep;
	}
	else
	{
		_deactivationTime = 0.0f;
		SetActivationState(0);
	}
}
/****************************************************************************
*						WantsSleeping
*************************************************************************//**
*  @fn        bool  RigidBody::WantsSleeping()
*  @brief     ---
*  @param[in] RigidBodyConstrunctionInfo& construnctionInfo
*  @return 　　bool
*****************************************************************************/
bool  RigidBody::WantsSleeping()
{
	if (GetActivationState() == DISABLE_DEACTIVATION)
		return false;

	//disable deactivation
	if (g_DisableDeactivation || (g_DeactivationTime == 0.0f))
		return false;

	if ((GetActivationState() == ISLAND_SLEEPING) || (GetActivationState() == WANTS_DEACTIVATION))
		return true;

	if (_deactivationTime > g_DeactivationTime)
	{
		return true;
	}
	return false;
}
/****************************************************************************
*						SetUpRigidBody
*************************************************************************//**
*  @fn        void RigidBody::SetUpRigidBody(const RigidBodyConstructionInfo& constructionInfo)
*  @brief     Set up rigid body (use reset rigidbody)
*  @param[in] RigidBodyConstrunctionInfo& construnctionInfo
*  @return 　　void
*****************************************************************************/
void RigidBody::SetUpRigidBody(const RigidBodyConstructionInfo& constructionInfo)
{
	_internalType = (int)CollisionObjectTypes::RigidBody;

	_linearVelocity = Vector3(0.0f, 0.0f, 0.0f);
	_angularVelocity = Vector3(0.0f, 0.0f, 0.0f);
	_angularFactor = Vector3(1.0f, 1.0f, 1.0f);
	_linearFactor = Vector3(1.0f, 1.0f, 1.0f);
	_gravity = Vector3(0.0f, 0.0f, 0.0f);
	_gravityAcceleration = Vector3(0.0f, 0.0f, 0.0f);
	_totalForce = Vector3(0.0f, 0.0f, 0.0f);
	_totalTorque = Vector3(0.0f, 0.0f, 0.0f);
	SetDamping(constructionInfo.LinearDamping, constructionInfo.AngularDamping);

	// set damping
	_linearSleepingThreshold = constructionInfo.LinearSleepingThreshold;
	_angularSleepingThreshold = constructionInfo.AngularSleepingThreshold;
	_optionalMotionState = constructionInfo.MotionState;
	ContactSolverType = NULL;
	FrictionSolverType = NULL;
	_additionalDamping = constructionInfo.AdditionalDamping;
	_additionalDampingFactor = constructionInfo.AdditionalDampingFactor;
	_additionalLinearDampingThresholdSqr = constructionInfo.AdditionalLinearDampingThresholdSqr;
	_additionalAngularDampingThresholdSpr = constructionInfo.AdditionalAngularDampingThresholdSqr;
	_additionalAngularDampingFactor = constructionInfo.AdditionalAngularDampingFactor;

	if (_optionalMotionState) { _optionalMotionState->GetWorldTransform(_worldTransform); }
	else { _worldTransform = constructionInfo.StartWorldTransform; }

	_interpolationWorldTransform = _worldTransform;
	_interpolationLinearVelocity = Vector3(0.0f, 0.0f, 0.0f);
	_interpolationAngularVelocity = Vector3(0.0f, 0.0f, 0.0f);

	_friction = constructionInfo.Friction;
	_rollingFriction = constructionInfo.RollingFriction;
	_spinningFriction = constructionInfo.SpinningFriction;

	_restitution = constructionInfo.Restitution;

	SetCollisionShape(constructionInfo.CollisionShape);
	_debugBodyID = uniqueID++;

	SetMassProperty(constructionInfo.Mass, constructionInfo.LocalInertia);
	UpdateInertialTensor();

	_rigidBodyFlags = (int)RigidBodyFlags::Gyroscopic_Force_Implicit_Body;

	_deltaLinearVelocity  = Vector3(0, 0, 0);
	_deltaAngularVelocity = Vector3(0, 0, 0);
	_invMass              = _inverseMass * _linearFactor;
	_pushVelocity         = Vector3(0, 0, 0);
	_turnVelocity         = Vector3(0, 0, 0);
}
/****************************************************************************
*						PredictIntegratedTransform
*************************************************************************//**
*  @fn        void RigidBody::PredictIntegratedTransform(float timeStep, Transform& predictedTransform)
*  @brief     Predicted integrated transform 
*  @param[in] float timeStep
*  @param[in,out] Transform predicted Transform
*  @return 　　void
*****************************************************************************/
void RigidBody::PredictIntegratedTransform(float timeStep, AffineTransform& predictedTransform)
{
	AffineTransformUtil::IntegrateTransform(_worldTransform, _linearVelocity, _angularVelocity, timeStep, predictedTransform);
}
/****************************************************************************
*						SaveKinematicState
*************************************************************************//**
*  @fn        void RigidBody::SaveKinematicState(float timeStep)
*  @brief     Save kinematic state (interpolationLinear, Angular, WorldTrans...)
*  @param[in] float timeStep
*  @return 　　void
*****************************************************************************/
void RigidBody::SaveKinematicState(float timeStep)
{
	if (timeStep != 0.0f)
	{
		if (GetMotionState()) { GetMotionState()->GetWorldTransform(_worldTransform); }

		Vector3 linearVelocity, angularVelocity;
		AffineTransformUtil::CalculateVelocity(_interpolationWorldTransform, _worldTransform, timeStep, _linearVelocity, _angularVelocity);
		_interpolationLinearVelocity  = _linearVelocity;
		_interpolationAngularVelocity = _angularVelocity;
		_interpolationWorldTransform  = _worldTransform;
		
	}
}
/****************************************************************************
*						GetAABB
*************************************************************************//**
*  @fn        void RigidBody::GetAABB(Vector3& aabbMin, Vector3& aabbMax) const
*  @brief     Get AABB
*  @param[out] Vector3 aabbMin
*  @param[out] Vector3 aabbMax
*  @return 　　void
*****************************************************************************/
void RigidBody::GetAABB(Vector3& aabbMin, Vector3& aabbMax) const
{
	GetCollisionShape()->GetAABB(_worldTransform, aabbMin, aabbMax);
}
/****************************************************************************
*						SetGravity
*************************************************************************//**
*  @fn        void RigidBody::SetGravity()
*  @brief     Clear gravity force
*  @param[in] Vector3& acceleration
*  @return 　　void
*****************************************************************************/
void RigidBody::SetGravity(const Vector3& acceleration)
{
	if (_inverseMass != 0.0f)
	{
		// F = mg (gravity unit is N)
		_gravity = acceleration * 1.0f / _inverseMass;
	}
	_gravityAcceleration = acceleration;
}
/****************************************************************************
*						SetDamping
*************************************************************************//**
*  @fn        void RigidBody::SetDamping(float linearDamping, float angularDamping)
*  @brief     set damping coefficients (resistance proportional to speed)
*  @param[in] float linear damping
*  @param[in] float angular damping
*  @return 　　void
*****************************************************************************/
void RigidBody::SetDamping(float linearDamping, float angularDamping)
{
	_linearDamping  = Clamp(linearDamping, 0.0f, 1.0f);
	_angularDamping = Clamp(angularDamping, 0.0f, 1.0f);
}
/****************************************************************************
*						ApplyDamping
*************************************************************************//**
*  @fn        void RigidBody::ApplyDamping(float timeStep)
*  @brief     This function (resistance proportional to speed) damps the velocity
*  @param[in] float timeStep
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyDamping(float timeStep)
{
	//On new damping: see discussion/issue report here: http://code.google.com/p/bullet/issues/detail?id=74
	_linearVelocity  *= Pow(1.0f - _linearDamping, timeStep); 
	_angularVelocity *= Pow(1.0f - _angularDamping, timeStep);

	if (_additionalDamping)
	{
		if ((NormSquared(_angularVelocity) < _additionalAngularDampingThresholdSpr) &&
			NormSquared(_linearVelocity) < _additionalLinearDampingThresholdSqr)
		{
			_angularVelocity = _additionalDampingFactor * _angularVelocity;
			_linearVelocity  = _additionalDampingFactor * _linearVelocity;
		}
		//Such damping is undesirable, so once the overall simulation quality of the rigid body dynamics system has improved, this should become obsolete
		/*-------------------------------------------------------------------
		-               linear velocity
		---------------------------------------------------------------------*/
		float speed = Norm(_linearVelocity);
		if (speed < _linearDamping)
		{
			float dampVelocity = 0.005f;
			if (speed > dampVelocity)
			{
				Vector3 direction = Normalize(_linearVelocity);
				_linearVelocity -= direction * dampVelocity;
			}
			else
			{
				_linearVelocity = Vector3(0.0f, 0.0f, 0.0f);
			}
		}

		/*-------------------------------------------------------------------
		-               angular velocity
		---------------------------------------------------------------------*/
		float angularSpeed = Norm(_angularVelocity);
		if (angularSpeed < _angularDamping)
		{
			float angularDampVelocity = 0.005f;
			if (angularSpeed > angularDampVelocity)
			{
				Vector3 direction = Normalize(_linearVelocity);
				_angularVelocity -= direction * angularDampVelocity;
			}
			else
			{
				_angularVelocity = Vector3(0.0f, 0.0f, 0.0f);
			}
		}
	}
}
/****************************************************************************
*						ApplyGravity
*************************************************************************//**
*  @fn        void RigidBody::ApplyGravity()
*  @brief     apply gravity force
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyGravity()
{
	if (IsStaticOrKinematicObject()) { return; }
	ApplyCentralForce(_gravity);
}
/****************************************************************************
*						ClearGravity
*************************************************************************//**
*  @fn        void RigidBody::ClearGravity()
*  @brief     Clear gravity force
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void RigidBody::ClearGravity()
{
	if (IsStaticOrKinematicObject()) { return; } // check object type
	ApplyCentralForce(-_gravity);                // cancel central force
}
/****************************************************************************
*						ProceedToTransform
*************************************************************************//**
*  @fn        void RigidBody::ProceedToTransform(const gm::Transform newTransform)
*  @brief     set new transform
*  @param[in] gm::Transform newTransform
*  @return 　　void
*****************************************************************************/
void RigidBody::ProceedToTransform(const gm::AffineTransform& newTransform)
{
	SetCenterOfMassTransform(newTransform);
}
/****************************************************************************
*						SetMassProperty
*************************************************************************//**
*  @fn        void RigidBody::SetMassProperty(float mass, const Vector3& inertia)
*  @brief     Set mass property
*  @param[in] float mass
*  @param[in] Vector3 inertia
*  @return 　　void
*****************************************************************************/
void RigidBody::SetMassProperty(float mass, const Vector3& inertia)
{
	if (mass == 0.0f)// if mass == 0, this object is static object
	{
		_collisionFlags |= (int)CollisionFlags::Static;
		_inverseMass     = 0.0f;
	}
	else
	{
		_collisionFlags &= (~(int)CollisionFlags::Static);
		_inverseMass = 1.0f / mass;
	}

	// Fg = m * a
	_gravity = mass * _gravityAcceleration;
	_inverseInertialLocal = Vector3(
		(float)(inertia.GetX() != 0.0f) ? 1.0f / (float)inertia.GetX() : 0.0f,
		(float)inertia.GetY() != 0.0f ? 1.0f / (float)inertia.GetY() : 0.0f,
		(float)inertia.GetZ() != 0.0f ? 1.0f / (float)inertia.GetZ() : 0.0f);

	_invMass = _linearFactor * _inverseMass;
}
/****************************************************************************
*						UpdateInertialTensor
*************************************************************************//**
*  @fn        void RigidBody::UpdateInertialTensor()
*  @brief     Update inertial tensor (慣性モーメント: 対角項(物体の回転しにくさ)　慣性乗積] 非対角項: 角運動量と回転軸とのずれ )
*  @param[in] void 
*  @return 　　void
*****************************************************************************/
void RigidBody::UpdateInertialTensor()
{
	auto scaledMatrix = ScalingMatrix3(_inverseInertialLocal);
	_inverseInertialTensorWorld = _worldTransform.GetBasis() * scaledMatrix * Transpose(_worldTransform.GetBasis());
}
/****************************************************************************
*						GetLocalInertia
*************************************************************************//**
*  @fn        Vector3 RigidBody::GetLocalInertia() const
*  @brief     Get inertial
*  @param[in] void
*  @return 　　Vector3 
*****************************************************************************/
Vector3 RigidBody::GetLocalInertia() const
{
	Vector3 inertialLocal;
	const Vector3 inertia = _inverseInertialLocal;
	inertialLocal.SetX((float)inertia.GetX() != 0.0f ? 1.0f / (float)inertia.GetX() : 0.0f);
	inertialLocal.SetY((float)inertia.GetY() != 0.0f ? 1.0f / (float)inertia.GetY() : 0.0f);
	inertialLocal.SetZ((float)inertia.GetZ() != 0.0f ? 1.0f / (float)inertia.GetZ() : 0.0f);
	return inertialLocal;
}
/****************************************************************************
*						ComputeGyroscopicForceExplicit
*************************************************************************//**
*  @fn        Vector3 RigidBody::ComputeGyroscopicForceExplicit(float maxGyroscopicForce) const
*  @brief     Compute Gyroscopic force
*  @param[in] float maxGyroscopicForce
*  @return 　　Vector3
*****************************************************************************/
Vector3 RigidBody::ComputeGyroscopicForceExplicit(float maxGyroscopicForce) const
{
	Vector3 inertiaLocal       = GetLocalInertia();
	Matrix3 scaled             = ScalingMatrix3(inertiaLocal);
	Matrix3 inertiaTensorWorld = GetWorldTransform().GetBasis() * scaled * Transpose(GetWorldTransform().GetBasis());
	Vector3 temp               = inertiaTensorWorld * GetAngularVelocity();
	Vector3 gf                 = Cross(GetAngularVelocity(), temp);
	Scalar  l2                 = NormSquared(gf);
	if ((float)l2 > maxGyroscopicForce * maxGyroscopicForce)
	{
		gf *= 1.0f / Sqrt(l2) * maxGyroscopicForce;
	}
	return gf;
}
/****************************************************************************
*						ComputeGyroscopicForceExplicit
*************************************************************************//**
*  @fn        Vector3 RigidBody::ComputeGyroscopicForceExplicit(float maxGyroscopicForce) const
*  @brief     Compute Gyroscopic force
*  @param[in] float step
*  @return 　　Vector3
*****************************************************************************/
Vector3 RigidBody::ComputeGyroscopicImpulseImplicitBody(float step) const
{
	Vector3 idl    = GetLocalInertia();
	Vector3 omega1 = GetAngularVelocity();
	Quaternion q   = GetWorldTransform().GetRotation();
	Vector3 omegab = QuaternionRotate(Inverse(q), omega1);
	Matrix3 Ib     = Matrix3(Vector3(idl.GetX(), 0, 0), Vector3(0, idl.GetY(), 0) , Vector3(0,0,idl.GetZ()));
	Vector3 ibo    = Ib * omegab;
	Vector3 f      = step * Cross(omegab, ibo);
	Matrix3 skew0;
	GetSkewSymmetricMatrix3(omegab, skew0);
	Vector3 om     = Ib * omegab;
	Matrix3 skew1;
	GetSkewSymmetricMatrix3(om, skew1);
	Matrix3 jacobian = Ib + (skew0 * Ib - skew1) * step;
	Vector3 omegaDiv = Solve33(f, jacobian);
	omegab = omegab - omegaDiv;
	Vector3 omega2 = QuaternionRotate(q, omegab);
	Vector3 gf = omega2 - omega1;
	return gf;

}
/****************************************************************************
*						ComputeGyroscopicForceExplicit
*************************************************************************//**
*  @fn        Vector3 RigidBody::ComputeGyroscopicForceExplicit(float maxGyroscopicForce) const
*  @brief     Compute Gyroscopic force
*  @param[in] float step
*  @return 　　Vector3
*****************************************************************************/
Vector3 RigidBody::ComputeGyroscopicImpulseImplicitWorld(float step) const
{
	const Vector3 inertiaLocal = GetLocalInertia();
	const Vector3 w0           = GetAngularVelocity();
	Matrix3 I;

	I = _worldTransform.GetBasis() * ScalingMatrix3(inertiaLocal) * Transpose(_worldTransform.GetBasis());
	Vector3 w1 = w0;

	// one step of newton's method
	const Vector3 fw = EvaluateEulerEquation(w1, w0, Vector3(0, 0, 0), step, I);
	const Matrix3 dfw = EvaluateEulerEquationDeriv(w1, w0, step, I);
	Vector3 dw;
	dw = Solve33(fw, dfw);
	w1 -= dw;

	Vector3 gf = (w1 - w0);
	return gf;
}
/****************************************************************************
*						IntegrateVelocities
*************************************************************************//**
*  @fn        void RigidBody::IntegrateVelocities(float step)
*  @brief     This class object is calculated total angular, linearVelocity; 
*  @param[in] float step
*  @return 　　 void
*****************************************************************************/
void RigidBody::IntegrateVelocities(float step)
{
	if (IsStaticOrKinematicObject()) { return; }

	_linearVelocity  += _totalForce * (_inverseMass * step);
	_angularVelocity += _inverseInertialTensorWorld * _totalTorque * step;

	float angVel = Norm(_angularVelocity);
	if (angVel * step > GM_HALF_PI)
	{
		_angularVelocity = _angularVelocity * (GM_HALF_PI / step) / angVel;
	}
}
/****************************************************************************
*						GetOrientation
*************************************************************************//**
*  @fn        Quaternion RigidBody::GetOrientation() const
*  @brief     Get Orientation
*  @param[in] void
*  @return 　　Quaternion
*****************************************************************************/
Quaternion RigidBody::GetOrientation() const
{
	return _worldTransform.GetRotation();
}
/****************************************************************************
*						SetCenterOfMassTransform
*************************************************************************//**
*  @fn        void RigidBody::SetCenterOfMassTransform(const AffineTransform& xForm)
*  @brief     Set center of mass transform 
*  @param[in] AffineTransform& xform
*  @return 　　void
*****************************************************************************/
void RigidBody::SetCenterOfMassTransform(const AffineTransform& xForm)
{
	if (IsKinematicObject())
	{
		_interpolationWorldTransform = _worldTransform;
	}
	else
	{
		_interpolationWorldTransform = xForm;
	}
	_interpolationLinearVelocity = GetLinearVelocity();
	_interpolationAngularVelocity = GetAngularVelocity();
	_worldTransform = xForm;
	UpdateInertialTensor();
}
/****************************************************************************
*						AddConstraintRef
*************************************************************************//**
*  @fn        void RigidBody::AddConstraintRef(TypedConstraint* constraint)
*  @brief     Add constraint ref
*  @param[in, out] TypedConstraint* constraint
*  @return 　　void
*****************************************************************************/
void RigidBody::AddConstraintRef(TypedConstraint* constraint)
{
	int index = _constraintRefs.findLinearSearch(constraint);
	//don't add constraints that are already referenced
	//btAssert(index == m_constraintRefs.size());
	if (index == _constraintRefs.size())
	{
		_constraintRefs.push_back(constraint);
		CollisionObject* collisionObjectA = &constraint->GetRigidBodyA();
		CollisionObject* collisionObjectB = &constraint->GetRigidBodyB();
		if (collisionObjectA == this)
		{
			collisionObjectA->SetIgnoreCollisionCheck(collisionObjectB, true);
		}
		else
		{
			collisionObjectB->SetIgnoreCollisionCheck(collisionObjectA, true);
		}
	}
}
/****************************************************************************
*						RemoveConstraintRef
*************************************************************************//**
*  @fn        void RigidBody::RemoveConstraintRef(TypedConstraint* constraint)
*  @brief     Remove constraint ref
*  @param[in, out] TypedConstraint* constraint
*  @return 　　void
*****************************************************************************/
void RigidBody::RemoveConstraintRef(TypedConstraint* constraint)
{
	int index = _constraintRefs.findLinearSearch(constraint);
	if (index < _constraintRefs.size())
	{
		_constraintRefs.remove(constraint);
		CollisionObject* collisionObjectA = &constraint->GetRigidBodyA();
		CollisionObject* collisionObjectB = &constraint->GetRigidBodyB();
		if (collisionObjectA == this)
		{
			collisionObjectA->SetIgnoreCollisionCheck(collisionObjectB, false);
		}
		else
		{
			collisionObjectB->SetIgnoreCollisionCheck(collisionObjectA, false);
		}
	}
}
/****************************************************************************
*						ApplyCentralForce
*************************************************************************//**
*  @fn        void RigidBody::ApplyCentralForce(const gm::Vector3& force)
*  @brief     Apply central force
*  @param[in] gm::Vector3& force
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyCentralForce(const gm::Vector3& force)
{
	_totalForce += force * _linearFactor;
}
/****************************************************************************
*						 ApplyForce
*************************************************************************//**
*  @fn        void RigidBody::ApplyForce(const Vector3& force, const Vector3& rel_pos)
*  @brief     Apply force
*  @param[in] gm::Vector3& force
*  @param[in] Vector3& rel_pos
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyForce(const Vector3& force, const Vector3& rel_pos)
{
	ApplyCentralForce(force);
	ApplyTorque(Cross(rel_pos, force * _linearFactor));
}
/****************************************************************************
*						 ApplyTorque
*************************************************************************//**
*  @fn        void RigidBody::ApplyTorque(const Vector3& torque)
*  @brief     Apply torque
*  @param[in] Vector3& torque
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyTorque(const Vector3& torque)
{
	_totalTorque += torque * _angularFactor;
}
/****************************************************************************
*						 ApplyCentralImpulse
*************************************************************************//**
*  @fn        void RigidBody::ApplyCentralImpulse(const Vector3& impulse)
*  @brief     Apply central impulse
*  @param[in] Vector3& impulse
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyCentralImpulse(const Vector3& impulse)
{
	_linearVelocity += impulse * _linearFactor * _inverseMass;
}
/****************************************************************************
*						 ApplyTorqueImpulse
*************************************************************************//**
*  @fn        void RigidBody::ApplyTorqueImpulse(const Vector3& torque)
*  @brief     Apply torque impulse
*  @param[in] Vector3& torque
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyTorqueImpulse(const Vector3& torque)
{
	_angularVelocity += _inverseInertialTensorWorld * torque * _angularFactor;
}
/****************************************************************************
*						 ApplyImpulse
*************************************************************************//**
*  @fn        RigidBody::ApplyImpulse(const Vector3& impulse, const Vector3& rel_pos)
*  @brief     Apply impulse (central force and torque)
*  @param[in] Vector3 impulse
*  @param[in] Vector3 rel_pos
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyImpulse(const Vector3& impulse, const Vector3& rel_pos)
{
	if (_inverseMass != 0.0f)
	{
		ApplyCentralForce(impulse);
		if (&_angularFactor != nullptr)
		{
			ApplyTorqueImpulse(Cross(rel_pos, impulse * _linearFactor));
		}
	}
}
/****************************************************************************
*						 ApplyPushImpulse
*************************************************************************//**
*  @fn        RigidBody::ApplyPushImpulse(const Vector3& impulse, const Vector3& rel_pos)
*  @brief     Apply impulse (central force and torque)
*  @param[in] Vector3 impulse
*  @param[in] Vector3 rel_pos
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyPushImpulse(const Vector3& impulse, const Vector3& rel_pos)
{
	if (_inverseMass != 0.0f)
	{
		ApplyCentralPushImpulse(impulse);
		if (&_angularFactor != nullptr)
		{
			ApplyTorqueTurnImpulse(Cross(rel_pos, impulse * _linearFactor));
		}
	}
}
/****************************************************************************
*						 ApplyCentralPushImpulse
*************************************************************************//**
*  @fn        RigidBody::ApplyCentralPushImpulse(const Vector3& impulse, const Vector3& rel_pos)
*  @brief     Apply central push impulse (central force and torque)
*  @param[in] Vector3 impulse
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyCentralPushImpulse(const Vector3& impulse)
{
	_pushVelocity += impulse * _linearFactor * _inverseMass;
}
/****************************************************************************
*						 ApplyTorqueTurnImpulse
*************************************************************************//**
*  @fn        void RigidBody::ApplyTorqueTurnImpulse(const Vector3& torque)
*  @brief     ApplyTorqueTurnImpulse
*  @param[in] Vector3 torque
*  @return 　　void
*****************************************************************************/
void RigidBody::ApplyTorqueTurnImpulse(const Vector3& torque)
{
	_turnVelocity += _inverseInertialTensorWorld * torque * _angularFactor;
}
/****************************************************************************
*						 ClearForces
*************************************************************************//**
*  @fn        void RigidBody::ClearForces()
*  @brief     Clear forces
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void RigidBody::ClearForces()
{
	_totalForce  = Vector3();
	_totalTorque = Vector3();
}
/****************************************************************************
*						 SetLinearFactor
*************************************************************************//**
*  @fn        void RigidBody::SetLinearFactor(const gm::Vector3& linearFactor)
*  @brief     SetLinearFactor
*  @param[in] Vector3& linearFactor
*  @return 　　void
*****************************************************************************/
void RigidBody::SetLinearFactor(const Vector3& linearFactor)
{
	_linearFactor = linearFactor;
	_invMass      = _linearFactor * _inverseMass;
}
/****************************************************************************
*						 SetLinearFactor
*************************************************************************//**
*  @fn        void RigidBody::SetSleepingThresholds(float linear, float angular)
*  @brief     Set sleeping linear and angular thresholds.
*  @param[in] float linear
*  @param[in] float angular
*  @return 　　void
*****************************************************************************/
void RigidBody::SetSleepingThresholds(float linear, float angular)
{
	_linearSleepingThreshold  = linear;
	_angularSleepingThreshold = angular;
}
void RigidBody::SetMotionState(IMotionState* motionState)
{
	_optionalMotionState = motionState;
	if (_optionalMotionState) { motionState->GetWorldTransform(_worldTransform); }
}
/****************************************************************************
*						 SetLinearVelocity
*************************************************************************//**
*  @fn        void RigidBody::SetLinearVelocity(const gm::Vector3& linearVelocity)
*  @brief     Set linear velocity
*  @param[in] Vector3& linearVelocity
*  @return 　　void
*****************************************************************************/
void RigidBody::SetLinearVelocity(const gm::Vector3& linearVelocity)
{
	_updateRevision++;
	_linearVelocity = linearVelocity;
}


void RigidBody::SetAngularVelocity(const gm::Vector3& angularVelocity)
{
	_updateRevision++;
	_angularVelocity = angularVelocity;
}

#pragma endregion Public Function