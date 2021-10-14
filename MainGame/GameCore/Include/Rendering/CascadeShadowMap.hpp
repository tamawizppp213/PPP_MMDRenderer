//////////////////////////////////////////////////////////////////////////////////
///             @file   CascadeShadowMap.hpp
///             @brief  Cascade Shadow Map Class
///             @author Toide Yutaro
///             @date   
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef CASCADE_SHADOW_MAP_HPP
#define CASCADE_SHADOW_MAP_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMMatrix.hpp"
#include "GameCore/Include/Core/GameActor.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include <array>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class ShadowViewType
{
	Near,
	Medium,
	Far,
	CountOfShadowViewType
};

class Camera;
//////////////////////////////////////////////////////////////////////////////////
//								Camera 
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			CascadeShadowMapMatrix
*************************************************************************//**
*  @struct    CascadeShadowMapMatrix
*  @brief     CascadeShadowMapMatrix
*****************************************************************************/
struct CascadeShadowMapMatrix
{
public:
	void CalculateLightViewProjectionCropMatrix(const Camera& camera, const gm::Vector3& lightDirection, const gm::Vector4& plane = gm::Vector4(0,1,0,0));
	const gm::Matrix4& GetLightViewProjectionCropMatrix(int shadowMapID) 
	{ 
		return _lightViewMatrix[shadowMapID]; 
	}
private:
	std::array<gm::Matrix4, (int)ShadowViewType::CountOfShadowViewType> _lightViewMatrix;
};

namespace shadow
{
	struct ShadowCameraInfo
	{
		gm::Matrix4 ViewProjection;
	};
}

/****************************************************************************
*				  			CascadeShadowMapMatrix
*************************************************************************//**
*  @struct    CascadeShadowMapMatrix
*  @brief     CascadeShadowMapMatrix
*****************************************************************************/
class CascadeShadowMap
{
	using CameraPtr              = std::unique_ptr<Camera>;
	using ColorBuffers           = std::vector<ColorBuffer>;
	using PipelineStateComPtrs   = std::vector<PipelineStateComPtr>;
	using SimpleGraphicPipelines = std::vector<SimpleGraphicPipeline>;
	using GameActors             = std::vector<GameActor*>;
	using ShadowBuffer           = std::unique_ptr<UploadBuffer<shadow::ShadowCameraInfo>>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize();
	void AddActor(GameActor* nearActor, GameActor* mediumActor, GameActor* farActor); // near, medium, far
	bool Execute(const gm::Vector3& lightDirection);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	CascadeShadowMap();
	~CascadeShadowMap();
protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool PrepareShader();
	bool PrepareResources();
	bool PrepareRootSignature();
	bool PreparePipelineState();
	bool PrepareCamera();
	bool PrepareConstantBuffer();

	bool UpdateLightCamera(const gm::Vector3& lightDirection);

	bool DrawPMXActor(int shadowType, int modelIndex);
	bool DrawPrimitiveActor(int shadowType, int modelIndex);
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	CascadeShadowMapMatrix  _cascadeShadowMapMatrix;
	ColorBuffers            _colorBuffers;
	RootSignatureComPtr     _rootSignature = nullptr;
	PipelineStateComPtrs    _pipelines;
	SimpleGraphicPipelines  _shader;
	GameActors              _actors[(int)ShadowViewType::CountOfShadowViewType];
	CameraPtr _lightCamera = nullptr;
	ShadowBuffer            _shadowBuffers[(int)ShadowViewType::CountOfShadowViewType];
	bool _isInitialized = false;

};
#endif