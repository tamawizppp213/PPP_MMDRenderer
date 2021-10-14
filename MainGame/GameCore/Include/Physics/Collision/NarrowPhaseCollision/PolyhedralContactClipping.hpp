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
#ifndef POLYHEDRAL_CONTACT_CLIPPING_HPP
#define POLYHEDRAL_CONTACT_CLIPPING_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DiscreteCollisionDetectorInterface.hpp"
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class ConvexPolyhedron;
using VertexArray = gm::AlignedObjectArray<gm::Vector3>;

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			PolyhedralContactClipping
*************************************************************************//**
*  @class     PolyhedralContactClipping
*  @brief     Polyhedral contact clipping
*****************************************************************************/
class PolyhedralContactClipping
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static void ClipHullAgainstHull(const gm::Vector3& separatingNormal, const ConvexPolyhedron& hullA, const ConvexPolyhedron& hullB, const gm::AffineTransform& transformA, const gm::AffineTransform& transformB, const float minDist, float maxDist, VertexArray& worldVertexB1, VertexArray& worldVertexB2, DiscreteCollisionDetectorInterface::Result& resultOut);
	static void ClipFaceAgainstHull(const gm::Vector3& separatingNormal, const ConvexPolyhedron& hullA, const gm::AffineTransform& transformA, VertexArray& worldVertexB1, VertexArray& worldVertexB2, const float minDist, float maxDist, DiscreteCollisionDetectorInterface::Result& resultOut);
	static bool FindSeparatingAxis(const ConvexPolyhedron& hullA, const ConvexPolyhedron& hullB, const gm::AffineTransform& transformA, const gm::AffineTransform& transformB, gm::Vector3& sep, DiscreteCollisionDetectorInterface::Result& resultOut);
	static void ClipFace(const VertexArray& vertexIn, VertexArray& vertexOut, const gm::Vector3& planeNormalWS, float planeEquationWS);


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif