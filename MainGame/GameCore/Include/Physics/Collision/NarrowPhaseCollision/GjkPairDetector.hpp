//////////////////////////////////////////////////////////////////////////////////
//            @file　　  VoronoiSimplexSolver.hpp
//            @brief    VoronoiSimplexSolver
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
#ifndef GJK_PAIR_DETECTOR_HPP
#define GJK_PAIR_DETECTOR_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DiscreteCollisionDetectorInterface.hpp"
#include "VoronoiSimplexSolver.hpp"
class ConvexShape;
class ConvexPenetrationDepthSolver;


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class GJKPairDetector : public DiscreteCollisionDetectorInterface
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	int LastUsedMethod;
	int CurrentIteration;
	int DegenerateSimplex;
	int CatchDegeneracies;
	int FixContactNormalDirection;

	virtual void GetClosestPoints (const ClosestPointInput& input, Result& output, class IDebugDraw* debugDraw, bool swapResults = false) override;
	void  GetClosestPointsNonVirtual(const ClosestPointInput& input, Result& output, class IDebugDraw* debugDraw);
	const gm::Vector3& GetCachedSeparatingAxis() const { return _cachedSeparatingAxis; }
	float GetCachedSeparatingDistance         () const { return _cachedSeparatingDistance; }

	void SetMinkowskiA(const ConvexShape* minkowskiA) { _minkowskiA = minkowskiA; }
	void SetMinkowskiB(const ConvexShape* minkowskiB) { _minkowskiB = minkowskiB; }
	void SetCachedSeparatingAxis(const gm::Vector3& separatingAxis) { _cachedSeparatingAxis = separatingAxis; }
	void SetPenetrationDepthSolver(ConvexPenetrationDepthSolver* penetrationDepthSolver) { _penetrationDepthSolver = penetrationDepthSolver; }
	void SetIgnoreMargin(bool ignoreMargin) { _ignoreMargin = ignoreMargin; }


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	GJKPairDetector(const ConvexShape* objectA, const ConvexShape* objectB, VolonoiSimplexSolver* simplexSolver, ConvexPenetrationDepthSolver* penetrationDepthSolver);
	GJKPairDetector(const ConvexShape* objectA, const ConvexShape* objectB, int shapeTypeA, int shapeTypeB, float marginA, float marginB, VolonoiSimplexSolver* simplexSolver, ConvexPenetrationDepthSolver* penetrationDepthSolver);
	~GJKPairDetector() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::Vector3                   _cachedSeparatingAxis;
	ConvexPenetrationDepthSolver* _penetrationDepthSolver;
	VolonoiSimplexSolver*         _simplexSolver;
	const ConvexShape*            _minkowskiA;
	const ConvexShape*            _minkowskiB;
	int                           _shapeTypeA;
	int                           _shapeTypeB;
	float                         _marginA;
	float                         _marginB;
	bool                          _ignoreMargin;
	float                         _cachedSeparatingDistance;

};
#endif