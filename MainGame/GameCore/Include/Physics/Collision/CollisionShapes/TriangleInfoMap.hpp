//////////////////////////////////////////////////////////////////////////////////
///             @file   TriangleInfoMap.hpp
///             @brief  TriangleInfoMap
///             @author Partially Edit by Toide
///             @date   2021_09_18
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
#pragma once
#ifndef TRIANGLE_INFO_MAP_HPP
#define TRIANGLE_INFO_MAP_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMVectorUtility.hpp"
#include "GameMath/Include/GMHashMap.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define TRIANGLE_INFO_V0V1_CONVEX 1
#define TRIANGLE_INFO_V1V2_CONVEX 2
#define TRIANGLE_INFO_V2V0_CONVEX 4

#define TRIANGLE_INFO_V0V1_SWAP_NORMALB 8
#define TRIANGLE_INFO_V1V2_SWAP_NORMALB 16
#define TRIANGLE_INFO_V2V0_SWAP_NORMALB 32

//////////////////////////////////////////////////////////////////////////////////
//                         TriangleINfo
//////////////////////////////////////////////////////////////////////////////////
struct TriangleInfo
{
	TriangleInfo()
	{
		EdgeV0V1Angle = gm::GM_2PI;
		EdgeV1V2Angle = gm::GM_2PI;
		EdgeV2V0Angle = gm::GM_2PI;
		Flags = 0;
	}

	int Flags;
	float EdgeV0V1Angle;
	float EdgeV1V2Angle;
	float EdgeV2V0Angle;
};

using InternalTriangleInfoMap = gm::HashMap<gm::HashInt, TriangleInfo>;
/****************************************************************************
*				  			ConcaveShape
*************************************************************************//**
*  @class     ConcaveShape
*  @brief     a
*****************************************************************************/
struct TriangleInfoMap :public InternalTriangleInfoMap
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	float ConvexEpsilon;         ///used to determine if an edge or contact normal is convex, using the dot product
	float PlanarEpsilon;         ///used to determine if a triangle edge is planar with zero angle
	float EqualVertexThreshold;    ///used to compute connectivity: if the distance between two vertices is smaller than m_equalVertexThreshold, they are considered to be 'shared'
	float EdgeDistanceThreshold;  ///used to determine edge contacts: if the closest distance between a contact point and an edge is smaller than this distance threshold it is considered to "hit the edge"
	float MaxEdgeAngleThreshold;  //ignore edges that connect triangles at an angle larger than this m_maxEdgeAngleThreshold
	float ZeroAreaThreshold;       ///used to determine if a triangle is degenerate (length squared of cross product of 2 triangle edges < threshold)

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	TriangleInfoMap()
	{
		ConvexEpsilon = 0.00f;
		PlanarEpsilon = 0.0001f;
		EqualVertexThreshold  = 0.0001f * 0.0001f;
		EdgeDistanceThreshold = 0.1f;
		ZeroAreaThreshold     = 0.0001f * 0.0001f;
		MaxEdgeAngleThreshold = gm::GM_2PI;
	}
	virtual ~TriangleInfoMap() {};

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif