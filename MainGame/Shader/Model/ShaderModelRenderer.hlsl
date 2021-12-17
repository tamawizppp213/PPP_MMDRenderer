//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderModelRenderer.hlsli
//            Content:  ModelRenderer
//             Author:  Toide Yutaro
//             Create:  2020_03_21
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_MODEL_RENDERER_HLSL
#define SHADER_MODEL_RENDERER_HLSL
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ShaderModelStruct.hlsli"
#include "ShaderModelMaterial.hlsli"
#include "../Core/ShaderBasicStruct.hlsli"
#include "../Core/ShaderConstantBuffer3D.hlsli"
#include "../Core/ShaderStaticSampler.hlsli"
#include "../Core/ShaderMath.hlsli"
#include "../Lighting/ShaderLambertPhong.hlsli"
#include "../Lighting/ShaderBRDF.hlsli"
#include "../Core/ShaderColor.hlsli"
//#include "../Core/ShaderTexture.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define PMDVertexIn  VSInputPMDModel
#define PMDVertexOut VSOutputPMDModel
#define PMXVertexIn  VSInputPMXModel
#define PMXVertexOut VSOutputPMXModel
#define PrimitiveVertexIn  VSInputPositionNormalTexture
#define PrimitiveVertexOut VSOutputPrimitiveModel
#define TILE_WIDTH (16)
#define TILE_HEIGHT (16)

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
float3 ComputeDirectionalLights(PMXVertexOut vertex);
PMDVertexOut PMD_VSMain(PMDVertexIn vertexIn);
float4       PMD_PSMain(PMDVertexOut input);
PMXVertexOut PMX_VSMain(PMXVertexIn vertexIn);
float4       PMX_PSMain(PMXVertexOut input) : SV_TARGET;
PrimitiveVertexOut Primitive_VSMain(PrimitiveVertexIn vertexIn);
float4 Primitive_PSMain(PrimitiveVertexOut input) : SV_TARGET;
//////////////////////////////////////////////////////////////////////////////////
//                             PMD File
//////////////////////////////////////////////////////////////////////////////////
PMDVertexOut PMD_VSMain(PMDVertexIn vertexIn)
{
    PMDVertexOut result;
    
    /*-------------------------------------------------------------------
	-        Set bone matrix config
	---------------------------------------------------------------------*/
    float weight      = (float) vertexIn.Weight / 100.0f; // normalize
    matrix boneMatrix = bones[vertexIn.BoneNo[0]] * weight 
                      + bones[vertexIn.BoneNo[1]] * (1.0f - weight); // bone weighting
   
    /*-------------------------------------------------------------------
	-        transform to world space 
	---------------------------------------------------------------------*/
    float4 positionBone  = mul(boneMatrix, vertexIn.Position);
    float4 positionWorld = mul(World,  positionBone); //mul(World, float4(vertexIn.Position.xyz, 1.0f));
    result.Position      = mul(ProjectionView, positionWorld);
    
    /*-------------------------------------------------------------------
	-        Set normal vector  
	---------------------------------------------------------------------*/
    vertexIn.Normal.w = 0;
    result.Normal     = mul(World, vertexIn.Normal);

    result.ViewNormal = mul(View, result.Normal);
    result.UV         = vertexIn.UV;
    
    return result;
}

