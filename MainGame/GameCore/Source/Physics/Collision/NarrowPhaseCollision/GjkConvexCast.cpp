//////////////////////////////////////////////////////////////////////////////////
//            @file　　  ConvexCast.hpp
//            @brief    ConvexCast
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


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/GjkConvexCast.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/GjkPairDetector.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/PointCollector.hpp"
#include "../../../../Include/Physics/Util/TransformUtil.hpp"


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define MAX_ITERATIONS 32
using namespace gm;


//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
GJKConvexCast::GJKConvexCast(const ConvexShape* convexA, const ConvexShape* convexB, VolonoiSimplexSolver* simplexSolver)
{
	_simplexSolver = simplexSolver;
	_convexA       = convexA;
	_convexB       = convexB;
}
/****************************************************************************
*						CalculateTimeOfImpact
*************************************************************************//**
*  @fn        bool SubSimplexConvexCast::CalculateTimeOfImpact(const AffineTransform& fromA, const AffineTransform& toA, const AffineTransform& fromB, const AffineTransform& toB, CastResult& result)
*  @brief     Calculate Time of Impact
*  @param[in] AffineTransform& fromA,
*  @param[in] AffineTransform& toA,
*  @param[in] AffineTransform& fromB,
*  @param[in] AffineTransform& toB
*  @return 　　bool
*****************************************************************************/
bool GJKConvexCast::CalculateTimeOfImpact(const AffineTransform& fromA, const AffineTransform& toA, const AffineTransform& fromB, const AffineTransform& toB, CastResult& result)
{
	_simplexSolver->Reset();

	Vector3 linearVelocityA = toA.GetTranslation() - fromA.GetTranslation();
	Vector3 linearVelocityB = toB.GetTranslation() - fromB.GetTranslation();
	Scalar  radius          = 0.001f;
	Scalar  lambda          = 0.0f;
	Vector3 v               = Vector3(1, 0, 0);
	int     maxIteration    = MAX_ITERATIONS;
	Vector3 n               = Vector3(0, 0, 0);
	Vector3 r               = linearVelocityA - linearVelocityB;
	Scalar lastLambda       = lambda;
	bool   hasResult        = false;
	int    numIteration     = 0;
	AffineTransform identityTransform;
	Vector3 c;

	/*-------------------------------------------------------------------
	-           Setting GJK Algorithm
	---------------------------------------------------------------------*/
	PointCollector  pointCollector;
	GJKPairDetector gjk(_convexA, _convexB, _simplexSolver, 0);
	GJKPairDetector::ClosestPointInput input;
	input.TransformA = fromA;
	input.TransformB = fromB;

	/*-------------------------------------------------------------------
	-           Get closest points
	---------------------------------------------------------------------*/
	gjk.GetClosestPoints(input, pointCollector, 0);

	hasResult = pointCollector.HasResult;
	c         = pointCollector.PointInWorld;

	/*-------------------------------------------------------------------
	-           Check result
	---------------------------------------------------------------------*/
	if (hasResult)
	{
		Scalar distance;
		distance = pointCollector.Distance;
		n        = pointCollector.NormalOnBInWorld;

		while ((float)distance > radius)
		{
			numIteration++;
			if (numIteration > maxIteration) { return false; }

			Scalar dLambda                 = 0.0f;
			Scalar projectedLinearVelocity = Dot(r, n);
			dLambda                        = distance / projectedLinearVelocity;
			lambda                         = lambda - dLambda;
			
			if (lambda > 1.0f) { return false; }
			if (lambda < 0.0f) { return false; }
			if ((float)lambda <= lastLambda) { return false; }
			lastLambda = lambda;

			/*-------------------------------------------------------------------
			-           Interpolate to next lambda
			---------------------------------------------------------------------*/
			result.DebugDraw(lambda);
			input.TransformA.SetTranslation(Lerp(fromA.GetTranslation(), toA.GetTranslation(), (float)lambda));
			input.TransformB.SetTranslation(Lerp(fromB.GetTranslation(), toB.GetTranslation(), (float)lambda));

			/*-------------------------------------------------------------------
			-           Point closest points
			---------------------------------------------------------------------*/
			gjk.GetClosestPoints(input, pointCollector, 0);
			if (pointCollector.HasResult)
			{
				if (pointCollector.Distance < 1.0f)
				{
					result.Fraction = lastLambda;
					n               = pointCollector.NormalOnBInWorld;
					result.Normal   = n;
					result.HitPoint = pointCollector.PointInWorld;
					return true;
				}

				c        = pointCollector.PointInWorld;
				n        = pointCollector.NormalOnBInWorld;
				distance = pointCollector.Distance;
			}
			else
			{
				return false;
			}
		}

		if (Dot(n, r) >= -result.AllowedPenetration) { return false; }
		result.Fraction = lambda;
		result.Normal   = n;
		result.HitPoint = c;

		return true;
	}
	return false;
}