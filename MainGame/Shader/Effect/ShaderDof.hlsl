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
#define MAX_DEPTH (1.0f)
cbuffer BlurEffect : register(b0)
{
    float2 TextureSize; //x: width, y: height
    float Radius;

}

cbuffer DepthField : register(b1)
{
    float NearClip;
    float FarClip;
};

// common
RWTexture2D<float4> OutputImage : register(u0);
RWTexture2D<float4> OutputImage1 : register(u1);
// vertical blur
Texture2D<float4> SourceTexture : register(t0);


// rhomboid blur
Texture2D<float4> BlurTexture0 : register(t0);
Texture2D<float4> BlurTexture1 : register(t1);

// Final blur
Texture2D<float4> BlurTexture  : register(t0);
Texture2D<float4> DepthTexture : register(t1);
Texture2D<float4> SourceTextureFinal : register(t2);

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
[numthreads(8, 8, 1)]
void VerticalBlur(uint3 id : SV_DispatchThreadID)
{
    float2 uv = float2((float2) (id.xy) / TextureSize);
    float blurStepLength = Radius / 4.0f;
    
    /*-------------------------------------------------------------------
	-        Calculate uvoffset step size
	---------------------------------------------------------------------*/
    float2 uvOffset = float2(0.0f, 1.0f / TextureSize.y);
    uvOffset *= blurStepLength;
    
    /*-------------------------------------------------------------------
	-        Vertical blur (足し合わせたときに0～1に切られてから1/4されるバグ)
	---------------------------------------------------------------------*/
    float4 outputVertical = float4(0,0,0,0);
    outputVertical += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 1, 0);
    outputVertical += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 2, 0);
    outputVertical += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 3, 0);
    outputVertical += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 4, 0);
    outputVertical /= 4.0f;
    
    OutputImage[id.xy] = outputVertical;
    /*-------------------------------------------------------------------
	-        Diagonal blur
	---------------------------------------------------------------------*/
    uvOffset.x = 0.86602f / TextureSize.x; // sin 60.
    uvOffset.y = -0.5f / TextureSize.y;
    uvOffset *= blurStepLength;
    
    float4 outputDiagonal = float4(0, 0, 0, 0);
    outputDiagonal += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 1, 0);
    outputDiagonal += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 2, 0);
    outputDiagonal += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 3, 0);
    outputDiagonal += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 4, 0);
    outputDiagonal += SourceTexture.SampleLevel(SamplerLinearWrap, uv, 0);
    outputDiagonal /= 5.0f;
    
    float4 outputTotal = OutputImage[id.xy] + outputDiagonal;
    outputTotal /= 2.0f;
    OutputImage1[id.xy] = outputTotal;
}

[numthreads(8, 8, 1)]
void RhomboidBlur(uint3 id : SV_DispatchThreadID)
{
    float2 uv = float2((float2) (id.xy) / TextureSize);
    float blurStepLength = Radius / 4.0f;
    
     /*-------------------------------------------------------------------
	-        Calculate uvoffset step size
	---------------------------------------------------------------------*/
    float2 uvOffset;
    uvOffset.x = 0.86602f / TextureSize.x; // sin 60.
    uvOffset.y = -0.5f / TextureSize.y;
    uvOffset *= blurStepLength;
    
    /*-------------------------------------------------------------------
	-        left down
	---------------------------------------------------------------------*/
    float4 output = float4(0, 0, 0, 0);
    output += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 1, 0);
    output += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 2, 0);
    output += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 3, 0);
    output += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 4, 0);
    
    /*-------------------------------------------------------------------
	-        right down
	---------------------------------------------------------------------*/
    uvOffset.x = -0.86602f / TextureSize.x * blurStepLength;
    output += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 1, 0);
    output += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 2, 0);
    output += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 3, 0);
    output += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 4, 0);
    output += SourceTexture.SampleLevel(SamplerLinearWrap, uv + uvOffset * 0, 0);
    output /= 9.0f;
    OutputImage[id.xy] = output;
}

[numthreads(8, 8, 1)]
void FinalBlur( uint3 id : SV_DispatchThreadID )
{
    float2 uv   = float2((float2) (id.xy) / TextureSize);
    float depth = DepthTexture.SampleLevel(SamplerLinearWrap, uv, 0).x;
    float4 blur = BlurTexture .SampleLevel(SamplerLinearWrap, uv, 0);
    
    // 最大値を超えたらボケ画像出して, 最小値以下なら元々の画像を出す.間はまぜる
    if      (depth < NearClip) {blur.a = 0.0f;}              // perfectly sourceTexture
    else if (depth > FarClip)  { blur.a = 1.0f; } // perfectly blur texture
    else { blur.a = (depth - NearClip) / (FarClip - NearClip); }
    OutputImage[id.xy] = blur.a * blur + (1.0f - blur.a) * SourceTextureFinal[id.xy];
}

#endif