float4 PMD_PSMain(PMDVertexOut input) : SV_TARGET
{
    // temp directional light 
    
    float3 light        = normalize(float3(1, -1, 1));
    float4 lightColor   = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float brightness    = saturate(dot(-light, input.Normal.xyz));

    float3 ray         = normalize(input.Position.xyz - EyePosition);
    float3 reflection  = normalize(reflect(light, input.Normal.xyz));
    float specular     = pow(saturate(dot(reflection, -ray)), pmdMaterial.SpecularPower);
    
    /*-------------------------------------------------------------------
	-        Set Texture Config  
	---------------------------------------------------------------------*/
    float4 textureColor = DiffuseMap.Sample(SamplerLinearWrap, input.UV);    // Diffuse Color
    float2 sphereMapUV  = input.ViewNormal.xy;                               // Sphere Map
    sphereMapUV         = (sphereMapUV + float2(1, -1)) * float2(0.5, -0.5);
    float4 toonDiffuse = ToonTextureDiffuse.Sample(SamplerLinearClamp, float2(0, 1.0 - brightness)); // Up and down reversal
    
    /*-------------------------------------------------------------------
	-        Return Pixel Color
	---------------------------------------------------------------------*/
    return 
    max(saturate(toonDiffuse
    * lightColor
    * pmdMaterial.Diffuse
    * textureColor
    * SphereMapMultiply.Sample(SamplerLinearWrap, sphereMapUV))
    + saturate(SphereMapAddition.Sample(SamplerLinearWrap, sphereMapUV) * textureColor
    + float4(specular * pmdMaterial.Specular.rgb, 1))
    , float4(textureColor.xyz * pmdMaterial.Ambient, 1));

}


//////////////////////////////////////////////////////////////////////////////////
//                             PMX File
//////////////////////////////////////////////////////////////////////////////////
PMXVertexOut PMX_VSMain(PMXVertexIn vertexIn)
{
    PMXVertexOut result;
    
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
    result.WorldPosition = positionWorld;
    
    /*-------------------------------------------------------------------
	-        Set normal vector  
	---------------------------------------------------------------------*/
    vertexIn.Normal.w   = 0;
    vertexIn.Tangent.w  = 0;
    vertexIn.BiNormal.w = 0;
    result.Normal     = mul(World, vertexIn.Normal);
    result.ViewNormal = mul(View, result.Normal);
    result.UV         = vertexIn.UV;
    result.Tangent    = mul(World, vertexIn.Tangent);
    result.BiNormal   = mul(World, vertexIn.BiNormal);
    return result;
}

float4 PMX_PSMain(PMXVertexOut input) : SV_TARGET
{
    float3 toEye = normalize(EyePosition - input.WorldPosition.xyz);
    
    /*-------------------------------------------------------------------
	-        Set Tile Config  
	---------------------------------------------------------------------*/
    float2 viewportPosition = input.Position.xy;
    uint numCellX  = (RenderTargetSize.x + (TILE_WIDTH - 1)) / TILE_WIDTH;
    uint tileIndex = floor(viewportPosition.x / TILE_WIDTH) + floor(viewportPosition.y / TILE_HEIGHT) * numCellX;
    uint pointLightStart = tileIndex       * PointLightNum;
    uint pointLightEnd   = pointLightStart + PointLightNum;
    uint spotLightStart  = tileIndex       * SpotLightNum;
    uint spotLightEnd    = spotLightStart  + SpotLightNum;
    
    /*-------------------------------------------------------------------
	-        Set Texture Config  
	---------------------------------------------------------------------*/
    float4 textureColor = DiffuseMap.Sample(SamplerLinearClamp, input.UV) * Diffuse; // Diffuse Color
    float4 ambientColor = AmbientMap.Sample(SamplerLinearClamp, float2(viewportPosition / RenderTargetSize));
    float2 sphereMapUV  = input.ViewNormal.xy; // Sphere Map
    sphereMapUV         = (sphereMapUV + float2(1, -1)) * float2(0.5, -0.5);
    float4 textureMultiply = SphereMapMultiply.Sample(SamplerLinearClamp, sphereMapUV);
    float4 textureAddition = saturate(SphereMapAddition.Sample(SamplerLinearClamp, sphereMapUV) * textureColor);
    textureColor           = saturate(textureMultiply * textureColor + textureAddition);
    
    /*-------------------------------------------------------------------
	-        Set BRDF Surface Config  
	---------------------------------------------------------------------*/
    BRDFSurface surface;
    surface.Diffuse           = textureColor.rgb;
    surface.Specular          = textureColor.rgb;
    surface.EmissiveColor     = EmissiveColor;
    surface.EmissiveIntensity = EmissiveIntensity;
    surface.Roughness         = Roughness;
    surface.Metalness         = Metalness;
    surface.Padding           = 0.0;
    surface.Normal            = input.Normal.xyz;
    
    float3 result = surface.EmissiveColor * surface.EmissiveIntensity / 100.0f;
    for (int i = 0; i < NUM_DIRECTIONAL_LIGHT; ++i)
    {
        result += Calculate_Directional_Light_Illumination(DirectionalLights[i], surface, toEye);
    }
    
    for (int pointLight = (int) pointLightStart; pointLight < (int) pointLightEnd; ++pointLight)
    {
        uint pointIndex = PointLightList[pointLight];
        if (pointIndex == 0xffffffff)
        {
            break;
        }
        result += Calculate_Point_Light_Illumination(PointLights[pointIndex], surface, float3(input.WorldPosition.xyz), toEye);
    }
    
    for (int spotLight = (int) spotLightStart; spotLight < (int) spotLightEnd; ++spotLight)
    {
        uint spotIndex = SpotLightList[spotLight];
        if (spotIndex == 0xffffffff)
        {
            break;
        }
        
        result += Calculate_Spot_Light_Illumination(SpotLights[spotIndex], surface, float3(input.WorldPosition.xyz), toEye);
    }
    
    if (TextureConfig == 1)
    {
        result += float4(textureColor * float4(Ambient, 1.0f) * float4(ambientColor.xyz, 1.0f)).xyz;
    }
    else
    {
        result += float4(textureColor * float4(Ambient, 1.0f)).xyz;

    }
    /*-------------------------------------------------------------------
	-        Return Pixel Color
	---------------------------------------------------------------------*/
    result.rgb     = SRGBToLinear(result.rgb);
    float4 finalPixel = float4(result.xyz, saturate(textureColor.a));
    return finalPixel;
}

