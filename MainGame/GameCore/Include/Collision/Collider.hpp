//////////////////////////////////////////////////////////////////////////////////
///             @file   Collider.hpp
///             @brief  Collider 2D, 3D
///             @author Toide Yutaro
///             @date   2021_02_07
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef COLLIDER_HPP
#define COLLIDER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "BoundingPlane.hpp"
#include <DirectXMath.h>
#include <DirectXCollision.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//								class
//////////////////////////////////////////////////////////////////////////////////
enum class ColliderShape2D
{
	Circle,
	Rectangle,
	MAX_SHAPE_COUNT
};

enum class ColliderShape3D
{
	Box,
	OrientedBox,
	Sphere,
	Frustum,
	Plane,
	MAX_SHAPE_COUNT
};

struct Collider
{
	union
	{
		ColliderShape2D shapeType2D;
		ColliderShape3D shapeType3D;
	};
};

/****************************************************************************
*				  			Collider2D
*************************************************************************//**
*  @class     Collider2D
*  @brief     Collider for 2D Object
*****************************************************************************/
struct Collider2D : public Collider
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static Collider2D CreateCircleCollider   (DirectX::XMFLOAT2& centerPos, float radius);
	static Collider2D CreateRectangleCollider(DirectX::XMFLOAT2& centerPos, float width, float height);
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	union
	{
		struct Circle
		{
			float radius;
		} circle;
		struct Rectangle
		{
			float width;
			float height;
		} rectangle;
	};
	void SetCenterPosition(DirectX::XMFLOAT3& center);
	DirectX::XMFLOAT3 centerPosition;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Collider2D();
	Collider2D(const Collider2D& collider);
	Collider2D& operator=(const Collider2D& collider) = default;
	~Collider2D() {};
private:
	
};

/****************************************************************************
*				  			Collider3D
*************************************************************************//**
*  @class     Collider3D
*  @brief     Collider for 3D Object
*****************************************************************************/
struct Collider3D : public Collider
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static Collider3D CreateBoxCollider        (const DirectX::XMFLOAT3& centerPosition, const DirectX::XMFLOAT3& boxSize);
	static Collider3D CreateBoxCollider        (const DirectX::XMFLOAT3& centerPosition, float width = 1.0f, float height = 1.0f, float depth = 1.0f);
	static Collider3D CreateOrientedBoxCollider(const DirectX::XMFLOAT3& centerPosition, const DirectX::XMFLOAT3& boxSize, const DirectX::XMFLOAT4& rotation);
	static Collider3D CreateSphereCollider     (const DirectX::XMFLOAT3& centerPosition, float radius);
	static Collider3D CreateFrustumCollider    (const DirectX::XMFLOAT3& originPosition, const DirectX::XMFLOAT4& orientation, const DirectX::XMFLOAT4& rltbSlope, float nearPlane = 0, float farPlane = 1.0f);
	static Collider3D CreateFrustumCollider    (const DirectX::XMFLOAT3& originPosition, const DirectX::XMFLOAT4& orientation, float rightSlope = 1.0f, float leftSlope = -1.0f, float topSlope = 1.0f, float bottomSlope = -1.0f, float nearPlane = 0, float farPlane = 1.0f);
	static Collider3D CreatePlaneCollider      (const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT3& pointC);
	static Collider3D CreatePlaneCollider      (const DirectX::XMFLOAT3& planePoint, const DirectX::XMFLOAT3& planeNormal);
	static Collider3D CreatePlaneCollider      (const DirectX::BoundingTriangle& triangle);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	union
	{
		DirectX::BoundingBox         box;
		DirectX::BoundingOrientedBox orientedBox;
		DirectX::BoundingSphere      sphere;
		DirectX::BoundingFrustum     frustum;
		DirectX::BoundingPlane       plane;
	};

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Collider3D();
	Collider3D(const Collider3D& collider);
	Collider3D& operator=(const Collider3D& collider) = default;
	~Collider3D() {};

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif