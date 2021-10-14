//////////////////////////////////////////////////////////////////////////////////
///             @file   GMAlignedAllocator.hpp
///             @brief  Aligned Allocator
///             @author Bullet likeなアロケータになっているので, 念のため記述
///             @date   
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
#ifndef MOTION_STATE_HPP
#define MOTION_STATE_HPP


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMTransform.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*					MotionState
*************************************************************************//**
*  @class    MotionState
*  @brief    MotionState interface class allows the dynamics world to synchronize
             and interpolate the updated world transforms with graphics
*****************************************************************************/
class IMotionState
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual void GetWorldTransform(gm::AffineTransform& worldTrans) const = 0;
	virtual void SetWorldTransform(gm::AffineTransform& worldTrans) = 0;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	virtual ~IMotionState() {};
};

/****************************************************************************
*					MotionState
*************************************************************************//**
*  @class    DefaultMotionState
*  @brief    MotionState class allows the dynamics world to synchronize
			 and interpolate the updated world transforms with graphics
*****************************************************************************/
ALIGNED16(struct)
DefaultMotionState : public IMotionState
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void GetWorldTransform(gm::AffineTransform& centerOfMassWorldTrans) const override
	{
		//centerOfMassWorldTrans = _graphicsWorldTransform * gm::Inverse(_centerOfMassOffset.GetMatrix());
	}
	virtual void SetWorldTransform(gm::AffineTransform& centerOfMassWorldTrans) override
	{
		_graphicsWorldTransform = centerOfMassWorldTrans * _centerOfMassOffset;
	}

	gm::AffineTransform _graphicsWorldTransform;
	gm::AffineTransform _centerOfMassOffset;
	gm::AffineTransform _startWorldTransform;
	void* _userPointer;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DefaultMotionState(const gm::AffineTransform& startTrans = gm::AffineTransform(), const gm::AffineTransform& centerOfMassOffset = gm::AffineTransform())
	{
		_graphicsWorldTransform = startTrans;
		_centerOfMassOffset     = centerOfMassOffset;
		_startWorldTransform    = startTrans;
		_userPointer            = nullptr;
	}
};
#endif