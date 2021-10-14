//////////////////////////////////////////////////////////////////////////////////
//            @file　　  TotalCollisionAlgorithm.hpp
//            @brief    Implement Total collision Algorithms
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_03
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
#include "GameCore/Include/Physics/Collision/CollisionDispatch/TotalCollisionAlgorithm.hpp"
#include "GameCore/Include/Physics/Collision/CollisionDispatch/TotalCollisionDetector.hpp"
#include "GameCore/Include/Physics/Collision/CollisionDispatch/CollisionObject.hpp"
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/PersistentManifold.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/BoxShape.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/SphereShape.hpp"
#include "GameCore/Include/Physics/Collision/CollisionShapes/Box2dShape.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define USE_PERSISTENT_CONTACTS (1)
#define CLEAR_MANIFOLD (1)
using namespace gm;

struct ClipVertex
{
	Vector3 V;
	int     ID;
};
int b2_maxManifoldPoints = 2;
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Box_Box

BoxBoxCollisionAlgorithm::BoxBoxCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap)
	: IActivatingCollisionAlgorithm(ci, body0Wrap, body1Wrap)
{
	_ownManifold = false;
	_manifold = manifold;

	if (!_manifold && _dispatcher->NeedsCollision(body0Wrap->GetCollisionObject(), body1Wrap->GetCollisionObject()))
	{
		_manifold    = _dispatcher->GetNewManifold(body0Wrap->GetCollisionObject(), body1Wrap->GetCollisionObject());
		_ownManifold = true;
	}
}

BoxBoxCollisionAlgorithm::~BoxBoxCollisionAlgorithm()
{
	if (_ownManifold)
	{
		if (_manifold)
		{
			_dispatcher->ReleaseManifold(_manifold);
		}
	}
}
/****************************************************************************
*						ProcessCollision
*************************************************************************//**
*  @fn        void BoxBoxCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     Execute box box collision test
*  @param[in] CollisionObjectWrapper*  body0Wrap
*  @param[in] CollisionObjectWrapper*  body1Wrap
*  @param[in] DispatchInfo& dispatchInfo
*  @param[out] ManifoldResult* resultOut
*  @return 　　void
*****************************************************************************/
void BoxBoxCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	if (!_manifold) { return; }

	/*-------------------------------------------------------------------
	-              Get collision shape
	---------------------------------------------------------------------*/
	const BoxShape* box0 = (BoxShape*)body0Wrap->GetCollisionShape();
	const BoxShape* box1 = (BoxShape*)body1Wrap->GetCollisionShape();

	resultOut->SetPersistentManifold(_manifold); // report a contact.
#ifndef USE_PERSISTENT_CONTACTS
	_manifold->ClearManifold();
#endif

	/*-------------------------------------------------------------------
	-              Set ClosestPointInput
	---------------------------------------------------------------------*/
	DiscreteCollisionDetectorInterface::ClosestPointInput input;
	input.MaximumDistanceSquared = FLT_MAX;
	input.TransformA = body0Wrap->GetWorldTransform();
	input.TransformB = body1Wrap->GetWorldTransform();

	/*-------------------------------------------------------------------
	-              Get Box-Box detection result
	---------------------------------------------------------------------*/
	BoxBoxDetector detector(box0, box1);
	detector.GetClosestPoints(input, *resultOut, dispatchInfo.DebugDraw);
#ifdef USE_PERSISTENT_CONTACTS
	if (_ownManifold) { resultOut->RefreshContactPoints(); }
#endif
}
/****************************************************************************
*						CalculateTimeOfImpact
*************************************************************************//**
*  @fn        float BoxBoxCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     This function calculates time of impact
*  @param[in,out] CollisionObject*  body0Wrap
*  @param[in,out] CollisionObject*  body1Wrap
*  @param[in] DispatchInfo& dispatchInfo
*  @para,[out] ManifoldResult* resultOut
*  @return 　  float 
*****************************************************************************/
float BoxBoxCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	return 1.0f; // not yet
}
/****************************************************************************
*						GetAllContactManifolds
*************************************************************************//**
*  @fn        void BoxBoxCollisionAlgorithm::GetAllContactManifolds(ManifoldArray& manifoldArray)
*  @brief     This function Get all contact objects
*  @param[out] ManifoldArray& manifoldArray
*  @return 　　void
*****************************************************************************/
void BoxBoxCollisionAlgorithm::GetAllContactManifolds(ManifoldArray& manifoldArray)
{
	if (_manifold && _ownManifold) { manifoldArray.push_back(_manifold); }
}

