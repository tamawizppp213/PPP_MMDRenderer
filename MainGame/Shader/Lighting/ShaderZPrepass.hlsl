//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderBlur.hlsl
//            Content:  Draw Blur Effect (compute shader)
//             Author:  Toide Yutaro
//             Create:  2021_07_16
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_ZPREPASS_HPP
#define SHADER_ZPREPASS_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../Model/ShaderModelStruct.hlsli"
#include "../Core/ShaderBasicStruct.hlsli"
#include "../Core/ShaderConstantBuffer3D.hlsli"
//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define PMXVertexIn VSInputPMXModel
#define PMXVertexOut VSOutputPMXModel
#define PrimitiveVertexIn VSInputPositionNormalTexture
struct PSIn
{
    float4 Position : SV_POSITION;
    float Depth : TEXCOORD0;
};

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
                   + pmxBones[vertexIn.BoneNo[1]] * (1.0f - vertexIn.Weight[0] );

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
    float4 positionBone  = mul(boneMatrix, vertexIn.Position);
    float4 positionWorld = mul(World, positionBone); //mul(World, float4(vertexIn.Position.xyz, 1.0f));
    result.Position      = mul(ProjectionView, positionWorld);
    
    
    /*-------------------------------------------------------------------
	-        Set normal vector  
	---------------------------------------------------------------------*/
    //result.Position = mul(Projection, result.Position);
    result.Depth = result.Position.z / result.Position.w; // normalize (0Å`1)
   // result.Depth = (result.Position.z - NearZ) / (FarZ - NearZ) * result.Position.w; // linear depth (no sensitivity)
    
    return result;
}

PSIn Primitive_VSMain(PrimitiveVertexIn vertexIn)
{
    PSIn result;
    
   
    /*-------------------------------------------------------------------
	-        transform to world space 
	---------------------------------------------------------------------*/
    float4 positionWorld = mul(World, vertexIn.Position); //mul(World, float4(vertexIn.Position.xyz, 1.0f));
    result.Position = mul(ProjectionView, positionWorld);
    
    
    /*-------------------------------------------------------------------
	-        Set normal vector  
	---------------------------------------------------------------------*/
    result.Depth = result.Position.z / result.Position.w; // normalize (0Å`1)
    
    return result;
}
float4 PMX_PSMain(PSIn psIn) : SV_TARGET
{
    return float4(psIn.Depth, psIn.Depth, psIn.Depth, 1.0f);
}

float4 Primitive_PSMain(PSIn psIn) : SV_TARGET
{
    return float4(psIn.Depth, psIn.Depth, psIn.Depth, 1.0f);
}
#endif