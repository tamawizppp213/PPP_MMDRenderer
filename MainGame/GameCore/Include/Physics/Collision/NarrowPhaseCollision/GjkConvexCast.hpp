//////////////////////////////////////////////////////////////////////////////////
//            @file　　  GJKConvexCast.hpp
//            @brief    GJKConvexCast
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_12(first checked)
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
#ifndef GJK_CONVEX_CAST_HPP
#define GJK_CONVEX_CAST_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConvexCast.hpp"
#include "VoronoiSimplexSolver.hpp"
class ConvexShape;
class MinkowskiSumShape;
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			 GJKConvexCast
*************************************************************************//**
*  @class     GJKConvexCast
*  @brief      GJKConvexCast
*****************************************************************************/
class GJKConvexCast : public ConvexCast
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool CalculateTimeOfImpact(const gm::AffineTransform& fromA, const gm::AffineTransform& toA, const gm::AffineTransform& fromB, const gm::AffineTransform& toB, CastResult& result) override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	GJKConvexCast(const ConvexShape* convexA, const ConvexShape* convexB, VolonoiSimplexSolver* simplexSolver);

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	VolonoiSimplexSolver* _simplexSolver;
	const ConvexShape* _convexA;
	const ConvexShape* _convexB;
};
#endif