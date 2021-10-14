//////////////////////////////////////////////////////////////////////////////////
///             @file   ConvexPointCloudShape.hpp
///             @brief  ConvexPointCloudShape
///             @author Partially edit by Toide
///             @date   2021_09_19
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
#ifndef COMPOUND_SHAPE_HPP
#define COMPOUND_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "CollisionShape.hpp"
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct Dbvt;
//////////////////////////////////////////////////////////////////////////////////
//                         ConvexPointCloudSha@e
//////////////////////////////////////////////////////////////////////////////////
__declspec(align(16))
struct CompoundShapeChild
{
	DECLARE_ALIGNED_ALLOCATOR;
	gm::AffineTransform    Trans;
	ICollisionShape*       ChildShape;
	int                    ChildShapeType;
	float                  ChildMargin;
	struct DbvtNode*       Node;
};

INLINE bool operator==(const CompoundShapeChild& c1, const CompoundShapeChild& c2)
{
	return (c1.Trans      == c2.Trans &&
		c1.ChildShape     == c2.ChildShape &&
		c1.ChildShapeType == c2.ChildShapeType &&
		c1.ChildMargin == c2.ChildMargin);
}

/****************************************************************************
*				  			CompoundShape
*************************************************************************//**
*  @class     CompoundShape
*  @brief     CompoundShape
*****************************************************************************/
__declspec(align(16))
class CompoundShape :public ICollisionShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void         AddChildShape          (const gm::AffineTransform& localTransform, ICollisionShape* shape);
	virtual void RemoveChildShape       (ICollisionShape* shape);
	void         RemoveChildShapeByIndex(int childShapeIndex);
	void         UpdateChildTransform(int childIndex, const gm::AffineTransform& newChildTransform, bool shouldRecalculateLocalAABB = true);
	void         CreateAABBTreeFromChildren();
	void         CalculatePrincipalAxisTransform(const float* masses, gm::AffineTransform& principal, gm::Vector3& inertia) const;
	int          GetUpdateRevision() const { return _updateRevision; }

	virtual void ReCalculateLocalAABB();
	virtual void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	int GetChildShapesCount() const { return int(_children.size()); }
	ICollisionShape*       GetChildShape    (int index)       { return _children[index].ChildShape; }
	const ICollisionShape* GetChildShape    (int index) const { return _children[index].ChildShape; }
	gm::AffineTransform&         GetChildTransform(int index)       { return _children[index].Trans; }
	const gm::AffineTransform&   GetChildTransform(int index) const { return _children[index].Trans; }
	CompoundShapeChild* GetChildList() { return &_children[0]; }

	void GetAABB(const gm::AffineTransform& t, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override;
	virtual const gm::Vector3& GetLocalScaling() const override { return _localScaling; }
	virtual float GetMargin()                    const          { return _collisionMargin; }
	virtual const char* GetName()                const          { return "Compound"; }
	const Dbvt*         GetDynamicAABBTree()     const          { return _dynamicAABBTree; }
	Dbvt*               GetDynamicAABBTree()                    { return _dynamicAABBTree; }
	virtual void SetLocalScaling(const gm::Vector3& scaling) override;
	virtual void SetMargin(float margin) { _collisionMargin = margin; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	explicit CompoundShape(bool enableDynamicAABBTree = true, const int initialChildCapability = 0);
	virtual ~CompoundShape();

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<CompoundShapeChild> _children;
	gm::Vector3 _localAABBMin;
	gm::Vector3 _localAABBMax;
	Dbvt*       _dynamicAABBTree;
	int         _updateRevision;
	float       _collisionMargin;
	float       _padding;
	gm::Vector3 _localScaling;
};
#endif