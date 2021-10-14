//////////////////////////////////////////////////////////////////////////////////
///             @file   SolverConstraint.hpp
///             @brief  Solver Constraint
///             @author Partially edit by Toide
///             @date   2021_09_230
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
#ifndef SOLVER_CONSTRAINT_HPP
#define SOLVER_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "JacobianEntry.hpp"
#include "SolverBody.hpp"
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			SolverConstraint
*************************************************************************//**
*  @class     SolverConstraint
*  @brief     1D constraint along a normal axis between bodyA and bodyB. 
              It can be combined to solve contact and friction constraints.
*****************************************************************************/
__declspec(align(16))
struct SolverConstraint
{
public:
	enum SolverConstraintType
	{
		Contact1D = 0,
		Friction1D
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3 RelativePosition2CrossNormal;
	gm::Vector3 ContactNormal1;
	gm::Vector3 RelativePosition3CrossNormal;
	gm::Vector3 ContactNormal2;
	gm::Vector3 AngularComponentA;
	gm::Vector3 AngularComponentB;
	mutable float AppliedPushImpulse;
	mutable float AppliedImpulse;
	float Friction;
	float JacobianDiagABInverse;
	
	float RHS;
	float CFM;
	float LowerLimit;
	float UpperLimit;
	
	float RHSPenetration;
	union
	{
		void* OriginalContactPoint;
		float UnUsedPadding[3];
		int RowsForNonContactConstraintCount;
	};

	int OverrideSolverIterationsCount;
	int FrictionIndex;
	int SolverBodyIDA;
	int SolverBodyIDB;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

using ConstraintArray = gm::AlignedObjectArray<SolverConstraint>;
#endif