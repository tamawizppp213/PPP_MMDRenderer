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
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../../../../Include/Physics/Collision/BroadPhaseCollision/QuantizedBvh.hpp"
#include "../../../../Include/Physics/Util/TransformUtil.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define RAYAABB2
#define VERSION 1
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
QuantizedBvh::QuantizedBvh() : _version(VERSION), _useQuantization(false), _traversalMode(TraversalMode::Stackless), _subtreeHeaderCount(0)
{
	_bvhAABBMin = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	_bvhAABBMax = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
}
QuantizedBvh::QuantizedBvh(QuantizedBvh& self, bool ownMemory)
{
	_bvhAABBMin      = self._bvhAABBMin;
	_bvhAABBMax      = self._bvhAABBMax;
	_bvhQuantization = self._bvhQuantization;
	_version         = VERSION;
}
QuantizedBvh::~QuantizedBvh() {};
void QuantizedBvh::BuildInternal()
{
	/*-------------------------------------------------------------------
	-           Assumes that caller filled in the _quantizedLeafNodes
	---------------------------------------------------------------------*/
	_useQuantization   = true;
	int leafNodeCount = 0;

	if (_useQuantization)
	{
		leafNodeCount = _quantizedLeafNodes.size();
		_quantizedLeafNodes.resize(2 * leafNodeCount);
	}

	_currentNodeIndex = 0;
	BuildTree(0, leafNodeCount);

	/*-------------------------------------------------------------------
	-    If the entire tree is small then subtree size, 
	     we need to create a header info for the tree
	---------------------------------------------------------------------*/
	if (_useQuantization && !_subtreeHeaders.size())
	{
		BvhSubtreeInfo& subtree = _subtreeHeaders.expand();
		subtree.SetAABBFromQuantizedNode(_quantizedContiguousNodes[0]);
		subtree.RootNodeIndex = 0;
		subtree.SubtreeSize = _quantizedContiguousNodes[0].IsLeafNode() ? 1 : _quantizedContiguousNodes[0].GetEscapeIndex();
	}

	_subtreeHeaderCount = _subtreeHeaders.size();

	_quantizedLeafNodes.clear();
	_leafNodes.clear();
}
void QuantizedBvh::SetQuantizationValues(const Vector3& bvhAABBMin, const Vector3& bvhAABBMax, float quantizationMargin)
{
	//enlarge the AABB to avoid division by zero when initializing the quantization values
	Vector3 clampValue(quantizationMargin, quantizationMargin, quantizationMargin);
	_bvhAABBMin = bvhAABBMin - clampValue;
	_bvhAABBMax = bvhAABBMax + clampValue;
	Vector3 aabbSize = _bvhAABBMax - _bvhAABBMin;
	_bvhQuantization = Vector3(65533.0f, 65533.0f, 65533.0f) / aabbSize;

	_useQuantization = true;

	{
		unsigned short vecIn[3];
		Vector3 v;
		{
			Quantize(vecIn, _bvhAABBMin, false);
			v = UnQuantize(vecIn);
			_bvhAABBMin = Min(_bvhAABBMin, (v - clampValue));
		}
		aabbSize = _bvhAABBMax - _bvhAABBMin;
		_bvhQuantization = Vector3(65533.0f, 65533.0f, 65533.0f) / aabbSize;
		{
			Quantize(vecIn, _bvhAABBMax, true);
			v = UnQuantize(vecIn);
			_bvhAABBMax = Max(_bvhAABBMax, (v + clampValue));
		}
		aabbSize = _bvhAABBMax - _bvhAABBMin;
		_bvhQuantization = gm::Vector3(65533.0f, 65533.0f, 65533.0f) / aabbSize;
	}
}
void QuantizedBvh::Quantize(unsigned short* out, const gm::Vector3& point, int isMax) const
{
	assert(_useQuantization);

	assert((float)point.GetX() <= _bvhAABBMax.GetX());
	assert((float)point.GetY() <= _bvhAABBMax.GetY());
	assert((float)point.GetZ() <= _bvhAABBMax.GetZ());

	assert((float)point.GetX() >= _bvhAABBMin.GetX());
	assert((float)point.GetY() >= _bvhAABBMin.GetY());
	assert((float)point.GetZ() >= _bvhAABBMin.GetZ());

	Vector3 v = (point - _bvhAABBMin) * _bvhQuantization;
	///Make sure rounding is done in a way that unQuantize(quantizeWithClamp(...)) is conservative
	///end-points always set the first bit, so that they are sorted properly (so that neighbouring AABBs overlap properly)
	///@todo: double-check this
	if (isMax)
	{
		out[0] = (unsigned short)(((unsigned short)(v.GetX() + Scalar(1.0f)) | 1));
		out[1] = (unsigned short)(((unsigned short)(v.GetY() + Scalar(1.0f)) | 1));
		out[2] = (unsigned short)(((unsigned short)(v.GetZ() + Scalar(1.0f)) | 1));
	}
	else
	{
		out[0] = (unsigned short)(((unsigned short)(v.GetX()) & 0xfffe));
		out[1] = (unsigned short)(((unsigned short)(v.GetY()) & 0xfffe));
		out[2] = (unsigned short)(((unsigned short)(v.GetZ()) & 0xfffe));
	}
}
void QuantizedBvh::QuantizeWithClamp(unsigned short* out, const gm::Vector3& point2, int isMax) const
{
	assert(_useQuantization);

	Vector3 clampedPoint(point2);
	clampedPoint = Max(clampedPoint, _bvhAABBMin);
	clampedPoint = Min(clampedPoint, _bvhAABBMax);

	Quantize(out, clampedPoint, isMax);
}
gm::Vector3 QuantizedBvh::UnQuantize(const unsigned short* vectorIn) const
{
	Vector3 vecOut;
	vecOut = Vector3(
		(float)(vectorIn[0]) / (_bvhQuantization.GetX()),
		(float)(vectorIn[1]) / (_bvhQuantization.GetY()),
		(float)(vectorIn[2]) / (_bvhQuantization.GetZ()));
	vecOut += _bvhAABBMin;
	return vecOut;
}

