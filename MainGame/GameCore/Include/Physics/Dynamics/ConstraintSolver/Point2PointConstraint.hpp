//////////////////////////////////////////////////////////////////////////////////
///             @file   Point2PointConstraint.hpp
///             @brief  Point to Point Constraint
///             @author Partially edit by Toide
///             @date   2021_09_30
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
#ifndef POINT2POINT_CONSTRAINT_HPP
#define POINT2POINT_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "TypedConstraint.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class RigidBody;

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
struct ConstraintSetting
{
	ConstraintSetting() : Tau(0.3f), Damping(1.0f), ImpulseClamp(0.0f) {};

	float Tau;
	float Damping;
	float ImpulseClamp;
};

enum class Point2PointFlags
{
	ERP = 1,
	CFM = 2
};
/****************************************************************************
*				  			Point2PointConstraint
*************************************************************************//**
*  @class     Point2PointConstraint
*  @brief     Point constraint (connected between A point and B point) usecase : chandelier
*             Point fixed. Rotation allowed
*             pivot:  object local connected point
*****************************************************************************/
__declspec(align(16))
class Point2PointConstraint : public TypedConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void UpdateRHS(float timeStep);


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool UseSolveConstraintObsolete;
	ConstraintSetting Setting;

	void  BuildJacobian() override;
	void  GetInfo1(ConstraintInfo1* infomation) override;
	void  GetInfo1NonVirtual(ConstraintInfo1* infomation);
	void  GetInfo2(ConstraintInfo2* infomation) override;
	void  GetInfo2NonVirtual(ConstraintInfo2* infomation, const gm::Transform& body0Transform, const gm::Transform& body1Transform);
	float GetParam(int num, int axis = -1) const;
	int   GetFlags() const { return _flags; }
	const gm::Vector3& GetPivotInA() const { return _pivotInA; }
	const gm::Vector3& GetPivotInB() const { return _pivotInB; }

	void SetPivotA(const gm::Vector3& pivotA) { _pivotInA = pivotA; }
	void SetPivotB(const gm::Vector3& pivotB) { _pivotInB = pivotB; }
	void SetParam(int num, float value, int axis = -1) override;
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	Point2PointConstraint(RigidBody& rigidbodyA, RigidBody& rigidbodyB, const gm::Vector3& pivotInA, const gm::Vector3& pivotInB);
	Point2PointConstraint(RigidBody& rigidbodyA, const gm::Vector3& pivotInA);


private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	JacobianEntry _jacobianEntry[3];

	gm::Vector3 _pivotInA;
	gm::Vector3 _pivotInB;

	int _flags;
	float _erp;
	float _cfm;
};
#endif