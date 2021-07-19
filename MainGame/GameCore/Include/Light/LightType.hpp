//////////////////////////////////////////////////////////////////////////////////
///             @file   FrameResource.hpp
///             @brief  Per Frame Resource Manager
///             @author Toide Yutaro
///             @date   2020_11_
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
	hlsl::float3 Position;
	float        Range;
	hlsl::float3 Color;
	float        Brightness;
	hlsl::float3 Attenuation;
	float        DepthBias;
};

struct SpotLight
{
	hlsl::float3 Position;
	float        OuterConeAngle;
	hlsl::float3 Color;
	float        Brightness;
	hlsl::float3 Direction;
	float        DepthBias;
	float        InnerConeAngle;
	hlsl::float3 Dummy;
};

struct DirectionalLight
{
	hlsl::float3 Direction;
	float        Brightness;
	hlsl::float3 Color;
	float        DepthBias;
	int          Shadowing;
	int          Enabled;
};
#endif