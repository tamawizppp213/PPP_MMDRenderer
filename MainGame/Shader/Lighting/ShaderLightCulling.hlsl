//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderLightCulling.hlsl
//            Content:  Shader light culling (compute shader)
//             Author:  Toide Yutaro 
//             Create:  2021_08_06
//////////////////////////////////////////////////////////////////////////////////
// reference: https://github.com/shoeisha-books/hlsl-grimoire-sample/blob/main/Sample_16_04/Sample_16_04/Assets/shader/lightCulling.fx
// reference: https://mobile.sites.google.com/site/monshonosuana/directx%E3%81%AE%E8%A9%B1/directx%E3%81%AE%E8%A9%B1-%E7%AC%AC125%E5%9B%9E

#ifndef SHADER_LIGHT_CULLING_HPP
#define SHADER_LIGHT_CULLING_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ShaderLightType.hlsli"
#include "../Core/ShaderConstantBuffer3D.hlsli"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define TILE_WIDTH  16
#define TILE_HEIGHT 16
#define TILE_SIZE (TILE_WIDTH * TILE_HEIGHT)

Texture2D<float>         DepthTexture     : register(t0);
RWStructuredBuffer<uint> LightIndices     : register(u0);
RWStructuredBuffer<uint> SpotLightIndices : register(u1);

groupshared uint MinDepth;
groupshared uint MaxDepth;
groupshared uint TileLightIndices[NUM_POINT_LIGHT];
groupshared uint TileSpotLightIndices[NUM_SPOT_LIGHT];
groupshared uint TileNumLights;
groupshared uint TileNumSpotLights;
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  		GetTileFrustrumPlane
*************************************************************************//**
*  @class     void GetTileFrustrumPlane(out float4 frustrumPlanes[6], uint3 groupID)
*  @brief     Get Frustrum plane of the each tile 
*  @param[out] float4 [6] frustrum planes 
*  @param[in] uint3 groupID
*****************************************************************************/
void GetTileFrustrumPlane(out float4 frustrumPlanes[6], uint3 groupID)
{
    float minTileZ = asfloat(MinDepth);
    float maxTileZ = asfloat(MaxDepth);
    
    float2 tileScale = RenderTargetSize * rcp(float(2 * TILE_WIDTH)); // rcp (1/(2 * width))
    float2 tileBias = tileScale - float2(groupID.xy);

    float4 c1 = float4(Projection._11 * tileScale.x, 0.0, tileBias.x, 0.0);
    float4 c2 = float4(0.0f, -Projection._22 * tileScale.y, tileBias.y, 0.0);
    float4 c4 = float4(0.0f, 0.0f, 1.0f, 0.0f);

    frustrumPlanes[0] = c4 - c1; // right
    frustrumPlanes[1] = c1;      // left
    frustrumPlanes[2] = c4 - c2; // top
    frustrumPlanes[3] = c2;      // bottom
    frustrumPlanes[4] = float4(0.0, 0.0, 1.0, -minTileZ);
    frustrumPlanes[5] = float4(0.0, 0.0, -1.0, maxTileZ);
    
    // normalize
    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        frustrumPlanes[i] *= rcp(length(frustrumPlanes[i].xyz));
    }

}

/****************************************************************************
*				  		ComputePositionInCamera
*************************************************************************//**
*  @class     void ComputePositionInCamera(uint2 globalCoords)
*  @brief     Calculate position in camera space.
*  @param[in] uint2 global Coords
*****************************************************************************/
float3 ComputePositionInCamera(uint2 globalCoords)
{
    float2 st = ((float2) globalCoords + 0.5) * rcp(RenderTargetSize);
    st = st * float2(2.0, -2.0) - float2(1.0, -1.0);
    
    float3 screenPosition;
    screenPosition.xy = st.xy;
    screenPosition.z = DepthTexture.Load(uint3(globalCoords, 0.0f));
    
    float4 cameraPosition = mul(InverseProjection, float4(screenPosition, 1.0f));
    return cameraPosition.xyz / cameraPosition.w;
}

/****************************************************************************
*				  		InitializeGroupMemory
*************************************************************************//**
*  @class     void InitializeGroupMemory(uint groupIndex)
*  @brief     Initialize Group shared memories.
*  @param[in] uint groupIndex
*****************************************************************************/
void InitializeGroupMemory(uint groupIndex)
{
    if (groupIndex == 0)
    {
        TileNumLights = 0;
        TileNumSpotLights = 0;
        MinDepth = 0x7F7FFFFF;
        MaxDepth = 0;
    }
}

/****************************************************************************
*				  		OutputPointLightLists
*************************************************************************//**
*  @class     void OutputPointLightLists(uint tileIndex, uint groupIndex)
*  @brief     This function outputs point light indexLists
*  @param[in] uint tileIndex
*  @param[in] uint groupIndex
*****************************************************************************/
void OutputPointLightLists(uint tileIndex, uint groupIndex)
{
    uint lightStart = PointLightNum * tileIndex;
    for (uint index = groupIndex; index < TileNumLights; index += TILE_SIZE)
    {
        LightIndices[lightStart + index] = TileLightIndices[index];
    }

    if ((groupIndex == 0) && (TileNumLights < (uint) PointLightNum))
    {
        LightIndices[lightStart + TileNumLights] = 0xffffffff; // -1
    }
}

