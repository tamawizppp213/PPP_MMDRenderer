//////////////////////////////////////////////////////////////////////////////////
//              Title:  GameConstantBufferConfig.hpp
//            Content:  For Default Constant Buffer Coniguration. 
//             Author:  Toide Yutaro (Reference: 3DGame Programming with DirectX12)
//             Create:  2021_03_13
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GAME_CONSTANT_BUFFER_CONFIG_HPP
#define GAME_CONSTANT_BUFFER_CONFIG_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Core.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/DirectX12MathHelper.hpp"
#include "GameCore/Include/HLSLUtility.hpp"
#include "GameCore/Include/Light/LightType.hpp"
#include <memory>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define NUM_POINT_LIGHTS           20
#define NUM_SPOT_LIGHTS            5
#define NUM_SHADOWING_POINT_LIGHTS 5
#define NUM_SHADOWING_SPOT_LIGHTS  5

//////////////////////////////////////////////////////////////////////////////////
//                              Struct
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			ObjectConstants
*************************************************************************//**
*  @struct    ObjectConstants
*  @brief     Object Infomation per frame
*****************************************************************************/
struct ObjectConstants
{
	hlsl::matrix World = DirectXMathHelper::Identity4X4();
};

/****************************************************************************
*				  			SceneConstants
*************************************************************************//**
*  @struct    SceneConstants
*  @brief     Scene Infomation per frame
*****************************************************************************/
struct SceneConstants
{
	hlsl::matrix View                    = DirectXMathHelper::Identity4X4();
	hlsl::matrix InverseView             = DirectXMathHelper::Identity4X4();
	hlsl::matrix Projection              = DirectXMathHelper::Identity4X4();
	hlsl::matrix InverseProjection       = DirectXMathHelper::Identity4X4();
	hlsl::matrix ViewProjection          = DirectXMathHelper::Identity4X4();
	hlsl::matrix InverseViewProjection   = DirectXMathHelper::Identity4X4();
	hlsl::float3 EyePosition             = { 0.0f, 0.0f, 0.0f };
	float        cbPerObjectPad1         = 0.0f;
	hlsl::float2 RenderTargetSize        = { 0.0f, 0.0f };
	hlsl::float2 InverseRenderTargetSize = { 0.0f, 0.0f };

	float  NearZ                         = 0.0f;
	float  FarZ                          = 0.0f;
	float  TotalTime                     = 0.0f;
	float  DeltaTime                     = 0.0f;
};

/****************************************************************************
*				  			MaterialConstants
*************************************************************************//**
*  @struct    MaterialConstants
*  @brief     Material Infomation per frame
*****************************************************************************/
struct MaterialConstants
{
	hlsl::float3 Diffuse;
	float        Alpha;
	hlsl::float3 EmissiveColor;
	float        EmissiveIntensity;
	hlsl::float3 Specular;
	float        Metalness;
	hlsl::float2 UVScale;
	float        Roughness;
	int          TextureConfig;
};

/****************************************************************************
*				  			SceneLightConstants
*************************************************************************//**
*  @struct    SceneLightConstants
*  @brief     SceneLight Infomation per frame
*****************************************************************************/
struct SceneLightConstants
{
	int PointLightNum;
	int SpotLightNum;
	int PointCasterNum;
	int SpotCasterNum;

	DirectionalLight Directional;
	hlsl::matrix     ShadowViewDirectional;
	PointLight       PointLights [NUM_POINT_LIGHTS];
	PointLight       PointCasters[NUM_SHADOWING_SPOT_LIGHTS];
	SpotLight        SpotLights  [NUM_SPOT_LIGHTS];
	SpotLight        SpotCasters [NUM_SHADOWING_SPOT_LIGHTS];
	hlsl::matrix     ShadowViews [NUM_SHADOWING_SPOT_LIGHTS];
};
#endif