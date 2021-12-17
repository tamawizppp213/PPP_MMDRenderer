//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderLighting.hlsli
//            Content:  Lighting Strict
//             Author:  Toide Yutaro
//             Create:  
//////////////////////////////////////////////////////////////////////////////////
#ifndef SHADER_BRDF_HLSLI
#define SHADER_BRDF_HLSLI
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../Lighting/ShaderF0Config.hlsli"
#include "../Lighting/ShaderLightType.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define PI      3.141592653589f
struct BRDFSurface
{
    float3 Normal;
    float  Roughness;
    float3 Diffuse;
    float  Metalness;
    float3 Specular;
    float  EmissiveIntensity;
    float3 EmissiveColor;
    float  Padding;
};

//////////////////////////////////////////////////////////////////////////////////
//                             BRDF Implement
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			Attenuation_BRDF
*************************************************************************//**
*  @fn        float Attenuation_BRDF(float distance)
*  @brief     Attenuated by distance
*  @param[in] float distance
*****************************************************************************/
float Attenuation_BRDF(float distance)
{
    return 1.0f / (distance * distance);
}

/****************************************************************************
*				  		Controlled_Attenuation_BRDF
*************************************************************************//**
*  @fn        float Attenuation_BRDF(float distance)
*  @brief     Attenuated by distance (enable coefficients)
*  @param[in] float distance
*  @param[in] in const float3 coefficients
*****************************************************************************/
float Controlled_Attenuation_BRDF(float distance, in const float3 coefficients)
{
    return 1.0f / (coefficients[0] + coefficients[1] * distance + coefficients[2] * distance * distance);

}


/****************************************************************************
*				  			Calculate_F0
*************************************************************************//**
*  @fn        float3 Calculate_F0(float refractive_1, float refractive_2 = 1.0f)
*  @brief     Calculate F0
*  @param[in] float refractive index
*****************************************************************************/
float3 Calculate_F0(float refractive_1, float refractive_2 = 1.0f)
{
    const float3 refVec_1 = float3(refractive_1, refractive_1, refractive_1);
    const float3 refVec_2 = float3(refractive_2, refractive_2, refractive_2);
    const float3 F0 = refVec_1 - refVec_2;

    return F0;
}

/****************************************************************************
*				  			NDF_GGX
*************************************************************************//**
*  @fn        float NDF_GGX(float NormalDotHalf, float roughness)
*  @brief     Calculate Specular D (normal distribution fuction)
*  @param[in] float NormalDotHalf
*  @param[in] float roughness
*****************************************************************************/
float NDF_GGX(float NormalDotHalf, float roughness)
{
    float result = 0.0f;
    const float alpha  = roughness * roughness;
    const float alpha2 = alpha * alpha;
    const float NDotH2 = pow(NormalDotHalf, 2);
    const float denominator = (PI * pow((NDotH2 * (alpha2 - 1.0f) + 1.0f), 2));
    if (denominator < 0.000001f)
    {
        result = 1.0f;
    }
    else
    {
        result = alpha2 / denominator;
    }
    return result;
}

/****************************************************************************
*				  			Geometry_Smiths_Schlick_GGX
*************************************************************************//**
*  @fn        float Geometry_Smiths_Schlick_GGX(float3 normal, float3 toEye, float roughness)
*  @brief     Calculate Specular G
*  @param[in] float3 normal
*  @param[in] float3 toEye 
*  @param[in] float roughness
*****************************************************************************/
float Geometry_Smiths_Schlick_GGX(float3 normal, float3 toEye, float roughness)
{
    const float k           = pow((roughness + 1.0f), 2) / 8.0f;
    const float NDotToEye   = max(0.0f, dot(normal, toEye));
    const float denominator = (NDotToEye * (1.0f - k) + k) + 0.0001f;
    
    return NDotToEye / denominator;

}

/****************************************************************************
*				  	Geometry_Smiths_Schlick_GGX_EnvironmentMap
*************************************************************************//**
*  @fn        float Geometry_Smiths_Schlick_GGX_EnvironmentMap(float3 normal, float3 toEye, float roughness)
*  @brief     Calculate Specular G
*  @param[in] float3 normal
*  @param[in] float3 toEye 
*  @param[in] float roughness
*****************************************************************************/
float Geometry_Smiths_Schlick_GGX_EnvironmentMap(float3 normal, float3 toEye, float roughness)
{
    const float k           = pow(roughness, 2) / 2.0f;
    const float NDotToEye   = max(0.0f, dot(normal, toEye));
    const float denominator = (NDotToEye * (1.0f - k) + k) + 0.0001f;
    return NDotToEye / denominator;

}

/****************************************************************************
*				  		Masking_And_Shadowing_Function
*************************************************************************//**
*  @fn        float Masking_And_Shadowing_Function(float3 normal, float3 toEye, float3 light, float roughness)
*  @brief     Masking, shadowing fuction
*  @param[in] float3 F0 : ((n1 - n2) / (n1 + n2))^2 : n(refraction, default n2 = 1 (air))
*  @param[in] float3 normal 
*  @param[in] float3 lightVector
*****************************************************************************/
float Masking_And_Shadowing_Function(float3 normal, float3 toEye, float3 light, float roughness)
{
    return Geometry_Smiths_Schlick_GGX(normal, toEye, roughness)
         * Geometry_Smiths_Schlick_GGX(normal, light, roughness);

}

/****************************************************************************
*				  		Masking_Abd_Shadowing_Function_EnvironmentMap
*************************************************************************//**
*  @fn        float Masking_Abd_Shadowing_Function_EnvironmentMap(float3 normal, float toEye, float3 light, float roughness)
*  @brief     Masking, shadowing fuction
*  @param[in] float3 F0 : ((n1 - n2) / (n1 + n2))^2 : n(refraction, default n2 = 1 (air))
*  @param[in] float3 normal 
*  @param[in] float3 lightVector
*****************************************************************************/
float Masking_Abd_Shadowing_Function_EnvironmentMap(float3 normal, float toEye, float3 light, float roughness)
{
    return Geometry_Smiths_Schlick_GGX_EnvironmentMap(normal, toEye, roughness)
         * Geometry_Smiths_Schlick_GGX_EnvironmentMap(normal, light, roughness);
}
/****************************************************************************
*				  			Fresnel_Schlick
*************************************************************************//**
*  @fn        float3 Fresnel_Schlick(float3 F0, float3 normal, float3 lightVector)
*  @brief     Calcurate Reflectance
*  @param[in] float3 F0 : ((n1 - n2) / (n1 + n2))^2 : n(refraction, default n2 = 1 (air))
*  @param[in] float3 normal 
*  @param[in] float3 lightVector
*****************************************************************************/
float3 Fresnel_Schlick(float3 F0, float3 normal, float3 lightVector)
{
    const float  normalDotLight = saturate(dot(normal, lightVector));
    const float3 reflectance = F0 + (float3(1,1,1) - F0) * pow(1.0 - normalDotLight, 5.0f);
    return reflectance;
}

/****************************************************************************
*				  			Fresnel_Gaussian
*************************************************************************//**
*  @fn        float3 Fresnel_Gaussian(float3 F0, float3 H, float3 toEye)
*  @brief     Calcurate Fresnel Gaussian 
*  @param[in] float3 F0
*  @param[in] float3 H (half vector) 
*  @param[in] float3 toEye
*****************************************************************************/
float3 Fresnel_Gaussian(float3 F0, float3 H, float3 toEye)
{
    const float c0 = -5.55373f;
    const float c1 = -6.98316f;
    const float toEyeDotH = max(0.0f, dot(toEye, H));
    
    return F0 + (float3(1,1,1) - F0) * pow(2.0f, (c0 * toEyeDotH - c1) * toEyeDotH);

}

