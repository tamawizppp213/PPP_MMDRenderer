//////////////////////////////////////////////////////////////////////////////////
///             @file   EmptyShape.hpp
///             @brief  EmptyShape
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
#include "GameCore/Include/Physics/Collision/CollisionShapes/EmptyShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
EmptyShape::EmptyShape() : ConcaveShape()
{
	_shapeType = (int)BroadPhaseNativeTypes::Empty_Shape;
}

void EmptyShape::GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
{
	Vector3 margin(GetMargin(), GetMargin(), GetMargin());

	aabbMin = transform.GetTranslation() - margin;

	aabbMax = transform.GetTranslation() + margin;
}

void EmptyShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
{
	assert(0);
}

void EmptyShape::ProcessAllTriangles(TriangleCallback*, const gm::Vector3&, const gm::Vector3&)const
{

}