//////////////////////////////////////////////////////////////////////////////////
///             @file   ZPrepass.hpp
///             @brief  ZPrepass Class
///             @author Toide Yutaro
///             @date   2021_07_30
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef ZPREPASS_HPP
#define ZPREPASS_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/GameActor.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//								Class 
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			RenderingEngine
*************************************************************************//**
*  @class    RenderingEngine
*  @brief     Frustum
*****************************************************************************/
class ZPrepass final
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(int width, int height);
	bool OnResize(int newWidth, int newHeight);
	bool Draw(D3D12_GPU_VIRTUAL_ADDRESS scene);

	bool ClearActors();
	bool ClearActor(GameActor& actor);
	bool AddActors (GameActor& actor);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	// call after draw function
	Texture& GetFinalRenderTexture()   { return _colorBuffer[0].GetColorBuffer(); }
	ColorBuffer& GetFinalColorBuffer() { return _colorBuffer[0]; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ZPrepass()                           = default;
	ZPrepass(const ZPrepass&)            = default;
	ZPrepass& operator=(const ZPrepass&) = default;
	ZPrepass(ZPrepass&&)                 = default;
	ZPrepass& operator=(ZPrepass&&)      = default;
	~ZPrepass();
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
	RootSignatureComPtr _rootSignature;
	std::vector<SimpleGraphicPipeline> _shader;
	std::vector<PipelineStateComPtr> _pipelines;
	std::vector<GameActor*>  _actors;


	std::vector<ColorBuffer> _colorBuffer;
	bool _initialized = false;
};

#endif