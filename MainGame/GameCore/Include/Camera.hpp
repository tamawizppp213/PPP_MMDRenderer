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
#include "GameMath/Include/GMMatrix.hpp"
#include "GameCore/Include/Screen.hpp"
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
	gm::Vector3 GetPosition()   const;
	gm::Float3  GetPosition3f() const;
	void SetPosition(const gm::Float3& position);
	void SetPosition(float x, float y, float z);

	// Rotate the camera
	void RotateRoll  (float angle);
	void RotatePitch (float angle);
	void RotateYaw   (float angle);
	void RotateWorldX(float angle);
	void RotateWorldY(float angle);
	void RotateWorldZ(float angle);

	// Get camera basis vectors
	gm::Vector3 GetRight()   const;
	gm::Float3  GetRight3f() const;
	gm::Vector3 GetUp()      const;
	gm::Float3  GetUp3f()    const;
	gm::Vector3 GetLook()    const;
	gm::Float3  GetLook3f()  const;

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
	gm::Matrix4   GetViewMatrix()           const;
	gm::Matrix4   GetProjectionMatrix()     const;
	gm::Float4x4  GetViewMatrix4x4f()       const;
	gm::Float4x4  GetProjectionMatrix4x4f() const;


	// Set frusum
	void SetLens(float fovVertical, float aspect, float nearZ, float farZ);
	void SetOrthoLens(float width, float height, float nearZ, float farZ);
	void SetZRange(float nearZ, float farZ);

	// Define camera space via LookAt parameters
	void LookAt(gm::Vector3 position, gm::Vector3 target, gm::Vector3 worldUp);
	void LookAt(const gm::Float3& position, const gm::Float3& target, const gm::Float3& up);

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
	gm::Float3 _position = { 0.0f, 0.0f, 0.0f };
	gm::Float3 _right    = { 1.0f, 0.0f, 0.0f };
	gm::Float3 _up       = { 0.0f, 1.0f, 0.0f };
	gm::Float3 _look     = { 0.0f, 0.0f, 1.0f };

	bool _viewDirty = true;

	Frustum frustum;

	gm::Float4x4 _view = gm::MatrixIdentityF();
	gm::Float4x4 _proj = gm::MatrixIdentityF();

	Screen _screen;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

};
#endif
