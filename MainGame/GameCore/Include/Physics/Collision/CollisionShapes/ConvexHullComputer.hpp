//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexHullShape.hpp
///             @brief  ConvexHullShape
///             @author Partially edit by Toide
///             @date   2021_09_20
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

#pragma once
#ifndef CONVEX_HULL_COMPUTER_HPP
#define CONVEX_HULL_COMPUTER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"
#include "GameMath/Include/GMVector.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

class ConvexHullComputer
{
public:
	class Edge
	{
	private:
		int _next;
		int _reverse;
		int _targetVertex;

		friend class ConvexHullComputer;

	public:
		int GetSourceVertex() const { return (this + _reverse)->_targetVertex; }
		int GetTargetVertex() const { return _targetVertex; }
		const Edge* GetNextEdgeOfVertex() const  // clockwise list of all edges of a vertex
		{
			return this + _next;
		}
		const Edge* GetNextEdgeOfFace() const  // counter-clockwise list of all edges 
		{
			return (this + _reverse)->GetNextEdgeOfVertex();
		}
		const Edge* GetReverseEdge() const { return this + _reverse; }
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	float Compute(const float* coords, int stride, int count, float shrink, float shrinkClamp)
	{
		return Compute(coords, false, stride, count, shrink, shrinkClamp);
	}

	float Compute(const gm::Vector3* coords, int stride, int count, float shrink, float shrinkClamp)
	{
		return Compute(coords, false, stride, count, shrink, shrinkClamp);
	}
	// same as above, but double precision
	float Compute(const double* coords, int stride, int count, float shrink, float shrinkClamp)
	{
		return Compute(coords, true, stride, count, shrink, shrinkClamp);
	}
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<gm::Vector3> Vertices;       // Vertices of the output hull
	gm::AlignedObjectArray<int> OriginalVertexIndex; 	// The original vertex index in the input coords array
	gm::AlignedObjectArray<Edge> Edges; 	            // Edges of the output hull
	gm::AlignedObjectArray<int> Faces; // Faces of the convex hull. Each entry is an index into the "edges" array pointing to an edge of the face. Faces are planar n-gons
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
private:
	float Compute(const void* cooards, bool doubleCoords, int stride, int count, float shrink, float shrinkClamp);
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};


typedef gm::AlignedObjectArray<unsigned int> TUIntArray;

class HullResult
{
public:
	HullResult(void)
	{
		Polygons = true;
		NumOutputVertices = 0;
		NumFaces = 0;
		NumIndices = 0;
	}
	bool Polygons;                                    // true if indices represents polygons, false indices are triangles
	unsigned int NumOutputVertices;                   // number of vertices in the output hull
	gm::AlignedObjectArray<gm::Vector3> OutputVertices;  // array of vertices
	unsigned int NumFaces;                            // the number of faces produced
	unsigned int NumIndices;                          // the total number of indices
	gm::AlignedObjectArray<unsigned int> m_Indices;      // pointer to indices.

	// If triangles, then indices are array indexes into the vertex list.
	// If polygons, indices are in the form (number of points in face) (p1, p2, p3, ..) etc..
};

enum HullFlag
{
	QF_TRIANGLES = (1 << 0),      // report results as triangles, not polygons.
	QF_REVERSE_ORDER = (1 << 1),  // reverse order of the triangle indices.
	QF_DEFAULT = QF_TRIANGLES
};

class HullDesc
{
public:
	HullDesc(void)
	{
		Flags = QF_DEFAULT;
		Vcount = 0;
		Vertices = 0;
		VertexStride = sizeof(gm::Vector3);
		NormalEpsilon = 0.001f;
		MaxVertices = 4096;  // maximum number of points to be considered for a convex hull.
		MaxFaces = 4096;
	};

	HullDesc(HullFlag flag,
		unsigned int vcount,
		const gm::Vector3* vertices,
		unsigned int stride = sizeof(gm::Vector3))
	{
		Flags         = flag;
		Vcount        = vcount;
		Vertices      = vertices;
		VertexStride  = stride;
		NormalEpsilon = 0.001f;
		MaxVertices = 4096;
	}

	bool HasHullFlag(HullFlag flag) const
	{
		if (Flags & flag) return true;
		return false;
	}

	void SetHullFlag(HullFlag flag)
	{
		Flags |= flag;
	}

	void ClearHullFlag(HullFlag flag)
	{
		Flags &= ~flag;
	}

