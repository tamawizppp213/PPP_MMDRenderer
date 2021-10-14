//////////////////////////////////////////////////////////////////////////////////
///             @file   LightType.hpp
///             @brief  LightType
///             @author Toide Yutaro
///             @date   2021_08_12
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef LIGHTING_TYPE_HPP
#define LIGHTING_TYPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/HLSLUtility.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct PointLight
{
	hlsl::float3 Position   = hlsl::float3(0,0,0);
	float        Range      = 1.0f;
	hlsl::float3 Color      = hlsl::float3(1,1,1);
	float        Brightness = 1.0f;

	PointLight();
	PointLight(gm::Float3 position, float range = 1.0f, float brightness = 1.0f, gm::Float3 color = gm::Float3(1, 1, 1));

};

struct SpotLight
{
	hlsl::float3 Position       = hlsl::float3(0,0,0);
	float        Range          = 1.0f;
	hlsl::float3 Color          = hlsl::float3(0,0,0);
	float        Brightness     = 1.0f;
	hlsl::float3 Direction      = hlsl::float3(0,0,0);
	float        InnerConeAngle = gm::GM_PI / 6.0f; // radian
	float        OuterConeAngle = gm::GM_PI / 4.0f; // radian
	hlsl::float3 Dummy;

	SpotLight();
	SpotLight(gm::Float3 position, float range = 1.0f, float brightness = 1.0f,
		gm::Float3 direction = gm::Float3(0, 0, 0),
		gm::Float3 color = gm::Float3(1, 1, 1),
		float innerConeAngle = gm::GM_PI / 6.0f,
		float outerConeAngle = gm::GM_PI / 4.0f);
};

struct DirectionalLight
{
	hlsl::float3 Direction  = hlsl::float3(0,0,0);
	float        Brightness = 1.0f;
	hlsl::float3 Color      = hlsl::float3(1,1,1);
	float        Padding    = 0.0f; 

	DirectionalLight();
	DirectionalLight(gm::Float3 direction, float brightness);
	DirectionalLight(gm::Float3 direction, float brightness, gm::Float3 color);
	DirectionalLight(gm::Float3 direction);
};


#endif