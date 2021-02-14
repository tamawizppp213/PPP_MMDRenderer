//////////////////////////////////////////////////////////////////////////////////
//              Title:  BasicShaderStruct.hlsli
//            Content:  Basic Vertex 
//             Author:  Toide Yutaro
//             Create:  2020_12_08
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ShaderBasicStruct.hlsli"
#include "ShaderTest.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define VertexIn  VSInputPositionNormalColorTexture
#define VertexOut VSOutputPositionNormalColorTexture
#define VertexColor VSInputPositionColor
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
float4 PSMain(VertexOut input) : SV_TARGET
{
    //return float4(0, 0, 0, 1);
    return float4(tex.Sample(smp,input.UV));
}

float4 PSColor(VertexColor input) : SV_TARGET
{
    return float4(input.Color);

}