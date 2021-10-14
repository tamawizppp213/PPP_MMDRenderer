//////////////////////////////////////////////////////////////////////////////////
///             @file   GMAlignedAllocator.hpp
///             @brief  Aligned Allocator
///             @author Bullet likeなアロケータになっているので, 念のため記述
///             @date   
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#pragma once
#ifndef GM_ALIGNED_ALLOCATOR_HPP
#define GM_ALIGNED_ALLOCATOR_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMVectorUtility.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
namespace gm
{
	using AlignedAllocatorFunction = void* (size_t size, int alignment);
	using AlignedFreeFunction      = void(void* memoryBlock);
	using AllocatorFunction        = void* (size_t size);
	using FreeFunction             = void(void* memoryBlock);

	void* AlignedAllocInternal(size_t size, int alignment);
	void  AlignedFreeInternal(void* ptr);
	void AlignedAllocSetCustom(AllocatorFunction* AllocatorFunction, FreeFunction* freeFunction);
	void AlignedAllocSetCustomAligned(AlignedAllocatorFunction* allocatorFunction, FreeFunction* freeFunction);

	template <typename T, unsigned int Alignment>
	class AlignedAllocator
	{
		using SelfType = AlignedAllocator<T, Alignment>;
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		
		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		AlignedAllocator() {}

		template <typename Other>
		AlignedAllocator(const AlignedAllocator<Other, Alignment>&)
		{

		}

		T*       Address(T& ref) const { return &ref; }
		const T* Address(const T& ref) const { return &ref; }
		T*       Allocate(int sizeType, const T** hint = 0)
		{
			(void)hint;
			return reinterpret_cast<T*>(AlignedAllocInternal(sizeof(T) * sizeType, Alignment));
		}
		void Construct(T* ptr, const T& value) { new(ptr) T(value); }
		void DeAllocate(T* ptr) { AlignedFreeInternal(reinterpret_cast<void*>(ptr)); }
		void Destroy(T* ptr) { ptr->~T(); }

		template <typename O>
		struct Rebind
		{
			using other = AlignedAllocator<O, Alignment>;
		};
		template <typename O>
		SelfType& operator=(const AlignedAllocator<O, Alignment>&) { return *this; }

		friend bool operator==(const SelfType&, const SelfType&) { return true; }

	private:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
	};
}

#define ALIGNED_ALLOC(a,b) gm::AlignedAllocInternal(a, b)
#define ALIGNED_FREE(ptr) gm::AlignedFreeInternal(ptr)

#define DECLARE_ALIGNED_ALLOCATOR                                                                     \
	INLINE void *operator new(size_t sizeInBytes) { return ALIGNED_ALLOC(sizeInBytes, 16); }   \
	INLINE void operator delete(void *ptr) {ALIGNED_FREE(ptr); }                              \
	INLINE void *operator new(size_t, void *ptr) { return ptr; }                                \
	INLINE void operator delete(void *, void *) {}                                              \
	INLINE void *operator new[](size_t sizeInBytes) { return ALIGNED_ALLOC(sizeInBytes, 16); } \
	INLINE void operator delete[](void *ptr) { ALIGNED_FREE(ptr); }                            \
	INLINE void *operator new[](size_t, void *ptr) { return ptr; }                              \
	INLINE void operator delete[](void *, void *) {}
#endif