//////////////////////////////////////////////////////////////////////////////////
///             @file   IDebugDraw.hpp
///             @brief  Draw wire frame interface
///             @author Partially edit by Toide
///             @date   2021_09_19
///                     2021_10_06 
//////////////////////////////////////////////////////////////////////////////////

/*Bullet Continuous Collision Detectionand Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.*/

#pragma once
#ifndef IDEBUG_DRAW_HPP
#define IDEBUG_DRAW_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMTransform.hpp"
#include "GameMath/Include/GMAlighedAllocator.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         SphereShape
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			SphereShape
*************************************************************************//**
*  @class     SphereShape
*  @brief     Sphere shape
*****************************************************************************/
class IDebugDraw
{
public:
	__declspec(align(16)) struct DefaultColors
	{
		gm::Vector3 ActiveObject;
		gm::Vector3 DeActivatedObject;
		gm::Vector3 WantsDeactivationObject;
		gm::Vector3 DisabledDeactivationObject;
		gm::Vector3 DisabledSimulationObject;
		gm::Vector3 AABB;
		gm::Vector3 ContactPoint;

		DefaultColors() : 
			ActiveObject(1, 1, 1), 
			DeActivatedObject(0,1,0),
			WantsDeactivationObject   (0, 1 ,1),
			DisabledDeactivationObject(1, 0, 0),
			DisabledSimulationObject(1,1,0),
			AABB(1, 0,0),
			ContactPoint(1,1,0)
		{

		}
	};

	enum DebugDrawModes
	{
		NoDebug              = 0x0000,
		DrawWireframe        = 0x0001,
		DrawAabb             = 0x0002,
		DrawFeaturesText     = 0x0004,
		DrawContactPoints    = 0x0008,
		NoDeactivation       = 0x0010,
		NoHelpText           = 0x0020,
		DrawText             = 0x0040,
		ProfileTimings       = 0x0080,
		EnableSatComparison  = 0x0100,
		DisableBulletLCP     = 0x0200,
		EnableCCD            = 0x0400,
		DrawConstraints      = 0x0800,
		DrawConstraintLimits = 0x1000,
		FastWireframe        = 0x2000,
		DrawNormals          = 0x4000,
		DrawFrames           = 0x8000,
		MAX_DEBUG_DRAW_MODE
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void DrawLine(const gm::Vector3& from, const gm::Vector3& to, const gm::Vector3& color) = 0;
	virtual void DrawLine();
	virtual void DrawSphere(const gm::Vector3& p, float radius, const gm::Vector3& color);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual DefaultColors GetDefaultColors() const { DefaultColors colors; return colors; }
	virtual DefaultColors SetDefaultColors(DefaultColors& colors) const {}

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	virtual ~IDebugDraw() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif