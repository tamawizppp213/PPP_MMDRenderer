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
#ifndef GM_ALIGNED_ALLOCATOR_ARRAY_HPP
#define GM_ALIGNED_ALLOCATOR_ARRAY_HPP


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GMAlighedAllocator.hpp"
#include <new>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

// std::vector like 
namespace gm
{
	template <typename T>
	class AlignedObjectArray
	{

	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		class less
		{
		public:
			bool operator()(const T& a, const T& b) const
			{
				return (a < b);
			}
		};

		INLINE AlignedObjectArray<T>& operator=(const AlignedObjectArray<T>& other)
		{
			CopyFromArray(other);
			return *this;
		}

		void CopyFromArray(const AlignedObjectArray& otherArray)
		{
			int otherSize = otherArray.size();
			resize(otherSize);
			otherArray.copy(0, otherSize, _data);
		}
		void InitializeFromBuffer(void* buffer, int size, int capacity)
		{
			clear();
			_ownsMemory = false;
			_data = (T*)buffer;
			_size = size;
			_capacity = capacity;
		}
		INLINE int      size()    const { return _size; }
		INLINE const T& at(int n) const
		{
			assert(n >= 0);
			assert(n < size());
			return _data[n];
		}
		INLINE T& at(int n)
		{
			assert(n >= 0);
			assert(n < size());
			return _data[n];
		}
		INLINE const T& operator[](int n) const
		{
			assert(n >= 0);
			assert(n < size());
			return _data[n];
		}
		INLINE T& operator[](int n)
		{
			assert(n >= 0);
			assert(n < size());
			return _data[n];
		}
		INLINE void clear()
		{
			destroy(0, size());
			deAllocate();
			init();
		}
		INLINE void pop_back()
		{
			assert(_size > 0);
			_size--;
			_data[_size].~T();
		}
		INLINE void resizeNoInitialize(int newSize)
		{
			if (newSize > size())
			{
				reserve(newSize);
			}
			_size = newSize;
		}
		INLINE void resize(int newSize, const T& fillData = T())
		{
			const int curSize = size();

			if (newSize < curSize)
			{

				for (int i = newSize; i < curSize; i++)
				{
					_data[i].~T();
				}
			}
			else
			{
				if (newSize > curSize)
				{
					reserve(newSize);
				}

				for (int i = curSize; i < newSize; i++)
				{
					new (&_data[i]) T(fillData);
				}
			}

			_size = newSize;
		}
		INLINE T& expandNonInitializing()
		{
			const int sz = size();
			if (sz == capacity())
			{
				reserve(allocatorSize(size()));
			}
			_size++;

			return _data[sz];
		}
		INLINE T& expand(const T& fillValue = T())
		{
			const int sz = size();
			if (sz == capacity())
			{
				reserve(allocatorSize(size()));
			}
			_size++;
			new (&_data[sz]) T(fillValue);  //use the in-place new (not really allocating heap memory)

			return _data[sz];
		}
		INLINE void push_back(const T& value)
		{
			const int sz = size();
			if (sz == capacity())
			{
				reserve(allocatorSize(size()));
			}

			new (&_data[_size]) T(value);
			_size++;
		}
		INLINE int  capacity() const { return _capacity; }
		INLINE void reserve(int count)
		{
			if (capacity() < count)
			{  // not enough room, reallocate
				T* s = (T*)allocate(count);

				copy(0, size(), s);

				destroy(0, size());

				deAllocate();

				//PCK: added this line
				_ownsMemory = true;

				_data = s;

				_capacity = count;
			}
		}
		void removeAtIndex(int index)
		{
			if (index < size())
			{
				swap(index, size() - 1);
				pop_back();
			}
		}
		void remove(const T& key)
		{
			int findIndex = findLinearSearch(key);
			removeAtIndex(findIndex);
		}
		void swap(int index0, int index1)
		{
			T temp = _data[index0];
			_data[index0] = _data[index1];
			_data[index1] = temp;
		}

		template <typename L>
		void quickSortInternal(const L& CompareFunc, int lo, int hi)
		{
			//  lo is the lower index, hi is the upper index
			//  of the region of array a that is to be sorted
			int i = lo, j = hi;
			T x = _data[(lo + hi) / 2];

			//  partition
			do
			{
				while (CompareFunc(_data[i], x))
					i++;
				while (CompareFunc(x, _data[j]))
					j--;
				if (i <= j)
				{
					swap(i, j);
					i++;
					j--;
				}
			} while (i <= j);

			//  recursion
			if (lo < j)
				quickSortInternal(CompareFunc, lo, j);
			if (i < hi)
				quickSortInternal(CompareFunc, i, hi);
		}

