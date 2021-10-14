//////////////////////////////////////////////////////////////////////////////////
//            @file　　  SubSimplexSolver.hpp
//            @brief    VoronoiSimplexSolver
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_12 (first checked)
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
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/RaycastCallback.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/TriangleShape.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/SubSimplexConvexCast.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/PenetrationDepthSolver.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/ContinuousConvexCollision.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
TriangleRayCastCallback::TriangleRayCastCallback(const Vector3& from, const gm::Vector3& to, unsigned int flags)
{
	From = from;
	To = to;
	Flags = flags;
	HitFraction = 1.0f;
}
/****************************************************************************
*						ProcessTriangle
*************************************************************************//**
*  @fn        void TriangleRayCastCallback::ProcessTriangle(gm::Vector3* triangle, int partID, int triangleIndex)
*  @brief     Triangle
*  @param[in, out] Vector3* triangle
*  @param[in] int partID
*  @param[om] triangleIndex
*  @return 　　void
*****************************************************************************/
void TriangleRayCastCallback::ProcessTriangle(Vector3* triangle, int partID, int triangleIndex)
{
	/*-------------------------------------------------------------------
	-           Calculate Vertex Distance
	---------------------------------------------------------------------*/
	const Vector3& vertex0 = triangle[0];
	const Vector3& vertex1 = triangle[1];
	const Vector3& vertex2 = triangle[2];
	Vector3 v10 = vertex1 - vertex0;
	Vector3 v20 = vertex2 - vertex0;

	Vector3 triangleNormal = Cross(v10, v20);
	const float dist = Dot(vertex0, triangleNormal);
	float distA = Dot(triangleNormal, From) - dist;
	float distB = Dot(triangleNormal, To) - dist;

	if (distA * distB >= 0.0f) { return; }
	if (((Flags & EFlags::FilterBackFaces) != 0) && (distA <= 0.0f)){ return; }

	/*-------------------------------------------------------------------
	-           Get  Distance
	---------------------------------------------------------------------*/
	const float projLength = distA - distB;
	const float distance   = distA / projLength;
	if (distance < HitFraction)
	{
		float edgeTolerance = NormSquared(triangleNormal);
		edgeTolerance *= -0.0001f;

		Vector3 point = Lerp(From, To, distance);
		{
			Vector3 v0p = vertex0 - point;
			Vector3 v1p = vertex1 - point;
			Vector3 cp0 = Cross(v0p, v1p);

			if (Dot(cp0, triangleNormal) >= edgeTolerance)
			{
				Vector3 v2p = vertex2 - point;
				Vector3 cp1 = Cross(v1p, v2p);
				if (Dot(cp1, triangleNormal) >= edgeTolerance)
				{
					Vector3 cp2 = Cross(v2p, v0p);
					if (Dot(cp2, triangleNormal) >= edgeTolerance)
					{
						triangleNormal = Normalize(triangleNormal);

						if (((Flags & EFlags::KeepUnflippedNormal) == 0) && (distA <= 0.0f))
						{
							HitFraction = ReportHit(-triangleNormal, distance, partID, triangleIndex);
						}
						else
						{
							HitFraction = ReportHit(triangleNormal, distance, partID, triangleIndex);
						}
					}
				}
			}
		}
	}
}

TriangleConvexCastCallback::TriangleConvexCastCallback(const ConvexShape* convexShape, const AffineTransform& convexShapeFrom, const AffineTransform& convexShapeTo, const AffineTransform& triangleToWorld, const float triangleCollisionMargin)
{
	ConvexShapePtr          = convexShape;
	ConvexShapeFrom         = convexShapeFrom;
	ConvexShapeTo           = convexShapeTo;
	TriangleToWorld         = triangleToWorld;
	HitFraction             = 1.0f;
	TriangleCollisionMargin = triangleCollisionMargin;
	AllowedPenetration      = 0.0f;
}
/****************************************************************************
*						ProcessTriangle
*************************************************************************//**
*  @fn        void TriangleRayCastCallback::ProcessTriangle(gm::Vector3* triangle, int partID, int triangleIndex)
*  @brief     Triangle
*  @param[in, out] Vector3* triangle
*  @param[in] int partID
*  @param[om] triangleIndex
*  @return 　　void
*****************************************************************************/
void TriangleConvexCastCallback::ProcessTriangle(Vector3* triangle, int partID, int triangleIndex)
{
	TriangleShape triangleShape(triangle[0], triangle[1], triangle[2]);
	triangleShape.SetMargin(TriangleCollisionMargin);

	VolonoiSimplexSolver simplexSolver;
	GjkEpaPenetrationDepthSolver gjkEpaPenetrationSolver;

#ifdef USE_SUBSIMPLEX_CONVEX_CAST
	SubSimplexConvexCast convexCaster(ConvexShape, &triangleShape, &simplexSolver);
#else
	//btGjkConvexCast	convexCaster(ConvexShapePtr,&triangleShape,&simplexSolver);
	ContinuousConvexCollision convexCaster(ConvexShapePtr, &triangleShape, &simplexSolver, &gjkEpaPenetrationSolver);
#endif  //#USE_SUBSIMPLEX_CONVEX_CAST

	ConvexCast::CastResult castResult;
	castResult.Fraction = 1.0f;
	castResult.AllowedPenetration = AllowedPenetration;
	if (convexCaster.CalculateTimeOfImpact(ConvexShapeFrom, ConvexShapeTo, TriangleToWorld, TriangleToWorld, castResult))
	{
		//add hit
		if (NormSquared(castResult.Normal) > 0.0001f)
		{
			if (castResult.Fraction < HitFraction)
			{
				castResult.Normal = Normalize(castResult.Normal);

				ReportHit(castResult.Normal, castResult.HitPoint, castResult.Fraction, partID, triangleIndex);
			}
		}
	}
}