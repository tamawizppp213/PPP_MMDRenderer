//////////////////////////////////////////////////////////////////////////////////
///             @file   DiscreteCollisionDetectorInterface.hpp
///             @brief  Discrete Collision Detector Interface
///             @author Partially edit by Toide
///             @date   2021_09_16
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
#ifndef DISCRETE_COLLISION_DETECTOR_INTERFACE_HPP
#define DISCRETE_COLLISION_DETECTOR_INTERFACE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMTransform.hpp"
#include "GameCore/Include/Physics/Util/IDebugDraw.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                       ICollisionAlgorithm
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			 ICollisionAlgorithm
*************************************************************************//**
*  @class     Dispatcher
*  @brief    CollisionAlgorithm is an collision interface that is compatible
*            with the Broadphase and btDispatcher. It is persistent over frames
*****************************************************************************/
struct DiscreteCollisionDetectorInterface
{
	struct Result
	{
		virtual ~Result() {};
		virtual void SetShapeIdentifiersA(int partID0, int index0) = 0;
		virtual void SetShapeIdentifiersB(int partID0, int index1) = 0;
		virtual void AddContactPoint(const gm::Vector3& normalOnBInWorld, const gm::Vector3& pointInWorld, float depth) = 0;

	};

	struct ClosestPointInput
	{
		ClosestPointInput() { MaximumDistanceSquared = FLT_MAX; }
		gm::AffineTransform TransformA;
		gm::AffineTransform TransformB;
		float MaximumDistanceSquared;
	};

	virtual ~DiscreteCollisionDetectorInterface() {};
	virtual void GetClosestPoints(const ClosestPointInput& input, Result& output, IDebugDraw* debugDraw,  bool swapResults = false) = 0;

};

struct StorageResult : public DiscreteCollisionDetectorInterface::Result
{
	gm::Vector3 NormalOnSurfaceB;
	gm::Vector3 ClosestPointInB;
	float Distance;

protected:
	StorageResult() :Distance(FLT_MAX) {};
public:
	virtual ~StorageResult() {};
	virtual void AddContactPoint(const gm::Vector3& normalOnBInWorld, const gm::Vector3& pointInWorld, float depth)
	{
		if (depth < Distance)
		{
			NormalOnSurfaceB = normalOnBInWorld;
			ClosestPointInB  = pointInWorld;
			Distance         = depth;
		}
	}
};
#endif