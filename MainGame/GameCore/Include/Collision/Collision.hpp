//////////////////////////////////////////////////////////////////////////////////
///             @file   Collision.hpp
///             @brief  Collision Detect, Make Collider...
///             @author Toide Yutaro
///             @date   2021_02_04 (Collision 2D) 2021_02_08 (Collision 3D)
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef COLLISION_HPP
#define COLLISION_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct Collider2D;
struct Collider3D;

//////////////////////////////////////////////////////////////////////////////////
//								Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			CollisionDetection
*************************************************************************//**
*  @class     Collision 
*  @brief     Collsiion Detection
*****************************************************************************/
class Collision
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static bool OnCollision2D(const Collider2D& first, const Collider2D& second);
	static bool OnCollision3D(const Collider3D& first, const Collider3D& second);
	static bool IsContainedObjectInFrustum(const Collider3D& frustum, const Collider3D& object);

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
#pragma region 2D
	static bool Circle_vs_Circle(const Collider2D& first, const Collider2D& second);
	static bool Rect_vs_Circle  (const Collider2D& first, const Collider2D& second);
	static bool Circle_vs_Rect  (const Collider2D& first, const Collider2D& second);
	static bool Rect_vs_Rect    (const Collider2D& first, const Collider2D& second);
#pragma endregion 2D

#pragma region 3D
#pragma region Box
	static bool Box_vs_Box                (const Collider3D& first, const Collider3D& second);
	static bool Box_vs_OrientedBox        (const Collider3D& first, const Collider3D& second);
	static bool Box_vs_Sphere             (const Collider3D& first, const Collider3D& second);
	static bool Box_vs_Frustum            (const Collider3D& first, const Collider3D& second);
	static bool Box_vs_Plane              (const Collider3D& first, const Collider3D& second);
#pragma endregion Box
#pragma region OrientedBox
	static bool OrientedBox_vs_Box        (const Collider3D& first, const Collider3D& second);
	static bool OrientedBox_vs_OrientedBox(const Collider3D& first, const Collider3D& second);
	static bool OrientedBox_vs_Sphere     (const Collider3D& first, const Collider3D& second);
	static bool OrientedBox_vs_Frustum    (const Collider3D& first, const Collider3D& second);
	static bool OrientedBox_vs_Plane      (const Collider3D& first, const Collider3D& second);
#pragma endregion OrientedBox
#pragma region Sphere
	static bool Sphere_vs_Box             (const Collider3D& first, const Collider3D& second);
	static bool Sphere_vs_OrientedBox     (const Collider3D& first, const Collider3D& second);
	static bool Sphere_vs_Sphere          (const Collider3D& first, const Collider3D& second);
	static bool Sphere_vs_Frustum         (const Collider3D& first, const Collider3D& second);
	static bool Sphere_vs_Plane           (const Collider3D& first, const Collider3D& second);
#pragma endregion Sphere
#pragma region Frustum
	static bool Frustum_vs_Box            (const Collider3D& first, const Collider3D& second);
	static bool Frustum_vs_OrientedBox    (const Collider3D& first, const Collider3D& second);
	static bool Frustum_vs_Sphere         (const Collider3D& first, const Collider3D& second);
	static bool Frustum_vs_Frustum        (const Collider3D& first, const Collider3D& second);
	static bool Frustum_vs_Plane          (const Collider3D& first, const Collider3D& second);
#pragma endregion Frustum
#pragma region Plane
	static bool Plane_vs_Box              (const Collider3D& first, const Collider3D& second);
	static bool Plane_vs_OrientedBox      (const Collider3D& first, const Collider3D& second);
	static bool Plane_vs_Sphere           (const Collider3D& first, const Collider3D& second);
	static bool Plane_vs_Frustum          (const Collider3D& first, const Collider3D& second);
	static bool Plane_vs_Plane            (const Collider3D& first, const Collider3D& second);
#pragma endregion Plane

#pragma region FrustumContained
	static bool IsContained_Box_In_Frustum        (const Collider3D& frustum, const Collider3D& box);
	static bool IsContained_OrientedBox_In_Frustum(const Collider3D& frustum, const Collider3D& orientedBox);
	static bool IsContained_Sphere_In_Frustum     (const Collider3D& frustum, const Collider3D& sphere);
	static bool IsContained_Frustum_In_Frustum    (const Collider3D& frustum, const Collider3D& frustumObject);
#pragma endregion FrustumContained
#pragma endregion 3D

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

#endif