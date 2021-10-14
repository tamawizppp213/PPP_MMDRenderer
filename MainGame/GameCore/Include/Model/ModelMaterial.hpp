//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12ModelLoader.hpp
///             @brief  Load Model
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MODEL_MATERIAL_HPP
#define MODEL_MATERIAL_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMVector.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct PBRMaterial
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Float4 Diffuse;
	gm::Float3 Specular;
	float      Roughness;
	gm::Float3 EmissiveColor;
	float      EmissiveIntensity;
	gm::Float3 Ambient;
	float      Metalness;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PBRMaterial()
	{
		this->Diffuse           = gm::Float4(1, 1, 1, 1);
		this->Specular          = gm::Float3(1, 1, 1);
		this->Roughness         = 0.5f;
		this->EmissiveColor     = gm::Float3(1, 1, 1);
		this->EmissiveIntensity = 0.0f;
		this->Ambient           = gm::Float3(0.5f, 0.5f, 0.5f);
		this->Metalness         = 0.0f;
	};
	~PBRMaterial()                             = default;
	PBRMaterial(const PBRMaterial&)            = default;
	PBRMaterial& operator=(const PBRMaterial&) = default;
	PBRMaterial(PBRMaterial&&)                 = default;
	PBRMaterial& operator=(PBRMaterial&&)      = default;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif