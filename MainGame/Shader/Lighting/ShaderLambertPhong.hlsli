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
#include "../Lighting/ShaderLightType.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define EPSILON (0.0001f)
float3 LambertDiffuseReflection(const in float3 lightDirection, const in float3 lightColor, const in float3 normal);
float3 PhongSpecularReflection (const in float3 lightDirection, const in float3 lightColor, const in float3 normal, const in float3 toEye, float specularPower);
float3 ComputeDirectionLight(const in DirectionalLight directionalLight, const in float3 normal, const in float3 toEye, float specularPower = 1.0f);
float3 ComputePointLight    (const in PointLight       pointLight      , const in float3 normal, const in float3 toEye, const in float3 surfacePosition, float specularPower = 1.0f);
float3 ComputeSpotLight     (const in SpotLight        spotLight       , const in float3 normal, const in float3 toEye, const in float3 surfacePosition, float specularPower = 1.0f);
//////////////////////////////////////////////////////////////////////////////////
//                             Struct
//////////////////////////////////////////////////////////////////////////////////
float3 LambertDiffuseReflection(const in float3 lightDirection, const in float3 lightColor, const in float3 normal)
{
    float  t = max(0.0f, dot(normal, lightDirection) * -1.0f);
    return lightColor * t;
}

float3 PhongSpecularReflection(const in float3 lightDirection, const in float3 lightColor, const in float3 normal, const in float3 toEye, float specularPower)
{
    float3 reflection = normalize(reflect(lightDirection, toEye));
    float specular    = saturate(dot(reflection, normal));
    specular          = pow(specular, specularPower);
    return specular;
}

/****************************************************************************
*				  		ComputeDirectionalLight
*************************************************************************//**
*  @fn        float3 ComputeDirectionLight(const in DirectionalLight directionalLight, const in float3 normal, const in float3 toEye, float specularPower = 1.0f)
*  @brief     Compute directinal light
*  @param[in] DirectionalLight
*  @param[in] float3 normal 
*  @param[in] float3 toEye
*  @param[in] float  specularPower
*****************************************************************************/
float3 ComputeDirectionLight(const in DirectionalLight directionalLight, const in float3 normal, const in float3 toEye, float specularPower)
{
    float3 light = 0.0f;
    
    light += LambertDiffuseReflection(
    normalize(directionalLight.Direction),
    directionalLight.Color, normalize(normal));
    
    light += PhongSpecularReflection(
    normalize(directionalLight.Direction),
    directionalLight.Color, 
    normalize(normal), toEye, specularPower);
    return light;
}

/****************************************************************************
*				  		ComputePointLight
*************************************************************************//**
*  @fn        float3 ComputeDirectionLight(const in PointLight pointLight, const in float3 normal, const in float3 toEye, const in float3 surfacePosition, float specularPower = 1.0f)
*  @brief     Compute point light
*  @param[in] PointLight
*  @param[in] float3 normal 
*  @param[in] float3 toEye
*  @param[in] float3 surfacePosition
*  @param[in] float  specularPower
*****************************************************************************/
float3 ComputePointLight(const in PointLight pointLight, const in float3 normal, const in float3 toEye, const in float3 surfacePosition, float specularPower)
{
    float3 light = 0.0f;
    
    float3 toSurface = normalize(surfacePosition - pointLight.Position);
    float  distance  = length   (surfacePosition - pointLight.Position);
    
    float attenuate  = 1.0f - min(1.0f, distance / (pointLight.Range + EPSILON));
    attenuate        = attenuate * attenuate;
    
    light += LambertDiffuseReflection( toSurface, pointLight.Color, normal) * attenuate;
    light += PhongSpecularReflection( toSurface, pointLight.Color, normal,
                                      toEye, specularPower) * attenuate;
    return light;
}

float3 ComputeSpotLight(const in SpotLight spotLight, const in float3 normal, const in float3 toEye, const in float3 surfacePosition, float specularPower)
{
    float3 light = 0.0f;
    
    float3 toSurface = normalize(surfacePosition - spotLight.Position);
    float distance   = length   (surfacePosition - spotLight.Position);
     
    light += LambertDiffuseReflection(toSurface, spotLight.Color, normal);
    light += PhongSpecularReflection (toSurface, spotLight.Color, normal,
                                      toEye, specularPower);
    
    // distance attenuation
    float attenuate = 1.0f - min(1.0f, distance / (spotLight.Range + EPSILON));
    attenuate = attenuate * attenuate;
    light *= attenuate;
    
    // angle attenuation
    float angle = dot(normalize(toSurface), normalize(spotLight.Direction));
    angle       = abs(acos(angle));
    attenuate   = saturate(1.0f - min(max(angle - spotLight.InnerConeAngle, 0.0f), spotLight.OuterConeAngle - spotLight.InnerConeAngle) / (spotLight.OuterConeAngle - spotLight.InnerConeAngle));
    //attenuate   = pow(attenuate, 0.5f);
    light       *= attenuate;
    return light;
}

#endif