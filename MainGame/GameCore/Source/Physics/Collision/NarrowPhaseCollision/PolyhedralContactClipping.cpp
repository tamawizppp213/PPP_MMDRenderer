//////////////////////////////////////////////////////////////////////////////////
//            @file　　  PolyhedralContactClipping.hpp
//            @brief    Polyhedral Contact cip
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_12 (first checked)
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
///Separating axis rest based on work from Pierre Terdiman, see
///And contact clipping based on work from Simon Hobbs


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/PolyhedralContactClipping.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/ConvexPolyhedralShape.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

int        g_ExpectedNbTests    = 0;
int        g_ActualNbTests      = 0;
bool       g_UseInternalObject  = true;
static int g_ActualSATPairTests = 0;
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
static bool TestSeparateAxis(const ConvexPolyhedron& hullA, const ConvexPolyhedron& hullB, const AffineTransform& transA, const AffineTransform& transB, const Vector3& sep_axis, float& depth, Vector3& witnessPointA, Vector3& witnessPointB)
{
	float Min0, Max0;
	float Min1, Max1;
	Vector3 witnesPtMinA, witnesPtMaxA;
	Vector3 witnesPtMinB, witnesPtMaxB;

	hullA.Project(transA, sep_axis, Min0, Max0, witnesPtMinA, witnesPtMaxA);
	hullB.Project(transB, sep_axis, Min1, Max1, witnesPtMinB, witnesPtMaxB);

	if (Max0 < Min1 || Max1 < Min0)
		return false;

	float d0 = Max0 - Min1; assert(d0 >= 0.0f);
	float d1 = Max1 - Min0; assert(d1 >= 0.0f);
	if (d0 < d1)
	{
		depth = d0;
		witnessPointA = witnesPtMaxA;
		witnessPointB = witnesPtMinB;
	}
	else
	{
		depth = d1;
		witnessPointA = witnesPtMinA;
		witnessPointB = witnesPtMaxB;
	}

	return true;
}

inline bool IsAlmostZero(const Vector3& v)
{
	if ((float)Abs(v.GetX()) > 1e-6 || (float)Abs(v.GetY()) > 1e-6 || (float)Abs(v.GetZ()) > 1e-6) return false;

	return true;
}
#ifdef TEST_INTERNAL_OBJECTS

inline void BoxSupport(const Vector3& extents, const Vector3& sv, Vector3& p)
{
	p.SetX(sv.GetX() < 0.0f ? -extents.GetX() : extents.GetX());
	p.SetY(sv.GetY() < 0.0f ? -extents.GetY() : extents.GetY());
	p.SetZ(sv.GetZ() < 0.0f ? -extents.GetZ() : extents.GetZ());
}

void InverseTransformPoint3x3(Vector3& out, const Vector3& in, const AffineTransform& tr)
{
	const Matrix3& rot = tr.GetBasis();
	const Vector3& r0 = rot.GetX();
	const Vector3& r1 = rot.GetY();
	const Vector3& r2 = rot.GetZ();

	const float x = r0.GetX() * in.GetX() + r1.GetX() * in.GetY() + r2.GetX() * in.GetZ();
	const float y = r0.GetY() * in.GetX() + r1.GetY() * in.GetY() + r2.GetY() * in.GetZ();
	const float z = r0.GetZ() * in.GetX() + r1.GetZ() * in.GetY() + r2.GetZ() * in.GetZ();

	out = Vector3(x, y, z);
}

