//////////////////////////////////////////////////////////////////////////////////
//            @file　　  HashedSimplePairCache.hpp
//            @brief    HashedSimplePairCache.hpp
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_03
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
#ifndef HASHED_SIMPLE_PAIR_CACHE_HPP
#define HASHED_SIMPLE_PAIR_CACHE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
const int SIMPLE_NULL_PAIR = 0xffffffff;
struct BtSimplePair
{
	BtSimplePair(int indexA, int indexB) : IndexA(indexA), IndexB(indexB), UserPointer(0) {};

	int IndexA;
	int IndexB;
	union
	{
		void* UserPointer;
		int UserValue;
	};
};

using BtSimplePairArray = gm::AlignedObjectArray<BtSimplePair>;
//////////////////////////////////////////////////////////////////////////////////
//                        HashedSimplePairCache
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			HashedSimplePairCache
*************************************************************************//**
*  @class     HashedSimplePairCache
*  @brief     temp
*****************************************************************************/
class HashedSimplePairCache
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void          RemoveAllPairs();
	virtual void* RemoveOverlappingPair(int indexA, int indexB);
	virtual BtSimplePair* AddOverlappingPair(int indexA, int indexB) { return InternalAddPair(indexA, indexB); }
	BtSimplePair* FindOverlappingPair(int indexA, int indexB);


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual BtSimplePair* GetOverlappingPairArrayPtr() { return &_overlappingPairArray[0]; }
	const BtSimplePair* GetOverlappingPairArrayPtr() const { return &_overlappingPairArray[0]; }
	BtSimplePairArray& GetOverlappingPairArray() { return _overlappingPairArray; }
	const BtSimplePairArray& GetOverlappingPairArray() const { return _overlappingPairArray; }
	int GetCount() const { return _overlappingPairArray.size(); }
	int GetOverlappingPairs() const { return _overlappingPairArray.size(); }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	HashedSimplePairCache();
	virtual ~HashedSimplePairCache();
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<int> _hashTable;
	gm::AlignedObjectArray<int> _next;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	BtSimplePair* InternalAddPair(int indexA, int indexB);
	void GrowTables();

	INLINE bool EqualsPair(const BtSimplePair& pair, int indexA, int indexB) { return pair.IndexA == indexA && pair.IndexB == indexB; }
	INLINE unsigned int GetHash(unsigned int indexA, unsigned int indexB)
	{
		unsigned int key = indexA | (indexB << 16);
		// Thomas Wang's hash

		key += ~(key << 15);
		key ^= (key >> 10);
		key += (key << 3);
		key ^= (key >> 6);
		key += ~(key << 11);
		key ^= (key >> 16);
		return key;
	}

	INLINE BtSimplePair* InternalFindPair(int proxyIDA, int proxyIDB, int hash)
	{
		int index = _hashTable[hash];

		while (index != SIMPLE_NULL_PAIR && EqualsPair(_overlappingPairArray[index], proxyIDA, proxyIDB) == false)
		{
			index = _next[index];
		}

		if (index == SIMPLE_NULL_PAIR)
		{
			return NULL;
		}

		assert(index < _overlappingPairArray.size());

		return &_overlappingPairArray[index];
	};
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	BtSimplePairArray _overlappingPairArray;
};
#endif