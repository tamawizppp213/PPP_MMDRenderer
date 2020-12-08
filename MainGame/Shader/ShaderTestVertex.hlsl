//////////////////////////////////////////////////////////////////////////////////
//              Title:  BasicShaderStruct.hlsli
//            Content:  Basic Vertex 
//             Author:  Toide Yutaro
//             Create:  2020_11_
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ShaderBasicStruct.hlsli"
#include "ShaderConstantBuffer.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define VertexIn  VSInputPositionTexture
#define VertexOut VSOutputPositionTexture
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
VertexOut VSMain(VertexIn vertexIn)
{
    VertexOut result;
    
    result.Position = vertexIn.Position;
    result.UV   = vertexIn.UV;
    
    return result;
}

float4 PSMain(VertexOut input) : SV_TARGET
{
    return float4(input.UV, 1, 1);
}


