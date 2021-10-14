//////////////////////////////////////////////////////////////////////////////////
///             @file   CompoundShape.hpp
///             @brief  CompoundShape
///             @author Partially edit by Toide
///             @date   2021_09_20
///                     2021_10_05 (first checked) first checked -> no debug.
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

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionShapes/CompoundShape.hpp"
#include "GameCore/Include/Physics/Collision/BroadPhaseCollision/DBVT.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
CompoundShape::CompoundShape(bool enableDynamicAABBTree, const int initialChildCapability)
{
	_localAABBMin    = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	_localAABBMax    = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	_dynamicAABBTree = NULL;
	_updateRevision  = 1;
	_collisionMargin = 0.0f;
	_localScaling    = Vector3(1.0f, 1.0f, 1.0f);
	_shapeType       = (int)BroadPhaseNativeTypes::Compound_Shape;

	if (enableDynamicAABBTree)
	{
		void* memory = AlignedAllocInternal(sizeof(Dbvt), 16);
		_dynamicAABBTree = new (memory)Dbvt();
		assert(memory == _dynamicAABBTree);
	}

	_children.reserve(initialChildCapability);
}

CompoundShape::~CompoundShape()
{
	if (_dynamicAABBTree)
	{
		_dynamicAABBTree->~Dbvt();
		AlignedFreeInternal(_dynamicAABBTree);
	}
}
#pragma region Public Function
/****************************************************************************
*						AddChildShape
*************************************************************************//**
*  @fn        void CompoundShape::AddChildShape(const gm::Transform& localTransform, ICollisionShape* shape)
*  @brief     Add child shape
*  @param[in] Transform& localTransform
*  @param[out]ICollisionShape* shape
*  @return 　　void
*****************************************************************************/
void CompoundShape::AddChildShape(const gm::AffineTransform& localTransform, ICollisionShape* shape)
{
	_updateRevision++;

	CompoundShapeChild child;
	child.Node           = 0;
	child.Trans          = localTransform;
	child.ChildShape     = shape;
	child.ChildShapeType = shape->GetShapeType();
	child.ChildMargin    = shape->GetMargin();

	/*-------------------------------------------------------------------
	-        Extend the local aabbMin/aabbMax
	---------------------------------------------------------------------*/
	Vector3 localAabbMin, localAabbMax;
	shape->GetAABB(localTransform, localAabbMin, localAabbMax);
	if ((float)_localAABBMin.GetX() > localAabbMin.GetX()){ _localAABBMin.SetX(localAabbMin.GetX()); }
	if ((float)_localAABBMax.GetX() < localAabbMax.GetX()){ _localAABBMax.SetX(localAabbMax.GetX()); }
	if ((float)_localAABBMin.GetY() > localAabbMin.GetY()) { _localAABBMin.SetY(localAabbMin.GetY()); }
	if ((float)_localAABBMax.GetY() < localAabbMax.GetY()) { _localAABBMax.SetY(localAabbMax.GetY()); }
	if ((float)_localAABBMin.GetZ() > localAabbMin.GetZ()) { _localAABBMin.SetZ(localAabbMin.GetZ()); }
	if ((float)_localAABBMax.GetZ() < localAabbMax.GetZ()) { _localAABBMax.SetZ(localAabbMax.GetZ()); }

	/*-------------------------------------------------------------------
	-        Dynamic AABB Tree check
	---------------------------------------------------------------------*/
	if (_dynamicAABBTree)
	{
		const DbvtVolume bounds = DbvtVolume::FromMM(localAabbMin, localAabbMax);
		size_t index            = _children.size();
		child.Node              = _dynamicAABBTree->Insert(bounds, reinterpret_cast<void*>(index));
	}

	_children.push_back(child);
}
/****************************************************************************
*						RemoveChildShape
*************************************************************************//**
*  @fn        void CompoundShape::RemoveChildShape(ICollisionShape* shape)
*  @brief     Find the children containing the shape specified, and remove those children.
*  @param[out]ICollisionShape* shape
*  @return 　　void
*****************************************************************************/
void CompoundShape::RemoveChildShape(ICollisionShape* shape)
{
	_updateRevision++;
	// Find the children containing the shape specified, and remove those children.
	//note: there might be multiple children using the same shape!
	for (int i = _children.size() - 1; i >= 0; i--)
	{
		if (_children[i].ChildShape == shape)
		{
			RemoveChildShapeByIndex(i);
		}
	}

	ReCalculateLocalAABB();
}
/****************************************************************************
*						RemoveChildShapeByIndex
*************************************************************************//**
*  @fn        void CompoundShape::RemoveChildShape(int childShapeIndex)
*  @brief     Remove child shape specified by index.
*  @param[out]ICollisionShape* shape
*  @return 　　void
*****************************************************************************/
void CompoundShape::RemoveChildShapeByIndex(int childShapeIndex)
{
	_updateRevision++;
	assert(childShapeIndex >= 0 && childShapeIndex < _children.size());

	if (_dynamicAABBTree)
	{
		_dynamicAABBTree->Remove(_children[childShapeIndex].Node);
	}
	_children.swap(childShapeIndex, _children.size() - 1);

	if (_dynamicAABBTree)
	{
		_children[childShapeIndex].Node->DataAsInt = childShapeIndex;
	}
	_children.pop_back();
}

