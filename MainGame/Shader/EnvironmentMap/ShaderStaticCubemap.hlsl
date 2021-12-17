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
#define VertexIn  VSInputPositionNormalColorTexture
struct VertexOut
{
    float4 Position : SV_POSITION;
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
    result.CubemapLookDirection = normalize(vertexIn.Position.xyz);
    
    /*-------------------------------------------------------------------
	-        transform to world space 
	---------------------------------------------------------------------*/
    float4 positionWorld = mul(World, float4(vertexIn.Position.xyz, 1.0f));
    positionWorld.xyz += EyePosition; // always center sky about camera
    
     /*-------------------------------------------------------------------
	-        Skybox always on far plane
	---------------------------------------------------------------------*/
    result.Position = mul(ProjectionView, positionWorld).xyww;
    return result;
}

float4 PSMain(VertexOut input) : SV_TARGET
{
    return CubeMap.Sample(SamplerLinearWrap, input.CubemapLookDirection);

}
#endif