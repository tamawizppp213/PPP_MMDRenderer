//////////////////////////////////////////////////////////////////////////////////
///             @file   GMVector.hpp
///             @brief  GMVector 
///             @date   2021_05_09
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

// Partially modified by Toide.
#pragma once
#ifndef GM_VECTOR_UTILITY_HPP
#define GM_VECTOR_UTILITY_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <DirectXMath.h>
#include <intrin.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define INLINE __forceinline
#define ALIGNED16(a) __declspec(align(16)) a
#define ALIGNED64(a) __declspec(align(64)) a
#define ALIGNED128(a) __declspec (align(128)) a


namespace gm
{
    constexpr float GM_HALF_PI = DirectX::XM_PIDIV2;
    constexpr float GM_PI      = DirectX::XM_PI;
    constexpr float GM_2PI     = DirectX::XM_2PI;
    constexpr float GM_RADS_PER_DEG(GM_2PI / 360.0f);
    constexpr float GM_DEGS_PER_RAD(360.0f / GM_2PI);

	namespace utils
	{
		template <typename T> __forceinline T AlignUpWithMask(T value, size_t mask)
		{
			return (T)(((size_t)value + mask) & ~mask);
		}

        template <typename T> __forceinline T AlignDownWithMask(T value, size_t mask)
        {
            return (T)((size_t)value & ~mask);
        }

        template <typename T> __forceinline T AlignUp(T value, size_t alignment)
        {
            return AlignUpWithMask(value, alignment - 1);
        }

        template <typename T> __forceinline T AlignDown(T value, size_t alignment)
        {
            return AlignDownWithMask(value, alignment - 1);
        }

        template <typename T> __forceinline bool IsAligned(T value, size_t alignment)
        {
            return 0 == ((size_t)value & (alignment - 1));
        }

        template <typename T> __forceinline T DivideByMultiple(T value, size_t alignment)
        {
            return (T)((value + alignment - 1) / alignment);
        }

        template <typename T> __forceinline bool IsPowerOfTwo(T value)
        {
            return 0 == (value & (value - 1));
        }

        template <typename T> __forceinline bool IsDivisible(T value, T divisor)
        {
            return (value / divisor) * divisor == value;
        }

        __forceinline uint8_t Log2(uint64_t value)
        {
            unsigned long mssb; // most significant set bit
            unsigned long lssb; // least significant set bit

            // If perfect power of two (only one set bit), return index of bit.  Otherwise round up
            // fractional log by adding 1 to most signicant set bit's index.
            if (_BitScanReverse64(&mssb, value) > 0 && _BitScanForward64(&lssb, value) > 0)
                return uint8_t(mssb + (mssb == lssb ? 0 : 1));
            else
                return 0;
        }

        template <typename T> __forceinline T AlignPowerOfTwo(T value)
        {
            return value == 0 ? 0 : 1 << Log2(value);
        }

        template <typename T>
        T* AlignmentPointer(T* unAlignedPtr, size_t alignment)
        {
            struct ConvertPointerSizeT
            {
                union
                {
                    T* ptr;
                    size_t integer;
                };
            };

            ConvertPointerSizeT converter;

            const size_t bitMask = ~(alignment - 1);
            converter.ptr = unAlignedPtr;
            converter.integer += alignment - 1;
            converter.integer &= bitMask;
            return converter.ptr;
        }

        ///rudimentary class to provide type info
        struct TypedObject
        {
            TypedObject(int objectType) : ObjectType(objectType) { }
            int ObjectType;
            inline int GetObjectType() const { return ObjectType; }
        };
	}

    namespace vector
    {
        using namespace DirectX;
        INLINE XMVECTOR SplatZero()
        {
            return XMVectorZero();
        }

#if !defined(_XM_NO_INTRINSICS_) && defined(_XM_SSE_INTRINSICS_)

