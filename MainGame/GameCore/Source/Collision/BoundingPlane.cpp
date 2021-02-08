//////////////////////////////////////////////////////////////////////////////////
///             @file   BoundingPlane.hpp
///             @brief  Bounding Plane (for DirectX Collision)
///             @author Toide Yutaro
///             @date   2021_02_08
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Collision/BoundingPlane.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//								Implement
//////////////////////////////////////////////////////////////////////////////////
namespace DirectX
{
	BoundingPlane::BoundingPlane(const BoundingTriangle& triangle)
	{
		CreatePlane(triangle);
	}

	BoundingPlane::BoundingPlane(const XMFLOAT3& planePoint, const XMFLOAT3& planeNormal)
	{
		CreatePlane(planePoint, planeNormal);
	}

	BoundingPlane::BoundingPlane(const XMFLOAT3& pointA, const XMFLOAT3& pointB, const XMFLOAT3& pointC)
	{
		CreatePlane(pointA, pointB, pointC);
	}

	/****************************************************************************
	*							CreatePlane
	*************************************************************************//**
	*  @fn        void BoundingPlane::CreatePlane(const XMFLOAT3& pointA, const XMFLOAT3& pointB, const XMFLOAT3& pointC)
	*  @brief     Create Plane
	*  @param[in] const XMFLOAT3& pointA
	*  @param[in] const XMFLOAT3& pointB
	*  @param[in] const XMFLOAT3& pointC
	*  @return 　　void
	*****************************************************************************/
	void BoundingPlane::CreatePlane(const XMFLOAT3& pointA, const XMFLOAT3& pointB, const XMFLOAT3& pointC)
	{
		XMVECTOR a = XMLoadFloat3(&pointA);
		XMVECTOR b = XMLoadFloat3(&pointB);
		XMVECTOR c = XMLoadFloat3(&pointC);
		XMStoreFloat4(&PlaneEquation, XMPlaneFromPoints(a, b, c));
	}

	/****************************************************************************
	*							CreatePlane
	*************************************************************************//**
	*  @fn        void BoundingPlane::CreatePlane(const BoundingTriangle& triangle)
	*  @brief     Create Plane
	*  @param[in] const XMFLOAT3& planePoint
	*  @param[in] const XMFLOAT3& planeNormal
	*  @return 　　void
	*****************************************************************************/
	void BoundingPlane::CreatePlane(const XMFLOAT3& planePoint, const XMFLOAT3& planeNormal)
	{
		XMVECTOR point  = XMLoadFloat3(&planePoint);
		XMVECTOR normal = XMLoadFloat3(&planeNormal);
		XMStoreFloat4(&PlaneEquation, XMPlaneFromPointNormal(point, normal));
	}

	/****************************************************************************
	*							CreatePlane
	*************************************************************************//**
	*  @fn        void BoundingPlane::CreatePlane(const BoundingTriangle& triangle)
	*  @brief     Create Plane
	*  @param[in] const BoundingTriangle& triangle
	*  @return 　　void
	*****************************************************************************/
	void BoundingPlane::CreatePlane(const BoundingTriangle& triangle)
	{
		XMVECTOR pointA = XMLoadFloat3(&triangle.Point_A);
		XMVECTOR pointB = XMLoadFloat3(&triangle.Point_B);
		XMVECTOR pointC = XMLoadFloat3(&triangle.Point_C);
		XMStoreFloat4(&PlaneEquation, XMPlaneFromPoints(pointA, pointB, pointC));
	}
};