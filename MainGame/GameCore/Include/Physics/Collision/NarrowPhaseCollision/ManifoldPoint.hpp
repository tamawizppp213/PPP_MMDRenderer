//////////////////////////////////////////////////////////////////////////////////
///             @file   ManifoldPoint.hpp
///             @brief  ManifoldPoint 
///             @author Partially Edit by Toide
///             @date   2021_09_15
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
#ifndef MANIFOLD_POINT_HPP
#define MANIFOLD_POINT_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMVector.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class ContactPointFlags
{
	Lateral_Friction_Initialized = 0x0001,
	Has_Contact_CFM              = 0x0002,
	Has_Contact_ERP              = 0x0004,
	Contact_Stiffness_Damping    = 0x0008,
	Friction_Anchor              = 0x0010
};

//////////////////////////////////////////////////////////////////////////////////
//                             Manifold point
//////////////////////////////////////////////////////////////////////////////////
__declspec(align(16))
struct ConstraintRow
{
	float Normal[3];
	float Rhs;
	float JacDiagInverse;
	float LowerLimit;
	float UpperLimit;
	float AccumulationImpulse;
};

class ManifoldPoint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const gm::Vector3& GetPositionWorldOnA() const { return PositionWorldOnA; }
	const gm::Vector3& GetPositionWorldOnB() const { return PositionWorldOnB; }
	float GetAppliedImpulse() const { return AppliedImpulse; }
	int   GetLifeTime      () const { return LifeTime; }
	float GetDistance      () const { return Distance1; }
	void  SetDistance(float distance) { Distance1 = distance; }
	gm::Vector3 LocalPointA;
	gm::Vector3 LocalPointB;
	gm::Vector3 PositionWorldOnA;
	gm::Vector3 PositionWorldOnB;
	gm::Vector3 NormalWorldOnB; 

	float Distance1;
	float CombinedFriction;
	float CombinedRollingFriction;  //torsional friction orthogonal to contact normal, useful to make spheres stop rolling forever
	float CombinedSpinningFriction; //torsional friction around contact normal, useful for grasping objects
	float CombinedRestitution;

	int   PartID0;
	int   PartID1;
	int   Index0;
	int   Index1;

	mutable void* UserPersistentData;
	int ContactPointFlag;

	float AppliedImpulse;
	float PreviousRHS;
	float AppliedImpulseLateral1;
	float AppliedImpulseLateral2;
	float ContactMotion1;
	float ContactMotion2;

	union
	{
		float ContactCFM;
		float CombinedContactStiffness1;
	};

	union
	{
		float ContactERP;
		float CombinedContactDamping1;
	};

	float FrictionCFM;

	int LifeTime; // lifetime of the contactpoint in frames

	gm::Vector3 LateralFrictionDirection1;
	gm::Vector3 LateralFrictionDirection2;



	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ManifoldPoint() = default;
	ManifoldPoint(const gm::Vector3& pointA, const gm::Vector3& pointB, const gm::Vector3& normal, float distance)
	{
		LocalPointA               = pointA;
		LocalPointB               = pointB;
		NormalWorldOnB            = normal;
		PositionWorldOnB          = gm::Vector3();
		Distance1                 = distance;
		CombinedFriction          = 0.0f;
		CombinedRollingFriction   = 0.0f;
		CombinedSpinningFriction  = 0.0f;
		CombinedRestitution       = 0.0f;
		PartID0                   = -1;
		PartID1                   = -1;
		Index0                    = -1;
		Index1                    = -1;
		UserPersistentData        = nullptr;
		ContactPointFlag          = 0;
		AppliedImpulse            = 0.0f;
		PreviousRHS               = 0.0f;
		AppliedImpulseLateral1    = 0.0f;
		AppliedImpulseLateral2    = 0.0f;
		ContactMotion1            = 0.0f;
		ContactMotion2            = 0.0f;
		ContactCFM                = 0.0f;
		ContactERP                = 0.0f;
		FrictionCFM               = 0.0f;
		LifeTime                  = 0;
		LateralFrictionDirection1 = gm::Vector3(0,0,0);
		LateralFrictionDirection2 = gm::Vector3(0, 0, 0);
	}
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif