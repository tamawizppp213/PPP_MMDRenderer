//////////////////////////////////////////////////////////////////////////////////
//            @file　　  VoronoiSimplexSolver.hpp
//            @brief    VoronoiSimplexSolver
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_11
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

	Elsevier CDROM license agreements grants nonexclusive license to use the software
	for any purpose, commercial or non-commercial as long as the following credit is included
	identifying the original source of the software:

	Parts of the source are "from the book Real-Time Collision Detection by
	Christer Ericson, published by Morgan Kaufmann Publishers,
	(c) 2005 Elsevier Inc."

*/
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/NarrowPhaseCollision/VoronoiSimplexSolver.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define VERTEX_A 0
#define VERTEX_B 1
#define VERTEX_C 2
#define VERTEX_D 3
#define CATCH_DEGENERATE_TETRAHEDRON 1

using namespace gm;
//////////////////////////////////////////////////////////////////////////////////
//                               Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*						RemoveVertex
*************************************************************************//**
*  @fn        void VolonoiSimplexSolver::RemoveVertex(int index)
*  @brief     Remove vertex is targeted by index 
*  @param[in] int index
*  @return 　　void
*****************************************************************************/
void VolonoiSimplexSolver::RemoveVertex(int index)
{
	assert(NumVertices > 0);
	NumVertices--;
	SimplexVectorW[index] = SimplexVectorW[NumVertices];
	SimplexPointsP[index] = SimplexPointsP[NumVertices];
	SimplexPointsQ[index] = SimplexVectorW[NumVertices];
}
/****************************************************************************
*						ReduceVertex
*************************************************************************//**
*  @fn        void VolonoiSimplexSolver::ReduceVertices(const UsageBitField& usedVerts)
*  @brief     Reduce vertex is targeted by index
*  @param[in] UsageBitField
*  @return 　　void
*****************************************************************************/
void VolonoiSimplexSolver::ReduceVertices(const UsageBitField& usedVerts)
{
	if ((VerticesCount() >= 4) && (!usedVerts.UsedVertexD)) { RemoveVertex(3); }
	if ((VerticesCount() >= 3) && (!usedVerts.UsedVertexC)) { RemoveVertex(2); }
	if ((VerticesCount() >= 2) && (!usedVerts.UsedVertexB)) { RemoveVertex(1); }
	if ((VerticesCount() >= 1) && (!usedVerts.UsedVertexA)) { RemoveVertex(0); }
}
/****************************************************************************
*						UpdateClosestVectorAndPoints
*************************************************************************//**
*  @fn        bool VolonoiSimplexSolver::UpdateClosestVectorAndPoints()
*  @brief     Update Vector and point by the pre-defined number of vertices.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VolonoiSimplexSolver::UpdateClosestVectorAndPoints()
{
	if (NeedsUpdate)
	{
		CachedBC.Reset();
		NeedsUpdate = false;

		switch (VerticesCount())
		{
			/*-------------------------------------------------------------------
			-           Vertex Count:  0
			---------------------------------------------------------------------*/
			case 0:
			{
				CachedValidClosest = false;
				break;
			}
			/*-------------------------------------------------------------------
			-           Vertex Count:  1
			---------------------------------------------------------------------*/
			case 1:
			{
				CachedP1 = SimplexPointsP[0];
				CachedP2 = SimplexPointsQ[0];
				CachedV  = CachedP1 - CachedP2;
				CachedBC.Reset();
				CachedBC.SetBarycentricCoordinates(1.0f, 0.0f, 0.0f, 0.0f);
				CachedValidClosest = CachedBC.IsValid();
				break;
			}
			/*-------------------------------------------------------------------
			-           Vertex Count:  2
			---------------------------------------------------------------------*/
			case 2:
			{
				// closest point origin from line segment
				const Vector3& from = SimplexVectorW[0];
				const Vector3& to   = SimplexVectorW[1];
				Vector3 nearest;

				Vector3 point(0.0f, 0.0f, 0.0f);
				Vector3 diff = point - from;
				Vector3 v    = to - from;
				float   t    = Dot(v, diff);

				if (t > 0)
				{
					float dotVV = Dot(v, v);
					if (t < dotVV)
					{
						t     = t / dotVV;
						diff -= t * v;
						CachedBC.UsedVertices.UsedVertexA = true;
						CachedBC.UsedVertices.UsedVertexB = true;
					}
					else
					{
						t    = 1;
						diff = diff - v;
						CachedBC.UsedVertices.UsedVertexB = true;

					}
				}
				else
				{
					t = 0;
					CachedBC.UsedVertices.UsedVertexA = true;
				}

				CachedBC.SetBarycentricCoordinates(1 - t, t);
				nearest = from + t * v;

				CachedP1 = SimplexPointsP[0] + t * (SimplexPointsP[1] - SimplexPointsP[0]);
				CachedP2 = SimplexPointsQ[0] + t * (SimplexPointsQ[1] - SimplexPointsQ[0]);
				CachedV  = CachedP1 - CachedP2;

				ReduceVertices(CachedBC.UsedVertices);

				CachedValidClosest = CachedBC.IsValid();
				break;
			}
			/*-------------------------------------------------------------------
			-           Vertex Count:  3
			---------------------------------------------------------------------*/
			case 3:
			{
				Vector3 point(0.0f, 0.0f, 0.0f);
				const Vector3& a = SimplexVectorW[0];
				const Vector3& b = SimplexVectorW[1];
				const Vector3& c = SimplexVectorW[2];

				ClosestPtPointTriangle(point, a, b, c, CachedBC);
				CachedP1 = SimplexPointsP[0] * CachedBC.BarycentricCoords[0] + SimplexPointsP[1] * CachedBC.BarycentricCoords[1] + SimplexPointsP[2] * CachedBC.BarycentricCoords[2];
				CachedP2 = SimplexPointsQ[0] * CachedBC.BarycentricCoords[0] + SimplexPointsQ[1] * CachedBC.BarycentricCoords[1] + SimplexPointsQ[2] * CachedBC.BarycentricCoords[2];
				CachedV  = CachedP1 - CachedP2;

				ReduceVertices(CachedBC.UsedVertices);
				CachedValidClosest = CachedBC.IsValid();
				break;
			}
			/*-------------------------------------------------------------------
			-           Vertex Count:  4
			---------------------------------------------------------------------*/
			case 4:
			{
				Vector3 point(0.0f, 0.0f, 0.0f);

				const Vector3& a = SimplexVectorW[0];
				const Vector3& b = SimplexVectorW[1];
				const Vector3& c = SimplexVectorW[2];
				const Vector3& d = SimplexVectorW[3];

				bool hasSeparation = ClosestPtPointTetrahedron(point, a, b, c, d, CachedBC);

				if (hasSeparation)
				{
					CachedP1 = SimplexPointsP[0] * CachedBC.BarycentricCoords[0] + SimplexPointsP[1] * CachedBC.BarycentricCoords[1] + SimplexPointsP[2] * CachedBC.BarycentricCoords[2] + SimplexPointsP[3] * CachedBC.BarycentricCoords[3];
					CachedP2 = SimplexPointsQ[0] * CachedBC.BarycentricCoords[0] + SimplexPointsQ[1] * CachedBC.BarycentricCoords[1] + SimplexPointsQ[2] * CachedBC.BarycentricCoords[2] + SimplexPointsQ[3] * CachedBC.BarycentricCoords[3];
					CachedV = CachedP1 - CachedP2;
					ReduceVertices(CachedBC.UsedVertices);
				}
				else
				{
					//					printf("sub distance got penetration\n");

					if (CachedBC.Degenerate)
					{
						CachedValidClosest = false;
					}
					else
					{
						CachedValidClosest = true;
						//degenerate case == false, penetration = true + zero
						CachedV = Vector3(0.0f, 0.0f, 0.0f);
					}
					break;
				}

				CachedValidClosest = CachedBC.IsValid();
				break;
			}
			default:
			{
				CachedValidClosest = false;
			}
		}
	}

	return CachedValidClosest;
}
/****************************************************************************
*						ClosestPtPointTetrahedron
*************************************************************************//**
*  @fn        bool VolonoiSimplexSolver::ClosestPtPointTetrahedron(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, SubSimplexClosestResult& finalResult)
*  @brief     Closest point triangle
*  @param[in] Vector3 p
*  @param[in] Vector3 a
*  @param[in] Vector3 b
*  @param[in] Vector3 c
*  @param[in] Vector3 d
*  @param[out] SubSimplexClosestResult& result 
*  @return 　　int
*****************************************************************************/
bool VolonoiSimplexSolver::ClosestPtPointTetrahedron(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, SubSimplexClosestResult& finalResult)
{
	SubSimplexClosestResult tempResult;

	// Start out assuming point inside all halfspaces, so closest to itself
	finalResult.ClosestPointOnSimplex = p;
	finalResult.UsedVertices.Reset();
	finalResult.UsedVertices.UsedVertexA = true;
	finalResult.UsedVertices.UsedVertexB = true;
	finalResult.UsedVertices.UsedVertexC = true;
	finalResult.UsedVertices.UsedVertexD = true;

	int pointOutsideABC = PointOutsideOfPlane(p, a, b, c, d);
	int pointOutsideACD = PointOutsideOfPlane(p, a, c, d, b);
	int pointOutsideADB = PointOutsideOfPlane(p, a, d, b, c);
	int pointOutsideBDC = PointOutsideOfPlane(p, b, d, c, a);

	if (pointOutsideABC < 0 || pointOutsideACD < 0 || pointOutsideADB < 0 || pointOutsideBDC < 0)
	{
		finalResult.Degenerate = true;
		return false;
	}

	if (!pointOutsideABC && !pointOutsideACD && !pointOutsideADB && !pointOutsideBDC)
	{
		return false;
	}

	float bestSqDist = FLT_MAX;
	// If point outside face abc then compute closest point on abc
	if (pointOutsideABC)
	{
		ClosestPtPointTriangle(p, a, b, c, tempResult);
		Vector3 q = tempResult.ClosestPointOnSimplex;

		float sqDist = Dot(q - p, q - p);
		// Update best closest point if (squared) distance is less than current best
		if (sqDist < bestSqDist)
		{
			bestSqDist = sqDist;
			finalResult.ClosestPointOnSimplex = q;
			//convert result bitmask!
			finalResult.UsedVertices.Reset();
			finalResult.UsedVertices.UsedVertexA = tempResult.UsedVertices.UsedVertexA;
			finalResult.UsedVertices.UsedVertexB = tempResult.UsedVertices.UsedVertexB;
			finalResult.UsedVertices.UsedVertexC = tempResult.UsedVertices.UsedVertexC;
			finalResult.SetBarycentricCoordinates(
				tempResult.BarycentricCoords[VERTEX_A],
				tempResult.BarycentricCoords[VERTEX_B],
				tempResult.BarycentricCoords[VERTEX_C],
				0);
		}
	}

	// Repeat test for face acd
	if (pointOutsideACD)
	{
		ClosestPtPointTriangle(p, a, c, d, tempResult);
		Vector3 q = tempResult.ClosestPointOnSimplex;
		//convert result bitmask!

		float sqDist = Dot(q - p,q - p);
		if (sqDist < bestSqDist)
		{
			bestSqDist = sqDist;
			finalResult.ClosestPointOnSimplex = q;
			finalResult.UsedVertices.Reset();
			finalResult.UsedVertices.UsedVertexA = tempResult.UsedVertices.UsedVertexA;

			finalResult.UsedVertices.UsedVertexC = tempResult.UsedVertices.UsedVertexB;
			finalResult.UsedVertices.UsedVertexD = tempResult.UsedVertices.UsedVertexC;
			finalResult.SetBarycentricCoordinates(
				tempResult.BarycentricCoords[VERTEX_A],
				0,
				tempResult.BarycentricCoords[VERTEX_B],
				tempResult.BarycentricCoords[VERTEX_C]);
		}
	}
	// Repeat test for face adb

	if (pointOutsideADB)
	{
		ClosestPtPointTriangle(p, a, d, b, tempResult);
		Vector3 q = tempResult.ClosestPointOnSimplex;
		//convert result bitmask!

		float sqDist = Dot(q - p, q - p);
		if (sqDist < bestSqDist)
		{
			bestSqDist = sqDist;
			finalResult.ClosestPointOnSimplex = q;
			finalResult.UsedVertices.Reset();
			finalResult.UsedVertices.UsedVertexA = tempResult.UsedVertices.UsedVertexA;
			finalResult.UsedVertices.UsedVertexB = tempResult.UsedVertices.UsedVertexC;

			finalResult.UsedVertices.UsedVertexD = tempResult.UsedVertices.UsedVertexB;
			finalResult.SetBarycentricCoordinates(
				tempResult.BarycentricCoords[VERTEX_A],
				tempResult.BarycentricCoords[VERTEX_C],
				0,
				tempResult.BarycentricCoords[VERTEX_B]);
		}
	}
	// Repeat test for face bdc

	if (pointOutsideBDC)
	{
		ClosestPtPointTriangle(p, b, d, c, tempResult);
		Vector3 q = tempResult.ClosestPointOnSimplex;
		//convert result bitmask!
		float sqDist = Dot(q - p, q - p);
		if (sqDist < bestSqDist)
		{
			bestSqDist = sqDist;
			finalResult.ClosestPointOnSimplex = q;
			finalResult.UsedVertices.Reset();
			//
			finalResult.UsedVertices.UsedVertexB = tempResult.UsedVertices.UsedVertexA;
			finalResult.UsedVertices.UsedVertexC = tempResult.UsedVertices.UsedVertexC;
			finalResult.UsedVertices.UsedVertexD = tempResult.UsedVertices.UsedVertexB;

			finalResult.SetBarycentricCoordinates(
				0,
				tempResult.BarycentricCoords[VERTEX_A],
				tempResult.BarycentricCoords[VERTEX_C],
				tempResult.BarycentricCoords[VERTEX_B]);
		}
	}

	//help! we ended up full !

	if (finalResult.UsedVertices.UsedVertexA &&
		finalResult.UsedVertices.UsedVertexB &&
		finalResult.UsedVertices.UsedVertexC &&
		finalResult.UsedVertices.UsedVertexD)
	{
		return true;
	}

	return true;
}
/****************************************************************************
*						PointOutsideOfPlane
*************************************************************************//**
*  @fn        int  VolonoiSimplexSolver::PointOutsideOfPlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d)
*  @brief     Closest point triangle
*  @param[in] Vector3 p
*  @param[in] Vector3 a
*  @param[in] Vector3 b
*  @param[in] Vector3 c
*  @param[in] Vector3 d
*  @return 　　int
*****************************************************************************/
int  VolonoiSimplexSolver::PointOutsideOfPlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d)
{
	Vector3 normal = Cross(b - a, c - a);
	float   signp = Dot(p - a, normal);  // [AP AB AC]
	float   signd = Dot(d - a, normal);  // [AD AB AC]

#ifdef CATCH_DEGENERATE_TETRAHEDRON
#ifdef BT_USE_DOUBLE_PRECISION
	if (signd * signd < (btScalar(1e-8) * btScalar(1e-8))) { return -1; }
#else
	if (signd * signd < (1e-4 * 1e-4)) { return -1; }
#endif

#endif
	// Points on opposite sides if expression signs are opposite
	return signp * signd < 0.0f;
}
/****************************************************************************
*						ClosestPtPointTriangle
*************************************************************************//**
*  @fn        bool VolonoiSimplexSolver::ClosestPtPointTriangle(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, SubSimplexClosestResult& result)
*  @brief     Closest point triangle
*  @param[in] Vector3 p
*  @param[in] Vector3 a
*  @param[in] Vector3 b
*  @param[in] Vector3 c
*  @param[out] SubSimplexClosestResult& result 
*  @return 　　bool
*****************************************************************************/
bool VolonoiSimplexSolver::ClosestPtPointTriangle(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, SubSimplexClosestResult& result)
{
	result.UsedVertices.Reset();

	// check if P int vertex region outside A
	Vector3 ab = b - a;
	Vector3 ac = c - a;
	Vector3 ap = p - a;
	float d1 = Dot(ab, ap);
	float d2 = Dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		result.ClosestPointOnSimplex = a;
		result.UsedVertices.UsedVertexA = true;
		result.SetBarycentricCoordinates(1, 0, 0);
		return true;
	}

	Vector3 bp = p - b;
	float d3 = Dot(ab, bp);
	float d4 = Dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3)
	{
		result.ClosestPointOnSimplex = b;
		result.UsedVertices.UsedVertexB = true;
		result.SetBarycentricCoordinates(0, 1, 0);
		return true;
	}
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		result.ClosestPointOnSimplex = a + v * ab;
		result.UsedVertices.UsedVertexA = true;
		result.UsedVertices.UsedVertexB = true;
		result.SetBarycentricCoordinates(1 - v, v, 0);
		return true;
	}

	Vector3 cp = p - c;
	float   d5 = Dot(ab, cp);
	float   d6 = Dot(ac,cp);
	if (d6 >= 0.0f && d5 <= d6)
	{
		result.ClosestPointOnSimplex = c;
		result.UsedVertices.UsedVertexC = true;
		result.SetBarycentricCoordinates(0, 0, 1);
		return true;  //c; // barycentric coordinates (0,0,1)
	}

	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		result.ClosestPointOnSimplex = a + w * ac;
		result.UsedVertices.UsedVertexA = true;
		result.UsedVertices.UsedVertexC = true;
		result.SetBarycentricCoordinates(1 - w, 0, w);
		return true;
		//return a + w * ac; // barycentric coordinates (1-w,0,w)
	}

	// Check if P in edge region of BC, if so return projection of P onto BC
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));

		result.ClosestPointOnSimplex = b + w * (c - b);
		result.UsedVertices.UsedVertexB = true;
		result.UsedVertices.UsedVertexC = true;
		result.SetBarycentricCoordinates(0, 1 - w, w);
		return true;
		// return b + w * (c - b); // barycentric coordinates (0,1-w,w)
	}

	// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;

	result.ClosestPointOnSimplex = a + ab * v + ac * w;
	result.UsedVertices.UsedVertexA = true;
	result.UsedVertices.UsedVertexB = true;
	result.UsedVertices.UsedVertexC = true;
	result.SetBarycentricCoordinates(1 - v - w, v, w);

	return true;
}
/****************************************************************************
*						Reset
*************************************************************************//**
*  @fn        void VolonoiSimplexSolver::Reset()
*  @brief     Clear the simplex, remove all the vertices
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void VolonoiSimplexSolver::Reset()
{
	CachedValidClosest = false;
	NumVertices        = 0;
	NeedsUpdate        = true;
	LastW              = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	CachedBC.Reset();
}
/****************************************************************************
*						AddVertex
*************************************************************************//**
*  @fn        void VolonoiSimplexSolver::AddVertex(const Vector3& w, const Vector3& p, const Vector3& q)
*  @brief     Add the simplex and vertex
*  @param[in] Vector3 w
*  @param[in] Vector3 p
*  @param[in: Vector3 q
*  @return 　　void
*****************************************************************************/
void VolonoiSimplexSolver::AddVertex(const Vector3& w, const Vector3& p, const Vector3& q)
{
	LastW                       = w;
	NeedsUpdate                 = true;
	SimplexVectorW[NumVertices] = w;
	SimplexPointsP[NumVertices] = p;
	SimplexPointsQ[NumVertices] = q;
	NumVertices++;
}
/****************************************************************************
*						Closest
*************************************************************************//**
*  @fn        bool VolonoiSimplexSolver::Closest()
*  @brief     Get calculate the closest vertex
*  @param[out] Vector3 v
*  @return 　　bool
*****************************************************************************/
bool VolonoiSimplexSolver::Closest(Vector3& v)
{
	bool success = UpdateClosestVectorAndPoints();
	v            = CachedV;
	return success;
}
/****************************************************************************
*						MaxVertex
*************************************************************************//**
*  @fn        float VolonoiSimplexSolver::MaxVertex()
*  @brief     Get max norm squared vertex
*  @param[in] void
*  @return 　　float
*****************************************************************************/
float VolonoiSimplexSolver::MaxVertex()
{
	int i, numVertex = VerticesCount();
	float maxV = 0.0f;
	for (i = 0; i < numVertex; ++i)
	{
		float currentLength2 = NormSquared(SimplexVectorW[i]);
		if (maxV < currentLength2)
		{
			maxV = currentLength2;
		}
	}
	return maxV;
}
/****************************************************************************
*						GetSimplex
*************************************************************************//**
*  @fn        int   VolonoiSimplexSolver::GetSimplex(Vector3* pBuf, Vector3* qBuf, Vector3* yBuf) const
*  @brief     Get current simplex and return vertex count;
*  @param[out] Vector3* pBuf
*  @param[out] Vector3* qBuf
*  @param[out] Vector3* yBuf
*  @return 　　int
*****************************************************************************/
int VolonoiSimplexSolver::GetSimplex(Vector3* pBuf, Vector3* qBuf, Vector3* yBuf) const
{
	int i;
	for (i = 0; i < VerticesCount(); ++i)
	{
		yBuf[i] = SimplexVectorW[i];
		pBuf[i] = SimplexPointsP[i];
		qBuf[i] = SimplexPointsQ[i];
	}
	return VerticesCount();
}
/****************************************************************************
*						InSimplex
*************************************************************************//**
*  @fn        bool VolonoiSimplexSolver::InSimplex(const Vector3& w)
*  @brief     In simplex
*  @param[in] w
*  @return 　　bool
*****************************************************************************/
bool  VolonoiSimplexSolver::InSimplex(const Vector3& w)
{
	bool found = false;
	int i, numVertices = VerticesCount();

	for (i = 0; i < numVertices; ++i)
	{
#ifdef  USE_EQUAL_VERTEX_THRESHOLD
		if (NormSquared(SimplexVectorW[i] - w) <= EqualVertexThreshold)
#else
		if(SimplexVectorW[i] == w)
#endif
		{
			found = true;
			break;
		}
	}
	if (w == LastW) { return true; }
	return found;
}
/****************************************************************************
*						BackupClosest
*************************************************************************//**
*  @fn        void VolonoiSimplexSolver::BackupClosest(Vector3& v)
*  @brief     Get Cached V
*  @param[out] Vector3 v 
*  @return 　　void
*****************************************************************************/
void  VolonoiSimplexSolver::BackupClosest(Vector3& v)
{
	v = CachedV;
}
/****************************************************************************
*						EmptySimplex
*************************************************************************//**
*  @fn        bool VolonoiSimplexSolver::EmptySimplex() const
*  @brief     Is Vertex count == 0
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VolonoiSimplexSolver::EmptySimplex() const
{
	return VerticesCount() == 0;
}
/****************************************************************************
*						ComputePoints
*************************************************************************//**
*  @fn        void  VolonoiSimplexSolver::ComputePoints(Vector3& p1, Vector3& p2)
*  @brief     Compute point
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void  VolonoiSimplexSolver::ComputePoints(Vector3& p1, Vector3& p2)
{
	UpdateClosestVectorAndPoints();
	p1 = CachedP1;
	p2 = CachedP2;
}
