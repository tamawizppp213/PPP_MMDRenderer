//////////////////////////////////////////////////////////////////////////////////
///             @file   Keyboard.cpp
///             @brief  Keyboard
///             @author Toide Yutaro
///             @date   2020_11_28
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Camera.hpp"
#include "GameCore/Include/Screen.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace DirectX;

//////////////////////////////////////////////////////////////////////////////////
//							Implement
//////////////////////////////////////////////////////////////////////////////////
Camera::Camera()
{
	Screen screen;
	SetLens(0.25f * DirectX::XM_PI, screen.AspectRatio(), 1.0f, 1000.0f);
}

Camera::~Camera()
{

}

XMVECTOR Camera::GetPosition() const
{
	return XMLoadFloat3(&_position);
}

XMFLOAT3 Camera::GetPosition3f() const
{
	return this->_position;
}

void Camera::SetPosition(const XMFLOAT3& position)
{
	_position  = position;
	_viewDirty = true;
}

void Camera::SetPosition(float x, float y, float z)
{
	_position  = XMFLOAT3(x, y, z);
	_viewDirty = true;
}


void Camera::RotateRoll(float angle)
{
	// Rotate up and right vector about the look vector.

	XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&_look), angle);

	XMStoreFloat3(&_up   , XMVector3TransformNormal(XMLoadFloat3(&_up   ), rotate));
	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), rotate));

	_viewDirty = true;
}

void Camera::RotatePitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&_right), angle);

	XMStoreFloat3(&_up,   XMVector3TransformNormal(XMLoadFloat3(&_up  ), rotate));
	XMStoreFloat3(&_look, XMVector3TransformNormal(XMLoadFloat3(&_look), rotate));

	_viewDirty = true;
}

void Camera::RotateYaw(float angle)
{
	// Rotate right and look vector about the up vector.

	XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&_up), angle);

	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), rotate));
	XMStoreFloat3(&_look , XMVector3TransformNormal(XMLoadFloat3(&_look) , rotate));

	_viewDirty = true;
}

void Camera::RotateWorldX(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationX(angle);

	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), R));
	XMStoreFloat3(&_up,    XMVector3TransformNormal(XMLoadFloat3(&_up),    R));
	XMStoreFloat3(&_look,  XMVector3TransformNormal(XMLoadFloat3(&_look),  R));

	_viewDirty = true;
}

void Camera::RotateWorldY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), R));
	XMStoreFloat3(&_up,    XMVector3TransformNormal(XMLoadFloat3(&_up),    R));
	XMStoreFloat3(&_look,  XMVector3TransformNormal(XMLoadFloat3(&_look),  R));

	_viewDirty = true;
}

void Camera::RotateWorldZ(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationZ(angle);

	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), R));
	XMStoreFloat3(&_up,    XMVector3TransformNormal(XMLoadFloat3(&_up),    R));
	XMStoreFloat3(&_look,  XMVector3TransformNormal(XMLoadFloat3(&_look),  R));

	_viewDirty = true;
}

XMVECTOR Camera::GetRight() const
{
	return XMLoadFloat3(&_right);
}

XMFLOAT3 Camera::GetRight3f() const
{
	return _right;
}

XMVECTOR Camera::GetUp() const
{
	return XMLoadFloat3(&_up);
}

XMFLOAT3 Camera::GetUp3f() const
{
	return _up;
}

XMVECTOR Camera::GetLook() const
{
	return XMLoadFloat3(&_look);
}

XMFLOAT3 Camera::GetLook3f() const
{
	return _look;
}

float Camera::GetNearZ() const
{
	return frustum.NearZ;
}

float Camera::GetFarZ() const
{
	return frustum.FarZ;
}

float Camera::GetAspect() const
{
	return frustum.Aspect;
}

float Camera::GetFovVertical() const
{
	return frustum.FovVertical;
}

float Camera::GetFovHorizontal() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atanf(halfWidth / frustum.NearZ);
}

float Camera::GetNearWindowWidth() const
{
	return frustum.Aspect * frustum.NearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return frustum.NearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return frustum.Aspect * frustum.FarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return frustum.FarWindowHeight;
}

