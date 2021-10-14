//////////////////////////////////////////////////////////////////////////////////
//            @file　　  ContinuousConvexCollision.hpp
//            @brief    CCD (Convexs)
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
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/ContinuousConvexCollision.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/GjkPairDetector.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/ConvexShape.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/PointCollector.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/ConvexShape.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/StaticPlaneShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define MAX_ITERATIONS 64

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
ContinuousConvexCollision::ContinuousConvexCollision(const ConvexShape* shapeA, const ConvexShape* shapeB, VolonoiSimplexSolver* simplexSolver, ConvexPenetrationDepthSolver* penetrationDepthSolver)
{
	_simplexSolver          = simplexSolver;
	_penetrationDepthSolver = penetrationDepthSolver;
	_convexA                = shapeA;
	_convexB                = shapeB;
	_planeShape             = NULL;
}
ContinuousConvexCollision::ContinuousConvexCollision(const ConvexShape* shapeA, const StaticPlaneShape* plane)
{
	_simplexSolver          = 0;
	_penetrationDepthSolver = 0;
	_convexA                = shapeA;
	_convexB                = 0;
	_planeShape             = plane;
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
bool ContinuousConvexCollision::CalculateTimeOfImpact(const AffineTransform& fromA, const AffineTransform& toA, const AffineTransform& fromB, const gm::AffineTransform& toB, CastResult& result)
{
	Vector3 linearVelocityA, angularVelocityA, linearVelocityB, angularVelocityB;
	AffineTransformUtil::CalculateVelocity(fromA, toA, 1.0f, linearVelocityA, angularVelocityA);
	AffineTransformUtil::CalculateVelocity(fromB, toB, 1.0f, linearVelocityB, angularVelocityB);

	Scalar boundingRadiusA             = _convexA->GetAngularMotionDisc();
	Scalar boundingRadiusB             = _convexB ? _convexB->GetAngularMotionDisc() : 0.0f;
	Scalar maxAngularProjectedVelocity = Norm(angularVelocityA) * boundingRadiusA + Norm(angularVelocityB) * boundingRadiusB;
	Vector3 relativeLinearVelocity     = (linearVelocityB - linearVelocityA);
	Scalar relativeVelocityLength      = Norm(linearVelocityB - linearVelocityA);
	Scalar lambda                      = 0.0f;
	Vector3 n                          = Vector3(0, 0, 0);
	if ((relativeVelocityLength + maxAngularProjectedVelocity) = 0.0f) { return false; }
	
	bool hasResult = false;
	Vector3 c;
	Scalar lastLambda   = lambda;
	int    numIteration = 0;
	Scalar radius       = 0.001f;

	/*-------------------------------------------------------------------
	-           Calculate closest point
	---------------------------------------------------------------------*/
	PointCollector pointCollector;
	{
		ComputeClosestPoints(fromA, fromB, pointCollector);
		hasResult = pointCollector.HasResult;
		c         = pointCollector.PointInWorld;
	}

	if (hasResult)
	{
		Scalar dist = pointCollector.Distance + result.AllowedPenetration;
		n           = pointCollector.NormalOnBInWorld; 
		Scalar projectedLinearVelocity = Dot(relativeLinearVelocity, n);
		if ((projectedLinearVelocity + maxAngularProjectedVelocity) <= FLT_EPSILON) { return false; }

		while ((float)dist > radius)
		{
			/*-------------------------------------------------------------------
			-           Draw Debug Collider
			---------------------------------------------------------------------*/
			if (result.DebugDrawer) { result.DebugDrawer->DrawSphere(c, 0.2f, Vector3(1, 1, 1)); }

			Scalar dLambda = 0.0f;
			projectedLinearVelocity = Dot(relativeLinearVelocity, n);
			if ((projectedLinearVelocity + maxAngularProjectedVelocity) <= FLT_EPSILON) { return false; }

			dLambda = dist / (projectedLinearVelocity + maxAngularProjectedVelocity);
			lambda  = lambda + dLambda;
			if (lambda > 1.0f || lambda < 0.0f) { return false; }
			if ((float)lambda <= lastLambda)    { return false; }
			lastLambda = lambda;

			/*-------------------------------------------------------------------
			-           Interpolate to next lambda
			---------------------------------------------------------------------*/
			AffineTransform interpolatedTransformA, interpolatedTransformB, relativeTransform;
			AffineTransformUtil::IntegrateTransform(fromA, linearVelocityA, angularVelocityA, lambda, interpolatedTransformA);
			AffineTransformUtil::IntegrateTransform(fromB, linearVelocityB, angularVelocityB, lambda, interpolatedTransformB);
			relativeTransform = AffineTransformUtil::InverseTimes(interpolatedTransformB, interpolatedTransformA);

			if (result.DebugDrawer) { result.DebugDrawer->DrawSphere(interpolatedTransformA.GetTranslation(), 0.2f, Vector3(1, 1, 1)); }
			result.DebugDraw(lambda);

			/*-------------------------------------------------------------------
			-           Point Collect
			---------------------------------------------------------------------*/
			PointCollector pointCollector;
			ComputeClosestPoints(interpolatedTransformA, interpolatedTransformB, pointCollector);
			if (pointCollector.HasResult)
			{
				dist = pointCollector.Distance + result.AllowedPenetration;
				c    = pointCollector.PointInWorld;
				n    = pointCollector.NormalOnBInWorld;
			}
			else
			{
				result.ReportFailure(-1, numIteration); return false;
			}

			numIteration++;
			if (numIteration > MAX_ITERATIONS)
			{
				result.ReportFailure(-2, numIteration);
				return false;
			}
		}
		result.Fraction = lambda;
		result.Normal   = n;
		result.HitPoint = c;
		return true;
	}
	return false;
}
/****************************************************************************
*						ComputeClosestPoints
*************************************************************************//**
*  @fn        void ContinuousConvexCollision::ComputeClosestPoints(const AffineTransform& transformA, const AffineTransform& transformB, struct PointCollector& pointCollector)
*  @brief     Compute closest point
*  @param[in] AffineTransform& transformA, 
*  @param[in] AffineTransform& transformB, 
*  @param[out] struct PointCollector& pointCollector
*  @return 　　bool
*****************************************************************************/
void ContinuousConvexCollision::ComputeClosestPoints(const AffineTransform& transformA, const AffineTransform& transformB, struct PointCollector& pointCollector)
{
	/*-------------------------------------------------------------------
	-           In case of ConvexB existance
	---------------------------------------------------------------------*/
	if (_convexB)
	{
		_simplexSolver->Reset();

		/*-------------------------------------------------------------------
		-           Get Closeset points
		---------------------------------------------------------------------*/
		GJKPairDetector gjk(_convexA, _convexB, _convexA->GetShapeType(), _convexB->GetShapeType(), _convexA->GetMargin(), _convexB->GetMargin(), _simplexSolver, _penetrationDepthSolver);
		GJKPairDetector::ClosestPointInput input;
		input.TransformA = transformA;
		input.TransformB = transformB;
		gjk.GetClosestPoints(input, pointCollector, 0);
	}
	/*-------------------------------------------------------------------
	-           In case of ConvexB no existance
	---------------------------------------------------------------------*/
	else
	{
		const ConvexShape*      convexShape   = _convexA;
		const StaticPlaneShape* planeShape    = _planeShape;
		const Vector3&          planeNormal   = planeShape->GetPlaneNormal();
		const Scalar&           planeConstant = planeShape->GetPlaneConstant();

		AffineTransform convexWorldTransform   = transformA;
		AffineTransform convexInPlaneTransform = InverseAffineTransform(transformB) * convexWorldTransform;
		AffineTransform planeInConvex          = InverseAffineTransform(convexInPlaneTransform) * transformB;

		Vector3 vertex                 = convexShape->LocalGetSupportingVertex(planeInConvex.GetBasis() * (-planeNormal));
		Vector3 vertexInPlane          = convexInPlaneTransform * vertex;
		Scalar  distance               = (Dot(planeNormal, vertexInPlane) - planeConstant);
		Vector3 vertexInPlaneProjected = vertexInPlane - distance * planeNormal;
		Vector3 vertexInPlaneWorld     = transformB * vertexInPlaneProjected;
		Vector3 normalOnSurfaceB       = transformB.GetBasis() * planeNormal;

		pointCollector.AddContactPoint(normalOnSurfaceB, vertexInPlaneWorld, distance);
	}
}