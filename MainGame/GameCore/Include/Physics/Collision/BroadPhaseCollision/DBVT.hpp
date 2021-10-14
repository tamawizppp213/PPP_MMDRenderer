//////////////////////////////////////////////////////////////////////////////////
//            @file　　  BroadPhaseProxy.hpp
//            @brief    Broad phase collision detection object interface  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partially edit by Toide
//            @date  : 2021_09_18
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
#ifndef DYNAMIC_BOUNDING_VOLUME_TREE_HPP
#define DYNAMIC_BOUNDING_VOLUME_TREE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../../../../../GameMath/Include/GMAlignedAllocatorArray.hpp"
#include "../../../../../GameMath/Include/GMTransform.hpp"
#include "../../Util/TransformUtil.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct DbvtAABBMm;
struct DbvtNode;

using DbvtVolume = DbvtAABBMm;
using NodeStack  = gm::AlignedObjectArray<const DbvtNode*>;
#define USE_SSE 1

#define DBVT_IMPL_GENERIC (0) // Generic implementation
#define DBVT_IMPL_SSE     (1) // SSE
#define DBVT_USE_INTRINSIC_SSE (1) // use only intrinsics instead of inline asm
#define DBVT_USE_MEMMOVE       (1) // use memmov for collide ocl
#define DBVT_ENABLE_BENCHMARK (0) // enable benchmarking code

#pragma region Define 
#ifdef _WIN32
#if (defined(_MSE_VER) && _MSC_VER>= 1400)
#define DBVT_USE_TEMPLATE 1
#else
#define DBVT_USE_TEMPLATE 0
#endif
#else
#define DBVT_USE_TEMPLATE 0
#endif

#if defined(USE_SSE)
#define DBVT_SELECT_IMPL DBVT_IMPL_SSE
#define DBVT_MERGE_IMPL  DBVT_IMPL_SSE
#define DBVT_INT0_IMPL   DBVT_IMPL_SSE
#else
#define DBVT_SELECT_IMPL DBVT_IMPL_GENERIC
#define DBVT_MERGE_IMPL  DBVT_IMPL_GENERIC
#define DBVT_INT0_IMPL   DBVT_IMPL_GENERIC
#endif

#if (DBVT_SELECT_IMPL == DBVT_IMPL_SSE) || \
	(DBVT_MERGE_IMPL  == DBVT_IMPL_SSE) ||  \
	(DBVT_INT0_IMPL   == DBVT_IMPL_SSE)
#include <emmintrin.h>
#endif

#if DBVT_USE_TEMPLATE
#define DBVT_VIRTUAL
#define DBVT_VIRTUAL_DESTRUCTOR(a)
#define DBVT_PREFIX template <typename T>
#define DBVT_IPOLICY T& policy
#define DBVT_CHECKTYPE                           \
	static const ICollide& typechecker = *(T*)1; \
	(void)typechecker;
#else
#define DBVT_VIRTUAL_DESTRUCTOR(a) \
	virtual ~a() {}
#define DBVT_VIRTUAL virtual
#define DBVT_PREFIX
#define DBVT_IPOLICY ICollide& policy
#define DBVT_CHECKTYPE
#endif

#if DBVT_USE_MEMMOVE
#if !defined(__CELLOS_LV2__) && !defined(__MWERKS__)
#include <memory>
#endif
#include <string>
#endif

#ifndef DBVT_USE_TEMPLATE
#error "DBVT_USE_TEMPLATE undefined"
#endif
#ifndef DBVT_USE_MEMMOVE
#error "DBVT_USE_MEMMOVE undefined"
#endif
#ifndef DBVT_ENABLE_BENCHMARK
#error "DBVT_ENABLE_BENCHMARK undefined"
#endif
#ifndef DBVT_SELECT_IMPL
#error "DBVT_SELECT_IMPL undefined"
#endif
#ifndef DBVT_MERGE_IMPL
#error "DBVT_MERGE_IMPL undefined"
#endif
#ifndef DBVT_INT0_IMPL
#error "DBVT_INT0_IMPL undefined"
#endif
#pragma endregion Define
//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			DbvtAABBMm
*************************************************************************//**
*  @class     DbvtAABBM
*  @brief     Default volumes
*****************************************************************************/
struct DbvtAABBMm
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	INLINE gm::Vector3 Center () const { return ((mi + mx) / 2); }
	INLINE gm::Vector3 Lengths() const { return (mx - mi); }
	INLINE gm::Vector3 Extensts() const { return ((mx - mi) / 2); }
	INLINE const gm::Vector3& Mins() const { return mi; }
	INLINE const gm::Vector3& Maxs() const { return mx; }
	INLINE void  Expand(const gm::Vector3& expand);
	INLINE void  SignedExpand(const gm::Vector3& e);
	INLINE bool  Contain(const DbvtAABBMm& a) const;
	INLINE int   Classify(const gm::Vector3& n, float p, int s) const;
	INLINE float ProjectMinimum(const gm::Vector3& v, unsigned signs) const;
	INLINE gm::Vector3& tMins() { return mi; }
	INLINE gm::Vector3& tMaxs() { return mx; }

	INLINE friend bool Intersect(const DbvtAABBMm& a, const DbvtAABBMm& b);
	INLINE friend bool Intersect(const DbvtAABBMm& a, const gm::Vector3& b);
	INLINE friend float Proximity(const DbvtAABBMm& a, const DbvtAABBMm& b);
	INLINE friend int  Select(const DbvtAABBMm& o, const DbvtAABBMm& a, const DbvtAABBMm& b);
	INLINE friend void Merge(const DbvtAABBMm& a, const DbvtAABBMm& b, const DbvtAABBMm& r);
	INLINE friend bool NotEqual(const DbvtAABBMm& a, const DbvtAABBMm& b);

	static inline DbvtAABBMm FromCE    (const gm::Vector3& c, const gm::Vector3& e);
	static inline DbvtAABBMm FromCR    (const gm::Vector3& c, float r);
	static inline DbvtAABBMm FromMM    (const gm::Vector3& mi, const gm::Vector3& mx);
	static inline DbvtAABBMm FromPoints(const gm::Vector3* points, int n);
	static inline DbvtAABBMm FromPoints(const gm::Vector3** pPoints, int n);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3 mi, mx;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	INLINE DbvtAABBMm() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	INLINE void AddSpan(const gm::Vector3& d, float& smi, float& smx) const;

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

};