	unsigned int Flags;         // flags to use when generating the convex hull.
	unsigned int Vcount;        // number of vertices in the input point cloud
	const gm::Vector3* Vertices;  // the array of vertices.
	unsigned int VertexStride;  // the stride of each vertex, in bytes.
	gm::Scalar NormalEpsilon;     // the epsilon for removing duplicates.  This is a normalized value, if normalized bit is on.
	unsigned int MaxVertices;   // maximum number of vertices to be considered for the hull!
	unsigned int MaxFaces;
};

enum HullError
{
	QE_OK,   // success!
	QE_FAIL  // failed.
};

class btPlane
{
public:
	gm::Vector3 normal;
	gm::Scalar dist;  // distance below origin - the D from plane equasion Ax+By+Cz+D=0
	btPlane(const gm::Vector3& n, gm::Scalar d) : normal(n), dist(d) {}
	btPlane() : normal(), dist(0) {}
};

class ConvexH
{
public:
	class HalfEdge
	{
	public:
		short ea;         // the other half of the edge (index into edges list)
		unsigned char v;  // the vertex at the start of this edge (index into vertices list)
		unsigned char p;  // the facet on which this edge lies (index into facets list)
		HalfEdge() {}
		HalfEdge(short _ea, unsigned char _v, unsigned char _p) : ea(_ea), v(_v), p(_p) {}
	};
	ConvexH()
	{
	}
	~ConvexH()
	{
	}
	gm::AlignedObjectArray<gm::Vector3> vertices;
	gm::AlignedObjectArray<HalfEdge> edges;
	gm::AlignedObjectArray<btPlane> facets;
	ConvexH(int vertices_size, int edges_size, int facets_size);
};

class int4
{
public:
	int x, y, z, w;
	int4() {};
	int4(int _x, int _y, int _z, int _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	const int& operator[](int i) const { return (&x)[i]; }
	int& operator[](int i) { return (&x)[i]; }
};

class PHullResult
{
public:
	PHullResult(void)
	{
		mVcount = 0;
		mIndexCount = 0;
		mFaceCount = 0;
		mVertices = 0;
	}

	unsigned int mVcount;
	unsigned int mIndexCount;
	unsigned int mFaceCount;
	gm::Vector3* mVertices;
	TUIntArray m_Indices;
};

///The HullLibrary class can create a convex hull from a collection of vertices, using the ComputeHull method.
///The btShapeHull class uses this HullLibrary to create a approximate convex mesh given a general (non-polyhedral) convex shape.
class HullLibrary
{
	gm::AlignedObjectArray<class btHullTriangle*> m_tris;

public:
	gm::AlignedObjectArray<int> m_vertexIndexMapping;

	HullError CreateConvexHull(const HullDesc& desc,  // describes the input request
		HullResult& result);   // contains the resulst
	HullError ReleaseResult(HullResult& result);      // release memory allocated for this result, we are done with it.

private:
	bool ComputeHull(unsigned int vcount, const gm::Vector3* vertices, PHullResult& result, unsigned int vlimit);

	class btHullTriangle* allocateTriangle(int a, int b, int c);
	void deAllocateTriangle(btHullTriangle*);
	void b2bfix(btHullTriangle* s, btHullTriangle* t);

	void removeb2b(btHullTriangle* s, btHullTriangle* t);

	void checkit(btHullTriangle* t);

	btHullTriangle* extrudable(gm::Scalar epsilon);

	int calchull(gm::Vector3* verts, int verts_count, TUIntArray& tris_out, int& tris_count, int vlimit);

	int calchullgen(gm::Vector3* verts, int verts_count, int vlimit);

	int4 FindSimplex(gm::Vector3* verts, int verts_count, gm::AlignedObjectArray<int>& allow);

	class ConvexH* ConvexHCrop(ConvexH& convex, const btPlane& slice);

	void extrude(class btHullTriangle* t0, int v);

	ConvexH* test_cube();

	//BringOutYourDead (John Ratcliff): When you create a convex hull you hand it a large input set of vertices forming a 'point cloud'.
	//After the hull is generated it give you back a set of polygon faces which index the *original* point cloud.
	//The thing is, often times, there are many 'dead vertices' in the point cloud that are on longer referenced by the hull.
	//The routine 'BringOutYourDead' find only the referenced vertices, copies them to an new buffer, and re-indexes the hull so that it is a minimal representation.
	void BringOutYourDead(const gm::Vector3* verts, unsigned int vcount, gm::Vector3* overts, unsigned int& ocount, unsigned int* indices, unsigned indexcount);

	bool CleanupVertices(unsigned int svcount,
		const gm::Vector3* svertices,
		unsigned int stride,
		unsigned int& vcount,  // output number of vertices
		gm::Vector3* vertices,   // location to store the results.
		gm::Scalar normalepsilon,
		gm::Vector3& scale);
};

#endif
