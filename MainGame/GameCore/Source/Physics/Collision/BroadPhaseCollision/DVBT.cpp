//////////////////////////////////////////////////////////////////////////////////
///             @file   Dbvt.cpp
///             @brief  Dbvt
///             @author Partially edit by toide
///             @date   2021_09_25
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
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/DBVT.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
using NodeArray      = AlignedObjectArray<DbvtNode*>;
using ConstNodeArray = AlignedObjectArray<const DbvtNode*>;

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
struct DbvtNodeEnumerator : Dbvt::ICollide
{
	ConstNodeArray Nodes;
	void Process(const DbvtNode* node) { Nodes.push_back(node); }
};

#pragma region Static CPP Function
/****************************************************************************
*							IndexOf
*************************************************************************//**
*  @fn        static INLINE int IndexOf(const DbvtNode* node)
*  @brief     Whether Parent->childNode is the same node.
*  @param[in] DbvtNode* node
*  @return 　　int (bool)
*****************************************************************************/
static INLINE int IndexOf(const DbvtNode* node) { return (node->Parent->Childs[1]) == node; }
/****************************************************************************
*							Marge
*************************************************************************//**
*  @fn        static INLINE DbvtVolume Merge(const DbvtVolume& a, const DbvtVolume& b)
*  @brief     Merge dynamic bounding volume tree between a and b.
*  @param[in] DbvtVolume& a 
*  @param[in] DbvtVolume& b
*  @return 　　DbvtVolume
*****************************************************************************/
static INLINE DbvtVolume Merge(const DbvtVolume& a, const DbvtVolume& b)
{
	DbvtVolume result;
	Merge(a, b, result);
	return result;
}

/****************************************************************************
*							Size
*************************************************************************//**
*  @fn        static INLINE float Size(const DbvtVolume& a)
*  @brief     Get edges + volume size (x*y*z + edgeX + edgeY + edgeZ)
*  @param[in] DbvtVolume& a
*  @return 　　float
*****************************************************************************/
static INLINE float Size(const DbvtVolume& a)
{
	const Vector3 edges = a.Lengths();
	return edges.GetX() * edges.GetY() * edges.GetZ() + edges.GetX() + edges.GetY() + edges.GetZ();
}