/****************************************************************************
*				  			DbvtNode
*************************************************************************//**
*  @class     DbvtNode
*  @brief     Node
*****************************************************************************/
struct DbvtNode
{
	DbvtVolume Volume;
	DbvtNode*  Parent;
	INLINE bool IsLeaf    () const { return (Childs[1] == 0); }
	INLINE bool IsInternal() const { return (!IsLeaf()); };
	union
	{
		DbvtNode* Childs[2];
		void* Data;
		int   DataAsInt;
	};
};

/****************************************************************************
*				  			DbvtNode
*************************************************************************//**
*  @class     DbvntNode
*  @brief     Dbv(normal)tNode
*****************************************************************************/
struct DbvntNode
{
	DbvtVolume Volume;
	gm::Vector3 Normal;
	float       Angle;
	INLINE bool IsLeaf    () const { return (Childs[1] == 0); }
	INLINE bool IsInternal() const { return (!IsLeaf()); };
	DbvntNode* Childs[2];
	void* Data;

	DbvntNode(const DbvtNode* node)
	{
		Volume = node->Volume;
		Normal = gm::Vector3(0, 0, 0);
		Angle  = 0;
		Data   = node->Data;
		Childs[0] = nullptr;
		Childs[1] = nullptr;
	}

	~DbvntNode()
	{
		if (Childs[0]) { delete Childs[0]; }
		if (Childs[1]) { delete Childs[1]; }
	}
};

/****************************************************************************
*				  			Dbvt
*************************************************************************//**
*  @class     Dbvt
*  @brief     Dbvt class implements a fast dynamic bounding volume tree based on axis aligned bounding boxes.
*             This class is used for soft body collision detection and for the Dbvt BroadPhase.
*             It has a fast insert, remove and update of nodes.
*             Unlike the QuantizedBvh, nodes can be dynamically moved around, which allows for change in topology of the underlying data structure.
*****************************************************************************/
struct Dbvt
{
	struct StackNN
	{
		const DbvtNode* A;
		const DbvtNode* B;
		StackNN() = default;
		StackNN(const DbvtNode* nodeA, const DbvtNode* nodeB)
		{
			A = nodeA;
			B = nodeB;
		}
	};
	struct StackNP
	{
		const DbvtNode* Node;
		int Mask;
		StackNP(const DbvtNode* node, unsigned m)
		{
			Node = node;
			Mask = m;
		}
	};
	struct StackNPS
	{
		const DbvtNode* Node;
		int Mask;
		float Value;
		StackNPS() = default;
		StackNPS(const DbvtNode* node, unsigned mask, float value)
		{
			Node = node;
			Mask = mask;
			Value = value;
		}
	};
	struct StackCLN
	{
		const DbvtNode* Node;
		DbvtNode* Parent;
		StackCLN(const DbvtNode* node, DbvtNode* parent)
		{
			Node = node;
			Parent = parent;
		}
	};
	struct StacknNN
	{
		const DbvntNode* A;
		const DbvntNode* B;
		StacknNN() = default;
		StacknNN(const DbvntNode* nodeA, const DbvntNode* nodeB)
		{
			A = nodeA;
			B = nodeB;
		}
	};

	struct ICollide
	{
		DBVT_VIRTUAL_DESTRUCTOR(ICollide);
		DBVT_VIRTUAL void Process(const DbvtNode*, const DbvtNode*) {}
		DBVT_VIRTUAL void Process(const DbvtNode*) {}
		DBVT_VIRTUAL void Process(const DbvtNode* n, float) { Process(n); }
		DBVT_VIRTUAL void Process(const DbvntNode*, const DbvntNode*) {}
		DBVT_VIRTUAL bool Descent(const DbvtNode*) { return (true); }
		DBVT_VIRTUAL bool AllLeaves(const DbvtNode*) { return (true); }
	};

	/* IWriter	*/
	struct IWriter
	{
		virtual ~IWriter() {}
		virtual void Prepare(const DbvtNode* root, int numnodes) = 0;
		virtual void WriteNode(const DbvtNode*, int index, int parent, int child0, int child1) = 0;
		virtual void WriteLeaf(const DbvtNode*, int index, int parent) = 0;
	};
	/* IClone	*/
	struct IClone
	{
		virtual ~IClone() {}
		virtual void CloneLeaf(DbvtNode*) {}
	};

	enum
	{
		Simple_StackSize = 64,
		Double_StackSize = Simple_StackSize * 2
	};
public:
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	DbvtNode* Root;
	DbvtNode* Free;
	int Lkhd;
	int Leaves;
	unsigned Opath;
	gm::AlignedObjectArray<StackNN> Stack;

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Clear();
	bool Empty() const { return (0 == Root); }
	void OptimizeBottomUp();
	void OptimizeTopDown(int bu_threshold = 128);
	void OptimizeIncremental(int passes);
	DbvtNode* Insert(const DbvtVolume& box, void* data);
	void Update(DbvtNode* leaf, int lookahead = -1);
	void Update(DbvtNode* leaf, DbvtVolume& volume);
	bool Update(DbvtNode* leaf, DbvtVolume& volume, const gm::Vector3& velocity, float margin);
	bool Update(DbvtNode* leaf, DbvtVolume& volume, const gm::Vector3& velocity);
	bool Update(DbvtNode* leaf, DbvtVolume& volume, float margin);
	void Remove(DbvtNode* leaf);
	void Write(IWriter* iwriter) const;
	void Clone(Dbvt& dest, IClone* iclone = 0) const;
	static int  MaxDepth(const DbvtNode* node);
	static int  CountLeaves(const DbvtNode* node);
	static void ExtractLeaves(const DbvtNode* node, gm::AlignedObjectArray<const DbvtNode*>& leaves);

	// DBVT_IPOLICY must support ICollide policy/interface
	DBVT_PREFIX
		static void EnumNodes(const DbvtNode* root, DBVT_IPOLICY);
	DBVT_PREFIX
		static void EnumLeaves(const DbvtNode* root, DBVT_IPOLICY);
	DBVT_PREFIX
		void CollideTT(const DbvtNode* root0, const DbvtNode* root1, DBVT_IPOLICY);
	DBVT_PREFIX
		void SelfCollideT(const DbvntNode* root, DBVT_IPOLICY);
	DBVT_PREFIX
		void SelfCollideTT(const DbvtNode* root, DBVT_IPOLICY);
	DBVT_PREFIX
		void CollideTTpersistentStack(const DbvtNode* root0, const DbvtNode* root1, DBVT_IPOLICY);
	DBVT_PREFIX
		void CollideTV(const DbvtNode* root, const DbvtVolume& volume, DBVT_IPOLICY) const;
	DBVT_PREFIX
		void CollideTVNoStackAlloc(const DbvtNode* root, const DbvtVolume& volume, NodeStack& stack, DBVT_IPOLICY) const;

	///rayTest is a re-entrant ray test, and can be called in parallel as long as the btAlignedAlloc is thread-safe (uses locking etc)
	///rayTest is slower than rayTestInternal, because it builds a local stack, using memory allocations, and it recomputes signs/rayDirectionInverses each time
	DBVT_PREFIX
		static void RayTest(const DbvtNode* root, const gm::Vector3& rayFrom, const gm::Vector3& rayTo, DBVT_IPOLICY);
	///rayTestInternal is faster than rayTest, because it uses a persistent stack (to reduce dynamic memory allocations to a minimum) and it uses precomputed signs/rayInverseDirections
	///rayTestInternal is used by btDbvtBroadphase to accelerate world ray casts
	DBVT_PREFIX
		void RayTestInternal(const DbvtNode* root,
			const gm::Vector3& rayFrom,
			const gm::Vector3& rayTo,
			const gm::Vector3& rayDirectionInverse,
			unsigned int signs[3],
			float lambda_max,
			const gm::Vector3& aabbMin,
			const gm::Vector3& aabbMax,
			gm::AlignedObjectArray<const DbvtNode*>& stack,
			DBVT_IPOLICY) const;

	DBVT_PREFIX
		static void CollideKDOP(const DbvtNode* root,
			const gm::Vector3* normals,
			const float* offsets,
			int count,
			DBVT_IPOLICY);
	DBVT_PREFIX
		static void CollideOCL(const DbvtNode* root,
			const gm::Vector3* normals,
			const float* offsets,
			const gm::Vector3& sortaxis,
			int count,
			DBVT_IPOLICY,
			bool fullsort = true);
	DBVT_PREFIX
		static void CollideTU(const DbvtNode* root, DBVT_IPOLICY);

#if DBVT_ENABLE_BENCHMARK
	static void BenchMark();
#else
	static void BenchMark() {};
#endif

	static INLINE int Nearest(const int* i, const Dbvt::StackNPS* a, float v, int l, int h)
	{
		int m = 0;
		while (l < h)
		{
			m = (l + h) >> 1;
			if (a[i[m]].Value >= v)
				l = m + 1;
			else
				h = m;
		}
		return (h);
	}

	static INLINE int Allocate(gm::AlignedObjectArray<int>& ifree,
		gm::AlignedObjectArray<StackNPS>& stock, const StackNPS& value)
	{
		int i;
		if (ifree.size() > 0)
		{
			i = ifree[ifree.size() - 1];
			ifree.pop_back();
			stock[i] = value;
		}
		else
		{
			i = stock.size();
			stock.push_back(value);
		}
		return (i);
	}
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Dbvt();
	~Dbvt();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	Dbvt(const Dbvt&) {};

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};


inline DbvtAABBMm  DbvtAABBMm::FromCE(const gm::Vector3& c, const gm::Vector3& e)
{
	DbvtAABBMm box;
	box.mi = c - e;
	box.mx = c + e;
	return box;
}
inline DbvtAABBMm DbvtAABBMm::FromCR(const gm::Vector3& c, float r)
{
	return (FromCE(c, gm::Vector3(r, r, r)));
}
inline DbvtAABBMm DbvtAABBMm::FromMM(const gm::Vector3& mi, const gm::Vector3& mx)
{
	DbvtAABBMm box;
	box.mi = mi;
	box.mx = mx;
	return box;
}
inline DbvtAABBMm DbvtAABBMm::FromPoints(const gm::Vector3* points, int n)
{
	DbvtAABBMm box;
	box.mi = box.mx = points[0];
	for (int i = 1; i < n; ++i)
	{
		box.mi = gm::Min(points[i], box.mi);
		box.mx = gm::Max(points[i], box.mx);
	}
	return box;
}
inline DbvtAABBMm DbvtAABBMm::FromPoints(const gm::Vector3** pPoints, int n)
{
	DbvtAABBMm box;
	box.mi = box.mx = *pPoints[0];
	for (int i = 1; i < n; ++i)
	{
		box.mi = gm::Min(box.mi, *pPoints[i]);
		box.mx = gm::Max(box.mx, *pPoints[i]);
	}
}

INLINE void DbvtAABBMm::Expand(const gm::Vector3& expand)
{
	mi -= expand;
	mx += expand;
}

INLINE void DbvtAABBMm::SignedExpand(const gm::Vector3& e)
{
	if (e.GetX() > 0.0f) { mx.SetX(mx.GetX() + e.GetX()); }
	else                 { mi.SetX(mi.GetX() + e.GetX()); }

	if (e.GetY() > 0.0f) { mx.SetY(mx.GetY() + e.GetY()); }
	else { mi.SetY(mi.GetY() + e.GetY()); }

	if (e.GetZ() > 0.0f) { mx.SetZ(mx.GetZ() + e.GetZ()); }
	else                 { mi.SetZ(mi.GetZ() + e.GetZ()); }
}

INLINE bool DbvtAABBMm::Contain(const DbvtAABBMm& a) const
{
	return  
		(((float)mi.GetX() <= (float)a.mi.GetX()) &&
		((float)mi.GetY()  <= (float)a.mi.GetY()) &&
		((float)mi.GetZ()  <= (float)a.mi.GetZ()) &&
		((float)mx.GetX()  >= (float)a.mx.GetX()) &&
		((float)mx.GetY()  >= (float)a.mx.GetY()) &&
		((float)mx.GetZ()  >= (float)a.mx.GetZ()));
}

