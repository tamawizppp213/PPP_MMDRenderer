//////////////////////////////////////////////////////////////////////////////////
//            @file　　  CollisionObject.hpp
//            @brief    Collision Object
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit by Toide
//            @date  :  2021_09_14
//                      2021_10_11 second checked
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

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionDispatch/CollisionObject.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define FLT_EPSILON 1.192092896e-07F 

//////////////////////////////////////////////////////////////////////////////////
//							Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
CollisionObject::CollisionObject()
{
	_interpolationLinearVelocity  = Vector3(0.0f, 0.0f, 0.0f);
	_interpolationAngularVelocity = Vector3(0.0f, 0.0f, 0.0f);
	_anisotropicFriction          = Vector3(1.0f, 1.0f, 1.0f);
	_hasAnisotropicFriction       = false;
	_contactProcessingThreshold   = FLT_MAX;
	_broadPhaseHandler            = NULL;
	_collisionShape               = NULL;
	_extensionPointer             = NULL;
	_rootCollisionShape           = NULL;
	_collisionFlags              = (int)CollisionFlags::Static;
	_islandTag1                  = -1;
	_companionID                 = -1;
	_worldArrayIndex             = -1;
	_activationState1            = 1;
	_deactivationTime            = 0.0f;
	_friction                    = 0.5f;
	_restitution                 = 0.0f;
	_rollingFriction             = 0.0f;
	_spinningFriction            = 0.0f;
	_contactDamping              = 0.1f;
	_contactStiffness            = FLT_MAX;
	_internalType                = (int)CollisionObjectTypes::CollisionObject;
	_userObjectPointer           = NULL;
	_userIndex                   = -1;
	_userIndex2                  = -1;
	_userIndex3                  = -1;
	_hitFraction                 = 1.0f;
	_ccdSweptSphereRadius        = 0.0f;
	_ccdMotionThreshold          = 0.0f;
	_checkCollideWith            = false;
	_updateRevision              = 0;
	_worldTransform              = AffineTransform();
	_interpolationWorldTransform = AffineTransform();
}
#pragma region Property

void CollisionObject::SetInterpolationWorldTransform(const gm::AffineTransform& transform)
{
	_updateRevision++;
	_interpolationWorldTransform = transform;
}
void CollisionObject::SetInterpolationLinearVelocity(const gm::Vector3& linearVelocity)
{
	_updateRevision++;
	_interpolationLinearVelocity;
}
void CollisionObject::SetInterpolationAngularVelocity(const gm::Vector3& angularVelocity)
{
	_updateRevision++;
	_interpolationAngularVelocity;
}

void CollisionObject::SetContactStiffnessAndDamping(float stiffness, float damping)
{
	_updateRevision++;
	_contactStiffness = stiffness;
	_contactDamping = damping;
	_collisionFlags = (int)CollisionFlags::HasContactStiffnessDamping;
	if (_contactStiffness < FLT_EPSILON)
	{
		_contactStiffness = FLT_EPSILON;
	}
}
/****************************************************************************
*						SetActivationState
*************************************************************************//**
*  @fn        void CollisionObject::SetActivationState(int newState) const
*  @brief     Set activation state1
*  @param[in] int newState
*  @return 　　void
*****************************************************************************/
void CollisionObject::SetActivationState(int newState) const
{
	if ((_activationState1 != DISABLE_DEACTIVATION) && (_activationState1 != DISABLE_SIMULATION))
		_activationState1 = newState;
}
/****************************************************************************
*						ForceActivationState
*************************************************************************//**
*  @fn        void CollisionObject::ForceActivationState(int newState) const
*  @brief     Force Set activation state1. (non check)
*  @param[in] int newState
*  @return 　　void
*****************************************************************************/
void CollisionObject::ForceActivationState(int newState) const
{
	_activationState1 = newState;
}
/****************************************************************************
*						Activate
*************************************************************************//**
*  @fn        void CollisionObject::Activate(bool forceActivation) const
*  @brief     Activation this object
*  @param[in] int newState
*  @return 　　void
*****************************************************************************/
void CollisionObject::Activate(bool forceActivation) const
{
	if (forceActivation || !(_collisionFlags & ((int)CollisionFlags::Static | (int)CollisionFlags::Kinematic)))
	{
		SetActivationState(ACTIVE_TAG);
		_deactivationTime = 0.0f;
	}
}

