//////////////////////////////////////////////////////////////////////////////////
///             @file   ManifoldResult.hpp
///             @brief  Manifold Result
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
#ifndef MANIFOLD_RESULT_HPP
#define MANIFOLD_RESULT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/PersistentManifold.hpp"
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/DiscreteCollisionDetectorInterface.hpp"
#include "GameCore/Include/Physics/Collision/CollisionDispatch/CollisionObject.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using ContactAddedCallback      = bool (*)(ManifoldPoint& point, const CollisionObjectWrapper* collisionObjectWrapper, int partID0, int index0, const CollisionObjectWrapper* collisionObjectWrapper1, int partID1, int index1);
using CalculateCombinedCallback = float (*)(const CollisionObject* body0, const CollisionObject* body1);

extern ContactAddedCallback      g_ContactAddedCallback;
extern CalculateCombinedCallback g_CalculateCombinedRestitutionCallback;
extern CalculateCombinedCallback g_CalculateCombinedFrictionCallback;
extern CalculateCombinedCallback g_CalculateCombinedRollingFrictionCallback;
extern CalculateCombinedCallback g_CalculateCombinedSpinningFrictionCallback;
extern CalculateCombinedCallback g_CalculateCombinedContactDampingCallback;
extern CalculateCombinedCallback g_CalculateCombinedContactStiffnessCallback;
//////////////////////////////////////////////////////////////////////////////////
//                       Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			 ManifoldResult
*************************************************************************//**
*  @class     ManifoldResult
*  @brief     Manifold result 
*****************************************************************************/
class ManifoldResult : public DiscreteCollisionDetectorInterface::Result
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void AddContactPoint(const gm::Vector3& normalOnBInWorld, const gm::Vector3& pointInWorld, float depth);
	void RefreshContactPoints();

	static float CalculateCombinedRestitution     (const CollisionObject* body0, const CollisionObject* body1);
	static float CalculateCombinedFriction        (const CollisionObject* body0, const CollisionObject* body1);
	static float CalculateCombinedRollingFriction (const CollisionObject* body0, const CollisionObject* body1);
	static float CalculateCombinedSpinningFriction(const CollisionObject* body0, const CollisionObject* body1);
	static float CalculateCombinedContactDamping  (const CollisionObject* body0, const CollisionObject* body1);
	static float CalculateCombinedContactStiffness(const CollisionObject* body0, const CollisionObject* body1);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	float ClosestPointDistanceThreshold;

	const PersistentManifold*     GetPersistentManifold() const { return _manifold; }
	PersistentManifold*           GetPersistentManifold()       { return _manifold; }
	const CollisionObjectWrapper* GetBody0Wrap()          const { return _body0Wrap; }
	const CollisionObjectWrapper* GetBody1Wrap()          const { return _body1Wrap; }
	const CollisionObject* GetBody0Internal() const { return _body0Wrap->GetCollisionObject(); }
	const CollisionObject* GetBody1Internal() const { return _body1Wrap->GetCollisionObject(); }

	void SetPersistentManifold(PersistentManifold* manifold) { _manifold = manifold; }
	void SetBody0Wrap(const CollisionObjectWrapper* obj0Wrap) { _body0Wrap = obj0Wrap; }
	void SetBody1Wrap(const CollisionObjectWrapper* obj1Wrap) { _body1Wrap = obj1Wrap; }
	virtual void SetShapeIdentifiersA(int partID0, int index0) { _partID0 = partID0; _index0 = index0; }
	virtual void SetShapeIdentifiersB(int partID1, int index1) { _partID1 = partID1; _index1 = index1; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ManifoldResult() = default;
	ManifoldResult(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap);
	virtual ~ManifoldResult() {};

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	PersistentManifold* _manifold;
	const CollisionObjectWrapper* _body0Wrap;
	const CollisionObjectWrapper* _body1Wrap;
	int _partID0;
	int _partID1;
	int _index0;
	int _index1;


};
#endif