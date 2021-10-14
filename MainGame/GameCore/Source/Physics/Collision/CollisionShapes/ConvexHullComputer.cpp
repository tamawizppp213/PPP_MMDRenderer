//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexHullComputer.cpp
///             @brief  Convex hullComputer
///             @author Partially edit by Toide
///             @date   2021_09_22
//////////////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011 Ole Kniemeyer, MAXON, www.maxon.net

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
#include "GameCore/Include/Physics/Collision/CollisionShapes/ConvexHullComputer.hpp"
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"
#include "DirectX12/Include/Core/DirectX12BlendState.hpp"
#include <string.h>
#pragma warning(disable: 4244)
#pragma warning(disable: 4305)
#pragma warning(disable: 28182)
#pragma warning(disable: 6011)
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
using btVector3 = Vector3;
using btScalar   = Scalar;


//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUC__
#include <stdint.h>
#elif defined(_MSC_VER)
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
typedef int int32_t;
typedef long long int int64_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;
#endif

//The definition of USE_X86_64_ASM is moved into the build system. You can enable it manually by commenting out the following lines
//#if (defined(__GNUC__) && defined(__x86_64__) && !defined(__ICL))  // || (defined(__ICL) && defined(_M_X64))   bug in Intel compiler, disable inline assembly
//	#define USE_X86_64_ASM
//#endif

//#define DEBUG_CONVEX_HULL
//#define SHOW_ITERATIONS

#if defined(DEBUG_CONVEX_HULL) || defined(SHOW_ITERATIONS)
#include <stdio.h>
#endif

// Convex hull implementation based on Preparata and Hong
// Ole Kniemeyer, MAXON Computer GmbH
class btConvexHullInternal
{
public:
	class Point64
	{
	public:
		int64_t x;
		int64_t y;
		int64_t z;

		Point64(int64_t x, int64_t y, int64_t z) : x(x), y(y), z(z)
		{
		}

		bool isZero()
		{
			return (x == 0) && (y == 0) && (z == 0);
		}

		int64_t dot(const Point64& b) const
		{
			return x * b.x + y * b.y + z * b.z;
		}
	};

	class Point32
	{
	public:
		int32_t x;
		int32_t y;
		int32_t z;
		int index;

		Point32()
		{
		}

		Point32(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z), index(-1)
		{
		}

		bool operator==(const Point32& b) const
		{
			return (x == b.x) && (y == b.y) && (z == b.z);
		}

		bool operator!=(const Point32& b) const
		{
			return (x != b.x) || (y != b.y) || (z != b.z);
		}

		bool isZero()
		{
			return (x == 0) && (y == 0) && (z == 0);
		}

		Point64 cross(const Point32& b) const
		{
			return Point64(((int64_t)y) * b.z - ((int64_t)z) * b.y, ((int64_t)z) * b.x - ((int64_t)x) * b.z, ((int64_t)x) * b.y - ((int64_t)y) * b.x);
		}

		Point64 cross(const Point64& b) const
		{
			return Point64(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
		}

		int64_t dot(const Point32& b) const
		{
			return ((int64_t)x) * b.x + ((int64_t)y) * b.y + ((int64_t)z) * b.z;
		}

		int64_t dot(const Point64& b) const
		{
			return x * b.x + y * b.y + z * b.z;
		}

		Point32 operator+(const Point32& b) const
		{
			return Point32(x + b.x, y + b.y, z + b.z);
		}

		Point32 operator-(const Point32& b) const
		{
			return Point32(x - b.x, y - b.y, z - b.z);
		}
	};

	class Int128
	{
	public:
		uint64_t low;
		uint64_t high;

		Int128()
		{
		}

		Int128(uint64_t low, uint64_t high) : low(low), high(high)
		{
		}

		Int128(uint64_t low) : low(low), high(0)
		{
		}

		Int128(int64_t value) : low(value), high((value >= 0) ? 0 : (uint64_t)-1LL)
		{
		}

		static Int128 mul(int64_t a, int64_t b);

		static Int128 mul(uint64_t a, uint64_t b);

		Int128 operator-() const
		{
			return Int128((uint64_t)-(int64_t)low, ~high + (low == 0));
		}

		Int128 operator+(const Int128& b) const
		{
#ifdef USE_X86_64_ASM
			Int128 result;
			__asm__(
				"addq %[bl], %[rl]\n\t"
				"adcq %[bh], %[rh]\n\t"
				: [rl] "=r"(result.low), [rh] "=r"(result.high)
				: "0"(low), "1"(high), [bl] "g"(b.low), [bh] "g"(b.high)
				: "cc");
			return result;
#else
			uint64_t lo = low + b.low;
			return Int128(lo, high + b.high + (lo < low));
#endif
		}

		Int128 operator-(const Int128& b) const
		{
#ifdef USE_X86_64_ASM
			Int128 result;
			__asm__(
				"subq %[bl], %[rl]\n\t"
				"sbbq %[bh], %[rh]\n\t"
				: [rl] "=r"(result.low), [rh] "=r"(result.high)
				: "0"(low), "1"(high), [bl] "g"(b.low), [bh] "g"(b.high)
				: "cc");
			return result;
#else
			return *this + -b;
#endif
		}

		Int128& operator+=(const Int128& b)
		{
#ifdef USE_X86_64_ASM
			__asm__(
				"addq %[bl], %[rl]\n\t"
				"adcq %[bh], %[rh]\n\t"
				: [rl] "=r"(low), [rh] "=r"(high)
				: "0"(low), "1"(high), [bl] "g"(b.low), [bh] "g"(b.high)
				: "cc");
#else
			uint64_t lo = low + b.low;
			if (lo < low)
			{
				++high;
			}
			low = lo;
			high += b.high;
#endif
			return *this;
		}

		Int128& operator++()
		{
			if (++low == 0)
			{
				++high;
			}
			return *this;
		}

		Int128 operator*(int64_t b) const;

		Scalar toScalar() const
		{
			return ((int64_t)high >= 0) ? Scalar(high) * (Scalar(0x100000000LL) *Scalar(0x100000000LL)) + Scalar(low)
				: -(-*this).toScalar();
		}

		int getSign() const
		{
			return ((int64_t)high < 0) ? -1 : (high || low) ? 1 : 0;
		}

		bool operator<(const Int128& b) const
		{
			return (high < b.high) || ((high == b.high) && (low < b.low));
		}

		int ucmp(const Int128& b) const
		{
			if (high < b.high)
			{
				return -1;
			}
			if (high > b.high)
			{
				return 1;
			}
			if (low < b.low)
			{
				return -1;
			}
			if (low > b.low)
			{
				return 1;
			}
			return 0;
		}
	};

	class Rational64
	{
	private:
		uint64_t m_numerator;
		uint64_t m_denominator;
		int sign;

	public:
		Rational64(int64_t numerator, int64_t denominator)
		{
			if (numerator > 0)
			{
				sign = 1;
				m_numerator = (uint64_t)numerator;
			}
			else if (numerator < 0)
			{
				sign = -1;
				m_numerator = (uint64_t)-numerator;
			}
			else
			{
				sign = 0;
				m_numerator = 0;
			}
			if (denominator > 0)
			{
				m_denominator = (uint64_t)denominator;
			}
			else if (denominator < 0)
			{
				sign = -sign;
				m_denominator = (uint64_t)-denominator;
			}
			else
			{
				m_denominator = 0;
			}
		}

		bool isNegativeInfinity() const
		{
			return (sign < 0) && (m_denominator == 0);
		}

		bool isNaN() const
		{
			return (sign == 0) && (m_denominator == 0);
		}

		int compare(const Rational64& b) const;

		Scalar toScalar() const
		{
			return sign * ((m_denominator == 0) ? FLT_MAX : m_numerator / m_denominator);
		}
	};

	class Rational128
	{
	private:
		Int128 numerator;
		Int128 denominator;
		int sign;
		bool isInt64;

	public:
		Rational128(int64_t value)
		{
			if (value > 0)
			{
				sign = 1;
				this->numerator = value;
			}
			else if (value < 0)
			{
				sign = -1;
				this->numerator = -value;
			}
			else
			{
				sign = 0;
				this->numerator = (uint64_t)0;
			}
			this->denominator = (uint64_t)1;
			isInt64 = true;
		}

		Rational128(const Int128& numerator, const Int128& denominator)
		{
			sign = numerator.getSign();
			if (sign >= 0)
			{
				this->numerator = numerator;
			}
			else
			{
				this->numerator = -numerator;
			}
			int dsign = denominator.getSign();
			if (dsign >= 0)
			{
				this->denominator = denominator;
			}
			else
			{
				sign = -sign;
				this->denominator = -denominator;
			}
			isInt64 = false;
		}

		int compare(const Rational128& b) const;

		int compare(int64_t b) const;

		btScalar toScalar() const
		{
			return (float)sign * (((float)denominator.getSign() == 0.0f) ? FLT_MAX : (float)numerator.toScalar() / (float)denominator.toScalar());
		}
	};

	class PointR128
	{
	public:
		Int128 x;
		Int128 y;
		Int128 z;
		Int128 denominator;

		PointR128()
		{
		}

		PointR128(Int128 x, Int128 y, Int128 z, Int128 denominator) : x(x), y(y), z(z), denominator(denominator)
		{
		}

		btScalar xvalue() const
		{
			return x.toScalar() / denominator.toScalar();
		}

		btScalar yvalue() const
		{
			return y.toScalar() / denominator.toScalar();
		}

		btScalar zvalue() const
		{
			return z.toScalar() / denominator.toScalar();
		}
	};

	class Edge;
	class Face;

	class Vertex
	{
	public:
		Vertex* next;
		Vertex* prev;
		Edge* edges;
		Face* firstNearbyFace;
		Face* lastNearbyFace;
		PointR128 point128;
		Point32 point;
		int copy;

		Vertex() : next(NULL), prev(NULL), edges(NULL), firstNearbyFace(NULL), lastNearbyFace(NULL), copy(-1)
		{
		}

#ifdef DEBUG_CONVEX_HULL
		void print()
		{
			printf("V%d (%d, %d, %d)", point.index, point.x, point.y, point.z);
		}

		void printGraph();
#endif

		Point32 operator-(const Vertex& b) const
		{
			return point - b.point;
		}

		Rational128 dot(const Point64& b) const
		{
			return (point.index >= 0) ? Rational128(point.dot(b))
				: Rational128(point128.x * b.x + point128.y * b.y + point128.z * b.z, point128.denominator);
		}

		btScalar xvalue() const
		{
			return (point.index >= 0) ? btScalar(point.x) : point128.xvalue();
		}

		btScalar yvalue() const
		{
			return (point.index >= 0) ? btScalar(point.y) : point128.yvalue();
		}

		btScalar zvalue() const
		{
			return (point.index >= 0) ? btScalar(point.z) : point128.zvalue();
		}

		void receiveNearbyFaces(Vertex* src)
		{
			if (lastNearbyFace)
			{
				lastNearbyFace->nextWithSameNearbyVertex = src->firstNearbyFace;
			}
			else
			{
				firstNearbyFace = src->firstNearbyFace;
			}
			if (src->lastNearbyFace)
			{
				lastNearbyFace = src->lastNearbyFace;
			}
			for (Face* f = src->firstNearbyFace; f; f = f->nextWithSameNearbyVertex)
			{
				assert(f->nearbyVertex == src);
				f->nearbyVertex = this;
			}
			src->firstNearbyFace = NULL;
			src->lastNearbyFace = NULL;
		}
	};

	class Edge
	{
	public:
		Edge* next;
		Edge* prev;
		Edge* reverse;
		Vertex* target;
		Face* face;
		int copy;

		~Edge()
		{
			next = NULL;
			prev = NULL;
			reverse = NULL;
			target = NULL;
			face = NULL;
		}

		void link(Edge* n)
		{
			assert(reverse->target == n->reverse->target);
			next = n;
			n->prev = this;
		}

#ifdef DEBUG_CONVEX_HULL
		void print()
		{
			printf("E%p : %d -> %d,  n=%p p=%p   (0 %d\t%d\t%d) -> (%d %d %d)", this, reverse->target->point.index, target->point.index, next, prev,
				reverse->target->point.x, reverse->target->point.y, reverse->target->point.z, target->point.x, target->point.y, target->point.z);
		}
#endif
	};

	class Face
	{
	public:
		Face* next;
		Vertex* nearbyVertex;
		Face* nextWithSameNearbyVertex;
		Point32 origin;
		Point32 dir0;
		Point32 dir1;

		Face() : next(NULL), nearbyVertex(NULL), nextWithSameNearbyVertex(NULL)
		{
		}

		void init(Vertex* a, Vertex* b, Vertex* c)
		{
			nearbyVertex = a;
			origin = a->point;
			dir0 = *b - *a;
			dir1 = *c - *a;
			if (a->lastNearbyFace)
			{
				a->lastNearbyFace->nextWithSameNearbyVertex = this;
			}
			else
			{
				a->firstNearbyFace = this;
			}
			a->lastNearbyFace = this;
		}

		Point64 getNormal()
		{
			return dir0.cross(dir1);
		}
	};

	template <typename UWord, typename UHWord>
	class DMul
	{
	private:
		static uint32_t high(uint64_t value)
		{
			return (uint32_t)(value >> 32);
		}

		static uint32_t low(uint64_t value)
		{
			return (uint32_t)value;
		}

		static uint64_t mul(uint32_t a, uint32_t b)
		{
			return (uint64_t)a * (uint64_t)b;
		}

		static void shlHalf(uint64_t& value)
		{
			value <<= 32;
		}

		static uint64_t high(Int128 value)
		{
			return value.high;
		}