/****************************************************************************
*				  		OutputSpotLightLists
*************************************************************************//**
*  @class     void OutputSpotLightLists(uint tileIndex, uint groupIndex)
*  @brief     This function outputs spot light indexLists
*  @param[in] uint tileIndex
*  @param[in] uint groupIndex
*****************************************************************************/
void OutputSpotLightLists(uint tileIndex, uint groupIndex)
{
    uint lightStart = SpotLightNum * tileIndex;
    for (uint index = groupIndex; index < TileNumSpotLights; index += TILE_SIZE)
    {
        SpotLightIndices[lightStart + index] = TileSpotLightIndices[index];
    }

    if ((groupIndex == 0) && (TileNumSpotLights < (uint) SpotLightNum))
    {
        SpotLightIndices[lightStart + TileNumSpotLights] = 0xffffffff; // -1
    }
}

/****************************************************************************
*				  		CollisionTestPointLights
*************************************************************************//**
*  @class     void CollisionTestPointLights(uint groupIndex, float4 frustrumPlanes[6])
*  @brief     This function executes collision test point lights vs frustum
*  @param[in] uint groupIndex
*  @param[in] float4 frustumPlanes[6]
*****************************************************************************/
void CollisionTestPointLights(uint groupIndex, float4 frustumPlanes[6])
{
    for (uint lightIndex = groupIndex; lightIndex < (uint) PointLightNum; lightIndex += TILE_SIZE)
    {
        PointLight light = PointLights[lightIndex];
        float4 positionWorld = mul(World, float4(PointLights[lightIndex].Position, 1.0f));
        float3 positionInView = mul(View, positionWorld).xyz;
        
        bool inFrustum = true;
        [unroll]
        for (uint i = 0; i < 6; ++i)
        {
            float4 lp = float4(positionInView, 1.0f);
            float d = dot(frustumPlanes[i], lp);
            inFrustum = inFrustum && (d >= -light.Range); // all region in frustum

        }

        if (inFrustum)
        {
            uint listIndex;
            InterlockedAdd(TileNumLights, 1, listIndex); // 
            TileLightIndices[listIndex] = lightIndex;
        }

    }
}

/****************************************************************************
*				  		CollisionTestSpotLights
*************************************************************************//**
*  @class     void CollisionTestSpotLights(uint groupIndex, float4 frustrumPlanes[6])
*  @brief     This function executes collision test spot lights vs frustum
*  @param[in] uint groupIndex
*  @param[in] float4 frustumPlanes[6]
*****************************************************************************/
void CollisionTestSpotLights(uint groupIndex, float4 frustrumPlanes[6])
{
    for (uint lightIndex = groupIndex; lightIndex < (uint) SpotLightNum; lightIndex += TILE_SIZE)
    {
        SpotLight light = SpotLights[lightIndex];
        
        float   spotLightConeHalfAngleCos = cos(light.OuterConeAngle * 0.5f);
        float   sphereRadius              = light.Range * 0.5f / (spotLightConeHalfAngleCos * spotLightConeHalfAngleCos);
        float3 spherePosition             = light.Position + light.Direction * sphereRadius;
        
        float4 positionWorld  = mul(World, float4(spherePosition, 1.0f));
        float3 positionInView = mul(View, positionWorld).xyz;
        
        bool inFrustum = true;
        [unroll]
        for (uint i = 0; i < 6; ++i)
        {
            float4 lp = float4(positionInView, 1.0f);
            float d   = dot(frustrumPlanes[i], lp);
            inFrustum = inFrustum && (d >= -light.Range); // all region in frustum

        }

        if (inFrustum)
        {
            uint listIndex;
            InterlockedAdd(TileNumSpotLights, 1, listIndex); // 
            TileSpotLightIndices[listIndex] = lightIndex;
        }

    }

}

[numthreads(TILE_WIDTH, TILE_HEIGHT, 1)]
void LightCulling( uint3 groupID          : SV_GroupID,
                   uint3 dispatchThreadID : SV_DispatchThreadID,
                   uint3 groupThreadID    : SV_GroupThreadID)
{
    /*-------------------------------------------------------------------
	-        Get group index
	---------------------------------------------------------------------*/
    uint groupIndex = groupThreadID.y * TILE_WIDTH + groupThreadID.x;
    InitializeGroupMemory(groupIndex);
    /*-------------------------------------------------------------------
	-        Get uv and position in view
	---------------------------------------------------------------------*/
    uint2 frameUV       = dispatchThreadID.xy;
    float3 positionView = ComputePositionInCamera(frameUV);    
    /*-------------------------------------------------------------------
	-        Sync group memory
	---------------------------------------------------------------------*/
    GroupMemoryBarrierWithGroupSync();
    InterlockedMin(MinDepth, asuint(positionView.z));
    InterlockedMax(MaxDepth, asuint(positionView.z));
    GroupMemoryBarrierWithGroupSync();
    /*-------------------------------------------------------------------
	-        Get frustrum planes
	---------------------------------------------------------------------*/
    float4 frustumPlanes[6];
    GetTileFrustrumPlane(frustumPlanes, groupID);
    
    /*-------------------------------------------------------------------
	-        Collision Detection between tiles and point, spot lights
	---------------------------------------------------------------------*/
    CollisionTestPointLights(groupIndex, frustumPlanes);
    CollisionTestSpotLights(groupIndex, frustumPlanes);
    GroupMemoryBarrierWithGroupSync();
    
     /*-------------------------------------------------------------------
	-        output light lists 
	---------------------------------------------------------------------*/
    uint numCellx   = (RenderTargetSize.x + TILE_WIDTH - 1) / TILE_WIDTH;
    uint tileIndex  = floor(frameUV.x / TILE_WIDTH) + floor(frameUV.y / TILE_HEIGHT) * numCellx;
    OutputPointLightLists(tileIndex, groupIndex);
    OutputSpotLightLists(tileIndex, groupIndex);

}

#endif