//////////////////////////////////////////////////////////////////////////////////
//                             Primitive Model
//////////////////////////////////////////////////////////////////////////////////
PrimitiveVertexOut Primitive_VSMain(PrimitiveVertexIn vertexIn)
{
    PrimitiveVertexOut result;
    
    /*-------------------------------------------------------------------
	-        transform to world space 
	---------------------------------------------------------------------*/
    float4 positionWorld = mul(World, vertexIn.Position); //mul(World, float4(vertexIn.Position.xyz, 1.0f));
    result.Position      = mul(ProjectionView, positionWorld);
    result.WorldPosition = positionWorld;
    
    /*-------------------------------------------------------------------
	-        Set normal vector  
	---------------------------------------------------------------------*/
    vertexIn.Normal.w = 0;
    result.Normal     = mul(World, vertexIn.Normal);
    result.ViewNormal = mul(View , result.Normal);
    result.UV         = vertexIn.UV;

    return result;
}

float4 Primitive_PSMain(PrimitiveVertexOut input) : SV_TARGET
{
    float3 toEye = normalize(EyePosition - input.WorldPosition.xyz);
    
    /*-------------------------------------------------------------------
	-        Set Tile Config  
	---------------------------------------------------------------------*/
    float2 viewportPosition = input.Position.xy;
    uint numCellX  = (RenderTargetSize.x + (TILE_WIDTH - 1)) / TILE_WIDTH;
    uint tileIndex = floor(viewportPosition.x / TILE_WIDTH) + floor(viewportPosition.y / TILE_HEIGHT) * numCellX;
    uint pointLightStart = tileIndex       * PointLightNum;
    uint pointLightEnd   = pointLightStart + PointLightNum;
    uint spotLightStart  = tileIndex       * SpotLightNum;
    uint spotLightEnd    = spotLightStart  + SpotLightNum;
    
    /*-------------------------------------------------------------------
	-        Set Texture Config  
	---------------------------------------------------------------------*/
    float4 textureColor = DiffuseMap.Sample(SamplerLinearWrap, input.UV); // Diffuse Color
    float4 ambientColor = AmbientMap.Sample(SamplerLinearWrap, float2(viewportPosition / RenderTargetSize));

    /*-------------------------------------------------------------------
	-        Set BRDF Surface Config  
	---------------------------------------------------------------------*/
    BRDFSurface surface;
    surface.Diffuse           = textureColor.rgb * Diffuse.rgb;
    surface.Specular          = textureColor.rgb * Specular.rgb;
    surface.EmissiveColor     = EmissiveColor;
    surface.EmissiveIntensity = EmissiveIntensity;
    surface.Roughness         = Roughness;
    surface.Metalness         = Metalness;
    surface.Padding           = 0.0f;
    surface.Normal            = input.Normal.xyz;
    
    float3 result = surface.EmissiveColor * surface.EmissiveIntensity / 100.0f;
    for (int i = 0; i < NUM_DIRECTIONAL_LIGHT; ++i)
    {
        result += Calculate_Directional_Light_Illumination(DirectionalLights[i], surface, toEye);
    }
    
    for (int pointLight = (int) pointLightStart; pointLight < (int) pointLightEnd; ++pointLight)
    {
        uint pointIndex = PointLightList[pointLight];
        if (pointIndex == 0xffffffff)
        {
            break;
        }
        result += Calculate_Point_Light_Illumination(PointLights[pointIndex], surface, float3(input.WorldPosition.xyz), toEye);
    }
    
    for (int spotLight = (int) spotLightStart; spotLight < (int) spotLightEnd; ++spotLight)
    {
        uint spotIndex = SpotLightList[spotLight];
        if (spotIndex == 0xffffffff)
        {
            break;
        }
        
        result += Calculate_Spot_Light_Illumination(SpotLights[spotIndex], surface, float3(input.WorldPosition.xyz), toEye);
    }
    
    if (TextureConfig == 1)
    {
        result += float4(textureColor * float4(Ambient, textureColor.a) * float4(ambientColor.xyz, 1.0f)).xyz;
    }
    else
    {
        result += float4(textureColor * float4(Ambient, textureColor.a)).xyz;

    }
    /*-------------------------------------------------------------------
	-        Return Pixel Color
	---------------------------------------------------------------------*/
    return (float4(result, saturate(textureColor.a)));
}

