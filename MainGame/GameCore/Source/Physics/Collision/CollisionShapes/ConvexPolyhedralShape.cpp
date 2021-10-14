//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexPolyhedronShape.hpp
///             @brief  ConvexPolyhedronShape
///             @author ---------------
///             @date   2021_09_18
///                     2021_10_05 (first checked) first checked -> no debug.
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2011 Advanced Micro Devices, Inc.  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

///This file was written by Erwin Coumans

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionShapes/ConvexPolyhedralShape.hpp"
#include "GameMath/Include/GMHashMap.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
struct InternalVertexPair
{
	short int V0;
	short int V1;
	InternalVertexPair(short int v0, short int v1) : V0(v0), V1(v1)
	{
		if (V1 > V0) { std::swap(V0, V1); }
	}
	int GetHash() const {return V0 + (V1 << 16);}
	bool equals(const InternalVertexPair& other) const{ return V0 == other.V0 && V1 == other.V1; }
};

struct InternalEdge
{
	short int Face0;
	short int Face1;
	InternalEdge() : Face0(-1), Face1(-1) {};
};
//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
inline bool IsAlmostZero(const Vector3& v)
{
	if ((float)Abs(v.GetX()) > 1e-6 || (float)Abs(v.GetY()) > 1e-6 || (float)Abs(v.GetZ()) > 1e-6) return false;
	return true;
}

#pragma region ConvexPolyheadron
void ConvexPolyhedron::Initialize()
{
	HashMap<InternalVertexPair, InternalEdge> edges;
	for (int i = 0; i < Faces.size(); ++i)
	{
		int numVertices = Faces[i].Indices.size();
		int nbTris      = numVertices;
		for (int j = 0; j < nbTris; j++)
		{
			int k = (j + 1) % numVertices;
			InternalVertexPair vp(Faces[i].Indices[j], Faces[i].Indices[k]);
			InternalEdge* edptr = edges.find(vp);
			Vector3 edge        = Vertices[vp.V1] - Vertices[vp.V0];
			edge                = Normalize(edge);

			bool found = false;

			for (int p = 0; p < UniqueEdges.size(); p++)
			{
				if (IsAlmostZero(UniqueEdges[p] - edge) ||
					IsAlmostZero(UniqueEdges[p] + edge))
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				UniqueEdges.push_back(edge);
			}

			if (edptr)
			{
				assert(edptr->Face0 >= 0);
				assert(edptr->Face1 < 0);
				edptr->Face1 = i;
			}
			else
			{
				InternalEdge ed;
				ed.Face0 = i;
				edges.insert(vp, ed);
			}
		}
	}

	Initialize2();
}
void ConvexPolyhedron::Initialize2()
{
	LocalCenter      = Vector3(0, 0, 0);
	Scalar TotalArea = 0.0f;
	for (int i = 0; i < Faces.size(); i++)
	{
		int numVertices = Faces[i].Indices.size();
		int NbTris = numVertices - 2;

		const Vector3& p0 = Vertices[Faces[i].Indices[0]];
		for (int j = 1; j <= NbTris; j++)
		{
			int k = (j + 1) % numVertices;
			const Vector3& p1 = Vertices[Faces[i].Indices[j]];
			const Vector3& p2 = Vertices[Faces[i].Indices[k]];
			Scalar Area       = Norm(Cross((p0 - p1), (p0 - p2))) * 0.5f;
			Vector3 Center = (p0 + p1 + p2) / 3.0f;
			LocalCenter   += Area * Center;
			TotalArea      = TotalArea + Area;
		}
	}
	LocalCenter /= TotalArea;

#ifdef TEST_INTERNAL_OBJECTS
	if (1)
	{
		Radius = FLT_MAX;
		for (int i = 0; i < Faces.size(); i++)
		{
			const Vector3 Normal(Faces[i].Plane[0], Faces[i].Plane[1], Faces[i].Plane[2]);
			const Scalar dist = Abs(Dot(LocalCenter, Normal) + Faces[i].Plane[3]);
			if (dist < Radius)
				Radius = dist;
		}

		float MinX = FLT_MAX;
		float MinY = FLT_MAX;
		float MinZ = FLT_MAX;
		float MaxX = -FLT_MAX;
		float MaxY = -FLT_MAX;
		float MaxZ = -FLT_MAX;
		for (int i = 0; i < Vertices.size(); i++)
		{
			const Vector3& pt = Vertices[i];
			if (pt.GetX() < MinX) MinX = pt.GetX();
			if (pt.GetX() > MaxX) MaxX = pt.GetX();
			if (pt.GetY() < MinY) MinY = pt.GetY();
			if (pt.GetY() > MaxY) MaxY = pt.GetY();
			if (pt.GetZ() < MinZ) MinZ = pt.GetZ();
			if (pt.GetZ() > MaxZ) MaxZ = pt.GetZ();
		}
		C = Vector3(MaxX + MinX, MaxY + MinY, MaxZ + MinZ);
		E = Vector3(MaxX - MinX, MaxY - MinY, MaxZ - MinZ);

		//		const btScalar r = m_radius / sqrtf(2.0f);
		const Scalar r = Radius / sqrtf(3.0f);
		const int LargestExtent = (int)MaxAxis3(E);
		const Scalar Step       = (E.GetElement(LargestExtent) * 0.5f - r) / 1024.0f;
		Extents.SetX(r); Extents.SetY(r); Extents.SetZ(r);
		Extents.SetElement(LargestExtent, E.GetElement(LargestExtent) * 0.5f);
		bool FoundBox = false;
		for (int j = 0; j < 1024; j++)
		{
			if (TestContainment())
			{
				FoundBox = true;
				break;
			}

			Extents.SetElement(LargestExtent, Extents.GetElement(LargestExtent) - Step);
		}
		if (!FoundBox)
		{
			Extents.SetX(r); Extents.SetY(r); Extents.SetZ(r);
		}
		else
		{
			// Refine the box
			const Scalar Step = (Radius - r) / 1024.0f;
			const int e0 = (1 << LargestExtent) & 3;
			const int e1 = (1 << e0) & 3;

			for (int j = 0; j < 1024; j++)
			{
				const Scalar Saved0 = Extents.GetElement(e0);
				const Scalar Saved1 = Extents.GetElement(e1);
				Extents.SetElement(e0, Extents.GetElement(e0) + Step);
				Extents.SetElement(e1, Extents.GetElement(e1) + Step);
				if (!TestContainment())
				{
					Extents.SetElement(e0,Saved0);
					Extents.SetElement(e1, Saved1);
					break;
				}
			}
		}
	}
#endif
}
bool ConvexPolyhedron::TestContainment() const
{
	for (int p = 0; p < 8; p++)
	{
		Vector3 LocalPt;
		if (p == 0)
			LocalPt = LocalCenter + Vector3(Extents.GetX(), Extents.GetY(), Extents.GetZ());
		else if (p == 1)
			LocalPt = LocalCenter + Vector3(Extents.GetX(), Extents.GetY(), -Extents.GetZ());
		else if (p == 2)
			LocalPt = LocalCenter + Vector3(Extents.GetX(), -Extents.GetY(), Extents.GetZ());
		else if (p == 3)
			LocalPt = LocalCenter + Vector3(Extents.GetX(), -Extents.GetY(), -Extents.GetZ());
		else if (p == 4)
			LocalPt = LocalCenter + Vector3(-Extents.GetX(), Extents.GetY(), Extents.GetZ());
		else if (p == 5)
			LocalPt = LocalCenter + Vector3(-Extents.GetX(), Extents.GetY(), -Extents.GetZ());
		else if (p == 6)
			LocalPt = LocalCenter + Vector3(-Extents.GetX(), -Extents.GetY(), Extents.GetZ());
		else if (p == 7)
			LocalPt = LocalCenter + Vector3(-Extents.GetX(), -Extents.GetY(), -Extents.GetZ());

		for (int i = 0; i < Faces.size(); i++)
		{
			const Vector3 Normal(Faces[i].Plane[0], Faces[i].Plane[1], Faces[i].Plane[2]);
			const Scalar d = Dot(LocalPt, Normal) + Faces[i].Plane[3];
			if (d > 0.0f)
				return false;
		}
	}
	return true;
}
void ConvexPolyhedron::Project(const AffineTransform& transform, const Vector3& direction, float& minProj, float& maxProj, Vector3& witnesPtMin, Vector3& witnesPtMax) const
{
	minProj = FLT_MAX;
	maxProj = -FLT_MAX;
	int numVerts = Vertices.size();
	for (int i = 0; i < numVerts; i++)
	{
		Vector3 pt = transform *  Vertices[i];
		Scalar dp = Dot(pt, direction);
		if (dp < minProj)
		{
			minProj = dp;
			witnesPtMin = pt;
		}
		if (dp > maxProj)
		{
			maxProj = dp;
			witnesPtMax = pt;
		}
	}
	if (minProj > maxProj)
	{
		float temp = minProj; minProj = maxProj; maxProj = temp;
		Swap(witnesPtMin, witnesPtMax);
	}
}
#pragma endregion ConvexPolyhedron
#pragma region PolyhedralConvexShape
/****************************************************************************
*                    LocalGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        Vector3 PolyhedralConvexShape::LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const
*  @brief     LocalGetSupportingVertexWithoutMargin
*  @param[in] Vector3* vectors,
*  @return 　　Vector3
*****************************************************************************/
Vector3 PolyhedralConvexShape::LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const
{
	Vector3 supVec(0, 0, 0);
#ifndef __SPU__
	int i;
	Scalar maxDot(-FLT_MAX);

	Vector3 vec   = vector;
	Scalar lenSqr = NormSquared(vec);
	if (lenSqr < 0.0001f)
	{
		vec = Vector3(1, 0, 0);
	}
	else
	{
		Scalar rlen = Scalar(1.0f) / Sqrt(lenSqr);
		vec *= rlen;
	}

	Vector3 vtx;
	Scalar newDot;

	for (int k = 0; k < GetVerticesCount(); k += 128)
	{
		Vector3 temp[128];
		int inner_count = static_cast<int>(Min(GetVerticesCount() - k, 128));
		for (i = 0; i < inner_count; i++)
			GetVertex(i, temp[i]);
		i = (int)MaxDot(vec, temp, inner_count, newDot);
		if ((float)newDot > maxDot)
		{
			maxDot = newDot;
			supVec = temp[i];
		}
	}
#endif
	return supVec;
}

