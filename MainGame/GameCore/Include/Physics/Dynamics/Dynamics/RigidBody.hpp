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

#pragma once
#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionDispatch/CollisionObject.hpp"
class IMotionState;
class ICollisionShape;
class TypedConstraint;
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class RigidBodyFlags
{
	Disable_World_Gravity = 0x0001,
	Gyroscopic_Force_Explicit = 0x0002,
	Gyroscopic_Force_Implicit_World = 0x0004,
	Gyroscopic_Force_Implicit_Body = 0x0008,
	Gyroscopic_Force = Gyroscopic_Force_Implicit_Body
};

/****************************************************************************
*					    RigidBody
*************************************************************************//**
*  @class     RigidBody
*  @brief     This class provides discrete rigidbody simulation.
*****************************************************************************/
class RigidBody : public CollisionObject
{
public:
	struct RigidBodyConstructionInfo
	{
		float         Mass;

		IMotionState* MotionState;
		gm::AffineTransform StartWorldTransform;

		ICollisionShape* CollisionShape;
		gm::Vector3   LocalInertia;
		float         LinearDamping;
		float         AngularDamping;

		float         Friction;        // best simulation results when friction is non-zero.
		float         RollingFriction;
		float         SpinningFriction;

		float         Restitution;
		float         LinearSleepingThreshold;
		float         AngularSleepingThreshold;

		bool          AdditionalDamping;
		float         AdditionalDampingFactor;
		float         AdditionalLinearDampingThresholdSqr;
		float         AdditionalAngularDampingThresholdSqr;
		float         AdditionalAngularDampingFactor;

		RigidBodyConstructionInfo(float mass, IMotionState* motionState, ICollisionShape* collisionShape, const gm::Vector3& localInertia = gm::Vector3(0, 0, 0))
			: Mass(mass),
			MotionState(motionState),
			CollisionShape(collisionShape),
			LocalInertia(localInertia),
			LinearDamping(0.0f),
			AngularDamping(0.0f),
			Friction(0.5f),
			RollingFriction(0.0f),
			SpinningFriction(0.0f),
			Restitution(0.0f),
			LinearSleepingThreshold(0.8f),
			AngularSleepingThreshold(1.0f),
			AdditionalDamping(false),
			AdditionalDampingFactor(0.005f),
			AdditionalLinearDampingThresholdSqr(0.01f),
			AdditionalAngularDampingThresholdSqr(0.01f),
			AdditionalAngularDampingFactor(0.01f)
		{
			StartWorldTransform = gm::AffineTransform();
		}
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static const RigidBody* Upcast(const CollisionObject* collisionObject);
	static RigidBody* Upcast(CollisionObject* collisionObject);

	void Translate(const gm::Vector3& v) { _worldTransform.SetTranslation(_worldTransform.GetTranslation() + v); }
	void UpdateInertialTensor();

	void ProceedToTransform(const gm::AffineTransform& newTransform);
	void PredictIntegratedTransform(float step, gm::AffineTransform& predictedTransform);
	void SaveKinematicState(float step);
	void IntegrateVelocities(float step);

	float ComputeImpulseDenominator(const gm::Vector3& position, const gm::Vector3& normal) const;
	float ComputeAngularImpulseDenominator(const gm::Vector3& axis) const;
	void  UpdateDeactivation(float timeStep);
	bool  WantsSleeping(); 

	gm::Vector3 ComputeGyroscopicImpulseImplicitWorld(float dt) const;
	gm::Vector3 ComputeGyroscopicImpulseImplicitBody(float step) const;
	gm::Vector3 ComputeGyroscopicForceExplicit(float maxGyrpscpicForce) const;

	void AddConstraintRef(TypedConstraint* c);
	void RemoveConstraintRef(TypedConstraint* c);

	bool IsInWorld() const { return GetBroadPhaseProxy() != 0; }
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	INLINE const ICollisionShape* GetCollisionShape() const { return _collisionShape; }
	INLINE ICollisionShape* GetCollisionShape      ()       { return _collisionShape; }
	const gm::Matrix3& GetInverseInertialTensorWorld() const { return _inverseInertialTensorWorld; }
	const gm::Vector3& GetInverseInertialDiagLocal() const { return _inverseInertialLocal; }
	const gm::Vector3& GetTotalForce     () const { return _totalForce; }
	const gm::Vector3& GetTotalTorque    () const { return _totalTorque; }
	const gm::Vector3& GetLinearVelocity () const { return _linearVelocity; }
	const gm::Vector3& GetAngularVelocity() const { return _angularVelocity; }
	const gm::Vector3& GetLinearFactor   () const { return _linearFactor; }
	const gm::Vector3& GetAngularFactor  () const { return _angularFactor; }
	const gm::Vector3& GetCenterOfMassPosition() const { return _worldTransform.GetTranslation(); }
	const gm::AffineTransform& GetCenterOfMassTransform() const { return _worldTransform; }
	gm::Quaternion GetOrientation() const;

	
	gm::Vector3        GetPushVelocity   () const { return _pushVelocity; }
	gm::Vector3        GetTurnVelocity   () const { return _turnVelocity; }
	gm::Vector3        GetVelocityInLocalPoint(const gm::Vector3& rel_pos) const { return _linearVelocity + Cross(_angularVelocity, rel_pos); }
	gm::Vector3        GetPushVelocityInLocalPoint(const gm::Vector3& rel_pos) const { return _pushVelocity + Cross(_turnVelocity, rel_pos); }
	gm::Vector3        GetLocalInertia() const;

