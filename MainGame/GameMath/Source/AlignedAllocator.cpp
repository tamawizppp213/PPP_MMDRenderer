//////////////////////////////////////////////////////////////////////////////////
///             @file   GMAlignedAllocator.cpp
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

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMAlighedAllocator.hpp"
#include <string>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
static void* AllocatorDefault(size_t size)
{
	char* data = (char*)std::malloc(size);
	if (data == NULL) { return nullptr; }
	std::memset(data, 0, size);
	return data;
}

static void FreeDefault(void* ptr)
{
	std::free(ptr);
}

static AllocatorFunction* g_AllocatorFunction = AllocatorDefault;
static FreeFunction* g_FreeFunction           = FreeDefault;

static inline void* AlignedAllocatorDefault(size_t size, int alignment)
{
	void* result;
	char* real;
	real = (char*)g_AllocatorFunction(size + sizeof(void*) + (alignment - 1));
	if (real)
	{
		result = utils::AlignmentPointer(real + sizeof(void*), alignment);
		*((void**)(result)-1) = (void*)(real);
	}
	else
	{
		result = (void*)(real);
	}
	if (result == NULL) { return nullptr; }
	std::memset((char*)result, 0, size);
	return result;
}

static inline void AlignedFreeDefault(void* ptr)
{
	void* real;
	if (ptr)
	{
		real = *((void**)(ptr)-1);
		g_FreeFunction(real);
	}
}

static AlignedAllocatorFunction* g_AlignedAllocFunc = AlignedAllocatorDefault;
static AlignedFreeFunction*      g_AlignedFreeFunc = AlignedFreeDefault;

void gm::AlignedAllocSetCustomAligned(AlignedAllocatorFunction* allocFunc, AlignedFreeFunction* freeFunc)
{
	g_AlignedAllocFunc = allocFunc ? allocFunc : AlignedAllocatorDefault;
	g_AlignedFreeFunc = freeFunc ? freeFunc : AlignedFreeDefault;
}

void gm::AlignedAllocSetCustom(AllocatorFunction* allocFunc, FreeFunction* freeFunc)
{
	g_AllocatorFunction = allocFunc ? allocFunc : AllocatorDefault;
	g_FreeFunction = freeFunc ? freeFunc : FreeDefault;
}

void* gm::AlignedAllocInternal(size_t size, int alignment)
{
	void* ptr;
	ptr = g_AlignedAllocFunc(size, alignment);
	return ptr;
}

void gm::AlignedFreeInternal(void* ptr)
{
	if (!ptr)
	{
		return;
	}

	//	printf("btAlignedFreeInternal %x\n",ptr);
	g_AlignedFreeFunc(ptr);
}