/****************************************************************************
*                       BatchedUnitVectorGetSupportingVertexWithoutMargin
*************************************************************************//**
*  @fn        void PolyhedralConvexShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
*  @brief     GetSupportingVertex
*  @param[in] Vector3* vectors,
*  @param[out] Vector3* verticesOut
*  @param[in]  int numVector
*  @return 　　void
*****************************************************************************/
void PolyhedralConvexShape::BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
{
#ifndef __SPU__
	int i;

	Vector3 vertex;
	Scalar newDot;
	std::vector<float> supports (numVectors, -FLT_MAX);

	for (int j = 0; j < numVectors; ++j)
	{
		const Vector3& vector = vectors[j];
		for (int k = 0; k < GetVerticesCount(); k += 128)
		{
			Vector3 temp[128];
			int innerCount = static_cast<int>(Min(GetVerticesCount() - k, 128));
			for (i = 0; i < innerCount; ++i)
			{
				GetVertex(i, temp[i]);
			}
			i = (int)MaxDot(vector, temp, innerCount, newDot);
			if ((float)newDot > supports[j])
			{
				supportVerticesOut[j] = temp[i];
				supports[j] = newDot;  // 必要なら変更
			}
		}
	}
#endif
}

/****************************************************************************
*                       CalculateLocalInertia
*************************************************************************//**
*  @fn        void PolyhedralConvexShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
*  @brief     Calculate local inertia 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PolyhedralConvexShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
{
#ifndef __SPU__
	float margin = GetMargin();

	AffineTransform identity;
	Vector3 aabbMin, aabbMax;
	GetAABB(identity, aabbMin, aabbMax);
	Vector3 halfExtents = (aabbMax - aabbMin) * 0.5f;

	float lx = 2.0f * (halfExtents.GetX() + margin);
	float ly = 2.0f * (halfExtents.GetY() + margin);
	float lz = 2.0f * (halfExtents.GetZ() + margin);
	const float x2 = lx * lx;
	const float y2 = ly * ly;
	const float z2 = lz * lz;
	const float scaledmass = mass * 0.08333333f;
#endif
}
#pragma endregion PolyhedralConvexShape

#pragma region PolyhedralConvexAABBCachingShape
/****************************************************************************
*                       ReCalculateLocalAABB
*************************************************************************//**
*  @fn        void PolyhedralConvexAABBCachingShape::ReCalculateLocalAABB()
*  @brief     Recalculate local aabb
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PolyhedralConvexAABBCachingShape::ReCalculateLocalAABB()
{
	_isLocalAABBValid = true;
	
	static const Vector3 _directions[] =
	{
		Vector3(1., 0., 0.),
		Vector3(0., 1., 0.),
		Vector3(0., 0., 1.),
		Vector3(-1., 0., 0.),
		Vector3(0., -1., 0.),
		Vector3(0., 0., -1.) };

	Vector3 _supporting[] =
	{
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.),
		Vector3(0., 0., 0.) };

	BatchedUnitVectorGetSupportingVertexWithoutMargin(_directions, _supporting, 6);

	for (int i = 0; i < 3; ++i)
	{
		_localAABBMax.SetElement(i, _supporting[i].GetElement(i)+ _collisionMargin);
		_localAABBMin.SetElement(i, _supporting[i + 3].GetElement(i) - _collisionMargin);
	}
}
#pragma endregion PolyhedralConvexAABBCachingShape