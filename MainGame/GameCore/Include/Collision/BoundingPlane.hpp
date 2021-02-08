//////////////////////////////////////////////////////////////////////////////////
///             @file   BoundingPlane.hpp
///             @brief  Bounding Plane (for DirectX Collision)
///             @author Toide Yutaro
///             @date   2021_02_08
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef BOUNDING_PLANE_HPP
#define BOUNDING_PLANE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <DirectXMath.h>
#include <DirectXCollision.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//								class
//////////////////////////////////////////////////////////////////////////////////
namespace DirectX
{
	struct BoundingTriangle
	{
		XMFLOAT3 Point_A;
		XMFLOAT3 Point_B;
		XMFLOAT3 Point_C;
	};

	struct BoundingPlane
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		void CreatePlane(const XMFLOAT3& pointA, const XMFLOAT3& pointB, const XMFLOAT3& pointC);
		void CreatePlane(const XMFLOAT3& planePoint, const XMFLOAT3& planeNormal);
		void CreatePlane(const BoundingTriangle& triangle);

		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/
		XMFLOAT4 PlaneEquation;

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		BoundingPlane()                                = default;
		BoundingPlane(const BoundingPlane&)            = default;
		BoundingPlane& operator=(const BoundingPlane&) = default;
		BoundingPlane(BoundingPlane&&)                 = default;
		BoundingPlane& operator=(BoundingPlane&&)      = default;

		BoundingPlane(const XMFLOAT4& planeEquation) { this->PlaneEquation = planeEquation; }
		
		BoundingPlane(const BoundingTriangle& triangle);
		BoundingPlane(const XMFLOAT3& planePoint, const XMFLOAT3& planeNormal);
		BoundingPlane(const XMFLOAT3& pointA, const XMFLOAT3& pointB, const XMFLOAT3& pointC);

	private:

	};
}

#endif