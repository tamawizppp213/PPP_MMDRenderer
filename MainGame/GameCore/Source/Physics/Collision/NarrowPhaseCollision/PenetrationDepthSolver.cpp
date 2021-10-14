//////////////////////////////////////////////////////////////////////////////////
//            @file　　  PenetrationDepthSolver.hpp
//            @brief    PenetrationDepthSolver
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_12
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

EPA Copyright (c) Ricardo Padrela 2006

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
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/PenetrationDepthSolver.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/VoronoiSimplexSolver.hpp"
#include "../../../../../GameMath/Include/GMTransform.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/GjkPairDetector.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/GjkEpa2.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/ConvexShape.hpp"
#include "../../../../Include/Physics/Util/IDebugDraw.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define UNIT_SPHERE_POINT_COUNT (42)
//////////////////////////////////////////////////////////////////////////////////
//                               Class
//////////////////////////////////////////////////////////////////////////////////
bool GjkEpaPenetrationDepthSolver::CalculatePenetrationDepth( VolonoiSimplexSolver& simplexSolver,
	const ConvexShape* convexA, const ConvexShape* convexB,
	const AffineTransform& transformA, const AffineTransform& transformB, Vector3& v, Vector3& pa, Vector3& pb, class IDebugDraw* debugDraw
)
{
	(void)debugDraw; (void)v; (void)simplexSolver;

	Vector3 guessVectors[] = {
		Vector3(Normalize(transformB.GetTranslation() - transformA.GetTranslation())),
		Vector3(Normalize(transformA.GetTranslation() - transformB.GetTranslation())),
		Vector3(0, 0, 1),
		Vector3(0, 1, 0),
		Vector3(1, 0, 0),
		Vector3(1, 1, 0),
		Vector3(1, 1, 1),
		Vector3(0, 1, 1),
		Vector3(1, 0, 1),
	};

	int numVectors = sizeof(guessVectors) / sizeof(Vector3);

	for (int i = 0; i < numVectors; i++)
	{
		simplexSolver.Reset();
		Vector3 guessVector = guessVectors[i];

		GjkEpaSolver2::Result results;

		if (GjkEpaSolver2::Penetration(convexA, transformA, convexB, transformB, guessVector, results))
		{
			pa = results.Witnesses[0];
			pb = results.Witnesses[1];
			v = results.Normal;
			return true;
		}
		else
		{
			if (GjkEpaSolver2::Distance(convexA, transformA, convexB, transformB, guessVector, results))
			{
				pa = results.Witnesses[0];
				pb = results.Witnesses[1];
				v = results.Normal;
				return false;
			}
		}
	}

	//failed to find a distance/penetration
	pa = Vector3(0, 0, 0);
	pb = Vector3(0, 0, 0);
	v  = Vector3(0, 0, 0);
	return false;
}

