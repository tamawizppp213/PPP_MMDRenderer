//////////////////////////////////////////////////////////////////////////////////
///             @file   ContactConstraint.hpp
///             @brief  Contact Constraint
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
#ifndef CONTACT_CONSTRAINT_HPP
#define CONTACT_CONSTRAINT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "TypedConstraint.hpp"
#include "JacobianEntry.hpp"
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/PersistentManifold.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			ContactConstraint
*************************************************************************//**
*  @class     ContactConstraint
*  @brief     ContactConstraint can be automatically created to solve contact constraints using the unified btTypedConstraint interface
*****************************************************************************/
__declspec(align(16))
class ContactConstraint : public TypedConstraint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void BuildJacobian() override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void GetInfo1(ConstraintInfo1* infomation) override;
	void GetInfo2(ConstraintInfo2* infomation) override;
	PersistentManifold*       GetContactManifold()       { return &_contactManifold; }
	const PersistentManifold* GetContactManifold() const { return &_contactManifold; }
	void SetContactManifold(PersistentManifold* contactManifold);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	virtual ~ContactConstraint() {};
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	ContactConstraint(PersistentManifold* contactManifold, RigidBody& rigidbodyA, RigidBody& rigidbodyB);


	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	PersistentManifold _contactManifold;

};

///very basic collision resolution without friction
float ResolveSingleCollision(RigidBody* body1, class CollisionObject* colObj2, const gm::Vector3& contactPositionWorld, const gm::Vector3& contactNormalOnB, const struct ContactSolverInfo& solverInfo, float distance);

///resolveSingleBilateral is an obsolete methods used for vehicle friction between two dynamic objects
void ResolveSingleBilateral(RigidBody& body1, const gm::Vector3& pos1,
	RigidBody& body2, const gm::Vector3& pos2,
	float distance, const gm::Vector3& normal, float& impulse, float timeStep);
#endif