//////////////////////////////////////////////////////////////////////////////////
//            @file　　  PersistentManifold.hpp
//            @brief    Persistent Collision Manifold 
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  ----------
//            @date  :  2021_09_15 Edit. (Toide)
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
#ifndef PERSISTENT_MANIFOLD_HPP
#define PERSISTENT_MONIFOLD_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/ManifoldPoint.hpp"
#include "GameMath/Include/GMTransform.hpp"
#include "GameMath/Include/GMAlighedAllocator.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define MANIFOLD_CACHE_SIZE (4)

class PersistentManifold;
extern float g_ContactBreakingThreshold;

using ContactDestroyedCallback  = bool (*)(void* userPersistentData);
using ContactProcessedCallback  = bool (*)(ManifoldPoint& cp, void* body0, void* body1);
using ContactStartedCallback    = void (*)(PersistentManifold* const& manifold);
using ContactEndedCallback      = void (*)(PersistentManifold* const& manifold);
extern ContactDestroyedCallback g_ContactDestroyedCallback;
extern ContactProcessedCallback g_ContactProcessedCallback;
extern ContactStartedCallback   g_ContactStartedCallback;
extern ContactEndedCallback     g_ContactEndedCallback;
enum class ContactManifoldTypes
{
	Min_Contact_Manifold_Type = 1024,
	Persistent_Manifold_Type
};
class CollisionObject;
//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			PersistentManifold
*************************************************************************//**
*  @class     PersistentManifold
*  @brief     Persistent Collision Manifold (PCM) class (note! : 16 bytes (for 16 alignment))
*  @detail    PCM is a fully distance-based collision detection system, which uses the Gilbert-Johnson-Keerthi (GJK) and Expanding Polytope (EPA)Algorithm
*             to calculate one contact at the point of deepest penetration
*****************************************************************************/
__declspec(align(16)) 
class PersistentManifold : public gm::utils::TypedObject
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	int  AddManifoldPoint   (const ManifoldPoint& newPoint, bool isPredictive = false);
	void RemoveContactPoint (int index);
	void ReplaceContactPoint(const ManifoldPoint& newPoint, int insertIndex);
	bool ValidContactDistance(const ManifoldPoint& point) const;
	void RefreshContactPoints(const gm::AffineTransform& transformA, const gm::AffineTransform& transformB);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	int CompanionIDA;
	int CompanionIDB;
	int Index1a;

	INLINE const CollisionObject* GetBody0() const { return _body0; }
	INLINE const CollisionObject* GetBody1() const { return _body1; }
	INLINE int GetContactCount            () const { return _cachedPoints; }
	INLINE const ManifoldPoint& GetContactPoint(int index) const { assert(index < _cachedPoints); return _pointCache[index]; }
	INLINE       ManifoldPoint& GetContactPoint(int index)       { assert(index < _cachedPoints); return _pointCache[index]; }

	float GetContactBreakingThreshold() const { return _contactBreakingThreshold; };
	float GetContactProcessingThreshold() const { return _contactProcessingThreshold; }
	int   GetCacheEntry(const ManifoldPoint& newPoint) const;

	void SetBodies(const CollisionObject* body0, const CollisionObject* body1) { _body0 = body0; _body1 = body1; }
	void SetContactsCount(int cachedPoints){ _cachedPoints = cachedPoints; }
	void SetContactBreakingThreshold  (float contactBreakingThreshold  ) { _contactBreakingThreshold = contactBreakingThreshold; }
	void SetContactProcessingThreshold(float contactProcessingThreshold) { _contactProcessingThreshold = contactProcessingThreshold; }


	void ClearUserCache(ManifoldPoint& point);
	void ClearManifold();
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	PersistentManifold() = default;
	PersistentManifold(const CollisionObject* body0, const CollisionObject* body1, int, float contactBreakingThreshold, float contactProcessingThreshold) : TypedObject((int)ContactManifoldTypes::Persistent_Manifold_Type)
	{
		_body0                      = body0;
		_body1                      = body1;
		_cachedPoints               = 0;
		_contactBreakingThreshold   = contactBreakingThreshold;
		_contactProcessingThreshold = contactProcessingThreshold;
		CompanionIDA                = 0;
		CompanionIDB                = 0;
		Index1a                     = 0;
	}
	~PersistentManifold() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	int SortCachedPoints(const ManifoldPoint& point);
	//int FindContactPoint(const ManifoldPoint* unUsed, int numUnused, const ManifoldPoint& point);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	ManifoldPoint _pointCache[MANIFOLD_CACHE_SIZE];

	const CollisionObject* _body0;
	const CollisionObject* _body1;

	int _cachedPoints;

	float _contactBreakingThreshold;
	float _contactProcessingThreshold;


};

#endif