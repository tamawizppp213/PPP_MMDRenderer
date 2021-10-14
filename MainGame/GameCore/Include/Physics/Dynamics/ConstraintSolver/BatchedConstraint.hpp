//////////////////////////////////////////////////////////////////////////////////
///             @file   BatchedConstraint.hpp
///             @brief  Batched Constraint
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


#pragma once
#ifndef BATCHED_CONSTRAINT_HPP
#define BATCHED_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "SolverBody.hpp"
#include "SolverConstraint.hpp"
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"

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
struct BatchedConstraints
{
	enum class BatchingMethod
	{
		BATCHING_METHOD_SPATIAL_GRID_2D,
		BATCHING_METHOD_SPATIAL_GRID_3D,
		BATCHING_METHOD_COUNT
	};

	struct Range
	{
		int Begin;
		int End;
		Range() : Begin(0), End(0) {}
		Range(int begin, int end) : Begin(begin), End(end) {}
	};
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void SetUp(ConstraintArray* constraints,
		const gm::AlignedObjectArray<SolverBody>& bodies,
		BatchingMethod batchingMethod,
		int minBatchSize,
		int maxBatchSize,
		gm::AlignedObjectArray<char>* scratchMemory);

	void Validate(ConstraintArray* constraints, const gm::AlignedObjectArray<SolverBody>& bodies) const;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<int>   ConstraintIndices; 
	gm::AlignedObjectArray<Range> Batches;// each batch is a range of indices in the m_constraintIndices array
	gm::AlignedObjectArray<Range> Phases;  // each phase is range of indices in the m_batches array
	gm::AlignedObjectArray<char>  PhaseGrainSize;// max grain size for each phase
	gm::AlignedObjectArray<int>   PhaseOrder;  // phases can be done in any order, so we can randomize the order here

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	BatchedConstraints() {};
	
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

#endif