//
INLINE int DbvtAABBMm::Classify(const gm::Vector3& n, float o, int s) const
{
	gm::Vector3 pi, px;
	switch (s)
	{
	case (0 + 0 + 0):
		px = gm::Vector3(mi.GetX(), mi.GetY(), mi.GetZ());
		pi = gm::Vector3(mx.GetX(), mx.GetY(), mx.GetZ());
		break;
	case (1 + 0 + 0):
		px = gm::Vector3(mx.GetX(), mi.GetY(), mi.GetZ());
		pi = gm::Vector3(mi.GetX(), mx.GetY(), mx.GetZ());
		break;
	case (0 + 2 + 0):
		px = gm::Vector3(mi.GetX(), mx.GetY(), mi.GetZ());
		pi = gm::Vector3(mx.GetX(), mi.GetY(), mx.GetZ());
		break;
	case (1 + 2 + 0):
		px = gm::Vector3(mx.GetX(), mx.GetY(), mi.GetZ());
		pi = gm::Vector3(mi.GetX(), mi.GetY(), mx.GetZ());
		break;
	case (0 + 0 + 4):
		px = gm::Vector3(mi.GetX(), mi.GetY(), mx.GetZ());
		pi = gm::Vector3(mx.GetX(), mx.GetY(), mi.GetZ());
		break;
	case (1 + 0 + 4):
		px = gm::Vector3(mx.GetX(), mi.GetY(), mx.GetZ());
		pi = gm::Vector3(mi.GetX(), mx.GetY(), mi.GetZ());
		break;
	case (0 + 2 + 4):
		px = gm::Vector3(mi.GetX(), mx.GetY(), mx.GetZ());
		pi = gm::Vector3(mx.GetX(), mi.GetY(), mi.GetZ());
		break;
	case (1 + 2 + 4):
		px = gm::Vector3(mx.GetX(), mx.GetY(), mx.GetZ());
		pi = gm::Vector3(mi.GetX(), mi.GetY(), mi.GetZ());
		break;
	}
	if ((Dot(n, px) + o) < 0.0f)  return (-1);
	if ((Dot(n, pi) + o) >= 0.0f) return (+1);
	return (0);
}

//
INLINE float DbvtAABBMm::ProjectMinimum(const gm::Vector3& v, unsigned signs) const
{
	const gm::Vector3* b[] = { &mx, &mi };
	const gm::Vector3 p(b[(signs >> 0) & 1]->GetX(),
		b[(signs >> 1) & 1]->GetY(),
		b[(signs >> 2) & 1]->GetZ());
	return (Dot(p, v));
}

INLINE void DbvtAABBMm::AddSpan(const gm::Vector3& d, float& smi, float& smx) const
{
	for (int i = 0; i < 3; ++i)
	{
		if (d.GetElement((gm::Vector3Element)i) < 0.0f)
		{
			smi += mx.GetElement((gm::Vector3Element)i) * d.GetElement((gm::Vector3Element)i);
			smx += mi.GetElement((gm::Vector3Element)i) * d.GetElement((gm::Vector3Element)i);
		}
		else
		{
			smi += mi.GetElement((gm::Vector3Element)i) * d.GetElement((gm::Vector3Element)i);
			smx += mx.GetElement((gm::Vector3Element)i) * d.GetElement((gm::Vector3Element)i);
		}
	}
}

//
INLINE bool Intersect(const DbvtAABBMm& a, const DbvtAABBMm& b)
{
#if DBVT_INT0_IMPL == DBVT_IMPL_SSE
	const __m128 rt(_mm_or_ps(_mm_cmplt_ps(b.mx, a.mi),
		_mm_cmplt_ps(a.mx, b.mi)));
#if defined(_WIN32)
	const __int32* pu((const __int32*)&rt);
#else
	const int* pu((const int*)&rt);
#endif
	return ((pu[0] | pu[1] | pu[2]) == 0);
#else
	return (((float)a.mi.GetX() <= (float)b.mx.GetX()) &&
		((float)a.mx.GetX() >= b.mi.GetX()) &&
		((float)a.mi.GetY() <= b.mx.GetY()) &&
		((float)a.mx.GetY() >= b.mi.GetY()) &&
		((float)a.mi.GetZ() <= b.mx.GetZ()) &&
		((float)a.mx.GetZ() >= b.mi.GetZ()));
#endif
}

//
INLINE bool Intersect(const DbvtAABBMm& a, const gm::Vector3& b)
{
	return (((float)b.GetX() >= a.mi.GetX()) &&
		((float)b.GetY() >= a.mi.GetY()) &&
		((float)b.GetZ() >= a.mi.GetZ()) &&
		((float)b.GetX() <= a.mx.GetX()) &&
		((float)b.GetY() <= a.mx.GetY()) &&
		((float)b.GetZ() <= a.mx.GetZ()));
}

//////////////////////////////////////

//
INLINE float Proximity(const DbvtAABBMm& a, const DbvtAABBMm& b)
{
	const gm::Vector3 d = (a.mi + a.mx) - (b.mi + b.mx);
	return (gm::Abs(d.GetX()) + gm::Abs(d.GetY()) + gm::Abs(d.GetZ()));
}

//
INLINE int Select(const DbvtAABBMm& o, const DbvtAABBMm& a, const DbvtAABBMm& b)
{
#if DBVT_SELECT_IMPL == DBVT_IMPL_SSE

#if defined(_WIN32)
	static ALIGNED16(const unsigned __int32) mask[] = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
#else
	static ALIGNED16(const unsigned int) mask[] = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x00000000 /*0x7fffffff*/ };
#endif
	///@todo: the intrinsic version is 11% slower
#if DBVT_USE_INTRINSIC_SSE

	union btSSEUnion  ///NOTE: if we use more intrinsics, move btSSEUnion into the LinearMath directory
	{
		__m128 ssereg;
		float floats[4];
		int ints[4];
	};

	(__m128)o.mi = _mm_add_ps(o.mi, o.mx);
	(__m128)a.mi = _mm_add_ps(a.mi, a.mx);
	(__m128)a.mi = _mm_sub_ps(a.mi, o.mi);
	(__m128)a.mi = _mm_and_ps(a.mi, _mm_load_ps((const float*)mask));
	(__m128)b.mi = _mm_add_ps(b.mi, b.mx);
	(__m128)b.mi = _mm_sub_ps(b.mi, o.mi);
	(__m128)b.mi = _mm_and_ps(b.mi, _mm_load_ps((const float*)mask));
	__m128 t0(_mm_movehl_ps(a.mi, a.mi));
	(__m128)a.mi = _mm_add_ps(a.mi, t0);
	(__m128)a.mi = _mm_add_ss(a.mi, _mm_shuffle_ps(a.mi, a.mi, 1));
	__m128 t1(_mm_movehl_ps(b.mi, b.mi));
	(__m128)b.mi = _mm_add_ps(b.mi, t1);
	(__m128)b.mi = _mm_add_ss(b.mi, _mm_shuffle_ps(b.mi, b.mi, 1));

	btSSEUnion tmp;
	tmp.ssereg = _mm_cmple_ss(b.mi, a.mi);
	return tmp.ints[0] & 1;

