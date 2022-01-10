//////////////////////////////////////////////////////////////////////////////////
///             @file   SSAO.hpp
///             @brief  SSAO
///             @author Toide Yutaro
///             @date   2021_08_16
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SSAO_HPP
#define SSAO_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/FrameResources.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define OFFSET_VECTOR_SIZE (14)
class Blur;
namespace ssao
{
	struct SSAOParameter
	{
		float TextureSize[2];
		float OcclusionRadius;
		float OcclusionSharpness;
		float OcclusionFadeStart;
		float OcculusionFadeEnd;
		float SurfaceEpsilon;
		gm::Float4 OffsetVectors[OFFSET_VECTOR_SIZE];
	};
}
//////////////////////////////////////////////////////////////////////////////////
//								Class 
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			LightCulling
*************************************************************************//**
*  @class     LightCulling
*  @brief     LightCulling
*****************************************************************************/
class SSAO final
{
	using SSAOParameterBuffer = std::unique_ptr<UploadBuffer<ssao::SSAOParameter>>;
	using SceneGPUAddress = D3D12_GPU_VIRTUAL_ADDRESS;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(int width, int height, ColorBuffer& normalMap, ColorBuffer& depthMap);
	bool ExecuteSSAO(SceneGPUAddress scene);
	void Finalize();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	Texture GetFinalTexture() { return _ambientMap.GetColorBuffer(); }
	ColorBuffer& GetFinalColorBuffer() { return _ambientMap; }
	void SetNormalMap(ColorBuffer& normalMap)    { _normalMap = &normalMap; }
	void SetDepthMap(ColorBuffer& depthMap)      { _depthMap  = &depthMap; }
	void SetOcclusionRadius(float radius)        { _parameter.OcclusionRadius = radius; }
	void SetOcclusionSharpness(float sharpness)             { _parameter.OcclusionSharpness = sharpness; }
	void SetOcclusionFadeStart(float startDepthInViewSpace) { _parameter.OcclusionFadeStart = startDepthInViewSpace; }
	void SetOcclusionFadeEnd(float endDepthInViewSpace)     { _parameter.OcculusionFadeEnd = endDepthInViewSpace; }
	void SetSurfaceEpsilon(float epsilonDepth)              { _parameter.SurfaceEpsilon = epsilonDepth; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	SSAO();
	~SSAO();
	SSAO(const SSAO&)            = default;
	SSAO& operator=(const SSAO&) = default;
	SSAO(SSAO&&)                 = default;
	SSAO& operator=(SSAO&&)      = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool PrepareResources(int width, int height);
	bool PrepareRootSignature();
	bool PreparePipelineState();
	bool PrepareParameterBuffer(int width, int height);
	bool PrepareRandomTexture();

	bool CopyToGPUSSAOParameter();
	bool OnResize(int newWidth, int newHeight);
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	RootSignatureComPtr _rootSignature      = nullptr;
	PipelineStateComPtr _pipeline           = nullptr;
	SSAOParameterBuffer _parameterBuffer    = nullptr;
	BlobComPtr          _vertexShader       = nullptr;
	BlobComPtr          _pixelShader        = nullptr;
	std::unique_ptr<Blur> _blur             = nullptr;
	std::unique_ptr<ColorBuffer> _randomMap = nullptr;
	ResourceComPtr _uploadBuffer            = nullptr;
	ColorBuffer _colorBuffer;
	ColorBuffer* _normalMap                 = nullptr;
	ColorBuffer* _depthMap                  = nullptr;
	ColorBuffer  _ambientMap;
	ssao::SSAOParameter _parameter;
	bool _initialized = false;
};

#endif