//////////////////////////////////////////////////////////////////////////////////
///             @file   Point2PointConstraint.cpp
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

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Dynamics/ConstraintSolver/Point2PointConstraint.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
Point2PointConstraint::Point2PointConstraint(RigidBody& rigidbodyA, RigidBody& rigidbodyB, const gm::Vector3& pivotInA, const gm::Vector3& pivotInB) : TypedConstraint(ConstraintType::Point2Point, rigidbodyA, rigidbodyB),
_pivotInA(pivotInA), _pivotInB(pivotInB), _flags(0), UseSolveConstraintObsolete(false)
{

}
Point2PointConstraint::Point2PointConstraint(RigidBody& rigidbodyA, const Vector3& pivotInA) : TypedConstraint(ConstraintType::Point2Point, rigidbodyA),
_pivotInA(pivotInA), _pivotInB(rigidbodyA.GetCenterOfMassTransform() * pivotInA),
_flags(0), UseSolveConstraintObsolete(false)
{

}
/****************************************************************************
*						UpdateRHS 
*************************************************************************//**
*  @fn        void Point2PointConstraint::UpdateRHS(float timeStep)
*  @brief     --
*  @param[in] flota timestep
*  @return 　　void
*****************************************************************************/
void Point2PointConstraint::UpdateRHS(float timeStep)
{
	
}
/****************************************************************************
*						BuildJacobian
*************************************************************************//**
*  @fn        void  Point2PointConstraint::BuildJacobian()
*  @brief     --
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void  Point2PointConstraint::BuildJacobian()
{

}
/****************************************************************************
*						GetInfo1
*************************************************************************//**
*  @fn        void  Point2PointConstraint::GetInfo1(ConstraintInfo1* infomation)
*  @brief     --
*  @param[in, out] ConstraintInfo1* infomation
*  @return 　　void
*****************************************************************************/
void  Point2PointConstraint::GetInfo1(ConstraintInfo1* infomation)
{

}
/****************************************************************************
*						GetInfo1NonVirtual
*************************************************************************//**
*  @fn        void CollisionDispatcher::ClearManifold(PersistentManifold* manifold)
*  @brief     --
*  @param[in, out] PersistentManifold* manifold
*  @return 　　void
*****************************************************************************/
void  Point2PointConstraint::GetInfo1NonVirtual(ConstraintInfo1* infomation)
{

}
/****************************************************************************
*						GetInfo2
*************************************************************************//**
*  @fn        void Point2PointConstraint::GetInfo2(ConstraintInfo2* infomation)
*  @brief     --
*  @param[in, out] ConstraintInfo2* infomation
*  @return 　　void
*****************************************************************************/
void  Point2PointConstraint::GetInfo2(ConstraintInfo2* infomation)
{

}
/****************************************************************************
*						GetInfo2NonVirtual
*************************************************************************//**
*  @fn        void  Point2PointConstraint::GetInfo2NonVirtual(ConstraintInfo2* infomation, const Transform& body0Transform, const gm::Transform& body1Transform)
*  @brief     --
*  @param[in, out] ConstraintInfo2* infomation
*  @param[in] Transform body0Transform, 
*  @param[in] Transform body1Transform
*  @return 　　void
*****************************************************************************/
void  Point2PointConstraint::GetInfo2NonVirtual(ConstraintInfo2* infomation, const Transform& body0Transform, const gm::Transform& body1Transform)
{

}
/****************************************************************************
*						GetParam
*************************************************************************//**
*  @fn        float Point2PointConstraint::GetParam(int num, int axis) const
*  @brief     --
*  @param[in] int num 
*  @param[in] int axis
*  @return 　　float
*****************************************************************************/
float Point2PointConstraint::GetParam(int num, int axis) const
{
	return true;
}
/****************************************************************************
*						SetParam
*************************************************************************//**
*  @fn        void Point2PointConstraint::SetParam(int num, float value, int axis)
*  @brief     --
*  @param[in] int num
*  @param[in] int value
*  @param[in] int axis
*  @return 　　void
*****************************************************************************/
void Point2PointConstraint::SetParam(int num, float value, int axis)
{

}