void QuantizedBvh::BuildTree(int startIndex, int endIndex)
{
	int splitAxis, splitIndex, i;
	int numIndices   = endIndex - startIndex;
	int currentIndex = _currentNodeIndex;
	assert(numIndices > 0);

	if (numIndices == 1)
	{
		AssignInternalNodeFromLeafNode(_currentNodeIndex, startIndex);
		_currentNodeIndex++; return;
	}

	splitAxis  = CalculateSplittingAxis(startIndex, endIndex);
	splitIndex = SortAndCalculateSplittingIndex(startIndex, endIndex, splitAxis);

	int internalNodeIndex = _currentNodeIndex;
	SetInternalNodeAABBMin(_currentNodeIndex, _bvhAABBMax); //can't use btVector3(SIMD_INFINITY,SIMD_INFINITY,SIMD_INFINITY)) because of quantization
	SetInternalNodeAABBMax(_currentNodeIndex, _bvhAABBMin); //can't use btVector3(-SIMD_INFINITY,-SIMD_INFINITY,-SIMD_INFINITY)) because of quantization

	for (i = startIndex; i < endIndex; ++i)
	{
		MergeInternalNodeAABB(_currentNodeIndex, GetAABBMin(i), GetAABBMax(i));
	}
	_currentNodeIndex++;


}
void QuantizedBvh::UpdateSubtreeHeader(int leftChildNodexIndex, int rightChildNodexIndex)
{
	assert(_useQuantization);

	QuantizedBvhNode& leftChildNode = _quantizedContiguousNodes[leftChildNodexIndex];
	int leftSubTreeSize = leftChildNode.IsLeafNode() ? 1 : leftChildNode.GetEscapeIndex();
	int leftSubTreeSizeInBytes = leftSubTreeSize * static_cast<int>(sizeof(QuantizedBvhNode));

	QuantizedBvhNode& rightChildNode = _quantizedContiguousNodes[rightChildNodexIndex];
	int rightSubTreeSize = rightChildNode.IsLeafNode() ? 1 : rightChildNode.GetEscapeIndex();
	int rightSubTreeSizeInBytes = rightSubTreeSize * static_cast<int>(sizeof(QuantizedBvhNode));

	if (leftSubTreeSizeInBytes <= MAX_SUBTREE_SIZE_IN_BYTES)
	{
		BvhSubtreeInfo& subtree = _subtreeHeaders.expand();
		subtree.SetAABBFromQuantizedNode(leftChildNode);
		subtree.RootNodeIndex = leftChildNodexIndex;
		subtree.SubtreeSize   = leftSubTreeSize;
	}

	if (rightSubTreeSizeInBytes <= MAX_SUBTREE_SIZE_IN_BYTES)
	{
		BvhSubtreeInfo& subtree = _subtreeHeaders.expand();
		subtree.SetAABBFromQuantizedNode(rightChildNode);
		subtree.RootNodeIndex = rightChildNodexIndex;
		subtree.SubtreeSize   = rightSubTreeSize;
	}

	//PCK: update the copy of the size
	_subtreeHeaderCount = _subtreeHeaders.size();
}
int QuantizedBvh::SortAndCalculateSplittingIndex(int startIndex, int endIndex, int splitAxis)
{
	int i;
	int splitIndex = startIndex;
	int numIndices = endIndex - startIndex;
	float splitValue;

	Vector3 means(0.0f, 0.0f, 0.0f);
	for (i = startIndex; i < endIndex; ++i)
	{
		Vector3 center = 0.5f * (GetAABBMax(i) + GetAABBMin(i));
		means += center;
	} 
	means = (1.0f / numIndices) * means;

	splitValue = means.GetElement(splitAxis);

	//sort leafNodes so all values larger then splitValue comes first, and smaller values start from 'splitIndex'.
	for (i = startIndex; i < endIndex; i++)
	{
		Vector3 center = 0.5f * (GetAABBMax(i) + GetAABBMin(i));
		if (center.GetElement(splitAxis) > splitValue)
		{
			//swap
			SwapLeafNodes(i, splitIndex);
			splitIndex++;
		}
	}

	//if the splitIndex causes unbalanced trees, fix this by using the center in between startIndex and endIndex
	//otherwise the tree-building might fail due to stack-overflows in certain cases.
	//unbalanced1 is unsafe: it can cause stack overflows
	//bool unbalanced1 = ((splitIndex==startIndex) || (splitIndex == (endIndex-1)));

	//unbalanced2 should work too: always use center (perfect balanced trees)
	//bool unbalanced2 = true;

	//this should be safe too:
	int rangeBalancedIndices = numIndices / 3;
	bool unbalanced = ((splitIndex <= (startIndex + rangeBalancedIndices)) || (splitIndex >= (endIndex - 1 - rangeBalancedIndices)));

	if (unbalanced)
	{
		splitIndex = startIndex + (numIndices >> 1);
	}

	bool unbal = (splitIndex == startIndex) || (splitIndex == (endIndex));
	(void)unbal;
	assert(!unbal);

	return splitIndex;
}
int QuantizedBvh::CalculateSplittingAxis(int startIndex, int endIndex)
{
	int i;

	Vector3 means(0.0f, 0.0f, 0.0f);
	Vector3 variance(0.0f, 0.0f, 0.0f);
	int numIndices = endIndex - startIndex;

	for (i = startIndex; i < endIndex; i++)
	{
		Vector3 center = 0.5f * (GetAABBMax(i) + GetAABBMin(i));
		means += center;
	}
	means = (1.0f / numIndices) * means;

	for (i = startIndex; i < endIndex; i++)
	{
		Vector3 center = 0.5f * (GetAABBMax(i) + GetAABBMin(i));
		Vector3 diff2 = center - means;
		diff2 = diff2 * diff2;
		variance += diff2;
	}
	variance = (1.0f / (numIndices - 1)) * variance;

	return (int)MaxAxis3(variance);
}
void QuantizedBvh::ReportAABBOverlappingNodex(NodeOverlapCallback* nodeCallback, const Vector3& aabbMin, const Vector3& aabbMax) const
{
	if (_useQuantization)
	{
		///quantize query AABB
		unsigned short int quantizedQueryAabbMin[3];
		unsigned short int quantizedQueryAabbMax[3];
		QuantizeWithClamp(quantizedQueryAabbMin, aabbMin, 0);
		QuantizeWithClamp(quantizedQueryAabbMax, aabbMax, 1);

		switch (_traversalMode)
		{
		case TraversalMode::Stackless:
			WalkStacklessQuantizedTree(nodeCallback, quantizedQueryAabbMin, quantizedQueryAabbMax, 0, _currentNodeIndex);
			break;
		case TraversalMode::Stackless_Cache_Friendly:
			WalkStacklessQuantizedTreeCacheFriendly(nodeCallback, quantizedQueryAabbMin, quantizedQueryAabbMax);
			break;
		case TraversalMode::Recursive:
		{
			const QuantizedBvhNode* rootNode = &_quantizedContiguousNodes[0];
			WalkRecursiveQuantizedTreeAgainstQueryAABB(rootNode, nodeCallback, quantizedQueryAabbMin, quantizedQueryAabbMax);
		}
		break;
		default:
			//unsupported
			assert(0);
		}
	}
	else
	{
		WalkStacklessTree(nodeCallback, aabbMin, aabbMax);
	}
}
void QuantizedBvh::WalkStacklessTree(NodeOverlapCallback* nodeCallback, const Vector3& aabbMin, const Vector3& aabbMax) const
{
	assert(!_useQuantization);

	const OptimizedBvhNode* rootNode = &_contiguousNodes[0];
	int escapeIndex, curIndex = 0;
	int walkIterations = 0;
	bool isLeafNode;
	//PCK: unsigned instead of bool
	unsigned aabbOverlap;

	while (curIndex < _currentNodeIndex)
	{
		//catch bugs in tree data
		assert(walkIterations < _currentNodeIndex);

		walkIterations++;
		aabbOverlap = TestAABBAgainstAABB2(aabbMin, aabbMax, rootNode->AABBMinOrigin, rootNode->AABBMaxOrigin);
		isLeafNode = rootNode->EscapeIndex == -1;

		//PCK: unsigned instead of bool
		if (isLeafNode && (aabbOverlap != 0))
		{
			nodeCallback->ProcessNode(rootNode->SubPart, rootNode->TriangleIndex);
		}

		//PCK: unsigned instead of bool
		if ((aabbOverlap != 0) || isLeafNode)
		{
			rootNode++;
			curIndex++;
		}
		else
		{
			escapeIndex = rootNode->EscapeIndex;
			rootNode += escapeIndex;
			curIndex += escapeIndex;
		}
	}
}
void QuantizedBvh::WalkRecursiveQuantizedTreeAgainstQueryAABB(const QuantizedBvhNode* currentNode, NodeOverlapCallback* nodeCallback, unsigned short int* quantizedQueryAABBMin, unsigned short int* quantizedQueryAABBMax) const
{
	assert(_useQuantization);

	bool isLeafNode;
	//PCK: unsigned instead of bool
	unsigned aabbOverlap;

	//PCK: unsigned instead of bool
	aabbOverlap = TestQuantizedAABBAgainstQuantizedAABB(quantizedQueryAABBMin, quantizedQueryAABBMax, currentNode->QuantizedAABBMin, currentNode->QuantizedAABBMax);
	isLeafNode = currentNode->IsLeafNode();

	//PCK: unsigned instead of bool
	if (aabbOverlap != 0)
	{
		if (isLeafNode)
		{
			nodeCallback->ProcessNode(currentNode->GetPartID(), currentNode->GetTriangleIndex());
		}
		else
		{
			//process left and right children
			const QuantizedBvhNode* leftChildNode = currentNode + 1;
			WalkRecursiveQuantizedTreeAgainstQueryAABB(leftChildNode, nodeCallback, quantizedQueryAABBMin, quantizedQueryAABBMax);

			const QuantizedBvhNode* rightChildNode = leftChildNode->IsLeafNode() ? leftChildNode + 1 : leftChildNode + leftChildNode->GetEscapeIndex();
			WalkRecursiveQuantizedTreeAgainstQueryAABB(rightChildNode, nodeCallback, quantizedQueryAABBMin, quantizedQueryAABBMax);
		}
	}
}
void QuantizedBvh::WalkStacklessTreeAgainstRay(NodeOverlapCallback* nodeCallback, const Vector3& raySource, const Vector3& rayTarget, const Vector3& aabbMin, const Vector3& aabbMax, int startNodeIndex, int endNodeIndex) const
{
	assert(!_useQuantization);

	const OptimizedBvhNode* rootNode = &_contiguousNodes[0];
	int escapeIndex, curIndex = 0;
	int walkIterations = 0;
	bool isLeafNode;
	//PCK: unsigned instead of bool
	unsigned aabbOverlap = 0;
	unsigned rayBoxOverlap = 0;
	float    lambda_max = 1.0;

	/* Quick pruning by quantized box */
	Vector3 rayAabbMin = raySource;
	Vector3 rayAabbMax = raySource;
	rayAabbMin = Min(rayAabbMin, rayTarget);
	rayAabbMax = Max(rayAabbMax, rayTarget);

	/* Add box cast extents to bounding box */
	rayAabbMin += aabbMin;
	rayAabbMax += aabbMax;

#ifdef RAYAABB2
	Vector3 rayDir = (rayTarget - raySource);
	Normalize(rayDir);// stephengold changed normalize to safeNormalize 2020-02-17
	lambda_max = Dot(rayDir, (rayTarget - raySource));
	///what about division by zero? --> just set rayDirection[i] to 1.0
	Vector3 rayDirectionInverse;
	rayDirectionInverse.SetX((float)rayDir.GetX() == 0.0f ? FLT_MAX : 1.0f / (float)rayDir.GetX());
	rayDirectionInverse.SetY((float)rayDir.GetY() == 0.0f ? FLT_MAX : 1.0f / (float)rayDir.GetY());
	rayDirectionInverse.SetZ((float)rayDir.GetZ() == 0.0f ? FLT_MAX : 1.0f / (float)rayDir.GetZ());
	unsigned int sign[3] = { (float)rayDirectionInverse.GetX() < 0.0f, rayDirectionInverse.GetY() < 0.0f, rayDirectionInverse.GetZ() < 0.0f };
#endif

	Vector3 bounds[2];

	while (curIndex < _currentNodeIndex)
	{
		float param = 1.0;
		//catch bugs in tree data
		assert(walkIterations < _currentNodeIndex);

		walkIterations++;

		bounds[0] = rootNode->AABBMinOrigin;
		bounds[1] = rootNode->AABBMaxOrigin;
		/* Add box cast extents */
		bounds[0] -= aabbMax;
		bounds[1] -= aabbMin;

		aabbOverlap = TestAABBAgainstAABB2(rayAabbMin, rayAabbMax, rootNode->AABBMinOrigin, rootNode->AABBMaxOrigin);
		//perhaps profile if it is worth doing the aabbOverlap test first

#ifdef RAYAABB2
		///careful with this check: need to check division by zero (above) and fix the unQuantize method
		///thanks Joerg/hiker for the reproduction case!
		///http://www.bulletphysics.com/Bullet/phpBB3/viewtopic.php?f=9&t=1858
		rayBoxOverlap = aabbOverlap ? RayAABB2(raySource, rayDirectionInverse, sign, bounds, param, 0.0f, lambda_max) : false;

#else
		btVector3 normal;
		rayBoxOverlap = btRayAabb(raySource, rayTarget, bounds[0], bounds[1], param, normal);
#endif

		isLeafNode = rootNode->EscapeIndex == -1;

		//PCK: unsigned instead of bool
		if (isLeafNode && (rayBoxOverlap != 0))
		{
			nodeCallback->ProcessNode(rootNode->SubPart, rootNode->TriangleIndex);
		}

		//PCK: unsigned instead of bool
		if ((rayBoxOverlap != 0) || isLeafNode)
		{
			rootNode++;
			curIndex++;
		}
		else
		{
			escapeIndex = rootNode->EscapeIndex;
			rootNode += escapeIndex;
			curIndex += escapeIndex;
		}
	}
}
void QuantizedBvh::WalkStacklessQuantizedTreeAgainstRay(NodeOverlapCallback* nodeCallback, const Vector3& raySource, const Vector3& rayTarget, const Vector3& aabbMin, const Vector3& aabbMax, int startNodeIndex, int endNodeIndex) const
{
	assert(_useQuantization);

	int curIndex = startNodeIndex;
	int walkIterations = 0;
	int subTreeSize = endNodeIndex - startNodeIndex;
	(void)subTreeSize;

	const QuantizedBvhNode* rootNode = &_quantizedContiguousNodes[startNodeIndex];
	int escapeIndex;

	bool isLeafNode;
	//PCK: unsigned instead of bool
	unsigned boxBoxOverlap = 0;
	unsigned rayBoxOverlap = 0;

	float lambda_max = 1.0;

#ifdef RAYAABB2
	Vector3 rayDirection = (rayTarget - raySource);
	rayDirection = Normalize(rayDirection);// stephengold changed normalize to safeNormalize 2020-02-17
	lambda_max = Dot(rayDirection, rayTarget - raySource);
	///what about division by zero? --> just set rayDirection[i] to 1.0
	rayDirection.SetX((float)rayDirection.GetX() == 0.0f ? FLT_MAX : 1.0f / (float)rayDirection.GetX());
	rayDirection.SetY((float)rayDirection.GetY() == 0.0f ? FLT_MAX : 1.0f / (float)rayDirection.GetY());
	rayDirection.SetZ((float)rayDirection.GetZ() == 0.0f ? FLT_MAX : 1.0f / (float)rayDirection.GetZ());
	unsigned int sign[3] = { (float)rayDirection.GetX() < 0.0f, rayDirection.GetY() < 0.0f, rayDirection.GetZ() < 0.0f };
#endif

	/* Quick pruning by quantized box */
	Vector3 rayAabbMin = raySource;
	Vector3 rayAabbMax = raySource;
	rayAabbMin = Min(rayAabbMin, rayTarget);
	rayAabbMax = Max(rayAabbMax, rayTarget);

	/* Add box cast extents to bounding box */
	rayAabbMin += aabbMin;
	rayAabbMax += aabbMax;

	unsigned short int quantizedQueryAabbMin[3];
	unsigned short int quantizedQueryAabbMax[3];
	QuantizeWithClamp(quantizedQueryAabbMin, rayAabbMin, 0);
	QuantizeWithClamp(quantizedQueryAabbMax, rayAabbMax, 1);

	while (curIndex < endNodeIndex)
	{
		//#define VISUALLY_ANALYZE_BVH 1
#ifdef VISUALLY_ANALYZE_BVH
		//some code snippet to debugDraw aabb, to visually analyze bvh structure
		static int drawPatch = 0;
		//need some global access to a debugDrawer
		extern btIDebugDraw* debugDrawerPtr;
		if (curIndex == drawPatch)
		{
			btVector3 aabbMin, aabbMax;
			aabbMin = unQuantize(rootNode->m_quantizedAabbMin);
			aabbMax = unQuantize(rootNode->m_quantizedAabbMax);
			btVector3 color(1, 0, 0);
			debugDrawerPtr->drawAabb(aabbMin, aabbMax, color);
		}
#endif  //VISUALLY_ANALYZE_BVH

		//catch bugs in tree data
		assert(walkIterations < subTreeSize);

		walkIterations++;
		//PCK: unsigned instead of bool
		// only interested if this is closer than any previous hit
		float param = 1.0;
		rayBoxOverlap = 0;
		boxBoxOverlap = TestQuantizedAABBAgainstQuantizedAABB(quantizedQueryAabbMin, quantizedQueryAabbMax, rootNode->QuantizedAABBMin, rootNode->QuantizedAABBMax);
		isLeafNode = rootNode->IsLeafNode();
		if (boxBoxOverlap)
		{
			Vector3 bounds[2];
			bounds[0] = UnQuantize(rootNode->QuantizedAABBMin);
			bounds[1] = UnQuantize(rootNode->QuantizedAABBMax);
			/* Add box cast extents */
			bounds[0] -= aabbMax;
			bounds[1] -= aabbMin;
			Vector3 normal;
#if 0
			bool ra2 = btRayAabb2(raySource, rayDirection, sign, bounds, param, 0.0, lambda_max);
			bool ra = btRayAabb(raySource, rayTarget, bounds[0], bounds[1], param, normal);
			if (ra2 != ra)
			{
				printf("functions don't match\n");
			}
#endif
#ifdef RAYAABB2
			///careful with this check: need to check division by zero (above) and fix the unQuantize method
			///thanks Joerg/hiker for the reproduction case!
			///http://www.bulletphysics.com/Bullet/phpBB3/viewtopic.php?f=9&t=1858

			//BT_PROFILE("btRayAabb2");
			rayBoxOverlap = RayAABB2(raySource, rayDirection, sign, bounds, param, 0.0f, lambda_max);

#else
			rayBoxOverlap = true;  //btRayAabb(raySource, rayTarget, bounds[0], bounds[1], param, normal);
#endif
		}

		if (isLeafNode && rayBoxOverlap)
		{
			nodeCallback->ProcessNode(rootNode->GetPartID(), rootNode->GetTriangleIndex());
		}

		//PCK: unsigned instead of bool
		if ((rayBoxOverlap != 0) || isLeafNode)
		{
			rootNode++;
			curIndex++;
		}
		else
		{
			escapeIndex = rootNode->GetEscapeIndex();
			rootNode += escapeIndex;
			curIndex += escapeIndex;
		}
	}
}
void QuantizedBvh::WalkStacklessQuantizedTree(NodeOverlapCallback* nodeCallback, unsigned short int* quantizedQueryAABBMin, unsigned short int* quantizedQueryAABBMax, int startNodeIndex, int endNodeIndex) const
{
	assert(_useQuantization);

	int curIndex = startNodeIndex;
	int walkIterations = 0;
	int subTreeSize = endNodeIndex - startNodeIndex;
	(void)subTreeSize;

	const QuantizedBvhNode* rootNode = &_quantizedContiguousNodes[startNodeIndex];
	int escapeIndex;

	bool isLeafNode;
	//PCK: unsigned instead of bool
	unsigned aabbOverlap;

	while (curIndex < endNodeIndex)
	{
		//#define VISUALLY_ANALYZE_BVH 1
#ifdef VISUALLY_ANALYZE_BVH
		//some code snippet to debugDraw aabb, to visually analyze bvh structure
		static int drawPatch = 0;
		//need some global access to a debugDrawer
		extern btIDebugDraw* debugDrawerPtr;
		if (curIndex == drawPatch)
		{
			btVector3 aabbMin, aabbMax;
			aabbMin = unQuantize(rootNode->m_quantizedAabbMin);
			aabbMax = unQuantize(rootNode->m_quantizedAabbMax);
			btVector3 color(1, 0, 0);
			debugDrawerPtr->drawAabb(aabbMin, aabbMax, color);
		}
#endif  //VISUALLY_ANALYZE_BVH

		//catch bugs in tree data
		assert(walkIterations < subTreeSize);

		walkIterations++;
		//PCK: unsigned instead of bool
		aabbOverlap = TestQuantizedAABBAgainstQuantizedAABB(quantizedQueryAABBMin, quantizedQueryAABBMax, rootNode->QuantizedAABBMin, rootNode->QuantizedAABBMax);
		isLeafNode = rootNode->IsLeafNode();

		if (isLeafNode && aabbOverlap)
		{
			nodeCallback->ProcessNode(rootNode->GetPartID(), rootNode->GetTriangleIndex());
		}

		//PCK: unsigned instead of bool
		if ((aabbOverlap != 0) || isLeafNode)
		{
			rootNode++;
			curIndex++;
		}
		else
		{
			escapeIndex = rootNode->GetEscapeIndex();
			rootNode += escapeIndex;
			curIndex += escapeIndex;
		}
	}
}
void QuantizedBvh::WalkStacklessQuantizedTreeCacheFriendly(NodeOverlapCallback* nodeCallback, unsigned short int* quantizedQueryAABBMin, unsigned short int* quantizedQueryAABBMax) const
{
	assert(_useQuantization);

	int i;

	for (i = 0; i < this->_subtreeHeaders.size(); i++)
	{
		const BvhSubtreeInfo& subtree = _subtreeHeaders[i];

		//PCK: unsigned instead of bool
		unsigned overlap = TestQuantizedAABBAgainstQuantizedAABB(quantizedQueryAABBMin, quantizedQueryAABBMax, subtree.QuantizedAABBMin, subtree.QuantizedAABBMax);
		if (overlap != 0)
		{
			WalkStacklessQuantizedTree(nodeCallback, quantizedQueryAABBMin, quantizedQueryAABBMax,
				subtree.RootNodeIndex,
				subtree.RootNodeIndex + subtree.SubtreeSize);
		}
	}
}