#else
	ALIGNED16(__int32 r[1]);
	__asm
	{
		mov		eax, o
		mov		ecx, a
		mov		edx, b
		movaps	xmm0, [eax]
		movaps	xmm5, mask
		addps	xmm0, [eax + 16]
		movaps	xmm1, [ecx]
		movaps	xmm2, [edx]
		addps	xmm1, [ecx + 16]
		addps	xmm2, [edx + 16]
		subps	xmm1, xmm0
		subps	xmm2, xmm0
		andps	xmm1, xmm5
		andps	xmm2, xmm5
		movhlps	xmm3, xmm1
		movhlps	xmm4, xmm2
		addps	xmm1, xmm3
		addps	xmm2, xmm4
		pshufd	xmm3, xmm1, 1
		pshufd	xmm4, xmm2, 1
		addss	xmm1, xmm3
		addss	xmm2, xmm4
		cmpless	xmm2, xmm1
		movss	r, xmm2
	}
	return (r[0] & 1);
#endif
#else
	return (Proximity(o, a) < Proximity(o, b) ? 0 : 1);
#endif
}

//
INLINE void Merge(const DbvtAABBMm& a, const DbvtAABBMm& b, DbvtAABBMm& r)
{
#if DBVT_MERGE_IMPL == DBVT_IMPL_SSE

	(__m128)r.mi = _mm_min_ps(a.mi, b.mi);
	(__m128)r.mx = _mm_max_ps(a.mx, b.mx);

#else
	for (int i = 0; i < 3; ++i)
	{
		if ((float)a.mi.GetElement((gm::Vector3Element)i) < (float)b.mi.GetElement((gm::Vector3Element)i))
			r.mi.GetElement((gm::Vector3Element)i) = (float)a.mi.GetElement((gm::Vector3Element)i);
		else
			r.mi.GetElement((gm::Vector3Element)i) = (float)b.mi.GetElement((gm::Vector3Element)i);
		if ((float)a.mx.GetElement((gm::Vector3Element)i) > (float)b.mx.GetElement((gm::Vector3Element)i))
			r.mx.GetElement((gm::Vector3Element)i) = (float)a.mx.GetElement((gm::Vector3Element)i);
		else
			r.mx.GetElement((gm::Vector3Element)i) = (float)b.mx.GetElement((gm::Vector3Element)i);
	}
#endif
}

//
INLINE bool NotEqual(const DbvtAABBMm& a,
	const DbvtAABBMm& b)
{
	return (((float)a.mi.GetX() != b.mi.GetX()) ||
		((float)a.mi.GetY() != b.mi.GetY()) ||
		((float)a.mi.GetZ() != b.mi.GetZ()) ||
		((float)a.mx.GetX() != b.mx.GetX()) ||
		((float)a.mx.GetY() != b.mx.GetY()) ||
		((float)a.mx.GetZ() != b.mx.GetZ()));
}

//
// Inline's
//

//
DBVT_PREFIX
inline void Dbvt::EnumNodes(const DbvtNode* root, DBVT_IPOLICY)
{
	DBVT_CHECKTYPE
		policy.Process(root);
	if (root->IsInternal())
	{
		EnumNodes(root->Childs[0], policy);
		EnumNodes(root->Childs[1], policy);
	}
}

//
DBVT_PREFIX
inline void Dbvt::EnumLeaves(const DbvtNode* root,
	DBVT_IPOLICY)
{
	DBVT_CHECKTYPE
		if (root->IsInternal())
		{
			EnumLeaves(root->Childs[0], policy);
			EnumLeaves(root->Childs[1], policy);
		}
		else
		{
			policy.Process(root);
		}
}

//
DBVT_PREFIX
inline void Dbvt::CollideTT(const DbvtNode* root0, const DbvtNode* root1, DBVT_IPOLICY)
{
	DBVT_CHECKTYPE
		if (root0 && root1)
		{
			int depth = 1;
			int treshold = Double_StackSize- 4;
			gm::AlignedObjectArray<StackNN> stkStack;
			stkStack.resize(Double_StackSize);
			stkStack[0] = StackNN(root0, root1);
			do
			{
				StackNN p = stkStack[--depth];
				if (depth > treshold)
				{
					stkStack.resize(stkStack.size() * 2);
					treshold = stkStack.size() - 4;
				}
				if (p.A == p.B)
				{
					if (p.A->IsInternal())
					{
						stkStack[depth++] = StackNN(p.A->Childs[0], p.A->Childs[0]);
						stkStack[depth++] = StackNN(p.A->Childs[1], p.A->Childs[1]);
						stkStack[depth++] = StackNN(p.A->Childs[0], p.A->Childs[1]);
					}
				}
				else if (Intersect(p.A->Volume, p.B->Volume))
				{
					if (p.A->IsInternal())
					{
						if (p.B->IsInternal())
						{
							stkStack[depth++] = StackNN(p.A->Childs[0], p.B->Childs[0]);
							stkStack[depth++] = StackNN(p.A->Childs[1], p.B->Childs[0]);
							stkStack[depth++] = StackNN(p.A->Childs[0], p.B->Childs[1]);
							stkStack[depth++] = StackNN(p.A->Childs[1], p.B->Childs[1]);
						}
						else
						{
							stkStack[depth++] = StackNN(p.A->Childs[0], p.B);
							stkStack[depth++] = StackNN(p.A->Childs[1], p.B);
						}
					}
					else
					{
						if (p.B->IsInternal())
						{
							stkStack[depth++] = StackNN(p.A, p.B->Childs[0]);
							stkStack[depth++] = StackNN(p.A, p.B->Childs[1]);
						}
						else
						{
							policy.Process(p.A, p.B);
						}
					}
				}
			} while (depth);
		}
}

