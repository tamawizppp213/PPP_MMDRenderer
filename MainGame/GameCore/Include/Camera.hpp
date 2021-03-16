//////////////////////////////////////////////////////////////////////////////////
///             @file   Camera.hpp
///             @brief  Camera Class
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <DirectXMath.h>
#include "DirectX12/Include/DirectX12MathHelper.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//								Camera 
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			Frustum
*************************************************************************//**
*  @struct    Frustum
*  @brief     Frustum
*****************************************************************************/
struct Frustum
{
	float NearZ;
	float FarZ;
	float Aspect;
	float FovVertical;
	float NearWindowHeight;
	float FarWindowHeight;
};
/****************************************************************************
*				  			Camera
*************************************************************************//**
*  @class     Camera
*  @brief     Camera
*****************************************************************************/
class Camera
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	// World Camera Position
	DirectX::XMVECTOR GetPosition()   const;
	DirectX::XMFLOAT3 GetPosition3f() const;
	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetPosition(float x, float y, float z);

	// Rotate the camera
	void RotateRoll  (float angle);
	void RotatePitch (float angle);
	void RotateYaw   (float angle);
	void RotateWorldX(float angle);
	void RotateWorldY(float angle);
	void RotateWorldZ(float angle);

	// Get camera basis vectors
	DirectX::XMVECTOR GetRight()   const;
	DirectX::XMFLOAT3 GetRight3f() const;
	DirectX::XMVECTOR GetUp()      const;
	DirectX::XMFLOAT3 GetUp3f()    const;
	DirectX::XMVECTOR GetLook()    const;
	DirectX::XMFLOAT3 GetLook3f()  const;

	// Get frustum properties
	float GetNearZ()         const;
	float GetFarZ()          const;
	float GetAspect()        const;
	float GetFovVertical()   const;
	float GetFovHorizontal() const;

	// Get near and far plane dimensions in view space coordinates
	float GetNearWindowWidth()  const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth()   const;
	float GetFarWindowHeight()  const;

	// Get View / Projection Matrix
	DirectX::XMMATRIX   GetViewMatrix()           const;
	DirectX::XMMATRIX   GetProjectionMatrix()     const;
	DirectX::XMFLOAT4X4 GetViewMatrix4x4f()       const;
	DirectX::XMFLOAT4X4 GetProjectionMatrix4x4f() const;


	// Set frusum
	void SetLens(float fovVertical, float aspect, float nearZ, float farZ);
	void SetZRange(float nearZ, float farZ);

	// Define camera space via LookAt parameters
	void LookAt(DirectX::FXMVECTOR position, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	// Camera Motion
	void Strafe(float distance);
	void Walk  (float distance);

	// After modifying camera position / orientation, call to rebuild the matrix.
	void UpdateViewMatrix();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Camera();
	Camera(const Camera&)            = default;
	Camera& operator=(const Camera&) = default;
	Camera(Camera&&)                 = default;
	Camera& operator=(Camera&&)      = default;

	~Camera();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	DirectX::XMFLOAT3 _position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 _right    = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 _up       = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 _look     = { 0.0f, 0.0f, 1.0f };

	bool _viewDirty = true;

	Frustum frustum;

	DirectX::XMFLOAT4X4 _view = DirectXMathHelper::Identity4X4();
	DirectX::XMFLOAT4X4 _proj = DirectXMathHelper::Identity4X4();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

};
#endif