		static uint64_t low(Int128 value)
		{
			return value.low;
		}

		static Int128 mul(uint64_t a, uint64_t b)
		{
			return Int128::mul(a, b);
		}

		static void shlHalf(Int128& value)
		{
			value.high = value.low;
			value.low = 0;
		}

	public:
		static void mul(UWord a, UWord b, UWord& resLow, UWord& resHigh)
		{
			UWord p00 = mul(low(a), low(b));
			UWord p01 = mul(low(a), high(b));
			UWord p10 = mul(high(a), low(b));
			UWord p11 = mul(high(a), high(b));
			UWord p0110 = UWord(low(p01)) + UWord(low(p10));
			p11 += high(p01);
			p11 += high(p10);
			p11 += high(p0110);
			shlHalf(p0110);
			p00 += p0110;
			if (p00 < p0110)
			{
				++p11;
			}
			resLow = p00;
			resHigh = p11;
		}
	};

private:
	class IntermediateHull
	{
	public:
		Vertex* minXy;
		Vertex* maxXy;
		Vertex* minYx;
		Vertex* maxYx;

		IntermediateHull() : minXy(NULL), maxXy(NULL), minYx(NULL), maxYx(NULL)
		{
		}

		void print();
	};

	enum Orientation
	{
		NONE,
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};

	template <typename T>
	class PoolArray
	{
	private:
		T* array;
		int size;

	public:
		PoolArray<T>* next;

		PoolArray(int size) : size(size), next(NULL)
		{
			array = (T*)AlignedAllocInternal(sizeof(T) * size, 16);
		}

		~PoolArray()
		{
			AlignedFreeInternal(array);
		}

		T* init()
		{
			T* o = array;
			for (int i = 0; i < size; i++, o++)
			{
				o->next = (i + 1 < size) ? o + 1 : NULL;
			}
			return array;
		}
	};

	template <typename T>
	class Pool
	{
	private:
		PoolArray<T>* arrays;
		PoolArray<T>* nextArray;
		T* freeObjects;
		int arraySize;

	public:
		Pool() : arrays(NULL), nextArray(NULL), freeObjects(NULL), arraySize(256)
		{
		}

		~Pool()
		{
			while (arrays)
			{
				PoolArray<T>* p = arrays;
				arrays = p->next;
				p->~PoolArray<T>();
				AlignedFreeInternal(p);
			}
		}

		void reset()
		{
			nextArray = arrays;
			freeObjects = NULL;
		}

		void setArraySize(int arraySize)
		{
			this->arraySize = arraySize;
		}

		T* newObject()
		{
			T* o = freeObjects;
			if (!o)
			{
				PoolArray<T>* p = nextArray;
				if (p != nullptr)
				{
					nextArray = p->next;
				}
				else
				{
					p = new (AlignedAllocInternal(sizeof(PoolArray<T>), 16)) PoolArray<T>(arraySize);
					if (p != nullptr)
					{
						p->next = arrays;
						arrays = p;
					}
					else { assert(0); }
				
				}
				o = p->init();
			}
			freeObjects = o->next;
			return new (o) T();
		};

		void freeObject(T* object)
		{
			object->~T();
			object->next = freeObjects;
			freeObjects = object;
		}
	};

	btVector3 scaling;
	btVector3 center;
	Pool<Vertex> vertexPool;
	Pool<Edge> edgePool;
	Pool<Face> facePool;
	AlignedObjectArray<Vertex*> originalVertices;
	int mergeStamp;
	int minAxis;
	int medAxis;
	int maxAxis;
	int usedEdgePairs;
	int maxUsedEdgePairs;

	static Orientation getOrientation(const Edge* prev, const Edge* next, const Point32& s, const Point32& t);
	Edge* findMaxAngle(bool ccw, const Vertex* start, const Point32& s, const Point64& rxs, const Point64& sxrxs, Rational64& minCot);
	void findEdgeForCoplanarFaces(Vertex* c0, Vertex* c1, Edge*& e0, Edge*& e1, Vertex* stop0, Vertex* stop1);

	Edge* newEdgePair(Vertex* from, Vertex* to);

	void removeEdgePair(Edge* edge)
	{
		Edge* n = edge->next;
		Edge* r = edge->reverse;

		assert(edge->target && r->target);

		if (n != edge)
		{
			n->prev = edge->prev;
			edge->prev->next = n;
			r->target->edges = n;
		}
		else
		{
			r->target->edges = NULL;
		}

		n = r->next;

		if (n != r)
		{
			n->prev = r->prev;
			r->prev->next = n;
			edge->target->edges = n;
		}
		else
		{
			edge->target->edges = NULL;
		}

		edgePool.freeObject(edge);
		edgePool.freeObject(r);
		usedEdgePairs--;
	}

	void computeInternal(int start, int end, IntermediateHull& result);

	bool mergeProjection(IntermediateHull& h0, IntermediateHull& h1, Vertex*& c0, Vertex*& c1);

	void merge(IntermediateHull& h0, IntermediateHull& h1);

	btVector3 toBtVector(const Point32& v);

	btVector3 getBtNormal(Face* face);

	bool shiftFace(Face* face, btScalar amount,AlignedObjectArray<Vertex*> stack);

public:
	Vertex* vertexList;

	void compute(const void* coords, bool doubleCoords, int stride, int count);

	btVector3 getCoordinates(const Vertex* v);

