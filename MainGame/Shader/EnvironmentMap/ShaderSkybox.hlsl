//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderSkybox.hlsli
//            Content:  Create Skybox
//             Author:  Toide Yutaro
//             Create:  2020_03_21
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_SKYBOX_HLSL
#define SHADER_SKYBOX_HLSL
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../Core/ShaderBasicStruct.hlsli"
#include "../Core/ShaderConstantBuffer3D.hlsli"
#include "../Core/ShaderStaticSampler.hlsli"
#include "../Core/ShaderTexture.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
//#define VertexIn  VSInputPositionNormalTexture
struct VertexIn
{
    float3 Position    : POSITION;
    float3 Normal      : NORMAL;
    float2 UV          : TEXCOORD;
};
struct VertexOut
{
    float4 Position             : SV_POSITION;
    float3 CubemapLookDirection : POSITION;
};

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
VertexOut VSMain(VertexIn vertexIn)
{
    VertexOut result;
    
    /*-------------------------------------------------------------------
	-         Use local vertex position as cubemap lookup vector
	---------------------------------------------------------------------*/
    result.CubemapLookDirection = vertexIn.Position.xyz;
    
    /*-------------------------------------------------------------------
	-        transform to world space 
	---------------------------------------------------------------------*/
    float4 positionWorld = mul(float4(vertexIn.Position.xyz, 1.0f), World);
    positionWorld.xyz   += EyePosition; // always center sky about camera
    
     /*-------------------------------------------------------------------
	-        Skybox always on far plane
	---------------------------------------------------------------------*/
    result.Position = mul(positionWorld, ViewProjection).xyww;
    return result;
}

float4 PSMain(VertexOut input) : SV_TARGET
{
    return CubeMap.Sample(SamplerLinearWrap, input.CubemapLookDirection);

}
#endif