//////////////////////////////////////////////////////////////////////////////////
///             @file   ConstraintSolver.hpp
///             @brief  ConstraintSolver
///             @author Partially edit by Toide
///             @date   2021_10_03
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library, http://bulletphysics.org
Copyright (C) 2006, 2007 Sony Computer Entertainment Inc.

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
#ifndef CONSTRAINT_SOLVER_HPP
#define CONSTRAINT_SOLVER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class PersistentManifold;
class RigidBody;
class CollisionObject;
class TypedConstraint;
struct ContactSolverInfo;
struct BroadPhaseProxy;
class IDispatcher;

enum class ConstraintSolverType
{
	SEQUENTIAL_IMPULSE_SOLVER = 1,
	MLCP_SOLVER = 2,
	NNCG_SOLVER = 4,
	MULTIBODY_SOLVER = 8,
	BLOCK_SOLVER = 16,
};
//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			IConstraintSolver
*************************************************************************//**
*  @class     IConstraintSolver
*  @brief     solver interface
*****************************************************************************/
class IConstraintSolver
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void   PrepareSolve(int numBodies, int numManifolds) {};
	virtual float SolveGroup(CollisionObject** bodies, int numBodies, PersistentManifold** manifold, int numManifolds, TypedConstraint** constraints, int numConstraints, const ContactSolverInfo& infomation, IDispatcher* dispatcher);
	virtual void  AllSolved(const ContactSolverInfo& infomation);
	virtual void  Reset() = 0;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual ConstraintSolverType GetSolverType() const = 0;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	virtual ~IConstraintSolver() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

#endif