void CollisionObject::SetCollisionShape(ICollisionShape* collisionShape)
{
	_updateRevision++;
	_collisionShape     = collisionShape;
	_rootCollisionShape = collisionShape;
}

void CollisionObject::SetAnisotropicFriction(const gm::Vector3& anisotropicFriction, int frictionMode)
{

	_anisotropicFriction = anisotropicFriction;
	bool isUnity = (anisotropicFriction.GetX() != 1.f) || (anisotropicFriction.GetY() != 1.f) || (anisotropicFriction.GetZ() != 1.f);
}
void CollisionObject::SetIgnoreCollisionCheck(const CollisionObject* collisionObject, bool ignoreCollisionCheck)
{
	if (ignoreCollisionCheck)
	{
		_objectsWithoutCollisionCheck.push_back(collisionObject);
	}
	else
	{
		_objectsWithoutCollisionCheck.remove(collisionObject);
	}
	_checkCollideWith = _objectsWithoutCollisionCheck.size() > 0;
}
/****************************************************************************
*						CheckCollideWithOverride
*************************************************************************//**
*  @fn        bool CollisionObject::CheckCollideWithOverride(const CollisionObject* collisionObject) const
*  @brief     CheckCollideWithOverride
*  @param[in] CollisionObject* collisionObject*
*  @return 　　bool
*****************************************************************************/
bool CollisionObject::CheckCollideWithOverride(const CollisionObject* collisionObject) const
{
	int index = _objectsWithoutCollisionCheck.findLinearSearch(collisionObject);
	if (index < _objectsWithoutCollisionCheck.size())
	{
		return false;
	}
	return true;
}
/****************************************************************************
*						CheckCollideWith
*************************************************************************//**
*  @fn        bool CollisionObject::CheckCollideWith(const CollisionObject* collisionObject) const
*  @brief     CheckCollideWith
*  @param[in] CollisionObject* collisionObject*
*  @return 　　bool
*****************************************************************************/
bool CollisionObject::CheckCollideWith(const CollisionObject* collisionObject) const
{
	if (_checkCollideWith) { return CheckCollideWithOverride(collisionObject); }
	return true;
}

#pragma region Debug
/****************************************************************************
*						RemoveCustomDebugColor
*************************************************************************//**
*  @fn        void CollisionObject::RemoveCustomDebugColor()
*  @brief     Remove custom debug color
*  @param[out] void
*  @return 　　bool has custom color
*****************************************************************************/
void CollisionObject::SetCustomDebugColor(const Vector3& colorRGB)
{
	_customDebugColorRGB = colorRGB;
	_collisionFlags |= (int)CollisionFlags::HasCustomDebugRenderingColor;
}
/****************************************************************************
*						RemoveCustomDebugColor
*************************************************************************//**
*  @fn        void CollisionObject::RemoveCustomDebugColor()
*  @brief     Remove custom debug color
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void CollisionObject::RemoveCustomDebugColor()
{
	_collisionFlags &= ~(int)CollisionFlags::HasCustomDebugRenderingColor;
}
/****************************************************************************
*						GetCustomDebugColor
*************************************************************************//**
*  @fn        bool CollisionObject::GetCustomDebugColor(Vector3& colorRGB) const
*  @brief     Get debug color (return hasCustomColor)
*  @param[out] Vector3 colorRGB
*  @return 　　bool has custom color
*****************************************************************************/
bool CollisionObject::GetCustomDebugColor(Vector3& colorRGB) const
{
	bool hasCustomColor = (0 != (_collisionFlags & (int)CollisionFlags::HasCustomDebugRenderingColor));
	if (hasCustomColor)
	{
		colorRGB = _customDebugColorRGB;
	}
	return hasCustomColor;
}
#pragma endregion Debug
#pragma endregion Property
#pragma endregion Public Function