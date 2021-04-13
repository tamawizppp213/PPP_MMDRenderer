//////////////////////////////////////////////////////////////////////////////////
//              Title:  BasicShaderStruct.hlsli
//            Content:  Basic Vertex 
//             Author:  Toide Yutaro
//             Create:  2020_12_08
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_TEST_VERTEX_HLSLI
#define SHADER_TEST_VERTEX_HLSLI
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Core/ShaderBasicStruct.hlsli"
#include "Model/ShaderModelStruct.hlsli"
#include "ShaderTest.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define VertexIn  VSInputPositionNormalColorTexture
#define VertexOut VSOutputPositionNormalColorTexture
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
VertexOut VSMain(VertexIn vertexIn)
{
    VertexOut result;
    
    result.Position = vertexIn.Position;
    result.Position = mul(projectionViewMatrix, result.Position);
    result.UV       = vertexIn.UV;
    result.Normal   = vertexIn.Normal;
    result.Color    = vertexIn.Color;
    
    return result;
}

float4 PSMain(VertexOut input) : SV_TARGET
{
    return float4(input.UV, 1, 1);
}

VSOutputPMDModel VSModel(VSInputPMDModel input)
{
    VSOutputPMDModel result;
    result.Position = input.Position;
    result.Position = mul(projectionViewMatrix, input.Position);
    result.UV       = input.UV;
    result.Normal   = input.Normal;
    result.Normal.w = 0;
    return result;
}

#endif

