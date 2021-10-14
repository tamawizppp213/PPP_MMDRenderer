//////////////////////////////////////////////////////////////////////////////////
///             @file   SphereShape.hpp
///             @brief  Sphere shape
///             @author Partially edit by Toide
///             @date   2021_09_19
///                     2021_10_06 (first checked) first checked -> no debug.
//////////////////////////////////////////////////////////////////////////////////

/*Bullet Continuous Collision Detectionand Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.*/

#pragma once
#ifndef OBB_TRIANGLE_MINKOWSKI_HPP
#define OBB_TRIANGLE_MINKOWSKI_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConvexShape.hpp"
#include "BoxShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         SphereShape
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			SphereShape
*************************************************************************//**
*  @class     SphereShape
*  @brief     Sphere shape
*****************************************************************************/
__declspec(align(16))
class TriangleShape : public PolyhedralConvexShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& dir) const override
	{
		gm::Vector3 dots = gm::Vector3(Dot(dir, Vertices1[0]), Dot(dir, Vertices1[1]), Dot(dir, Vertices1[2]));
		return Vertices1 [(int)gm::MaxAxis3(dots)];
	}
	void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const
	{
		for (int i = 0; i < numVectors; ++i)
		{
			const gm::Vector3& dir = vectors[i];
			gm::Vector3 dots = gm::Vector3(Dot(dir, Vertices1[0]), Dot(dir, Vertices1[1]), Dot(dir, Vertices1[2]));
			supportVerticesOut[i] = Vertices1[(int)gm::MaxAxis3(dots)];

		}
	}
	void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override
	{
		(void)mass; assert(NULL);
		inertia = gm::Vector3(0, 0, 0);
	}
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Vector3 Vertices1[3];

	int GetVerticesCount() const override { return 3; }
	int GetEdgesCount   () const override { return 3; }
	void GetEdge(int i, gm::Vector3& pa, gm::Vector3& pb)const override { GetVertex(i, pa); GetVertex((i + 1) % 3, pb); }
	void GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override { GetAABBSlow(transform, aabbMin, aabbMax); }
	void GetPlane(gm::Vector3& planeNormal, gm::Vector3& planeSupport, int i) const{ GetPlaneEquation(i, planeNormal, planeSupport);}
	int GetPlanesCount() const { return 1; }
	virtual void GetPlaneEquation(int i, gm::Vector3& planeNormal, gm::Vector3& planeSupport) const
	{
		(void)i;
		CalculateNormal(planeNormal);
		planeSupport = Vertices1[0];
	}
	void CalculateNormal(gm::Vector3& normal) const { normal = gm::Cross((Vertices1[1] - Vertices1[0]), Vertices1[2] - Vertices1[0]); }
	gm::Vector3&       GetVertexPtr(int index)       { return Vertices1[index]; }
	const gm::Vector3& GetVertexPtr(int index) const { return Vertices1[index]; }
	void GetVertex(int index, gm::Vector3& vert) const override { vert = Vertices1[index]; }
	const char* GetName() const override { return "Triangle"; }
	int GetPreferredPenetrationDirectionsCount() const override { return 2; }
	void GetPreferredPenetrationDirection(int index, gm::Vector3& penetrationVector) const override
	{
		CalculateNormal(penetrationVector);
		if (index) { penetrationVector = penetrationVector * (-1.0f); }
	}
	bool IsInside(const gm::Vector3& point, float tolerance) const override
	{
		gm::Vector3 normal;
		CalculateNormal(normal);
		float dist = Dot(point, normal);
		float planeConst = Dot(Vertices1[0], normal);
		dist -= planeConst;

		if (dist >= -tolerance && dist <= tolerance)
		{
			int i;
			for (i = 0; i < 3; ++i)
			{
				gm::Vector3 pa, pb;
				GetEdge(i, pa, pb);
				gm::Vector3 edge = pb - pa;
				gm::Vector3 edgeNormal = Cross(edge, normal);
				Normalize(edgeNormal);
				float dist = Dot(point, edgeNormal);
				float edgeConst = Dot(pa, edgeNormal);
				dist -= edgeConst;
				if (dist < -tolerance) { return false; }
			}

			return true;
		}

		return false;
	}

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	TriangleShape() : PolyhedralConvexShape() { _shapeType = (int)BroadPhaseNativeTypes::Triangle_Shape; }
	TriangleShape(const gm::Vector3& p0, const gm::Vector3& p1, const gm::Vector3& p2) : PolyhedralConvexShape()
	{
		_shapeType = (int)BroadPhaseNativeTypes::Triangle_Shape;
		Vertices1[0] = p0;
		Vertices1[1] = p1;
		Vertices1[2] = p2;
	}
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif