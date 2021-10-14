//////////////////////////////////////////////////////////////////////////////////
//            @file　　  VoronoiSimplexSolver.hpp
//            @brief    VoronoiSimplexSolver
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
#ifndef POINT_COLLECTOR_HPP
#define POINT_COLLECTOR_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DiscreteCollisionDetectorInterface.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			PointCollector
*************************************************************************//**
*  @class     PointCollector
*  @brief     Point collect
*****************************************************************************/
struct PointCollector : public DiscreteCollisionDetectorInterface::Result
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void SetShapeIdentifiersA(int partID0, int index0) override { (void)partID0; (void)index0; }
	void SetShapeIdentifiersB(int partID1, int index1) override { (void)partID1; (void)index1; }
	void AddContactPoint(const gm::Vector3& normalOnBInWorld, const gm::Vector3& pointInWorld, float depth) override
	{
		if (depth < Distance)
		{
			HasResult        = true;
			NormalOnBInWorld = normalOnBInWorld;
			PointInWorld     = pointInWorld;
			Distance         = depth;
		}
	}

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3 NormalOnBInWorld;
	gm::Vector3 PointInWorld;
	float       Distance;
	bool        HasResult;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PointCollector() : Distance(FLT_MAX), HasResult(false) {};

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif