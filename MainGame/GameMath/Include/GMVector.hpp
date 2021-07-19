//////////////////////////////////////////////////////////////////////////////////
///             @file   GMVector.hpp
///             @brief  GMVector
///             @author James Stanard  // Partially modified by Toide.
///             @date   2021_05_09 (modified)
//////////////////////////////////////////////////////////////////////////////////

//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard 
//

#pragma once
#ifndef GM_VECTOR_HPP
#define GM_VECTOR_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GMScalar.hpp"

#pragma warning(disable: 26495)

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
namespace gm
{
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Float2;
	struct Float3;
	struct Float4;

	/****************************************************************************
	*				  			Float2
	*************************************************************************//**
	*  @class     Float2
	*  @brief     Float2 class for holding the value
	*****************************************************************************/
	struct Float2 : public DirectX::XMFLOAT2
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/

		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/
		INLINE bool operator == (const Float2& V) const noexcept { DirectX::XMVector2Equal(*this, V); };
		INLINE bool operator != (const Float2& V) const noexcept { DirectX::XMVector2NotEqual(*this, V); };

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		Float2()                    : XMFLOAT2(0.0f, 0.0f) {};
		Float2(float ix)            : XMFLOAT2(ix, ix)     {};
		Float2(float ix, float iy)  : XMFLOAT2(ix, iy)     {};
		Float2(const XMFLOAT2& V)             { this->x = V.x; this->y = V.y; }
		//Float2(DirectX::FXMVECTOR v)        { DirectX::XMStoreFloat2(this, v); } // キャストが簡単になりすぎる Float2 = (Float2)Vector2 + Vector2が出来てしまう)
		Float2(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; }
		explicit Float2(_In_reads_(2) const float* pArray) : XMFLOAT2(pArray) {}

		Float2(const Float2&)            = default;
		Float2& operator=(const Float2&) = default;
		Float2(Float2&&)                 = default;
		Float2& operator=(Float2&&)      = default;

		operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat2(this); }
	private:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
	};

	/****************************************************************************
	*				  			Float3
	*************************************************************************//**
	*  @class     Float3
	*  @brief     Float3 class for holding the value
	*****************************************************************************/
	struct Float3 : public DirectX::XMFLOAT3
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/

		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/
		INLINE bool operator == (const Float3& V) const noexcept { DirectX::XMVector3Equal(*this, V); };
		INLINE bool operator != (const Float3& V) const noexcept { DirectX::XMVector3NotEqual(*this, V); };

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		Float3()                             : XMFLOAT3(0.0f, 0.0f, 0.0f) {};
		Float3(float ix)                     : XMFLOAT3(ix, ix, ix) {};
		Float3(float ix, float iy, float iz) : XMFLOAT3(ix, iy, iz) {};
		Float3(const XMFLOAT3& V)             { this->x = V.x; this->y = V.y; this->z = V.z; }
		//Float3(DirectX::FXMVECTOR v)        { DirectX::XMStoreFloat3(this, v); } // キャストが簡単になりすぎる Float3 = (Float3)Vector3 + Vector3が出来てしまう)
		Float3(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2];}
		explicit Float3(_In_reads_(3) const float* pArray) : XMFLOAT3(pArray) {}

		Float3(const Float3&)            = default;
		Float3& operator=(const Float3&) = default;
		Float3(Float3&&)                 = default;
		Float3& operator=(Float3&&)      = default;

		operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat3(this); }
	private:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
	};

	/****************************************************************************
	*				  			Float4
	*************************************************************************//**
	*  @class     Float4
	*  @brief     Float4 class for holding the value
	*****************************************************************************/
	struct Float4 : public DirectX::XMFLOAT4
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/

		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/
		INLINE bool operator == (const Float4& V) const noexcept { DirectX::XMVector4Equal(*this, V); };
		INLINE bool operator != (const Float4& V) const noexcept { DirectX::XMVector4NotEqual(*this, V); };

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		Float4()                                       : XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) {};
		Float4(float ix)                               : XMFLOAT4(ix, ix, ix, ix) {};
		Float4(float ix, float iy, float iz, float iw) : XMFLOAT4(ix, iy, iz, iw) {};
		Float4(const XMFLOAT4& V)             { this->x = V.x; this->y = V.y; this->z = V.z; this->w = V.w;}
		//Float4(DirectX::FXMVECTOR v)        { DirectX::XMStoreFloat4(this, v); } // キャストが簡単になりすぎる Float2 = (Float2)Vector2 + Vector2が出来てしまう)
		Float4(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3];}
		explicit Float4(_In_reads_(4) const float* pArray) : XMFLOAT4(pArray) {}

		Float4(const Float4&)            = default;
		Float4& operator=(const Float4&) = default;
		Float4(Float4&&)                 = default;
		Float4& operator=(Float4&&)      = default;

		operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat4(this); }
	private:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
	};


	/****************************************************************************
	*				  			Vector2
	*************************************************************************//**
	*  @struct    Vector2
	*  @brief     Vector2 class for calculations  (note! : 16 bytes (for 16 alignment))
	*****************************************************************************/
	struct Vector2
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		INLINE Vector2 operator- ()           const { return Vector2(DirectX::XMVectorNegate(_vector)); }
		INLINE Vector2 operator+ (Vector2 v2) const { return Vector2(DirectX::XMVectorAdd(_vector, v2)); }
		INLINE Vector2 operator- (Vector2 v2) const { return Vector2(DirectX::XMVectorSubtract(_vector, v2)); }
		INLINE Vector2 operator* (Vector2 v2) const { return Vector2(DirectX::XMVectorMultiply(_vector, v2)); }
		INLINE Vector2 operator/ (Vector2 v2) const { return Vector2(DirectX::XMVectorDivide(_vector, v2)); }
		INLINE Vector2 operator* (Scalar  v2) const { return *this * Vector2(v2); }
		INLINE Vector2 operator/ (Scalar  v2) const { return *this / Vector2(v2); }
		INLINE Vector2 operator* (float   v2) const { return *this * Scalar(v2); }
		INLINE Vector2 operator/ (float   v2) const { return *this / Scalar(v2); }

		INLINE Vector2& operator += (Vector2 v) { *this = *this + v; return *this; }
		INLINE Vector2& operator -= (Vector2 v) { *this = *this - v; return *this; }
		INLINE Vector2& operator *= (Vector2 v) { *this = *this * v; return *this; }
		INLINE Vector2& operator /= (Vector2 v) { *this = *this / v; return *this; }

		INLINE void operator*= (float   v2) { *this = *this * Scalar(v2); }
		INLINE void operator/= (float   v2) { *this = *this / Scalar(v2); }

		INLINE friend Vector2 operator* (Scalar  v1, Vector2 v2) { return Vector2(v1) * v2; }
		INLINE friend Vector2 operator/ (Scalar  v1, Vector2 v2) { return Vector2(v1) / v2; }
		INLINE friend Vector2 operator* (float   v1, Vector2 v2) { return Scalar(v1) * v2; }
		INLINE friend Vector2 operator/ (float   v1, Vector2 v2) { return Scalar(v1) / v2; }

		INLINE bool operator == (const Vector2& V) const noexcept { DirectX::XMVector2Equal(*this, V); };
		INLINE bool operator != (const Vector2& V) const noexcept { DirectX::XMVector2NotEqual(*this, V); };

		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/
		INLINE Scalar GetX() const { return Scalar(DirectX::XMVectorSplatX(_vector)); }
		INLINE Scalar GetY() const { return Scalar(DirectX::XMVectorSplatY(_vector)); }
		INLINE void SetX(Scalar x) { _vector = DirectX::XMVectorPermute<4, 1, 2, 3>(_vector, x); }
		INLINE void SetY(Scalar y) { _vector = DirectX::XMVectorPermute<0, 5, 2, 3>(_vector, y); }
		INLINE Float2 ToFloat2() { 
			Float2 value;
			DirectX::XMStoreFloat2(&value, _vector);
			return value;
		}
		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		INLINE Vector2() {}
		INLINE Vector2(float x, float y)                { _vector = DirectX::XMVectorSet(x, y, 0, 0); }
		INLINE Vector2(const DirectX::XMFLOAT2& v)      { _vector = XMLoadFloat2(&v); }
		INLINE Vector2(const Float2& v)                 { _vector = XMLoadFloat2(&v); }
		INLINE Vector2(const Vector2& v)                { _vector = v; }
		INLINE Vector2(const Scalar& s)                 { _vector = s; }
		INLINE explicit Vector2(DirectX::FXMVECTOR vec) { _vector = vec; }
		INLINE explicit Vector2(EZeroTag)               { _vector = vector::SplatZero(); }
		INLINE explicit Vector2(EIdentityTag)           { _vector = vector::SplatOne(); }
		INLINE explicit Vector2(EXUnitVector)           { _vector = vector::CreateXUnitVector(); }
		INLINE explicit Vector2(EYUnitVector)           { _vector = vector::CreateYUnitVector(); }
		INLINE operator DirectX::XMVECTOR() const { return _vector; }

	protected:
		/****************************************************************************
		**                Protected Function
		*****************************************************************************/

		/****************************************************************************
		**                Protected Member Variables
		*****************************************************************************/
		DirectX::XMVECTOR _vector;
	};


	/****************************************************************************
	*				  			Vector3
	*************************************************************************//**
	*  @class     Vector3
	*  @brief     Vector3 class for calculations (note! : 16 bytes (for 16 alignment))
	*****************************************************************************/
	struct Vector3
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/

		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/
		INLINE operator DirectX::XMVECTOR() const { return _vector; }
		INLINE Scalar GetX() const { return Scalar(DirectX::XMVectorSplatX(_vector)); }
		INLINE Scalar GetY() const { return Scalar(DirectX::XMVectorSplatY(_vector)); }
		INLINE Scalar GetZ() const { return Scalar(DirectX::XMVectorSplatZ(_vector)); }
		INLINE void SetX(Scalar x) { _vector = DirectX::XMVectorPermute<4, 1, 2, 3>(_vector, x); }
		INLINE void SetY(Scalar y) { _vector = DirectX::XMVectorPermute<0, 5, 2, 3>(_vector, y); }
		INLINE void SetZ(Scalar z) { _vector = DirectX::XMVectorPermute<0, 1, 6, 3>(_vector, z); }
		INLINE Float3 ToFloat3()
		{
			Float3 value;
			DirectX::XMStoreFloat3(&value, _vector);
			return value;
		}

		INLINE Vector3 operator- () const { return Vector3(DirectX::XMVectorNegate(_vector)); }
		INLINE Vector3 operator+ (Vector3 v2) const { return Vector3(DirectX::XMVectorAdd(_vector, v2)); }
		INLINE Vector3 operator- (Vector3 v2) const { return Vector3(DirectX::XMVectorSubtract(_vector, v2)); }
		INLINE Vector3 operator* (Vector3 v2) const { return Vector3(DirectX::XMVectorMultiply(_vector, v2)); }
		INLINE Vector3 operator/ (Vector3 v2) const { return Vector3(DirectX::XMVectorDivide(_vector, v2)); }
		INLINE Vector3 operator* (Scalar  v2) const { return *this * Vector3(v2); }
		INLINE Vector3 operator/ (Scalar  v2) const { return *this / Vector3(v2); }
		INLINE Vector3 operator* (float   v2) const { return *this * Scalar(v2); }
		INLINE Vector3 operator/ (float   v2) const { return *this / Scalar(v2); }

		INLINE Vector3& operator += (Vector3 v) { *this = *this + v; return *this; }
		INLINE Vector3& operator -= (Vector3 v) { *this = *this - v; return *this; }
		INLINE Vector3& operator *= (Vector3 v) { *this = *this * v; return *this; }
		INLINE Vector3& operator /= (Vector3 v) { *this = *this / v; return *this; }

		INLINE friend Vector3 operator* (Scalar  v1, Vector3 v2) { return Vector3(v1) * v2; }
		INLINE friend Vector3 operator/ (Scalar  v1, Vector3 v2) { return Vector3(v1) / v2; }
		INLINE friend Vector3 operator* (float   v1, Vector3 v2) { return Scalar(v1) * v2; }
		INLINE friend Vector3 operator/ (float   v1, Vector3 v2) { return Scalar(v1) / v2; }

		INLINE bool operator == (const Vector3& V) const noexcept { DirectX::XMVector3Equal(*this, V); };
		INLINE bool operator != (const Vector3& V) const noexcept { DirectX::XMVector3NotEqual(*this, V); };

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		INLINE Vector3()                           { _vector = vector::SplatZero(); }
		INLINE Vector3(float x, float y, float z)  { _vector = DirectX::XMVectorSet(x, y, z, z); }
		INLINE Vector3(const DirectX::XMFLOAT3& v) { _vector = DirectX::XMLoadFloat3(&v); }
		INLINE Vector3(const Float3& v)            { _vector = XMLoadFloat3(&v); }
		INLINE Vector3(const Vector3& vector)      { _vector = vector; }
		INLINE Vector3(Scalar scalar)              { _vector = scalar; }
		INLINE explicit Vector3(Vector4 vec);
		INLINE explicit Vector3(DirectX::FXMVECTOR vector) { _vector = vector; }
		INLINE explicit Vector3(EZeroTag)     { _vector = vector::SplatZero(); }
		INLINE explicit Vector3(EIdentityTag) { _vector = vector::SplatOne(); }
		INLINE explicit Vector3(EXUnitVector) { _vector = vector::CreateXUnitVector(); }
		INLINE explicit Vector3(EYUnitVector) { _vector = vector::CreateYUnitVector(); }
		INLINE explicit Vector3(EZUnitVector) { _vector = vector::CreateZUnitVector(); }


	protected:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/
		

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
		DirectX::XMVECTOR _vector;
	};


	/****************************************************************************
	*				  			Vector4
	*************************************************************************//**
	*  @class     Vector4
	*  @brief     Vector4 class for calculations (note! : 16 bytes (for 16 alignment))
	*****************************************************************************/
	struct Vector4
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		INLINE Vector4 operator- ()           const { return Vector4(DirectX::XMVectorNegate  (_vector)); }
		INLINE Vector4 operator+ (Vector4 v2) const { return Vector4(DirectX::XMVectorAdd     (_vector, v2)); }
		INLINE Vector4 operator- (Vector4 v2) const { return Vector4(DirectX::XMVectorSubtract(_vector, v2)); }
		INLINE Vector4 operator* (Vector4 v2) const { return Vector4(DirectX::XMVectorMultiply(_vector, v2)); }
		INLINE Vector4 operator/ (Vector4 v2) const { return Vector4(DirectX::XMVectorDivide  (_vector, v2)); }
		INLINE Vector4 operator* (Scalar  v2) const { return *this * Vector4(v2); }
		INLINE Vector4 operator/ (Scalar  v2) const { return *this / Vector4(v2); }
		INLINE Vector4 operator* (float   v2) const { return *this * Scalar(v2); }
		INLINE Vector4 operator/ (float   v2) const { return *this / Scalar(v2); }

		INLINE void operator*= (float   v2) { *this = *this * Scalar(v2); }
		INLINE void operator/= (float   v2) { *this = *this / Scalar(v2); }

		INLINE friend Vector4 operator* (Scalar  v1, Vector4 v2) { return Vector4(v1) * v2; }
		INLINE friend Vector4 operator/ (Scalar  v1, Vector4 v2) { return Vector4(v1) / v2; }
		INLINE friend Vector4 operator* (float   v1, Vector4 v2) { return Scalar(v1) * v2; }
		INLINE friend Vector4 operator/ (float   v1, Vector4 v2) { return Scalar(v1) / v2; }

		INLINE bool operator == (const Vector4& V) const noexcept { DirectX::XMVector4Equal(*this, V); };
		INLINE bool operator != (const Vector4& V) const noexcept { DirectX::XMVector4NotEqual(*this, V); };
		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/
		INLINE Scalar GetX() const { return Scalar(DirectX::XMVectorSplatX(_vector)); }
		INLINE Scalar GetY() const { return Scalar(DirectX::XMVectorSplatY(_vector)); }
		INLINE Scalar GetZ() const { return Scalar(DirectX::XMVectorSplatZ(_vector)); }
		INLINE Scalar GetW() const { return Scalar(DirectX::XMVectorSplatW(_vector)); }
		INLINE void SetX(Scalar x) { _vector = DirectX::XMVectorPermute<4, 1, 2, 3>(_vector, x); }
		INLINE void SetY(Scalar y) { _vector = DirectX::XMVectorPermute<0, 5, 2, 3>(_vector, y); }
		INLINE void SetZ(Scalar z) { _vector = DirectX::XMVectorPermute<0, 1, 6, 3>(_vector, z); }
		INLINE void SetW(Scalar w) { _vector = DirectX::XMVectorPermute<0, 1, 2, 7>(_vector, w); }
		INLINE void SetXYZ(Vector3 xyz) { _vector = DirectX::XMVectorPermute<0, 1, 2, 7>(xyz, _vector); }
		INLINE DirectX::XMVECTOR* VectorPtr() { return &_vector; }
		INLINE Float4 ToFloat4()
		{
			Float4 value;
			DirectX::XMStoreFloat4(&value, _vector);
			return value;
		}

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		INLINE Vector4()                                   { _vector = vector::SplatZero(); }
		INLINE Vector4(float x, float y, float z, float w) { _vector = DirectX::XMVectorSet(x, y, z, w); }
		INLINE Vector4(const DirectX::XMFLOAT4& v)         { _vector = XMLoadFloat4(&v); }
		INLINE Vector4(Vector3 xyz, float w)               { _vector = DirectX::XMVectorSetW(xyz, w); }
		INLINE Vector4(const Float4& v)                    { _vector = XMLoadFloat4(&v); }
		INLINE Vector4(const Vector4& v)                   { _vector = v; }
		INLINE Vector4(const Scalar& s)                    { _vector = s; }
		INLINE explicit Vector4(Vector3 xyz)               { _vector = vector::SetWToOne(xyz); }
		INLINE explicit Vector4(DirectX::FXMVECTOR vec)    { _vector = vec; }
		INLINE explicit Vector4(EZeroTag)                  { _vector = vector::SplatZero(); }
		INLINE explicit Vector4(EIdentityTag)              { _vector = vector::SplatOne(); }
		INLINE explicit Vector4(EXUnitVector)              { _vector = vector::CreateXUnitVector(); }
		INLINE explicit Vector4(EYUnitVector)              { _vector = vector::CreateYUnitVector(); }
		INLINE explicit Vector4(EZUnitVector)              { _vector = vector::CreateZUnitVector(); }
		INLINE explicit Vector4(EWUnitVector)              { _vector = vector::CreateWUnitVector(); }
		INLINE operator DirectX::XMVECTOR() const { return _vector; }

	protected:
		/****************************************************************************
		**                Protected Function
		*****************************************************************************/

		/****************************************************************************
		**                Protected Member Variables
		*****************************************************************************/
		DirectX::XMVECTOR _vector;
	};

	INLINE Vector3::Vector3(Vector4 vec) : _vector((DirectX::XMVECTOR)vec)
	{
	}

	// For W != 1, divide XYZ by W.  If W == 0, do nothing
	INLINE Vector3 MakeHomogeneous(Vector4 v)
	{
		Scalar W = v.GetW();
		return Vector3(DirectX::XMVectorSelect(DirectX::XMVectorDivide(v, W), v, DirectX::XMVectorEqual(W, vector::SplatZero())));
	}

	CREATE_SIMD_FUNCTIONS(Vector2);
	CREATE_SIMD_FUNCTIONS(Vector3);
	CREATE_SIMD_FUNCTIONS(Vector4);

	INLINE Vector2 ReplicateVector2(float value) { return Vector2(DirectX::XMVectorReplicate(value)); }
	INLINE Vector3 ReplicateVector3(float value) { return Vector3(DirectX::XMVectorReplicate(value)); }
	INLINE Vector4 ReplicateVector4(float value) { return Vector4(DirectX::XMVectorReplicate(value)); }

	INLINE Scalar Norm(const Vector2& v)                                          { return Scalar(DirectX::XMVector2Length(v)); }
	INLINE Scalar Norm(const Vector3& v)                                          { return Scalar(DirectX::XMVector3Length(v)); }
	INLINE Scalar Norm(const Vector4& v)                                          { return Scalar(DirectX::XMVector4Length(v)); }

	INLINE Scalar NormSquared(const Vector2& v)                                   { return Scalar(DirectX::XMVector2LengthSq(v)); }
	INLINE Scalar NormSquared(const Vector3& v)                                   { return Scalar(DirectX::XMVector3LengthSq(v)); }
	INLINE Scalar NormSquared(const Vector4& v)                                   { return Scalar(DirectX::XMVector4LengthSq(v)); }

	INLINE Scalar NormRecip(const Vector2& v)                                     { return Scalar(DirectX::XMVector2ReciprocalLength(v)); }
	INLINE Scalar NormRecip(const Vector3& v)                                     { return Scalar(DirectX::XMVector3ReciprocalLength(v)); }
	INLINE Scalar NormRecip(const Vector4& v)                                     { return Scalar(DirectX::XMVector4ReciprocalLength(v)); }

	INLINE Vector2 ClampNorm(const Vector2& v, float minNorm, float maxNorm)      { return Vector2(DirectX::XMVector2ClampLength(v, minNorm, maxNorm)); }
	INLINE Vector3 ClampNorm(const Vector3& v, float minNorm, float maxNorm)      { return Vector3(DirectX::XMVector3ClampLength(v, minNorm, maxNorm)); }
	INLINE Vector4 ClampNorm(const Vector4& v, float minNorm, float maxNorm)      { return Vector4(DirectX::XMVector4ClampLength(v, minNorm, maxNorm)); }

	INLINE Scalar Dot(const Vector2& v1, const Vector2& v2)                       { return Scalar(DirectX::XMVector2Dot(v1, v2)); }
	INLINE Scalar Dot(const Vector3& v1, const Vector3& v2)                       { return Scalar(DirectX::XMVector3Dot(v1, v2)); }
	INLINE Scalar Dot(const Vector4& v1, const Vector4& v2)                       { return Scalar(DirectX::XMVector4Dot(v1, v2)); }

	INLINE Vector2 Cross(const Vector2& v1, const Vector2& v2)                    { return Vector2(DirectX::XMVector2Cross(v1, v2)); }
	INLINE Vector3 Cross(const Vector3& v1, const Vector3& v2)                    { return Vector3(DirectX::XMVector3Cross(v1, v2)); }
	INLINE Vector4 Cross(const Vector4& v1, const Vector4& v2, const Vector4& v3) { return Vector4(DirectX::XMVector4Cross(v1, v2, v3)); }

	INLINE Vector2 Normalize(const Vector2& v)                                    { return Vector2(DirectX::XMVector2Normalize(v)); }
	INLINE Vector3 Normalize(const Vector3& v)                                    { return Vector3(DirectX::XMVector3Normalize(v)); }
	INLINE Vector4 Normalize(const Vector4& v)                                    { return Vector4(DirectX::XMVector4Normalize(v)); }
	
	INLINE Scalar Distance(const Vector2& v1, const Vector2& v2)                  { return Scalar(Norm(v1 - v2)); }
	INLINE Scalar Distance(const Vector3& v1, const Vector3& v2)                  { return Scalar(Norm(v1 - v2)); }
	INLINE Scalar Distance(const Vector4& v1, const Vector4& v2)                  { return Scalar(Norm(v1 - v2)); }

	INLINE Scalar DistanceSquared(const Vector2& v1, const Vector2& v2)           { return Scalar(NormSquared(v1 - v2)); }
	INLINE Scalar DistanceSquared(const Vector3& v1, const Vector3& v2)           { return Scalar(NormSquared(v1 - v2)); }
	INLINE Scalar DistanceSquared(const Vector4& v1, const Vector4& v2)           { return Scalar(NormSquared(v1 - v2)); }

	INLINE Vector2 Reflect(const Vector2& incident, const Vector2& normal)        { return Vector2(DirectX::XMVector2Reflect(incident, normal)); }
	INLINE Vector3 Reflect(const Vector3& incident, const Vector3& normal)        { return Vector3(DirectX::XMVector3Reflect(incident, normal)); }
	INLINE Vector4 Reflect(const Vector4& incident, const Vector4& normal)        { return Vector4(DirectX::XMVector4Reflect(incident, normal)); }

	INLINE Vector2 Refract(const Vector2& incident, const Vector2& normal, float refractRatio)                     { return Vector2(DirectX::XMVector2Refract(incident, normal, refractRatio)); }
	INLINE Vector3 Refract(const Vector3& incident, const Vector3& normal, float refractRatio)                     { return Vector3(DirectX::XMVector3Refract(incident, normal, refractRatio)); }
	INLINE Vector4 Refract(const Vector4& incident, const Vector4& normal, float refractRatio)                     { return Vector4(DirectX::XMVector4Refract(incident, normal, refractRatio)); }

	INLINE Vector2 Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t)    { return Vector2(DirectX::XMVectorHermite(v1, t1, v2, t2, t)); }
	INLINE Vector3 Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t)    { return Vector3(DirectX::XMVectorHermite(v1, t1, v2, t2, t)); }
	INLINE Vector4 Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t)    { return Vector4(DirectX::XMVectorHermite(v1, t1, v2, t2, t)); }

	INLINE Vector2 CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t) { return Vector2(DirectX::XMVectorCatmullRom(v1, v2, v3, v4, t));}
	INLINE Vector3 CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t) { return Vector3(DirectX::XMVectorCatmullRom(v1, v2, v3, v4, t)); }
	INLINE Vector4 CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t) { return Vector4(DirectX::XMVectorCatmullRom(v1, v2, v3, v4, t)); }
	
	INLINE Vector2 BaryCentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float wf, float wg)        { return Vector2(DirectX::XMVectorBaryCentric(v1, v2, v3, wf, wg)); }
	INLINE Vector3 BaryCentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float wf, float wg)        { return Vector3(DirectX::XMVectorBaryCentric(v1, v2, v3, wf, wg)); }
	INLINE Vector4 BaryCentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float wf, float wg)        { return Vector4(DirectX::XMVectorBaryCentric(v1, v2, v3, wf, wg)); }
	
	INLINE Vector2 AngleBetweenNormalizeVector(const Vector2& v1, const Vector2& v2) { return Vector2(DirectX::XMVector2AngleBetweenNormals(v1, v2)); }
	INLINE Vector3 AngleBetweenNormalizeVector(const Vector3& v1, const Vector3& v2) { return Vector3(DirectX::XMVector3AngleBetweenNormals(v1, v2)); }
	INLINE Vector2 AngleBetweenNormalizeVectorEstimation(const Vector2& v1, const Vector2& v2) { return Vector2(DirectX::XMVector2AngleBetweenNormalsEst(v1, v2)); }
	INLINE Vector3 AngleBetweenNormalizeVectorEstimation(const Vector3& v1, const Vector3& v2) { return Vector3(DirectX::XMVector3AngleBetweenNormalsEst(v1, v2)); }
	INLINE Vector2 AngleBetweenVectors(const Vector2& v1, const Vector2& v2) { return Vector2(DirectX::XMVector2AngleBetweenVectors(v1, v2)); }
	INLINE Vector3 AngleBetweenVectors(const Vector3& v1, const Vector3& v2) { return Vector3(DirectX::XMVector3AngleBetweenVectors(v1, v2)); }

	INLINE bool InBounds(const Vector2& v, const Vector2& bounds) { return DirectX::XMVector2InBounds(v, bounds); }
	INLINE bool InBounds(const Vector3& v, const Vector3& bounds) { return DirectX::XMVector3InBounds(v, bounds); }
	INLINE bool InBounds(const Vector4& v, const Vector4& bounds) { return DirectX::XMVector4InBounds(v, bounds); }

	INLINE bool IsInfinite(const Vector2& v) { return DirectX::XMVector2IsInfinite(v); }
	INLINE bool IsInfinite(const Vector3& v) { return DirectX::XMVector3IsInfinite(v); }
	INLINE bool IsInfinite(const Vector4& v) { return DirectX::XMVector4IsInfinite(v); }

	INLINE bool IsNAN(const Vector2& v) { return DirectX::XMVector2IsNaN(v); }
	INLINE bool IsNAN(const Vector3& v) { return DirectX::XMVector3IsNaN(v); }
	INLINE bool IsNAN(const Vector4& v) { return DirectX::XMVector4IsNaN(v); }

	INLINE Vector2 LinePointDistance(const Vector2& linePoint1, const Vector2& linePoint2, const Vector2& point) { return Vector2(DirectX::XMVector2LinePointDistance(linePoint1, linePoint2, point)); }
	INLINE Vector3 LinePointDistance(const Vector3& linePoint1, const Vector3& linePoint2, const Vector3& point) { return Vector3(DirectX::XMVector3LinePointDistance(linePoint1, linePoint2, point)); }

	INLINE Vector2 IntersectLine(const Vector2& line1Point1, const Vector2& line1Point2, const Vector2& line2Point1, const Vector2& line2Point2) { return Vector2(DirectX::XMVector2IntersectLine(line1Point1, line1Point2, line2Point1, line2Point2)); }
	
}
#endif