	btScalar shrink(btScalar amount, btScalar clampAmount);
};

btConvexHullInternal::Int128 btConvexHullInternal::Int128::operator*(int64_t b) const
{
	bool negative = (int64_t)high < 0;
	Int128 a = negative ? -*this : *this;
	if (b < 0)
	{
		negative = !negative;
		b = -b;
	}
	Int128 result = mul(a.low, (uint64_t)b);
	result.high += a.high * (uint64_t)b;
	return negative ? -result : result;
}

btConvexHullInternal::Int128 btConvexHullInternal::Int128::mul(int64_t a, int64_t b)
{
	Int128 result;

#ifdef USE_X86_64_ASM
	__asm__("imulq %[b]"
		: "=a"(result.low), "=d"(result.high)
		: "0"(a), [b] "r"(b)
		: "cc");
	return result;

#else
	bool negative = a < 0;
	if (negative)
	{
		a = -a;
	}
	if (b < 0)
	{
		negative = !negative;
		b = -b;
	}
	DMul<uint64_t, uint32_t>::mul((uint64_t)a, (uint64_t)b, result.low, result.high);
	return negative ? -result : result;
#endif
}

btConvexHullInternal::Int128 btConvexHullInternal::Int128::mul(uint64_t a, uint64_t b)
{
	Int128 result;

#ifdef USE_X86_64_ASM
	__asm__("mulq %[b]"
		: "=a"(result.low), "=d"(result.high)
		: "0"(a), [b] "r"(b)
		: "cc");

#else
	DMul<uint64_t, uint32_t>::mul(a, b, result.low, result.high);
#endif

	return result;
}

int btConvexHullInternal::Rational64::compare(const Rational64& b) const
{
	if (sign != b.sign)
	{
		return sign - b.sign;
	}
	else if (sign == 0)
	{
		return 0;
	}

	//	return (numerator * b.denominator > b.numerator * denominator) ? sign : (numerator * b.denominator < b.numerator * denominator) ? -sign : 0;

#ifdef USE_X86_64_ASM

	int result;
	int64_t tmp;
	int64_t dummy;
	__asm__(
		"mulq %[bn]\n\t"
		"movq %%rax, %[tmp]\n\t"
		"movq %%rdx, %%rbx\n\t"
		"movq %[tn], %%rax\n\t"
		"mulq %[bd]\n\t"
		"subq %[tmp], %%rax\n\t"
		"sbbq %%rbx, %%rdx\n\t"  // rdx:rax contains 128-bit-difference "numerator*b.denominator - b.numerator*denominator"
		"setnsb %%bh\n\t"        // bh=1 if difference is non-negative, bh=0 otherwise
		"orq %%rdx, %%rax\n\t"
		"setnzb %%bl\n\t"      // bl=1 if difference if non-zero, bl=0 if it is zero
		"decb %%bh\n\t"        // now bx=0x0000 if difference is zero, 0xff01 if it is negative, 0x0001 if it is positive (i.e., same sign as difference)
		"shll $16, %%ebx\n\t"  // ebx has same sign as difference
		: "=&b"(result), [tmp] "=&r"(tmp), "=a"(dummy)
		: "a"(m_denominator), [bn] "g"(b.m_numerator), [tn] "g"(m_numerator), [bd] "g"(b.m_denominator)
		: "%rdx", "cc");
	return result ? result ^ sign  // if sign is +1, only bit 0 of result is inverted, which does not change the sign of result (and cannot result in zero)
								   // if sign is -1, all bits of result are inverted, which changes the sign of result (and again cannot result in zero)
		: 0;

#else

	return sign * Int128::mul(m_numerator, b.m_denominator).ucmp(Int128::mul(m_denominator, b.m_numerator));

#endif
}

int btConvexHullInternal::Rational128::compare(const Rational128& b) const
{
	if (sign != b.sign)
	{
		return sign - b.sign;
	}
	else if (sign == 0)
	{
		return 0;
	}
	if (isInt64)
	{
		return -b.compare(sign * (int64_t)numerator.low);
	}

	Int128 nbdLow, nbdHigh, dbnLow, dbnHigh;
	DMul<Int128, uint64_t>::mul(numerator, b.denominator, nbdLow, nbdHigh);
	DMul<Int128, uint64_t>::mul(denominator, b.numerator, dbnLow, dbnHigh);

	int cmp = nbdHigh.ucmp(dbnHigh);
	if (cmp)
	{
		return cmp * sign;
	}
	return nbdLow.ucmp(dbnLow) * sign;
}

int btConvexHullInternal::Rational128::compare(int64_t b) const
{
	if (isInt64)
	{
		int64_t a = sign * (int64_t)numerator.low;
		return (a > b) ? 1 : (a < b) ? -1 : 0;
	}
	if (b > 0)
	{
		if (sign <= 0)
		{
			return -1;
		}
	}
	else if (b < 0)
	{
		if (sign >= 0)
		{
			return 1;
		}
		b = -b;
	}
	else
	{
		return sign;
	}

	return numerator.ucmp(denominator * b) * sign;
}

btConvexHullInternal::Edge* btConvexHullInternal::newEdgePair(Vertex* from, Vertex* to)
{
	assert(from && to);
	Edge* e = edgePool.newObject();
	Edge* r = edgePool.newObject();
	e->reverse = r;
	r->reverse = e;
	e->copy = mergeStamp;
	r->copy = mergeStamp;
	e->target = to;
	r->target = from;
	e->face = NULL;
	r->face = NULL;
	usedEdgePairs++;
	if (usedEdgePairs > maxUsedEdgePairs)
	{
		maxUsedEdgePairs = usedEdgePairs;
	}
	return e;
}

bool btConvexHullInternal::mergeProjection(IntermediateHull& h0, IntermediateHull& h1, Vertex*& c0, Vertex*& c1)
{
	Vertex* v0 = h0.maxYx;
	Vertex* v1 = h1.minYx;
	if ((v0->point.x == v1->point.x) && (v0->point.y == v1->point.y))
	{
		assert(v0->point.z < v1->point.z);
		Vertex* v1p = v1->prev;
		if (v1p == v1)
		{
			c0 = v0;
			if (v1->edges)
			{
				assert(v1->edges->next == v1->edges);
				v1 = v1->edges->target;
				assert(v1->edges->next == v1->edges);
			}
			c1 = v1;
			return false;
		}
		Vertex* v1n = v1->next;
		v1p->next = v1n;
		v1n->prev = v1p;
		if (v1 == h1.minXy)
		{
			if ((v1n->point.x < v1p->point.x) || ((v1n->point.x == v1p->point.x) && (v1n->point.y < v1p->point.y)))
			{
				h1.minXy = v1n;
			}
			else
			{
				h1.minXy = v1p;
			}
		}
		if (v1 == h1.maxXy)
		{
			if ((v1n->point.x > v1p->point.x) || ((v1n->point.x == v1p->point.x) && (v1n->point.y > v1p->point.y)))
			{
				h1.maxXy = v1n;
			}
			else
			{
				h1.maxXy = v1p;
			}
		}
	}

	v0 = h0.maxXy;
	v1 = h1.maxXy;
	Vertex* v00 = NULL;
	Vertex* v10 = NULL;
	int32_t sign = 1;

	for (int side = 0; side <= 1; side++)
	{
		int32_t dx = (v1->point.x - v0->point.x) * sign;
		if (dx > 0)
		{
			while (true)
			{
				int32_t dy = v1->point.y - v0->point.y;

				Vertex* w0 = side ? v0->next : v0->prev;
				if (w0 != v0)
				{
					int32_t dx0 = (w0->point.x - v0->point.x) * sign;
					int32_t dy0 = w0->point.y - v0->point.y;
					if ((dy0 <= 0) && ((dx0 == 0) || ((dx0 < 0) && (dy0 * dx <= dy * dx0))))
					{
						v0 = w0;
						dx = (v1->point.x - v0->point.x) * sign;
						continue;
					}
				}

				Vertex* w1 = side ? v1->next : v1->prev;
				if (w1 != v1)
				{
					int32_t dx1 = (w1->point.x - v1->point.x) * sign;
					int32_t dy1 = w1->point.y - v1->point.y;
					int32_t dxn = (w1->point.x - v0->point.x) * sign;
					if ((dxn > 0) && (dy1 < 0) && ((dx1 == 0) || ((dx1 < 0) && (dy1 * dx < dy * dx1))))
					{
						v1 = w1;
						dx = dxn;
						continue;
					}
				}

				break;
			}
		}
		else if (dx < 0)
		{
			while (true)
			{
				int32_t dy = v1->point.y - v0->point.y;

				Vertex* w1 = side ? v1->prev : v1->next;
				if (w1 != v1)
				{
					int32_t dx1 = (w1->point.x - v1->point.x) * sign;
					int32_t dy1 = w1->point.y - v1->point.y;
					if ((dy1 >= 0) && ((dx1 == 0) || ((dx1 < 0) && (dy1 * dx <= dy * dx1))))
					{
						v1 = w1;
						dx = (v1->point.x - v0->point.x) * sign;
						continue;
					}
				}

				Vertex* w0 = side ? v0->prev : v0->next;
				if (w0 != v0)
				{
					int32_t dx0 = (w0->point.x - v0->point.x) * sign;
					int32_t dy0 = w0->point.y - v0->point.y;
					int32_t dxn = (v1->point.x - w0->point.x) * sign;
					if ((dxn < 0) && (dy0 > 0) && ((dx0 == 0) || ((dx0 < 0) && (dy0 * dx < dy * dx0))))
					{
						v0 = w0;
						dx = dxn;
						continue;
					}
				}

				break;
			}
		}
		else
		{
			int32_t x = v0->point.x;
			int32_t y0 = v0->point.y;
			Vertex* w0 = v0;
			Vertex* t;
			while (((t = side ? w0->next : w0->prev) != v0) && (t->point.x == x) && (t->point.y <= y0))
			{
				w0 = t;
				y0 = t->point.y;
			}
			v0 = w0;

			int32_t y1 = v1->point.y;
			Vertex* w1 = v1;
			while (((t = side ? w1->prev : w1->next) != v1) && (t->point.x == x) && (t->point.y >= y1))
			{
				w1 = t;
				y1 = t->point.y;
			}
			v1 = w1;
		}

		if (side == 0)
		{
			v00 = v0;
			v10 = v1;

			v0 = h0.minXy;
			v1 = h1.minXy;
			sign = -1;
		}
	}

	v0->prev = v1;
	v1->next = v0;

	v00->next = v10;
	v10->prev = v00;

	if (h1.minXy->point.x < h0.minXy->point.x)
	{
		h0.minXy = h1.minXy;
	}
	if (h1.maxXy->point.x >= h0.maxXy->point.x)
	{
		h0.maxXy = h1.maxXy;
	}

	h0.maxYx = h1.maxYx;

	c0 = v00;
	c1 = v10;

	return true;
}

void btConvexHullInternal::computeInternal(int start, int end, IntermediateHull& result)
{
	int n = end - start;
	switch (n)
	{
	case 0:
		result.minXy = NULL;
		result.maxXy = NULL;
		result.minYx = NULL;
		result.maxYx = NULL;
		return;
	case 2:
	{
		Vertex* v = originalVertices[start];
		Vertex* w = v + 1;
		if (v->point != w->point)
		{
			int32_t dx = v->point.x - w->point.x;
			int32_t dy = v->point.y - w->point.y;

			if ((dx == 0) && (dy == 0))
			{
				if (v->point.z > w->point.z)
				{
					Vertex* t = w;
					w = v;
					v = t;
				}
				assert(v->point.z < w->point.z);
				v->next = v;
				v->prev = v;
				result.minXy = v;
				result.maxXy = v;
				result.minYx = v;
				result.maxYx = v;
			}
			else
			{
				v->next = w;
				v->prev = w;
				w->next = v;
				w->prev = v;

				if ((dx < 0) || ((dx == 0) && (dy < 0)))
				{
					result.minXy = v;
					result.maxXy = w;
				}
				else
				{
					result.minXy = w;
					result.maxXy = v;
				}

				if ((dy < 0) || ((dy == 0) && (dx < 0)))
				{
					result.minYx = v;
					result.maxYx = w;
				}
				else
				{
					result.minYx = w;
					result.maxYx = v;
				}
			}

			Edge* e = newEdgePair(v, w);
			e->link(e);
			v->edges = e;

			e = e->reverse;
			e->link(e);
			w->edges = e;

			return;
		}
		{
			Vertex* v = originalVertices[start];
			v->edges = NULL;
			v->next = v;
			v->prev = v;

			result.minXy = v;
			result.maxXy = v;
			result.minYx = v;
			result.maxYx = v;
		}

		return;
	}

	case 1:
	{
		Vertex* v = originalVertices[start];
		v->edges = NULL;
		v->next = v;
		v->prev = v;

		result.minXy = v;
		result.maxXy = v;
		result.minYx = v;
		result.maxYx = v;

		return;
	}
	}

	int split0 = start + n / 2;
	Point32 p = originalVertices[split0 - 1]->point;
	int split1 = split0;
	while ((split1 < end) && (originalVertices[split1]->point == p))
	{
		split1++;
	}
	computeInternal(start, split0, result);
	IntermediateHull hull1;
	computeInternal(split1, end, hull1);
#ifdef DEBUG_CONVEX_HULL
	printf("\n\nMerge\n");
	result.print();
	hull1.print();
#endif
	merge(result, hull1);
#ifdef DEBUG_CONVEX_HULL
	printf("\n  Result\n");
	result.print();
#endif
}

#ifdef DEBUG_CONVEX_HULL
void btConvexHullInternal::IntermediateHull::print()
{
	printf("    Hull\n");
	for (Vertex* v = minXy; v;)
	{
		printf("      ");
		v->print();
		if (v == maxXy)
		{
			printf(" maxXy");
		}
		if (v == minYx)
		{
			printf(" minYx");
		}
		if (v == maxYx)
		{
			printf(" maxYx");
		}
		if (v->next->prev != v)
		{
			printf(" Inconsistency");
		}
		printf("\n");
		v = v->next;
		if (v == minXy)
		{
			break;
		}
	}
	if (minXy)
	{
		minXy->copy = (minXy->copy == -1) ? -2 : -1;
		minXy->printGraph();
	}
}

void btConvexHullInternal::Vertex::printGraph()
{
	print();
	printf("\nEdges\n");
	Edge* e = edges;
	if (e)
	{
		do
		{
			e->print();
			printf("\n");
			e = e->next;
		} while (e != edges);
		do
		{
			Vertex* v = e->target;
			if (v->copy != copy)
			{
				v->copy = copy;
				v->printGraph();
			}
			e = e->next;
		} while (e != edges);
	}
}
#endif

btConvexHullInternal::Orientation btConvexHullInternal::getOrientation(const Edge* prev, const Edge* next, const Point32& s, const Point32& t)
{
	assert(prev->reverse->target == next->reverse->target);
	if (prev->next == next)
	{
		if (prev->prev == next)
		{
			Point64 n = t.cross(s);
			Point64 m = (*prev->target - *next->reverse->target).cross(*next->target - *next->reverse->target);
			assert(!m.isZero());
			int64_t dot = n.dot(m);
			assert(dot != 0);
			return (dot > 0) ? COUNTER_CLOCKWISE : CLOCKWISE;
		}
		return COUNTER_CLOCKWISE;
	}
	else if (prev->prev == next)
	{
		return CLOCKWISE;
	}
	else
	{
		return NONE;
	}
}

btConvexHullInternal::Edge* btConvexHullInternal::findMaxAngle(bool ccw, const Vertex* start, const Point32& s, const Point64& rxs, const Point64& sxrxs, Rational64& minCot)
{
	Edge* minEdge = NULL;

#ifdef DEBUG_CONVEX_HULL
	printf("find max edge for %d\n", start->point.index);
#endif
	Edge* e = start->edges;
	if (e)
	{
		do
		{
			if (e->copy > mergeStamp)
			{
				Point32 t = *e->target - *start;
				Rational64 cot(t.dot(sxrxs), t.dot(rxs));
#ifdef DEBUG_CONVEX_HULL
				printf("      Angle is %f (%d) for ", (float)btAtan(cot.toScalar()), (int)cot.isNaN());
				e->print();
#endif
				if (cot.isNaN())
				{
					assert(ccw ? (t.dot(s) < 0) : (t.dot(s) > 0));
				}
				else
				{
					int cmp;
					if (minEdge == NULL)
					{
						minCot = cot;
						minEdge = e;
					}
					else if ((cmp = cot.compare(minCot)) < 0)
					{
						minCot = cot;
						minEdge = e;
					}
					else if ((cmp == 0) && (ccw == (getOrientation(minEdge, e, s, t) == COUNTER_CLOCKWISE)))
					{
						minEdge = e;
					}
				}
#ifdef DEBUG_CONVEX_HULL
				printf("\n");
#endif
			}
			e = e->next;
		} while (e != start->edges);
	}
	return minEdge;
}

void btConvexHullInternal::findEdgeForCoplanarFaces(Vertex* c0, Vertex* c1, Edge*& e0, Edge*& e1, Vertex* stop0, Vertex* stop1)
{
	Edge* start0 = e0;
	Edge* start1 = e1;
	Point32 et0 = start0 ? start0->target->point : c0->point;
	Point32 et1 = start1 ? start1->target->point : c1->point;
	Point32 s = c1->point - c0->point;
	Point64 normal = ((start0 ? start0 : start1)->target->point - c0->point).cross(s);
	int64_t dist = c0->point.dot(normal);
	assert(!start1 || (start1->target->point.dot(normal) == dist));
	Point64 perp = s.cross(normal);
	assert(!perp.isZero());

#ifdef DEBUG_CONVEX_HULL
	printf("   Advancing %d %d  (%p %p, %d %d)\n", c0->point.index, c1->point.index, start0, start1, start0 ? start0->target->point.index : -1, start1 ? start1->target->point.index : -1);
#endif

	int64_t maxDot0 = et0.dot(perp);
	if (e0)
	{
		while (e0->target != stop0)
		{
			Edge* e = e0->reverse->prev;
			if (e->target->point.dot(normal) < dist)
			{
				break;
			}
			assert(e->target->point.dot(normal) == dist);
			if (e->copy == mergeStamp)
			{
				break;
			}
			int64_t dot = e->target->point.dot(perp);
			if (dot <= maxDot0)
			{
				break;
			}
			maxDot0 = dot;
			e0 = e;
			et0 = e->target->point;
		}
	}

	int64_t maxDot1 = et1.dot(perp);
	if (e1)
	{
		while (e1->target != stop1)
		{
			Edge* e = e1->reverse->next;
			if (e->target->point.dot(normal) < dist)
			{
				break;
			}
			assert(e->target->point.dot(normal) == dist);
			if (e->copy == mergeStamp)
			{
				break;
			}
			int64_t dot = e->target->point.dot(perp);
			if (dot <= maxDot1)
			{
				break;
			}
			maxDot1 = dot;
			e1 = e;
			et1 = e->target->point;
		}
	}

#ifdef DEBUG_CONVEX_HULL
	printf("   Starting at %d %d\n", et0.index, et1.index);
#endif

	int64_t dx = maxDot1 - maxDot0;
	if (dx > 0)
	{
		while (true)
		{
			int64_t dy = (et1 - et0).dot(s);

			if (e0 && (e0->target != stop0))
			{
				Edge* f0 = e0->next->reverse;
				if (f0->copy > mergeStamp)
				{
					int64_t dx0 = (f0->target->point - et0).dot(perp);
					int64_t dy0 = (f0->target->point - et0).dot(s);
					if ((dx0 == 0) ? (dy0 < 0) : ((dx0 < 0) && (Rational64(dy0, dx0).compare(Rational64(dy, dx)) >= 0)))
					{
						et0 = f0->target->point;
						dx = (et1 - et0).dot(perp);
						e0 = (e0 == start0) ? NULL : f0;
						continue;
					}
				}
			}

			if (e1 && (e1->target != stop1))
			{
				Edge* f1 = e1->reverse->next;
				if (f1->copy > mergeStamp)
				{
					Point32 d1 = f1->target->point - et1;
					if (d1.dot(normal) == 0)
					{
						int64_t dx1 = d1.dot(perp);
						int64_t dy1 = d1.dot(s);
						int64_t dxn = (f1->target->point - et0).dot(perp);
						if ((dxn > 0) && ((dx1 == 0) ? (dy1 < 0) : ((dx1 < 0) && (Rational64(dy1, dx1).compare(Rational64(dy, dx)) > 0))))
						{
							e1 = f1;
							et1 = e1->target->point;
							dx = dxn;
							continue;
						}
					}
					else
					{
						assert((e1 == start1) && (d1.dot(normal) < 0));
					}
				}
			}

			break;
		}
	}
	else if (dx < 0)
	{
		while (true)
		{
			int64_t dy = (et1 - et0).dot(s);

			if (e1 && (e1->target != stop1))
			{
				Edge* f1 = e1->prev->reverse;
				if (f1->copy > mergeStamp)
				{
					int64_t dx1 = (f1->target->point - et1).dot(perp);
					int64_t dy1 = (f1->target->point - et1).dot(s);
					if ((dx1 == 0) ? (dy1 > 0) : ((dx1 < 0) && (Rational64(dy1, dx1).compare(Rational64(dy, dx)) <= 0)))
					{
						et1 = f1->target->point;
						dx = (et1 - et0).dot(perp);
						e1 = (e1 == start1) ? NULL : f1;
						continue;
					}
				}
			}

			if (e0 && (e0->target != stop0))
			{
				Edge* f0 = e0->reverse->prev;
				if (f0->copy > mergeStamp)
				{
					Point32 d0 = f0->target->point - et0;
					if (d0.dot(normal) == 0)
					{
						int64_t dx0 = d0.dot(perp);
						int64_t dy0 = d0.dot(s);
						int64_t dxn = (et1 - f0->target->point).dot(perp);
						if ((dxn < 0) && ((dx0 == 0) ? (dy0 > 0) : ((dx0 < 0) && (Rational64(dy0, dx0).compare(Rational64(dy, dx)) < 0))))
						{
							e0 = f0;
							et0 = e0->target->point;
							dx = dxn;
							continue;
						}
					}
					else
					{
						assert((e0 == start0) && (d0.dot(normal) < 0));
					}
				}
			}

			break;
		}
	}
#ifdef DEBUG_CONVEX_HULL
	printf("   Advanced edges to %d %d\n", et0.index, et1.index);
#endif
}

void btConvexHullInternal::merge(IntermediateHull& h0, IntermediateHull& h1)
{
	if (!h1.maxXy)
	{
		return;
	}
	if (!h0.maxXy)
	{
		h0 = h1;
		return;
	}

	mergeStamp--;

	Vertex* c0 = NULL;
	Edge* toPrev0 = NULL;
	Edge* firstNew0 = NULL;
	Edge* pendingHead0 = NULL;
	Edge* pendingTail0 = NULL;
	Vertex* c1 = NULL;
	Edge* toPrev1 = NULL;
	Edge* firstNew1 = NULL;
	Edge* pendingHead1 = NULL;
	Edge* pendingTail1 = NULL;
	Point32 prevPoint;

	if (mergeProjection(h0, h1, c0, c1))
	{
		Point32 s = *c1 - *c0;
		Point64 normal = Point32(0, 0, -1).cross(s);
		Point64 t = s.cross(normal);
		assert(!t.isZero());

		Edge* e = c0->edges;
		Edge* start0 = NULL;
		if (e)
		{
			do
			{
				int64_t dot = (*e->target - *c0).dot(normal);
				assert(dot <= 0);
				if ((dot == 0) && ((*e->target - *c0).dot(t) > 0))
				{
					if (!start0 || (getOrientation(start0, e, s, Point32(0, 0, -1)) == CLOCKWISE))
					{
						start0 = e;
					}
				}
				e = e->next;
			} while (e != c0->edges);
		}

		e = c1->edges;
		Edge* start1 = NULL;
		if (e)
		{
			do
			{
				int64_t dot = (*e->target - *c1).dot(normal);
				assert(dot <= 0);
				if ((dot == 0) && ((*e->target - *c1).dot(t) > 0))
				{
					if (!start1 || (getOrientation(start1, e, s, Point32(0, 0, -1)) == COUNTER_CLOCKWISE))
					{
						start1 = e;
					}
				}
				e = e->next;
			} while (e != c1->edges);
		}

		if (start0 || start1)
		{
			findEdgeForCoplanarFaces(c0, c1, start0, start1, NULL, NULL);
			if (start0)
			{
				c0 = start0->target;
			}
			if (start1)
			{
				c1 = start1->target;
			}
		}

		prevPoint = c1->point;
		prevPoint.z++;
	}
	else
	{
		prevPoint = c1->point;
		prevPoint.x++;
	}

	Vertex* first0 = c0;
	Vertex* first1 = c1;
	bool firstRun = true;

	while (true)
	{
		Point32 s = *c1 - *c0;
		Point32 r = prevPoint - c0->point;
		Point64 rxs = r.cross(s);
		Point64 sxrxs = s.cross(rxs);

#ifdef DEBUG_CONVEX_HULL
		printf("\n  Checking %d %d\n", c0->point.index, c1->point.index);
#endif
		Rational64 minCot0(0, 0);
		Edge* min0 = findMaxAngle(false, c0, s, rxs, sxrxs, minCot0);
		Rational64 minCot1(0, 0);
		Edge* min1 = findMaxAngle(true, c1, s, rxs, sxrxs, minCot1);
		if (!min0 && !min1)
		{
			Edge* e = newEdgePair(c0, c1);
			e->link(e);
			c0->edges = e;

			e = e->reverse;
			e->link(e);
			c1->edges = e;
			return;
		}
		else
		{
			int cmp = !min0 ? 1 : !min1 ? -1 : minCot0.compare(minCot1);
#ifdef DEBUG_CONVEX_HULL
			printf("    -> Result %d\n", cmp);
#endif
			if (firstRun || ((cmp >= 0) ? !minCot1.isNegativeInfinity() : !minCot0.isNegativeInfinity()))
			{
				Edge* e = newEdgePair(c0, c1);
				if (pendingTail0)
				{
					pendingTail0->prev = e;
				}
				else
				{
					pendingHead0 = e;
				}
				e->next = pendingTail0;
				pendingTail0 = e;

				e = e->reverse;
				if (pendingTail1)
				{
					pendingTail1->next = e;
				}
				else
				{
					pendingHead1 = e;
				}
				e->prev = pendingTail1;
				pendingTail1 = e;
			}

			Edge* e0 = min0;
			Edge* e1 = min1;

#ifdef DEBUG_CONVEX_HULL
			printf("   Found min edges to %d %d\n", e0 ? e0->target->point.index : -1, e1 ? e1->target->point.index : -1);
#endif

			if (cmp == 0)
			{
				findEdgeForCoplanarFaces(c0, c1, e0, e1, NULL, NULL);
			}

			if ((cmp >= 0) && e1)
			{
				if (toPrev1)
				{
					for (Edge* e = toPrev1->next, *n = NULL; e != min1; e = n)
					{
						n = e->next;
						removeEdgePair(e);
					}
				}

				if (pendingTail1)
				{
					if (toPrev1)
					{
						toPrev1->link(pendingHead1);
					}
					else
					{
						min1->prev->link(pendingHead1);
						firstNew1 = pendingHead1;
					}
					pendingTail1->link(min1);
					pendingHead1 = NULL;
					pendingTail1 = NULL;
				}
				else if (!toPrev1)
				{
					firstNew1 = min1;
				}

				prevPoint = c1->point;
				c1 = e1->target;
				toPrev1 = e1->reverse;
			}

			if ((cmp <= 0) && e0)
			{
				if (toPrev0)
				{
					for (Edge* e = toPrev0->prev, *n = NULL; e != min0; e = n)
					{
						n = e->prev;
						removeEdgePair(e);
					}
				}

				if (pendingTail0)
				{
					if (toPrev0)
					{
						pendingHead0->link(toPrev0);
					}
					else
					{
						pendingHead0->link(min0->next);
						firstNew0 = pendingHead0;
					}
					min0->link(pendingTail0);
					pendingHead0 = NULL;
					pendingTail0 = NULL;
				}
				else if (!toPrev0)
				{
					firstNew0 = min0;
				}

				prevPoint = c0->point;
				c0 = e0->target;
				toPrev0 = e0->reverse;
			}
		}

		if ((c0 == first0) && (c1 == first1))
		{
			if (toPrev0 == NULL)
			{
				pendingHead0->link(pendingTail0);
				c0->edges = pendingTail0;
			}
			else
			{
				for (Edge* e = toPrev0->prev, *n = NULL; e != firstNew0; e = n)
				{
					n = e->prev;
					removeEdgePair(e);
				}
				if (pendingTail0)
				{
					pendingHead0->link(toPrev0);
					firstNew0->link(pendingTail0);
				}
			}

			if (toPrev1 == NULL)
			{
				pendingTail1->link(pendingHead1);
				c1->edges = pendingTail1;
			}
			else
			{
				for (Edge* e = toPrev1->next, *n = NULL; e != firstNew1; e = n)
				{
					n = e->next;
					removeEdgePair(e);
				}
				if (pendingTail1)
				{
					toPrev1->link(pendingHead1);
					pendingTail1->link(firstNew1);
				}
			}

			return;
		}

		firstRun = false;
	}
}

class pointCmp
{
public:
	bool operator()(const btConvexHullInternal::Point32& p, const btConvexHullInternal::Point32& q) const
	{
		return (p.y < q.y) || ((p.y == q.y) && ((p.x < q.x) || ((p.x == q.x) && (p.z < q.z))));
	}
};

void btConvexHullInternal::compute(const void* coords, bool doubleCoords, int stride, int count)
{
	btVector3 minimum(btScalar(FLT_MAX), btScalar(FLT_MAX), btScalar(FLT_MAX)), maximum(btScalar(-FLT_MAX), btScalar(-FLT_MAX), btScalar(-FLT_MAX));
	const char* ptr = (const char*)coords;
	if (doubleCoords)
	{
		for (int i = 0; i < count; i++)
		{
			const double* v = (const double*)ptr;
			btVector3 p((btScalar)v[0], (btScalar)v[1], (btScalar)v[2]);
			ptr += stride;
			minimum = gm::Min(minimum, p);
			maximum = gm::Max(maximum, p);
		}
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			const float* v = (const float*)ptr;
			btVector3 p(v[0], v[1], v[2]);
			ptr += stride;
			minimum = gm::Min(minimum, p);
			maximum = gm::Max(maximum, p);
		}
	}