bool TestInternalObjects(const AffineTransform& trans0, const AffineTransform& trans1, const Vector3& delta_c, const Vector3& axis, const ConvexPolyhedron& convex0, const ConvexPolyhedron& convex1, float dmin)
{
	const float dp = Dot(delta_c, axis);

	Vector3 localAxis0; 
	InverseTransformPoint3x3(localAxis0, axis, trans0);

	Vector3 localAxis1;
	InverseTransformPoint3x3(localAxis1, axis, trans1);

	Vector3 p0; BoxSupport(convex0.Extents, localAxis0, p0);
	Vector3 p1; BoxSupport(convex1.Extents, localAxis1, p1);

	const float Radius0 = p0.GetX() * localAxis0.GetX() + p0.GetY() * localAxis0.GetY() + p0.GetZ() * localAxis0.GetZ();
	const float Radius1 = p1.GetX() * localAxis1.GetX() + p1.GetY() * localAxis1.GetY() + p1.GetZ() * localAxis1.GetZ();

	const float MinRadius = Radius0 > convex0.Radius ? Radius0 : convex0.Radius;
	const float MaxRadius = Radius1 > convex1.Radius ? Radius1 : convex1.Radius;

	const float MinMaxRadius = MaxRadius + MinRadius;
	const float d0 = MinMaxRadius + dp;
	const float d1 = MinMaxRadius - dp;

	const float depth = d0 < d1 ? d0 : d1;
	if (depth > dmin)
		return false;
	return true;
}
INLINE void SegmentsClosestPoints( Vector3& ptsVector, Vector3& offsetA, Vector3& offsetB, Scalar& tA, Scalar& tB, const Vector3& translation, const Vector3& dirA, Scalar hlenA, const Vector3& dirB, Scalar hlenB)
{
	// compute the parameters of the closest points on each line segment

	Scalar dirA_dot_dirB  = Dot(dirA, dirB);
	Scalar dirA_dot_trans = Dot(dirA, translation);
	Scalar dirB_dot_trans = Dot(dirB, translation);

	Scalar denom = 1.0f - dirA_dot_dirB * dirA_dot_dirB;

	if (denom == 0.0f)
	{
		tA = 0.0f;
	}
	else
	{
		tA = (dirA_dot_trans - dirB_dot_trans * dirA_dot_dirB) / denom;
		if ((float)tA < -hlenA)
			tA = -hlenA;
		else if ((float)tA > hlenA)
			tA = hlenA;
	}

	tB = tA * dirA_dot_dirB - dirB_dot_trans;

	if ((float)tB < -hlenB)
	{
		tB = -hlenB;
		tA = tB * dirA_dot_dirB + dirA_dot_trans;

		if ((float)tA < -hlenA)
			tA = -hlenA;
		else if ((float)tA > hlenA)
			tA = hlenA;
	}
	else if ((float)tB > hlenB)
	{
		tB = hlenB;
		tA = tB * dirA_dot_dirB + dirA_dot_trans;

		if ((float)tA < -hlenA)
			tA = -hlenA;
		else if ((float)tA > hlenA)
			tA = hlenA;
	}

	// compute the closest points relative to segment centers.

	offsetA = dirA * tA;
	offsetB = dirB * tB;

	ptsVector = translation - offsetA + offsetB;
}
#endif  //TEST_INTERNAL_OBJECTS
void PolyhedralContactClipping::ClipHullAgainstHull(const Vector3& separatingNormal1, const ConvexPolyhedron& hullA, const ConvexPolyhedron& hullB, const gm::AffineTransform& transformA, const gm::AffineTransform& transformB, const float minDist, float maxDist, VertexArray& worldVertexB1, VertexArray& worldVertexB2, DiscreteCollisionDetectorInterface::Result& resultOut)
{
	Vector3 separatingNormal = Normalize(separatingNormal1);
	//	const btVector3 c0 = transA * hullA.m_localCenter;
	//	const btVector3 c1 = transB * hullB.m_localCenter;
	//const btVector3 DeltaC2 = c0 - c1;

	int closestFaceB = -1;
	float dmax = -FLT_MAX;
	{
		for (int face = 0; face < hullB.Faces.size(); face++)
		{
			const Vector3 Normal(hullB.Faces[face].Plane[0], hullB.Faces[face].Plane[1], hullB.Faces[face].Plane[2]);
			const Vector3 WorldNormal = transformB.GetBasis() * Normal;
			Scalar d = Dot(WorldNormal, separatingNormal);
			if (d > dmax)
			{
				dmax = d;
				closestFaceB = face;
			}
		}
	}
	worldVertexB1.resize(0);
	{
		const ConvexFace& polyB = hullB.Faces[closestFaceB];
		const int numVertices = polyB.Indices.size();
		for (int e0 = 0; e0 < numVertices; e0++)
		{
			const Vector3& b = hullB.Vertices[polyB.Indices[e0]];
			worldVertexB1.push_back(transformB * b);
		}
	}

	if (closestFaceB >= 0)
		ClipFaceAgainstHull(separatingNormal, hullA, transformA, worldVertexB1, worldVertexB2, minDist, maxDist, resultOut);
}
void PolyhedralContactClipping::ClipFaceAgainstHull(const Vector3& separatingNormal, const ConvexPolyhedron& hullA, const gm::AffineTransform& transformA, VertexArray& worldVertexB1, VertexArray& worldVertexB2, const float minDist, float maxDist, DiscreteCollisionDetectorInterface::Result& resultOut)
{
	worldVertexB2.resize(0);
	VertexArray* pVtxIn  = &worldVertexB1;
	VertexArray* pVtxOut = &worldVertexB2;
	pVtxOut->reserve(pVtxIn->size());

	int closestFaceA = -1;
	{
		float dmin = FLT_MAX;
		for (int face = 0; face < hullA.Faces.size(); face++)
		{
			const Vector3 Normal(hullA.Faces[face].Plane[0], hullA.Faces[face].Plane[1], hullA.Faces[face].Plane[2]);
			const Vector3 faceANormalWS = transformA.GetBasis() * Normal;

			Scalar d = Dot(faceANormalWS, separatingNormal);
			if (d < dmin)
			{
				dmin = d;
				closestFaceA = face;
			}
		}
	}
	if (closestFaceA < 0)
		return;

	const ConvexFace& polyA = hullA.Faces[closestFaceA];

	// clip polygon to back of planes of all faces of hull A that are adjacent to witness face
	int numVerticesA = polyA.Indices.size();
	for (int e0 = 0; e0 < numVerticesA; e0++)
	{
		const Vector3& a = hullA.Vertices[polyA.Indices[e0]];
		const Vector3& b = hullA.Vertices[polyA.Indices[(e0 + 1) % numVerticesA]];
		const Vector3 edge0 = a - b;
		const Vector3 WorldEdge0 = transformA.GetBasis() * edge0;
		Vector3 worldPlaneAnormal1 = transformA.GetBasis() * Vector3(polyA.Plane[0], polyA.Plane[1], polyA.Plane[2]);

		Vector3 planeNormalWS1 = -Cross(WorldEdge0, worldPlaneAnormal1);  //.cross(WorldEdge0);
		Vector3 worldA1   = transformA * a;
		Scalar planeEqWS1 = -Dot(worldA1, planeNormalWS1);

		//int otherFace=0;
#ifdef BLA1
		int otherFace = polyA.m_connectedFaces[e0];
		btVector3 localPlaneNormal(hullA.m_faces[otherFace].m_plane[0], hullA.m_faces[otherFace].m_plane[1], hullA.m_faces[otherFace].m_plane[2]);
		btScalar localPlaneEq = hullA.m_faces[otherFace].m_plane[3];

		btVector3 planeNormalWS = transA.getBasis() * localPlaneNormal;
		btScalar planeEqWS = localPlaneEq - planeNormalWS.dot(transA.getOrigin());
#else
		Vector3 planeNormalWS = planeNormalWS1;
		Scalar planeEqWS = planeEqWS1;

#endif
		//clip face

		ClipFace(*pVtxIn, *pVtxOut, planeNormalWS, planeEqWS);
		std::swap(pVtxIn, pVtxOut);
		pVtxOut->resize(0);
	}

	//#define ONLY_REPORT_DEEPEST_POINT

	Vector3 point;

	// only keep points that are behind the witness face
	{
		Vector3 localPlaneNormal(polyA.Plane[0], polyA.Plane[1], polyA.Plane[2]);
		Scalar localPlaneEq   = polyA.Plane[3];
		Vector3 planeNormalWS = transformA.GetBasis() * localPlaneNormal;
		Scalar planeEqWS      = localPlaneEq - Dot(planeNormalWS, transformA.GetTranslation());
		for (int i = 0; i < pVtxIn->size(); i++)
		{
			Vector3 vtx = pVtxIn->at(i);
			Scalar depth = Dot(planeNormalWS, vtx) + planeEqWS;
			if (depth <= minDist)
			{
				//				printf("clamped: depth=%f to minDist=%f\n",depth,minDist);
				depth = minDist;
			}

			if (depth <= maxDist)
			{
				Vector3 point = pVtxIn->at(i);
#ifdef ONLY_REPORT_DEEPEST_POINT
				curMaxDist = depth;
#else
#if 0
				if (depth < -3)
				{
					printf("error in btPolyhedralContactClipping depth = %f\n", depth);
					printf("likely wrong separatingNormal passed in\n");
				}
#endif
				resultOut.AddContactPoint(separatingNormal, point, depth);
#endif
			}
		}
	}
#ifdef ONLY_REPORT_DEEPEST_POINT
	if (curMaxDist < maxDist)
	{
		resultOut.AddContactPoint(separatingNormal, point, curMaxDist);
	}
#endif  //ONLY_REPORT_DEEPEST_POINT
}
bool PolyhedralContactClipping::FindSeparatingAxis(const ConvexPolyhedron& hullA, const ConvexPolyhedron& hullB, const AffineTransform& transformA, const gm::AffineTransform& transformB, gm::Vector3& sep, DiscreteCollisionDetectorInterface::Result& resultOut)
{
	g_ActualSATPairTests++;

	//#ifdef TEST_INTERNAL_OBJECTS
	const Vector3 c0 = transformA * hullA.LocalCenter;
	const Vector3 c1 = transformB * hullB.LocalCenter;
	const Vector3 DeltaC2 = c0 - c1;
	//#endif

	float dmin = FLT_MAX;
	int curPlaneTests = 0;

	int numFacesA = hullA.Faces.size();
	// Test normals from hullA
	for (int i = 0; i < numFacesA; i++)
	{
		const Vector3 Normal(hullA.Faces[i].Plane[0], hullA.Faces[i].Plane[1], hullA.Faces[i].Plane[2]);
		Vector3 faceANormalWS = transformA.GetBasis() * Normal;
		if (Dot(DeltaC2, faceANormalWS) < 0.0f)
			faceANormalWS = (-1.0f) * faceANormalWS;

		curPlaneTests++;
#ifdef TEST_INTERNAL_OBJECTS
		g_ExpectedNbTests++;
		if (g_UseInternalObject && !TestInternalObjects(transformA, transformB, DeltaC2, faceANormalWS, hullA, hullB, dmin))
			continue;
		g_ActualNbTests++;
#endif

		float d;
		Vector3 wA, wB;
		if (!TestSeparateAxis(hullA, hullB, transformA, transformB, faceANormalWS, d, wA, wB))
			return false;

		if (d < dmin)
		{
			dmin = d;
			sep = faceANormalWS;
		}
	}

	int numFacesB = hullB.Faces.size();
	// Test normals from hullB
	for (int i = 0; i < numFacesB; i++)
	{
		const Vector3 Normal(hullB.Faces[i].Plane[0], hullB.Faces[i].Plane[1], hullB.Faces[i].Plane[2]);
		Vector3 WorldNormal = transformB.GetBasis() * Normal;
		if (Dot(DeltaC2, WorldNormal) < 0.0f)
			WorldNormal = (-1.f) * WorldNormal;

		curPlaneTests++;
#ifdef TEST_INTERNAL_OBJECTS
		g_ExpectedNbTests++;
		if (g_UseInternalObject && !TestInternalObjects(transformA, transformB, DeltaC2, WorldNormal, hullA, hullB, dmin))
			continue;
		g_ActualNbTests++;
#endif

		float d;
		Vector3 wA, wB;
		if (!TestSeparateAxis(hullA, hullB, transformA, transformB, WorldNormal, d, wA, wB))
			return false;

		if (d < dmin)
		{
			dmin = d;
			sep = WorldNormal;
		}
	}

	Vector3 edgeAstart, edgeAend, edgeBstart, edgeBend;
	int edgeA = -1;
	int edgeB = -1;
	Vector3 worldEdgeA;
	Vector3 worldEdgeB;
	Vector3 witnessPointA(0, 0, 0), witnessPointB(0, 0, 0);

	int curEdgeEdge = 0;
	// Test edges
	for (int e0 = 0; e0 < hullA.UniqueEdges.size(); e0++)
	{
		const Vector3 edge0 = hullA.UniqueEdges[e0];
		const Vector3 WorldEdge0 = transformA.GetBasis() * edge0;
		for (int e1 = 0; e1 < hullB.UniqueEdges.size(); e1++)
		{
			const Vector3 edge1 = hullB.UniqueEdges[e1];
			const Vector3 WorldEdge1 = transformB.GetBasis() * edge1;

			Vector3 cross = Cross(WorldEdge0, WorldEdge1);
			curEdgeEdge++;
			if (!IsAlmostZero(cross))
			{
				cross = Normalize(cross);
				if ((float)Dot(DeltaC2, cross) < 0.0f)
					cross = (-1.f) * cross;

#ifdef TEST_INTERNAL_OBJECTS
				g_ExpectedNbTests++;
				if (g_UseInternalObject && !TestInternalObjects(transformA, transformB, DeltaC2, cross, hullA, hullB, dmin))
					continue;
				g_ActualNbTests++;
#endif

				float dist;
				Vector3 wA, wB;
				if (!TestSeparateAxis(hullA, hullB, transformA, transformB, cross, dist, wA, wB))
					return false;

				if (dist < dmin)
				{
					dmin = dist;
					sep = cross;
					edgeA = e0;
					edgeB = e1;
					worldEdgeA = WorldEdge0;
					worldEdgeB = WorldEdge1;
					witnessPointA = wA;
					witnessPointB = wB;
				}
			}
		}
	}

	if (edgeA >= 0 && edgeB >= 0)
	{
		//		printf("edge-edge\n");
		//add an edge-edge contact

		Vector3 ptsVector;
		Vector3 offsetA;
		Vector3 offsetB;
		Scalar tA;
		Scalar tB;

		Vector3 translation = witnessPointB - witnessPointA;

		Vector3 dirA = worldEdgeA;
		Vector3 dirB = worldEdgeB;

		Scalar hlenB = 1e30f;
		Scalar hlenA = 1e30f;

		SegmentsClosestPoints(ptsVector, offsetA, offsetB, tA, tB,
			translation,
			dirA, hlenA,
			dirB, hlenB);

		Scalar nlSqrt = NormSquared(ptsVector);
		if (nlSqrt > FLT_EPSILON)
		{
			Scalar nl = Sqrt(nlSqrt);
			ptsVector *= 1.f / nl;
			if (Dot(ptsVector, DeltaC2) < 0.f)
			{
				ptsVector = (-1.f) * ptsVector;
			}
			Vector3 ptOnB = witnessPointB + offsetB;
			Scalar distance = nl;
			resultOut.AddContactPoint(ptsVector, ptOnB, -distance);
		}
	}

	if ((float)Dot(DeltaC2, sep) < 0.0f)
		sep = -sep;

	return true;
}
void PolyhedralContactClipping::ClipFace(const VertexArray& vertexIn, VertexArray& vertexOut, const Vector3& planeNormalWS, float planeEquationWS)
{
	int ve;
	float ds, de;
	int numVerts = vertexIn.size();
	if (numVerts < 2) { return; }

	Vector3 firstVertex = vertexIn[vertexIn.size() - 1];
	Vector3 endVertex   = vertexIn[0];

	ds = Dot(planeNormalWS, firstVertex) + planeEquationWS;

	for (ve = 0; ve < numVerts; ve++)
	{
		endVertex = vertexIn[ve];

		de = Dot(planeNormalWS, endVertex) + planeEquationWS;

		if (ds < 0)
		{
			if (de < 0)
			{
				// Start < 0, end < 0, so output endVertex
				vertexOut.push_back(endVertex);
			}
			else
			{
				// Start < 0, end >= 0, so output intersection
				vertexOut.push_back(Lerp(firstVertex, endVertex,ds * 1.f / (ds - de)));
			}
		}
		else
		{
			if (de < 0)
			{
				// Start >= 0, end < 0 so output intersection and end
				vertexOut.push_back(Lerp(firstVertex, endVertex, ds * 1.f / (ds - de)));
				vertexOut.push_back(endVertex);
			}
		}
		firstVertex = endVertex;
		ds = de;
	}
}