//////////////////////////////////////////////////////////////////////////////////
///             @file   Collision.hpp
///             @brief  Collision Detect, Make Collider...
///             @author Toide Yutaro
///             @date   2021_02_05 (Collision 2D (DirectX Coordinates)), 2021_02_08 (Collision 3D)
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Collision/Collision.hpp"
#include "GameCore/Include/Collision/Collider.hpp"
#include <algorithm>
#include <DirectXCollision.h>
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define MAX_COLLISION_2D_TYPE 2
#define MAX_COLLISION_3D_TYPE 5

//////////////////////////////////////////////////////////////////////////////////
//								Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*							OnCollision2D
*************************************************************************//**
*  @fn        bool Collision::OnCollision2D(const Collider2D& first, const Collider2D& second)
*  @brief     Collider 2D Detection 
*  @param[in] const Collider2D& first
*  @param[in] const Collider2D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::OnCollision2D(const Collider2D& first, const Collider2D& second)
{
	static bool(* const functionList[(int)ColliderShape2D::MAX_SHAPE_COUNT][(int)ColliderShape2D::MAX_SHAPE_COUNT])
		(const Collider2D&, const Collider2D&) =
	{
		{Circle_vs_Circle, Circle_vs_Rect},
		{Rect_vs_Circle,   Rect_vs_Rect},
	};

	return functionList[static_cast<int>(first.shapeType2D)][static_cast<int>(second.shapeType2D)](first, second);
}

/****************************************************************************
*							OnCollision3D
*************************************************************************//**
*  @fn        bool Collision::OnCollision3D(const Collider2D& first, const Collider2D& second)
*  @brief     Collider 3D Detection
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::OnCollision3D(const Collider3D& first, const Collider3D& second)
{
	static bool(* const functionList[(int)ColliderShape3D::MAX_SHAPE_COUNT][(int)ColliderShape3D::MAX_SHAPE_COUNT])
		(const Collider3D&, const Collider3D&) = 
	{
		{Box_vs_Box,         Box_vs_OrientedBox,         Box_vs_Sphere,         Box_vs_Frustum,         Box_vs_Plane},
		{OrientedBox_vs_Box, OrientedBox_vs_OrientedBox, OrientedBox_vs_Sphere, OrientedBox_vs_Frustum, OrientedBox_vs_Plane},
		{Sphere_vs_Box,      Sphere_vs_OrientedBox,      Sphere_vs_Sphere,      Sphere_vs_Frustum,      Sphere_vs_Plane},
		{Frustum_vs_Box,     Frustum_vs_OrientedBox,     Frustum_vs_Sphere,     Frustum_vs_Frustum,     Frustum_vs_Plane},
		{Plane_vs_Box,       Plane_vs_OrientedBox,       Plane_vs_Sphere,       Plane_vs_Frustum,       Plane_vs_Plane}
	};
	return functionList[static_cast<int>(first.shapeType3D)][static_cast<int>(second.shapeType3D)](first, second);
}

/****************************************************************************
*							IsContainedObjectInFrustum
*************************************************************************//**
*  @fn        bool Collision::IsContainedObjectInFrustum(const Collider3D& frustum, const Collider3D& object)
*  @brief     Is Contained  In Frustum
*  @param[in] const Collider3D& frustum
*  @param[in] const Collider3D& object
*  @return 　　bool
*****************************************************************************/
bool Collision::IsContainedObjectInFrustum(const Collider3D& frustum, const Collider3D& object)
{
	if (frustum.shapeType3D != ColliderShape3D::Frustum)
	{
		::OutputDebugString(L"Error!: frustum is not assigned.");
		return false;
	}

	static bool(*functionList[(int)ColliderShape3D::MAX_SHAPE_COUNT - 1])
		(const Collider3D&, const Collider3D&) =
	{
		IsContained_Box_In_Frustum,
		IsContained_OrientedBox_In_Frustum,
		IsContained_Sphere_In_Frustum,
		IsContained_Frustum_In_Frustum
	};
	return functionList[static_cast<int>(object.shapeType3D)](frustum, object);
}
#pragma endregion Public Function