float3 ComputeDirectionalLights(PMXVertexOut vertex)
{
    float3 light = 0.0f;
    float3 toEye = normalize(EyePosition - vertex.Position.xyz);
    for (int i = 0; i < NUM_DIRECTIONAL_LIGHT; ++i)
    {
        light += ComputeDirectionLight(DirectionalLights[i], vertex.Normal.xyz, toEye, pmxMaterial.SpecularPower);
    }
    
    return light;
}

//float4 PMX_PSMain(PMXVertexOut input) : SV_TARGET
//{
//    float3 light     = ComputeDirectionalLights(input);
//    //float brightness = saturate(dot(-light, input.Normal.xyz));
//    //float3 light = normalize(float3(1,-1,1));
//    float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
//    float brightness  = saturate(dot(-light, input.Normal.xyz));

//    /*-------------------------------------------------------------------
//	-        Set Texture Config  
//	---------------------------------------------------------------------*/
//    float4 textureColor = DiffuseMap.Sample(SamplerLinearWrap, input.UV); // Diffuse Color
//    float2 sphereMapUV  = input.ViewNormal.xy; // Sphere Map
//    sphereMapUV         = (sphereMapUV + float2(1, -1)) * float2(0.5, -0.5);
//    float4 toonDiffuse  = ToonTextureDiffuse.Sample(SamplerLinearClamp, float2(0, 1.0 - brightness)); // Up and down reversal

    
//    /*-------------------------------------------------------------------
//	-        Return Pixel Color
//	---------------------------------------------------------------------*/
//    return
//    max(saturate(
//    toonDiffuse
//    // float4(DirectionalLights[0].Color.xyz, 1.0f)
//    * pmxMaterial.Diffuse // ‚¢‚ç‚È‚¢
//    //* float4(light, 1)
//    * textureColor
//    * SphereMapMultiply.Sample(SamplerLinearClamp, sphereMapUV))
//    + saturate(SphereMapAddition.Sample(SamplerLinearClamp, sphereMapUV) * textureColor)
//    , float4(textureColor * float4(pmxMaterial.Ambient, 1.0f)));
//}
#endif