        INLINE XMVECTOR SplatOne(XMVECTOR zero = SplatZero())
        {
            __m128i AllBits = _mm_castps_si128(_mm_cmpeq_ps(zero, zero));
            return _mm_castsi128_ps(_mm_slli_epi32(_mm_srli_epi32(AllBits, 25), 23));	// return 0x3F800000
            //return _mm_cvtepi32_ps(_mm_srli_epi32(SetAllBits(zero), 31));				// return (float)1;  (alternate method)
        }

#if defined(_XM_SSE4_INTRINSICS_)
        INLINE XMVECTOR CreateXUnitVector(XMVECTOR one = SplatOne())
        {
            return _mm_insert_ps(one, one, 0x0E);
        }
        INLINE XMVECTOR CreateYUnitVector(XMVECTOR one = SplatOne())
        {
            return _mm_insert_ps(one, one, 0x0D);
        }
        INLINE XMVECTOR CreateZUnitVector(XMVECTOR one = SplatOne())
        {
            return _mm_insert_ps(one, one, 0x0B);
        }
        INLINE XMVECTOR CreateWUnitVector(XMVECTOR one = SplatOne())
        {
            return _mm_insert_ps(one, one, 0x07);
        }
        INLINE XMVECTOR SetWToZero(FXMVECTOR vec)
        {
            return _mm_insert_ps(vec, vec, 0x08);
        }
        INLINE XMVECTOR SetWToOne(FXMVECTOR vec)
        {
            return _mm_blend_ps(vec, SplatOne(), 0x8);
        }
#else
        INLINE XMVECTOR CreateXUnitVector(XMVECTOR one = SplatOne())
        {
            return _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(one), 12));
        }
        INLINE XMVECTOR CreateYUnitVector(XMVECTOR one = SplatOne())
        {
            XMVECTOR unitx = CreateXUnitVector(one);
            return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(unitx), 4));
        }
        INLINE XMVECTOR CreateZUnitVector(XMVECTOR one = SplatOne())
        {
            XMVECTOR unitx = CreateXUnitVector(one);
            return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(unitx), 8));
        }
        INLINE XMVECTOR CreateWUnitVector(XMVECTOR one = SplatOne())
        {
            return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(one), 12));
        }
        INLINE XMVECTOR SetWToZero(FXMVECTOR vec)
        {
            __m128i MaskOffW = _mm_srli_si128(_mm_castps_si128(_mm_cmpeq_ps(vec, vec)), 4);
            return _mm_and_ps(vec, _mm_castsi128_ps(MaskOffW));
        }
        INLINE XMVECTOR SetWToOne(FXMVECTOR vec)
        {
            return _mm_movelh_ps(vec, _mm_unpackhi_ps(vec, SplatOne()));
        }

#endif

#else // !_XM_SSE_INTRINSICS_

        INLINE XMVECTOR SplatOne() { return XMVectorSplatOne(); }
        INLINE XMVECTOR CreateXUnitVector() { return g_XMIdentityR0; }
        INLINE XMVECTOR CreateYUnitVector() { return g_XMIdentityR1; }
        INLINE XMVECTOR CreateZUnitVector() { return g_XMIdentityR2; }
        INLINE XMVECTOR CreateWUnitVector() { return g_XMIdentityR3; }
        INLINE XMVECTOR SetWToZero(FXMVECTOR vec) { return XMVectorAndInt(vec, g_XMMask3); }
        INLINE XMVECTOR SetWToOne(FXMVECTOR vec) { return XMVectorSelect(g_XMIdentityR3, vec, g_XMMask3); }

#endif

    }
    
    enum EZeroTag     { kZero, kOrigin };
    enum EIdentityTag { kOne, kIdentity };
    enum EXUnitVector { kXUnitVector };
    enum EYUnitVector { kYUnitVector };
    enum EZUnitVector { kZUnitVector };
    enum EWUnitVector { kWUnitVector };

    class BoolVector
    {
    public:
        INLINE BoolVector(DirectX::FXMVECTOR vec) { _vector = vec; }
        INLINE operator DirectX::XMVECTOR() const { return _vector; }
    protected:
        DirectX::XMVECTOR _vector;
    };
}

#endif