//
DBVT_PREFIX
inline void Dbvt::SelfCollideT(const DbvntNode* root, DBVT_IPOLICY)
{
	DBVT_CHECKTYPE
		if (root)
		{
			int depth = 1;
			int treshold = Double_StackSize - 4;
			gm::AlignedObjectArray<StacknNN> stkStack;
			stkStack.resize(Double_StackSize);
			stkStack[0] = StacknNN(root, root);
			do
			{
				StacknNN p = stkStack[--depth];
				if (depth > treshold)
				{
					stkStack.resize(stkStack.size() * 2);
					treshold = stkStack.size() - 4;
				}
				if (p.A == p.B)
				{
					if (p.A->IsInternal() && p.A->Angle > gm::GM_PI)
					{
						stkStack[depth++] = StacknNN(p.A->Childs[0], p.A->Childs[0]);
						stkStack[depth++] = StacknNN(p.A->Childs[1], p.A->Childs[1]);
						stkStack[depth++] = StacknNN(p.A->Childs[0], p.A->Childs[1]);
					}
				}
				else if (Intersect(p.A->Volume, p.B->Volume))
				{
					if (p.A->IsInternal())
					{
						if (p.B->IsInternal())
						{
							stkStack[depth++] = StacknNN(p.A->Childs[0], p.B->Childs[0]);
							stkStack[depth++] = StacknNN(p.A->Childs[1], p.B->Childs[0]);
							stkStack[depth++] = StacknNN(p.A->Childs[0], p.B->Childs[1]);
							stkStack[depth++] = StacknNN(p.A->Childs[1], p.B->Childs[1]);
						}
						else
						{
							stkStack[depth++] = StacknNN(p.A->Childs[0], p.B);
							stkStack[depth++] = StacknNN(p.A->Childs[1], p.B);
						}
					}
					else
					{
						if (p.B->IsInternal())
						{
							stkStack[depth++] = StacknNN(p.A, p.B->Childs[0]);
							stkStack[depth++] = StacknNN(p.A, p.B->Childs[1]);
						}
						else
						{
							policy.Process(p.A, p.B);
						}
					}
				}
			} while (depth);
		}
}

//
DBVT_PREFIX
inline void Dbvt::SelfCollideTT(const DbvtNode* root, DBVT_IPOLICY)
{
	DBVT_CHECKTYPE
		if (root)
		{
			int depth = 1;
			int treshold = Double_StackSize - 4;
			gm::AlignedObjectArray<StackNN> stkStack;
			stkStack.resize(Double_StackSize);
			stkStack[0] = StackNN(root, root);
			do
			{
				StackNN p = stkStack[--depth];
				if (depth > treshold)
				{
					stkStack.resize(stkStack.size() * 2);
					treshold = stkStack.size() - 4;
				}
				if (p.A == p.B)
				{
					if (p.A->IsInternal())
					{
						stkStack[depth++] = StackNN(p.A->Childs[0], p.A->Childs[0]);
						stkStack[depth++] = StackNN(p.A->Childs[1], p.A->Childs[1]);
						stkStack[depth++] = StackNN(p.A->Childs[0], p.A->Childs[1]);
					}
				}
				else if (Intersect(p.A->Volume, p.B->Volume))
				{
					if (p.A->IsInternal())
					{
						if (p.B->IsInternal())
						{
							stkStack[depth++] = StackNN(p.A->Childs[0], p.B->Childs[0]);
							stkStack[depth++] = StackNN(p.A->Childs[1], p.B->Childs[0]);
							stkStack[depth++] = StackNN(p.A->Childs[0], p.B->Childs[1]);
							stkStack[depth++] = StackNN(p.A->Childs[1], p.B->Childs[1]);
						}
						else
						{
							stkStack[depth++] = StackNN(p.A->Childs[0], p.B);
							stkStack[depth++] = StackNN(p.A->Childs[1], p.B);
						}
					}
					else
					{
						if (p.B->IsInternal())
						{
							stkStack[depth++] = StackNN(p.A, p.B->Childs[0]);
							stkStack[depth++] = StackNN(p.A, p.B->Childs[1]);
						}
						else
						{
							policy.Process(p.A, p.B);
						}
					}
				}
			} while (depth);
		}
}


DBVT_PREFIX
inline void Dbvt::CollideTTpersistentStack(const DbvtNode* root0, const DbvtNode* root1, DBVT_IPOLICY)
{
	DBVT_CHECKTYPE
		if (root0 && root1)
		{
			int depth = 1;
			int treshold = Double_StackSize - 4;

			Stack.resize(Double_StackSize);
			Stack[0] = StackNN(root0, root1);
			do
			{
				StackNN p = Stack[--depth];
				if (depth > treshold)
				{
					Stack.resize(Stack.size() * 2);
					treshold = Stack.size() - 4;
				}
				if (p.A == p.B)
				{
					if (p.A->IsInternal())
					{
						Stack[depth++] = StackNN(p.A->Childs[0], p.A->Childs[0]);
						Stack[depth++] = StackNN(p.A->Childs[1], p.A->Childs[1]);
						Stack[depth++] = StackNN(p.A->Childs[0], p.A->Childs[1]);
					}
				}
				else if (Intersect(p.A->Volume, p.B->Volume))
				{
					if (p.A->IsInternal())
					{
						if (p.B->IsInternal())
						{
							Stack[depth++] = StackNN(p.A->Childs[0], p.B->Childs[0]);
							Stack[depth++] = StackNN(p.A->Childs[1], p.B->Childs[0]);
							Stack[depth++] = StackNN(p.A->Childs[0], p.B->Childs[1]);
							Stack[depth++] = StackNN(p.A->Childs[1], p.B->Childs[1]);
						}
						else
						{
							Stack[depth++] = StackNN(p.A->Childs[0], p.B);
							Stack[depth++] = StackNN(p.A->Childs[1], p.B);
						}
					}
					else
					{
						if (p.B->IsInternal())
						{
							Stack[depth++] = StackNN(p.A, p.B->Childs[0]);
							Stack[depth++] = StackNN(p.A, p.B->Childs[1]);
						}
						else
						{
							policy.Process(p.A, p.B);
						}
					}
				}
			} while (depth);
		}
}



DBVT_PREFIX
inline void Dbvt::CollideTV(const DbvtNode* root, const DbvtVolume& vol, DBVT_IPOLICY) const
{
	DBVT_CHECKTYPE
		if (root)
		{
			ALIGNED16(DbvtVolume)
				volume(vol);
			gm::AlignedObjectArray<const DbvtNode*> stack;
			stack.resize(0);
#ifndef BT_DISABLE_STACK_TEMP_MEMORY
			char tempmemory[Simple_StackSize * sizeof(const DbvtNode*)];
			stack.InitializeFromBuffer(tempmemory, 0, Simple_StackSize);
#else
			stack.reserve(SIMPLE_STACKSIZE);
#endif  //BT_DISABLE_STACK_TEMP_MEMORY

			stack.push_back(root);
			do
			{
				const DbvtNode* n = stack[stack.size() - 1];
				stack.pop_back();
				if (Intersect(n->Volume, volume))
				{
					if (n->IsInternal())
					{
						stack.push_back(n->Childs[0]);
						stack.push_back(n->Childs[1]);
					}
					else
					{
						policy.Process(n);
					}
				}
			} while (stack.size() > 0);
		}
}

