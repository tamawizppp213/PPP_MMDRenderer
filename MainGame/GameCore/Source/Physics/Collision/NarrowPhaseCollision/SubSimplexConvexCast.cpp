//////////////////////////////////////////////////////////////////////////////////
///             @file   PersistentManifold.cpp
///             @brief  Persistent Collision Manifold
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
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/SubSimplexConvexCast.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/ConvexShape.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
SubSimplexConvexCast::SubSimplexConvexCast(const ConvexShape* convexA, const ConvexShape* convexB, VolonoiSimplexSolver* simplexSolver)
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
*  @return @@bool
*****************************************************************************/
bool SubSimplexConvexCast::CalculateTimeOfImpact(const AffineTransform& fromA, const AffineTransform& toA, const AffineTransform& fromB, const AffineTransform& toB, CastResult& result)
{
	/*-------------------------------------------------------------------
	-           Reset Solver
	---------------------------------------------------------------------*/
	_simplexSolver->Reset();

	/*-------------------------------------------------------------------
	-           Prepare Vector
	---------------------------------------------------------------------*/
	Vector3 linearVelocityA, linearVelocityB;
	linearVelocityA               = toA.GetTranslation() - fromA.GetTranslation();
	linearVelocityB               = toB.GetTranslation() - fromB.GetTranslation();
	float lambda                  = 0.0f;
	AffineTransform interpolatedA = fromA;
	AffineTransform interpolatedB = fromB;
	Vector3 r                     = linearVelocityA - linearVelocityB;
	Vector3 supVertexA = fromA * _convexA->LocalGetSupportingVertex(fromA.GetBasis() * -r);
	Vector3 supVertexB = fromB * _convexB->LocalGetSupportingVertex(fromB.GetBasis() * r);
	Vector3 v          = supVertexA - supVertexB;
	int maxIteration = result.SubSimplexCastMaxIterations;

	Vector3 n = Vector3(0, 0, 0);
	Vector3 c;
	float   dist2 = NormSquared(v);

	Vector3 w, p; 
	float VdotR = Dot(v, w);

	/*-------------------------------------------------------------------
	-           Loop
	---------------------------------------------------------------------*/
	while ((dist2 > result.SubSimplexCastEpsilon) && maxIteration--)
	{
		supVertexA = interpolatedA * _convexA->LocalGetSupportingVertex(interpolatedA.GetBasis() * (-v));
		supVertexB = interpolatedB * _convexB->LocalGetSupportingVertex(interpolatedB.GetBasis() * v);
		w = supVertexA - supVertexB;

		float VdotW = Dot(v, w);
		if (lambda > 1.0f) { return false; }

		if (VdotW > 0.0f)
		{
			VdotR = Dot(v, r);

			if (VdotR >= -(FLT_EPSILON * FLT_EPSILON)) { return false; }
			else
			{
				lambda = lambda - VdotW / VdotR;
				//interpolate to next lambda
				//	x = s + lambda * r;
				interpolatedA.SetTranslation(Lerp(fromA.GetTranslation(), toA.GetTranslation(), lambda));
				interpolatedB.SetTranslation(Lerp(fromB.GetTranslation(), toB.GetTranslation(), lambda));

				//check next line
				w = supVertexA - supVertexB;

				n = v;
			}
		}

		if (!_simplexSolver->InSimplex(w)) { _simplexSolver->AddVertex(w, supVertexA, supVertexB); }
		if (_simplexSolver->Closest(v)) { dist2 = NormSquared(v); }
		else { dist2 = 0.0f; }

	}

	result.Fraction = lambda;
	if (NormSquared(n) >= (FLT_EPSILON * FLT_EPSILON)) { result.Normal = Normalize(n); }
	else { result.Normal = Vector3(0.0f, 0.0f, 0.0f); }

	if (Dot(result.Normal, r) >= -result.AllowedPenetration) { return false; }

	/*-------------------------------------------------------------------
	-           Compute Hit Points
	---------------------------------------------------------------------*/
	Vector3 hitA, hitB;
	_simplexSolver->ComputePoints(hitA, hitB);
	result.HitPoint = hitB;
	return true;
}