#pragma endregion         Box_Box
#pragma region Sphere_Box
SphereBoxCollisionAlgorithm::SphereBoxCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, bool isSwapped)
	: IActivatingCollisionAlgorithm(ci, body0Wrap, body1Wrap)
{
	_ownManifold = false;
	_manifold = manifold;
	_isSwapped = isSwapped;

	const CollisionObjectWrapper* sphereObjWrap = _isSwapped ? body1Wrap : body0Wrap;
	const CollisionObjectWrapper* boxObjWrap    = _isSwapped ? body0Wrap : body1Wrap;

	if (!_manifold && _dispatcher->NeedsCollision(sphereObjWrap->GetCollisionObject(), boxObjWrap->GetCollisionObject()))
	{
		_manifold = _dispatcher->GetNewManifold(sphereObjWrap->GetCollisionObject(), boxObjWrap->GetCollisionObject());
		_manifold = _dispatcher->GetNewManifold(sphereObjWrap->GetCollisionObject(), boxObjWrap->GetCollisionObject());
		_ownManifold = true;
	}
}
SphereBoxCollisionAlgorithm::~SphereBoxCollisionAlgorithm()
{
	if (_ownManifold)
	{
		if (_manifold) { _dispatcher->ReleaseManifold(_manifold); }
	}
}
/****************************************************************************
*						ProcessCollision
*************************************************************************//**
*  @fn        void BoxBoxCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     Execute box box collision test
*  @param[in] CollisionObjectWrapper*  body0Wrap
*  @param[in] CollisionObjectWrapper*  body1Wrap
*  @param[in] DispatchInfo& dispatchInfo
*  @param[out] ManifoldResult* resultOut
*  @return 　　void
*****************************************************************************/
void SphereBoxCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	(void)dispatchInfo; (void)resultOut;
	if (!_manifold) { return; }

	const CollisionObjectWrapper* sphereObject = _isSwapped ? body1Wrap : body0Wrap;
	const CollisionObjectWrapper* boxObject    = _isSwapped ? body0Wrap : body1Wrap;

	Vector3 pointOnBox;
	Vector3 normalOnSurfaceB;
	float   penetrationDepth;
	Vector3 sphereCenter       = sphereObject->GetWorldTransform().GetTranslation();
	const SphereShape* sphere0 = (const SphereShape*)sphereObject->GetCollisionShape();
	float radius               = sphere0->GetRadius();
	float maxContactDistance   = _manifold->GetContactBreakingThreshold();

	resultOut->SetPersistentManifold(_manifold);

	if (GetSphereDistance(boxObject, pointOnBox, normalOnSurfaceB, penetrationDepth, sphereCenter, radius, maxContactDistance))
	{
		resultOut->AddContactPoint(normalOnSurfaceB, pointOnBox, penetrationDepth); // report a contact/
	}

	if (_ownManifold)
	{
		if (_manifold->GetContactCount())
		{
			resultOut->RefreshContactPoints();
		}
	}
}
/****************************************************************************
*						CalculateTimeOfImpact
*************************************************************************//**
*  @fn        float BoxBoxCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     This function calculates time of impact
*  @param[in,out] CollisionObject*  collision0
*  @param[in,out] CollisionObject*  collision1
*  @param[in] DispatchInfo& dispatchInfo
*  @para,[out] ManifoldResult* resultOut
*  @return 　  float
*****************************************************************************/
float SphereBoxCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* collision0, CollisionObject* collision1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	(void)resultOut; (void)dispatchInfo; (void)collision0; (void)collision1;
	return 1.0f;
}
/****************************************************************************
*						GetSphereDistance
*************************************************************************//**
*  @fn        bool SphereBoxCollisionAlgorithm::GetSphereDistance(const CollisionObjectWrapper* boxObjWrap, Vector3& pointOnBox, Vector3& normal, float& penetrationDepth, const Vector3& sphereCenter, float radius, float maxContactDistance)
*  @brief     Get sphere distance
*  @param[in]  CollisionObjectWrapper* boxObjectWrap, 
   @param[out] Vector3 pointOnBox, 
   @param[out] Vector3 normal, 
   @param[out] float penetrationDepth,
   @param[out] Vector3 sphereCenter, 
   @param[in]  float radius, 
   @param[in] float maxContactDistance
*  @return 　  bool
*****************************************************************************/
bool SphereBoxCollisionAlgorithm::GetSphereDistance(const CollisionObjectWrapper* boxObjectWrap, Vector3& pointOnBox, Vector3& normal, float& penetrationDepth, const Vector3& sphereCenter, float radius, float maxContactDistance)
{
	const BoxShape* boxShape     = (const BoxShape*)boxObjectWrap->GetCollisionShape();
	Vector3 const& boxHalfExtent = boxShape->GetHalfExtentsWithoutMargin();
	float boxMargin              = boxShape->GetMargin();
	penetrationDepth             = 1.0f;

	// convert the sphere position to the box's local space
	AffineTransform m44T           = boxObjectWrap->GetWorldTransform();
	Vector3 sphereRelativePosition = InverseXForm(m44T, sphereCenter);

	// Determine the closest point to the sphere center in the box
	Vector3 closestPoint = sphereRelativePosition;
	closestPoint.SetX(Min(boxHalfExtent.GetX(), closestPoint.GetX()));
	closestPoint.SetX(Max(-boxHalfExtent.GetX(), closestPoint.GetX()));
	closestPoint.SetY(Min(boxHalfExtent.GetY(), closestPoint.GetY()));
	closestPoint.SetY(Max(-boxHalfExtent.GetY(), closestPoint.GetY()));
	closestPoint.SetZ(Min(boxHalfExtent.GetZ(), closestPoint.GetZ()));
	closestPoint.SetZ(Max(-boxHalfExtent.GetZ(), closestPoint.GetZ()));

	float intersectionDist = radius + boxMargin;
	float contactDist      = intersectionDist + maxContactDistance;
	normal                 = sphereRelativePosition - closestPoint;

	float dist2 = NormSquared(normal);
	if (dist2 > contactDist * contactDist) { return false; }

	float distance;
	if (dist2 <= FLT_EPSILON)
	{
		distance = -GetSpherePenetration(boxHalfExtent, sphereRelativePosition, closestPoint, normal);
	}
	else
	{
		distance = Norm(normal);
		normal   = normal / distance;
	}

	pointOnBox = closestPoint + normal * boxMargin;
	penetrationDepth = distance - intersectionDist;

	Vector3 temp = m44T * pointOnBox;
	pointOnBox = temp;
	temp       = m44T.GetBasis() * normal;
	normal     = temp;

	return true;
}
/****************************************************************************
*						GetSpherePenetration
*************************************************************************//**
*  @fn        float SphereBoxCollisionAlgorithm::GetSpherePenetration(Vector3 const& boxHalfExtent, Vector3 const& sphereRelativePosition, Vector3& closestPoint, Vector3& normal)
*  @brief     Get sphere distance
*  @param[in]  CollisionObjectWrapper* boxHalfExtent
*  @param[in]  Vector3 sphereRelativePosition
*  @param[out] Vector3& closestPoint, 
*  @param[out] Vector3& normal
*  @return 　  float
*****************************************************************************/
float SphereBoxCollisionAlgorithm::GetSpherePenetration(Vector3 const& boxHalfExtent, Vector3 const& sphereRelativePosition, Vector3& closestPoint, Vector3& normal)
{
	//project the center of the sphere on the closest face of the box
	float faceDist = boxHalfExtent.GetX() - sphereRelativePosition.GetX();
	float minDist  = faceDist;
	closestPoint.SetX(boxHalfExtent.GetX());
	normal = Vector3(1.0f, 0.0f, 0.0f);

	faceDist = boxHalfExtent.GetX() + sphereRelativePosition.GetX();
	if (faceDist < minDist)
	{
		minDist      = faceDist;
		closestPoint = sphereRelativePosition;
		closestPoint.SetX(-boxHalfExtent.GetX());
		normal = Vector3(-1.0f, 0.0f, 0.0f);
	}

	faceDist = boxHalfExtent.GetY() - sphereRelativePosition.GetY();
	if (faceDist < minDist)
	{
		minDist      = faceDist;
		closestPoint = sphereRelativePosition;
		closestPoint.SetY(boxHalfExtent.GetY());
		normal       = Vector3(0.0f, 1.0f, 0.0f);
	}

	faceDist = boxHalfExtent.GetY() + sphereRelativePosition.GetY();
	if (faceDist < minDist)
	{
		minDist = faceDist;
		closestPoint = sphereRelativePosition;
		closestPoint.SetY(-boxHalfExtent.GetY());
		normal       = Vector3(0.0f, -1.0f, 0.0f);
	}

	faceDist = boxHalfExtent.GetZ() - sphereRelativePosition.GetZ();
	if (faceDist < minDist)
	{
		minDist      = faceDist;
		closestPoint = sphereRelativePosition;
		closestPoint.SetZ(boxHalfExtent.GetZ());
		normal = Vector3(0.0f, 0.0f, 1.0f);
	}

	faceDist = boxHalfExtent.GetZ() + sphereRelativePosition.GetZ();
	if (faceDist < minDist)
	{
		minDist = faceDist;
		closestPoint = sphereRelativePosition;
		closestPoint.SetZ(-boxHalfExtent.GetZ());
		normal = Vector3(0.0f, 0.0f, -1.0f);
	}

	return minDist;
}
void SphereBoxCollisionAlgorithm::GetAllContactManifolds(ManifoldArray& manifoldArray)
{
	if (_manifold && _ownManifold) { manifoldArray.push_back(_manifold); }
}
#pragma endregion      Sphere_Box
#pragma region Sphere_Sphere
SphereSphereCollisionAlgorithm::SphereSphereCollisionAlgorithm(const CollisionAlgorithmConstructionInfo& ci) 
	: IActivatingCollisionAlgorithm(ci)
{
}
SphereSphereCollisionAlgorithm::SphereSphereCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap)
	:IActivatingCollisionAlgorithm(ci, body0Wrap, body1Wrap)
{
	_ownManifold = false;
	_manifold    = manifold; 
	if (!_manifold)
	{
		_manifold    = _dispatcher->GetNewManifold(body0Wrap->GetCollisionObject(), body1Wrap->GetCollisionObject());
		_ownManifold = true;
	}
}
SphereSphereCollisionAlgorithm::~SphereSphereCollisionAlgorithm()
{
	if (_ownManifold)
	{
		if (_manifold)
		{
			_dispatcher->ReleaseManifold(_manifold);
		}
	}
}
/****************************************************************************
*						ProcessCollision
*************************************************************************//**
*  @fn        void SphereSphereCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     Execute sphere sphere collision test
*  @param[in] CollisionObjectWrapper*  body0Wrap
*  @param[in] CollisionObjectWrapper*  body1Wrap
*  @param[in] DispatchInfo& dispatchInfo
*  @param[out] ManifoldResult* resultOut
*  @return 　　void
*****************************************************************************/
void  SphereSphereCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	(void)dispatchInfo;
	if (!_manifold) { return; }

	resultOut->SetPersistentManifold(_manifold);

	SphereShape* sphere0 = (SphereShape*)body0Wrap->GetCollisionShape();
	SphereShape* sphere1 = (SphereShape*)body1Wrap->GetCollisionShape();

	Vector3 diff = body0Wrap->GetWorldTransform().GetTranslation() - body1Wrap->GetWorldTransform().GetTranslation();
	float length = Norm(diff);
	float radius0 = sphere0->GetRadius();
	float radius1 = sphere1->GetRadius();

#ifdef CLEAR_MANIFOLD
	_manifold->ClearManifold();
#endif

	if (length > (radius0 + radius1 + resultOut->ClosestPointDistanceThreshold))
	{
#ifndef CLEAR_MANIFOLD
		resultOut->RefreshContactPoints();
#endif
		return;
	}

	float dist = length - (radius0 + radius1);
	Vector3 normalOnSurfaceB(1, 0, 0);
	if (length > FLT_EPSILON)
	{
		normalOnSurfaceB = diff / length;
	}

	Vector3 position1 = body1Wrap->GetWorldTransform().GetTranslation() + radius1 * normalOnSurfaceB;
	resultOut->AddContactPoint(normalOnSurfaceB, position1, dist);
#ifndef CLEAR_MANIFOLD
	resultOut->RefreshContactPoints();
#endif  //CLEAR_MANIFOLD
}
/****************************************************************************
*						SphereSphereCollisionAlgorithm
*************************************************************************//**
*  @fn        float SphereSphereCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     This function calculates time of impact
*  @param[in,out] CollisionObject*  body0Wrap
*  @param[in,out] CollisionObject*  body1Wrap
*  @param[in] DispatchInfo& dispatchInfo
*  @para,[out] ManifoldResult* resultOut
*  @return 　  float
*****************************************************************************/
float SphereSphereCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	(void)body0; (void)body1; (void)dispatchInfo; (void)resultOut;
	return 1.0f;
}
/****************************************************************************
*						GetAllContactManifolds
*************************************************************************//**
*  @fn        void SphereSphereCollisionAlgorithm::GetAllContactManifolds(ManifoldArray& manifoldArray)
*  @brief     This function Get all contact objects
*  @param[out] ManifoldArray& manifoldArray
*  @return 　　void
*****************************************************************************/
void SphereSphereCollisionAlgorithm::GetAllContactManifolds(ManifoldArray& manifoldArray)
{
	if (_manifold && _ownManifold)
	{
		manifoldArray.push_back(_manifold);
	}
}
#pragma endregion   Sphere_Sphere
#pragma region Sphere_Triangle
SphereTriangleCollisionAlgorithm::SphereTriangleCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, bool swapped)
	:IActivatingCollisionAlgorithm(ci, body0Wrap, body1Wrap)
{
	_ownManifold = false;
	_manifold    = manifold;
	_swapped     = swapped;

	if (!_manifold)
	{
		_manifold = _dispatcher->GetNewManifold(body0Wrap->GetCollisionObject(), body1Wrap->GetCollisionObject());
		_ownManifold = true;
	}
}
SphereTriangleCollisionAlgorithm::~SphereTriangleCollisionAlgorithm()
{
	if (_ownManifold)
	{
		if (_manifold)
		{
			_dispatcher->ReleaseManifold(_manifold);
		}
	}
}
/****************************************************************************
*						ProcessCollision
*************************************************************************//**
*  @fn        void SphereTriangleCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     Execute sphere triangle collision test
*  @param[in] CollisionObjectWrapper*  body0Wrap
*  @param[in] CollisionObjectWrapper*  body1Wrap
*  @param[in] DispatchInfo& dispatchInfo
*  @param[out] ManifoldResult* resultOut
*  @return 　　void
*****************************************************************************/
void  SphereTriangleCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	if (!_manifold) { return; }

	const CollisionObjectWrapper* sphereObjWrap   = _swapped ? body1Wrap : body0Wrap;
	const CollisionObjectWrapper* triangleObjWrap = _swapped ? body0Wrap : body1Wrap;

	SphereShape* sphere     = (SphereShape*)  sphereObjWrap->GetCollisionShape();
	TriangleShape* triangle = (TriangleShape*)triangleObjWrap->GetCollisionShape();

	resultOut->SetPersistentManifold(_manifold);
	SphereTriangleDetector detector(sphere, triangle, _manifold->GetContactBreakingThreshold() + resultOut->ClosestPointDistanceThreshold);

	DiscreteCollisionDetectorInterface::ClosestPointInput input;
	input.MaximumDistanceSquared = FLT_MAX;
	input.TransformA = sphereObjWrap->GetWorldTransform();
	input.TransformB = triangleObjWrap->GetWorldTransform();

	bool swapResult = _swapped;
	detector.GetClosestPoints(input, *resultOut, dispatchInfo.DebugDraw, swapResult);
	if (_ownManifold) { resultOut->RefreshContactPoints(); }
}
/****************************************************************************
*						CalculateTimeOfImpact
*************************************************************************//**
*  @fn        float SphereTriangleCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     This function calculates time of impact
*  @param[in,out] CollisionObject*  body0Wrap
*  @param[in,out] CollisionObject*  body1Wrap
*  @param[in] DispatchInfo& dispatchInfo
*  @para,[out] ManifoldResult* resultOut
*  @return 　  float
*****************************************************************************/
float SphereTriangleCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	(void)resultOut; (void)dispatchInfo; (void)body0; (void)body1;
	return 1.0f;
}
/****************************************************************************
*						GetAllContactManifolds
*************************************************************************//**
*  @fn        void SphereTriangleCollisionAlgorithm::GetAllContactManifolds(ManifoldArray& manifoldArray)
*  @brief     This function Get all contact objects
*  @param[out] ManifoldArray& manifoldArray
*  @return 　　void
*****************************************************************************/
void  SphereTriangleCollisionAlgorithm::GetAllContactManifolds(ManifoldArray& manifoldArray)
{
	if (_manifold && _ownManifold)
	{
		manifoldArray.push_back(_manifold);
	}
}
#pragma endregion Sphere_Triangle
#pragma region Convex_Convex

