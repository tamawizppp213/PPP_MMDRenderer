#ifndef SHADER_ZPREPASS_HPP
#define SHADER_ZPREPASS_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../Model/ShaderModelStruct.hlsli"
#include "../Core/ShaderBasicStruct.hlsli"
#include "../Core/ShaderConstantBuffer3D.hlsli"
#include "../Core/ShaderStaticSampler.hlsli"
//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define PMXVertexIn VSInputPMXModel
#define PMXVertexOut VSOutputPMXModel
#define PrimitiveVertexIn VSInputPositionNormalTexture


struct PSIn
{
    float4 Position   : SV_POSITION;
    float4 Normal     : NORMAL0;
    float4 ViewNormal : NORMAL1;
    float4 Tangent    : TANGENT;
    float2 UV         : TEXCOORD;
    float4 WorldPosition : TEXCOORD1;
};

struct PSPrimitiveIn
{
    float4 Position   : SV_POSITION;
    float4 Normal     : NORMAL0;
    float4 ViewNormal : NORMAL1;
    float2 UV         : TEXCOORD;
    float4 WorldPosition : TEXCOORD1;
};

struct PSOut
{
    float4 Albedo            : SV_Target0;
    float4 Normal            : SV_Target1;
    float4 WorldPosition     : SV_Target2;
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
    
    
    /*-------------------------------------------------------------------
	-        Set normal vector  
	---------------------------------------------------------------------*/
    result.Position      = mul(ProjectionView, positionWorld);
    result.WorldPosition = positionWorld;
    vertexIn.Normal.w    = 0;
    vertexIn.Tangent.w   = 0;
    vertexIn.BiNormal.w  = 0;
    result.Normal        = mul(World, vertexIn.Normal);
    result.ViewNormal    = mul(View, result.Normal);
    result.UV            = vertexIn.UV;
    result.Tangent       = mul(World, vertexIn.Tangent);
    return result;
}

PSOut PMX_PSMain(PSIn input)
{
    PSOut result;
    
    /*-------------------------------------------------------------------
	-        Set Texture Config  
	---------------------------------------------------------------------*/
    float4 textureColor    = DiffuseMap.Sample(SamplerLinearWrap, input.UV); // Diffuse Color
    float2 sphereMapUV     = input.ViewNormal.xy; // Sphere Map
    sphereMapUV            = (sphereMapUV + float2(1, -1)) * float2(0.5, -0.5);
    float4 textureMultiply = SphereMapMultiply.Sample(SamplerLinearClamp, sphereMapUV);
    float4 textureAddition = SphereMapAddition.Sample(SamplerLinearClamp, sphereMapUV) * textureColor;
   // float4 toonDiffuse  = ToonTextureDiffuse.Sample(SamplerLinearClamp, float2(0, 1.0 - brightness)); // Up and down reversal
    
    /*-------------------------------------------------------------------
	-        Set Result 
	---------------------------------------------------------------------*/
    result.WorldPosition = input.WorldPosition;
    result.Albedo.xyz    = saturate(textureMultiply.xyz * textureColor.xyz + textureAddition.xyz).xyz;
    result.Albedo.w = textureColor.a;
    result.Normal.w = 1.0f;
    result.Normal   = (input.Normal * 0.5f) + 0.5f;

    return result;
}

PSPrimitiveIn Primitive_VSMain(PrimitiveVertexIn vertexIn)
{
    PSPrimitiveIn result;
    
    /*-------------------------------------------------------------------
	-        transform to world space 
	---------------------------------------------------------------------*/
    float4 positionWorld = mul(World, vertexIn.Position); //mul(World, float4(vertexIn.Position.xyz, 1.0f));
    
    
    /*-------------------------------------------------------------------
	-        Set normal vector  
	---------------------------------------------------------------------*/
    result.Position      = mul(ProjectionView, positionWorld);
    result.WorldPosition = positionWorld;
    vertexIn.Normal.w    = 0;
    result.Normal        = mul(World, vertexIn.Normal);
    result.ViewNormal    = mul(View, result.Normal);
    result.UV = vertexIn.UV;

    return result;
}

PSOut Primitive_PSMain(PSPrimitiveIn input)
{
    PSOut result;
    
    /*-------------------------------------------------------------------
	-        Set Texture Config  
	---------------------------------------------------------------------*/
    float4 textureColor = DiffuseMap.Sample(SamplerLinearWrap, input.UV); // Diffuse Color
   
   // float4 toonDiffuse  = ToonTextureDiffuse.Sample(SamplerLinearClamp, float2(0, 1.0 - brightness)); // Up and down reversal
    
    /*-------------------------------------------------------------------
	-        Set Result 
	---------------------------------------------------------------------*/
    result.WorldPosition = input.WorldPosition;
    result.Albedo        = textureColor * Diffuse;
    result.Normal.w      = 1.0f;
    result.Normal        = (input.Normal * 0.5f) + 0.5f;

    return result;
}
#endif