	float GetLinearDamping               () const { return _linearDamping; }
	float GetAngularDamping              () const { return _angularDamping; }
	float GetLinearSleepingThreshold     () const { return _linearSleepingThreshold; }
	float GetAngularSleepingThreshold    () const { return _angularSleepingThreshold; }
	const gm::Vector3& GetGravity            () const { return _gravityAcceleration; }
	float GetInverseMass                 () const { return _inverseMass; }
	float GetMass                        () const { return _inverseMass == 0.0f ? 0.0f : 1.0f / _inverseMass;}
	TypedConstraint* GetConstraintRefs(int index) { return _constraintRefs[index]; }
	int   GetConstraintRefsCount() const { return _constraintRefs.size(); }
	int   GetFlags                       () const { return _rigidBodyFlags; }
	void  GetAABB(gm::Vector3& aabbMin, gm::Vector3& aabbMax) const;
	IMotionState*       GetMotionState()              { return _optionalMotionState; }
	const IMotionState* GetMotionState() const        { return _optionalMotionState; }
	const BroadPhaseProxy* GetBroadPhaseProxy() const { return _broadPhaseHandler; }
	BroadPhaseProxy*       GetBroadPhaseProxy()       { return _broadPhaseHandler; }

	void ApplyTorque(const gm::Vector3& torque);
	void ApplyForce(const gm::Vector3& force, const gm::Vector3& rel_pos);
	
	void ApplyCentralImpulse(const gm::Vector3& impulse);
	void ApplyTorqueImpulse(const gm::Vector3& torque);
	void ApplyPushImpulse(const gm::Vector3& impulse, const gm::Vector3& rel_pos);
	void ApplyCentralPushImpulse(const gm::Vector3& impulse);
	void ApplyTorqueTurnImpulse(const gm::Vector3& torque);
	void ApplyImpulse(const gm::Vector3& impulse, const gm::Vector3& rel_pos);

	void SetGravity(const gm::Vector3& acceleration);
	void ApplyGravity();
	void ClearGravity();

	void ApplyCentralForce(const gm::Vector3& force);
	
	void ApplyDamping(float timeStep);
	void SetDamping(float linearDamping, float angularDamping);

	void SetInverseInertialDiagLocal(const gm::Vector3& diagInverseInvertia) { _inverseInertialLocal = diagInverseInvertia; }
	void SetFlags (int flags) { _rigidBodyFlags = flags; }
	void SetPushVelocity(const gm::Vector3& velocity) { _pushVelocity = velocity; }
	void SetTurnVelocity(const gm::Vector3& velocity) { _turnVelocity = velocity; }
	void SetLinearFactor(const gm::Vector3& linearFactor);
	void SetLinearVelocity(const gm::Vector3& linearVelocity);
	void SetAngularVelocity(const gm::Vector3& angularVelocity);
	void SetAngularFactor(float angularFactor) { _updateRevision++; _angularFactor = gm::Vector3(angularFactor, angularFactor, angularFactor); }
	void SetAngularFactor(const gm::Vector3& angularFactor) { _updateRevision++; _angularFactor = angularFactor; }
	void SetSleepingThresholds(float linear, float angular);
	void SetNewBroadPhaseProxy(BroadPhaseProxy* broadPhaseProxy) { _broadPhaseHandler = broadPhaseProxy; }
	void SetMotionState(IMotionState* motionState);
	void SetMassProperty(float mass, const gm::Vector3& inertia);
	void SetCenterOfMassTransform(const gm::AffineTransform& xForm);
	void ClearForces();

	int ContactSolverType;
	int FrictionSolverType;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	RigidBody(const RigidBodyConstructionInfo& constructionInfo);
	RigidBody(float mass, IMotionState* motionState, ICollisionShape* collisionShape, const gm::Vector3& localInertia = gm::Vector3(0, 0, 0));
	virtual ~RigidBody(){}
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	void SetUpRigidBody(const RigidBodyConstructionInfo& constructionInfo);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::Vector3 _deltaLinearVelocity;
	gm::Vector3 _deltaAngularVelocity;
	gm::Vector3 _angularFactor;
	gm::Vector3 _pushVelocity;
	gm::Vector3 _turnVelocity;
	gm::Vector3 _invMass;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	gm::Matrix3 _inverseInertialTensorWorld;
	gm::Vector3 _linearVelocity;
	gm::Vector3 _angularVelocity; 
	float       _inverseMass;
	gm::Vector3 _linearFactor;
	gm::Vector3 _gravity; // F = mg (N)
	gm::Vector3 _gravityAcceleration;
	gm::Vector3 _inverseInertialLocal;
	gm::Vector3 _totalForce;
	gm::Vector3 _totalTorque;

	float _linearDamping;
	float _angularDamping;

	bool _additionalDamping;
	float _additionalDampingFactor;
	float _additionalLinearDampingThresholdSqr;
	float _additionalAngularDampingThresholdSpr;
	float _additionalAngularDampingFactor;

	float _linearSleepingThreshold;
	float _angularSleepingThreshold;
	
	IMotionState* _optionalMotionState;

	gm::AlignedObjectArray<TypedConstraint*> _constraintRefs;
	int _rigidBodyFlags;

	int _debugBodyID;
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	
};
#endif