/****************************************************************************
*							GetMaxDepth
*************************************************************************//**
*  @fn        static void GetMaxDepth(const DbvtNode* node, int depth, int& maxDepth)
*  @brief     Get tree maxDepth
*  @param[in] DbvtNode* node
*  @param[in] depth
*  @param[out]maxDepth
*  @return 　　void
*****************************************************************************/
static void GetMaxDepth(const DbvtNode* node, int depth, int& maxDepth)
{
	if (node->IsInternal())
	{
		GetMaxDepth(node->Childs[0], depth + 1, maxDepth);
		GetMaxDepth(node->Childs[1], depth + 1, maxDepth);
	}
	else
	{
		maxDepth = static_cast<int>(Max(maxDepth, depth));
	}
}
/****************************************************************************
*							DeleteNode
*************************************************************************//**
*  @fn        static INLINE void DeleteNode(Dbvt* dbvt, DbvtNode* node)
*  @brief     Delete tree node
*  @param[in,out] Dbvt* dbvt
*  @param[in,out] DbvtNode* node
*  @return void
*****************************************************************************/
static INLINE void DeleteNode(Dbvt* dbvt, DbvtNode* node)
{
	AlignedFreeInternal(dbvt->Free);
	dbvt->Free = node;
}
/****************************************************************************
*							RecursiveDeleteNode
*************************************************************************//**
*  @fn        static void RecursiveDeleteNode(Dbvt* dbvt, DbvtNode* node)
*  @brief     recursive delete node
*  @param[in,out] Dbvt* dbvt
*  @param[in,out] DbvtNode* node
*  @return 　　void
*****************************************************************************/
static void RecursiveDeleteNode(Dbvt* dbvt, DbvtNode* node)
{
	// node
	if (node == 0) { return; }

	// recursive binary leaf
	if (!node->IsLeaf())
	{
		RecursiveDeleteNode(dbvt, node->Childs[0]);
		RecursiveDeleteNode(dbvt, node->Childs[1]);
	}

	// delete
	if (node == dbvt->Root)
	{
		dbvt->Root = 0;
	}
	DeleteNode(dbvt, node);
}
/****************************************************************************
*							CreateNode
*************************************************************************//**
*  @fn        static INLINE DbvtNode* CreateNode(Dbvt* dbvt, DbvtNode* parent, void* data)
*  @brief     Create node
*  @param[in,out] Dbvt* dbvt
*  @param[in,out] DbvtNode* parent
*  @param[in] void* data
*  @return 　　DbvtNode*
*****************************************************************************/
static INLINE DbvtNode* CreateNode(Dbvt* dbvt, DbvtNode* parent, void* data)
{
	DbvtNode* node;
	if (dbvt->Free)
	{
		node       = dbvt->Free;
		dbvt->Free = 0;
	}
	else
	{
		node = new(AlignedAllocInternal(sizeof(DbvtNode), 16)) DbvtNode();
	}
	node->Parent    = parent;
	node->Data      = data;
	node->Childs[1] = 0;
	return node;
}
/****************************************************************************
*							CreateNode
*************************************************************************//**
*  @fn        static INLINE DbvtNode* CreateNode(Dbvt* dbvt, DbvtNode* parent, const DbvtVolume& volume, void* data)
*  @brief     Create node
*  @param[in,out] Dbvt* dbvt
*  @param[in,out] DbvtNode* parent
*  @param[in] DbvtVolume& volume
*  @param[in] void* data
*  @return 　　DbvtNode*
*****************************************************************************/
static INLINE DbvtNode* CreateNode(Dbvt* dbvt, DbvtNode* parent, const DbvtVolume& volume, void* data)
{
	DbvtNode* node = CreateNode(dbvt, parent, data);
	node->Volume   = volume;
	return node;
}
/****************************************************************************
*							CreateNode
*************************************************************************//**
*  @fn        static INLINE DbvtNode* CreateNode(Dbvt* dbvt, DbvtNode* parent, const DbvtVolume& volume0, const DbvtVolume& volume1, void* data)
*  @brief     Create node
*  @param[in,out] Dbvt* dbvt
*  @param[in,out] DbvtNode* parent
*  @param[in] DbvtVolume& volume0
*  @param[in] DbvtVolume& volume1
*  @param[in] void* data
*  @return 　　DbvtNode*
*****************************************************************************/
static INLINE DbvtNode* CreateNode(Dbvt* dbvt, DbvtNode* parent, const DbvtVolume& volume0, const DbvtVolume& volume1, void* data)
{
	DbvtNode* node = CreateNode(dbvt, parent, data);
	Merge(volume0, volume1, node->Volume);
	return node;
}
/****************************************************************************
*							InsertLeaf
*************************************************************************//**
*  @fn        static void InsertLeaf(Dbvt* dbvt, DbvtNode* root, DbvtNode* leaf)
*  @brief     insert leaf
*  @param[in,out] Dbvt* dbvt
*  @param[in,out] DbvtNode* root
*  @param[in,out] DbvtNode* leaf
*  @return 　　void
*****************************************************************************/
static void InsertLeaf(Dbvt* dbvt, DbvtNode* root, DbvtNode* leaf)
{
	if (!dbvt->Root)
	{
		dbvt->Root   = leaf;
		leaf->Parent = nullptr;
	}
	else
	{
		if (!root->IsLeaf())
		{
			do
			{
				root = root->Childs[Select(leaf->Volume, root->Childs[0]->Volume, root->Childs[1]->Volume)];
			} while (!root->IsLeaf());
		}
		DbvtNode* previous = root->Parent;
		DbvtNode* node = CreateNode(dbvt, previous, leaf->Volume, root->Volume, 0);
		if (previous)
		{
			previous->Childs[IndexOf(root)] = node;
			node->Childs[0]                 = root;
			root->Parent                    = node;
			node->Childs[1]                 = leaf;
			leaf->Parent                    = node;
			do
			{
				if (!previous->Volume.Contain(node->Volume))
					Merge(previous->Childs[0]->Volume, previous->Childs[1]->Volume, previous->Volume);
				else { break; }
				node = previous;
			} while (0 != (previous = node->Parent));
		}
		else
		{
			node->Childs[0] = root;
			root->Parent    = node;
			node->Childs[1] = leaf;
			leaf->Parent    = node;
			dbvt->Root      = node;
		}
	}
}
/****************************************************************************
*							RemoveLeaf
*************************************************************************//**
*  @fn        static DbvtNode* RemoveLeaf(Dbvt* dbvt, DbvtNode* leaf)
*  @brief     Remove 
*  @param[in,out] Dbvt* dbvt
*  @param[in    ] DbvtNode* leaf
*  @return 　　DbvtNode*
*****************************************************************************/
static DbvtNode* RemoveLeaf(Dbvt* dbvt, DbvtNode* leaf)
{
	if (leaf == dbvt->Root)
	{
		dbvt->Root = NULL;
		return NULL;
	}
	else
	{
		DbvtNode* parent   = leaf->Parent;
		DbvtNode* previous = parent->Parent;
		DbvtNode* sibling  = parent->Childs[1 - IndexOf(leaf)]; // 0 or 1
		if (previous)
		{
			previous->Childs[IndexOf(parent)] = sibling;
			sibling->Parent = previous;
			DeleteNode(dbvt, parent);
			while (previous)
			{
				const DbvtVolume pb = previous->Volume;
				Merge(previous->Childs[0]->Volume, previous->Childs[1]->Volume, previous->Volume);
				if (NotEqual(pb, previous->Volume))
				{
					previous = previous->Parent;
				}
				else
				{
					break;
				}
			}
			return (previous ? previous : dbvt->Root);
		}
		else
		{
			dbvt->Root      = sibling;
			sibling->Parent = NULL;
			DeleteNode(dbvt, parent);
			return dbvt->Root;
		}
	}
}
/****************************************************************************
*							FetchLeaves
*************************************************************************//**
*  @fn        static void FetchLeaves(Dbvt* dbvt, DbvtNode* root, NodeArray& leaves, int depth = -1)
*  @brief     Fetch leaves
*  @param[in,out] Dbvt* dbvt
*  @param[in]     DbvtNode* root
*  @param[in]     NodeArray& leaes
*  @return 　　void
*****************************************************************************/
static void FetchLeaves(Dbvt* dbvt, DbvtNode* root, NodeArray& leaves, int depth = -1)
{
	if (root->IsInternal() && depth)
	{
		FetchLeaves(dbvt, root->Childs[0], leaves, depth - 1);
		FetchLeaves(dbvt, root->Childs[1], leaves, depth - 1);
		DeleteNode(dbvt, root);
	}
	else
	{
		leaves.push_back(root);
	}
}
/****************************************************************************
*							LeftOfAxis
*************************************************************************//**
*  @fn        static bool LeftOfAxis(const DbvtNode* node, const Vector3& org, const Vector3& axis)
*  @brief     Define left of axis
*  @param[in] DbvtNode* node
*  @param[in] Vector3& origin
*  @param[in] Vector3& axis
*  @return 　　bool
*****************************************************************************/
static bool LeftOfAxis(const DbvtNode* node, const Vector3& origin, const Vector3& axis)
{
	return Dot(axis, node->Volume.Center() - origin) <= 0.0f;
}
/****************************************************************************
*							Split
*************************************************************************//**
*  @fn        static int Split(DbvtNode** leaves, int count, const Vector3& origin, const Vector3& axis)
*  @brief     split
*  @param[in,out] DbvtNode** leaves
*  @param[in] int count
*  @param[in] Vector3& origin
*  @param[in] Vector3& axis
*  @return 　　int
*****************************************************************************/
static int Split(DbvtNode** leaves, int count, const Vector3& origin, const Vector3& axis)
{
	int begin = 0;
	int end = count;
	for (;;)
	{
		// begin -> currentIndex
		while (begin != end && LeftOfAxis(leaves[begin], origin, axis)){ ++begin; }
		if    (begin == end){ break; }

		// end-> currentIndexBack
		while (begin != end && !LeftOfAxis(leaves[end - 1], origin, axis)) {--end;}
		if    (begin == end) { break; }

		// swap out of place nodes
		--end;
		DbvtNode* temp = leaves[begin];
		leaves[begin]  = leaves[end];
		leaves[end]    = temp;
		++begin;
	}

	return begin;
}
/****************************************************************************
*							Bounds
*************************************************************************//**
*  @fn        static DbvtVolume Bounds(DbvtNode** leaves, int count)
*  @brief     Bounds
*  @param[in,out] DbvtNode** leaves
*  @param[in] int count
*  @return 　　DbvtVolume
*****************************************************************************/
static DbvtVolume Bounds(DbvtNode** leaves, int count)
{
#ifdef USE_SSE
	ALIGNED16(char locals[sizeof(DbvtVolume)]);
	DbvtVolume* ptr = (DbvtVolume*)locals;
	DbvtVolume& volume = *ptr;
	volume = leaves[0]->Volume;
#else
	DbvtVolume volume = leaves[0]->Volume;
#endif
	for (int i = 1, ni = count; i < ni; ++i)
	{
		Merge(volume, leaves[i]->Volume, volume);
	}
	return volume;
}
/****************************************************************************
*							BottomUp
*************************************************************************//**
*  @fn        static void BottomUp(Dbvt* dbvt, DbvtNode** leaves, int count)
*  @brief     BottomUp 
*  @param[in, out] Dbvt* dbvt
*  @param[in,out] DbvtNode** leaves
*  @param[in] int count
*  @return 　　void
*****************************************************************************/
static void BottomUp(Dbvt* dbvt, DbvtNode** leaves, int count)
{
	while (count > 1)
	{
		Scalar minSize = FLT_MAX;
		int minIndex[2] = { -1, -1 };
		for (int i = 0; i < count; ++i)
		{
			for (int j = i + 1; j < count; ++j)
			{
				const Scalar size = Size(Merge(leaves[i]->Volume, leaves[j]->Volume));
				if ((float)size < minSize)
				{
					minSize = size;
					minIndex[0] = i;
					minIndex[1] = j;
				}
			}
		}
		DbvtNode* n[] = { leaves[minIndex[0]], leaves[minIndex[1]] };
		DbvtNode* p = CreateNode(dbvt, 0, n[0]->Volume, n[1]->Volume, 0);
		p->Childs[0] = n[0];
		p->Childs[1] = n[1];
		n[0]->Parent = p;
		n[1]->Parent = p;
		leaves[minIndex[0]] = p;
		leaves[minIndex[1]] = leaves[count - 1];
		--count;
	}
}
/****************************************************************************
*							TopDown
*************************************************************************//**
*  @fn        static DbvtNode* TopDown(Dbvt* dbvt, DbvtNode** leaves, int count, int bu_threshold)
*  @brief     TopDown
*  @param[in,out] Dbvt* dbvt
*  @param[in,out] DbvtNode** leaves
*  @param[in] int count
*  @param[in] int bu_threshold
*  @return 　　DbvtNode*
*****************************************************************************/
static DbvtNode* TopDown(Dbvt* dbvt, DbvtNode** leaves, int count, int bu_threshold)
{
	static const Vector3 axis[] = { Vector3(1, 0, 0),
									 Vector3(0, 1, 0),
									 Vector3(0, 0, 1) };
	assert(bu_threshold > 2);
	if (count > 1)
	{
		if (count > bu_threshold)
		{
			const DbvtVolume vol = Bounds(leaves, count);
			const Vector3 org = vol.Center();
			int partition;
			int bestaxis = -1;
			int bestmidp = count;
			int splitcount[3][2] = { {0, 0}, {0, 0}, {0, 0} };
			int i;
			for (i = 0; i < count; ++i)
			{
				const Vector3 x = leaves[i]->Volume.Center() - org;
				for (int j = 0; j < 3; ++j)
				{
					++splitcount[j][Dot(x, axis[j]) > 0.0f ? 1 : 0];
				}
			}
			for (i = 0; i < 3; ++i)
			{
				if ((splitcount[i][0] > 0) && (splitcount[i][1] > 0))
				{
					const int midp = (int)Abs(Scalar(splitcount[i][0] - splitcount[i][1]));
					if (midp < bestmidp)
					{
						bestaxis = i;
						bestmidp = midp;
					}
				}
			}
			if (bestaxis >= 0)
			{
				partition = Split(leaves, count, org, axis[bestaxis]);
				assert(partition != 0 && partition != count);
			}
			else
			{
				partition = count / 2 + 1;
			}
			DbvtNode* node = CreateNode(dbvt, 0, vol, 0);
			node->Childs[0] = TopDown(dbvt, &leaves[0], partition, bu_threshold);
			node->Childs[1] = TopDown(dbvt, &leaves[partition], count - partition, bu_threshold);
			node->Childs[0]->Parent = node;
			node->Childs[1]->Parent = node;
			return (node);
		}
		else
		{
			BottomUp(dbvt, leaves, count);
			return (leaves[0]);
		}
	}
	return (leaves[0]);
}
/****************************************************************************
*							Sort
*************************************************************************//**
*  @fn        static INLINE DbvtNode* Sort(DbvtNode* n, DbvtNode*& r)
*  @brief     sort 
*  @param[in,out] DbvtNode* n
*  @param[in,out] DbvtNode* r
*  @return 　　DbvtNode*
*****************************************************************************/
static INLINE DbvtNode* Sort(DbvtNode* n, DbvtNode*& r)
{
	DbvtNode* p = n->Parent;
	assert(n->IsInternal());
	if (p > n)
	{
		const int i = IndexOf(n);
		const int j = 1 - i;
		DbvtNode* s = p->Childs[j];
		DbvtNode* q = p->Parent;
		assert(n == p->Childs[i]);
		if (q) { q->Childs[IndexOf(p)] = n; }
		else { r = n; }

		s->Parent = n;
		p->Parent = n;
		n->Parent = q;
		p->Childs[0] = n->Childs[0];
		p->Childs[1] = n->Childs[1];
		p->Childs[0]->Parent = p;
		p->Childs[1]->Parent = p;
		n->Childs[i] = p;
		n->Childs[j] = s;
		DbvtVolume temp = p->Volume;
		p->Volume = n->Volume;
		n->Volume = temp;
		return p;
	}
	return n;
}
#pragma endregion Static CPP Function

