//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderBlur.hlsl
//            Content:  Draw Blur Effect (compute shader)
//             Author:  Toide Yutaro
//             Create:  2021_07_16
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_BLUR_HPP
#define SHADER_BLUR_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
cbuffer WeightTable : register(b0)
{
    float4 Weights[2];
}

cbuffer TextureSize : register(b1)
{
    int2 OriginalTexture;
    int2 XBlurTexture;
    int2 YBlurTexture; // y blur texture size
}

Texture2D           inputImage    : register(t0);
RWTexture2D<float4> outputImage   : register(u0);
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			ConvertFloat4ToRGBA32
*************************************************************************//**
*  @fn        uint ConvertFloat4ToRGBA32(float4 input)
*  @brief     Convert Float4 to RGBA32 format
*  @param[in] float4 input
*****************************************************************************/
uint ConvertFloat4ToRGBA32(float4 input)
{
    return (uint) (input.a * 255) << 24
          | (uint) (input.b * 255) << 16
          | (uint) (input.g * 255) << 8
          | (uint) (input.r * 255);
}

/****************************************************************************
*				  			ConvertRGBA32ToFloat4
*************************************************************************//**
*  @fn        float4 ConvertRGBA32ToFloat4(uint input)
*  @brief     Convert RGBA32 to float4 format
*  @param[in] uint input
*****************************************************************************/
float4 ConvertRGBA32ToFloat4(uint input)
{
    float4 fColor;
    fColor.a = (float) (input >> 24 & 0xFF);
    fColor.b = (float) ((input >> 16) & 0xFF);
    fColor.g = (float) ((input >> 8) & 0xFF);
    fColor.r = (float) (input & 0xFF);

    return fColor / 255.0f;
}

/****************************************************************************
*				  			GetPixelIndexFromXYCoordinate
*************************************************************************//**
*  @fn        uint GetPixelIndexFromXYCoordinate(int x, int y, int width)
*  @brief     Get Pixel index from xy coordinate
*  @param[in] int x
*  @param[in] int y
*  @param[in] int width
*****************************************************************************/
uint GetPixelIndexFromXYCoordinate(int x, int y, int width)
{
    return width * y + x;
}

/****************************************************************************
*				  			GetPixelColor
*************************************************************************//**
*  @fn        float4 GetPixelColor(int x, int y, int2 textureSize)
*  @brief     Convert RGBA32 to float4 format
*  @param[in] int x
*  @param[in] int y
*  @param[in] int2 textureSize
*****************************************************************************/
float4 GetPixelColor(int x, int y)
{
    return inputImage[int2(x,y)];
}

[numthreads(4, 4, 1)]
void XBlur(uint3 id : SV_DispatchThreadID)
{
    uint2 basePosition = uint2(id.x * 2 , id.y);
    float4 color = GetPixelColor(basePosition.x, basePosition.y) * Weights[0].x;
    color += GetPixelColor(basePosition.x + 1, basePosition.y) * Weights[0].y;
    color += GetPixelColor(basePosition.x + 2, basePosition.y) * Weights[0].z;
    color += GetPixelColor(basePosition.x + 3, basePosition.y) * Weights[0].w;
    color += GetPixelColor(basePosition.x + 4, basePosition.y) * Weights[1].x;
    color += GetPixelColor(basePosition.x + 5, basePosition.y) * Weights[1].y;
    color += GetPixelColor(basePosition.x + 6, basePosition.y) * Weights[1].z;
    color += GetPixelColor(basePosition.x + 7, basePosition.y) * Weights[1].w;
    
    color += GetPixelColor(basePosition.x - 1, basePosition.y) * Weights[0].y;
    color += GetPixelColor(basePosition.x - 2, basePosition.y) * Weights[0].z;
    color += GetPixelColor(basePosition.x - 3, basePosition.y) * Weights[0].w;
    color += GetPixelColor(basePosition.x - 4, basePosition.y) * Weights[1].x;
    color += GetPixelColor(basePosition.x - 5, basePosition.y) * Weights[1].y;
    color += GetPixelColor(basePosition.x - 6, basePosition.y) * Weights[1].z;
    color += GetPixelColor(basePosition.x - 7, basePosition.y) * Weights[1].w;
    
    outputImage[id.xy] = color;
    //outputImage[pixelIndex] = ConvertFloat4ToRGBA32(color);
}

[numthreads(4, 4, 1)]
void YBlur(uint3 id : SV_DispatchThreadID)
{
    uint2 basePosition = uint2(id.x, id.y * 2);
    float4 color = GetPixelColor(basePosition.x, basePosition.y) * Weights[0].x;
    color += GetPixelColor(basePosition.x, basePosition.y + 1) * Weights[0].y;
    color += GetPixelColor(basePosition.x, basePosition.y + 2) * Weights[0].z;
    color += GetPixelColor(basePosition.x, basePosition.y + 3) * Weights[0].w;
    color += GetPixelColor(basePosition.x, basePosition.y + 4) * Weights[1].x;
    color += GetPixelColor(basePosition.x, basePosition.y + 5) * Weights[1].y;
    color += GetPixelColor(basePosition.x, basePosition.y + 6) * Weights[1].z;
    color += GetPixelColor(basePosition.x, basePosition.y + 7) * Weights[1].w;
    
    color += GetPixelColor(basePosition.x, basePosition.y - 1) * Weights[0].y;
    color += GetPixelColor(basePosition.x, basePosition.y - 2) * Weights[0].z;
    color += GetPixelColor(basePosition.x, basePosition.y - 3) * Weights[0].w;
    color += GetPixelColor(basePosition.x, basePosition.y - 4) * Weights[1].x;
    color += GetPixelColor(basePosition.x, basePosition.y - 5) * Weights[1].y;
    color += GetPixelColor(basePosition.x, basePosition.y - 6) * Weights[1].z;
    color += GetPixelColor(basePosition.x, basePosition.y - 7) * Weights[1].w;
    
    outputImage[id.xy] = color;

    
}


[numthreads(4, 4, 1)]
void FinalBlur( uint3 id : SV_DispatchThreadID )
{
    uint2 basePosition = uint2(id.x / 2, id.y / 2);
    
    // bilinear filter
    float4 color = GetPixelColor(basePosition.x, basePosition.y );
    color += GetPixelColor(basePosition.x    , basePosition.y + 1);
    color += GetPixelColor(basePosition.x + 1, basePosition.y    );
    color += GetPixelColor(basePosition.x + 1, basePosition.y + 1);    
    color /= 4.0f;
    
    
    
    uint pixelIndex         = GetPixelIndexFromXYCoordinate(id.x, id.y, OriginalTexture.x);
    outputImage[id.xy] = color;
    //outputImage[pixelIndex] = ConvertFloat4ToRGBA32(color);
}

#endif