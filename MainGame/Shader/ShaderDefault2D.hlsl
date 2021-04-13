//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderDefault2D.hlsl
//            Content:  Shader Default 2D (Vertex, Pixel Shader)
//             Author:  Toide Yutaro
//             Create:  2020_03_18
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_DEFAULT_2D_HLSLI
#define SHADER_DEFAULT_2D_HLSLI
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Core/ShaderBasicStruct.hlsli"
#include "Core/ShaderConstantBuffer2D.hlsli"
#include "Core/ShaderStaticSampler.hlsli"
#include "Core/ShaderTexture.hlsli"
#include "Core/ShaderColor.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define VertexIn  VSInputPositionNormalColorTexture
#define VertexOut VSOutputPositionNormalColorTexture

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
VertexOut VSMain(VertexIn vertexIn)
{
    VertexOut result;
    
    result.Position = vertexIn.Position;
    result.Position = mul(ViewProjection, result.Position);
    result.UV       = vertexIn.UV;
    result.Normal   = vertexIn.Normal;
    result.Color    = vertexIn.Color;
    
    return result;
}

float4 PSMain(VertexOut input) : SV_TARGET
{
    float4 result = input.Color * float4(DiffuseMap.Sample(SamplerLinearWrap, input.UV));
    return result;
}

float4 PSHDRMain(VertexOut input) : SV_TARGET
{
    float4 result;
    float3 rec2020  = Rec709ToRec2020(input.Color.rgb);
    float3 hdr10    = LinearToST2084(rec2020 * (80.0 / 10000.0));
    input.Color.rgb = hdr10;
    
    result = input.Color * float4(DiffuseMap.Sample(SamplerLinearWrap, input.UV));
    return result;
}
#endif