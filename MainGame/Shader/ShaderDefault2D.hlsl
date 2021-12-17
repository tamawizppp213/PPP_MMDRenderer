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
    //result.Position = mul(ViewProjection, result.Position);
    result.UV       = vertexIn.UV;
    result.Normal   = vertexIn.Normal;
    result.Color    = vertexIn.Color;
    
    return result;
}

VertexOut VSBlend(VertexIn vertexIn)
{
    VertexOut result;
    
    result.Position = vertexIn.Position;
    //result.Position = mul(ViewProjection, result.Position);
    result.UV       = vertexIn.UV;
    result.Normal   = vertexIn.Normal;
    result.Color    = vertexIn.Color;
    
    return result;
}

// å„Ç≈è„èëÇ´ópÇ…ÇµÇƒÇ®Ç≠
float4 PSMain(VertexOut input) : SV_TARGET
{
    float4 result = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    return result;
}

float4 PSNormal(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 result = dest;
    return result;
}

float4 PSAddition(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap .Sample(SamplerLinearWrap, input.UV));
    
    float4 result;
    result.rgb = dest.rgb + source.rgb;
    result.a   = source.a;
    return result;
}

float4 PSSubtraction(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap .Sample(SamplerLinearWrap, input.UV));
    
    float4 result;
    result.rgb = dest.rgb - source.rgb;
    result.a   = source.a;
    return result;
}

float4 PSMultiplication(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap .Sample(SamplerLinearWrap, input.UV));
    
    float4 result;
    result.rgb = dest.rgb * source.rgb;
    result.a   = source.a;
    return result;
}

float4 PSDivision(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap .Sample(SamplerLinearWrap, input.UV));
    
    float4 result;
    result.rgb = dest.rgb / source.rgb;
    result.a   = source.a;
    return result;
}

float4 PSScreen(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap .Sample(SamplerLinearWrap, input.UV));
    
    float4 result;
    result.rgb = dest.rgb + source.rgb - dest.rgb * source.rgb;
    result.a   = source.a;
    return result;
}

float4 PSOverlay(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    
    float4 result;
    if (dest.r < 0.5)
        result.r = dest.r * source.r * 2;
    else
        result.r = 2 * (dest.r + source.r - dest.r * source.r) - 1.0;
    if (dest.g < 0.5)
        result.g = dest.g * source.g * 2;
    else
        result.g = 2 * (dest.g + source.g - dest.g * source.g) - 1.0;
    if (dest.b < 0.5)
        result.b = dest.b * source.b * 2;
    else
        result.b = 2 * (dest.b + source.b - dest.b * source.b) - 1.0;
    result.a = source.a;
    return result;
}


float4 PSDark(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap .Sample(SamplerLinearWrap, input.UV));
    
    float4 result;
    result.rgb = min(dest.rgb, source.rgb);
    result.a   = source.a;
    return result;
}

float4 PSLight(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 result;
    result.rgb = max(dest.rgb, source.rgb);
    result.a = source.a;

    return result;
}

float4 PSColorDark(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src    = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    result.rgb = (src.r + src.g + src.b > dest.r + dest.g + dest.b) ? dest.rgb : src.rgb;
    result.a   = src.a;

    return result;
}

float4 PSColorLight(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    result.rgb = (src.r + src.g + src.b > dest.r + dest.g + dest.b) ? src.rgb : dest.rgb;
    result.a = src.a;

    return result;
}

float4 PSBurnColor(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    result.rgb = 1.0 - (1.0 - dest.rgb) / src.rgb;
    result.a = src.a;

    return result;
}

float4 PSBurnLinear(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    result.rgb = dest.rgb + src.rgb <= 1.0 ? 0.0 : dest.rgb + src.rgb - 1.0;
    result.a = src.a;

    return result;
}

float4 PSDodgeColor(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    result.rgb = dest.rgb / (1.0 - src.rgb);
    result.a = src.a;

    return result;
}

float4 PSDodgeLinear(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    result.rgb = src.rgb + dest.rgb;
    result.a   = src.a;

    return result;
}

float4 PSSoftLight(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 result;
    if (source.r < 0.5)
        result.r = 2 * dest.r * source.r + pow(dest.r, dest.r) * (1.0f - 2 * source.r);
    else
        result.r = 2 * dest.r * (1.0 - source.r) + sqrt(dest.r) * (2 * source.r - 1.0);
    if (source.g < 0.5)
        result.g = 2 * dest.g * source.g + pow(dest.g, dest.g) * (1.0f - 2 * source.g);
    else
        result.g = 2 * dest.g * (1.0 - source.g) + sqrt(dest.g) * (2 * source.g - 1.0);
    if (source.b < 0.5)
        result.b = 2 * dest.b * source.b + pow(dest.b, dest.b) * (1.0f - 2 * source.b);
    else
        result.b = 2 * dest.b * (1.0 - source.b) + sqrt(dest.b) * (2 * source.b - 1.0);
    result.a = source.a;
    return result;
}

float4 PSHardLight(VertexOut input) : SV_TARGET
{
    float4 dest   = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 source = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    if (dest.r > 0.5)
        result.r = dest.r * source.r * 2;
    else
        result.r = 2 * (dest.r + source.r - dest.r * source.r) - 1.0;
    if (dest.g > 0.5)
        result.g = dest.g * source.g * 2;
    else
        result.g = 2 * (dest.g + source.g - dest.g * source.g) - 1.0;
    if (dest.b > 0.5)
        result.b = dest.b * source.b * 2;
    else
        result.b = 2 * (dest.b + source.b - dest.b * source.b) - 1.0;
    result.a = source.a;

    return result;
}

float4 PSVividLight(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src   = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    if (src.r < 0.5)
        result.r = 1.0 - (1.0 - dest.r) / (2 * src.r);
    else
        result.r = dest.r / (1.0 - 2 * (src.r - 0.5));
    if (src.g < 0.5)
        result.g = 1.0 - (1.0 - dest.g) / (2 * src.g);
    else
        result.g = dest.g / (1.0 - 2 * (src.g - 0.5));
    if (src.b < 0.5)
        result.b = 1.0 - (1.0 - dest.b) / (2 * src.b);
    else
        result.b = dest.b / (1.0 - 2 * (src.b - 0.5));
    result.a = src.a;

    return result;
}

float4 PSLinearLight(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    if (src.r < 0.5)
        result.r = dest.r + 2 * src.r - 1.0;
    else
        result.r = dest.r + 2 * (src.r - 0.5);
    if (src.g < 0.5)
        result.g = dest.g + 2 * src.g - 1.0;
    else
        result.g = dest.g + 2 * (src.g - 0.5);
    if (src.b < 0.5)
        result.b = dest.b + 2 * src.b - 1.0;
    else
        result.b = dest.b + 2 * (src.b - 0.5);
    result.a = src.a;

    return result;
}

float4 PSPinLight(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    if (src.r < 0.5)
        result.r = min(dest.r, 2 * src.r);
    else
        result.r = max(dest.r, 2 * (src.r - 0.5));
    if (src.g < 0.5)
        result.g = min(dest.g, 2 * src.g);
    else
        result.g = max(dest.g, 2 * (src.g - 0.5));
    if (src.b < 0.5)
        result.b = min(dest.b, 2 * src.b);
    else
        result.b = max(dest.b, 2 * (src.b - 0.5));
    result.a = src.a;

    return result;
}

float4 PSHardMix(VertexOut input) : SV_TARGET
{
	// ñ¢é¿ëï
    return float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
}

float4 PSAbsoluteSubtract(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    result.rgb = abs(src.rgb - dest.rgb);
    result.a   = src.a;

    return result;
}

float4 PSExclusion(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 src = float4(SrcDiffuseMap.Sample(SamplerLinearWrap, input.UV));

    float4 result;
    result.rgb = dest.rgb + src.rgb - 2 * dest.rgb * src.rgb;
    result.a = src.a;

    return result;
}

float4 PSMonochrome(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float Y     = dot(dest.rgb, float3(0.299, 0.587, 0.114));
    return float4(Y, Y, Y, 1);
}

float4 PSBrightExtract(VertexOut input) : SV_TARGET
{
    return float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
}

float4 PSColorKeyAlpha(VertexOut input) : SV_TARGET
{
    return float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
}

float4 PSColorTone(VertexOut input) : SV_TARGET
{
    return float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
}

float4 PSGrayScale(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float Y     = dot(dest.rgb, float3(0.2126, 0.7152, 0.0722));
    return float4(Y, Y, Y, 1);
}

float4 PSInvert(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 result = float4(1.0f - dest.rgb, dest.a);
    return result;
}

float4 PSMosaic(VertexOut input) : SV_TARGET
{
    return float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
}

float4 PSSepia(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float4 result;
    result.r = 0.393 * dest.r + 0.769 * dest.g + 0.189 * dest.b;
    result.g = 0.349 * dest.r + 0.666 * dest.g + 0.168 * dest.b;
    result.b = 0.272 * dest.r + 0.534 * dest.g + 0.131 * dest.b;
    result.a = dest.a;
    return result;
}

float4 PSBinary(VertexOut input) : SV_TARGET
{
    float4 dest = float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    float Y = dot(dest.rgb, float3(0.2126, 0.7152, 0.0722));
    Y = (Y < 128.0f / 255.0f) ? 0 : 1;
    float4 result = float4(Y, Y, Y, 1);
    return result;

}

float4 PSHDRMain(VertexOut input) : SV_TARGET
{
    float4 result;
    float3 rec2020  = Rec709ToRec2020(input.Color.rgb);
    float3 hdr10    = LinearToST2084(rec2020 * (80.0 / 10000.0));
    input.Color.rgb = hdr10;
    
    result = input.Color * float4(DestDiffuseMap.Sample(SamplerLinearWrap, input.UV));
    return result;
}
#endif