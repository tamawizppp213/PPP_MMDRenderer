//////////////////////////////////////////////////////////////////////////////////
//            @file　　  VoronoiSimplexSolver.hpp
//            @brief    VoronoiSimplexSolver
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_03
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
#ifndef VORONOI_SIMPLEX_SOLVER_HPP
#define VORONOI_SIMPLEX_SOLVER_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMVector.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define VORONOI_SIMPLEX_MAX_VERTS 5
#define USE_EQUAL_VERTEX_THRESHOLD

//////////////////////////////////////////////////////////////////////////////////
//                               Class
//////////////////////////////////////////////////////////////////////////////////
struct UsageBitField
{
	UsageBitField()
	{
		Reset();
	}

	void Reset()
	{
		UsedVertexA = false;
		UsedVertexB = false;
		UsedVertexC = false;
		UsedVertexD = false;
	}
	unsigned short UsedVertexA : 1;
	unsigned short UsedVertexB : 1;
	unsigned short UsedVertexC : 1;
	unsigned short UsedVertexD : 1;
	unsigned short Unused1 : 1;
	unsigned short Unused2 : 1;
	unsigned short Unused3 : 1;
	unsigned short Unused4 : 1;
};

struct SubSimplexClosestResult
{
	gm::Vector3 ClosestPointOnSimplex;
	//MASK for m_usedVertices
	//stores the simplex vertex-usage, using the MASK,
	// if m_usedVertices & MASK then the related vertex is used
	UsageBitField UsedVertices;
	float BarycentricCoords[4];
	bool  Degenerate;

	void Reset()
	{
		Degenerate = false;
		SetBarycentricCoordinates();
		UsedVertices.Reset();
	}
	bool IsValid()
	{
		bool valid = (BarycentricCoords[0] >= 0.0f) &&
			(BarycentricCoords[1] >= 0.0f) &&
			(BarycentricCoords[2] >= 0.0f) &&
			(BarycentricCoords[3] >= 0.0f);

		return valid;
	}
	void SetBarycentricCoordinates(float a = 0.0f, float b = 0.0f, float c = 0.0f, float d = 0.0f)
	{
		BarycentricCoords[0] = a;
		BarycentricCoords[1] = b;
		BarycentricCoords[2] = c;
		BarycentricCoords[3] = d;
	}
};

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     VolonoiSimplexSolver
*  @brief     VolonoiSimplexSolver is an implementation of the closest point distance algorithm from a 1- 4 points simplex to the origin.
*             Can be used with gik as an alternatiave to tJohnson distance algorithm
*****************************************************************************/
__declspec(align(16))
class VolonoiSimplexSolver
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void RemoveVertex(int index);
	void ReduceVertices(const UsageBitField& usedVerts);
	bool UpdateClosestVectorAndPoints();
	bool ClosestPtPointTetrahedron(const gm::Vector3& p, const gm::Vector3& a, const gm::Vector3& b, const gm::Vector3& c, const gm::Vector3& d, SubSimplexClosestResult& finalResult);
	int  PointOutsideOfPlane(const gm::Vector3& p, const gm::Vector3& a, const gm::Vector3& b, const gm::Vector3& c, const gm::Vector3& d);
	bool ClosestPtPointTriangle(const gm::Vector3& p, const gm::Vector3& a, const gm::Vector3& b, const gm::Vector3& c, SubSimplexClosestResult& result);

	void Reset();
	void AddVertex(const gm::Vector3& w, const gm::Vector3& p, const gm::Vector3& q);
	bool Closest(gm::Vector3& v);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	int NumVertices;

	gm::Vector3 SimplexVectorW[VORONOI_SIMPLEX_MAX_VERTS];
	gm::Vector3 SimplexPointsP[VORONOI_SIMPLEX_MAX_VERTS];
	gm::Vector3 SimplexPointsQ[VORONOI_SIMPLEX_MAX_VERTS];

	gm::Vector3 CachedP1;
	gm::Vector3 CachedP2;
	gm::Vector3 CachedV;
	gm::Vector3 LastW;

	float EqualVertexThreshold;
	bool CachedValidClosest;

	SubSimplexClosestResult CachedBC;

	bool NeedsUpdate;

	float MaxVertex();
	bool  FullSimplex() const { return NumVertices == 4; }
	float GetEqualVertexThreshold() const { return EqualVertexThreshold; }
	int   GetSimplex(gm::Vector3* pBuf, gm::Vector3* qBuf, gm::Vector3* yBuf) const;
	bool  InSimplex(const gm::Vector3& w);
	void  BackupClosest(gm::Vector3& v);
	bool  EmptySimplex() const;
	void  ComputePoints(gm::Vector3& p1, gm::Vector3& p2);
	int   VerticesCount() const { return NumVertices; }
	void SetEqualVertexThreshold(float threshold) { EqualVertexThreshold = threshold; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	VolonoiSimplexSolver() : EqualVertexThreshold(VORONOI_SIMPLEX_MAX_VERTS) {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

#endif