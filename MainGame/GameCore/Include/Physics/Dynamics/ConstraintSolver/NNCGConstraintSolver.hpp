//////////////////////////////////////////////////////////////////////////////////
///             @file   NNCGConstraintSolver.hpp
///             @brief  NNCG Constraint Solver
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
#ifndef NNCG_CONSTRAINT_SOLVER_HPP
#define NNCG_CONSTRAINT_SOLVER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "SequentialImpulseConstraintSolver.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

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
class NNCGConstraintSolver : public SequentialImpulseConstraintSolver
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool OnlyForNoneContact;

	virtual ConstraintSolverType GetSolverType() const { return ConstraintSolverType::NNCG_SOLVER; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	NNCGConstraintSolver() : SequentialImpulseConstraintSolver(), OnlyForNoneContact(false) {}
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	virtual float SolveGroupCacheFriendlyFinish(CollisionObject** bodies, int numBodies, const ContactSolverInfo& infoGlobal);
	virtual float SolveSingleIteration(int iteration, CollisionObject** bodies, int numBodies, PersistentManifold** manifoldPtr, int numManifolds, TypedConstraint** constraints, int numConstraints, const ContactSolverInfo& infoGlobal);

	virtual float SolveGroupCacheFriendlySetup(CollisionObject** bodies, int numBodies, PersistentManifold** manifoldPtr, int numManifolds, TypedConstraint** constraints, int numConstraints, const ContactSolverInfo& infoGlobal);
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	float _deltaLengthSqrPrevious;
	gm::AlignedObjectArray<float> _pNC;   // p for None Contact constraints
	gm::AlignedObjectArray<float> _pC;    // p for Contact constraints
	gm::AlignedObjectArray<float> _pCF;   // p for ContactFriction constraints
	gm::AlignedObjectArray<float> _pCRF;  // p for ContactRollingFriction constraints

	//These are recalculated in every iterations. We just keep these to prevent reallocation in each iteration.
	gm::AlignedObjectArray<float> _deltaNC;   // deltaf for NoneContact constraints
	gm::AlignedObjectArray<float> _deltaC;    // deltaf for Contact constraints
	gm::AlignedObjectArray<float> _deltaCF;   // deltaf for ContactFriction constraints
	gm::AlignedObjectArray<float> _deltaCRF;  // deltaf for ContactRollingFriction constraints
};

#endif