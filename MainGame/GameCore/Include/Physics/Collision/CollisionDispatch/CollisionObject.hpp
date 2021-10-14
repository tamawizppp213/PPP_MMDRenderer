//////////////////////////////////////////////////////////////////////////////////
//            @file　　  CollisionObject.hpp
//            @brief    collision object  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_09_14
//                      2021_10_11 (second check)
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
#ifndef COLLISION_OBJECT_HPP
#define COLLISION_OBJECT_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"
#include "GameMath/Include/GMTransform.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct  BroadPhaseProxy;
class   ICollisionShape;
#define CONVEX_DISTANCE_MARGIN (0.04f)

#define ACTIVE_TAG            1
#define ISLAND_SLEEPING       2
#define WANTS_DEACTIVATION    3
#define DISABLE_DEACTIVATION  4
#define DISABLE_SIMULATION    5
#define FIXED_BASE_MULTI_BODY 6

#define DECLARE_STACK_ONLY_OBJECT \
private:                             \
	void* operator new(size_t size); \
	void operator delete(void*);

enum class CollisionFlags
{
	Dynamic           = 0x0000,
	Static            = 0x0001,
	Kinematic         = 0x0002,
	NoContactResponse = 0x0004,
	CustomMaterialCallback        = 0x0008,
	CharacterObject               = 0x0010,
	DisableVisualObject           = 0x0020,
	DisableSPUCollisionProcessing = 0x0040,
	HasContactStiffnessDamping    = 0x0080,
	HasCustomDebugRenderingColor  = 0x0100,
	HasFrictionAnchor             = 0x0200,
	HasCollisionSoundTrigger      = 0x0400
};

enum class CollisionObjectTypes
{
	CollisionObject = 0x0001,
	RigidBody       = 0x0002,
	GhostObject     = 0x0004
};

