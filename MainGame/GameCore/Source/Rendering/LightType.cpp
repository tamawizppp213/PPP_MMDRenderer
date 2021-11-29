//////////////////////////////////////////////////////////////////////////////////
///             @file   LightType.hpp
///             @brief  LightType
///             @author Toide Yutaro
///             @date   2021_08_12
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Rendering/LightType.hpp"

using namespace gm;
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
DirectionalLight::DirectionalLight()
{
	Direction  = hlsl::float3(0, 0, 0);
	Brightness = 1.0f;
	Color      = hlsl::float3(1, 1, 1);
	Padding    = 0.0f;
}

DirectionalLight::DirectionalLight(Float3 direction)
{
	Direction  = direction;
	Brightness = 1.0f;
	Color      = hlsl::float3(1, 1, 1);
	Padding    = 0.0f;
}

DirectionalLight::DirectionalLight(Float3 direction, float brightness, Float3 color)
{
	Direction  = direction;
	Brightness = brightness;
	Color      = color;
	Padding = 0.0f;
}

DirectionalLight::DirectionalLight(Float3 direction, float brightness)
{
	Direction  = direction;
	Brightness = brightness;
	Color      = hlsl::float3(1, 1, 1);
	Padding = 0.0f;
}

PointLight::PointLight()
{
	Position   = Float3(0.0f, 0.0f, 0.0f);
	Range      = 1.0f;
	Color      = Float3(1.0f, 1.0f, 1.0f);
	Brightness = 1.0f;
}

PointLight::PointLight(Float3 position, float range, float brightness, Float3 color)
{
	Position   = position;
	Range      = range;
	Color      = color;
	Brightness = brightness;
}

SpotLight::SpotLight(
)
{
	Position       = Float3(0.0f, 0.0f, 0.0f);
	Range          = 1.0f;
	Color          = Float3(0, 0, 0);
	Brightness     = 1.0f;
	Direction      = Float3(0, 0, 0);
	InnerConeAngle = 3.141592f / 6.0f;
	OuterConeAngle = 3.141592f / 4.0f;
}
SpotLight::SpotLight(gm::Float3 position, float range, float brightness,
	gm::Float3 direction,
	gm::Float3 color,
	float innerConeAngle,
	float outerConeAngle)
{
	Position       = position;
	Range          = range;
	Color          = color;
	Brightness     = brightness;
	Direction      = direction;
	InnerConeAngle = innerConeAngle;
	OuterConeAngle = outerConeAngle;
}