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
#ifndef GM_POOL_ALLOCATOR_HPP
#define GM_POOL_ALLOCATOR_HPP


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GMAlighedAllocator.hpp"
#include <mutex>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
namespace gm
{
	/****************************************************************************
	*				  			 ICollisionAlgorithm
	*************************************************************************//**
	*  @class     Dispatcher
	*  @brief    CollisionAlgorithm is an collision interface that is compatible
	*            with the Broadphase and btDispatcher. It is persistent over frames
	*****************************************************************************/
	class PoolAllocator
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/


		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/
		int GetFreeCount() const { return _freeCount; }
		int GetUsedCount() const { return _maxElementCount - _freeCount; }
		int GetMaxCount() const { return _maxElementCount; }
		int GetElementSize() const { return _elementSize; }
		unsigned char* GetPoolAddress() { return _pool; }
		const unsigned char* GetPoolAddress() const { return _pool; }
		void* Allocate(int size)
		{
			(void)size;
			_mutex.lock();
			assert(!size || size <= _elementSize);
			void* result = _firstFree;
			if (NULL != _firstFree)
			{
				_firstFree = *(void**)_firstFree;
				--_freeCount;
			}
			_mutex.unlock();
			return result;
		}

		bool ValidPtr(void* ptr)
		{
			if (ptr)
			{
				if (((unsigned char*)ptr >= _pool && (unsigned char*)ptr < _pool + _maxElementCount * _elementSize))
				{
					return true;
				}
			}
			return false;
		}

		void FreeMemory(void* ptr)
		{
			if (ptr)
			{
				assert((unsigned char*)ptr >= _pool && (unsigned char*)ptr < _pool + _maxElementCount * _elementSize);

				_mutex.lock();
				*(void**)ptr = _firstFree;
				_firstFree = ptr;
				++_freeCount;
				_mutex.unlock();
			}
		}
		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		PoolAllocator(int elementSize, int maxElementCount)
		{
			_elementSize    = elementSize;
			_maxElementCount = maxElementCount;
			_pool = (unsigned char*)gm::AlignedAllocInternal(static_cast<unsigned int>(_elementSize * _maxElementCount), 16);
			unsigned char* p = _pool;
			_firstFree = p;
			_freeCount = _maxElementCount;
			int count = _maxElementCount;
			while (--count)
			{
				*(void**)p = (p + _elementSize);
				p += _elementSize;
			}
			*(void**)p = 0;
		}
		~PoolAllocator() { gm::AlignedFreeInternal(_pool); }
	private:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
		int   _elementSize;
		int   _maxElementCount;
		int   _freeCount;
		void* _firstFree;
		unsigned char* _pool;
		std::mutex _mutex;
	};
}
#endif