	btVector3 s = maximum - minimum;
	maxAxis = (int)gm::MaxAxis3(s);
	minAxis = (int)gm::MinAxis3(s);
	if (minAxis == maxAxis)
	{
		minAxis = (maxAxis + 1) % 3;
	}
	medAxis = 3 - maxAxis - minAxis;

	s /= btScalar(10216);
	if (((medAxis + 1) % 3) != maxAxis)
	{
		s = -1 * s;
	}
	scaling = s;

	if (s.GetElement(0) != 0.0f)
	{
		s.SetElement(0, btScalar(1) / s.GetElement(0));
	}
	if (s.GetElement(1) != 0.0f)
	{
		s.SetElement(1, btScalar(1) / s.GetElement(1));
	}
	if (s.GetElement(2) != 0.0f)
	{
		s.SetElement(2, btScalar(2) / s.GetElement(2));
	}

	center = (minimum + maximum) * btScalar(0.5);

	AlignedObjectArray<Point32> points;
	points.resize(count);
	ptr = (const char*)coords;
	if (doubleCoords)
	{
		for (int i = 0; i < count; i++)
		{
			const double* v = (const double*)ptr;
			btVector3 p((btScalar)v[0], (btScalar)v[1], (btScalar)v[2]);
			ptr += stride;
			p = (p - center) * s;
			points[i].x = (int32_t)p.GetElement(medAxis);
			points[i].y = (int32_t)p.GetElement(maxAxis);
			points[i].z = (int32_t)p.GetElement(minAxis);
			points[i].index = i;
		}
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			const float* v = (const float*)ptr;
			btVector3 p(v[0], v[1], v[2]);
			ptr += stride;
			p = (p - center) * s;
			points[i].x = (int32_t)p.GetElement(medAxis);
			points[i].y = (int32_t)p.GetElement(maxAxis);
			points[i].z = (int32_t)p.GetElement(minAxis);
			points[i].index = i;
		}
	}
	points.quickSort(pointCmp());

	vertexPool.reset();
	vertexPool.setArraySize(count);
	originalVertices.resize(count);
	for (int i = 0; i < count; i++)
	{
		Vertex* v = vertexPool.newObject();
		v->edges = NULL;
		v->point = points[i];
		v->copy = -1;
		originalVertices[i] = v;
	}

	points.clear();

	edgePool.reset();
	edgePool.setArraySize(6 * count);

	usedEdgePairs = 0;
	maxUsedEdgePairs = 0;

	mergeStamp = -3;

	IntermediateHull hull;
	computeInternal(0, count, hull);
	vertexList = hull.minXy;
#ifdef DEBUG_CONVEX_HULL
	printf("max. edges %d (3v = %d)", maxUsedEdgePairs, 3 * count);
#endif
}

btVector3 btConvexHullInternal::toBtVector(const Point32& v)
{
	btVector3 p;
	p.SetElement(medAxis, btScalar(v.x));
	p.SetElement(maxAxis, btScalar(v.y));
	p.SetElement(minAxis, btScalar(v.z));
	return p * scaling;
}

btVector3 btConvexHullInternal::getBtNormal(Face* face)
{
	return Normalize(Cross(toBtVector(face->dir0), (toBtVector(face->dir1))));
}

btVector3 btConvexHullInternal::getCoordinates(const Vertex* v)
{
	btVector3 p;
	p.SetElement(medAxis, v->xvalue());
	p.SetElement(maxAxis, v->yvalue());
	p.SetElement(minAxis, v->zvalue());
	return p * scaling + center;
}