#pragma region Minkowski
bool MinkowskiPenetrationDepthSolver::CalculatePenetrationDepth(VolonoiSimplexSolver& simplexSolver,
	const ConvexShape* convexA, const ConvexShape* convexB,
	const AffineTransform& transformA, const AffineTransform& transformB, Vector3& v, Vector3& pa, Vector3& pb, class IDebugDraw* debugDraw
)
{
	(void)v;
	bool check2D = convexA->IsConvex2D() && convexB->IsConvex2D();

	struct IntermediateResult : public DiscreteCollisionDetectorInterface::Result
	{
		Vector3 NormalOnBWorld;
		Vector3 PointInWorld;
		Scalar  Depth;
		bool    HasResult;
		IntermediateResult() : HasResult(false) {};

		void SetShapeIdentifiersA(int partID0, int index0) override{ (void)partID0; (void)index0; }
		void SetShapeIdentifiersB(int partID1, int index1) override { (void)partID1; (void)index1; }
		void AddContactPoint(const Vector3& normalOnBInWorld, const Vector3& pointInWorld, float depth) override
		{
			NormalOnBWorld = normalOnBInWorld;
			PointInWorld   = pointInWorld;
			Depth          = depth;
			HasResult      = true;
		}
	};

	Scalar minProj = Scalar(FLT_MAX);
	Vector3 minNorm(0.0f, 0.0f, 0.0f);
	Vector3 minA, minB;
	Vector3 separatingAxisInA, separatingAxisInB;
	Vector3 pInA, qInB, pWorld, qWorld, w;

#ifndef __SPU__
#define USE_BATCHED_SUPPORT 1
#endif
#ifdef USE_BATCHED_SUPPORT
	Vector3 supportVerticesABatch[UNIT_SPHERE_POINT_COUNT + MAX_PREFERRED_PENETRATION_DIRECTIONS * 2];
	Vector3 supportVerticesBBatch[UNIT_SPHERE_POINT_COUNT + MAX_PREFERRED_PENETRATION_DIRECTIONS * 2];
	Vector3 separatingAxisInABatch[UNIT_SPHERE_POINT_COUNT + MAX_PREFERRED_PENETRATION_DIRECTIONS * 2];
	Vector3 separatingAxisInBBatch[UNIT_SPHERE_POINT_COUNT + MAX_PREFERRED_PENETRATION_DIRECTIONS * 2];
	int i;

	int numSampleDirections = UNIT_SPHERE_POINT_COUNT;

	for (i = 0; i < numSampleDirections; i++)
	{
		Vector3 norm = GetPenetrationDirections()[i];
		separatingAxisInABatch[i] = transformA.GetBasis() * (-norm);
		separatingAxisInBBatch[i] = transformB.GetBasis() * norm;
	}

	{
		int numPDA = convexA->GetPreferredPenetrationDirectionsCount();
		if (numPDA)
		{
			for (int i = 0; i < numPDA; i++)
			{
				Vector3 norm;
				convexA->GetPreferredPenetrationDirection(i, norm);
				norm = transformA.GetBasis() * norm;
				GetPenetrationDirections()[numSampleDirections] = norm;
				separatingAxisInABatch[numSampleDirections] = transformA.GetBasis() * (-norm);
				separatingAxisInBBatch[numSampleDirections] = transformB.GetBasis() * norm;
				numSampleDirections++;
			}
		}
	}

	{
		int numPDB = convexB->GetPreferredPenetrationDirectionsCount();
		if (numPDB)
		{
			for (int i = 0; i < numPDB; i++)
			{
				Vector3 norm;
				convexB->GetPreferredPenetrationDirection(i, norm);
				norm = transformB.GetBasis() * norm;
				GetPenetrationDirections()[numSampleDirections] = norm;
				separatingAxisInABatch[numSampleDirections] = transformA.GetBasis() * (-norm);
				separatingAxisInBBatch[numSampleDirections] = transformB.GetBasis() * norm;
				numSampleDirections++;
			}
		}
	}

	convexA->BatchedUnitVectorGetSupportingVertexWithoutMargin(separatingAxisInABatch, supportVerticesABatch, numSampleDirections);
	convexB->BatchedUnitVectorGetSupportingVertexWithoutMargin(separatingAxisInBBatch, supportVerticesBBatch, numSampleDirections);

	for (i = 0; i < numSampleDirections; i++)
	{
		Vector3 norm = GetPenetrationDirections()[i];
		if (check2D)
		{
			norm.SetZ(0.f);
		}
		if (NormSquared(norm) > 0.01f)
		{
			separatingAxisInA = separatingAxisInABatch[i];
			separatingAxisInB = separatingAxisInBBatch[i];

			pInA = supportVerticesABatch[i];
			qInB = supportVerticesBBatch[i];

			pWorld = transformA * pInA;
			qWorld = transformB * qInB;
			if (check2D)
			{
				pWorld.SetZ(0.0f);
				qWorld.SetZ(0.0f);
			}

			w = qWorld - pWorld;
			Scalar delta = Dot(norm, w);
			//find smallest delta
			if ((float)delta < minProj)
			{
				minProj = delta;
				minNorm = norm;
				minA    = pWorld;
				minB    = qWorld;
			}
		}
	}
#else

	int numSampleDirections = UNIT_SPHERE_POINT_COUNT;

#ifndef __SPU__
	{
		int numPDA = convexA->GetPreferredPenetrationDirectionsCount();
		if (numPDA)
		{
			for (int i = 0; i < numPDA; i++)
			{
				Vector3 norm;
				convexA->GetPreferredPenetrationDirection(i, norm);
				norm = transformA.GetBasis() * norm;
				GetPenetrationDirections()[numSampleDirections] = norm;
				numSampleDirections++;
			}
		}
	}

	{
		int numPDB = convexB->GetPreferredPenetrationDirectionsCount();
		if (numPDB)
		{
			for (int i = 0; i < numPDB; i++)
			{
				Vector3 norm;
				convexB->GetPreferredPenetrationDirection(i, norm);
				norm = transformB.GetBasis() * norm;
				GetPenetrationDirections()[numSampleDirections] = norm;
				numSampleDirections++;
			}
		}
	}
#endif  // __SPU__

	for (int i = 0; i < numSampleDirections; i++)
	{
		const Vector3& norm = GetPenetrationDirections()[i];
		separatingAxisInA = transformA.GetBasis() * (-norm);
		separatingAxisInB = transformB.GetBasis() * norm;
		pInA = convexA->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInA);
		qInB = convexB->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInB);
		pWorld = transformA * pInA;
		qWorld = transformB * qInB;
		w = qWorld - pWorld;
		Scalar delta = Dot(norm, w);
		//find smallest delta
		if ((float)delta < minProj)
		{
			minProj = delta;
			minNorm = norm;
			minA = pWorld;
			minB = qWorld;
		}
	}
  //USE_BATCHED_SUPPORT