XMMATRIX Camera::GetViewMatrix() const
{
	//assert(!_viewDirty);
	return XMLoadFloat4x4(&_view);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
	return XMLoadFloat4x4(&_proj);
}

XMFLOAT4X4 Camera::GetViewMatrix4x4f() const
{
	assert(!_viewDirty);
	return _view;
}

XMFLOAT4X4 Camera::GetProjectionMatrix4x4f() const
{
	return _proj;
}

void Camera::SetLens(float fovVertical, float aspect, float nearZ, float farZ)
{
	frustum.FovVertical = fovVertical;
	frustum.Aspect      = aspect;
	frustum.NearZ       = nearZ;
	frustum.FarZ        = farZ;

	frustum.NearWindowHeight = 2.0f * frustum.NearZ * tanf(0.5f * frustum.FovVertical);
	frustum.FarWindowHeight  = 2.0f * frustum.FarZ  * tanf(0.5f * frustum.FovVertical);

	// Perspective Field of View Left-Handed
	XMMATRIX P = XMMatrixPerspectiveFovLH(frustum.FovVertical, frustum.Aspect, frustum.NearZ, frustum.FarZ);
	XMStoreFloat4x4(&_proj, P);
}

void Camera::SetZRange(float nearZ, float farZ)
{
	frustum.NearZ = nearZ;
	frustum.FarZ  = farZ;
}



void Camera::LookAt(FXMVECTOR position, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR look  = XMVector3Normalize(target - position); // diff
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, look));
	XMVECTOR up    = XMVector3Cross(look, right);

	XMStoreFloat3(&_position, position);
	XMStoreFloat3(&_look , look );
	XMStoreFloat3(&_right, right);
	XMStoreFloat3(&_up   , up   );

	_viewDirty = true;

}

void Camera::LookAt(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&position);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	_viewDirty = true;
}

void Camera::Strafe(float distance)
{
	// _position += d * _right
	XMVECTOR s = XMVectorReplicate(distance);
	XMVECTOR r = XMLoadFloat3(&_right);
	XMVECTOR p = XMLoadFloat3(&_position);
	XMStoreFloat3(&_position, XMVectorMultiplyAdd(s, r, p));

	_viewDirty = true;
}

void Camera::Walk(float distance)
{
	// _position += d*_look
	XMVECTOR s = XMVectorReplicate(distance);
	XMVECTOR l = XMLoadFloat3(&_look);
	XMVECTOR p = XMLoadFloat3(&_position);
	XMStoreFloat3(&_position, XMVectorMultiplyAdd(s, l, p));

	_viewDirty = true;
}

void Camera::UpdateViewMatrix()
{
	if (_viewDirty)
	{
		XMVECTOR right    = XMLoadFloat3(&_right);
		XMVECTOR up       = XMLoadFloat3(&_up);
		XMVECTOR look     = XMLoadFloat3(&_look);
		XMVECTOR position = XMLoadFloat3(&_position);

		// Keep camera's axes orthogonal to each other and of unit length.
		look  = XMVector3Normalize(look);
		up    = XMVector3Normalize(XMVector3Cross(look, right));

		// U, L already ortho-normal, so no need to normalize cross product.
		right = XMVector3Cross(up, look);

		// Fill in the view matrix entries
		float x = -XMVectorGetX(XMVector3Dot(position, right));
		float y = -XMVectorGetX(XMVector3Dot(position, up));
		float z = -XMVectorGetX(XMVector3Dot(position, look));

		XMStoreFloat3(&_right, right);
		XMStoreFloat3(&_up,    up);
		XMStoreFloat3(&_look,  look);

		_view(0, 0) = _right.x;
		_view(1, 0) = _right.y;
		_view(2, 0) = _right.z;
		_view(3, 0) = x;

		_view(0, 1) = _up.x;
		_view(1, 1) = _up.y;
		_view(2, 1) = _up.z;
		_view(3, 1) = y;

		_view(0, 2) = _look.x;
		_view(1, 2) = _look.y;
		_view(2, 2) = _look.z;
		_view(3, 2) = z;

		_view(0, 3) = 0.0f;
		_view(1, 3) = 0.0f;
		_view(2, 3) = 0.0f;
		_view(3, 3) = 1.0f;

		_viewDirty = false;
	}
}