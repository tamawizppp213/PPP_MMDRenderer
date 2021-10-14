//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexPolyhedronShape.hpp
///             @brief  ConvexPolyhedronShape
///             @author ---------------
///             @date   2021_09_18
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

#pragma once
#ifndef CONVEX_POLYHEDRAL_SHAPE_HPP
#define CONVEX_POLYHEDRAL_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConvexShape.hpp"
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

#define TEST_INTERNAL_OBJECTS 1
//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////

struct ConvexFace
{
	gm::AlignedObjectArray<int> Indices;
	float Plane[4];
};
/****************************************************************************
*				  			ConvexPolyhedron
*************************************************************************//**
*  @class     ConvexPolyhedron
*  @brief     temp
*****************************************************************************/
__declspec(align(16))
class ConvexPolyhedron
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize();
	void Initialize2();
	bool TestContainment() const;
	void Project(const gm::AffineTransform& transform, const gm::Vector3& direction, float& minProj, float& maxProj, gm::Vector3& witnesPtMin, gm::Vector3& witnesPtMax) const;


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<gm::Vector3> Vertices;
	gm::AlignedObjectArray<ConvexFace> Faces;
	gm::AlignedObjectArray<gm::Vector3> UniqueEdges;

	gm::Vector3 LocalCenter;
	gm::Vector3 Extents;
	float       Radius;
	float       Padding[3];
	gm::Vector3 C;
	gm::Vector3 E;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ConvexPolyhedron() = default;
	virtual ~ConvexPolyhedron() {};
	DECLARE_ALIGNED_ALLOCATOR;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

__declspec(align(16))
class PolyhedralConvexShape : public ConvexInternalShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const override;
	void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const override;
	void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;
	virtual bool InitializePolyhedralFeatures(int shiftVerticeszByMargin = 0) {	return true;}

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const ConvexPolyhedron* GetConvexPolyhedron() const { return _polyhedron; }
	
	virtual int GetVerticesCount()                                          const = 0;
	virtual int GetEdgesCount()                                             const = 0;
	virtual void GetEdge      (int index, gm::Vector3& pa, gm::Vector3& pb) const = 0;
	virtual void GetVertex    (int index, gm::Vector3& vertex)              const = 0;
	virtual int GetPlanesCount()                                            const = 0;
	virtual void GetPlane(gm::Vector3& planeNormal, gm::Vector3& planeSupport, int index) const = 0;
	virtual bool IsInside(const gm::Vector3& pt, float tolerance) const = 0;

	virtual void SetPolyhedralFeatures(ConvexPolyhedron& polyhedron) {};

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	PolyhedralConvexShape() : ConvexInternalShape() { _polyhedron = 0; }
	virtual ~PolyhedralConvexShape() {};
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	ConvexPolyhedron* _polyhedron;

};

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class PolyhedralConvexAABBCachingShape : public PolyhedralConvexShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	inline void GetNonVirtualAABB(const gm::AffineTransform& trans, gm::Vector3& aabbMin, gm::Vector3& aabbMax, float margin) const
	{
		assert(_isLocalAABBValid);
		TransformAABB(_localAABBMin, _localAABBMax, margin, trans, aabbMin, aabbMax);
	}

	void SetLocalScaling(const gm::Vector3& scaling) override
	{
		ConvexInternalShape::SetLocalScaling(scaling);
		ReCalculateLocalAABB();
	}
	void GetAABB(const gm::AffineTransform& t, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override
	{
		GetNonVirtualAABB(t, aabbMin, aabbMax, GetMargin());
	}
	void ReCalculateLocalAABB();
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
	void SetCachedLocalAABB(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax)
	{
		_isLocalAABBValid = true;
		_localAABBMin = aabbMin;
		_localAABBMax = aabbMax;
	}

	inline void GetCachedLocalAABB(gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
	{
		assert(_isLocalAABBValid);
		aabbMin = _localAABBMin;
		aabbMax = _localAABBMax;
	}

	PolyhedralConvexAABBCachingShape() : PolyhedralConvexShape()
	{
		_localAABBMin = gm::Vector3(1, 1, 1);
		_localAABBMax = gm::Vector3(-1, -1, -1);
		_isLocalAABBValid = false;
	};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::Vector3 _localAABBMin;
	gm::Vector3 _localAABBMax;
	bool _isLocalAABBValid;
};
#endif