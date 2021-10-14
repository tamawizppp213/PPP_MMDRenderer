//////////////////////////////////////////////////////////////////////////////////
///             @file   GearConstraint.hpp
///             @brief  Gear Constraint
///             @author Partially edit by Toide
///             @date   2021_10_01
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2012 Advanced Micro Devices, Inc.  http://bulletphysics.org

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
#ifndef GEAR_CONSTRAINT_HPP
#define GEAR_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "TypedConstraint.hpp"

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
class GearConstraint : public TypedConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void GetInfo1(ConstraintInfo1* infomation) override;
	void GetInfo2(ConstraintInfo2* infomation) override;
	const gm::Vector3& GetAxisA() const { return _axisInA; }
	const gm::Vector3& GetAxisB() const { return _axisInB; }
	float GetRatio             () const { return _ratio; }
	void SetAxisA(gm::Vector3& axisA);
	void SetAxisB(gm::Vector3& axisB);
	void SetRatio(float ratio);

	float GetParam(int num, int axis = -1) const override { (void)num;  (void)axis; assert(0); return 0.0f; }
	void SetParam(int num, float value, int axis = -1)override { (void)num; (void)value; (void)axis; assert(NULL); }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	GearConstraint(RigidBody& rigidBodyA, RigidBody& rigidBodyB, const gm::Vector3& axisInA, const gm::Vector3& axisInB, float ratio = 1.0f);
	virtual ~GearConstraint();

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::Vector3& _axisInA;
	gm::Vector3& _axisInB;
	bool         _userFrameA;
	float        _ratio;
};

#endif