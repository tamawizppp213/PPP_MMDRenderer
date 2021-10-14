//////////////////////////////////////////////////////////////////////////////////
//            @file　　  ContinuousConvexCollision.hpp
//            @brief    CCD (Convexs)
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
#ifndef CONTINUOUS_CONVEX_COLLISION_HPP
#define CONTINUOUS_CONVEX_COLLISION

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConvexCast.hpp"
#include "VoronoiSimplexSolver.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class ConvexPenetrationDepthSolver;
class ConvexShape;
class StaticPlaneShape;

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			ContinuousConvexCollision
*************************************************************************//**
*  @class     ContinuousConvexCollision
*  @brief     temp
*****************************************************************************/
class ContinuousConvexCollision : public ConvexCast
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool CalculateTimeOfImpact(const gm::AffineTransform& fromA, const gm::AffineTransform& toA, const gm::AffineTransform& fromB, const gm::AffineTransform& toB, CastResult& result) override;


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ContinuousConvexCollision(const ConvexShape* shapeA, const ConvexShape* shapeB, VolonoiSimplexSolver* simplexSolver, ConvexPenetrationDepthSolver* penetrationDepthSolver);
	ContinuousConvexCollision(const ConvexShape* shapeA, const StaticPlaneShape* plane);

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void ComputeClosestPoints(const gm::AffineTransform& transformA, const gm::AffineTransform& transformB, struct PointCollector& pointCollector);


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	VolonoiSimplexSolver*         _simplexSolver;
	ConvexPenetrationDepthSolver* _penetrationDepthSolver;
	const ConvexShape*            _convexA;
	const ConvexShape*            _convexB;
	const StaticPlaneShape*       _planeShape;
};
#endif