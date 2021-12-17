//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderConstantBuffer.hlsli
//            Content:  Constant Buffer
//             Author:  Toide Yutaro
//             Create:  2020_03_10
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_CONSTANT_BUFFER_3D_HLSLI
#define SHADER_CONSTANT_BUFFER_3D_HLSLI
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../Lighting/ShaderLightType.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define NUM_DIRECTIONAL_LIGHT 4
#define NUM_POINT_LIGHT 100
#define NUM_SPOT_LIGHT  20
cbuffer ObjectConstants : register(b0)
{
	matrix World;
    int    TextureConfig;
    int3   ObjectPadding;
}

cbuffer SceneConstants : register(b1)
{
    matrix View;
    matrix InverseView;
    matrix Projection;
    matrix InverseProjection;
    matrix ProjectionView;
    matrix InverseProjectionView;
    float3 EyePosition;
    float  Padding1;
    float2 RenderTargetSize;
    float2 InverseRenderTargetSize;
    float  NearZ;
    float  FarZ;
    float  TotalTime;
    float  DeltaTime;
}

cbuffer MaterialConstants : register(b2)
{
    float4  Diffuse;
    float3  Specular;
    float   Roughness;
    float3  EmissiveColor;
    float   EmissiveIntensity;
    float3  Ambient;
    float   Metalness;
}


cbuffer SceneLights : register(b3)
{
    int PointLightNum; // for lighting
    int SpotLightNum; // for lighting
    int2 Padding;
    
    DirectionalLight DirectionalLights[NUM_DIRECTIONAL_LIGHT];
    PointLight       PointLights      [NUM_POINT_LIGHT];
    SpotLight        SpotLights       [NUM_SPOT_LIGHT];
    
};


//ConstantBuffer<SceneLights> SceneLightConstants : register(b3);

#endif