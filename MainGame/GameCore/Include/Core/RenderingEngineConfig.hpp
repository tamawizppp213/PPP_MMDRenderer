//////////////////////////////////////////////////////////////////////////////////
//              Title:  RenderingEngineConfig.hpp
//            Content:  RenderingEngineConfig
//             Author:  Toide Yutaro
//             Create:  2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef RENDERING_ENGINE_CONFIG_HPP
#define RENDERING_ENGINE_CONFIG_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Rendering/LightType.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                         Define
//////////////////////////////////////////////////////////////////////////////////
#define NUM_POINT_LIGHTS           100
#define NUM_SPOT_LIGHTS            20
#define NUM_DIRECTIONAL_LIGHTS     4

struct Sprite;
struct Texture;
//////////////////////////////////////////////////////////////////////////////////
//                         Alias 
//////////////////////////////////////////////////////////////////////////////////
enum class RenderEffectFlag
{
	AllowSSAO = 0x0001
};

struct SpriteRenderInfo
{
	std::vector<Sprite>* SpriteListPtr;
	Texture* TexturePtr;
};

struct SceneLightConstants
{
	int PointLightNum = 0;
	int SpotLightNum  = 0;
	int Padding[2]    = { 0,0 };

	DirectionalLight DirectionalLights[NUM_DIRECTIONAL_LIGHTS];
	PointLight       PointLights      [NUM_POINT_LIGHTS];
	SpotLight        SpotLights       [NUM_SPOT_LIGHTS];

	SceneLightConstants()                                      = default;
	SceneLightConstants(const SceneLightConstants&)            = default;
	SceneLightConstants& operator=(const SceneLightConstants&) = default;
	SceneLightConstants(SceneLightConstants&&)                 = default;
	SceneLightConstants& operator=(SceneLightConstants&&)      = default;
	~SceneLightConstants();
};
#endif