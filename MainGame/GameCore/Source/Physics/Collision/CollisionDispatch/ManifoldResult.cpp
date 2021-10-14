//////////////////////////////////////////////////////////////////////////////////
///             @file   ManifoldResult.cpp
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

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionDispatch/ManifoldResult.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define INVALID_ID (-1)
ContactAddedCallback g_ContactAddedCallback = 0;

CalculateCombinedCallback g_CalculateCombinedRestitutionCallback      = &ManifoldResult::CalculateCombinedRestitution;
CalculateCombinedCallback g_CalculateCombinedFrictionCallback         = &ManifoldResult::CalculateCombinedFriction;
CalculateCombinedCallback g_CalculateCombinedRollingFrictionCallback  = &ManifoldResult::CalculateCombinedRollingFriction;
CalculateCombinedCallback g_CalculateCombinedSpinningFrictionCallback = &ManifoldResult::CalculateCombinedSpinningFriction;
CalculateCombinedCallback g_CalculateCombinedContactDampingCallback   = &ManifoldResult::CalculateCombinedContactDamping;
CalculateCombinedCallback g_CalculateCombinedContactStiffnessCallback = &ManifoldResult::CalculateCombinedContactStiffness;
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Static Function

float ManifoldResult::CalculateCombinedRestitution(const CollisionObject* body0, const CollisionObject* body1)
{
	return body0->GetRestitution() * body1->GetRestitution();
}

float ManifoldResult::CalculateCombinedFriction(const CollisionObject* body0, const CollisionObject* body1)
{
	float friction = body0->GetFriction() * body1->GetFriction();

	const float maxFriction = 10.0f;
	if (friction < -maxFriction) { friction = -maxFriction; }
	if (friction > maxFriction) { friction = maxFriction; }
	return friction;

}

float ManifoldResult::CalculateCombinedRollingFriction(const CollisionObject* body0, const CollisionObject* body1)
{
	float friction = body0->GetRollingFriction() * body1->GetFriction() + body1->GetRollingFriction() * body0->GetFriction();

	const float maxFriction = 10.0f;
	if (friction < -maxFriction) { friction = -maxFriction; }
	if (friction > maxFriction)  { friction = maxFriction; }
	return friction;

	
}

float ManifoldResult::CalculateCombinedSpinningFriction(const CollisionObject* body0, const CollisionObject* body1)
{
	float friction = body0->GetSpinningFriction() * body1->GetFriction() + body1->GetSpinningFriction() * body0->GetFriction();

	const float maxFriction = 10.0f;
	if (friction < -maxFriction) { friction = -maxFriction; }
	if (friction > maxFriction)  { friction = maxFriction; }
	return friction;
}

float ManifoldResult::CalculateCombinedContactDamping(const CollisionObject* body0, const CollisionObject* body1)
{
	return body0->GetRestitution() * body1->GetRestitution();
}

float ManifoldResult::CalculateCombinedContactStiffness(const CollisionObject* body0, const CollisionObject* body1)
{
	float s0 = body0->GetContactStiffness();
	float s1 = body1->GetContactStiffness();

	float temp0 = 1.0f / s0;
	float temp1 = 1.0f / s1;
	float combinedStiffness = 1.0f / (temp0 + temp1);
	return combinedStiffness;
}
#pragma endregion Static Function

#pragma region Public Function
ManifoldResult::ManifoldResult(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap)
{
	_manifold  = nullptr;
	_body0Wrap = body0Wrap;
	_body1Wrap = body1Wrap;
	_partID0   = INVALID_ID;
	_partID1   = INVALID_ID;
	_index0    = INVALID_ID;
	_index1    = INVALID_ID;
	ClosestPointDistanceThreshold = 0;
}