btScalar btConvexHullInternal::shrink(btScalar amount, btScalar clampAmount)
{
	if (!vertexList)
	{
		return 0;
	}
	int stamp = --mergeStamp;
	AlignedObjectArray<Vertex*> stack;
	vertexList->copy = stamp;
	stack.push_back(vertexList);
	AlignedObjectArray<Face*> faces;

	Point32 ref = vertexList->point;
	Int128 hullCenterX(0, 0);
	Int128 hullCenterY(0, 0);
	Int128 hullCenterZ(0, 0);
	Int128 volume(0, 0);

	while (stack.size() > 0)
	{
		Vertex* v = stack[stack.size() - 1];
		stack.pop_back();
		Edge* e = v->edges;
		if (e)
		{
			do
			{
				if (e->target->copy != stamp)
				{
					e->target->copy = stamp;
					stack.push_back(e->target);
				}
				if (e->copy != stamp)
				{
					Face* face = facePool.newObject();
					face->init(e->target, e->reverse->prev->target, v);
					faces.push_back(face);
					Edge* f = e;

					Vertex* a = NULL;
					Vertex* b = NULL;
					do
					{
						if (a && b)
						{
							int64_t vol = (v->point - ref).dot((a->point - ref).cross(b->point - ref));
							assert(vol >= 0);
							Point32 c = v->point + a->point + b->point + ref;
							hullCenterX += vol * c.x;
							hullCenterY += vol * c.y;
							hullCenterZ += vol * c.z;
							volume += vol;
						}

						assert(f->copy != stamp);
						f->copy = stamp;
						f->face = face;

						a = b;
						b = f->target;

						f = f->reverse->prev;
					} while (f != e);
				}
				e = e->next;
			} while (e != v->edges);
		}
	}

	if (volume.getSign() <= 0)
	{
		return 0;
	}

	btVector3 hullCenter;
	hullCenter.SetElement(medAxis, hullCenterX.toScalar());
	hullCenter.SetElement(maxAxis, hullCenterY.toScalar());
	hullCenter.SetElement(minAxis, hullCenterZ.toScalar());
	hullCenter = hullCenter / (4.0f * volume.toScalar());
	hullCenter *= scaling;

	int faceCount = faces.size();

	if (clampAmount > 0.0f)
	{
		btScalar minDist = FLT_MAX;
		for (int i = 0; i < faceCount; i++)
		{
			btVector3 normal = getBtNormal(faces[i]);
			float dist = Dot(normal, (toBtVector(faces[i]->origin) - hullCenter));
			if (dist < minDist)
			{
				minDist = dist;
			}
		}

		if (minDist <= 0.0f)
		{
			return 0;
		}

		amount = Min(amount, minDist * clampAmount);
	}

	unsigned int seed = 243703;
	for (int i = 0; i < faceCount; i++, seed = 1664525 * seed + 1013904223)
	{
		auto temp = faces[i];
		faces[i]  = faces[seed % faceCount];
		faces[seed % faceCount] = temp;
	}

	for (int i = 0; i < faceCount; i++)
	{
		if (!shiftFace(faces[i], amount, stack))
		{
			return -amount;
		}
	}

	return amount;
}

bool btConvexHullInternal::shiftFace(Face* face, btScalar amount, AlignedObjectArray<Vertex*> stack)
{
	btVector3 origShift = getBtNormal(face) * -amount;
	if (scaling.GetElement(0) != 0.0f)
	{
		origShift.SetElement(0, origShift.GetElement(0) / scaling.GetElement(0));
	}
	if (scaling.GetElement(1) != 0.0f)
	{
		origShift.SetElement(1, origShift.GetElement(1) / scaling.GetElement(1));
	}
	if (scaling.GetElement(2) != 0.0f)
	{
		origShift.SetElement(2, origShift.GetElement(2) / scaling.GetElement(2));
	}
	Point32 shift((int32_t)origShift.GetElement(medAxis), (int32_t)origShift.GetElement(maxAxis), (int32_t)origShift.GetElement(minAxis));
	if (shift.isZero())
	{
		return true;
	}
	Point64 normal = face->getNormal();
#ifdef DEBUG_CONVEX_HULL
	printf("\nShrinking face (%d %d %d) (%d %d %d) (%d %d %d) by (%d %d %d)\n",
		face->origin.x, face->origin.y, face->origin.z, face->dir0.x, face->dir0.y, face->dir0.z, face->dir1.x, face->dir1.y, face->dir1.z, shift.x, shift.y, shift.z);
#endif
	int64_t origDot = face->origin.dot(normal);
	Point32 shiftedOrigin = face->origin + shift;
	int64_t shiftedDot = shiftedOrigin.dot(normal);
	assert(shiftedDot <= origDot);
	if (shiftedDot >= origDot)
	{
		return false;
	}

	Edge* intersection = NULL;

	Edge* startEdge = face->nearbyVertex->edges;
#ifdef DEBUG_CONVEX_HULL
	printf("Start edge is ");
	startEdge->print();
	printf(", normal is (%lld %lld %lld), shifted dot is %lld\n", normal.x, normal.y, normal.z, shiftedDot);
#endif
	Rational128 optDot = face->nearbyVertex->dot(normal);
	int cmp = optDot.compare(shiftedDot);
#ifdef SHOW_ITERATIONS
	int n = 0;
#endif
	if (cmp >= 0)
	{
		Edge* e = startEdge;
		do
		{
#ifdef SHOW_ITERATIONS
			n++;
#endif
			Rational128 dot = e->target->dot(normal);
			assert(dot.compare(origDot) <= 0);
#ifdef DEBUG_CONVEX_HULL
			printf("Moving downwards, edge is ");
			e->print();
			printf(", dot is %f (%f %lld)\n", (float)dot.toScalar(), (float)optDot.toScalar(), shiftedDot);
#endif
			if (dot.compare(optDot) < 0)
			{
				int c = dot.compare(shiftedDot);
				optDot = dot;
				e = e->reverse;
				startEdge = e;
				if (c < 0)
				{
					intersection = e;
					break;
				}
				cmp = c;
			}
			e = e->prev;
		} while (e != startEdge);

		if (!intersection)
		{
			return false;
		}
	}
	else
	{
		Edge* e = startEdge;
		do
		{
#ifdef SHOW_ITERATIONS
			n++;
#endif
			Rational128 dot = e->target->dot(normal);
			assert(dot.compare(origDot) <= 0);
#ifdef DEBUG_CONVEX_HULL
			printf("Moving upwards, edge is ");
			e->print();
			printf(", dot is %f (%f %lld)\n", (float)dot.toScalar(), (float)optDot.toScalar(), shiftedDot);
#endif
			if (dot.compare(optDot) > 0)
			{
				cmp = dot.compare(shiftedDot);
				if (cmp >= 0)
				{
					intersection = e;
					break;
				}
				optDot = dot;
				e = e->reverse;
				startEdge = e;
			}
			e = e->prev;
		} while (e != startEdge);

		if (!intersection)
		{
			return true;
		}
	}

#ifdef SHOW_ITERATIONS
	printf("Needed %d iterations to find initial intersection\n", n);
#endif

	if (cmp == 0)
	{
		Edge* e = intersection->reverse->next;
#ifdef SHOW_ITERATIONS
		n = 0;
#endif
		while (e->target->dot(normal).compare(shiftedDot) <= 0)
		{
#ifdef SHOW_ITERATIONS
			n++;
#endif
			e = e->next;
			if (e == intersection->reverse)
			{
				return true;
			}
#ifdef DEBUG_CONVEX_HULL
			printf("Checking for outwards edge, current edge is ");
			e->print();
			printf("\n");
#endif
		}
#ifdef SHOW_ITERATIONS
		printf("Needed %d iterations to check for complete containment\n", n);
#endif
	}

	Edge* firstIntersection = NULL;
	Edge* faceEdge = NULL;
	Edge* firstFaceEdge = NULL;

#ifdef SHOW_ITERATIONS
	int m = 0;
#endif
	while (true)
	{
#ifdef SHOW_ITERATIONS
		m++;
#endif
#ifdef DEBUG_CONVEX_HULL
		printf("Intersecting edge is ");
		intersection->print();
		printf("\n");
#endif
		if (cmp == 0)
		{
			Edge* e = intersection->reverse->next;
			startEdge = e;
#ifdef SHOW_ITERATIONS
			n = 0;
#endif
			while (true)
			{
#ifdef SHOW_ITERATIONS
				n++;
#endif
				if (e->target->dot(normal).compare(shiftedDot) >= 0)
				{
					break;
				}
				intersection = e->reverse;
				e = e->next;
				if (e == startEdge)
				{
					return true;
				}
			}
#ifdef SHOW_ITERATIONS
			printf("Needed %d iterations to advance intersection\n", n);
#endif
		}

#ifdef DEBUG_CONVEX_HULL
		printf("Advanced intersecting edge to ");
		intersection->print();
		printf(", cmp = %d\n", cmp);
#endif

		if (!firstIntersection)
		{
			firstIntersection = intersection;
		}
		else if (intersection == firstIntersection)
		{
			break;
		}

		int prevCmp = cmp;
		Edge* prevIntersection = intersection;
		Edge* prevFaceEdge = faceEdge;

		Edge* e = intersection->reverse;
#ifdef SHOW_ITERATIONS
		n = 0;
#endif
		while (true)
		{
#ifdef SHOW_ITERATIONS
			n++;
#endif
			e = e->reverse->prev;
			assert(e != intersection->reverse);
			cmp = e->target->dot(normal).compare(shiftedDot);
#ifdef DEBUG_CONVEX_HULL
			printf("Testing edge ");
			e->print();
			printf(" -> cmp = %d\n", cmp);
#endif
			if (cmp >= 0)
			{
				intersection = e;
				break;
			}
		}
#ifdef SHOW_ITERATIONS
		printf("Needed %d iterations to find other intersection of face\n", n);
#endif

		if (cmp > 0)
		{
			Vertex* removed = intersection->target;
			e = intersection->reverse;
			if (e->prev == e)
			{
				removed->edges = NULL;
			}
			else
			{
				removed->edges = e->prev;
				e->prev->link(e->next);
				e->link(e);
			}
#ifdef DEBUG_CONVEX_HULL
			printf("1: Removed part contains (%d %d %d)\n", removed->point.x, removed->point.y, removed->point.z);
#endif

			Point64 n0 = intersection->face->getNormal();
			Point64 n1 = intersection->reverse->face->getNormal();
			int64_t m00 = face->dir0.dot(n0);
			int64_t m01 = face->dir1.dot(n0);
			int64_t m10 = face->dir0.dot(n1);
			int64_t m11 = face->dir1.dot(n1);
			int64_t r0 = (intersection->face->origin - shiftedOrigin).dot(n0);
			int64_t r1 = (intersection->reverse->face->origin - shiftedOrigin).dot(n1);
			Int128 det = Int128::mul(m00, m11) - Int128::mul(m01, m10);
			assert(det.getSign() != 0);
			Vertex* v = vertexPool.newObject();
			v->point.index = -1;
			v->copy = -1;
			v->point128 = PointR128(Int128::mul(face->dir0.x * r0, m11) - Int128::mul(face->dir0.x * r1, m01) + Int128::mul(face->dir1.x * r1, m00) - Int128::mul(face->dir1.x * r0, m10) + det * shiftedOrigin.x,
				Int128::mul(face->dir0.y * r0, m11) - Int128::mul(face->dir0.y * r1, m01) + Int128::mul(face->dir1.y * r1, m00) - Int128::mul(face->dir1.y * r0, m10) + det * shiftedOrigin.y,
				Int128::mul(face->dir0.z * r0, m11) - Int128::mul(face->dir0.z * r1, m01) + Int128::mul(face->dir1.z * r1, m00) - Int128::mul(face->dir1.z * r0, m10) + det * shiftedOrigin.z,
				det);
			v->point.x = (int32_t)v->point128.xvalue();
			v->point.y = (int32_t)v->point128.yvalue();
			v->point.z = (int32_t)v->point128.zvalue();
			intersection->target = v;
			v->edges = e;

			stack.push_back(v);
			stack.push_back(removed);
			stack.push_back(NULL);
		}

		if (cmp || prevCmp || (prevIntersection->reverse->next->target != intersection->target))
		{
			faceEdge = newEdgePair(prevIntersection->target, intersection->target);
			if (prevCmp == 0)
			{
				faceEdge->link(prevIntersection->reverse->next);
			}
			if ((prevCmp == 0) || prevFaceEdge)
			{
				prevIntersection->reverse->link(faceEdge);
			}
			if (cmp == 0)
			{
				intersection->reverse->prev->link(faceEdge->reverse);
			}
			faceEdge->reverse->link(intersection->reverse);
		}
		else
		{
			faceEdge = prevIntersection->reverse->next;
		}

		if (prevFaceEdge)
		{
			if (prevCmp > 0)
			{
				faceEdge->link(prevFaceEdge->reverse);
			}
			else if (faceEdge != prevFaceEdge->reverse)
			{
				stack.push_back(prevFaceEdge->target);
				while (faceEdge->next != prevFaceEdge->reverse)
				{
					Vertex* removed = faceEdge->next->target;
					removeEdgePair(faceEdge->next);
					stack.push_back(removed);
#ifdef DEBUG_CONVEX_HULL
					printf("2: Removed part contains (%d %d %d)\n", removed->point.x, removed->point.y, removed->point.z);
#endif
				}
				stack.push_back(NULL);
			}
		}
		faceEdge->face = face;
		faceEdge->reverse->face = intersection->face;

		if (!firstFaceEdge)
		{
			firstFaceEdge = faceEdge;
		}
	}
#ifdef SHOW_ITERATIONS
	printf("Needed %d iterations to process all intersections\n", m);
#endif

	if (cmp > 0)
	{
		firstFaceEdge->reverse->target = faceEdge->target;
		firstIntersection->reverse->link(firstFaceEdge);
		firstFaceEdge->link(faceEdge->reverse);
	}
	else if (firstFaceEdge != faceEdge->reverse)
	{
		stack.push_back(faceEdge->target);
		while (firstFaceEdge->next != faceEdge->reverse)
		{
			Vertex* removed = firstFaceEdge->next->target;
			removeEdgePair(firstFaceEdge->next);
			stack.push_back(removed);
#ifdef DEBUG_CONVEX_HULL
			printf("3: Removed part contains (%d %d %d)\n", removed->point.x, removed->point.y, removed->point.z);
#endif
		}
		stack.push_back(NULL);
	}

	assert(stack.size() > 0);
	vertexList = stack[0];

#ifdef DEBUG_CONVEX_HULL
	printf("Removing part\n");
#endif
#ifdef SHOW_ITERATIONS
	n = 0;
#endif
	int pos = 0;
	while (pos < stack.size())
	{
		int end = stack.size();
		while (pos < end)
		{
			Vertex* kept = stack[pos++];
#ifdef DEBUG_CONVEX_HULL
			kept->print();
#endif
			bool deeper = false;
			Vertex* removed;
			while ((removed = stack[pos++]) != NULL)
			{
#ifdef SHOW_ITERATIONS
				n++;
#endif
				kept->receiveNearbyFaces(removed);
				while (removed->edges)
				{
					if (!deeper)
					{
						deeper = true;
						stack.push_back(kept);
					}
					stack.push_back(removed->edges->target);
					removeEdgePair(removed->edges);
				}
			}
			if (deeper)
			{
				stack.push_back(NULL);
			}
		}
	}
#ifdef SHOW_ITERATIONS
	printf("Needed %d iterations to remove part\n", n);
#endif

	stack.resize(0);
	face->origin = shiftedOrigin;

	return true;
}

