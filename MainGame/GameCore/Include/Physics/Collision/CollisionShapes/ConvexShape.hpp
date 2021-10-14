//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexShape.hpp
///             @brief  ConvexShape
///             @author Partially Edit by Toide
///             @date   2021_09_18
///                     2021_10_05 (first checked) first checked -> no debug.
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

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
#ifndef CONVEX_SHAPE_HPP
#define CONVEX_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "CollisionShape.hpp"
#include "../../Util/TransformUtil.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define CONVEX_DISTANCE_MARGIN (0.04f)
#define MAX_PREFERRED_PENETRATION_DIRECTIONS 10
//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////


/****************************************************************************
*				  			ConcaveShape
*************************************************************************//**
*  @class     ConcaveShape
*  @brief     Concave shape
*****************************************************************************/
__declspec(align(16))
class ConvexShape : public ICollisionShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual gm::Vector3 LocalGetSupportingVertex             (const gm::Vector3& vector) const = 0;
	virtual gm::Vector3 LocalGetSupportingVertexWithoutMargin(const gm::Vector3& vector) const = 0;
	
	virtual void Project(const gm::AffineTransform& transform, const gm::Vector3& direction, float& minProj, float& maxProj, gm::Vector3& witnesPtMin, gm::Vector3& witnesPtMax) const;
	virtual void BatchedUnitVectorGetSupportingVertexWithoutMargin(const gm::Vector3* vectors, gm::Vector3* supportVerticesOut, int numVectors) const = 0;


	gm::Vector3  LocalGetSupportVertexWithoutMarginNonVirtual(const gm::Vector3& localDir) const;
	gm::Vector3  LocalGetSupportVertexNonVirtual(const gm::Vector3& vector) const;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	float GetMarginNonVirtual() const;
	void  GetAABBNonVirtual (const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const;
	virtual void GetAABB    (const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const = 0;
	virtual void GetAABBSlow(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const = 0;
	virtual void GetPreferredPenetrationDirection(int index, gm::Vector3& penetrationVector) const = 0;
	virtual float GetMargin() const = 0;
	virtual int GetPreferredPenetrationDirectionsCount() const = 0;
	virtual const gm::Vector3& GetLocalScaling() const = 0;


	virtual void SetLocalScaling(const gm::Vector3& scaling) = 0;
	virtual void SetMargin(float margin) = 0;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	ConvexShape() {};
	virtual ~ConvexShape() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

/****************************************************************************
*				  			ConvexInternalShape
*************************************************************************//**
*  @class     ConvexInternalShape
*  @brief     temp
*****************************************************************************/
__declspec(align(16))
class ConvexInternalShape : public ConvexShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual gm::Vector3 LocalGetSupportingVertex(const gm::Vector3& vector) const override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const gm::Vector3& GetImplicitShapeDimensions() const { return _implicitShapeDimensions; }
	void          GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const { GetAABBSlow(transform, aabbMin, aabbMax); }
	void          GetAABBSlow(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override;
	float         GetMargin                              () const override { return _collisionMargin; };
	int           GetPreferredPenetrationDirectionsCount () const override { return 0; };
	const gm::Vector3& GetLocalScaling                   () const override { return _localScaling; };
	const         gm::Vector3& GetLocalScalingNV         () const          { return _localScaling; }
	float         GetMarginNV                            () const          { return _collisionMargin; }
	void          GetPreferredPenetrationDirection(int index, gm::Vector3& penetrationVector) const override
	{
		(void)penetrationVector;
		(void)index;
		assert(0);
	}

	virtual void SetLocalScaling(const gm::Vector3& scaling) override;
	virtual void SetMargin      (float margin)               override { _collisionMargin = margin; };
	void SetImplicitShapeDimensions(const gm::Vector3& dimensions)     { _implicitShapeDimensions = dimensions; }
	void SetSafeMargin(float minDimension, float defaultMarginMultiplifier = 0.1f)
	{
		float safeMargin = defaultMarginMultiplifier * minDimension;
		if (safeMargin < GetMargin()) { SetMargin(safeMargin); }
	}
	void SetSafeMargin(const gm::Vector3& halfExtents, float defaultMarginMultiplier = 0.1f)
	{
		float minDimensions = halfExtents.GetElement(MinAxis3(halfExtents));
		SetSafeMargin(minDimensions, defaultMarginMultiplier);
	}
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	virtual ~ConvexInternalShape() {};
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	ConvexInternalShape();

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::Vector3 _localScaling;
	gm::Vector3 _implicitShapeDimensions;
	float       _collisionMargin;
	float       _padding[3];
};

///btConvexInternalAabbCachingShape adds local aabb caching for convex shapes, to avoid expensive bounding box calculations
class ConvexInternalAabbCachingShape : public ConvexInternalShape
{
	gm::Vector3 _localAabbMin;
	gm::Vector3 _localAabbMax;
	bool _isLocalAabbValid;

protected:
	ConvexInternalAabbCachingShape();

	void SetCachedLocalAABB(const gm::Vector3& aabbMin, const gm::Vector3& aabbMax)
	{
		_isLocalAabbValid = true;
		_localAabbMin = aabbMin;
		_localAabbMax = aabbMax;
	}

	inline void GetCachedLocalAABB(gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
	{
		assert(_isLocalAabbValid);
		aabbMin = _localAabbMin;
		aabbMax = _localAabbMax;
	}

	inline void GetNonVirtualAABB(const gm::AffineTransform& trans, gm::Vector3& aabbMin, gm::Vector3& aabbMax, float margin) const
	{
		//lazy evaluation of local aabb
		assert(_isLocalAabbValid);
		TransformAABB(_localAabbMin, _localAabbMax, margin, trans, aabbMin, aabbMax);
	}

public:
	void SetLocalScaling(const gm::Vector3& scaling) override;

	void GetAABB(const gm::AffineTransform& t, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override;

	void ReCalculateLocalAABB();
};


#endif