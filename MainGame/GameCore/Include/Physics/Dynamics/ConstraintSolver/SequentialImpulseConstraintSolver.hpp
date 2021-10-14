//////////////////////////////////////////////////////////////////////////////////
///             @file   SequentialImpulseConstraintSolver.hpp
///             @brief  SequentialImpulseConstraintSolver
///             @author Partially edit by Toide
///             @date   2021_10_03
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
#ifndef SEQUENTIAL_IMPULSE_CONSTRAINT_SOLVER_HPP
#define SEQUENTIAL_IMPULSE_CONSTRAINT_SOLVER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "TypedConstraint.hpp"
#include "ConstraintSolver.hpp"
#include "SolverBody.hpp"
#include "SolverConstraint.hpp"
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/ManifoldPoint.hpp"
#include "ConstraintSolver.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
typedef float(*SingleConstraintRowSolver)(SolverBody&, SolverBody&, const SolverConstraint&);

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
struct SolverAnalyticsData
{
	SolverAnalyticData()
	{
		NumSolverCalls                = 0;
		NumIterationsUsed             = -1;
		RemainingLeastSquaresResidual = -1;
		IslandID                      = -2;
	}

	int IslandID;
	int NumBodies;
	int NumContactManifolds;
	int NumSolverCalls;
	int NumIterationsUsed;
	float RemainingLeastSquaresResidual;
};
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
__declspec(align(16))
class SequentialImpulseConstraintSolver : public IConstraintSolver
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual float SolveGroup(CollisionObject** bodies, int numBodies, PersistentManifold** manifold, int numManifolds, TypedConstraint** constraint, int numConstraints, const ContactSolverInfo& infomation, IDispatcher* dispatcher) override;
	virtual void  Reset() override;
	


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	unsigned long Rand2();
	int          RandInt2(int n);
	void         SetRandSeed(unsigned long seed) { _seed2 = seed; }
	unsigned int GetRandSeed() const { return _seed2; }

	virtual ConstraintSolverType GetSolverType() const override{ return ConstraintSolverType::SEQUENTIAL_IMPULSE_SOLVER; }
	SingleConstraintRowSolver GetActiveConstraintRowSolverGeneric() { return _resolveSingleConstraintRowGeneric; }
	void SetConstraintRowSolverGeneric(SingleConstraintRowSolver rowSolver) { _resolveSingleConstraintRowGeneric = rowSolver; }
	SingleConstraintRowSolver GetActiveConstraintRowSolverLowerLimit() { return _resolveSingleConstraintRowLowerLimit; }
	void SetConstraintRowSolverLowerLimit(SingleConstraintRowSolver rowSolver)
	{
		_resolveSingleConstraintRowLowerLimit = rowSolver;
	}

	///Various implementations of solving a single constraint row using a generic equality constraint, using scalar reference, SSE2 or SSE4
	SingleConstraintRowSolver GetScalarConstraintRowSolverGeneric();
	SingleConstraintRowSolver GetSSE2ConstraintRowSolverGeneric();
	SingleConstraintRowSolver GetSSE4_1ConstraintRowSolverGeneric();

	///Various implementations of solving a single constraint row using an inequality (lower limit) constraint, using scalar reference, SSE2 or SSE4
	SingleConstraintRowSolver GetScalarConstraintRowSolverLowerLimit();
	SingleConstraintRowSolver GetSSE2ConstraintRowSolverLowerLimit();
	SingleConstraintRowSolver GetSSE4_1ConstraintRowSolverLowerLimit();
	SolverAnalyticsData _analyticsData;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	SequentialImpulseConstraintSolver();
	virtual ~SequentialImpulseConstraintSolver();

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	float RestitutionCurve(float relativeVelocity, float restitution, float velocityThreshold);

	static void ApplyAnisotropicImpulse(CollisionObject* collisionObject, gm::Vector3& frictionDirection, int frictionMode);

	SolverConstraint& AddFrictionConstraint(const gm::Vector3& normalAxis, int solverBodyIDA, int solverBodyIDB, int frictionIndex, ManifoldPoint& cp, const gm::Vector3& relativePosition1, const gm::Vector3& relativePosition2, CollisionObject* collisionObject0, CollisionObject* collisionObject1, float relaxation, const ContactSolverInfo& infomationGlobal, float desiredVelocity = 0.0f, float cfmSlip = 0.0f);
	SolverConstraint& AddTorsionalFrictionConstraint(const gm::Vector3& normalAxis, int solverBodyIDA, int solverBodyIDB, int frictionIndex, ManifoldPoint& cp, float torsionalFriction, const gm::Vector3& relativePosition1, const gm::Vector3& relativePosition2, CollisionObject* collisionObject0, CollisionObject* collisionObject1, float relaxation, float desiredVelocity = 0, float cfmSlip = 0.0f);

	void SetUpFrictionConstraint(SolverConstraint& solverConstraint, const gm::Vector3& normalAxis, int solverBodyIDA, int solverBodyIDB, ManifoldPoint& cp, const gm::Vector3& relativePosition1, const gm::Vector3& relativePosition2,
		CollisionObject* collisionObject0, CollisionObject* collisionObject1, float relaxation,
		const ContactSolverInfo& infomationGlobal, float desiredVelocity = 0.0f, float cfmSlip = 0.0f);
	void SetUpTorsionalFrictionConstraint(SolverConstraint& solverConstraint, const gm::Vector3& normalAxis, int solverBodyIDA, int solverBodyIDB, ManifoldPoint& cp, float combinedTorsionalFriction, const gm::Vector3& relativePosition1, const gm::Vector3& relativePosition2,
		CollisionObject* collisionObject0, CollisionObject* collisionObject1, float relaxation, float desiredVelocity = 0.0f, float cfmSlip = 0.0f);
	void SetUpContactConstraint(SolverConstraint& solverConstraint, int solverBodyIDA, int solverBodyIDB, ManifoldPoint& cp, const ContactSolverInfo& infomationGlobal, float& relaxation, const gm::Vector3& relativePosition1, const gm::Vector3& relativePosition2);
	void SetFrictionConstraintImpulse(SolverConstraint& solverConstraint, int solverBodyIDA, int solverBodyIDB, ManifoldPoint& cp, const ContactSolverInfo& infomationGlobal);
	float ResolveSplitPenetrationSIMD(SolverBody& bodyA, SolverBody& bodyB, const SolverConstraint& contactConstraint)
	{
		return _resolveSplitPenetrationImpulse(bodyA, bodyB, contactConstraint);
	}

	float ResolveSplitPenetrationImpulseCacheFriendly(SolverBody& bodyA, SolverBody& bodyB, const SolverConstraint& contactConstraint)
	{
		return _resolveSplitPenetrationImpulse(bodyA, bodyB, contactConstraint);
	}

	virtual void ConvertContacts(PersistentManifold** manifoldPtr, int numManifolds, const ContactSolverInfo& infomationGlobal);
	void         ConvertContacts(PersistentManifold* manifold, const ContactSolverInfo& infoGlobal);
	virtual void ConvertJoints(TypedConstraint** constraints, int numConstraints, const ContactSolverInfo& infoGlobal);
	void         ConvertContacts(SolverConstraint* currentConstraintRow, TypedConstraint* constraint, const TypedConstraint::ConstraintInfo1& infomation1, int solverBodyIDA, int solverBodyIDB, const ContactSolverInfo& infomationGlobal);
	virtual void ConvertBodies(CollisionObject** bodies, int numBodies, const ContactSolverInfo& infomationGlobal);
	
	int GetOrInitSolverBody(CollisionObject& body, float timeStep);
	void InitializeSolverBody(SolverBody* solverBody, CollisionObject* collisionObject, float timeStep);
	
	float ResolveSingleConstraintRowGeneric    (SolverBody& bodyA, SolverBody& bodyB, const SolverConstraint& contactConstraint);
	float ResolveSingleConstraintRowGenericSIMD(SolverBody& bodyA, SolverBody& bodyB, const SolverConstraint& contactConstraint);
	float ResolveSingleConstraintRowLowerLimit (SolverBody& bodyA, SolverBody& bodyB, const SolverConstraint& contactConstraint);
	float ResolveSingleConstraintRowLowerLimitSIMD(SolverBody& bodyA, SolverBody& bodyB, const SolverConstraint& contactConstraint);
	float ResolveSplitPenetrationImpulse       (SolverBody& bodyA, SolverBody& bodyB, const SolverConstraint& contactConstraint)
	{
		return _resolveSplitPenetrationImpulse(bodyA, bodyB, contactConstraint);
	}

	void WriteBackContacts(int iBegin, int iEnd, const ContactSolverInfo& infoGlobal);
	void WriteBackJoints(int iBegin, int iEnd, const ContactSolverInfo& infoGlobal);
	void WriteBackBodies(int iBegin, int iEnd, const ContactSolverInfo& infoGlobal);
	virtual void SolveGroupCacheFriendlySplitImpulseIterations(CollisionObject** bodies, int numBodies, PersistentManifold** manifoldPtr, int numManifolds, TypedConstraint** constraints, int numConstraints, const ContactSolverInfo& infoGlobal);
	virtual float SolveGroupCacheFriendlyFinish(CollisionObject** bodies, int numBodies, const ContactSolverInfo& infoGlobal);
	virtual float SolveSingleIteration(int iteration, CollisionObject** bodies, int numBodies, PersistentManifold** manifoldPtr, int numManifolds, TypedConstraint** constraints, int numConstraints, const ContactSolverInfo& infoGlobal);

	virtual float SolveGroupCacheFriendlySetup(CollisionObject** bodies, int numBodies, PersistentManifold** manifoldPtr, int numManifolds, TypedConstraint** constraints, int numConstraints, const ContactSolverInfo& infoGlobal);
	virtual float SolveGroupCacheFriendlyIterations(CollisionObject** bodies, int numBodies, PersistentManifold** manifoldPtr, int numManifolds, TypedConstraint** constraints, int numConstraints, const ContactSolverInfo& infoGlobal);
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<SolverBody> _temoSolverBodyPool;
	ConstraintArray _tempSolverContactConstraintPool;
	ConstraintArray _tempSolverNonContactConstraintPool;
	ConstraintArray _tempSolverContactFrictionConstraintPool;
	ConstraintArray _tempSolverContactRollingFrictionConstraintPool;

	gm::AlignedObjectArray<int> _orderTempConstraintPool;
	gm::AlignedObjectArray<int> _orderNonContactConstraintPool;
	gm::AlignedObjectArray<int> _orderFrictionConstraintPool;
	gm::AlignedObjectArray<TypedConstraint::ConstraintInfo1> _tempConstraintSizePool;
	int _maxOverrideNumSolverIterations;
	int _fixedBodyID;

	gm::AlignedObjectArray<int> _kinematicBodyUniqueIDToSolverBodyTable;

	SingleConstraintRowSolver _resolveSingleConstraintRowGeneric;
	SingleConstraintRowSolver _resolveSingleConstraintRowLowerLimit;
	SingleConstraintRowSolver _resolveSplitPenetrationImpulse;
	int           _cachedSolverMode;
	float         _leastSquaresResidual;
	unsigned long _seed2;

};

#endif