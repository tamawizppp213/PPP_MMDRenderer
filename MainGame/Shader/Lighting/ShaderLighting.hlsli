//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderLighting.hlsli
//            Content:  Lighting Strict
//             Author:  Toide Yutaro
//             Create:  
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_LIGHTING_HLSLI
#define SHADER_LIGHTING_HLSLI
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ShaderBRDF.hlsli"
#include "ShaderLightType.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define NUM_POINT_LIGHTS           20
#define NUM_SPOT_LIGHTS            5
#define NUM_SHADOWING_POINT_LIGHTS 5
#define NUM_SHADOWING_SPOT_LIGHTS  5

inline int HasDiffuseMap                    (int textureConfig) { return ((textureConfig & (1 << 0)) > 0 ? 1 : 0); }
inline int HasNormalMap                     (int textureConfig) { return ((textureConfig & (1 << 1)) > 0 ? 1 : 0); }
inline int HasAmbientOcclusionMap           (int textureConfig) { return ((textureConfig & (1 << 2)) > 0 ? 1 : 0); }
inline int HasAlphaMap                      (int textureConfig) { return ((textureConfig & (1 << 3)) > 0 ? 1 : 0); }
inline int HasRoughnessMap                  (int textureConfig) { return ((textureConfig & (1 << 4)) > 0 ? 1 : 0); }
inline int HasMetallicMap                   (int textureConfig) { return ((textureConfig & (1 << 5)) > 0 ? 1 : 0); }
inline int HasHeightMap                     (int textureConfig) { return ((textureConfig & (1 << 6)) > 0 ? 1 : 0); }
inline int HasEmissiveMap                   (int textureConfig) { return ((textureConfig & (1 << 7)) > 0 ? 1 : 0); }
inline int HasOcclusionRoughnessMetalnessMap(int textureConfig) { return ((textureConfig & (1 << 8)) > 0 ? 1 : 0); }

//////////////////////////////////////////////////////////////////////////////////
//                             Struct
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  		SpotLight_Intensity
*************************************************************************//**
*  @class     float SpotLight_Intensity(in SpotLight light, in const float3 worldPosition)
*  @brief     Calculate SpotLight Intensity
*  @param[in] in SpotLight light
*  @param[in] in const float3 worldPosition
*****************************************************************************/
float SpotLight_Intensity(in SpotLight light, in const float3 worldPosition)
{
    const float3 pixelDirectionInWorldSpace = normalize(worldPosition - light.Position);
    const float3 spotLightDirection         = normalize(light.Direction);
#if 1
    const float theta = acos(dot(pixelDirectionInWorldSpace, spotLightDirection));
    return 1.0f - min(max(theta - light.InnerConeAngle, 0.0f), light.OuterConeAngle - light.InnerConeAngle) / (light.OuterConeAngle - light.InnerConeAngle);
#else
    
    return step(cos(light.OuterConeAngle), dot(spotLightDirection, pixelDirectionInWorldSpace));
#endif
}

/****************************************************************************
*				  		Calculate_Point_Light_Illumination
*************************************************************************//**
*  @class     float3 Calculate_Point_Light_Illumination(in const PointLight light, in BRDFSurface surface, const in float3 surfacePosition, const in float3 toEye )
*  @brief     Calculate point light illumination
*  @param[in] in const PointLight light
*  @param[in] in BRDFSurface surface
*  @param[in] const in float3 surfacePosition 
*  @param[in] const in float3 toEye
*****************************************************************************/
float3 Calculate_Point_Light_Illumination(in const PointLight light, in BRDFSurface surface, const in float3 surfacePosition, const in float3 toEye )
{
    float3 IdIs = 0.0f.xxx;
    
    const float3 lightPostion   = light.Position;
    const float3 lightVector    = normalize(light.Position - surfacePosition);
    const float  distance       = length   (light.Position - surfacePosition);
    const float  normalDotLight = saturate (dot(surface.Normal, lightVector));
    const float3 radiance       = Attenuation_BRDF(distance) * light.Color * light.Brightness;
    
    IdIs += step(distance, light.Range) * Lambert_Cook_Torrence_Model(surface, lightVector, toEye) * radiance * normalDotLight;
  
    return IdIs;
}

/****************************************************************************
*				  		Calculate_Spot_Light_Illumination
*************************************************************************//**
*  @class     float3 Calculate_Spot_Light_Illumination(in const SpotLight light, in BRDFSurface surface, const in float3 surfacePosition, const in float3 toEye)
*  @brief     Calculate spot light illumination
*  @param[in] in const SpotLight light
*  @param[in] in BRDFSurface surface
*  @param[in] const in float3 surfacePosition 
*  @param[in] const in float3 toEye
*****************************************************************************/
float3 Calculate_Spot_Light_Illumination(in const SpotLight light, in BRDFSurface surface, const in float3 surfacePosition, const in float3 toEye)
{
    float3 IdIs = 0.0f.xxx;
    
    const float3 lightVector    = normalize(light.Position - surfacePosition);
    const float3 radiance       = SpotLight_Intensity(light, surfacePosition) * light.Color * light.Brightness * Attenuation_BRDF(length(light.Position - surfacePosition));
    const float  normalDotLight = saturate(dot(surface.Normal, lightVector));
    IdIs += Lambert_Cook_Torrence_Model(surface, lightVector, toEye) * radiance * normalDotLight;
    return IdIs;
}

/****************************************************************************
*				  		Calculate_Directional_Light_Illumination
*************************************************************************//**
*  @class     float3 Calculate_Directional_Light_Illumination(in const PointLight light, in BRDFSurface surface, const in float3 surfacePosition, const in float3 toEye )
*  @brief     Calculate directional light illumination
*  @param[in] in const Directional;Light light
*  @param[in] in BRDFSurface surface
*  @param[in] const in float3 surfacePosition 
*  @param[in] const in float3 toEye
*****************************************************************************/
float3 Calculate_Directional_Light_Illumination(in const DirectionalLight light, in BRDFSurface surface, const in float3 toEye)
{
    const float3 lightVector   = normalize(-light.Direction);
    const float3 radiance      = light.Color * light.Brightness;
    const float normalDotLight = saturate(dot(surface.Normal, lightVector));
    return Lambert_Cook_Torrence_Model(surface, lightVector, toEye) * radiance * normalDotLight;
}


float3 Calculate_EnvironmentMap_Illumination()
{
    float3 a;
    return a;
}
#endif