static int getVertexCopy(btConvexHullInternal::Vertex* vertex, AlignedObjectArray<btConvexHullInternal::Vertex*>& vertices)
{
	int index = vertex->copy;
	if (index < 0)
	{
		index = vertices.size();
		vertex->copy = index;
		vertices.push_back(vertex);
#ifdef DEBUG_CONVEX_HULL
		printf("Vertex %d gets index *%d\n", vertex->point.index, index);
#endif
	}
	return index;
}

float ConvexHullComputer::Compute(const void* coords, bool doubleCoords, int stride, int count, float shrink, float shrinkClamp)
{
	if (count <= 0)
	{
		Vertices.clear();
		Edges.clear();
		Faces.clear();
		return 0;
	}

	btConvexHullInternal hull;
	hull.compute(coords, doubleCoords, stride, count);

	btScalar shift = 0;
	if ((shrink > 0) && ((shift = hull.shrink(shrink, shrinkClamp)) < 0.0f))
	{
		Vertices.clear();
		Edges.clear();
		Faces.clear();
		return shift;
	}

	Vertices.resize(0);
	OriginalVertexIndex.resize(0);
	Edges.resize(0);
	Faces.resize(0);

	AlignedObjectArray<btConvexHullInternal::Vertex*> oldVertices;
	getVertexCopy(hull.vertexList, oldVertices);
	int copied = 0;
	while (copied < oldVertices.size())
	{
		btConvexHullInternal::Vertex* v = oldVertices[copied];
		Vertices.push_back(hull.getCoordinates(v));
		OriginalVertexIndex.push_back(v->point.index);
		btConvexHullInternal::Edge* firstEdge = v->edges;
		if (firstEdge)
		{
			int firstCopy = -1;
			int prevCopy = -1;
			btConvexHullInternal::Edge* e = firstEdge;
			do
			{
				if (e->copy < 0)
				{
					int s = Edges.size();
					Edges.push_back(Edge());
					Edges.push_back(Edge());
					Edge* c = &Edges[s];
					Edge* r = &Edges[s + 1];
					e->copy = s;
					e->reverse->copy = s + 1;
					c->_reverse = 1;
					r->_reverse = -1;
					c->_targetVertex = getVertexCopy(e->target, oldVertices);
					r->_targetVertex = copied;
#ifdef DEBUG_CONVEX_HULL
					printf("      CREATE: Vertex *%d has edge to *%d\n", copied, c->getTargetVertex());
#endif
				}
				if (prevCopy >= 0)
				{
					Edges[e->copy]._next = prevCopy - e->copy;
				}
				else
				{
					firstCopy = e->copy;
				}
				prevCopy = e->copy;
				e = e->next;
			} while (e != firstEdge);
			Edges[firstCopy]._next = prevCopy - firstCopy;
		}
		copied++;
	}

	for (int i = 0; i < copied; i++)
	{
		btConvexHullInternal::Vertex* v = oldVertices[i];
		btConvexHullInternal::Edge* firstEdge = v->edges;
		if (firstEdge)
		{
			btConvexHullInternal::Edge* e = firstEdge;
			do
			{
				if (e->copy >= 0)
				{
#ifdef DEBUG_CONVEX_HULL
					printf("Vertex *%d has edge to *%d\n", i, edges[e->copy].getTargetVertex());
#endif
					Faces.push_back(e->copy);
					btConvexHullInternal::Edge* f = e;
					do
					{
#ifdef DEBUG_CONVEX_HULL
						printf("   Face *%d\n", edges[f->copy].getTargetVertex());
#endif
						f->copy = -1;
						f = f->reverse->prev;
					} while (f != e);
				}
				e = e->next;
			} while (e != firstEdge);
		}
	}

	return shift;
}

#pragma region ConvexHull

class int3
{
public:
	int x, y, z;
	int3() {};
	int3(int _x, int _y, int _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	const int& operator[](int i) const { return (&x)[i]; }
	int& operator[](int i) { return (&x)[i]; }
};

//------- btPlane ----------

inline btPlane PlaneFlip(const btPlane& plane) { return btPlane(-plane.normal, -plane.dist); }
inline int operator==(const btPlane& a, const btPlane& b) { return (a.normal == b.normal && (float)a.dist == b.dist); }
inline int coplanar(const btPlane& a, const btPlane& b) { return (a == b || a == PlaneFlip(b)); }

//--------- Utility Functions ------

btVector3 PlaneLineIntersection(const btPlane& plane, const btVector3& p0, const btVector3& p1);
btVector3 PlaneProject(const btPlane& plane, const btVector3& point);

btVector3 ThreePlaneIntersection(const btPlane& p0, const btPlane& p1, const btPlane& p2);
btVector3 ThreePlaneIntersection(const btPlane& p0, const btPlane& p1, const btPlane& p2)
{
	btVector3 N1 = p0.normal;
	btVector3 N2 = p1.normal;
	btVector3 N3 = p2.normal;

	btVector3 n2n3;
	n2n3 = Cross(N2, N3);
	btVector3 n3n1;
	n3n1 = Cross(N3, N1);
	btVector3 n1n2;
	n1n2 = Cross(N1, N2);

	btScalar quotient = (Dot(N1, n2n3));

	assert((float)Abs(quotient) > btScalar(0.000001));

	quotient = btScalar(-1.) / quotient;
	n2n3 *= p0.dist;
	n3n1 *= p1.dist;
	n1n2 *= p2.dist;
	btVector3 potentialVertex = n2n3;
	potentialVertex += n3n1;
	potentialVertex += n1n2;
	potentialVertex *= quotient;

	btVector3 result(potentialVertex.GetX(), potentialVertex.GetY(), potentialVertex.GetZ());
	return result;
}

btScalar DistanceBetweenLines(const btVector3& ustart, const btVector3& udir, const btVector3& vstart, const btVector3& vdir, btVector3* upoint = NULL, btVector3* vpoint = NULL);
btVector3 TriNormal(const btVector3& v0, const btVector3& v1, const btVector3& v2);
btVector3 NormalOf(const btVector3* vert, const int n);

btVector3 PlaneLineIntersection(const btPlane& plane, const btVector3& p0, const btVector3& p1)
{
	// returns the point where the line p0-p1 intersects the plane n&d
	btVector3 dif;
	dif = p1 - p0;
	btScalar dn = Dot(plane.normal, dif);
	btScalar t = -(plane.dist + Dot(plane.normal, p0)) / dn;
	return p0 + (dif * t);
}

btVector3 PlaneProject(const btPlane& plane, const btVector3& point)
{
	return point - plane.normal * (Dot(point, plane.normal) + plane.dist);
}

btVector3 TriNormal(const btVector3& v0, const btVector3& v1, const btVector3& v2)
{
	// return the normal of the triangle
	// inscribed by v0, v1, and v2
	btVector3 cp = Cross(v1 - v0, v2 - v1);
	btScalar m   = Norm(cp);
	if ((float)m == 0) return btVector3(1, 0, 0);
	return cp * (btScalar(1.0) / m);
}

btScalar DistanceBetweenLines(const btVector3& ustart, const btVector3& udir, const btVector3& vstart, const btVector3& vdir, btVector3* upoint, btVector3* vpoint)
{
	btVector3 cp;
	cp = Normalize(Cross(udir, vdir));

	btScalar distu = -Dot(cp, ustart);
	btScalar distv = -Dot(cp, vstart);
	btScalar dist = (btScalar)fabs(distu - distv);
	if (upoint)
	{
		btPlane plane;
		plane.normal = Normalize(Cross(vdir, cp));
		plane.dist = -Dot(plane.normal, vstart);
		*upoint = PlaneLineIntersection(plane, ustart, ustart + udir);
	}
	if (vpoint)
	{
		btPlane plane;
		plane.normal = Normalize(Cross(udir, cp));
		plane.dist = -Dot(plane.normal, ustart);
		*vpoint = PlaneLineIntersection(plane, vstart, vstart + vdir);
	}
	return dist;
}

#define COPLANAR (0)
#define UNDER (1)
#define OVER (2)
#define SPLIT (OVER | UNDER)
#define PAPERWIDTH (btScalar(0.001))

btScalar planetestepsilon = PAPERWIDTH;

typedef ConvexH::HalfEdge HalfEdge;

ConvexH::ConvexH(int vertices_size, int edges_size, int facets_size)
{
	vertices.resize(vertices_size);
	edges.resize(edges_size);
	facets.resize(facets_size);
}

int PlaneTest(const btPlane& p, const btVector3& v);
int PlaneTest(const btPlane& p, const btVector3& v)
{
	btScalar a = Dot(v, p.normal) + p.dist;
	int flag = ((float)a > planetestepsilon) ? OVER : (((float)a < -planetestepsilon) ? UNDER : COPLANAR);
	return flag;
}

int SplitTest(ConvexH& convex, const btPlane& plane);
int SplitTest(ConvexH& convex, const btPlane& plane)
{
	int flag = 0;
	for (int i = 0; i < convex.vertices.size(); i++)
	{
		flag |= PlaneTest(plane, convex.vertices[i]);
	}
	return flag;
}

class VertFlag
{
public:
	unsigned char planetest;
	unsigned char junk;
	unsigned char undermap;
	unsigned char overmap;
};
class EdgeFlag
{
public:
	unsigned char planetest;
	unsigned char fixes;
	short undermap;
	short overmap;
};
class PlaneFlag
{
public:
	unsigned char undermap;
	unsigned char overmap;
};
class Coplanar
{
public:
	unsigned short ea;
	unsigned char v0;
	unsigned char v1;
};

template <class T>
int maxdirfiltered(const T* p, int count, const T& dir, AlignedObjectArray<int>& allow)
{
	assert(count);
	int m = -1;
	for (int i = 0; i < count; i++)
		if (allow[i])
		{
			if (m == -1 || (float)Dot(p[i], dir) > Dot(p[m], dir))
				m = i;
		}
	assert(m != -1);
	return m;
}

btVector3 orth(const btVector3& v);
btVector3 orth(const btVector3& v)
{
	btVector3 a = Cross(v, btVector3(0, 0, 1));
	btVector3 b = Cross(v, btVector3(0, 1, 0));
	if ((float)Norm(a) > Norm(b))
	{
		return Normalize(a);
	}
	else
	{
		return Normalize(b);
	}
}

template <class T>
int maxdirsterid(const T* p, int count, const T& dir, AlignedObjectArray<int>& allow)
{
	int m = -1;
	while (m == -1)
	{
		m = maxdirfiltered(p, count, dir, allow);
		if (allow[m] == 3) return m;
		T u = orth(dir);
		T v = Cross(u, dir);
		int ma = -1;
		for (float x = 0.0f; x <=360.0f; x += 45.0f)
		{
			btScalar s = Sin(GM_RADS_PER_DEG * (x));
			btScalar c = Cos(GM_RADS_PER_DEG * (x));
			int mb = maxdirfiltered(p, count, dir + (u * s + v * c) * btScalar(0.025), allow);
			if (ma == m && mb == m)
			{
				allow[m] = 3;
				return m;
			}
			if (ma != -1 && ma != mb)  // Yuck - this is really ugly
			{
				int mc = ma;
				for (float xx = x -40.0f; xx <= x; xx += 5.0f)
				{
					btScalar s = Sin(GM_RADS_PER_DEG * (xx));
					btScalar c = Cos(GM_RADS_PER_DEG * (xx));
					int md = maxdirfiltered(p, count, dir + (u * s + v * c) * btScalar(0.025), allow);
					if (mc == m && md == m)
					{
						allow[m] = 3;
						return m;
					}
					mc = md;
				}
			}
			ma = mb;
		}
		allow[m] = 0;
		m = -1;
	}
	assert(0);
	return m;
}

int operator==(const int3& a, const int3& b);
int operator==(const int3& a, const int3& b)
{
	for (int i = 0; i < 3; i++)
	{
		if (a[i] != b[i]) return 0;
	}
	return 1;
}

int above(btVector3* vertices, const int3& t, const btVector3& p, btScalar epsilon);
int above(btVector3* vertices, const int3& t, const btVector3& p, btScalar epsilon)
{
	btVector3 n = TriNormal(vertices[t[0]], vertices[t[1]], vertices[t[2]]);
	return ((float)Dot(n, p - vertices[t[0]]) > epsilon);  // EPSILON???
}
int hasedge(const int3& t, int a, int b);
int hasedge(const int3& t, int a, int b)
{
	for (int i = 0; i < 3; i++)
	{
		int i1 = (i + 1) % 3;
		if (t[i] == a && t[i1] == b) return 1;
	}
	return 0;
}
int hasvert(const int3& t, int v);
int hasvert(const int3& t, int v)
{
	return (t[0] == v || t[1] == v || t[2] == v);
}
int shareedge(const int3& a, const int3& b);
int shareedge(const int3& a, const int3& b)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		int i1 = (i + 1) % 3;
		if (hasedge(a, b[i1], b[i])) return 1;
	}
	return 0;
}

