//////////////////////////////////////////////////////////////////////////////////
///             @file   Collider.cpp
///             @brief  Make Collider...
///             @author Toide Yutaro
///             @date   2021_02_04 (Collision 2D) 2021_02_08 (Collision 3D)
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Collision/Collider.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 26495)
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//								Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region 2D
Collider2D::Collider2D() 
{
	this->shapeType2D   = ColliderShape2D::Circle;
	this->circle.radius = 0.1f;
}

Collider2D::Collider2D(const Collider2D& collider)
{
	this->shapeType2D = collider.shapeType2D;
	switch (this->shapeType2D)
	{
	case ColliderShape2D::Circle:
		this->circle = collider.circle;
		break;
	case ColliderShape2D::Rectangle:
		this->rectangle = collider.rectangle;
		break;
	}
}

/****************************************************************************
*							CreateCircleCollider
*************************************************************************//**
*  @fn        Collider2D Collider2D::CreateCircleCollider(DirectX::XMFLOAT2& centerPos, float radius)
*  @brief     Create Circle Collider
*  @param[in] const DirectX:XMFLOAT2& centerPos
*  @param[in] float radius
*  @return 　　Collider2D
*****************************************************************************/
Collider2D Collider2D::CreateCircleCollider(Float2& centerPos, float radius)
{
	Collider2D collider;

	collider.centerPosition = Float3(centerPos.x, centerPos.y, 0.0f);
	collider.shapeType2D    = ColliderShape2D::Circle;
	collider.circle.radius  = radius;
	return collider;
}

/****************************************************************************
*							CreateRectangleCollider
*************************************************************************//**
*  @fn        Collider2D Collider2D::CreateRectangleCollider(DirectX::XMFLOAT2& centerPos, float radius)
*  @brief     Create Rectangle Collider
*  @param[in] const DirectX:XMFLOAT2& centerPos
*  @param[in] float width
*  @param[in] float height
*  @return 　　Collider2D
*****************************************************************************/
Collider2D Collider2D::CreateRectangleCollider(Float2& centerPos, float width, float height)
{
	Collider2D collider;
	collider.centerPosition   = Float3(centerPos.x, centerPos.y, 0.0f);
	collider.shapeType2D      = ColliderShape2D::Rectangle;
	collider.rectangle.width  = width;
	collider.rectangle.height = height;
	return collider;
}

/****************************************************************************
*							SetCenterPosition
*************************************************************************//**
*  @fn        Collider2D::SetCenterPosition(DirectX::XMFLOAT3& center)
*  @brief     Set Center Position
*  @param[in] DirectX::XMFLOAT3& center
*  @return 　　void
*****************************************************************************/
void Collider2D::SetCenterPosition(Float2& center)
{
	this->centerPosition = Float3(center.x, center.y, 0.0f);
}
#pragma endregion 2D

#pragma region 3D
Collider3D::Collider3D()
{
	this->shapeType3D   = ColliderShape3D::Sphere;
	this->sphere.Center = Float3(0.0f, 0.0f, 0.0f);
	this->sphere.Radius = 0.1f;
}

Collider3D::Collider3D(const Collider3D& collider)
{
	this->shapeType3D = collider.shapeType3D;
	switch (this->shapeType3D)
	{
	case ColliderShape3D::Box:
		this->box = collider.box;
		break;
	case ColliderShape3D::OrientedBox:
		this->orientedBox = collider.orientedBox;
		break;
	case ColliderShape3D::Sphere:
		this->sphere = collider.sphere;
		break;
	case ColliderShape3D::Frustum:
		this->frustum = collider.frustum;
		break;
	case ColliderShape3D::Plane:
		this->plane = collider.plane;
		break;
	}
}

/****************************************************************************
*							CreateBoxCollider
*************************************************************************//**
*  @fn        Collider3D Collider3D::CreateBoxCollider(const DirectX::XMFLOAT3& centerPosition, const DirectX::XMFLOAT3& boxSize)
*  @brief     Create Box Collider
*  @param[in] const DirectX:XMFLOAT3& centerPos
*  @param[in] const DirectX::XMFLOAT3& boxSize
*  @return 　　Collider3D
*****************************************************************************/
Collider3D Collider3D::CreateBoxCollider(const Float3& centerPosition, const Float3& boxSize)
{
	Collider3D collider;
	collider.box.Center  = centerPosition;
	collider.box.Extents = Float3(boxSize.x / 2, boxSize.y / 2, boxSize.z / 2);
	return collider;
}

/****************************************************************************
*							CreateBoxCollider
*************************************************************************//**
*  @fn        Collider3D Collider3D::CreateBoxCollider(const DirectX::XMFLOAT3& centerPosition, const DirectX::XMFLOAT3& boxSize)
*  @brief     Create Box Collider
*  @param[in] const DirectX:XMFLOAT3& centerPos
*  @param[in] float width
*  @param[in] float height
*  @param[in] float depth
*  @return 　　Collider3D
*****************************************************************************/
Collider3D Collider3D::CreateBoxCollider(const Float3& centerPosition, float width, float height, float depth)
{
	Collider3D collider;
	collider.box.Center  = centerPosition;
	collider.box.Extents = Float3(width / 2, height / 2, depth / 2);
	return collider;
}

/****************************************************************************
*							CreateOrientedBoxCollider
*************************************************************************//**
*  @fn        Collider3D Collider3D::CreateOrientedBoxCollider(const DirectX::XMFLOAT3& centerPosition, const DirectX::XMFLOAT3& boxSize)
*  @brief     Create OrientedBox Collider
*  @param[in] const DirectX:XMFLOAT3& centerPos
*  @param[in] const DirectX::XMFLOAT3& boxSize
*  @param[in] const DirectX::XMFLOAT4& rotation
*  @return 　　Collider3D
*****************************************************************************/
Collider3D Collider3D::CreateOrientedBoxCollider(const Float3& centerPosition, const Float3& boxSize, const Float4& rotation)
{
	Collider3D collider;
	collider.orientedBox.Center      = centerPosition;
	collider.orientedBox.Extents     = Float3 (boxSize.x / 2, boxSize.y / 2, boxSize.z / 2);
	collider.orientedBox.Orientation = rotation; // unit rotation quaternion (object->world).
	return collider;
}

/****************************************************************************
*							CreateSphereCollider
*************************************************************************//**
*  @fn        Collider3D Collider3D::CreateSphereCollider(const DirectX::XMFLOAT3& centerPosition, float radius)
*  @brief     Create Box Collider
*  @param[in] const DirectX:XMFLOAT3& centerPos
*  @param[in] float radius
*  @return 　　Collider3D
*****************************************************************************/
Collider3D Collider3D::CreateSphereCollider(const Float3& centerPosition, float radius)
{
	Collider3D collider;
	collider.sphere.Center = centerPosition;
	collider.sphere.Radius = radius;
	return collider;
}

/****************************************************************************
*							CreateFrustumCollider
*************************************************************************//**
*  @fn        Collider3D Collider3D::CreateFrustumCollider(const DirectX::XMFLOAT3& originPosition, const DirectX::XMFLOAT4& orientation, const DirectX::XMFLOAT4& rltbSlope, float nearPlane, float farPlane)
*  @brief     Create Frustum Collider
*  @param[in] const DirectX::XMFLOAT3& originPosition 
*  @param[in] const DirectX::XMFLOAT4& orientation
*  @param[in] const DirectX::XMFLOAT4& rltbSlope
*  @param[in] float nearPlane
*  @param[in] float farPlane
*  @return 　　Collider3D
*****************************************************************************/
Collider3D Collider3D::CreateFrustumCollider(const Float3& originPosition, const Float4& orientation, const Float4& rltbSlope, float nearPlane, float farPlane)
{
	Collider3D collider;
	collider.frustum.Origin      = originPosition;
	collider.frustum.Orientation = orientation;
	collider.frustum.RightSlope  = rltbSlope.x; // (z axis / x axis)
	collider.frustum.LeftSlope   = rltbSlope.y; // (-z axis / x axis)
	collider.frustum.TopSlope    = rltbSlope.z; // (z axis / y axis)
	collider.frustum.BottomSlope = rltbSlope.w; // (-z axis / y axis)
	return collider;
}