		template <typename L>
		void quickSort(const L& CompareFunc)
		{
			//don't sort 0 or 1 elements
			if (size() > 1)
			{
				quickSortInternal(CompareFunc, 0, size() - 1);
			}
		}

		///heap sort from http://www.csse.monash.edu.au/~lloyd/tildeAlgDS/Sort/Heap/
		template <typename L>
		void downHeap(T* pArr, int k, int n, const L& CompareFunc)
		{
			/*  PRE: a[k+1..N] is a heap */
			/* POST:  a[k..N]  is a heap */

			T temp = pArr[k - 1];
			/* k has child(s) */
			while (k <= n / 2)
			{
				int child = 2 * k;

				if ((child < n) && CompareFunc(pArr[child - 1], pArr[child]))
				{
					child++;
				}
				/* pick larger child */
				if (CompareFunc(temp, pArr[child - 1]))
				{
					/* move child up */
					pArr[k - 1] = pArr[child - 1];
					k = child;
				}
				else
				{
					break;
				}
			}
			pArr[k - 1] = temp;
		} /*downHeap*/

		template <typename L>
		void heapSort(const L& CompareFunc)
		{
			/* sort a[0..N-1],  N.B. 0 to N-1 */
			int k;
			int n = _size;
			for (k = n / 2; k > 0; k--)
			{
				downHeap(_data, k, n, CompareFunc);
			}

			/* a[1..N] is now a heap */
			while (n >= 1)
			{
				swap(0, n - 1); /* largest of a[0..n-1] */

				n = n - 1;
				/* restore a[1..i-1] heap */
				downHeap(_data, 1, n, CompareFunc);
			}
		}

		///non-recursive binary search, assumes sorted array
		int findBinarySearch(const T& key) const
		{
			int first = 0;
			int last = size() - 1;

			//assume sorted array
			while (first <= last)
			{
				int mid = (first + last) / 2;  // compute mid point.
				if (key > _data[mid])
					first = mid + 1;  // repeat search in top half.
				else if (key < _data[mid])
					last = mid - 1;  // repeat search in bottom half.
				else
					return mid;  // found it. return position /////
			}
			return size();  // failed to find key
		}

		int findLinearSearch(const T& key) const
		{
			int index = size();
			int i;

			for (i = 0; i < size(); i++)
			{
				if (_data[i] == key)
				{
					index = i;
					break;
				}
			}
			return index;
		}

		// If the key is not in the array, return -1 instead of 0,
		// since 0 also means the first element in the array.
		int findLinearSearch2(const T& key) const
		{
			int index = -1;
			int i;

			for (i = 0; i < size(); i++)
			{
				if (_data[i] == key)
				{
					index = i;
					break;
				}
			}
			return index;
		}


		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		AlignedObjectArray() { init(); }
		~AlignedObjectArray() { clear(); }
		AlignedObjectArray(const AlignedObjectArray& otherArray)
		{
			init();
			int otherSize = otherArray.size();
			resize(otherSize);
			otherArray.copy(0, otherSize, _data);
		}

	protected:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		INLINE int   allocatorSize(int size) { return (size ? size * 2 : 1); }
		INLINE void  copy(int start, int end, T* dest) const
		{
			int i;
			for (i = start; i < end; ++i) { new (&dest[i]) T(_data[i]); }
		}
		INLINE void  init()
		{
			_ownsMemory = true;
			_data = nullptr;
			_size = 0;
			_capacity = 0;
		}
		INLINE void  destroy(int first, int last)
		{
			int i;
			for (i = first; i < last; ++i)
			{
				_data[i].~T();
			}
		}
		INLINE void* allocate(int size)
		{
			if (size) { return _allocator.Allocate(size); }
			else      { return nullptr; }
		}
		INLINE void  deAllocate()
		{
			if (_data)
			{
				//PCK: enclosed the deallocation in this block
				if (_ownsMemory)
				{
					_allocator.DeAllocate(_data);
				}
				_data = 0;
			}
		}
	private:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/


		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
		gm::AlignedAllocator<T, 16> _allocator;
		int  _size;
		int  _capacity;
		T*   _data;
		bool _ownsMemory;
	};
}


#endif