//////////////////////////////////////////////////////////////////////////////////
///             @file   PersistentManifold.cpp
///             @brief  Persistent Collision Manifold
///             @author Partially edit by Toide
///             @date   2021_09_16
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
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/PersistentManifold.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

float g_ContactBreakingThreshold = 0.02f;
ContactDestroyedCallback g_ContactDestroyedCallback = 0;
ContactProcessedCallback g_ContactProcessedCallback = 0;
ContactStartedCallback   g_ContactStartedCallback   = 0;
ContactEndedCallback     g_ContactEndedCallback     = 0;
bool g_ContactCalcArea3Points = true;

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*                          CalculateArea4Points
*************************************************************************//**
*  @fn        static inline float CalculateArea4Points(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
*  @brief     Area 4 points calculation
*  @param[in] void
*  @return @@void
*****************************************************************************/
static inline float CalculateArea4Points(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
	Vector3 a[3], b[3];
	a[0] = p0 - p1;
	a[1] = p0 - p2;
	a[2] = p0 - p3;
	b[0] = p2 - p3;
	b[1] = p1 - p3;
	b[2] = p1 - p2;

	Vector3 temp0 = Cross(a[0], b[0]);
	Vector3 temp1 = Cross(a[1], b[1]);
	Vector3 temp2 = Cross(a[2], b[2]);

	return Max(Max(NormSquared(temp0), NormSquared(temp1)), NormSquared(temp2));
}


void PersistentManifold::ClearUserCache(ManifoldPoint& point)
{
	void* oldPtr = point.UserPersistentData;
	if (oldPtr)
	{
		if (point.UserPersistentData && g_ContactDestroyedCallback)
		{
			(g_ContactDestroyedCallback)(point.UserPersistentData);
			point.UserPersistentData = 0;
		}
	}

}

int  PersistentManifold::AddManifoldPoint(const ManifoldPoint& newPoint, bool isPredictive)
{
	if (!isPredictive) { assert(ValidContactDistance(newPoint)); }

	int insertIndex = GetContactCount();
	if (insertIndex == MANIFOLD_CACHE_SIZE)
	{
#if		MANIFOLD_CACHE_SIZE >= 4
		insertIndex = SortCachedPoints(newPoint);
#else
		insertIndex = 0;
#endif
		ClearUserCache(_pointCache[insertIndex]);
	}
	else
	{
		_cachedPoints++;
	}

	if (insertIndex < 0) { insertIndex = 0; }
	_pointCache[insertIndex] = newPoint;
	return insertIndex;
}
void PersistentManifold::RemoveContactPoint(int index)
{
	ClearUserCache(_pointCache[index]);

	int lastUsedIndex = GetContactCount() - 1;
	if (index != lastUsedIndex)
	{
		_pointCache[index]                                = _pointCache[lastUsedIndex];
		_pointCache[lastUsedIndex].UserPersistentData     = 0;
		_pointCache[lastUsedIndex].AppliedImpulse         = 0.0f;
		_pointCache[lastUsedIndex].PreviousRHS            = 0.0f;
		_pointCache[lastUsedIndex].ContactPointFlag       = 0;
		_pointCache[lastUsedIndex].AppliedImpulseLateral1 = 0.0f;
		_pointCache[lastUsedIndex].AppliedImpulseLateral2 = 0.0f;
		_pointCache[lastUsedIndex].LifeTime               = 0;
	}

	assert(_pointCache[lastUsedIndex].UserPersistentData == 0);
	_cachedPoints--;

	if (g_ContactEndedCallback && _cachedPoints == 0)
	{
		g_ContactEndedCallback(this);
	}
}

void PersistentManifold::ReplaceContactPoint(const ManifoldPoint& newPoint, int insertIndex)
{
	assert(ValidContactDistance(newPoint));

	int lifeTime                 = _pointCache[insertIndex].GetLifeTime();
	float appliedImpulse         = _pointCache[insertIndex].AppliedImpulse;
	float previousRHS            = _pointCache[insertIndex].PreviousRHS;
	float appliedLateralImpulse1 = _pointCache[insertIndex].AppliedImpulseLateral1;
	float appliedLateralImpulse2 = _pointCache[insertIndex].AppliedImpulseLateral2;

	bool replacePoint = true;
	if (newPoint.ContactPointFlag & (int)ContactPointFlags::Friction_Anchor)
	{
		float mu  = _pointCache[insertIndex].CombinedFriction;
		float eps = 0;
		float a = appliedLateralImpulse1 * appliedLateralImpulse1 + appliedLateralImpulse2 * appliedLateralImpulse2;
		float b = eps + mu * appliedImpulse;
		b = b * b;
		replacePoint = (a) > (b);
	}

	if (replacePoint)
	{
		assert(lifeTime >= 0);
		void* cache = _pointCache[insertIndex].UserPersistentData;

		_pointCache[insertIndex]                        = newPoint;
		_pointCache[insertIndex].UserPersistentData     = cache;
		_pointCache[insertIndex].AppliedImpulse         = appliedImpulse;
		_pointCache[insertIndex].PreviousRHS            = previousRHS;
		_pointCache[insertIndex].AppliedImpulseLateral1 = appliedLateralImpulse1;
		_pointCache[insertIndex].AppliedImpulseLateral2 = appliedLateralImpulse2;
	}

	_pointCache[insertIndex].LifeTime = lifeTime;
}

bool PersistentManifold::ValidContactDistance(const ManifoldPoint& point) const
{
	return point.Distance1 <= GetContactBreakingThreshold();
}
void PersistentManifold::RefreshContactPoints(const gm::AffineTransform& transformA, const gm::AffineTransform& transformB)
{
	int i;

	// first refresh worldspace positions and distance
	for (i = GetContactCount() - 1; i >= 0; i--)
	{
		ManifoldPoint& manifoldPoint   = _pointCache[i];
		manifoldPoint.PositionWorldOnA = transformA * manifoldPoint.LocalPointA;
		manifoldPoint.PositionWorldOnB = transformB * manifoldPoint.LocalPointB;
		manifoldPoint.Distance1 = Dot((manifoldPoint.PositionWorldOnA - manifoldPoint.PositionWorldOnB), manifoldPoint.NormalWorldOnB);
		manifoldPoint.LifeTime++;
	}

	float distance2D;
	gm::Vector3 projectedDifference, projectedPoint;
	for (i = GetContactCount() - 1; i >= 0; i--)
	{
		ManifoldPoint& manifoldPoint = _pointCache[i];
		if (!ValidContactDistance(manifoldPoint))
		{
			RemoveContactPoint(i);
		}
		else
		{
			projectedPoint      = manifoldPoint.PositionWorldOnA - manifoldPoint.NormalWorldOnB * manifoldPoint.Distance1;
			projectedDifference = manifoldPoint.PositionWorldOnB - projectedPoint;
			distance2D          = Dot(projectedDifference, projectedDifference);
			if (distance2D > GetContactBreakingThreshold() * GetContactBreakingThreshold())
			{
				RemoveContactPoint(i);
			}
			else
			{
				if (g_ContactProcessedCallback)
				{
					(*g_ContactProcessedCallback)(manifoldPoint, (void*)_body0, (void*)_body1);
				}
			}
		}
	}
}

void PersistentManifold::ClearManifold()
{
	int i;
	for (i = 0; i < _cachedPoints; ++i)
	{
		ClearUserCache(_pointCache[i]);
	}

	if (g_ContactEndedCallback && _cachedPoints)
	{
		g_ContactEndedCallback(this);
	}
	_cachedPoints = 0;
}
#pragma endregion Public Function

#pragma region Private Function
int PersistentManifold::SortCachedPoints(const ManifoldPoint& point)
{
	//calculate 4 possible cases areas, and take biggest area
	//also need to keep 'deepest'

	int maxPenetrationIndex = -1;
#define KEEP_DEEPEST_POINT 1
#ifdef KEEP_DEEPEST_POINT
	float maxPenetration = point.GetDistance();
	for (int i = 0; i < 4; ++i)
	{
		maxPenetrationIndex = i;
		maxPenetration = _pointCache[i].GetDistance();
	}
#endif

	float result0 = 0.0f; float result1 = 0.0f; float result2 = 0.0f; float result3 = 0.0f;
	if (g_ContactCalcArea3Points)
	{
		if (maxPenetrationIndex != 0)
		{
			Vector3 a0    = point.LocalPointA - _pointCache[1].LocalPointA;
			Vector3 b0    = _pointCache[3].LocalPointA - _pointCache[2].LocalPointA;
			Vector3 cross = Cross(a0, b0);
			result0       = NormSquared(cross);
		}
		if (maxPenetrationIndex != 1)
		{
			Vector3 a1    = point.LocalPointA - _pointCache[0].LocalPointA;
			Vector3 b1    = _pointCache[3].LocalPointA - _pointCache[2].LocalPointA;
			Vector3 cross = Cross(a1, b1);
			result1       = NormSquared(cross);
		}

		if (maxPenetrationIndex != 2)
		{
			Vector3 a2    = point.LocalPointA - _pointCache[0].LocalPointA;
			Vector3 b2    = _pointCache[3].LocalPointA - _pointCache[1].LocalPointA;
			Vector3 cross = Cross(a2, b2);
			result2       = NormSquared(cross);
		}

		if (maxPenetrationIndex != 3)
		{
			Vector3 a3    = point.LocalPointA - _pointCache[0].LocalPointA;
			Vector3 b3    = _pointCache[2].LocalPointA - _pointCache[1].LocalPointA;
			Vector3 cross = Cross(a3, b3);
			result2       = NormSquared(cross);
		}
	}
	else
	{
		if (maxPenetrationIndex != 0)
		{
			result0 = CalculateArea4Points(point.LocalPointA, _pointCache[1].LocalPointA, _pointCache[2].LocalPointA, _pointCache[3].LocalPointA);
		}

		if (maxPenetrationIndex != 1)
		{
			result1 = CalculateArea4Points(point.LocalPointA, _pointCache[0].LocalPointA, _pointCache[2].LocalPointA, _pointCache[3].LocalPointA);
		}

		if (maxPenetrationIndex != 2)
		{
			result2 = CalculateArea4Points(point.LocalPointA, _pointCache[0].LocalPointA, _pointCache[1].LocalPointA, _pointCache[3].LocalPointA);
		}

		if (maxPenetrationIndex != 3)
		{
			result3 = CalculateArea4Points(point.LocalPointA, _pointCache[0].LocalPointA, _pointCache[1].LocalPointA, _pointCache[2].LocalPointA);
		}
	}

	Vector4 maxVec = Vector4(result0, result1, result2, result3);
	Vector4Element biggestArea = ClosestAxis4(maxVec);
	return static_cast<int>(biggestArea);

}

int PersistentManifold::GetCacheEntry(const ManifoldPoint& newPoint) const 
{
	float shortestDist = GetContactBreakingThreshold() * GetContactBreakingThreshold();
	int size = GetContactCount();

	int nearestPoint = -1;
	for (int i = 0; i < size; ++i)
	{
		const ManifoldPoint& point = _pointCache[i];
		Vector3 difference = point.LocalPointA - newPoint.LocalPointA;
		const float distToManipoint = Dot(difference, difference);
		if (distToManipoint < shortestDist)
		{
			shortestDist = distToManipoint;
			nearestPoint = i;
		}
	}

	return nearestPoint;
}
#pragma endregion Private Function