//
DBVT_PREFIX
inline void Dbvt::CollideTVNoStackAlloc(const DbvtNode* root, const DbvtVolume& vol, NodeStack& stack, DBVT_IPOLICY) const
{
	DBVT_CHECKTYPE
		if (root)
		{
			ALIGNED16(DbvtVolume)
				volume(vol);
			stack.resize(0);
			stack.reserve(Simple_StackSize);
			stack.push_back(root);
			do
			{
				const DbvtNode* n = stack[stack.size() - 1];
				stack.pop_back();
				if (Intersect(n->Volume, volume))
				{
					if (n->IsInternal())
					{
						stack.push_back(n->Childs[0]);
						stack.push_back(n->Childs[1]);
					}
					else
					{
						policy.Process(n);
					}
				}
			} while (stack.size() > 0);
		}
}

DBVT_PREFIX
inline void Dbvt::RayTestInternal(const DbvtNode* root,
	const gm::Vector3& rayFrom,
	const gm::Vector3& rayTo,
	const gm::Vector3& rayDirectionInverse,
	unsigned int signs[3],
	float lambda_max,
	const gm::Vector3& aabbMin,
	const gm::Vector3& aabbMax,
	gm::AlignedObjectArray<const DbvtNode*>& stack,
	DBVT_IPOLICY) const
{
	(void)rayTo;
	DBVT_CHECKTYPE
		if (root)
		{
			gm::Vector3 resultNormal;

			int depth = 1;
			int treshold = Double_StackSize- 2;
			stack.resize(Double_StackSize);
			stack[0] = root;
			gm::Vector3 bounds[2];
			do
			{
				const DbvtNode* node = stack[--depth];
				bounds[0] = node->Volume.Mins() - aabbMax;
				bounds[1] = node->Volume.Maxs() - aabbMin;
				float tmin = 1.f, lambda_min = 0.f;
				unsigned int result1 = false;
				result1 = RayAABB2(rayFrom, rayDirectionInverse, signs, bounds, tmin, lambda_min, lambda_max);
				if (result1)
				{
					if (node->IsInternal())
					{
						if (depth > treshold)
						{
							stack.resize(stack.size() * 2);
							treshold = stack.size() - 2;
						}
						stack[depth++] = node->Childs[0];
						stack[depth++] = node->Childs[1];
					}
					else
					{
						policy.Process(node);
					}
				}
			} while (depth);
		}
}

//
DBVT_PREFIX
inline void Dbvt::RayTest(const DbvtNode* root,
	const gm::Vector3& rayFrom,
	const gm::Vector3& rayTo,
	DBVT_IPOLICY)
{
	DBVT_CHECKTYPE
		if (root)
		{
			gm::Vector3 rayDir = (rayTo - rayFrom);
			Normalize(rayDir);

			///what about division by zero? --> just set rayDirection[i] to INF/BT_LARGE_FLOAT
			gm::Vector3 rayDirectionInverse;

			rayDirectionInverse.SetX(rayDir.GetX() == float(0.0) ? float(FLT_MAX) : float(1.0) / (float)rayDir.GetX());
			rayDirectionInverse.SetY(rayDir.GetY() == float(0.0) ? float(FLT_MAX) : float(1.0) / (float)rayDir.GetY());
			rayDirectionInverse.SetZ(rayDir.GetZ() == float(0.0) ? float(FLT_MAX) : float(1.0) / (float)rayDir.GetZ());
			unsigned int signs[3] = { (float)rayDirectionInverse.GetX() < 0.0, (float)rayDirectionInverse.GetY() < 0.0, (float)rayDirectionInverse.GetZ() < 0.0 };

			float lambda_max = Dot(rayDir, rayTo - rayFrom);

			gm::Vector3 resultNormal;

			gm::AlignedObjectArray<const DbvtNode*> stack;

			int depth = 1;
			int treshold = Double_StackSize - 2;

			char tempmemory[Double_StackSize * sizeof(const DbvtNode*)];
#ifndef BT_DISABLE_STACK_TEMP_MEMORY
			stack.InitializeFromBuffer(tempmemory, Double_StackSize, Double_StackSize);
#else   //BT_DISABLE_STACK_TEMP_MEMORY
			stack.resize(DOUBLE_STACKSIZE);
#endif  //BT_DISABLE_STACK_TEMP_MEMORY
			stack[0] = root;
			gm::Vector3 bounds[2];
			do
			{
				const DbvtNode* node = stack[--depth];

				bounds[0] = node->Volume.Mins();
				bounds[1] = node->Volume.Maxs();

				float tmin = 1.f, lambda_min = 0.f;
				unsigned int result1 = RayAABB2(rayFrom, rayDirectionInverse, signs, bounds, tmin, lambda_min, lambda_max);

#ifdef COMPARE_BTRAY_AABB2
				btScalar param = 1.f;
				bool result2 = btRayAabb(rayFrom, rayTo, node->volume.Mins(), node->volume.Maxs(), param, resultNormal);
				btAssert(result1 == result2);
#endif  //TEST_BTRAY_AABB2

				if (result1)
				{
					if (node->IsInternal())
					{
						if (depth > treshold)
						{
							stack.resize(stack.size() * 2);
							treshold = stack.size() - 2;
						}
						stack[depth++] = node->Childs[0];
						stack[depth++] = node->Childs[1];
					}
					else
					{
						policy.Process(node);
					}
				}
			} while (depth);
		}
}

//
DBVT_PREFIX
inline void Dbvt::CollideKDOP(const DbvtNode* root,
	const gm::Vector3* normals,
	const float* offsets,
	int count,
	DBVT_IPOLICY)
{
	DBVT_CHECKTYPE
		if (root)
		{
			const int inside = (1 << count) - 1;
			gm::AlignedObjectArray<StackNP> stack;
			int signs[sizeof(unsigned) * 8];
			assert(count < int(sizeof(signs) / sizeof(signs[0])));
			for (int i = 0; i < count; ++i)
			{
				signs[i] = (((float)normals[i].GetX() >= 0) ? 1 : 0) +
					(((float)normals[i].GetY() >= 0) ? 2 : 0) +
					(((float)normals[i].GetZ() >= 0) ? 4 : 0);
			}
			stack.reserve(Simple_StackSize);
			stack.push_back(StackNP(root, 0));
			do
			{
				StackNP se = stack[stack.size() - 1];
				bool out = false;
				stack.pop_back();
				for (int i = 0, j = 1; (!out) && (i < count); ++i, j <<= 1)
				{
					if (0 == (se.Mask & j))
					{
						const int side = se.Node->Volume.Classify(normals[i], offsets[i], signs[i]);
						switch (side)
						{
						case -1:
							out = true;
							break;
						case +1:
							se.Mask |= j;
							break;
						}
					}
				}
				if (!out)
				{
					if ((se.Mask != inside) && (se.Node->IsInternal()))
					{
						stack.push_back(StackNP(se.Node->Childs[0], se.Mask));
						stack.push_back(StackNP(se.Node->Childs[1], se.Mask));
					}
					else
					{
						if (policy.AllLeaves(se.Node)) EnumLeaves(se.Node, policy);
					}
				}
			} while (stack.size());
		}
}

//
DBVT_PREFIX
inline void Dbvt::CollideOCL(const DbvtNode* root,
	const gm::Vector3* normals,
	const float* offsets,
	const gm::Vector3& sortaxis,
	int count,
	DBVT_IPOLICY,
	bool fsort)
{
	DBVT_CHECKTYPE
		if (root)
		{
			const unsigned srtsgns = ((float)sortaxis.GetX() >= 0 ? 1 : 0) +
				((float)sortaxis.GetY() >= 0 ? 2 : 0) +
				((float)sortaxis.GetZ() >= 0 ? 4 : 0);
			const int inside = (1 << count) - 1;
			gm::AlignedObjectArray<StackNPS> stock;
			gm::AlignedObjectArray<int> ifree;
			gm::AlignedObjectArray<int> stack;
			int signs[sizeof(unsigned) * 8];
			assert(count < int(sizeof(signs) / sizeof(signs[0])));
			for (int i = 0; i < count; ++i)
			{
				signs[i] = (((float)normals[i].GetX() >= 0) ? 1 : 0) +
					(((float)normals[i].GetY() >= 0) ? 2 : 0) +
					(((float)normals[i].GetZ() >= 0) ? 4 : 0);
			}
			stock.reserve(Simple_StackSize);
			stack.reserve(Simple_StackSize);
			ifree.reserve(Simple_StackSize);
			stack.push_back(Allocate(ifree, stock, StackNPS(root, 0, root->Volume.ProjectMinimum(sortaxis, srtsgns))));
			do
			{
				const int id = stack[stack.size() - 1];
				StackNPS se = stock[id];
				stack.pop_back();
				ifree.push_back(id);
				if (se.Mask != inside)
				{
					bool out = false;
					for (int i = 0, j = 1; (!out) && (i < count); ++i, j <<= 1)
					{
						if (0 == (se.Mask & j))
						{
							const int side = se.Node->Volume.Classify(normals[i], offsets[i], signs[i]);
							switch (side)
							{
							case -1:
								out = true;
								break;
							case +1:
								se.Mask |= j;
								break;
							}
						}
					}
					if (out) continue;
				}
				if (policy.Descent(se.Node))
				{
					if (se.Node->IsInternal())
					{
						const DbvtNode* pns[] = { se.Node->Childs[0], se.Node->Childs[1] };
						StackNPS nes[] = { StackNPS(pns[0], se.Mask, pns[0]->Volume.ProjectMinimum(sortaxis, srtsgns)),
										 StackNPS(pns[1], se.Mask, pns[1]->Volume.ProjectMinimum(sortaxis, srtsgns)) };
						const int q = nes[0].Value < nes[1].Value ? 1 : 0;
						int j = stack.size();
						if (fsort && (j > 0))
						{
							/* Insert 0	*/
							j = Nearest(&stack[0], &stock[0], nes[q].Value, 0, stack.size());
							stack.push_back(0);

							//void * memmove ( void * destination, const void * source, size_t num );

#if DBVT_USE_MEMMOVE
							{
								int num_items_to_move = stack.size() - 1 - j;
								if (num_items_to_move > 0)
									memmove(&stack[j + 1], &stack[j], sizeof(int) * num_items_to_move);
							}
#else
							for (int k = stack.size() - 1; k > j; --k)
							{
								stack[k] = stack[k - 1];
							}
#endif
							stack[j] = Allocate(ifree, stock, nes[q]);
							/* Insert 1	*/
							j = Nearest(&stack[0], &stock[0], nes[1 - q].Value, j, stack.size());
							stack.push_back(0);
#if DBVT_USE_MEMMOVE
							{
								int num_items_to_move = stack.size() - 1 - j;
								if (num_items_to_move > 0)
									memmove(&stack[j + 1], &stack[j], sizeof(int) * num_items_to_move);
							}
#else
							for (int k = stack.size() - 1; k > j; --k)
							{
								stack[k] = stack[k - 1];
							}
#endif
							stack[j] = Allocate(ifree, stock, nes[1 - q]);
						}
						else
						{
							stack.push_back(Allocate(ifree, stock, nes[q]));
							stack.push_back(Allocate(ifree, stock, nes[1 - q]));
						}
					}
					else
					{
						policy.Process(se.Node, se.Value);
					}
				}
			} while (stack.size());
		}
}

//
DBVT_PREFIX
inline void Dbvt::CollideTU(const DbvtNode* root, DBVT_IPOLICY)
{
	DBVT_CHECKTYPE
	if (root)
	{
		gm::AlignedObjectArray<const DbvtNode*> stack;
		stack.reserve(Simple_StackSize);
		stack.push_back(root);
		do
		{
			const DbvtNode* n = stack[stack.size() - 1];
			stack.pop_back();
			if (policy.Descent(n))
			{
				if (n->IsInternal())
				{
					stack.push_back(n->Childs[0]);
					stack.push_back(n->Childs[1]);
				}
				else
				{
					policy.Process(n);
				}
			}
		} while (stack.size() > 0);
	}
}

#undef DBVT_USE_MEMMOVE
#undef DBVT_USE_TEMPLATE
#undef DBVT_VIRTUAL_DTOR
#undef DBVT_VIRTUAL
#undef DBVT_PREFIX
#undef DBVT_IPOLICY
#undef DBVT_CHECKTYPE
#undef DBVT_IMPL_GENERIC
#undef DBVT_IMPL_SSE
#undef DBVT_USE_INTRINSIC_SSE
#undef DBVT_SELECT_IMPL
#undef DBVT_MERGE_IMPL
#undef DBVT_INT0_IMPL
#endif