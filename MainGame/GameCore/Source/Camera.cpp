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

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//							Implement
//////////////////////////////////////////////////////////////////////////////////
Camera::Camera()
{
	SetLens(0.25f * DirectX::XM_PI, _screen.AspectRatio(), 1.0f, 1000.0f);
}

Camera::~Camera()
{

}

Vector3 Camera::GetPosition() const
{
	return Vector3(_position);
}

Float3 Camera::GetPosition3f() const
{
	return this->_position;
}

void Camera::SetPosition(const Float3& position)
{
	_position  = position;
	_viewDirty = true;
}

void Camera::SetPosition(float x, float y, float z)
{
	_position  = Float3(x, y, z);
	_viewDirty = true;
}


void Camera::RotateRoll(float angle)
{
	// Rotate up and right vector about the look vector.

	Matrix4 rotate = RotationAxis(_look, angle);

	_up    = TransformNormal(Vector3(_up)   , rotate).ToFloat3();
	_right = TransformNormal(Vector3(_right), rotate).ToFloat3();

	_viewDirty = true;
}

void Camera::RotatePitch(float angle)
{
	// Rotate up and look vector about the right vector.

	Matrix4 rotate = RotationAxis(Vector3(_right), angle);

	_up   = TransformNormal(Vector3(_up), rotate).ToFloat3();
	_look = TransformNormal(Vector3(_look), rotate).ToFloat3();

	_viewDirty = true;
}

void Camera::RotateYaw(float angle)
{
	// Rotate right and look vector about the up vector.

	Matrix4 rotate = RotationAxis(Vector3(_up), angle);

	_right = TransformNormal(Vector3(_right), rotate).ToFloat3();
	_look  = TransformNormal(Vector3(_look) , rotate).ToFloat3();

	_viewDirty = true;
}

void Camera::RotateWorldX(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	Matrix4 R = RotationX(angle);

	_right = TransformNormal(Vector3(_right), R).ToFloat3();
	_up    = TransformNormal(Vector3(_up),    R).ToFloat3();
	_look  = TransformNormal(Vector3(_look),  R).ToFloat3();

	_viewDirty = true;
}

void Camera::RotateWorldY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	Matrix4 R = RotationY(angle);

	_right = TransformNormal(_right, R).ToFloat3();
	_up    = TransformNormal(_up, R).ToFloat3();
	_look  = TransformNormal(Vector3(_look), R).ToFloat3();
	_viewDirty = true;
}

void Camera::RotateWorldZ(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	Matrix4 R = RotationZ(angle);

	_right = TransformNormal(Vector3(_right), R).ToFloat3();
	_up    = TransformNormal(Vector3(_up),    R).ToFloat3();
	_look  = TransformNormal(Vector3(_look),  R).ToFloat3();

	_viewDirty = true;
}

Vector3 Camera::GetRight() const
{
	return Vector3(_right);
}

Float3 Camera::GetRight3f() const
{
	return _right;
}

Vector3 Camera::GetUp() const
{
	return Vector3(_up);
}

Float3 Camera::GetUp3f() const
{
	return _up;
}

Vector3 Camera::GetLook() const
{
	return Vector3(_look);
}

Float3 Camera::GetLook3f() const
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

Matrix4 Camera::GetViewMatrix() const
{
	//assert(!_viewDirty);
	return Matrix4(_view);
}

Matrix4 Camera::GetProjectionMatrix() const
{
	return Matrix4(_proj);
}

Float4x4 Camera::GetViewMatrix4x4f() const
{
	assert(!_viewDirty);
	return _view;
}

Float4x4 Camera::GetProjectionMatrix4x4f() const
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
	Matrix4 P = PerspectiveFovLH(frustum.FovVertical, frustum.Aspect, frustum.NearZ, frustum.FarZ);
	_proj     = P.ToFloat4x4();
}

void Camera::SetZRange(float nearZ, float farZ)
{
	frustum.NearZ = nearZ;
	frustum.FarZ  = farZ;
}



void Camera::LookAt(Vector3 position, Vector3 target, Vector3 worldUp)
{
	Vector3 look  = Normalize(target - position); // diff
	Vector3 right = Normalize(Cross(worldUp, look));
	Vector3 up    = Cross(look, right);

	_position = position.ToFloat3();
	_look     = look    .ToFloat3();
	_right    = right   .ToFloat3();
	_up       = up      .ToFloat3();

	_viewDirty = true;

}

void Camera::LookAt(const Float3& position, const Float3& target, const Float3& up)
{
	Vector3 P = Vector3(position);
	Vector3 T = Vector3(target);
	Vector3 U = Vector3(up);

	LookAt(P, T, U);

	_viewDirty = true;
}

void Camera::Strafe(float distance)
{
	// _position += d * _right
	Vector3 s = ReplicateVector3(distance);
	Vector3 r = Vector3(_right);
	Vector3 p = Vector3(_position);
	_position = (s * r + p).ToFloat3();

	_viewDirty = true;
}

void Camera::Walk(float distance)
{
	// _position += d*_look
	Vector3 s = ReplicateVector3(distance);
	Vector3 l = Vector3(_look);
	Vector3 p = Vector3(_position);
	_position = (s * l + p).ToFloat3();

	_viewDirty = true;
}

void Camera::UpdateViewMatrix()
{
	if (_viewDirty)
	{
		Vector3 right    = Vector3(_right);
		Vector3 up       = Vector3(_up);
		Vector3 look     = Vector3(_look);
		Vector3 position = Vector3(_position);

		// Keep camera's axes orthogonal to each other and of unit length.
		look  = Normalize(look);
		up    = Normalize(Cross(look, right));

		// U, L already ortho-normal, so no need to normalize cross product.
		right = Cross(up, look);

		// Fill in the view matrix entries
		float x = -Dot(position, right);
		float y = -Dot(position, up);
		float z = -Dot(position, look);

		_right = right.ToFloat3();
		_up    = up.   ToFloat3();
		_look  = look.ToFloat3();

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