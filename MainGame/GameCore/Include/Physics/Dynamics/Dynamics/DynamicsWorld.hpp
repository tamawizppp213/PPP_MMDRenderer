//////////////////////////////////////////////////////////////////////////////////
//            @file　　  DynamicsWorld.hpp
//            @brief    Set world  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  ----------
//            @date  :  ----------
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

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
#ifndef DYNAMICS_WORLD_HPP
#define DYNAMICS_WORLD_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
//#include "GameCore/Include/Physics/Collision/CollisionDispatch/"
#include "GameMath/Include/GMVector.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class RigidBody;
enum class DynamicsWorldType
{
	Simple   = 1,
	Discrete = 2
};

/****************************************************************************
*					    IDynamicsWorld
*************************************************************************//**
*  @class     IDynamicsWorld
*  @brief     This class is the interface class for several dynamics implementation, basic, discrete, parallel, and continuous etc.
*****************************************************************************/
class IDynamicsWorld 
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	// timeStep : [second], fixed TimeStep : default 
	virtual int  StepSmulation(double timeStep, int maxSubSteps = 1, double fixedTimeStep = 1.0 / 60.0f) = 0;
	virtual void DebugDrawWorld() = 0;
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual void AddConstraint   () {};
	virtual void RemoveConstraint() {};

	virtual void AddAction   () = 0;
	virtual void RemoveAction() = 0;

	virtual void        SetGravity(const gm::Vector3& gravity) = 0;
	virtual gm::Vector3 GetGravity() const                     = 0;

	virtual void SynchronizeMotionStates() = 0;

	virtual void AddRigidbody()          = 0;
	virtual void AddRigidbody(int group) = 0;
	virtual void RemoveRigidbody()       = 0;
	
	virtual void SetConstraintSolver() = 0;
	virtual void GetConstraintSolver() = 0;
	virtual int  GetConstraintsCount() const { return 0; }

	virtual void      GetConstraint(int index) {};
	virtual const int GetConstraint(int index) const {};

	virtual DynamicsWorldType GetWorldType() const = 0;
	
	virtual void ClearForces() = 0;

	void SetInternalTickCallback() {};

	void SetWorldUserInfo();
	void GetWorldUserInfo();
	void GetSolverInfo();
	const int GetSolverInfo() const;

	// vehicleとcharacterは後で.
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	IDynamicsWorld() {};
	virtual ~IDynamicsWorld() {};
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
};

/****************************************************************************
*					    DiscreteDynamicsWorld
*************************************************************************//**
*  @class     DiscreteDynamicsWorld
*  @brief     This class provides discrete rigidbody simulation.
*****************************************************************************/
class DiscreteDynamicsWorld : public IDynamicsWorld
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual void        SetGravity(const gm::Vector3& gravity) override;
	virtual gm::Vector3 GetGravity() const                     override;
	virtual void        ApplyGravity();
	virtual void        ClearForces()                          override;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	std::vector<RigidBody*> _nonStaticRigidBodies;
	gm::Vector3 _gravity;
	double      _localTime;
	double      _fixedTimeStep;

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif