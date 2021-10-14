//////////////////////////////////////////////////////////////////////////////////
///             @file   QuantizedBvh.hpp
///             @brief  Quantized bvh
///             @author Toide Yutaro
///             @date   2021_10_07
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
#ifndef QUANTIZED_BVH_HPP
#define QUANTIZED_BVH_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"
#include "GameMath/Include/GMVector.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define MAX_SUBTREE_SIZE_IN_BYTES 2048
#define MAX_NUM_PARTS_IN_BITS 10

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
__declspec(align(16))
struct QuantizedBvhNode
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool IsLeafNode    () const { return EscapeIndexOrTriangleIndex >= 0; }
	int  GetEscapeIndex() const { assert(!IsLeafNode()); return -EscapeIndexOrTriangleIndex; }
	int  GetTriangleIndex() const
	{
		assert(!IsLeafNode());
		unsigned int x = 0; 
		unsigned int y = (~(x & 0)) << (31 - MAX_NUM_PARTS_IN_BITS);
		return (EscapeIndexOrTriangleIndex & ~(y));
	}
	int  GetPartID() const
	{
		assert(IsLeafNode());
		return (EscapeIndexOrTriangleIndex >> (31 - MAX_NUM_PARTS_IN_BITS));
	}

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	unsigned short int QuantizedAABBMin[3];
	unsigned short int QuantizedAABBMax[3];
	int EscapeIndexOrTriangleIndex;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
};

__declspec(align(16))
struct OptimizedBvhNode
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3 AABBMinOrigin;
	gm::Vector3 AABBMaxOrigin;

	int EscapeIndex;
	int SubPart;
	int TriangleIndex;

	char Padding[20];

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
};

__declspec(align(16))
class BvhSubtreeInfo
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	unsigned short int QuantizedAABBMin[3];
	unsigned short int QuantizedAABBMax[3];
	int                RootNodeIndex; // points to the root of the subtree
	int                SubtreeSize;
	int                Padding[3];
	void SetAABBFromQuantizedNode(const QuantizedBvhNode& quantizedNode)
	{
		QuantizedAABBMin[0] = quantizedNode.QuantizedAABBMin[0];
		QuantizedAABBMin[1] = quantizedNode.QuantizedAABBMin[1];
		QuantizedAABBMin[2] = quantizedNode.QuantizedAABBMin[2];
		QuantizedAABBMax[0] = quantizedNode.QuantizedAABBMax[0];
		QuantizedAABBMax[1] = quantizedNode.QuantizedAABBMax[1];
		QuantizedAABBMax[2] = quantizedNode.QuantizedAABBMax[2];
	}

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	BvhSubtreeInfo() {};
};

class NodeOverlapCallback
{
public: 
	virtual ~NodeOverlapCallback() {};
	virtual void ProcessNode(int subPart, int triangleIndex) = 0;
};
using NodeArray           = gm::AlignedObjectArray<OptimizedBvhNode>;
using QuantizedNodeArray  = gm::AlignedObjectArray<QuantizedBvhNode>;
using BvhSubtreeInfoArray = gm::AlignedObjectArray<BvhSubtreeInfo>;

__declspec(align(16))
class QuantizedBvh
{
public:
	enum TraversalMode
	{
		Stackless                = 0, 
		Stackless_Cache_Friendly = 1,
		Recursive                = 2
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void ReportAABBOverlappingNodex(NodeOverlapCallback* nodeCallback, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax) const;
	void ReportRayOverlappingNodex(NodeOverlapCallback* nodeCallback, const gm::Vector3& raySOurce, const gm::Vector3& rayTarget) const;
	void ReportBoxCastOverlappingNodex(NodeOverlapCallback* nodeCallback, const gm::Vector3& raySource, const gm::Vector3& rayTarget, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax) const;

	void Quantize(unsigned short* out, const gm::Vector3& point, int isMax) const;
	void QuantizeWithClamp(unsigned short* out, const gm::Vector3& point2, int isMax) const;
	gm::Vector3 UnQuantize(const unsigned short* vectorIn) const;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	QuantizedNodeArray& GetLeafNodeArray() { return _quantizedLeafNodes; }
	void BuildInternal();
	void SetQuantizationValues(const gm::Vector3& bvhAABBMin, const gm::Vector3& bvhAABBMax, float quantizationMargin = 1.0f);
	void SetTraversalMode(TraversalMode traversalMode) { _traversalMode = traversalMode; }
	INLINE QuantizedNodeArray& GetQuantizedNodeArray() { return _quantizedContiguousNodes; }
	INLINE BvhSubtreeInfoArray& GetSubtreeInfoArray()  { return _subtreeHeaders; }
	INLINE bool IsQuantized() { return _useQuantization; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	QuantizedBvh();
	virtual ~QuantizedBvh();
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	void SwapLeafNodes(int firstIndex, int secondIndex);
	void AssignInternalNodeFromLeafNode(int internalNode, int leafNodeIndex);
	void BuildTree(int startIndex, int endIndex);
	int  CalculateSplittingAxis(int startIndex, int endIndex);
	int  SortAndCalculateSplittingIndex(int startIndex, int endIndex, int splitAxis);
	void WalkStacklessTree(NodeOverlapCallback* nodeCallback, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax) const;
	void WalkStacklessQuantizedTreeAgainstRay(NodeOverlapCallback* nodeCallback, const gm::Vector3& raySource, const gm::Vector3& rayTarget, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, int startNodeIndex, int endNodeIndex) const;
	void WalkStacklessQuantizedTree(NodeOverlapCallback* nodeCallback, unsigned short int* quantizedQueryAABBMin, unsigned short int* quantizedQueryAABBMax, int startNodeIndex, int endNodeIndex) const;
	void WalkStacklessTreeAgainstRay(NodeOverlapCallback* nodeCallback, const gm::Vector3& raySource, const gm::Vector3& rayTarget, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax, int startNodeIndex, int endNodeIndex) const;
	void WalkStacklessQuantizedTreeCacheFriendly(NodeOverlapCallback* nodeCallback, unsigned short int* quantizedQueryAABBMin, unsigned short int* quantizedQueryAABBMax) const;
	void WalkRecursiveQuantizedTreeAgainstQueryAABB(const QuantizedBvhNode* currentNode, NodeOverlapCallback* nodeCallback, unsigned short int* quantizedQueryAABBMin, unsigned short int* quantizedQueryAABBMax) const;
	void WalkRecursiveQuantizedTreeAgainstQuantizedTree(const QuantizedBvhNode* treeNodeA, const QuantizedBvhNode* treeNodeB, NodeOverlapCallback* nodeCallback) const;
	void UpdateSubtreeHeader(int leftChildNodexIndex, int rightChildNodexIndex);

	gm::Vector3 GetAABBMin(int nodeIndex) const
	{
		if (_useQuantization) { return UnQuantize(&_quantizedLeafNodes[nodeIndex].QuantizedAABBMin[0]); }
		return _leafNodes[nodeIndex].AABBMinOrigin;
	}
	gm::Vector3 GetAABBMax(int nodeIndex) const
	{
		if (_useQuantization) { return UnQuantize(&_quantizedLeafNodes[nodeIndex].QuantizedAABBMax[0]); }
		return _leafNodes[nodeIndex].AABBMaxOrigin;
	}
	void SetInternalNodeAABBMin(int nodeIndex, const gm::Vector3& aabbMin)
	{
		if (_useQuantization) { Quantize(&_quantizedContiguousNodes[nodeIndex].QuantizedAABBMin[0], aabbMin, 0); }
		else { _contiguousNodes[nodeIndex].AABBMinOrigin = aabbMin; }
	}
	void SetInternalNodeAABBMax(int nodeIndex, const gm::Vector3& aabbMax)
	{
		if (_useQuantization) { Quantize(&_quantizedContiguousNodes[nodeIndex].QuantizedAABBMax[0], aabbMax, 1); }
		else { _contiguousNodes[nodeIndex].AABBMaxOrigin = aabbMax; }
	}
	void SetInternalNodeEscapeIndex(int nodeIndex, int escapeIndex)
	{
		if (_useQuantization) { _quantizedContiguousNodes[nodeIndex].EscapeIndexOrTriangleIndex = -escapeIndex; }
		else                  { _contiguousNodes[nodeIndex].EscapeIndex = escapeIndex; }
	}
	void MergeInternalNodeAABB(int nodeIndex, const gm::Vector3& newAABBMin, const gm::Vector3& newAABBMax)
	{
		if (_useQuantization)
		{
			unsigned short int quantizedAABBMin[3];
			unsigned short int quantizedAABBMax[3];
			Quantize(quantizedAABBMin, newAABBMin, 0);
			Quantize(quantizedAABBMax, newAABBMax, 1);
		}
		else
		{
			_contiguousNodes[nodeIndex].AABBMinOrigin = Min(_contiguousNodes[nodeIndex].AABBMinOrigin, newAABBMin);
			_contiguousNodes[nodeIndex].AABBMaxOrigin = Min(_contiguousNodes[nodeIndex].AABBMaxOrigin, newAABBMax);
		}
	}
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::Vector3 _bvhAABBMin;
	gm::Vector3 _bvhAABBMax;
	gm::Vector3 _bvhQuantization;
	
	int  _version;
	int  _currentNodeIndex;
	bool _useQuantization;

	NodeArray          _leafNodes;
	NodeArray          _contiguousNodes;
	QuantizedNodeArray _quantizedLeafNodes;
	QuantizedNodeArray _quantizedContiguousNodes;

	TraversalMode       _traversalMode;
	BvhSubtreeInfoArray _subtreeHeaders;

	mutable int         _subtreeHeaderCount;

private:
	QuantizedBvh(QuantizedBvh& other, bool ownMemory);
};
#endif