#endif

	minA += minNorm * convexA->GetMarginNonVirtual();
	minB -= minNorm * convexB->GetMarginNonVirtual();
	if (minProj < 0.0f) { return false; }

	Scalar extraSeparation = 0.5f;
	minProj = minProj + extraSeparation + (convexA->GetMarginNonVirtual() + convexB->GetMarginNonVirtual());


	GJKPairDetector gjkdet(convexA, convexB, &simplexSolver, 0);

	Scalar  offsetDist = minProj;
	Vector3 offset     = minNorm * offsetDist;

	GJKPairDetector::ClosestPointInput input;
	Vector3 newOrg                 = transformA.GetTranslation() + offset;
	AffineTransform displacedTrans = transformA;
	displacedTrans.SetTranslation(newOrg);

	input.TransformA = displacedTrans;
	input.TransformB = transformB;
	input.MaximumDistanceSquared = Scalar(FLT_MAX);  //minProj;

	IntermediateResult res;
	gjkdet.SetCachedSeparatingAxis(-minNorm);
	gjkdet.GetClosestPoints(input, res, debugDraw);

	Scalar correctedMinNorm = minProj - res.Depth;

	//the penetration depth is over-estimated, relax it
	Scalar penetration_relaxation = Scalar(1.);
	minNorm *= penetration_relaxation;

	if (res.HasResult)
	{
		pa = res.PointInWorld - minNorm * correctedMinNorm;
		pb = res.PointInWorld;
		v = minNorm;

#ifdef DEBUG_DRAW
		if (debugDraw)
		{
			Vector3 color(1, 0, 0);
			debugDraw->DrawLine(pa, pb, color);
		}
#endif  //DEBUG_DRAW
	}
	return res.HasResult;
}