/****************************************************************************
*						UpdateChildTransform
*************************************************************************//**
*  @fn        void CompoundShape::UpdateChildTransform(int childIndex, const gm::Transform& newChildTransform, bool shouldRecalculateLocalAABB)
*  @brief     Update child transform
*  @param[in] int child index
*  @param[in] Transform newChildTransform
*  @param[in] bool shouldRecalculateLocalAABB
*  @return 　　void
*****************************************************************************/
void CompoundShape::UpdateChildTransform(int childIndex, const gm::AffineTransform& newChildTransform, bool shouldRecalculateLocalAABB)
{
	_children[childIndex].Trans = newChildTransform;

	if (_dynamicAABBTree)
	{
		///update the dynamic aabb tree
		Vector3 localAabbMin, localAabbMax;
		_children[childIndex].ChildShape->GetAABB(newChildTransform, localAabbMin, localAabbMax);
		ALIGNED16(DbvtVolume)
			bounds = DbvtVolume::FromMM(localAabbMin, localAabbMax);
		//int index = m_children.size()-1;
		_dynamicAABBTree->Update(_children[childIndex].Node, bounds);
	}

	if (shouldRecalculateLocalAABB)
	{
		ReCalculateLocalAABB();
	}
}

/****************************************************************************
*						CreateAABBTreeFromChildren
*************************************************************************//**
*  @fn        void CompoundShape::CreateAABBTreeFromChildren()
*  @brief     Create AABB tree from childShape
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void CompoundShape::CreateAABBTreeFromChildren()
{
	if (!_dynamicAABBTree)
	{
		void* mem = AlignedAllocInternal(sizeof(Dbvt), 16);
		_dynamicAABBTree = new (mem) Dbvt();
		assert(mem == _dynamicAABBTree);

		for (int index = 0; index < _children.size(); index++)
		{
			CompoundShapeChild& child = _children[index];

			//extend the local aabbMin/aabbMax
			Vector3 localAabbMin, localAabbMax;
			child.ChildShape->GetAABB(child.Trans, localAabbMin, localAabbMax);

			const DbvtVolume bounds = DbvtVolume::FromMM(localAabbMin, localAabbMax);
			size_t index2 = index;
			child.Node = _dynamicAABBTree->Insert(bounds, reinterpret_cast<void*>(index2));
		}
	}
}

/****************************************************************************
*						CalculatePrincipalAxisTransform
*************************************************************************//**
*  @fn        void CompoundShape::CalculatePrincipalAxisTransform(const float* masses, gm::Transform& principal, gm::Vector3& inertia) const
*  @brief     Get Principal Axis Transform
*  @param[in] const float* masses
*  @param[out] Transform& principal
*  @param[out] Vector3 inertia
*  @return 　　void
*****************************************************************************/
void CompoundShape::CalculatePrincipalAxisTransform(const float* masses, gm::AffineTransform& principal, gm::Vector3& inertia) const
{
	int n = _children.size();

	float totalMass = 0;
	Vector3 center(0, 0, 0);
	int k;

	for (k = 0; k < n; k++)
	{
		assert(masses[k] > 0);
		center    += _children[k].Trans.GetTranslation() * masses[k];
		totalMass += masses[k];
	}

	assert(totalMass > 0);

	center = center / totalMass;
	principal.SetTranslation(center);

	Matrix3 tensor;
	tensor.SetX(Vector3(0, 0, 0));
	tensor.SetY(Vector3(0, 0, 0));
	tensor.SetZ(Vector3(0, 0, 0));
	for (k = 0; k < n; k++)
	{
		Vector3 i;
		_children[k].ChildShape->CalculateLocalInertia(masses[k], i);

		const AffineTransform& t = _children[k].Trans;
		Vector3 o = t.GetTranslation() - center;

		//compute inertia tensor in coordinate system of compound shape
		Matrix3 j = Transpose(t.GetBasis());
		j.SetX( j.GetX() * i.GetX());
		j.SetY(j.GetY() * i.GetY());
		j.SetZ(j.GetZ() * i.GetZ());
		j = t.GetBasis() * j;

		//add inertia tensor
		tensor.SetX(tensor.GetX() + j.GetX());
		tensor.SetY(tensor.GetY() + j.GetY());
		tensor.SetZ(tensor.GetZ() + j.GetZ());

		//compute inertia tensor of pointmass at o
		float o2 = NormSquared(o);
		j.SetX(Vector3(o2, 0, 0));
		j.SetY(Vector3(0, o2, 0));
		j.SetZ(Vector3(0, 0, o2));
		j.SetX( j.GetX() + o * -o.GetX());
		j.SetY( j.GetY() + o * -o.GetY());
		j.SetZ( j.GetZ() + o * -o.GetZ());

		//add inertia tensor of pointmass
		tensor.SetX( tensor.GetX() + masses[k] * j.GetX());
		tensor.SetY(tensor.GetY() + masses[k] * j.GetY());
		tensor.SetZ(tensor.GetZ() + masses[k] * j.GetZ());
	}
	
	Matrix3 rotation;
	Diagonalize(tensor, rotation, 0.00001f, 20);
	
	// 慣性モーメント
	inertia = Vector3(tensor.GetElement(0,0), tensor.GetElement(1,1), tensor.GetElement(2,2));
}
/****************************************************************************
*						ReCalculateLocalAABB
*************************************************************************//**
*  @fn        void CompoundShape::ReCalculateLocalAABB()
*  @brief     Recreate AABB tree all children
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void CompoundShape::ReCalculateLocalAABB()
{
	_localAABBMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	_localAABBMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	//extend the local aabbMin/aabbMax
	for (int j = 0; j < _children.size(); j++)
	{
		Vector3 localAabbMin, localAabbMax;
		_children[j].ChildShape->GetAABB(_children[j].Trans, localAabbMin, localAabbMax);
		if ((float)_localAABBMin.GetX() > localAabbMin.GetX()) { _localAABBMin.SetX(localAabbMin.GetX()); }
		if ((float)_localAABBMax.GetX() < localAabbMax.GetX()) { _localAABBMax.SetX(localAabbMax.GetX()); }
		if ((float)_localAABBMin.GetY() > localAabbMin.GetY()) { _localAABBMin.SetY(localAabbMin.GetY()); }
		if ((float)_localAABBMax.GetY() < localAabbMax.GetY()) { _localAABBMax.SetY(localAabbMax.GetY()); }
		if ((float)_localAABBMin.GetZ() > localAabbMin.GetZ()) { _localAABBMin.SetZ(localAabbMin.GetZ()); }
		if ((float)_localAABBMax.GetZ() < localAabbMax.GetZ()) { _localAABBMax.SetZ(localAabbMax.GetZ()); }
	}
}
void CompoundShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
{
	AffineTransform ident;

	Vector3 aabbMin, aabbMax;
	GetAABB(ident, aabbMin, aabbMax);

	Vector3 halfExtents = (aabbMax - aabbMin) * 0.5f;

	float lx = 2.0f * (halfExtents.GetX());
	float ly = 2.0f * (halfExtents.GetY());
	float lz = 2.0f * (halfExtents.GetZ());

	inertia.SetElement(0, mass / (12.0f) * (ly * ly + lz * lz));
	inertia.SetElement(1, mass / (12.0f) * (lx * lx + lz * lz));
	inertia.SetElement(2, mass / (12.0f) * (lx * lx + ly * ly));
}

/****************************************************************************
*						GetAABB
*************************************************************************//**
*  @fn        void CompoundShape::GetAABB(const gm::Transform& t, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
*  @brief     Get AABB
*  @param[in] Transform transform
*  @param[in] Vector3 aabbMin
*  @param[in] Vector3 aabbMax
*  @return 　　void
*****************************************************************************/
void CompoundShape::GetAABB(const gm::AffineTransform& t, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
{
	Vector3 localHalfExtents = 0.5f * (_localAABBMax - _localAABBMin);
	Vector3 localCenter      = 0.5f * (_localAABBMax + _localAABBMin);

	//avoid an illegal AABB when there are no children
	if (!_children.size())
	{
		localHalfExtents = Vector3(0, 0, 0);
		localCenter      = Vector3(0, 0, 0);
	}
	localHalfExtents += Vector3(GetMargin(), GetMargin(), GetMargin());

	gm::Matrix3 abs_b = Absolute(t.GetBasis());
	Vector3 center = t.GetBasis() * localCenter + t.GetTranslation();

	Vector3 extent = abs_b * localHalfExtents;
	aabbMin = center - extent;
	aabbMax = center + extent;
}

void CompoundShape::SetLocalScaling(const gm::Vector3& scaling)
{
	for (int i = 0; i < _children.size(); i++)
	{
		AffineTransform childTrans = GetChildTransform(i);
		Vector3 childScale = _children[i].ChildShape->GetLocalScaling();
		//		childScale = childScale * (childTrans.getBasis() * scaling);
		childScale = childScale * scaling / _localScaling;
		_children[i].ChildShape->SetLocalScaling(childScale);
		childTrans.SetTranslation(childTrans.GetTranslation() * scaling / _localScaling);
		UpdateChildTransform(i, childTrans, false);
	}

	_localScaling = scaling;
	ReCalculateLocalAABB();
}
#pragma endregion Public Function
