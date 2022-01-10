//////////////////////////////////////////////////////////////////////////////////
///             @file   ZPrepass.hpp
///             @brief  ZPrepass Class
///             @author Toide Yutaro
///             @date   2021_07_30
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GBUFFER_HPP
#define GBUFFER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/GameActor.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class GBufferType
{
	Albedo,
	Normal,
	WorldPosition,
	CountOfGBufferType
};

//////////////////////////////////////////////////////////////////////////////////
//								Class 
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			RenderingEngine
*************************************************************************//**
*  @class    RenderingEngine
*  @brief     Frustum
*****************************************************************************/
class GBuffer final // –¢ŽÀ‘• (08/13)
{
	using RenderTargets = std::array<ColorBuffer, (int)GBufferType::CountOfGBufferType>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(int width, int height);
	bool OnResize(int newWidth, int newHeight);
	bool Draw(D3D12_GPU_VIRTUAL_ADDRESS scene);
	void Finalize();

	bool ClearActors();
	bool ClearActor(GameActor& actor);
	bool AddActor(GameActor& actor);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	// call after draw function
	Texture& GetRenderTexture(GBufferType type) { return _colorBuffer[(int)type].GetColorBuffer(); }
	ColorBuffer& GetGBuffer(GBufferType type) { return _colorBuffer[(int)type]; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	GBuffer() = default;
	GBuffer(const GBuffer&)            = default;
	GBuffer& operator=(const GBuffer&) = default;
	GBuffer(GBuffer&&)                 = default;
	GBuffer& operator=(GBuffer&&)      = default;
	~GBuffer();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool PrepareShader();
	bool PrepareRootSignature();
	bool PreparePipelineState();
	bool PrepareResources(int width, int height);


	bool DrawPMXActor(int index);
	bool DrawPrimitiveActor(int index);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::vector<RootSignatureComPtr> _rootSignatures;
	std::vector<SimpleGraphicPipeline> _shader;
	std::vector<PipelineStateComPtr> _pipelines;
	std::vector<GameActor*>  _actors;

	 RenderTargets _colorBuffer;
};

#endif