/****************************************************************************
*							CreateFrustumCollider
*************************************************************************//**
*  @fn        Collider3D Collider3D::CreateFrustumCollider(const DirectX::XMFLOAT3& originPosition, const DirectX::XMFLOAT4& orientation, float rightSlope, float leftSlope, float topSlope, float bottomSlope, float nearPlane, float farPlane)
*  @brief     Create Frustum Collider
*  @param[in] const DirectX::XMFLOAT3& originPosition
*  @param[in] const DirectX::XMFLOAT4& orientation
*  @param[in] float rightSlope
*  @param[in] float leftSlope 
*  @param[in] float topSlope 
*  @param[in] float bottomSlope
*  @param[in] float nearPlane
*  @param[in] float farPlane
*  @return 　　Collider3D
*****************************************************************************/
Collider3D Collider3D::CreateFrustumCollider(const Float3& originPosition, const Float4& orientation, float rightSlope, float leftSlope, float topSlope, float bottomSlope, float nearPlane, float farPlane)
{
	Collider3D collider;
	collider.frustum.Origin      = originPosition;
	collider.frustum.Orientation = orientation;
	collider.frustum.RightSlope  = rightSlope;
	collider.frustum.LeftSlope   = leftSlope;
	collider.frustum.TopSlope    = topSlope;
	collider.frustum.BottomSlope = bottomSlope;
	return collider;
}

/****************************************************************************
*							CreatePlaneCollider
*************************************************************************//**
*  @fn        Collider3D Collider3D::CreatePlaneCollider(const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT3& pointC)
*  @brief     Create Plane Collider
*  @param[in] const DirectX::XMFLOAT3& pointA
*  @param[in] const DirectX::XMFLOAT3& pointB
*  @param[in] const DirectX::XMFLOAT3& pointC
*  @return 　　Collider3D
*****************************************************************************/
Collider3D Collider3D::CreatePlaneCollider(const Float3& pointA, const Float3& pointB, const Float3& pointC)
{
	Collider3D collider;
	collider.plane.CreatePlane(pointA, pointB, pointC);
	return collider;
}

/****************************************************************************
*							CreatePlaneCollider
*************************************************************************//**
*  @fn        Collider3D Collider3D::CreatePlaneCollider(const DirectX::XMFLOAT3& planePoint, const DirectX::XMFLOAT3& planeNormal)
*  @brief     Create Plane Collider
*  @param[in] const DirectX::XMFLOAT3& planePoint
*  @param[in] const DirectX::XMFLOAT3& planeNormal
*  @return 　　Collider3D
*****************************************************************************/
Collider3D Collider3D::CreatePlaneCollider(const Float3& planePoint, const Float3& planeNormal)
{
	Collider3D collider;
	collider.plane.CreatePlane(planePoint, planeNormal);
	return collider;
}

/****************************************************************************
*							CreatePlaneCollider
*************************************************************************//**
*  @fn        Collider3D Collider3D::CreatePlaneCollider(const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT3& pointC)
*  @brief     Create Plane Collider
*  @param[in] const DirectX::BoundingTriangle& triangle
*  @return 　　Collider3D
*****************************************************************************/
Collider3D Collider3D::CreatePlaneCollider(const DirectX::BoundingTriangle& triangle)
{
	Collider3D collider;
	collider.plane.CreatePlane(triangle);
	return collider;
}
#pragma endregion 3D