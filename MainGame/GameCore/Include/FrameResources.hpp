//////////////////////////////////////////////////////////////////////////////////
///             @file   FrameResource.hpp
///             @brief  Per Frame Resource Manager
///             @author Toide Yutaro
///             @date   2021_03_12
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef FRAME_RESOURCE_HPP
#define FRAME_RESOURCE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Core.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/DirectX12MathHelper.hpp"
#include "GameCore/Include/GameConstantBufferConfig.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include <DirectXMath.h>
#include <memory>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using ObjectConstantBuffer     = std::unique_ptr<UploadBuffer<ObjectConstants>>;
using SceneConstantBuffer      = std::unique_ptr<UploadBuffer<SceneConstants>>;
using MaterialConstantBuffer   = std::unique_ptr<UploadBuffer<MaterialConstants>>;
using SceneLightConstantBuffer = std::unique_ptr<UploadBuffer<SceneLightConstants>>;

class Camera;

struct FrameResourceCounter
{
	UINT MaxObjectCount     = 0;
	UINT MaxSceneCount      = 0;
	UINT MaxMaterialCount   = 0;
	UINT MaxSceneLightCount = 0;
};

/****************************************************************************
*				  			FrameResource
*************************************************************************//**
*  @struct     FrameResource
*  @brief      per frame resources
*****************************************************************************/
struct FrameResource
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void UpdateObjectConstants();
	void UpdateSceneConstants(SceneConstants* scene, const Camera* camera);
	void UpdateMaterialConstants();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	// We cannot update a cbuffer until the GPU is done processing the commands
	// that reference it.  So each frame needs their own cbuffers.
	ObjectConstantBuffer     ObjectConstantsBuffer     = nullptr;
	SceneConstantBuffer      SceneConstantsBuffer      = nullptr;
	MaterialConstantBuffer   MaterialConstantsBuffer   = nullptr;
	SceneLightConstantBuffer SceneLightConstantsBuffer = nullptr;

	// Fence value to mark commands up to this fence point.  This lets us
	// check if these frame resources are still in use by the GPU.
	UINT64 Fence = 0;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	FrameResource(const FrameResourceCounter& frameResourceCounter, const GameTimer& gameTimer);
	FrameResource(const FrameResource& rhs)            = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	FrameResource(FrameResource&&)                     = delete;
	FrameResource& operator=(FrameResource&&)          = delete;
	~FrameResource() {};

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	const GameTimer* _gameTimer = nullptr;
	
};


#endif