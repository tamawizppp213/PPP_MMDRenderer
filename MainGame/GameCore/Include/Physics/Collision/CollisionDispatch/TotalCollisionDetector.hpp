//////////////////////////////////////////////////////////////////////////////////
//            @file　　  TotalCollisionDetector.hpp
//            @brief    Implement Total collision Algorithms
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_03
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
#ifndef TOTAL_COLLISION_DETECTOR_HPP
#define TOTAL_COLLISION_DETECTOR_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/DiscreteCollisionDetectorInterface.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class BoxShape;
class SphereShape;
class TriangleShape;

//////////////////////////////////////////////////////////////////////////////////
//                               Class
//////////////////////////////////////////////////////////////////////////////////
#pragma region SphereTriagnle
/****************************************************************************
*				  			HashedSimplePairCache
*************************************************************************//**
*  @class     HashedSimplePairCache
*  @brief     temp
*****************************************************************************/
struct SphereTriangleDetector : public DiscreteCollisionDetectorInterface
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void GetClosestPoints(const ClosestPointInput& input, Result& output, IDebugDraw* debugDraw, bool swapResult = false);
	bool Collide(const gm::Vector3& sphereCenter, gm::Vector3& point, gm::Vector3& resultNormal, float& depth, float& timeOfImpact, float contactBreakingThreshold);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	SphereTriangleDetector(SphereShape* sphere, TriangleShape* triangle, float contactBreakingThreshold);
	virtual ~SphereTriangleDetector() {};

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool PointInTriangle(const gm::Vector3 vertices[], const gm::Vector3& normal, gm::Vector3* p);
	bool FaceContains(const gm::Vector3& p, const gm::Vector3* vertices, gm::Vector3& normal);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	SphereShape*   _sphere;
	TriangleShape* _triangle;
	float          _contactBreakingThreshold;

};
#pragma endregion SphereTriangle
#pragma region BoxBoxDetector
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
struct BoxBoxDetector : public DiscreteCollisionDetectorInterface
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void GetClosestPoints(const ClosestPointInput& input, Result& output,IDebugDraw* debugDraw, bool swapResult = false) override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const BoxShape* Box1;
	const BoxShape* Box2;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	BoxBoxDetector(const BoxShape* box1, const BoxShape* box2);
	virtual ~BoxBoxDetector() {};

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#pragma endregion BoxBoxDetector
#endif