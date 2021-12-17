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
// 32 bytes
struct PointLight
{
    float3 Position;
    float  Range;
    float3 Color;
    float  Brightness;
    
};

// 64 bytes
struct SpotLight
{
    float3 Position;
    float  Range;
    float3 Color;
    float  Brightness;
    float3 Direction;
    float  InnerConeAngle;
    float  OuterConeAngle;
    float3 Dummy;
};

// 32 bytes
struct DirectionalLight
{
    float3 Direction;
    float  Brightness;
    float3 Color;
    float  Padding;
};

#endif