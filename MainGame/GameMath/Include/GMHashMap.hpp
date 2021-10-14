//////////////////////////////////////////////////////////////////////////////////
///             @file   GMHashMap.hpp
///             @brief  Hash Map
///             @author Bullet likeなアロケータになっているので, 念のため記述
///             @date   
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

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
#ifndef GM_HASH_MAP
#define GM_HASH_MAP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "GMAlignedAllocatorArray.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

namespace gm
{
	const int BT_HASH_NULL = 0xffffffff;
	struct HashString
	{
		std::string _string1;
		unsigned int _hash;

		INLINE unsigned int GetHash() const
		{
			return _hash;
		}

		HashString()
		{
			_string1 = "";
			_hash = 0;
		}
		HashString(const char* name)
			: _string1(name)
		{
			/* magic numbers from http://www.isthe.com/chongo/tech/comp/fnv/ */
			static const unsigned int InitialFNV = 2166136261u;
			static const unsigned int FNVMultiple = 16777619u;

			/* Fowler / Noll / Vo (FNV) Hash */
			unsigned int hash = InitialFNV;

			for (int i = 0; _string1.c_str()[i]; i++)
			{
				hash = hash ^ (_string1.c_str()[i]); /* xor  the low 8 bits */
				hash = hash * FNVMultiple;            /* multiply by the magic number */
			}
			_hash = hash;
		}

		bool equals(const HashString& other) const
		{
			return (_string1 == other._string1);
		}
	};

	class HashInt
	{
		int _uid;

	public:
		HashInt()
		{
		}

		HashInt(int uid) : _uid(uid)
		{
		}

		int GetUid1() const
		{
			return _uid;
		}

		void SetUid1(int uid)
		{
			_uid = uid;
		}

		bool equals(const HashInt& other) const
		{
			return GetUid1() == other.GetUid1();
		}
		//to our success
		INLINE unsigned int GetHash() const
		{
			unsigned int key = _uid;
			// Thomas Wang's hash
			key += ~(key << 15);
			key ^= (key >> 10);
			key += (key << 3);
			key ^= (key >> 6);
			key += ~(key << 11);
			key ^= (key >> 16);

			return key;
		}
	};

	class HashPtr
	{
		union
		{
			const void* _pointer;
			unsigned int _hashValues[2];
		};

	public:
		HashPtr(const void* ptr)
			: _pointer(ptr)
		{
		}

		const void* GetPointer() const
		{
			return _pointer;
		}

		bool equals(const HashPtr& other) const
		{
			return GetPointer() == other.GetPointer();
		}

		//to our success
		INLINE unsigned int GetHash() const
		{
			const bool VOID_IS_8 = ((sizeof(void*) == 8));

			unsigned int key = VOID_IS_8 ? _hashValues[0] + _hashValues[1] : _hashValues[0];
			// Thomas Wang's hash
			key += ~(key << 15);
			key ^= (key >> 10);
			key += (key << 3);
			key ^= (key >> 6);
			key += ~(key << 11);
			key ^= (key >> 16);
			return key;
		}
	};

	template <class Value>
	class HashKeyPtr
	{
		int _uid;

	public:
		HashKeyPtr(int uid) : _uid(uid)
		{
		}

		int GetUid1() const
		{
			return _uid;
		}

		bool equals(const HashKeyPtr<Value>& other) const
		{
			return GetUid1() == other.GetUid1();
		}

		//to our success
		INLINE unsigned int GetHash() const
		{
			unsigned int key = _uid;
			// Thomas Wang's hash
			key += ~(key << 15);
			key ^= (key >> 10);
			key += (key << 3);
			key ^= (key >> 6);
			key += ~(key << 11);
			key ^= (key >> 16);
			return key;
		}
	};

	template <class Value>
	class HashKey
	{
		int _uid;

	public:
		HashKey(int uid) : _uid(uid)
		{
		}

		int GetUid1() const
		{
			return _uid;
		}

		bool equals(const HashKey<Value>& other) const
		{
			return GetUid1() == other.GetUid1();
		}
		//to our success
		INLINE unsigned int GetHash() const
		{
			unsigned int key = _uid;
			// Thomas Wang's hash
			key += ~(key << 15);
			key ^= (key >> 10);
			key += (key << 3);
			key ^= (key >> 6);
			key += ~(key << 11);
			key ^= (key >> 16);
			return key;
		}
	};

	template <class Key, class Value>
	class HashMap
	{
	protected:
		/****************************************************************************
		**                Protected Function
		*****************************************************************************/
		void GrowTables(const Key& key)
		{
			int newCapacity = _valueArray.capacity();

			if (_hashTable.size() < newCapacity)
			{
				//grow hashtable and next table
				int curHashtableSize = _hashTable.size();

				_hashTable.resize(newCapacity);
				_next.resize(newCapacity);

				int i;

				for (i = 0; i < newCapacity; ++i)
				{
					_hashTable[i] = BT_HASH_NULL;
				}
				for (i = 0; i < newCapacity; ++i)
				{
					_next[i] = BT_HASH_NULL;
				}

				for (i = 0; i < curHashtableSize; i++)
				{
					int hashValue = _keyArray[i].GetHash() & (_valueArray.capacity() - 1);  // New hash value with new mask
					_next[i] = _hashTable[hashValue];
					_hashTable[hashValue] = i;
				}
			}
		}

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
		gm::AlignedObjectArray<int>   _hashTable;
		gm::AlignedObjectArray<int>   _next;
		gm::AlignedObjectArray<Value> _valueArray;
		gm::AlignedObjectArray<Key>   _keyArray;
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		void   insert(const Key& key, const Value& value)
		{
			int hash = key.GetHash() & (_valueArray.capacity() - 1);

			//replace value if the key is already there
			int index = findIndex(key);
			if (index != BT_HASH_NULL)
			{
				_valueArray[index] = value;
				return;
			}

			int count = _valueArray.size();
			int oldCapacity = _valueArray.capacity();
			_valueArray.push_back(value);
			_keyArray.push_back(key);

			int newCapacity = _valueArray.capacity();
			if (oldCapacity < newCapacity)
			{
				GrowTables(key);
				//hash with new capacity
				hash = key.GetHash() & (_valueArray.capacity() - 1);
			}
			_next[count]     = _hashTable[hash];
			_hashTable[hash] = count;
		}
		void   remove(const Key& key)
		{
			int hash = key.GetHash() & (_valueArray.capacity() - 1);

			int pairIndex = findIndex(key);

			if (pairIndex == BT_HASH_NULL)
			{
				return;
			}

			// Remove the pair from the hash table.
			int index = _hashTable[hash];
			assert(index != BT_HASH_NULL);

			int previous = BT_HASH_NULL;
			while (index != pairIndex)
			{
				previous = index;
				index = _next[index];
			}

			if (previous != BT_HASH_NULL)
			{
				assert(_next[previous] == pairIndex);
				_next[previous] = _next[pairIndex];
			}
			else
			{
				_hashTable[hash] = _next[pairIndex];
			}

			// We now move the last pair into spot of the
			// pair being removed. We need to fix the hash
			// table indices to support the move.

			int lastPairIndex = _valueArray.size() - 1;

			// If the removed pair is the last pair, we are done.
			if (lastPairIndex == pairIndex)
			{
				_valueArray.pop_back();
				_keyArray.pop_back();
				return;
			}

			// Remove the last pair from the hash table.
			int lastHash = _keyArray[lastPairIndex].GetHash() & (_valueArray.capacity() - 1);

			index = _hashTable[lastHash];
			assert(index != BT_HASH_NULL);

			previous = BT_HASH_NULL;
			while (index != lastPairIndex)
			{
				previous = index;
				index    = _next[index];
			}

			if (previous != BT_HASH_NULL)
			{
				btAssert(_next[previous] == lastPairIndex);
				_next[previous] = _next[lastPairIndex];
			}
			else
			{
				_hashTable[lastHash] = _next[lastPairIndex];
			}

			// Copy the last pair into the remove pair's spot.
			_valueArray[pairIndex] = _valueArray[lastPairIndex];
			_keyArray[pairIndex]   = _keyArray[lastPairIndex];

			// Insert the last pair into the hash table
			_next[pairIndex]     = _hashTable[lastHash];
			_hashTable[lastHash] = pairIndex;

			_valueArray.pop_back();
			_keyArray  .pop_back();
		}
		int    size() const { return _valueArray.size(); }
		
		Value* find(const Key& key)
		{
			int index = findIndex(key);
			if (index == BT_HASH_NULL)
			{
				return NULL;
			}
			return &_valueArray[index];
		}
		const Value* find(const Key& key) const
		{
			int index = findIndex(key);
			if (index == BT_HASH_NULL)
			{
				return NULL;
			}
			return &_valueArray[index];
		}
		int          findIndex(const Key& key) const
		{
			unsigned int hash = key.GetHash() & (_valueArray.capacity() - 1);

			if (hash >= (unsigned int)_hashTable.size())
			{
				return BT_HASH_NULL;
			}

			int index = _hashTable[hash];
			while ((index != BT_HASH_NULL) && key.equals(_keyArray[index]) == false)
			{
				index = _next[index];
			}
			return index;
		}

		const Value* getAtIndex(int index) const
		{
			assert(index < _valueArray.size());
			assert(index >= 0);
			if (index >= 0 && index < _valueArray.size())
			{
				return &_valueArray[index];
			}
			return nullptr;
		}
		Value*       getAtIndex(int index)
		{
			assert(index < _valueArray.size());
			assert(index >= 0);
			if (index >= 0 && index < _valueArray.size())
			{
				return &_valueArray[index];
			}
			return nullptr;
		}
		Key          getKeyAtIndex(int index)
		{
			assert(index < _keyArray.size());
			assert(index >= 0);
			return _keyArray[index];
		}
		const Key    getKeyAtIndex(int index) const 
		{
			assert(index < _keyArray.size());
			assert(index >= 0);
			return _keyArray[index];
		}
		
		void clear()
		{
			_hashTable .clear();
			_next      .clear();
			_valueArray.clear();
			_keyArray  .clear();
		}
		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/


		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		Value* operator[](const Key& key) { return find(key); }
		const Value* operator[](const Key& key) const
		{
			return find(key);
		}

	};
}
#endif