void ManifoldResult::AddContactPoint(const gm::Vector3& normalOnBInWorld, const gm::Vector3& pointInWorld, float depth)
{
	assert(_manifold);

	if (depth > _manifold->GetContactBreakingThreshold()) { return; }

	bool isSwapped      = _manifold->GetBody0() != _body0Wrap->GetCollisionObject();
	bool isNewCollision = _manifold->GetContactCount() == 0;

	Vector3 pointA = pointInWorld + normalOnBInWorld * depth;
	Vector3 localA;
	Vector3 localB;

	if (isSwapped)
	{
		auto transformA = _body1Wrap->GetCollisionObject()->GetWorldTransform();
		auto transformB = _body0Wrap->GetCollisionObject()->GetWorldTransform();

		Vector3 va = pointA       - transformA.GetTranslation();
		Vector3 vb = pointInWorld - transformB.GetTranslation();

		localA = Transpose(transformA.GetBasis()) * va;
		localB = Transpose(transformB.GetBasis()) * vb;
	}
	else
	{
		auto transformA = _body0Wrap->GetCollisionObject()->GetWorldTransform();
		auto transformB = _body1Wrap->GetCollisionObject()->GetWorldTransform();

		Vector3 va = pointA       - transformA.GetTranslation();
		Vector3 vb = pointInWorld - transformB.GetTranslation();

		// noscaling (ŠÔˆá‚Á‚Ä‚½‚çC³)
		localA = Transpose(transformA.GetBasis()) * va;
		localB = Transpose(transformB.GetBasis()) * vb;
	}

	ManifoldPoint newPoint(localA, localB, normalOnBInWorld, depth);
	newPoint.PositionWorldOnA = pointA;
	newPoint.PositionWorldOnB = pointInWorld;

	int insertIndex = _manifold->GetCacheEntry(newPoint);
	newPoint.CombinedFriction         = g_CalculateCombinedFrictionCallback        (_body0Wrap->GetCollisionObject(), _body1Wrap->GetCollisionObject());
	newPoint.CombinedRestitution      = g_CalculateCombinedRestitutionCallback     (_body0Wrap->GetCollisionObject(), _body1Wrap->GetCollisionObject());
	newPoint.CombinedRollingFriction  = g_CalculateCombinedRollingFrictionCallback (_body0Wrap->GetCollisionObject(), _body1Wrap->GetCollisionObject());
	newPoint.CombinedSpinningFriction = g_CalculateCombinedSpinningFrictionCallback(_body0Wrap->GetCollisionObject(), _body1Wrap->GetCollisionObject());

	if ((_body0Wrap->GetCollisionObject()->GetCollisionFlags() & (int)CollisionFlags::HasContactStiffnessDamping) ||
		(_body1Wrap->GetCollisionObject()->GetCollisionFlags() & (int)CollisionFlags::HasContactStiffnessDamping))
	{
		newPoint.CombinedContactDamping1   = g_CalculateCombinedContactDampingCallback(_body0Wrap->GetCollisionObject(), _body1Wrap->GetCollisionObject());
		newPoint.CombinedContactStiffness1 = g_CalculateCombinedContactStiffnessCallback(_body0Wrap->GetCollisionObject(), _body1Wrap->GetCollisionObject());
		newPoint.ContactPointFlag |= (int)ContactPointFlags::Contact_Stiffness_Damping;
	}

	if ((_body0Wrap->GetCollisionObject()->GetCollisionFlags() & (int)CollisionFlags::HasFrictionAnchor) ||
		(_body1Wrap->GetCollisionObject()->GetCollisionFlags() & (int)CollisionFlags::HasFrictionAnchor))
	{
		newPoint.ContactPointFlag |= (int)ContactPointFlags::Friction_Anchor;
	}

	PlaneSpace(newPoint.NormalWorldOnB, newPoint.LateralFrictionDirection1, newPoint.LateralFrictionDirection2);

	//BP mod, store contact triangles.
	if (isSwapped)
	{
		newPoint.PartID0 = _partID1;
		newPoint.PartID1 = _partID0;
		newPoint.Index0  = _index1;
		newPoint.Index1  = _index0;
	}
	else
	{
		newPoint.PartID0 = _partID0;
		newPoint.PartID1 = _partID1;
		newPoint.Index0 = _index0;
		newPoint.Index1 = _index1;
	}

	if (insertIndex >= 0)
	{
		_manifold->ReplaceContactPoint(newPoint, insertIndex);
	}
	else
	{
		insertIndex = _manifold->AddManifoldPoint(newPoint);
	}

	//User can override friction and/or restitution
	if (g_ContactAddedCallback &&
		//and if either of the two bodies requires custom material
		((_body0Wrap->GetCollisionObject()->GetCollisionFlags() & (int)CollisionFlags::CustomMaterialCallback) ||
			(_body1Wrap->GetCollisionObject()->GetCollisionFlags() & (int)CollisionFlags::CustomMaterialCallback)))
	{
		//experimental feature info, for per-triangle material etc.
		const CollisionObjectWrapper* obj0Wrap = isSwapped ? _body1Wrap : _body0Wrap;
		const CollisionObjectWrapper* obj1Wrap = isSwapped ? _body0Wrap : _body1Wrap;
		(*g_ContactAddedCallback)(_manifold->GetContactPoint(insertIndex), obj0Wrap, newPoint.PartID0, newPoint.Index0, obj1Wrap, newPoint.PartID1, newPoint.Index1);
	}

	if (g_ContactStartedCallback && isNewCollision)
	{
		g_ContactStartedCallback(_manifold);
	}
}

void ManifoldResult::RefreshContactPoints()
{
	assert(_manifold);
	if (!_manifold->GetContactCount()) { return; }

	bool isSwapped = _manifold->GetBody0() != _body0Wrap->GetCollisionObject();
	if (isSwapped)
	{
		_manifold->RefreshContactPoints(_body1Wrap->GetCollisionObject()->GetWorldTransform(), _body0Wrap->GetCollisionObject()->GetWorldTransform());
	}
	else
	{
		_manifold->RefreshContactPoints(_body0Wrap->GetCollisionObject()->GetWorldTransform(), _body1Wrap->GetCollisionObject()->GetWorldTransform());
	}
}
//#pragma endregion Public Function