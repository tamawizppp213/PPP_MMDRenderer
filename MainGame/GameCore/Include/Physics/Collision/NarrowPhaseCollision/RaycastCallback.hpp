//////////////////////////////////////////////////////////////////////////////////
//            @file　　  RaycastCallback.hpp
//            @brief    Raycast Callback class
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_12 (first checked)
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
#ifndef RAYCAST_TRIANGLE_CALLBACK_HPP
#define RAYCAST_TRIANGLE_CALLBACK_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../CollisionShapes/TriangleCallback.hpp"
#include "../../../../../GameMath/Include/GMTransform.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class ConvexShape;

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class TriangleRayCastCallback : public TriangleCallback
{
public:
	enum EFlags
	{
		None                           = 0x0000,
		FilterBackFaces                = 0x0001,
		KeepUnflippedNormal            = 0x0002,
		UseSubSimplexConvexCastRaytest = 0x0004,
		UseGjkConvexCastRaytest        = 0x0008,
		DisableHeightFieldAccelerator  = 0x0010,
		Terminator                     = 0xFFFFFFFF
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void  ProcessTriangle(gm::Vector3* triangle, int partID, int triangleIndex) override;
	virtual float ReportHit(const gm::Vector3& hitNormalLocal, float hitFraction, int partID, int triangleIndex) = 0;


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3  From;
	gm::Vector3  To;
	unsigned int Flags;
	float        HitFraction;


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	TriangleRayCastCallback(const gm::Vector3& from, const gm::Vector3& to, unsigned int flags = 0);
	

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class TriangleConvexCastCallback : public TriangleCallback
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void ProcessTriangle(gm::Vector3* triangle, int partID, int triangleIndex) override;
	virtual float ReportHit(const gm::Vector3& hitNormal, const gm::Vector3& hitPointLocal, float hitFraction, int partID, int triangleIndex) = 0;


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const ConvexShape*  ConvexShapePtr;
	gm::AffineTransform ConvexShapeFrom;
	gm::AffineTransform ConvexShapeTo;
	gm::AffineTransform TriangleToWorld;
	float               HitFraction;
	float               TriangleCollisionMargin;
	float               AllowedPenetration;


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	TriangleConvexCastCallback(const ConvexShape* convexShape, const gm::AffineTransform& convexShapeFrom, const gm::AffineTransform& convexShapeTo, const gm::AffineTransform& triangleToWorld, const float triangleCollisionMargin);

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif