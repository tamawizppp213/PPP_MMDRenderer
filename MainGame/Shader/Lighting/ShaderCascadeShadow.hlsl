//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderCascadeShadow.hlsl
//            Content:  Cascade shadow pixel shader (compute shader)
//             Author:  Toide Yutaro
//             Create:  2021_07_16
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_CASCADE_SHADOW_HPP
#define SHADER_CASCADE_SHADOW_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../Model/ShaderModelStruct.hlsli"
#include "../Core/ShaderBasicStruct.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define PMXVertexIn VSInputPMXModel
#define PMXVertexOut VSOutputPMXModel
#define PrimitiveVertexIn VSInputPositionNormalTexture
struct PSIn
{
    float4 Position : SV_POSITION;
    
};

cbuffer ObjectConstants : register(b0)
{
    matrix World;
    int TextureConfig;
    int3 ObjectPadding;
}

cbuffer SceneConstants : register(b1)
{
    matrix ProjectionView;
}


//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
PSIn PMX_VSMain(PMXVertexIn vertexIn)
{
    PSIn result;
    
    /*-------------------------------------------------------------------
	-        Set bone matrix config
	---------------------------------------------------------------------*/
    matrix boneMatrix;
    if (vertexIn.WeightType == 0)
    {
        boneMatrix = pmxBones[vertexIn.BoneNo[0]];

    }
    else if (vertexIn.WeightType == 1)
    {
        boneMatrix = pmxBones[vertexIn.BoneNo[0]] * vertexIn.Weight[0]
                   + pmxBones[vertexIn.BoneNo[1]] * (1.0f - vertexIn.Weight[0]);

    }
    else if (vertexIn.WeightType == 2)
    {
        boneMatrix = pmxBones[vertexIn.BoneNo[0]] * vertexIn.Weight[0]
                   + pmxBones[vertexIn.BoneNo[1]] * vertexIn.Weight[1]
                   + pmxBones[vertexIn.BoneNo[2]] * vertexIn.Weight[2]
                   + pmxBones[vertexIn.BoneNo[3]] * vertexIn.Weight[3]; // bone weighting
    }
    else
    {
        boneMatrix = pmxBones[vertexIn.BoneNo[0]] * vertexIn.Weight[0]
                   + pmxBones[vertexIn.BoneNo[1]] * (1.0f - vertexIn.Weight[0]);
        
    }
    
   
    /*-------------------------------------------------------------------
	-        transform to world space 
	---------------------------------------------------------------------*/
    float4 positionBone = mul(boneMatrix, vertexIn.Position);
    float4 positionWorld = mul(World, positionBone); //mul(World, float4(vertexIn.Position.xyz, 1.0f));
    result.Position = mul(ProjectionView, positionWorld);
    
    return result;
}

PSIn Primitive_VSMain(PrimitiveVertexIn vertexIn)
{
    PSIn result;
    
    float4 positionWorld = mul(World, vertexIn.Position); //mul(World, float4(vertexIn.Position.xyz, 1.0f));
    result.Position = mul(ProjectionView, positionWorld);
    return result;
}
float4 PMX_PSMain(PSIn psIn) : SV_TARGET
{
    return float4(psIn.Position.z, psIn.Position.z, psIn.Position.z, 1.0f);
}

float4 Primitive_PSMain(PSIn psIn) : SV_TARGET
{
    return float4(psIn.Position.z, psIn.Position.z, psIn.Position.z, 1.0f);
}
#endif