enum class AnisotropicFrictionFlags
{
	FrictionDisabled = 0,
	Friction         = 1,
	RollingFriction  = 2
};
//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*					CollisionWorld
*************************************************************************//**
*  @class     CollisionWorld
*  @brief     CollisionWorld
*****************************************************************************/
__declspec(align(16)) 
class CollisionObject
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Activate(bool forceActivation = false) const;

	INLINE bool MergesSimulationIslands() const {return ((_collisionFlags & ((int)CollisionFlags::Static | (int)CollisionFlags::Kinematic | (int)CollisionFlags::NoContactResponse)) == 0);
	}
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::AffineTransform& GetWorldTransform() { return _worldTransform; }
	const gm::AffineTransform& GetWorldTransform() const { return _worldTransform; }
	gm::AffineTransform&       GetInterpolationWorldTransform()       { return _interpolationWorldTransform; }
	const gm::AffineTransform& GetInterpolationWorldTransform() const { return _interpolationWorldTransform; }
	INLINE bool IsStaticObject           () const { return (_collisionFlags & (int)CollisionFlags::Static) != 0; }
	INLINE bool IsKinematicObject        () const { return (_collisionFlags & (int)CollisionFlags::Kinematic) != 0; }
	INLINE bool IsStaticOrKinematicObject() const { return (_collisionFlags & ((int)CollisionFlags::Static | (int)CollisionFlags::Kinematic)) != 0; }
	INLINE bool HasContactResponse       () const { return (_collisionFlags & (int)CollisionFlags::NoContactResponse) != 0; }
	INLINE int  GetInternalType          () const { return _internalType; }
	INLINE int  GetActivationState       () const { return _activationState1; }
	INLINE float GetRollingFriction      () const { return _rollingFriction; }
	INLINE float GetSpinningFriction     () const { return _spinningFriction; }
	INLINE float GetFriction             () const { return _friction; }
	INLINE float GetRestitution          () const { return _restitution; }
	INLINE float GetContactStiffness     () const { return _contactStiffness; }
	INLINE float GetContactDamping       () const { return _contactDamping; }
	INLINE float GetCCDSweptSphereRadius () const { return _ccdSweptSphereRadius; }
	INLINE float GetCCDMotionThreshold   () const { return _ccdMotionThreshold; }
	INLINE float GetCCDSquareMotionThreshold() const { return _ccdMotionThreshold * _ccdMotionThreshold; }
	INLINE float GetDeactivationTime        () const { return _deactivationTime; }
	INLINE float GetHitFraction             () const { return _hitFraction; }
	INLINE int   GetCollisionFlags                    () const { return _collisionFlags; }
	INLINE const ICollisionShape* GetCollisionShape    () const { return _collisionShape; }
	INLINE       ICollisionShape* GetCollisionShape    ()       { return _collisionShape; }
	INLINE       BroadPhaseProxy* GetBroadPhaseHandler()       { return _broadPhaseHandler; }
	INLINE const BroadPhaseProxy* GetBroadPhaseHandler() const { return _broadPhaseHandler; }
	INLINE int   GetUpdateRevisionInternal            () const { return _updateRevision; }
	INLINE int   GetIslandTag                         () const { return _islandTag1; }
	INLINE int GetCompanionID                         () const { return _companionID; }
	INLINE int GetWorldArrayIndex                     () const { return _worldArrayIndex; }
	float        GetContactProcessingThreshold        () const { return _contactProcessingThreshold; }
	int GetNumObjectsWithoutCollision                 () const { return _objectsWithoutCollisionCheck.size(); }
	void* InternalGetExtensionPointer                 () const { return _extensionPointer; }
	void* GetUserPointer() const { return _userObjectPointer; }
	int   GetUserIndex () const { return _userIndex; }
	int   GetUserIndex2() const { return _userIndex2; }
	int   GetUserIndex3() const { return _userIndex3; }

	const gm::Vector3& GetInterpolationLinearVelocity () const { return _interpolationLinearVelocity; }
	const gm::Vector3& GetInterpolationAngularVelocity() const { return _interpolationAngularVelocity; }
	const gm::Vector3& GetAnisotropicFriction         () const { return _anisotropicFriction; }
	const CollisionObject* GetObjectWithoutCollision(int index) { return _objectsWithoutCollisionCheck[index]; }

	virtual bool CheckCollideWithOverride(const CollisionObject* collisionObject) const;
	bool CheckCollideWith(const CollisionObject* collisionObject) const;

	virtual void SetCollisionShape(ICollisionShape* collisionShape);
	void SetCollisionFlags  (int flags)         { _collisionFlags   = flags; }
	void SetDeactivationTime(float time)        { _deactivationTime = time; }
	void SetRestitution     (float rest)        { _updateRevision++; _restitution = rest; }
	void SetFriction        (float friction)    { _updateRevision++; _friction    = friction; }
	void SetRollingFriction (float friction)    { _updateRevision++; _friction    = friction; }
	void SetSpinningFriction(float friction)    { _updateRevision++; _friction    = friction; }
	void SetHitFraction     (float hitFraction) { _hitFraction = hitFraction; }
	void SetWorldTransform  (const gm::AffineTransform& worldTransform) { _updateRevision++; _worldTransform = worldTransform; }
	void SetCCDSweptSphereRadius(float radius) { _ccdSweptSphereRadius = radius; }
	void SetCCDMotionThreshold(float ccdMotionThreshold) { _ccdMotionThreshold = ccdMotionThreshold; }
	void SetAnisotropicFriction(const gm::Vector3& anisotropicFriction, int frictionMode = (int)AnisotropicFrictionFlags::Friction);
	void SetActivationState(int newState) const;
	void SetBroadPhaseHandler(BroadPhaseProxy* handler) { _broadPhaseHandler = handler; }
	void SetContactProcessingThreshold(float contactProcessingThreshold) { _contactProcessingThreshold = contactProcessingThreshold; }
	void SetIslandTag      (int tag)   { _islandTag1 = tag; }
	void SetCompanionID    (int id)    { _companionID = id; }
	void SetWorldArrayIndex(int index) { _worldArrayIndex = index; }
	void SetUserPointer(void* userPointer) { _userObjectPointer = userPointer; }
	void SetUserIndex (int index) { _userIndex  = index; }
	void SetUserIndex2(int index) { _userIndex2 = index; }
	void SetUserIndex3(int index) { _userIndex3 = index; }

	void ForceActivationState(int newState) const;
	void InternalSetExtensionPointer(void* pointer) { _extensionPointer = pointer; }
	
	void SetInterpolationWorldTransform(const gm::AffineTransform& transform);
	void SetInterpolationLinearVelocity(const gm::Vector3& linearVelocity);
	void SetInterpolationAngularVelocity(const gm::Vector3& angularVelocity);

	void SetIgnoreCollisionCheck(const CollisionObject* collisionObject, bool ignoreCollisionCheck);

	void SetContactStiffnessAndDamping(float stiffness, float damping);
	bool IsActive() const { return ((GetActivationState() != FIXED_BASE_MULTI_BODY) && (GetActivationState() != ISLAND_SLEEPING) && (GetActivationState() != DISABLE_SIMULATION));}
	

	bool HasAnisotropicFriction(int frictionMode = (int)AnisotropicFrictionFlags::Friction){return (_hasAnisotropicFriction & frictionMode) != 0;}

