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
//#include "../Lighting/ShaderLighting.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
cbuffer ObjectConstants : register(b0)
{
	matrix World;
}

cbuffer SceneConstants : register(b1)
{
    matrix View;
    matrix InverseView;
    matrix Projection;
    matrix InverseProjection;
    matrix ViewProjection;
    matrix InverseViewProjection;
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
    float3 Diffuse;
    float  Alpha;
    float3 EmissiveColor;
    float  EmissiveIntensity;
    float3 Specular;
    float  Metalness;
    float2 UVScale;
    float  Roughness;
    int    TextureConfig;
    
}

/*
cbuffer SceneLightConstants : register(b3)
{
    int PointLightNum;// for lighting
    int SpotLightNum; // for lighting
    int PointCaster;  // for shadowing
    int SpotCaster;   // for shadowing

    //------------------------------------------
    DirectionalLight Directional;
    matrix ShadowViewDirectional;
    //------------------------------------------
    PointLight PointLights [NUM_POINT_LIGHTS];
    PointLight PointCasters[NUM_SHADOWING_POINT_LIGHTS];
    //------------------------------------------
    SpotLight SpotLights [NUM_SPOT_LIGHTS];
    SpotLight SpotCasters[NUM_SHADOWING_SPOT_LIGHTS];
    //------------------------------------------
    matrix ShadowViews[NUM_SHADOWING_SPOT_LIGHTS];
}
*/

#endif