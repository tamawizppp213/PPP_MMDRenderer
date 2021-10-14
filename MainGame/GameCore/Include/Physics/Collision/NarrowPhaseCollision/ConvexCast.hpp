//////////////////////////////////////////////////////////////////////////////////
//            @file　　  ConvexCast.hpp
//            @brief    ConvexCast
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_03
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
#ifndef CONVEX_CAST_HPP
#define CONVEX_CAST_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMTransform.hpp"
#include "../../Util/IDebugDraw.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#ifdef USE_DOUBLE_PRECISION
#define MAX_CONVEX_CAST_ITERATIONS 64
#define MAX_CONVEX_CAST_EPSILON (DBL_EPSILON * 10)
#else
#define MAX_CONVEX_CAST_ITERATIONS 32
#define MAX_CONVEX_CAST_EPSILON (0.0001)
#endif

//////////////////////////////////////////////////////////////////////////////////
//                           Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			ConvexCast
*************************************************************************//**
*  @class     ConvexCast
*  @brief     Interface for casting. 
*  @detail    Typically the conservative advancement reaches solution in a few iterations, clip it to 32 for degenerate cases.
              See discussion about this here http://continuousphysics.com/Bullet/phpBB2/viewtopic.php?t=565
              will need to digg deeper to make the algorithm more robust
              since, a large epsilon can cause an early termination with false
              positive results (ray intersections that shouldn't be there)
*****************************************************************************/
class ConvexCast
{
public:
	struct CastResult
	{
		gm::AffineTransform HitTransformA;
		gm::AffineTransform HitTransformB;
		gm::Vector3         Normal;
		gm::Vector3         HitPoint;
		float               Fraction; // input and output
		IDebugDraw*         DebugDrawer;
		float               AllowedPenetration;
		int                 SubSimplexCastMaxIterations;
		float               SubSimplexCastEpsilon;

		CastResult()
		{
			Fraction                    = FLT_MAX;
			DebugDrawer                 = NULL;
			AllowedPenetration          = 0.0f;
			SubSimplexCastMaxIterations = MAX_CONVEX_CAST_ITERATIONS;
			SubSimplexCastEpsilon       = MAX_CONVEX_CAST_EPSILON;
		}

		~CastResult() {};

		virtual void DebugDraw(float fraction) { (void)fraction; }
		virtual void DrawCoordSystem(const gm::AffineTransform& transform) { (void)transform; }
		virtual void ReportFailure(int errorNo, int numIterations) { (void)errorNo, (void)numIterations; }
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual bool CalculateTimeOfImpact(const gm::AffineTransform& fromA, const gm::AffineTransform& toA, const gm::AffineTransform& fromB, const gm::AffineTransform& toB, CastResult& result) = 0;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif