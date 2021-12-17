//////////////////////////////////////////////////////////////////////////////////
//              Title:  BasicShaderStruct.hlsli
//            Content:  Basic Vertex 
//             Author:  Toide Yutaro
//             Create:  2020_12_08
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Core/ShaderBasicStruct.hlsli"
#include "Core/ShaderStaticSampler.hlsli"
#include "Model/ShaderModelStruct.hlsli"
#include "ShaderTest.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define VertexIn    VSInputPositionNormalColorTexture
#define VertexOut   VSOutputPositionNormalColorTexture
#define VertexColor VSInputPositionColor
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
float4 PSMain(VertexOut input) : SV_TARGET
{
    //return float4(0, 0, 0, 1);
    float4 result = input.Color * float4(tex.Sample(SamplerLinearWrap, input.UV));
    result.a = saturate(result.a);
    return result;
}

float4 PSColor(VertexColor input) : SV_TARGET
{
    return float4(input.Color);

}

float4 PSModel(VSOutputPMDModel input) : SV_TARGET
{
    float3 light     = normalize(float3(1, -1, 1));
    float brightness = dot(-light, input.Normal.xyz);
    return float4(brightness, brightness, brightness, 1);
}