#pragma region Dbvt
Dbvt::Dbvt()
{
	Root   = 0;
	Free   = 0;
	Lkhd   = -1;
	Leaves = 0;
	Opath  = 0;
}

Dbvt::~Dbvt()
{
	Clear();
}

/****************************************************************************
*							Clear
*************************************************************************//**
*  @fn        void Dbvt::Clear()
*  @brief     Clear all node. Callback on Destructor
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Dbvt::Clear()
{
	// all clear
	if (Root)
	{
		RecursiveDeleteNode(this, Root);
	}
	AlignedFreeInternal(Free);
	Free  = 0;
	Lkhd  = -1;
	Stack.clear();
	Opath = 0;
}
/****************************************************************************
*							OptimizeBottomUp
*************************************************************************//**
*  @fn        void Dbvt::OptimizeBottomUp()
*  @brief     Bottom up
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Dbvt::OptimizeBottomUp()
{
	if (Root)
	{
		NodeArray leaves;
		leaves.reserve(Leaves);
		FetchLeaves(this, Root, leaves);
		BottomUp(this, &leaves[0], leaves.size());
		Root = leaves[0];
	}
}
/****************************************************************************
*							OptimizeTopDown
*************************************************************************//**
*  @fn        void Dbvt::OptimizeTopDown(int bu_threshold)
*  @brief     Topdown
*  @param[in] int bu_threshold
*  @return 　　void
*****************************************************************************/
void Dbvt::OptimizeTopDown(int bu_threshold)
{
	if (Root)
	{
		NodeArray leaves;
		leaves.reserve(Leaves);
		FetchLeaves(this, Root, leaves);
		Root = TopDown(this, &leaves[0], leaves.size(), bu_threshold);
	}
}
/****************************************************************************
*							OptimizeIncremental
*************************************************************************//**
*  @fn        void Dbvt::OptimizeIncremental(int passes)
*  @brief     incremental
*  @param[in] int passes
*  @return 　　void
*****************************************************************************/
void Dbvt::OptimizeIncremental(int passes)
{
	if (passes < 0) { passes = Leaves; }
	if (Root && (passes > 0))
	{
		do
		{
			DbvtNode* node = Root;
			unsigned bit = 0;
			while (node->IsInternal())
			{
				node = Sort(node, Root)->Childs[(Opath >> bit) & 1];
				bit = (bit + 1) & (sizeof(unsigned) * 8 - 1);
			}
			Update(node);
			++Opath;
		} while (--passes);
	}
}
/****************************************************************************
*							Update
*************************************************************************//**
*  @fn        DbvtNode* Dbvt::Insert(const DbvtVolume& volume, void* data)
*  @brief     Insert node
*  @param[in] DbvtVolume volume
*  [param[in] void* data
*  @return 　　DbvtNode*
*****************************************************************************/
DbvtNode* Dbvt::Insert(const DbvtVolume& volume, void* data)
{
	DbvtNode* leaf = CreateNode(this, 0, volume, data);
	InsertLeaf(this, Root, leaf);
	++Leaves;
	return leaf;
}
/****************************************************************************
*							Update
*************************************************************************//**
*  @fn        void Dbvt::Update(DbvtNode* leaf, int lookahead)
*  @brief     Update
*  @param[in,out] DbvtNode* leaf
*  @param[in]     int lookahead
*  @return 　　void
*****************************************************************************/
void Dbvt::Update(DbvtNode* leaf, int lookahead)
{
	DbvtNode* root = RemoveLeaf(this, leaf);
	if (root)
	{
		if (lookahead >= 0)
		{
			for (int i = 0; (i < lookahead) && root->Parent; ++i)
			{
				root = root->Parent;
			}
		}
		else
		{
			root = Root;
		}
	}
	InsertLeaf(this, root, leaf);
}
/****************************************************************************
*							Update
*************************************************************************//**
*  @fn        void Dbvt::Update(DbvtNode* leaf, DbvtVolume& volume)
*  @brief     Update
*  @param[in,out] DbvtNode* leaf
*  @param[in,out] DbvtVolume& volume
*  @return 　　void
*****************************************************************************/
void Dbvt::Update(DbvtNode* leaf, DbvtVolume& volume)
{
	DbvtNode* root = RemoveLeaf(this, leaf);
	if (root)
	{
		if (Lkhd >= 0)
		{
			for (int i = 0; (i < Lkhd) && root->Parent; ++i)
			{
				root = root->Parent;
			}
		}
		else
			root = Root;
	}
	leaf->Volume = volume;
	InsertLeaf(this, root, leaf);
}
/****************************************************************************
*							Update
*************************************************************************//**
*  @fn        bool Dbvt::Update(DbvtNode* leaf, DbvtVolume& volume, const Vector3& velocity, float margin)
*  @brief     Update
*  @param[in,out] DbvtNode* leaf
*  @param[in,out] DbvtVolume& volume
*  @param[in]     Vector3& velocity
*  @param[in]     float margin
*  @return 　　bool
*****************************************************************************/
bool Dbvt::Update(DbvtNode* leaf, DbvtVolume& volume, const Vector3& velocity, float margin)
{
	if (leaf->Volume.Contain(volume)) return (false);
	volume.Expand(Vector3(margin, margin, margin));
	volume.SignedExpand(velocity);
	Update(leaf, volume);
	return (true);
}
/****************************************************************************
*							Update
*************************************************************************//**
*  @fn        bool Dbvt::Update(DbvtNode* leaf, DbvtVolume& volume, const Vector3& velocity)
*  @brief     Update
*  @param[in,out] DbvtNode* leaf
*  @param[in,out] DbvtVolume& volume
*  @param[in]     Vector3& velocity
*  @return 　　bool
*****************************************************************************/
bool Dbvt::Update(DbvtNode* leaf, DbvtVolume& volume, const Vector3& velocity)
{
	if (leaf->Volume.Contain(volume)) return (false);
	volume.SignedExpand(velocity);
	Update(leaf, volume);
	return (true);
}
/****************************************************************************
*							Update
*************************************************************************//**
*  @fn        bool Dbvt::Update(DbvtNode* leaf, DbvtVolume& volume, float margin)
*  @brief     Update
*  @param[in,out] DbvtNode* leaf
*  @param[in,out] DbvtVolume& volume
*  @param[in]     float margin
*  @return 　　bool
*****************************************************************************/
bool Dbvt::Update(DbvtNode* leaf, DbvtVolume& volume, float margin)
{
	if (leaf->Volume.Contain(volume)) return (false);
	volume.Expand(Vector3(margin, margin, margin));
	Update(leaf, volume);
	return (true);
}
/****************************************************************************
*							Remove
*************************************************************************//**
*  @fn        void Dbvt::Remove(DbvtNode* leaf)
*  @brief     Remove leaf and node
*  @param[in,out] DbvtNode* leaf
*  @return 　 void
*****************************************************************************/
void Dbvt::Remove(DbvtNode* leaf)
{
	RemoveLeaf(this, leaf);
	DeleteNode(this, leaf);
	--Leaves;
}
/****************************************************************************
*							Write
*************************************************************************//**
*  @fn        void Dbvt::Write(IWriter* iwriter) const
*  @brief     Write
*  @param[in,out] IWrite* iwriter
*  @return 　 void
*****************************************************************************/
void Dbvt::Write(IWriter* iwriter) const
{
	DbvtNodeEnumerator nodes;
	nodes.Nodes.reserve(Leaves * 2);
	EnumNodes(Root, nodes);
	iwriter->Prepare(Root, nodes.Nodes.size());

	for (int i = 0; i < nodes.Nodes.size(); ++i)
	{
		const DbvtNode* n = nodes.Nodes[i];
		int p = -1;
		if (n->Parent)
		{
			p = nodes.Nodes.findLinearSearch(n->Parent);
		}
		if (n->IsInternal())
		{
			const int c0 = nodes.Nodes.findLinearSearch(n->Childs[0]);
			const int c1 = nodes.Nodes.findLinearSearch(n->Childs[1]);
			iwriter->WriteNode(n, i, p, c0, c1);
		}
		else
		{
			iwriter->WriteLeaf(n, i, p);
		}
	}
}
/****************************************************************************
*							Clone
*************************************************************************//**
*  @fn        void Dbvt::Clone(Dbvt& dest, IClone* iclone = 0)
*  @brief     Clone 
*  @param[out] DbvtNode& destination
*  @param[out] IClone* iclone
*  @return 　 void
*****************************************************************************/
void Dbvt::Clone(Dbvt& dest, IClone* iclone) const
{
	dest.Clear();
	if (Root != 0)
	{
		AlignedObjectArray<StackCLN> stack;
		stack.reserve(Leaves);
		stack.push_back(StackCLN(Root, 0));
		do
		{
			const int i = stack.size() - 1;
			const StackCLN e = stack[i];
			DbvtNode* n = CreateNode(&dest, e.Parent, e.Node->Volume, e.Node->Data);
			stack.pop_back();
			if (e.Parent != 0)
				e.Parent->Childs[i & 1] = n;
			else
				dest.Root = n;
			if (e.Node->IsInternal())
			{
				stack.push_back(StackCLN(e.Node->Childs[0], n));
				stack.push_back(StackCLN(e.Node->Childs[1], n));
			}
			else
			{
				iclone->CloneLeaf(n);
			}
		} while (stack.size() > 0);
	}
}
/****************************************************************************
*							MaxDepth
*************************************************************************//**
*  @fn        int Dbvt::MaxDepth(const DbvtNode* node)
*  @brief     Count max depth size
*  @param[in] DbvtNode* node
*  @return 　 int 
*****************************************************************************/
int Dbvt::MaxDepth(const DbvtNode* node)
{
	int depth = 0;
	if (node)
	{
		GetMaxDepth(node, 1, depth);
	}
	return depth;
}
/****************************************************************************
*							CountLeaves
*************************************************************************//**
*  @fn        int Dbvt::CountLeaves(const DbvtNode* node)
*  @brief     Count leaves 
*  @param[in] DbvtNode* node
*  @return 　 int 
*****************************************************************************/
int Dbvt::CountLeaves(const DbvtNode* node)
{
	if (node->IsInternal())
	{
		return CountLeaves(node->Childs[0]) + CountLeaves(node->Childs[1]);
	}
	else
	{
		return 1;
	}
}
/****************************************************************************
*							ExtractLeaves
*************************************************************************//**
*  @fn        void Dbvt::ExtractLeaves(const DbvtNode* node, gm::AlignedObjectArray<const DbvtNode*>& leaves)
*  @brief     Extract leaves
*  @param[in] DbvtNode* node
*  @param[in] AlignedObjectArray<const DbvtNode*>& leaves
*  @return 　 void
*****************************************************************************/
void Dbvt::ExtractLeaves(const DbvtNode* node, gm::AlignedObjectArray<const DbvtNode*>& leaves)
{
	if (node->IsInternal())
	{
		ExtractLeaves(node->Childs[0], leaves);
		ExtractLeaves(node->Childs[1], leaves);
	}
	else
	{
		leaves.push_back(node);
	}
}

#pragma endregion Dbvt
