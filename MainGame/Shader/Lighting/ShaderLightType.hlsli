//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderLightType.hlsli
//            Content:  Define Shader LightType
//             Author:  Toide Yutaro
//             Create:  
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_LIGHTTYPE_HLSLI
#define SHADER_LIGHTTYPE_HLSLI
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
// 48 bytes
struct PointLight
{
    float3 Position;
    float  Range;
    float3 Color;
    float  Brightness;
    float3 Attenuation;
    float  DepthBias;
};

// 48 bytes
struct SpotLight
{
    float3 Position;
    float  OuterConeAngle;
    float3 Color;
    float  Brightness;
    float3 Direction;
    float  DepthBias;
    float  InnerConeAngle;
    float3 Dummy;
};

struct DirectionalLight
{
    float3 Direction;
    float  Brightness;
    float3 Color;
    float  DepthBias;
    int    Shadowing;
    int    Enabled;
};

#endif