#pragma region Private Function
#pragma region 2D
/****************************************************************************
*							Circle_vs_Circle
*************************************************************************//**
*  @fn        bool Collision::Circle_vs_Circle(const Collider2D& first, const Collider2D& second)
*  @brief     Collider 2D Detection (circle vs circle)
*  @param[in] const Collider2D& first
*  @param[in] const Collider2D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Circle_vs_Circle(const Collider2D& first, const Collider2D& second)
{
	using namespace DirectX;
	/*-------------------------------------------------------------------
	-        distance = || (first.centerPos - second.centerPos) ||;
	---------------------------------------------------------------------*/
	float distance = 0.0f;
	DirectX::XMStoreFloat(&distance,XMVector3Length(XMVectorSubtract(XMLoadFloat3(&first.centerPosition), XMLoadFloat3(&first.centerPosition))));
	float rSum = first.circle.radius + second.circle.radius;

	return (distance < rSum) ? true : false;
}

/****************************************************************************
*							Rect_vs_Circle
*************************************************************************//**
*  @fn        bool Collision::Rect_vs_Circle(const Collider2D& first, const Collider2D& second)
*  @brief     Collider 2D Detection (rect vs circle)
*  @param[in] const Collider2D& first
*  @param[in] const Collider2D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Rect_vs_Circle(const Collider2D& first, const Collider2D& second)
{
	using namespace DirectX;
	
	const XMFLOAT2 rectLeftTop     (first.centerPosition.x  - first. rectangle.width / 2, first. centerPosition.y + first.rectangle.height / 2);
	const XMFLOAT2 rectRightBottom (first.centerPosition.x  + first. rectangle.width / 2, first. centerPosition.y - first.rectangle.height / 2);
	const XMFLOAT2 circlePos    = XMFLOAT2(second.centerPosition.x, second.centerPosition.y);
	const XMFLOAT2 dLeftTop     = XMFLOAT2(circlePos.x - rectLeftTop.x,     circlePos.y - rectLeftTop.y);
	const XMFLOAT2 dRightBottom = XMFLOAT2(circlePos.x - rectRightBottom.x, circlePos.y - rectRightBottom.y);
	const float radius          = second.circle.radius;

	/*-------------------------------------------------------------------
	-        Rectangle Collision Detection
	---------------------------------------------------------------------*/
	if(rectLeftTop.x - radius > circlePos.x || rectRightBottom.x + radius < circlePos.x  ||
		rectLeftTop.y < circlePos.y || rectRightBottom.y > circlePos.y)
	{
		return false;
	}
	/*-------------------------------------------------------------------
	-        Upper Left Collision Detection
	---------------------------------------------------------------------*/
	if(rectLeftTop.x > circlePos.x && rectLeftTop.y < circlePos.y && 
		!(dLeftTop.x * dLeftTop.x + dLeftTop.y * dLeftTop.y < radius * radius))
	{
		return false;
	}
	/*-------------------------------------------------------------------
	-        Upper Right Collision Detection
	---------------------------------------------------------------------*/
	if (rectRightBottom.x < circlePos.x && rectRightBottom.y < circlePos.y &&
		!(dRightBottom.x * dRightBottom.x + dLeftTop.y * dLeftTop.y < radius * radius))
	{
		return false;
	}
	/*-------------------------------------------------------------------
	-        Lower Left Collision Detection
	---------------------------------------------------------------------*/
	if (rectLeftTop.x > circlePos.x && rectRightBottom.y > circlePos.y &&
		!(dLeftTop.x * dLeftTop.x + dRightBottom.y * dRightBottom.y < radius * radius))
	{
		return false;
	}
	/*-------------------------------------------------------------------
	-        Lower Right Collision Detection
	---------------------------------------------------------------------*/
	if (rectRightBottom.x < circlePos.x && rectRightBottom.y > circlePos.y &&
		!(dRightBottom.x * dRightBottom.x + dRightBottom.y * dRightBottom.y < radius * radius))
	{
		return false;
	}
	return true;
}

/****************************************************************************
*							Circle_vs_Rect
*************************************************************************//**
*  @fn        bool Collision::Circle_vs_Rect(const Collider2D& first, const Collider2D& second)
*  @brief     Collider 2D Detection (circle vs rect)
*  @param[in] const Collider2D& first
*  @param[in] const Collider2D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Circle_vs_Rect(const Collider2D& first, const Collider2D& second)
{
	return Rect_vs_Circle(second, first);
}

bool Collision::Rect_vs_Rect(const Collider2D& first, const Collider2D& second)
{
	using namespace DirectX;

	const XMFLOAT2 firstLeftTop     (first. centerPosition.x - first. rectangle.width / 2, first. centerPosition.y + first. rectangle.height / 2);
	const XMFLOAT2 firstRightBottom (first. centerPosition.x + first. rectangle.width / 2, first. centerPosition.y - first. rectangle.height / 2);
	const XMFLOAT2 secondLeftTop    (second.centerPosition.x - second.rectangle.width / 2, second.centerPosition.y + second.rectangle.height / 2);
	const XMFLOAT2 secondRightBottom(second.centerPosition.x + second.rectangle.width / 2, second.centerPosition.y - second.rectangle.height / 2);
	
	if (firstRightBottom.x < secondLeftTop.x || firstLeftTop.x > secondRightBottom.x) { return false; }
	if (firstRightBottom.y > secondLeftTop.y || firstLeftTop.y < secondRightBottom.y) { return false; }
	return true;
}
#pragma endregion 2D
#pragma region 3D

#pragma region Box
/****************************************************************************
*							Box_vs_Box
*************************************************************************//**
*  @fn        bool Collision::Box_vs_Box(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (box vs box)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Box_vs_Box(const Collider3D& first, const Collider3D& second)
{
	return first.box.Intersects(second.box);
}

/****************************************************************************
*							Box_vs_OrientedBox
*************************************************************************//**
*  @fn        bool Collision::Box_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (box vs orientedBox)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Box_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
{
	return first.box.Intersects(second.orientedBox);
}

/****************************************************************************
*							Box_vs_Sphere
*************************************************************************//**
*  @fn        bool Collision::Box_vs_Sphere(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (box vs sphere)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Box_vs_Sphere(const Collider3D& first, const Collider3D& second)
{
	return first.box.Intersects(second.sphere);
}

/****************************************************************************
*							Box_vs_Frustum
*************************************************************************//**
*  @fn        bool Collision::Box_vs_Frustum(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (box vs frustum)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Box_vs_Frustum(const Collider3D& first, const Collider3D& second)
{
	return first.box.Intersects(second.frustum);
}

/****************************************************************************
*							Box_vs_Plane
*************************************************************************//**
*  @fn        bool Collision::Box_vs_Box(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (box vs box)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Box_vs_Plane(const Collider3D& first, const Collider3D& second)
{
	using namespace DirectX;
	XMVECTOR plane = XMLoadFloat4(&second.plane.PlaneEquation);
	return (first.box.Intersects(plane) == PlaneIntersectionType::INTERSECTING) ? true : false;
}
#pragma endregion Box
#pragma region OrientedBox
/****************************************************************************
*							OrientedBox_vs_Box
*************************************************************************//**
*  @fn        bool Collision::OrientedBox_vs_Box(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (orientedBox vs box)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::OrientedBox_vs_Box(const Collider3D& first, const Collider3D& second)
{
	return Box_vs_OrientedBox(second, first);
}

/****************************************************************************
*							OrientedBox_vs_Box
*************************************************************************//**
*  @fn        bool Collision::OrientedBox_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (orientedBox vs orinetedBox)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::OrientedBox_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
{
	return first.orientedBox.Intersects(second.orientedBox);
}

/****************************************************************************
*							OrientedBox_vs_Box
*************************************************************************//**
*  @fn        bool Collision::OrientedBox_vs_Sphere(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (orientedBox vs sphere)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::OrientedBox_vs_Sphere(const Collider3D& first, const Collider3D& second)
{
	return first.orientedBox.Intersects(second.sphere);
}

/****************************************************************************
*							OrientedBox_vs_Frustum
*************************************************************************//**
*  @fn        bool Collision::OrientedBox_vs_Frustum(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (orientedBox vs frustum)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::OrientedBox_vs_Frustum(const Collider3D& first, const Collider3D& second)
{
	return first.orientedBox.Intersects(second.frustum);
}

/****************************************************************************
*							OrientedBox_vs_Plane
*************************************************************************//**
*  @fn        bool Collision::OrientedBox_vs_Plane(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (orientedBox vs plane)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::OrientedBox_vs_Plane(const Collider3D& first, const Collider3D& second)
{
	using namespace DirectX;
	XMVECTOR plane = XMLoadFloat4(&second.plane.PlaneEquation);
	return (first.orientedBox.Intersects(plane) == PlaneIntersectionType::INTERSECTING) ? true : false;
}
#pragma endregion OrientedBox
#pragma region Sphere
/****************************************************************************
*							Sphere_vs_Box
*************************************************************************//**
*  @fn        bool Collision::Sphere_vs_Box(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (sphere vs box)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Sphere_vs_Box(const Collider3D& first, const Collider3D& second)
{
	return Box_vs_Sphere(second, first);
}

/****************************************************************************
*							Sphere_vs_OrientedBox
*************************************************************************//**
*  @fn        bool Collision::Sphere_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (sphere vs orientedBox)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Sphere_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
{
	return OrientedBox_vs_Sphere(second, first);
}

/****************************************************************************
*							Sphere_vs_Sphere
*************************************************************************//**
*  @fn        bool Collision::Sphere_vs_Sphere(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (sphere vs sphere)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Sphere_vs_Sphere(const Collider3D& first, const Collider3D& second)
{
	return first.sphere.Intersects(second.sphere);
}

/****************************************************************************
*							Sphere_vs_Frustum
*************************************************************************//**
*  @fn        bool Collision::Sphere_vs_Frustum(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (sphere vs frustum)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Sphere_vs_Frustum(const Collider3D& first, const Collider3D& second)
{
	return first.sphere.Intersects(second.frustum);
}

/****************************************************************************
*							Sphere_vs_Plane
*************************************************************************//**
*  @fn        bool Collision::Sphere_vs_Plane(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (sphere vs plane)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Sphere_vs_Plane(const Collider3D& first, const Collider3D& second)
{
	using namespace DirectX;
	XMVECTOR plane = XMLoadFloat4(&second.plane.PlaneEquation);
	return (first.sphere.Intersects(plane) == PlaneIntersectionType::INTERSECTING) ? true : false;
}
#pragma endregion Sphere
#pragma region Frustum
/****************************************************************************
*							Frustum_vs_Box
*************************************************************************//**
*  @fn        bool Collision::Frustum_vs_Box(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (frustum vs box)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Frustum_vs_Box(const Collider3D& first, const Collider3D& second)
{
	return Box_vs_Frustum(second, first);
}

/****************************************************************************
*							Frustum_vs_OrientedBox
*************************************************************************//**
*  @fn        bool Collision::Frustum_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (frustum vs orientedBox)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Frustum_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
{
	return OrientedBox_vs_Frustum(second, first);
}

/****************************************************************************
*							Frustum_vs_Sphere
*************************************************************************//**
*  @fn        bool Collision::Frustum_vs_Sphere(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (frustum vs sphere)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Frustum_vs_Sphere(const Collider3D& first, const Collider3D& second)
{
	return Sphere_vs_Frustum(second, first);
}

/****************************************************************************
*							Frustum_vs_Frustum
*************************************************************************//**
*  @fn        bool Collision::Frustum_vs_Frustum(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (frustum vs frustum)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Frustum_vs_Frustum(const Collider3D& first, const Collider3D& second)
{
	return first.frustum.Intersects(second.frustum);
}

/****************************************************************************
*							Frustum_vs_Plane
*************************************************************************//**
*  @fn        bool Collision::Frustum_vs_Plane(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (frustum vs plane)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Frustum_vs_Plane(const Collider3D& first, const Collider3D& second)
{
	using namespace DirectX;
	XMVECTOR plane = XMLoadFloat4(&second.plane.PlaneEquation);
	return (first.frustum.Intersects(plane) == PlaneIntersectionType::INTERSECTING) ? true : false;
}
#pragma endregion Frustum
#pragma region Plane
/****************************************************************************
*							Plane_vs_Box
*************************************************************************//**
*  @fn        bool Collision::Plane_vs_Box(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (plane vs box)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Plane_vs_Box(const Collider3D& first, const Collider3D& second)
{
	return Box_vs_Plane(second, first);
}

/****************************************************************************
*							Plane_vs_OrientedBox
*************************************************************************//**
*  @fn        bool Collision::Plane_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (plane vs orientedBox)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Plane_vs_OrientedBox(const Collider3D& first, const Collider3D& second)
{
	return OrientedBox_vs_Plane(second, first);
}

/****************************************************************************
*							Plane_vs_Sphere
*************************************************************************//**
*  @fn        bool Collision::Plane_vs_Sphere(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (plane vs sphere)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Plane_vs_Sphere(const Collider3D& first, const Collider3D& second)
{
	return Sphere_vs_Plane(second, first);
}

/****************************************************************************
*							Plane_vs_Frustum
*************************************************************************//**
*  @fn        bool Collision::Plane_vs_Frustum(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (plane vs frustum)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Plane_vs_Frustum(const Collider3D& first, const Collider3D& second)
{
	return Frustum_vs_Plane(second, first);
}

/****************************************************************************
*							Plane_vs_Plane
*************************************************************************//**
*  @fn        bool Collision::Plane_vs_Plane(const Collider3D& first, const Collider3D& second)
*  @brief     Collider 3D Detection (plane vs plane)
*  @param[in] const Collider3D& first
*  @param[in] const Collider3D& second
*  @return 　　bool
*****************************************************************************/
bool Collision::Plane_vs_Plane(const Collider3D& first, const Collider3D& second)
{
	using namespace DirectX;
	XMVECTOR planeFirst  = XMLoadFloat4(&first .plane.PlaneEquation);
	XMVECTOR planeSecond = XMLoadFloat4(&second.plane.PlaneEquation);
	XMVECTOR firstPoint, secondPoint;
	DirectX::XMPlaneIntersectPlane(&firstPoint, &secondPoint, planeFirst, planeSecond);

	return (!XMVector3IsNaN(firstPoint) && !XMVector3IsNaN(secondPoint)) ? true : false;
}
#pragma endregion Plane

#pragma region FrustumContained
/****************************************************************************
*							IsContained_Box_In_Frustum
*************************************************************************//**
*  @fn        bool Collision::IsContained_Box_In_Frustum(const Collider3D& frustum, const Collider3D& box)
*  @brief     Is Contained box In Frustum
*  @param[in] const Collider3D& frustum
*  @param[in] const Collider3D& box
*  @return 　　bool
*****************************************************************************/
bool Collision::IsContained_Box_In_Frustum(const Collider3D& frustum, const Collider3D& box)
{
	using namespace DirectX;
	return (frustum.frustum.Contains(box.box) == (ContainmentType::CONTAINS || ContainmentType::INTERSECTS)) ? true : false;
}

/****************************************************************************
*							IsContained_OrientedBox_In_Frustum
*************************************************************************//**
*  @fn        bool Collision::IsContained_OrientedBox_In_Frustum(const Collider3D& frustum, const Collider3D& box)
*  @brief     Is Contained OrientedBox In Frustum
*  @param[in] const Collider3D& frustum
*  @param[in] const Collider3D& orientedBox
*  @return 　　bool
*****************************************************************************/
bool Collision::IsContained_OrientedBox_In_Frustum(const Collider3D& frustum, const Collider3D& orientedBox)
{
	using namespace DirectX;
	return (frustum.frustum.Contains(orientedBox.orientedBox) == (ContainmentType::CONTAINS || ContainmentType::INTERSECTS)) ? true : false;
}