#pragma endregion Convex_Convex
#pragma region Box2D_Box2D
static int   ClipSegmentToLine(ClipVertex vOut[2], ClipVertex vIn[2], const Vector3& normal, float offset)
{
	// Start with no output points
	int numOut = 0;

	// Calculate the distance of end points to the line
	float distance0 = Dot(normal, vIn[0].V) - offset;
	float distance1 = Dot(normal, vIn[1].V) - offset;

	// If the points are behind the plane
	if (distance0 <= 0.0f) vOut[numOut++] = vIn[0];
	if (distance1 <= 0.0f) vOut[numOut++] = vIn[1];

	// If the points are on different sides of the plane
	if (distance0 * distance1 < 0.0f)
	{
		// Find intersection point of edge and plane
		float interp = distance0 / (distance0 - distance1);
		vOut[numOut].V = vIn[0].V + interp * (vIn[1].V - vIn[0].V);
		if (distance0 > 0.0f)
		{
			vOut[numOut].ID = vIn[0].ID;
		}
		else
		{
			vOut[numOut].ID = vIn[1].ID;
		}
		++numOut;
	}

	return numOut;
}
static float EdgeSeparation(const Box2DShape* poly1, const AffineTransform& transform1, int edge1, const Box2DShape* poly2, const AffineTransform transform2)
{
	const Vector3* vertices1 = poly1->GetVertices();
	const Vector3* normals1  = poly1->GetNormals();
	int count2               = poly2->GetVertexCount();
	const Vector3* vertices2 = poly2->GetVertices();

	assert(0 <= edge1 && edge1 < poly1->GetVertexCount());

	// Convert normal from poly1's frame into poly2's frame.
	Vector3 normal1World = transform1.GetBasis() * normals1[edge1];
	Vector3 normal1      = (Transpose(transform2.GetBasis()) * normal1World);

	// Find support vertex on poly2 for -normal.
	int    index  = 0;
	Scalar minDot = FLT_MAX;

	if (count2 > 0)
		index = (int)MinDot(normal1, vertices2, count2, minDot);

	Vector3 v1 = transform1 * vertices1[edge1];
	Vector3 v2 = transform2 * vertices2[index];
	Scalar separation = Dot(v2 - v1, normal1World);
	return separation;
}
static float FindMaxSeparation(int* edgeIndex, const Box2DShape* poly1, const AffineTransform& transform1, const Box2DShape* poly2, const AffineTransform& transform2)
{
	int count1              = poly1->GetVertexCount();
	const Vector3* normals1 = poly1->GetNormals();

	// Vector pointing from the centroid of poly1 to the centroid of poly2.
	Vector3 d       = transform2 * poly2->GetCentroid() - transform1 * poly1->GetCentroid();
	Vector3 dLocal1 = Transpose(transform1.GetBasis()) * d;

	// Find edge normal on poly1 that has the largest projection onto d.
	int edge = 0;
	Scalar maxDot;
	if (count1 > 0)
		edge = (int)MaxDot(dLocal1, normals1, count1, maxDot);

	// Get the separation for the edge normal.
	Scalar s = EdgeSeparation(poly1, transform1, edge, poly2, transform2);
	if (s > 0.0f)
	{
		return s;
	}

	// Check the separation for the previous edge normal.
	int prevEdge = edge - 1 >= 0 ? edge - 1 : count1 - 1;
	Scalar sPrev = EdgeSeparation(poly1, transform1, prevEdge, poly2, transform2);
	if (sPrev > 0.0f)
	{
		return sPrev;
	}

	// Check the separation for the next edge normal.
	int nextEdge = edge + 1 < count1 ? edge + 1 : 0;
	Scalar sNext = EdgeSeparation(poly1, transform1, nextEdge, poly2, transform2);
	if (sNext > 0.0f)
	{
		return sNext;
	}

	// Find the best edge and the search direction.
	int bestEdge;
	Scalar bestSeparation;
	int increment;
	if ((float)sPrev > s && (float)sPrev > sNext)
	{
		increment      = -1;
		bestEdge       = prevEdge;
		bestSeparation = sPrev;
	}
	else if ((float)sNext > s)
	{
		increment      = 1;
		bestEdge       = nextEdge;
		bestSeparation = sNext;
	}
	else
	{
		*edgeIndex = edge;
		return s;
	}

	// Perform a local search for the best edge normal.
	for (;;)
	{
		if (increment == -1)
			edge = bestEdge - 1 >= 0 ? bestEdge - 1 : count1 - 1;
		else
			edge = bestEdge + 1 < count1 ? bestEdge + 1 : 0;

		s = EdgeSeparation(poly1, transform1, edge, poly2, transform2);
		if (s > 0.0f)
		{
			return s;
		}

		if ((float)s > bestSeparation)
		{
			bestEdge       = edge;
			bestSeparation = s;
		}
		else
		{
			break;
		}
	}

	*edgeIndex = bestEdge;
	return bestSeparation;
}
static void  FindIncidentEdge(ClipVertex c[2], const Box2DShape* poly1, const AffineTransform& transform1, int edge1, const Box2DShape* poly2, const AffineTransform& transform2)
{
	const Vector3* normals1 = poly1->GetNormals();

	int count2 = poly2->GetVertexCount();
	const Vector3* vertices2 = poly2->GetVertices();
	const Vector3* normals2  = poly2->GetNormals();

	assert(0 <= edge1 && edge1 < poly1->GetVertexCount());

	// Get the normal of the reference edge in poly2's frame.
	Vector3 normal1 = Transpose(transform2.GetBasis()) * (transform1.GetBasis() * normals1[edge1]);

	// Find the incident edge on poly2.
	int index = 0;
	Scalar minDot = FLT_MAX;
	for (int i = 0; i < count2; ++i)
	{
		Scalar dot = Dot(normal1, normals2[i]);
		if ((float)dot < minDot)
		{
			minDot = dot;
			index = i;
		}
	}

	// Build the clip vertices for the incident edge.
	int i1 = index;
	int i2 = i1 + 1 < count2 ? i1 + 1 : 0;

	c[0].V = transform2 * vertices2[i1];
	c[1].V = transform2 * vertices2[i2];
}
/****************************************************************************
*						B2CollidePolygons
*************************************************************************//**
*  @fn        void B2CollidePolygons(ManifoldResult* manifold, const Box2DShape* polyA, const AffineTransform& transformA, const Box2DShape* polyB, const AffineTransform& transformB)
*  @brief     --
*  @param[in, out] ManifoldResult* manifold
*  @param[in] Box2DShape* polyA, 
*  @param[in] AffineTransform transformA, 
*  @param[in] Box2DShape* polyB, 
*  @param[in] AffineTransform& transformB
*  @return 　　void
*****************************************************************************/
void B2CollidePolygons(ManifoldResult* manifold, const Box2DShape* polyA, const AffineTransform& transformA, const Box2DShape* polyB, const AffineTransform& transformB)
{
	/*-------------------------------------------------------------------
	-           Find max separation
	---------------------------------------------------------------------*/
	int   edgeA = 0;
	float separationA = FindMaxSeparation(&edgeA, polyA, transformA, polyB, transformB);
	if   (separationA > 0.0f) { return; }

	int   edgeB = 0;
	float separationB = FindMaxSeparation(&edgeB, polyB, transformB, polyA, transformA);
	if (separationB > 0.0f) { return; }

	/*-------------------------------------------------------------------
	-           Prepare variables
	---------------------------------------------------------------------*/
	const Box2DShape* poly1; // reference only
	const Box2DShape* poly2; // incident  only
	AffineTransform transform1, transform2;
	int   edge1; // referenceEdge
	unsigned char flip;
	const float k_RelativeTolerance = 0.98f;
	const float k_AbsoluteTolerance = 0.001f;

	if (separationB > k_RelativeTolerance * separationA + k_AbsoluteTolerance)
	{
		poly1      = polyB;
		poly2      = polyA;
		transform1 = transformB;
		transform2 = transformA;
		edge1      = edgeB;
		flip       = 1;
	}
	else
	{
		poly1      = polyA;
		poly2      = polyB;
		transform1 = transformA;
		transform2 = transformB;
		edge1      = edgeA;
		flip       = 0;
	}

	ClipVertex incidentEdge[2];
	FindIncidentEdge(incidentEdge, poly1, transform1, edge1, poly2, transform2);

	int count1               = poly1->GetVertexCount();
	const Vector3* vertices1 = poly1->GetVertices();
	Vector3 v11              = vertices1[edge1];
	Vector3 v12              = edge1 + 1 < count1 ? vertices1[edge1 + 1] : vertices1[0];
	Vector3 sideNormal       = Normalize(transform1.GetBasis() * (v12 - v11));
	Vector3 frontNormal      = Vector3(1.0f * sideNormal.GetY(), -1.0f * sideNormal.GetX(), 0.0f);

	v11 = transform1 * v11;
	v12 = transform2 * v12;

	float frontOffset = Dot(frontNormal, v11);
	float sideOffset1 = -Dot(sideNormal, v11);
	float sideOffset2 = Dot(sideNormal, v12);

	ClipVertex clipPoints1[2]; clipPoints1->V = Vector3();
	ClipVertex clipPoints2[2]; clipPoints2->V = Vector3();

	int np;
	np = ClipSegmentToLine(clipPoints1, incidentEdge, -sideNormal, sideOffset1);
	if (np < 2) { return; }

	np = ClipSegmentToLine(clipPoints2, clipPoints1, sideNormal, sideOffset2);
	if (np < 2) { return; }

	Vector3 manifoldNormal = flip ? -frontNormal : frontNormal;

	int pointCount = 0;
	for (int i = 0; i < b2_maxManifoldPoints; ++i)
	{
		float separation = Dot(frontNormal, clipPoints2[i].V) - frontOffset;
		if (separation <= 0.0f)
		{
			manifold->AddContactPoint(-manifoldNormal, clipPoints2[i].V, separation);
			++pointCount;
		}
	}
}
Box2DBox2DCollisionAlgorithm::Box2DBox2DCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap)
	: IActivatingCollisionAlgorithm(ci, body0Wrap, body1Wrap)
{
	_ownManifold = false;
	_manifold = manifold;

	if (!_manifold && _dispatcher->NeedsCollision(body0Wrap->GetCollisionObject(), body1Wrap->GetCollisionObject()))
	{
		_manifold = _dispatcher->GetNewManifold(body0Wrap->GetCollisionObject(), body1Wrap->GetCollisionObject());
		_ownManifold = true;
	}
}