#pragma region Debug
	void SetCustomDebugColor(const gm::Vector3& colorRGB);
	void RemoveCustomDebugColor();
	bool GetCustomDebugColor(gm::Vector3& colorRGB) const;
#pragma endregion Debug
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	CollisionObject();
	virtual ~CollisionObject() {};
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::AffineTransform _worldTransform;
	gm::AffineTransform _interpolationWorldTransform; // _interpolationWorldTransform is used for CCD and interpolation
	
	//those two are experimental: just added for bullet time effect, so you can still apply impulses (directly modifying velocities)
	//without destroying the continuous interpolated motion (which uses this interpolation velocities)
	gm::Vector3 _interpolationLinearVelocity;
	gm::Vector3 _interpolationAngularVelocity;
	gm::Vector3 _anisotropicFriction;
	bool  _hasAnisotropicFriction;
	float _contactProcessingThreshold;

	BroadPhaseProxy* _broadPhaseHandler;
	ICollisionShape* _collisionShape;
	void*            _extensionPointer; // _extensionPointer is used by some internal low-level bullet extensions

	ICollisionShape* _rootCollisionShape;
	int _collisionFlags;

	int _islandTag1;
	int _companionID;
	int _worldArrayIndex;

	mutable int   _activationState1;
	mutable float _deactivationTime;

	float _friction;
	float _restitution;
	float _rollingFriction;
	float _spinningFriction;
	float _contactDamping;
	float _contactStiffness;

	int _internalType; // m_internalType is reserved to distinguish Bullet's CollisionObject, RigidBody, SoftBody, GhostObject etc

	void* _userObjectPointer;
	int   _userIndex2;
	int   _userIndex;
	int   _userIndex3;

	float _hitFraction; // time of impact calculation
	float _ccdSweptSphereRadius;
	float _ccdMotionThreshold;
	int   _checkCollideWith;

	gm::AlignedObjectArray<const CollisionObject*> _objectsWithoutCollisionCheck;
	int _updateRevision;///internal update revision number. It will be increased when the object changes. This allows some subsystems to perform lazy evaluation.
	gm::Vector3 _customDebugColorRGB;
};

/****************************************************************************
*					CollisionObjectWrapper
*************************************************************************//**
*  @class     CollisionObjectWrapper
*  @brief     CollisionObjectWrapper
*****************************************************************************/
struct CollisionObjectWrapper;
struct CollisionObjectWrapper
{
	DECLARE_STACK_ONLY_OBJECT;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	INLINE const gm::AffineTransform& GetWorldTransform   () const { return WorldTransform; }
	INLINE const CollisionObject* GetCollisionObject() const { return CollisionObj; }
	INLINE const ICollisionShape* GetCollisionShape  () const { return Shape; }

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const CollisionObjectWrapper* Parent;
	const ICollisionShape*        Shape;
	const CollisionObject*        CollisionObj;
	const gm::AffineTransform&          WorldTransform;
	const gm::AffineTransform*          PreviousTransform;
	int PartID;
	int Index;

	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	CollisionObjectWrapper(const CollisionObjectWrapper* parent, const ICollisionShape* shape, const CollisionObject* collisionObject, const gm::AffineTransform& worldTransform, int partID, int index)
		: Parent(parent), Shape(shape), CollisionObj(collisionObject), WorldTransform(worldTransform),PreviousTransform(NULL), PartID(partID), Index(index){}
	CollisionObjectWrapper(const CollisionObjectWrapper* parent, const ICollisionShape* shape, const CollisionObject* collisionObject, const gm::AffineTransform& worldTransform, const gm::AffineTransform& preTransform, int partID, int index)
		: Parent(parent), Shape(shape), CollisionObj(collisionObject), WorldTransform(worldTransform), PreviousTransform(&preTransform), PartID(partID), Index(index){ }
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	CollisionObjectWrapper(const CollisionObjectWrapper&); // not implemented. not allowed
	CollisionObjectWrapper* operator=(const CollisionObjectWrapper&) {};


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

using CollisionObjectArray = gm::AlignedObjectArray<class CollisionObject*>;
#endif