/****************************************************************************
*				  			Fresnel_With_Roughness
*************************************************************************//**
*  @fn        float3 Fresnel_With_Roughness(float3 F0, float cosTheta, float roughness)
*  @brief     Calcurate Fresnel With Roughness
*  @param[in] float3 F0
*  @param[in] float  cosTheta
*  @param[in] float  roughness
*****************************************************************************/
float3 Fresnel_With_Roughness(float3 F0, float cosTheta, float roughness)
{
    return F0 + (max((1.0f - roughness).xxx, F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


/****************************************************************************
*				  			Fresnel_Schlick_F90
*************************************************************************//**
*  @fn        float3 Fresnel_Schlick(float3 F0, float3 normal, float3 lightVector)
*  @brief     Calcurate Reflectance
*  @param[in] float3 F0 : ((n1 - n2) / (n1 + n2))^2 : n(refraction, default n2 = 1 (air))
*  @param[in] float3 F90
*  @param[in] float3 normal 
*  @param[in] float3 lightVector
*****************************************************************************/
float3 Fresnel_Schlick_F90(float3 F0, float3 F90, float3 normal, float3 lightVector)
{
    const float  normalDotLight = saturate(dot(normal, lightVector));
    const float3 reflectance    = F0 + (F90 - F0) * pow(1.0 - normalDotLight, 5.0f);
    return reflectance;
}

/****************************************************************************
*				  			Fresnel_LambertDiffuse
*************************************************************************//**
*  @fn        float3 Fresnel_LambertDiffuse(float kd)
*  @brief     Calcurate Lambert Diffuse
*  @param[in] float cDiffuse
*****************************************************************************/
float3 Fresnel_LambertDiffuse(float3 cDiffuse)
{
    return cDiffuse / PI;
}

float3 Bling_Phong_Model()
{
    
    float3 a;
    return a;
}

/****************************************************************************
*				  	Lambert_Cook_Torrence_Model
*************************************************************************//**
*  @class     float3 Lambert_Cook_Torrence_Model(in BRDFSurface surface, float3 light, float3 toEye)
*  @brief     Diffuse: Lambert Specular: Cook_Torrence_Model
*  @param[in] BRDFSurface surface
*  @param[in] float3 light
*  @param[in] float3 toEye
*****************************************************************************/
float3 Lambert_Cook_Torrence_Model(in BRDFSurface surface, float3 light, float3 toEye)
{
    /*-------------------------------------------------------------------
	-             variable define
	---------------------------------------------------------------------*/
    const float3 toEyeNormalize = normalize(toEye);
    const float3 normal         = normalize(surface.Normal);
    const float3 halfVector     = normalize(toEyeNormalize + surface.Normal);
    const float  normalDotHalf  = saturate(dot(normal, halfVector));
    const float  toEyeDotHalf   = saturate(dot(toEyeNormalize, halfVector));
    const float  normalDotToEye = saturate(dot(normal, toEyeNormalize));
    const float  normalDotLight = saturate(dot(normal, light));

    /*-------------------------------------------------------------------
	-             Surface 
	---------------------------------------------------------------------*/
    const float3 albedo    = surface.Diffuse;
    const float  roughness = surface.Roughness;
    const float  metalness = surface.Metalness;
    const float3 F0        = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metalness);
    
    /*-------------------------------------------------------------------
	-             Fresnel_Cook-Torrence (Specular)
	---------------------------------------------------------------------*/
    const float3 F = Fresnel_Schlick(F0, halfVector, toEyeNormalize);
    const float  G = Masking_And_Shadowing_Function(normal, toEyeNormalize, light, roughness);
    const float  D = NDF_GGX(normalDotHalf, roughness);
    
    const float  denominator = (4.0f * normalDotToEye * normalDotLight) + 0.0001f;
    const float3 specular    = D * F * G / denominator;
    const float3 Is          = specular * surface.Specular;
    
    /*-------------------------------------------------------------------
	-             Lambert 
	---------------------------------------------------------------------*/
    const float3 kSpecular = F;
    const float3 kDiffuse = (float3(1.0f, 1.0f, 1.0f) - kSpecular) * (1.0f - metalness) * albedo;
    const float3 Id        = Fresnel_LambertDiffuse(kDiffuse);
    
    return Is + Id;
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
    const float3 spotLightDirection = normalize(light.Direction);
#if 1
    const float theta = acos(dot(pixelDirectionInWorldSpace, spotLightDirection));
    return 1.0f - min(max(theta - light.InnerConeAngle, 0.0f), light.OuterConeAngle - light.InnerConeAngle) / (light.OuterConeAngle - light.InnerConeAngle);
#else
    
    return step(cos(light.OuterConeAngle), dot(spotLightDirection, pixelDirectionInWorldSpace));
#endif
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
    const float3 radiance       = SpotLight_Intensity(light, surfacePosition) * light.Color 
                                  * light.Brightness 
                                  * Attenuation_BRDF(length(light.Position - surfacePosition));
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