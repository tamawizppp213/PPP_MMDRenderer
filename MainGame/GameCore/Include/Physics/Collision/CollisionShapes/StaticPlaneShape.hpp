//////////////////////////////////////////////////////////////////////////////////
///             @file   TemplateText.hpp
///             @brief  TemplateText
///             @author Toide Yutaro
///             @date   2021_10_06 (first checked) first checked -> no debug.
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef STATIC_PLANE_SHAPE_HPP
#define STATIC_PLANE_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConcaveShape.hpp"

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
__declspec(align(16))
class StaticPlaneShape : public ConcaveShape
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void ProcessAllTriangles(TriangleCallback* callback, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax) const override;
	virtual void CalculateLocalInertia(float mass, gm::Vector3& inertia) const override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	virtual void GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override;
	virtual const gm::Vector3& GetLocalScaling () const override;
	const gm::Vector3&         GetPlaneNormal  () const { return _planeNormal; }
	const float&               GetPlaneConstant() const { return _planeConstant; }

	virtual const char* GetName() const { return "STATICPLANE"; }
	virtual void SetLocalScaling(const gm::Vector3& scaling);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	StaticPlaneShape(const gm::Vector3& planeNormal, float planeConstant);
	virtual ~StaticPlaneShape() {};
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::Vector3 _localAABBMin;
	gm::Vector3 _localAABBMax;
	gm::Vector3 _planeNormal;
	gm::Vector3 _localScaling;
	float _planeConstant;
	
private :
	float _padding[3];
};

#endif