class btHullTriangle;

class btHullTriangle : public int3
{
public:
	int3 n;
	int id;
	int vmax;
	btScalar rise;
	btHullTriangle(int a, int b, int c) : int3(a, b, c), n(-1, -1, -1)
	{
		vmax = -1;
		rise = btScalar(0.0);
	}
	~btHullTriangle()
	{
	}
	int& neib(int a, int b);
};

int& btHullTriangle::neib(int a, int b)
{
	static int er = -1;
	int i;
	for (i = 0; i < 3; i++)
	{
		int i1 = (i + 1) % 3;
		int i2 = (i + 2) % 3;
		if ((*this)[i] == a && (*this)[i1] == b) return n[i2];
		if ((*this)[i] == b && (*this)[i1] == a) return n[i2];
	}
	assert(0);
	return er;
}
void HullLibrary::b2bfix(btHullTriangle* s, btHullTriangle* t)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		int i1 = (i + 1) % 3;
		int i2 = (i + 2) % 3;
		int a = (*s)[i1];
		int b = (*s)[i2];
		assert(m_tris[s->neib(a, b)]->neib(b, a) == s->id);
		assert(m_tris[t->neib(a, b)]->neib(b, a) == t->id);
		m_tris[s->neib(a, b)]->neib(b, a) = t->neib(b, a);
		m_tris[t->neib(b, a)]->neib(a, b) = s->neib(a, b);
	}
}

void HullLibrary::removeb2b(btHullTriangle* s, btHullTriangle* t)
{
	b2bfix(s, t);
	deAllocateTriangle(s);

	deAllocateTriangle(t);
}

void HullLibrary::checkit(btHullTriangle* t)
{
	(void)t;

	int i;
	assert(m_tris[t->id] == t);
	for (i = 0; i < 3; i++)
	{
		int i1 = (i + 1) % 3;
		int i2 = (i + 2) % 3;
		int a = (*t)[i1];
		int b = (*t)[i2];

		// release compile fix
		(void)i1;
		(void)i2;
		(void)a;
		(void)b;

		assert(a != b);
		assert(m_tris[t->n[i]]->neib(b, a) == t->id);
	}
}

btHullTriangle* HullLibrary::allocateTriangle(int a, int b, int c)
{

	void* mem = AlignedAllocInternal(sizeof(btHullTriangle), 16);
	btHullTriangle* tr = new (mem) btHullTriangle(a, b, c);
	tr->id = m_tris.size();
	m_tris.push_back(tr);

	return tr;
}

void HullLibrary::deAllocateTriangle(btHullTriangle* tri)
{
	assert(m_tris[tri->id] == tri);
	m_tris[tri->id] = NULL;
	tri->~btHullTriangle();
	AlignedFreeInternal(tri);
}

void HullLibrary::extrude(btHullTriangle* t0, int v)
{
	int3 t = *t0;
	int n = m_tris.size();
	btHullTriangle* ta = allocateTriangle(v, t[1], t[2]);
	ta->n = int3(t0->n[0], n + 1, n + 2);
	m_tris[t0->n[0]]->neib(t[1], t[2]) = n + 0;
	btHullTriangle* tb = allocateTriangle(v, t[2], t[0]);
	tb->n = int3(t0->n[1], n + 2, n + 0);
	m_tris[t0->n[1]]->neib(t[2], t[0]) = n + 1;
	btHullTriangle* tc = allocateTriangle(v, t[0], t[1]);
	tc->n = int3(t0->n[2], n + 0, n + 1);
	m_tris[t0->n[2]]->neib(t[0], t[1]) = n + 2;
	checkit(ta);
	checkit(tb);
	checkit(tc);
	if (hasvert(*m_tris[ta->n[0]], v)) removeb2b(ta, m_tris[ta->n[0]]);
	if (hasvert(*m_tris[tb->n[0]], v)) removeb2b(tb, m_tris[tb->n[0]]);
	if (hasvert(*m_tris[tc->n[0]], v)) removeb2b(tc, m_tris[tc->n[0]]);
	deAllocateTriangle(t0);
}

btHullTriangle* HullLibrary::extrudable(gm::Scalar epsilon)
{
	int i;
	btHullTriangle* t = nullptr;
	for (i = 0; i < m_tris.size(); i++)
	{
		if (!t || (m_tris[i] && (float)t->rise < m_tris[i]->rise))
		{
			t = m_tris[i];
		}
	}

	if (t == nullptr){ return nullptr; }
	else if((float)t->rise <= epsilon){ return nullptr; }
	else                              { return nullptr; }
}

int4 HullLibrary::FindSimplex(btVector3* verts, int verts_count, AlignedObjectArray<int>& allow)
{
	btVector3 basis[3];
	basis[0] = btVector3(btScalar(0.01), btScalar(0.02), btScalar(1.0));
	int p0 = maxdirsterid(verts, verts_count, basis[0], allow);
	int p1 = maxdirsterid(verts, verts_count, -basis[0], allow);
	basis[0] = verts[p0] - verts[p1];
	if (p0 == p1 || basis[0] == btVector3(0, 0, 0))
		return int4(-1, -1, -1, -1);
	basis[1] = Cross(btVector3(btScalar(1), btScalar(0.02), btScalar(0)), basis[0]);
	basis[2] = Cross(btVector3(btScalar(-0.02), btScalar(1), btScalar(0)), basis[0]);
	if ((float)Norm(basis[1]) > Norm(basis[2]))
	{
		Normalize(basis[1]);
	}
	else
	{
		basis[1] = basis[2];
		Normalize(basis[1]);
	}
	int p2 = maxdirsterid(verts, verts_count, basis[1], allow);
	if (p2 == p0 || p2 == p1)
	{
		p2 = maxdirsterid(verts, verts_count, -basis[1], allow);
	}
	if (p2 == p0 || p2 == p1)
		return int4(-1, -1, -1, -1);
	basis[1] = verts[p2] - verts[p0];
	basis[2] = Normalize(Cross(basis[1], basis[0]));
	int p3 = maxdirsterid(verts, verts_count, basis[2], allow);
	if (p3 == p0 || p3 == p1 || p3 == p2) p3 = maxdirsterid(verts, verts_count, -basis[2], allow);
	if (p3 == p0 || p3 == p1 || p3 == p2)
		return int4(-1, -1, -1, -1);
	assert(!(p0 == p1 || p0 == p2 || p0 == p3 || p1 == p2 || p1 == p3 || p2 == p3));
	if (Dot(verts[p3] - verts[p0], Cross(verts[p1] - verts[p0], verts[p2] - verts[p0])) < (float)0.0f)
	{
		int temp = p2;
		p2 = p3;
		p3 = temp;
	}
	return int4(p0, p1, p2, p3);
}

int HullLibrary::calchullgen(btVector3* verts, int verts_count, int vlimit)
{
	if (verts_count < 4) return 0;
	if (vlimit == 0) vlimit = 1000000000;
	int j;
	btVector3 bmin(*verts), bmax(*verts);
	AlignedObjectArray<int> isextreme;
	isextreme.reserve(verts_count);
	AlignedObjectArray<int> allow;
	allow.reserve(verts_count);

	for (j = 0; j < verts_count; j++)
	{
		allow.push_back(1);
		isextreme.push_back(0);
		Min(bmin, verts[j]);
		Max(bmax, verts[j]);
	}
	btScalar epsilon = Norm(bmax - bmin) * btScalar(0.001);
	assert((float)epsilon != 0.0);

	int4 p = FindSimplex(verts, verts_count, allow);
	if (p.x == -1) return 0;  // simplex failed

	btVector3 center = (verts[p[0]] + verts[p[1]] + verts[p[2]] + verts[p[3]]) / btScalar(4.0);  // a valid interior point
	btHullTriangle* t0 = allocateTriangle(p[2], p[3], p[1]);
	t0->n = int3(2, 3, 1);
	btHullTriangle* t1 = allocateTriangle(p[3], p[2], p[0]);
	t1->n = int3(3, 2, 0);
	btHullTriangle* t2 = allocateTriangle(p[0], p[1], p[3]);
	t2->n = int3(0, 1, 3);
	btHullTriangle* t3 = allocateTriangle(p[1], p[0], p[2]);
	t3->n = int3(1, 0, 2);
	isextreme[p[0]] = isextreme[p[1]] = isextreme[p[2]] = isextreme[p[3]] = 1;
	checkit(t0);
	checkit(t1);
	checkit(t2);
	checkit(t3);

	for (j = 0; j < m_tris.size(); j++)
	{
		btHullTriangle* t = m_tris[j];
		assert(t);
		assert(t->vmax < 0);
		btVector3 n = TriNormal(verts[(*t)[0]], verts[(*t)[1]], verts[(*t)[2]]);
		t->vmax = maxdirsterid(verts, verts_count, n, allow);
		t->rise = Dot(n, verts[t->vmax] - verts[(*t)[0]]);
	}
	btHullTriangle* te;
	vlimit -= 4;
	while (vlimit > 0 && ((te = extrudable(epsilon)) != 0))
	{
		//int3 ti=*te;
		int v = te->vmax;
		assert(v != -1);
		assert(!isextreme[v]);  // wtf we've already done this vertex
		isextreme[v] = 1;
		//if(v==p0 || v==p1 || v==p2 || v==p3) continue; // done these already
		j = m_tris.size();
		while (j--)
		{
			if (!m_tris[j]) continue;
			int3 t = *m_tris[j];
			if (above(verts, t, verts[v], btScalar(0.01) * epsilon))
			{
				extrude(m_tris[j], v);
			}
		}
		// now check for those degenerate cases where we have a flipped triangle or a really skinny triangle
		j = m_tris.size();
		while (j--)
		{
			if (!m_tris[j]) continue;
			if (!hasvert(*m_tris[j], v)) break;
			int3 nt = *m_tris[j];
			if (above(verts, nt, center, btScalar(0.01) * epsilon) || (float)Norm(Cross(verts[nt[1]] - verts[nt[0]], verts[nt[2]] - verts[nt[1]])) < epsilon * epsilon * btScalar(0.1))
			{
				btHullTriangle* nb = m_tris[m_tris[j]->n[0]];
				assert(nb);
				assert(!hasvert(*nb, v));
				assert(nb->id < j);
				extrude(nb, v);
				j = m_tris.size();
			}
		}
		j = m_tris.size();
		while (j--)
		{
			btHullTriangle* t = m_tris[j];
			if (!t) continue;
			if (t->vmax >= 0) break;
			btVector3 n = TriNormal(verts[(*t)[0]], verts[(*t)[1]], verts[(*t)[2]]);
			t->vmax = maxdirsterid(verts, verts_count, n, allow);
			if (isextreme[t->vmax])
			{
				t->vmax = -1;  // already done that vertex - algorithm needs to be able to terminate.
			}
			else
			{
				t->rise = Dot(n, verts[t->vmax] - verts[(*t)[0]]);
			}
		}
		vlimit--;
	}
	return 1;
}

int HullLibrary::calchull(btVector3* verts, int verts_count, TUIntArray& tris_out, int& tris_count, int vlimit)
{
	int rc = calchullgen(verts, verts_count, vlimit);
	if (!rc) return 0;
	AlignedObjectArray<int> ts;
	int i;

	for (i = 0; i < m_tris.size(); i++)
	{
		if (m_tris[i])
		{
			for (int j = 0; j < 3; j++)
				ts.push_back((*m_tris[i])[j]);
			deAllocateTriangle(m_tris[i]);
		}
	}
	tris_count = ts.size() / 3;
	tris_out.resize(ts.size());

	for (i = 0; i < ts.size(); i++)
	{
		tris_out[i] = static_cast<unsigned int>(ts[i]);
	}
	m_tris.resize(0);

	return 1;
}

bool HullLibrary::ComputeHull(unsigned int vcount, const btVector3* vertices, PHullResult& result, unsigned int vlimit)
{
	int tris_count;
	int ret = calchull((btVector3*)vertices, (int)vcount, result.m_Indices, tris_count, static_cast<int>(vlimit));
	if (!ret) return false;
	result.mIndexCount = (unsigned int)(tris_count * 3);
	result.mFaceCount = (unsigned int)tris_count;
	result.mVertices = (btVector3*)vertices;
	result.mVcount = (unsigned int)vcount;
	return true;
}

void ReleaseHull(PHullResult& result);
void ReleaseHull(PHullResult& result)
{
	if (result.m_Indices.size())
	{
		result.m_Indices.clear();
	}

	result.mVcount = 0;
	result.mIndexCount = 0;
	result.mVertices = 0;
}

//*********************************************************************
//*********************************************************************
//********  HullLib header
//*********************************************************************
//*********************************************************************

//*********************************************************************
//*********************************************************************
//********  HullLib implementation
//*********************************************************************
//*********************************************************************

HullError HullLibrary::CreateConvexHull(const HullDesc& desc,  // describes the input request
	HullResult& result)    // contains the resulst
{
	HullError ret = QE_FAIL;

	PHullResult hr;

	unsigned int vcount = desc.Vcount;
	if (vcount < 8) vcount = 8;

	AlignedObjectArray<btVector3> vertexSource;
	vertexSource.resize(static_cast<int>(vcount));

	btVector3 scale;

	unsigned int ovcount;

	bool ok = CleanupVertices(desc.Vcount, desc.Vertices, desc.VertexStride, ovcount, &vertexSource[0], desc.NormalEpsilon, scale);  // normalize point cloud, remove duplicates!

	if (ok)
	{
		//		if ( 1 ) // scale vertices back to their original size.
		{
			for (unsigned int i = 0; i < ovcount; i++)
			{
				btVector3& v = vertexSource[static_cast<int>(i)];
				v.SetX(v.GetX() * scale.GetX());
				v.SetX(v.GetY() * scale.GetY());
				v.SetX(v.GetZ() * scale.GetZ());
			}
		}

		ok = ComputeHull(ovcount, &vertexSource[0], hr, desc.MaxVertices);

		if (ok)
		{
			// re-index triangle mesh so it refers to only used vertices, rebuild a new vertex table.
			AlignedObjectArray<btVector3> vertexScratch;
			vertexScratch.resize(static_cast<int>(hr.mVcount));

			BringOutYourDead(hr.mVertices, hr.mVcount, &vertexScratch[0], ovcount, &hr.m_Indices[0], hr.mIndexCount);

			ret = QE_OK;

			if (desc.HasHullFlag(QF_TRIANGLES))  // if he wants the results as triangle!
			{
				result.Polygons = false;
				result.NumOutputVertices = ovcount;
				result.OutputVertices.resize(static_cast<int>(ovcount));
				result.NumFaces = hr.mFaceCount;
				result.NumIndices = hr.mIndexCount;

				result.m_Indices.resize(static_cast<int>(hr.mIndexCount));

				memcpy(&result.OutputVertices[0], &vertexScratch[0], sizeof(btVector3) * ovcount);

				if (desc.HasHullFlag(QF_REVERSE_ORDER))
				{
					const unsigned int* source = &hr.m_Indices[0];
					unsigned int* dest = &result.m_Indices[0];

					for (unsigned int i = 0; i < hr.mFaceCount; i++)
					{
						dest[0] = source[2];
						dest[1] = source[1];
						dest[2] = source[0];
						dest += 3;
						source += 3;
					}
				}
				else
				{
					memcpy(&result.m_Indices[0], &hr.m_Indices[0], sizeof(unsigned int) * hr.mIndexCount);
				}
			}
			else
			{
				result.Polygons = true;
				result.NumOutputVertices = ovcount;
				result.OutputVertices.resize(static_cast<int>(ovcount));
				result.NumFaces = hr.mFaceCount;
				result.NumIndices = hr.mIndexCount + hr.mFaceCount;
				result.m_Indices.resize(static_cast<int>(result.NumIndices));
				memcpy(&result.OutputVertices[0], &vertexScratch[0], sizeof(btVector3) * ovcount);

				//				if ( 1 )
				{
					const unsigned int* source = &hr.m_Indices[0];
					unsigned int* dest = &result.m_Indices[0];
					for (unsigned int i = 0; i < hr.mFaceCount; i++)
					{
						dest[0] = 3;
						if (desc.HasHullFlag(QF_REVERSE_ORDER))
						{
							dest[1] = source[2];
							dest[2] = source[1];
							dest[3] = source[0];
						}
						else
						{
							dest[1] = source[0];
							dest[2] = source[1];
							dest[3] = source[2];
						}

						dest += 4;
						source += 3;
					}
				}
			}
			ReleaseHull(hr);
		}
	}

	return ret;
}

HullError HullLibrary::ReleaseResult(HullResult& result)  // release memory allocated for this result, we are done with it.
{
	if (result.OutputVertices.size())
	{
		result.NumOutputVertices = 0;
		result.OutputVertices.clear();
	}
	if (result.m_Indices.size())
	{
		result.NumIndices = 0;
		result.m_Indices.clear();
	}
	return QE_OK;
}

static void addPoint(unsigned int& vcount, btVector3* p, btScalar x, btScalar y, btScalar z)
{
	// XXX, might be broken
	btVector3& dest = p[vcount];
	dest.SetX(x);
	dest.SetY(y);
	dest.SetZ(z);
	vcount++;
}

btScalar GetDist(btScalar px, btScalar py, btScalar pz, const btScalar* p2);
btScalar GetDist(btScalar px, btScalar py, btScalar pz, const btScalar* p2)
{
	btScalar dx = px - p2[0];
	btScalar dy = py - p2[1];
	btScalar dz = pz - p2[2];

	return dx * dx + dy * dy + dz * dz;
}

bool HullLibrary::CleanupVertices(unsigned int svcount,
	const btVector3* svertices,
	unsigned int stride,
	unsigned int& vcount,  // output number of vertices
	btVector3* vertices,   // location to store the results.
	btScalar normalepsilon,
	btVector3& scale)
{
	if (svcount == 0) return false;

	m_vertexIndexMapping.resize(0);

#define EPSILON btScalar(0.000001) /* close enough to consider two btScalaring point numbers to be 'the same'. */

	vcount = 0;

	btScalar recip[3] = { 0.f, 0.f, 0.f };

	if (true)
	{
		scale.SetX(1);
		scale.SetY(1);
		scale.SetZ(1);
	}

	btScalar bmin[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
	btScalar bmax[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	const char* vtx = (const char*)svertices;

	//	if ( 1 )
	{
		for (unsigned int i = 0; i < svcount; i++)
		{
			const btScalar* p = (const btScalar*)vtx;

			vtx += stride;

			for (int j = 0; j < 3; j++)
			{
				if ((float)p[j] < bmin[j]) bmin[j] = p[j];
				if ((float)p[j] > bmax[j]) bmax[j] = p[j];
			}
		}
	}

	btScalar dx = bmax[0] - bmin[0];
	btScalar dy = bmax[1] - bmin[1];
	btScalar dz = bmax[2] - bmin[2];

	btVector3 center;

	center.SetX(dx * btScalar(0.5) + bmin[0]);
	center.SetY(dy * btScalar(0.5) + bmin[1]);
	center.SetZ(dz * btScalar(0.5) + bmin[2]);

	if ((float)dx < EPSILON || (float)dy < EPSILON || (float)dz < EPSILON || svcount < 3)
	{
		btScalar len = FLT_MAX;

		if ((float)dx > EPSILON && (float)dx < len) len = dx;
		if ((float)dy > EPSILON && (float)dy < len) len = dy;
		if ((float)dz > EPSILON && (float)dz < len) len = dz;

		if (len == FLT_MAX)
		{
			dx = dy = dz = btScalar(0.01);  // one centimeter
		}
		else
		{
			if ((float)dx < EPSILON) dx = len * btScalar(0.05);  // 1/5th the shortest non-zero edge.
			if ((float)dy < EPSILON) dy = len * btScalar(0.05);
			if ((float)dz < EPSILON) dz = len * btScalar(0.05);
		}

		btScalar x1 = center.GetX() - dx;
		btScalar x2 = center.GetX() + dx;

		btScalar y1 = center.GetY() - dy;
		btScalar y2 = center.GetY() + dy;

		btScalar z1 = center.GetZ() - dz;
		btScalar z2 = center.GetZ() + dz;

		addPoint(vcount, vertices, x1, y1, z1);
		addPoint(vcount, vertices, x2, y1, z1);
		addPoint(vcount, vertices, x2, y2, z1);
		addPoint(vcount, vertices, x1, y2, z1);
		addPoint(vcount, vertices, x1, y1, z2);
		addPoint(vcount, vertices, x2, y1, z2);
		addPoint(vcount, vertices, x2, y2, z2);
		addPoint(vcount, vertices, x1, y2, z2);

		return true;  // return cube
	}
	else
	{
		if (true)
		{
			scale.SetX(dx);
			scale.SetY(dy);
			scale.SetZ(dz);

			recip[0] = 1.0f / dx;
			recip[1] = 1.0f / dy;
			recip[2] = 1.0f / dz;

			center.SetX(center.GetX() * recip[0]);
			center.SetY(center.GetY() * recip[1]);
			center.SetZ(center.GetZ() * recip[2]);
		}
	}

	vtx = (const char*)svertices;

	for (unsigned int i = 0; i < svcount; i++)
	{
		const btVector3* p = (const btVector3*)vtx;
		vtx += stride;

		btScalar px = p->GetX();
		btScalar py = p->GetY();
		btScalar pz = p->GetZ();

		if (&scale != nullptr)
		{
			px = px * recip[0];  // normalize
			py = py * recip[1];  // normalize
			pz = pz * recip[2];  // normalize
		}

		//		if ( 1 )
		{
			unsigned int j;

			for (j = 0; j < vcount; j++)
			{
				/// XXX might be broken
				btVector3& v = vertices[j];

				btScalar x = v.GetX();
				btScalar y = v.GetY();
				btScalar z = v.GetZ();

				btScalar dx = Abs(x - px);
				btScalar dy = Abs(y - py);
				btScalar dz = Abs(z - pz);

				if ((float)dx < normalepsilon && (float)dy < normalepsilon && (float)dz < normalepsilon)
				{
					// ok, it is close enough to the old one
					// now let us see if it is further from the center of the point cloud than the one we already recorded.
					// in which case we keep this one instead.
					Scalar temp[3];
					temp[0] = center.GetX(); temp[1] = center.GetY(); temp[2] = center.GetZ();
					btScalar dist1 = GetDist(px, py, pz, temp);
					btScalar dist2 = GetDist(v.GetX(), v.GetY(), v.GetZ(), temp);

					if ((float)dist1 > dist2)
					{
						v.SetX(px);
						v.SetY(py);
						v.SetZ(pz);
					}

					break;
				}
			}

			if (j == vcount)
			{
				btVector3& dest = vertices[vcount];
				dest.SetX(px);
				dest.SetY(py);
				dest.SetZ(pz);
				vcount++;
			}
			m_vertexIndexMapping.push_back(j);
		}
	}

	// ok..now make sure we didn't prune so many vertices it is now invalid.
	//	if ( 1 )
	{
		btScalar bmin[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
		btScalar bmax[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (unsigned int i = 0; i < vcount; i++)
		{
			const btVector3& p = vertices[i];
			for (int j = 0; j < 3; j++)
			{
				if ((float)p.GetElement(j) < bmin[j]) bmin[j] = p.GetElement(j);
				if ((float)p.GetElement(j) > bmax[j]) bmax[j] = p.GetElement(j);
			}
		}

		btScalar dx = bmax[0] - bmin[0];
		btScalar dy = bmax[1] - bmin[1];
		btScalar dz = bmax[2] - bmin[2];

		if ((float)dx < EPSILON || (float)dy < EPSILON || (float)dz < EPSILON || vcount < 3)
		{
			btScalar cx = dx * btScalar(0.5) + bmin[0];
			btScalar cy = dy * btScalar(0.5) + bmin[1];
			btScalar cz = dz * btScalar(0.5) + bmin[2];

			btScalar len = FLT_MAX;

			if ((float)dx >= EPSILON && (float)dx < len) len = dx;
			if ((float)dy >= EPSILON && (float)dy < len) len = dy;
			if ((float)dz >= EPSILON && (float)dz < len) len = dz;

			if (len == FLT_MAX)
			{
				dx = dy = dz = btScalar(0.01);  // one centimeter
			}
			else
			{
				if ((float)dx < EPSILON) dx = len * btScalar(0.05);  // 1/5th the shortest non-zero edge.
				if ((float)dy < EPSILON) dy = len * btScalar(0.05);
				if ((float)dz < EPSILON) dz = len * btScalar(0.05);
			}

			btScalar x1 = cx - dx;
			btScalar x2 = cx + dx;

			btScalar y1 = cy - dy;
			btScalar y2 = cy + dy;

			btScalar z1 = cz - dz;
			btScalar z2 = cz + dz;

			vcount = 0;  // add box

			addPoint(vcount, vertices, x1, y1, z1);
			addPoint(vcount, vertices, x2, y1, z1);
			addPoint(vcount, vertices, x2, y2, z1);
			addPoint(vcount, vertices, x1, y2, z1);
			addPoint(vcount, vertices, x1, y1, z2);
			addPoint(vcount, vertices, x2, y1, z2);
			addPoint(vcount, vertices, x2, y2, z2);
			addPoint(vcount, vertices, x1, y2, z2);

			return true;
		}
	}

	return true;
}

void HullLibrary::BringOutYourDead(const btVector3* verts, unsigned int vcount, btVector3* overts, unsigned int& ocount, unsigned int* indices, unsigned indexcount)
{
	AlignedObjectArray<int> tmpIndices;
	tmpIndices.resize(m_vertexIndexMapping.size());
	int i;

	for (i = 0; i < m_vertexIndexMapping.size(); i++)
	{
		tmpIndices[i] = m_vertexIndexMapping[i];
	}

	TUIntArray usedIndices;
	usedIndices.resize(static_cast<int>(vcount));
	memset(&usedIndices[0], 0, sizeof(unsigned int) * vcount);

	ocount = 0;

	for (i = 0; i < int(indexcount); i++)
	{
		unsigned int v = indices[i];  // original array index

		assert(v >= 0 && v < vcount);

		if (usedIndices[static_cast<int>(v)])  // if already remapped
		{
			indices[i] = usedIndices[static_cast<int>(v)] - 1;  // index to new array
		}
		else
		{
			indices[i] = ocount;  // new index mapping

			overts[ocount].SetX(verts[v].GetX());  // copy old vert to new vert array
			overts[ocount].SetY(verts[v].GetY());
			overts[ocount].SetZ(verts[v].GetZ());

			for (int k = 0; k < m_vertexIndexMapping.size(); k++)
			{
				if (tmpIndices[k] == int(v))
					m_vertexIndexMapping[k] = ocount;
			}

			ocount++;  // increment output vert count

			assert(ocount >= 0 && ocount <= vcount);

			usedIndices[static_cast<int>(v)] = ocount;  // assign new index remapping
		}
	}
}
#pragma endregion ConvexHull