/****************************************************************************
*							IsContained_Sphere_In_Frustum
*************************************************************************//**
*  @fn        bool Collision::IsContained_Sphere_In_Frustum(const Collider3D& frustum, const Collider3D& box)
*  @brief     Is Contained Sphere In Frustum
*  @param[in] const Collider3D& frustum
*  @param[in] const Collider3D& sphere
*  @return 　　bool
*****************************************************************************/
bool Collision::IsContained_Sphere_In_Frustum(const Collider3D& frustum, const Collider3D& sphere)
{
	using namespace DirectX;
	return (frustum.frustum.Contains(sphere.sphere) == (ContainmentType::CONTAINS || ContainmentType::INTERSECTS)) ? true : false;
}

/****************************************************************************
*							IsContained_Frustum_In_Frustum
*************************************************************************//**
*  @fn        bool Collision::IsContained_Frustum_In_Frustum(const Collider3D& frustum, const Collider3D& box)
*  @brief     Is Contained Frustum In Frustum
*  @param[in] const Collider3D& frustum
*  @param[in] const Collider3D& frustum
*  @return 　　bool
*****************************************************************************/
bool Collision::IsContained_Frustum_In_Frustum(const Collider3D& frustum, const Collider3D& frustumObject)
{
	using namespace DirectX;
	return (frustum.frustum.Contains(frustum.frustum) == (ContainmentType::CONTAINS || ContainmentType::INTERSECTS)) ? true : false;
}
#pragma endregion FrustumContained
#pragma endregion 3D
#pragma endregion Private Function
