//////////////////////////////////////////////////////////////////////////////////
///             @file   JjkEpa.hpp
///             @brief  GJK _ EPA Algorithm class
///             @author Partially edit by Toide
///             @date   2021_10_14
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2008 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the
use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software in a
product, an acknowledgment in the product documentation would be appreciated
but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

/*
GJK-EPA collision solver by Nathanael Presson, 2008
*/
#pragma once
#ifndef GJK_EPA_HPP
#define GJK_EPA_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionShapes/ConvexShape.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
struct GjkEpaSolver2
{
public:
	struct Result
	{
		enum eStatus
		{
			Separated,   // Shape doesn't penetrate
			Penetrating, // Shapes are penetrating
			GJK_Failed,  // gjk failed, no big issue, shapes are probably just 'touching'	
			EPA_Failed   //  EPA phase fail, bigger problem, need to save parameters, and debug
		} Status;

		gm::Vector3 Witnesses[2];
		gm::Vector3 Normal;
		float       Distance;
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static int   StackSizeRequirement();
	static bool  Distance(const ConvexShape* shape0, const gm::AffineTransform& transform0, const ConvexShape* shape1, const gm::AffineTransform& transform1, const gm::Vector3& guess, Result& results);
	static bool  Penetration(const ConvexShape* shape0, const gm::AffineTransform& transform0, const ConvexShape* shape1, const gm::AffineTransform& transform1, const gm::Vector3& guess, Result& results, bool useMargins = true);
	static float SignedDistance(const gm::Vector3& position, float margin, const ConvexShape* shape, const gm::AffineTransform& transform, Result& results);
	static bool  SignedDistance(const ConvexShape* shape0, const gm::AffineTransform& transform0, const ConvexShape* shape1, const gm::AffineTransform& transform1, const gm::Vector3& guess, Result& results);


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