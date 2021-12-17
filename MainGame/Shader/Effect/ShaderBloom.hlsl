//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderBloom.hlsl
//            Content:  Draw Bloom Effect (compute shader)
//             Author:  Toide Yutaro
//             Create:  2021_07_26
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_BLOOM_HPP
#define SHADER_BLOOM_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../Core/ShaderStaticSampler.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////

Texture2D<float4> MainTexture : register(t0);
Texture2D<float4> BlurTexture0 : register(t1);
Texture2D<float4> BlurTexture1 : register(t2);
Texture2D<float4> BlurTexture2 : register(t3);
Texture2D<float4> BlurTexture3 : register(t4);

RWTexture2D<float4> OutputImage : register(u0);

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
[numthreads(8, 8, 1)]
void SamplingLuminance(uint3 id : SV_DispatchThreadID)
{
    int2 pixelID = int2(id.x, id.y);
    float4 color = MainTexture[pixelID];
    float t = dot(color.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    if (t - 0.99f < 0.0f)
    {
        color = float4(0, 0, 0, 1);
    }

    OutputImage[id.xy] = color;
}


[numthreads(8, 8, 1)]
void FinalBloom(uint3 id : SV_DispatchThreadID)
{
    uint2 pixelID = int2(id.x, id.y);
    float4 color = BlurTexture0[pixelID];
    color += BlurTexture1[pixelID / 2];
    color += BlurTexture2[pixelID / 4];
    color += BlurTexture3[pixelID / 8];
    color /= 4.0f;
    
    color.a = 1.0f;
    OutputImage[pixelID] = color + MainTexture[pixelID];
}
#endif