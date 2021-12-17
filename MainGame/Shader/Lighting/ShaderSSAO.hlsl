//////////////////////////////////////////////////////////////////////////////////
//             Title:  ShaderSSAO.hlsl
//             Content:  ssao (compute shader)
//             Author:  Toide Yutaro 
//             Create:  2021_08_14
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_SSAO_HPP
#define SHADER_SSAO_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../Core/ShaderConstantBuffer3D.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
cbuffer TextureInfo : register(b0)
{
    float2 TextureSize;
    float OcclusionRadius;    // view space
    float OcclusionSharpness;
    
    float OcclusionFadeStart; // view space
    float OcclusionFadeEnd;   // view space
    float SurfaceEpsilon; // view space
    float PaddingTextureInfo;
    
    float4 OffsetVectors[14];
}

Texture2D<float4> NormalTexture   : register(t0);
Texture2D<float>  DepthTexture    : register(t1);
Texture2D<float4> RandomTexture   : register(t2);
RWTexture2D<float4> OutputTexture : register(u0);

SamplerState SamplerDepth       : register(s0);
SamplerState SamplerPointClamp  : register(s1);
SamplerState SamplerLinearWrap : register(s2);

static const int SampleCount = 14;
static const float2 UVs[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};


struct VertexOut
{
    float4 Position     : SV_Position;
    float3 ViewPosition : POSITION;
    float2 UV           : TEXCOORD0;
    float4x4 ProjectionTexture : PROJECTION;
};

float OcclusionFunction(float distZ)
{
    //       1.0     -------------\
	//               |           |  \
	//               |           |    \
	//               |           |      \ 
	//               |           |        \
	//               |           |          \
	//               |           |            \
	//  ------|------|-----------|-------------|---------|--> zv
	//        0     Eps          z0            z1        
	//
    
    float occulusion = 0.0f;
    if (distZ > SurfaceEpsilon)
    {
        float fadeLength = OcclusionFadeEnd - OcclusionFadeStart;
        occulusion = saturate((OcclusionFadeEnd - distZ) / fadeLength);
    }
    return occulusion;
}

float NDCDepthToViewDepth(float z_ndc)
{
    float viewZ = Projection[3][2] / (z_ndc - Projection[2][2]);
    return viewZ;
}

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
VertexOut VSMain(uint vid : SV_VertexID)
{
    VertexOut vertex;
    vertex.UV = UVs[vid];
    vertex.Position = float4(2.0f * vertex.UV.x - 1.0f, 1.0f - 2.0f * vertex.UV.y, 0.0f, 1.0f);
    float4 position = mul(InverseProjection, vertex.Position);
    vertex.ViewPosition = position.xyz / position.w;
    
    float4x4 T = 
    {
        0.5f, 0.0f, 0.0f, 0.5f,
        0.0f, -0.5f, 0.0f, 0.5f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    vertex.ProjectionTexture = mul(T, Projection);
    return vertex;
}

float4 SSAO(VertexOut input) : SV_Target
{
    /*-------------------------------------------------------------------
	-        Get ssao variables
	---------------------------------------------------------------------*/
    float  depth      = DepthTexture.SampleLevel(SamplerDepth, input.UV, 0);
    float  viewDepth  = NDCDepthToViewDepth(depth);
    float3 viewPosition = (viewDepth / input.ViewPosition.z) * input.ViewPosition;
    float3 normal     = normalize((NormalTexture.SampleLevel(SamplerPointClamp, input.UV, 0).xyz * 2) - 1); // world
    float3 viewNormal = normalize(mul(View, float4(normal, 0))).xyz;
    float3 random = normalize(2.0f * RandomTexture.SampleLevel(SamplerLinearWrap, 4.0f * input.UV, 0).xyz - 1); // - 1Å` 1
    
    float occlusionSum = 0.0f;
    float division     = 0.0f;
    
     /*-------------------------------------------------------------------
	-     SSAO
	---------------------------------------------------------------------*/
    for (int i = 0; i < SampleCount; ++i)
    {
        float3 offset = reflect(OffsetVectors[i].xyz, random); 
        float flip    = sign(dot(offset, viewNormal));
           
        float3 sample = viewPosition.xyz + random * OcclusionRadius * flip;
            
        float4 projectionSample = 0.0f;
        projectionSample        = mul(input.ProjectionTexture, float4(sample, 1.0f));
        projectionSample       /= projectionSample.w;
        
        float sampleDepth         = DepthTexture.SampleLevel(SamplerDepth, projectionSample.xy, 0);
        float viewSampleDepth     = NDCDepthToViewDepth(sampleDepth);
        float3 viewSamplePosition = (viewSampleDepth / sample.z) * sample;
        
        float distZ = viewPosition.z - viewSamplePosition.z;
        float dp    = max(dot(viewNormal, normalize(viewSamplePosition - viewPosition)), 0.0f);
        float occlusion = dp * OcclusionFunction(distZ);
        occlusionSum += occlusion; // if(depth < result){return 1;}

    }
    occlusionSum /= SampleCount;

    return saturate(pow(1.0f - occlusionSum, OcclusionSharpness)); // sharpness
}



#endif