gm::Vector3* MinkowskiPenetrationDepthSolver::GetPenetrationDirections()
{
	static Vector3 sPenetrationDirections[UNIT_SPHERE_POINT_COUNT + MAX_PREFERRED_PENETRATION_DIRECTIONS * 2] =
	{
		Vector3(Scalar(0.000000), Scalar(-0.000000), Scalar(-1.000000)),
		Vector3(Scalar(0.723608), Scalar(-0.525725), Scalar(-0.447219)),
		Vector3(Scalar(-0.276388), Scalar(-0.850649), Scalar(-0.447219)),
		Vector3(Scalar(-0.894426), Scalar(-0.000000), Scalar(-0.447216)),
		Vector3(Scalar(-0.276388), Scalar(0.850649), Scalar(-0.447220)),
		Vector3(Scalar(0.723608), Scalar(0.525725), Scalar(-0.447219)),
		Vector3(Scalar(0.276388), Scalar(-0.850649), Scalar(0.447220)),
		Vector3(Scalar(-0.723608), Scalar(-0.525725), Scalar(0.447219)),
		Vector3(Scalar(-0.723608), Scalar(0.525725), Scalar(0.447219)),
		Vector3(Scalar(0.276388), Scalar(0.850649), Scalar(0.447219)),
		Vector3(Scalar(0.894426), Scalar(0.000000), Scalar(0.447216)),
		Vector3(Scalar(-0.000000), Scalar(0.000000), Scalar(1.000000)),
		Vector3(Scalar(0.425323), Scalar(-0.309011), Scalar(-0.850654)),
		Vector3(Scalar(-0.162456), Scalar(-0.499995), Scalar(-0.850654)),
		Vector3(Scalar(0.262869), Scalar(-0.809012), Scalar(-0.525738)),
		Vector3(Scalar(0.425323), Scalar(0.309011), Scalar(-0.850654)),
		Vector3(Scalar(0.850648), Scalar(-0.000000), Scalar(-0.525736)),
		Vector3(Scalar(-0.525730), Scalar(-0.000000), Scalar(-0.850652)),
		Vector3(Scalar(-0.688190), Scalar(-0.499997), Scalar(-0.525736)),
		Vector3(Scalar(-0.162456), Scalar(0.499995), Scalar(-0.850654)),
		Vector3(Scalar(-0.688190), Scalar(0.499997), Scalar(-0.525736)),
		Vector3(Scalar(0.262869), Scalar(0.809012), Scalar(-0.525738)),
		Vector3(Scalar(0.951058), Scalar(0.309013), Scalar(0.000000)),
		Vector3(Scalar(0.951058), Scalar(-0.309013), Scalar(0.000000)),
		Vector3(Scalar(0.587786), Scalar(-0.809017), Scalar(0.000000)),
		Vector3(Scalar(0.000000), Scalar(-1.000000), Scalar(0.000000)),
		Vector3(Scalar(-0.587786), Scalar(-0.809017), Scalar(0.000000)),
		Vector3(Scalar(-0.951058), Scalar(-0.309013), Scalar(-0.000000)),
		Vector3(Scalar(-0.951058), Scalar(0.309013), Scalar(-0.000000)),
		Vector3(Scalar(-0.587786), Scalar(0.809017), Scalar(-0.000000)),
		Vector3(Scalar(-0.000000), Scalar(1.000000), Scalar(-0.000000)),
		Vector3(Scalar(0.587786), Scalar(0.809017), Scalar(-0.000000)),
		Vector3(Scalar(0.688190), Scalar(-0.499997),Scalar(0.525736)),
		Vector3(Scalar(-0.262869), Scalar(-0.809012), Scalar(0.525738)),
		Vector3(Scalar(-0.850648), Scalar(0.000000), Scalar(0.525736)),
		Vector3(Scalar(-0.262869), Scalar(0.809012), Scalar(0.525738)),
		Vector3(Scalar(0.688190), Scalar(0.499997), Scalar(0.525736)),
		Vector3(Scalar(0.525730), Scalar(0.000000), Scalar(0.850652)),
		Vector3(Scalar(0.162456), Scalar(-0.499995), Scalar(0.850654)),
		Vector3(Scalar(-0.425323), Scalar(-0.309011), Scalar(0.850654)),
		Vector3(Scalar(-0.425323), Scalar(0.309011), Scalar(0.850654)),
		Vector3(Scalar(0.162456), Scalar(0.499995), Scalar(0.850654)) };

	return sPenetrationDirections;
}
#pragma endregion Minkowski