Box2DBox2DCollisionAlgorithm::~Box2DBox2DCollisionAlgorithm()
{
	if (_ownManifold)
	{
		if (_manifold)
		{
			_dispatcher->ReleaseManifold(_manifold);
		}
	}
}
/****************************************************************************
*						ProcessCollision
*************************************************************************//**
*  @fn        void Box2DBox2DCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     Execute box box collision test
*  @param[in] CollisionObjectWrapper*  body0Wrap
*  @param[in] CollisionObjectWrapper*  body1Wrap
*  @param[in] DispatchInfo& dispatchInfo
*  @param[out] ManifoldResult* resultOut
*  @return 　　void
*****************************************************************************/
void Box2DBox2DCollisionAlgorithm::ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	if (!_manifold) { return; }

	const Box2DShape* box0 = (const Box2DShape*)body0Wrap->GetCollisionShape();
	const Box2DShape* box1 = (const Box2DShape*)body1Wrap->GetCollisionShape();

	resultOut->SetPersistentManifold(_manifold);
	B2CollidePolygons(resultOut, box0, body0Wrap->GetWorldTransform(), box1, body1Wrap->GetWorldTransform());

	if (_ownManifold)
	{
		resultOut->RefreshContactPoints();
	}
}

/****************************************************************************
*						CalculateTimeOfImpact
*************************************************************************//**
*  @fn        float BoxBoxCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
*  @brief     This function calculates time of impact
*  @param[in,out] CollisionObject*  body0Wrap
*  @param[in,out] CollisionObject*  body1Wrap
*  @param[in] DispatchInfo& dispatchInfo
*  @para,[out] ManifoldResult* resultOut
*  @return 　  float
*****************************************************************************/
float Box2DBox2DCollisionAlgorithm::CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut)
{
	return 1.0f; // not yel;
}
/****************************************************************************
*						GetAllContactManifolds
*************************************************************************//**
*  @fn        void BoxBoxCollisionAlgorithm::GetAllContactManifolds(ManifoldArray& manifoldArray)
*  @brief     This function Get all contact objects
*  @param[out] ManifoldArray& manifoldArray
*  @return 　　void
*****************************************************************************/
void Box2DBox2DCollisionAlgorithm::GetAllContactManifolds(ManifoldArray& manifoldArray)
{

}
#pragma endregion Box2D_Box2D