void QuantizedBvh::ReportRayOverlappingNodex(NodeOverlapCallback* nodeCallback, const Vector3& raySource, const Vector3& rayTarget) const
{
	ReportBoxCastOverlappingNodex(nodeCallback, raySource, rayTarget, Vector3(0, 0, 0), Vector3(0, 0, 0));
}
void QuantizedBvh::ReportBoxCastOverlappingNodex(NodeOverlapCallback* nodeCallback, const gm::Vector3& raySource, const Vector3& rayTarget, const Vector3& aabbMin, const Vector3& aabbMax) const
{
	if (_useQuantization)
	{
		WalkStacklessQuantizedTreeAgainstRay(nodeCallback, raySource, rayTarget, aabbMin, aabbMax, 0, _currentNodeIndex);
	}
	else
	{
		WalkStacklessTreeAgainstRay(nodeCallback, raySource, rayTarget, aabbMin, aabbMax, 0, _currentNodeIndex);
	}
}
void QuantizedBvh::SwapLeafNodes(int i, int splitIndex)
{
	if (_useQuantization)
	{
		QuantizedBvhNode tmp = _quantizedLeafNodes[i];
		_quantizedLeafNodes[i] = _quantizedLeafNodes[splitIndex];
		_quantizedLeafNodes[splitIndex] = tmp;
	}
	else
	{
		OptimizedBvhNode tmp = _leafNodes[i];
		_leafNodes[i] = _leafNodes[splitIndex];
		_leafNodes[splitIndex] = tmp;
	}
}
void QuantizedBvh::AssignInternalNodeFromLeafNode(int internalNode, int leafNodeIndex)
{
	if (_useQuantization)
	{
		_quantizedContiguousNodes[internalNode] = _quantizedLeafNodes[leafNodeIndex];
	}
	else
	{
		_contiguousNodes[internalNode] = _leafNodes[leafNodeIndex];
	}
}

