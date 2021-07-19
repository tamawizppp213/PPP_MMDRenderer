//////////////////////////////////////////////////////////////////////////////////
///             @file   PostEffect.hpp
///             @brief  Default Post Effect
///                     ① Initialize
///                     ② Draw
///             @author Toide Yutaro
///             @date   2021_07_13
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef POST_EFFECT_HPP
#define POST_EFFECT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Sprite/Sprite.hpp"
#include "DirectX12/Include/Core/DirectX12BlendState.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                            Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			PostEffect
*************************************************************************//**
*  @class     Post Effect
*  @brief     Post Effect (Apply to the entire screen)
*****************************************************************************/
class PostEffect
{
	using VertexBufferPtr = std::unique_ptr<UploadBuffer<VertexPositionNormalColorTexture>>;
	using MeshBufferPtr   = std::unique_ptr<MeshBuffer>;
	using MatrixBufferPtr = std::unique_ptr<UploadBuffer<gm::Matrix4>>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(PostEffectBlendStateType type);
	virtual bool OnResize();
	virtual bool Draw(); // バックバッファにテクスチャを書き込む.

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PostEffect()                             = default;
	PostEffect(const PostEffect&)            = default;
	PostEffect& operator=(const PostEffect&) = default;
	PostEffect(PostEffect&&)                 = default;
	PostEffect& operator=(PostEffect&&)      = default;
	~PostEffect() = default;

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	bool PrepareRootSignature(PostEffectBlendStateType type);
	bool PreparePipelineState(PostEffectBlendStateType type);
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();
	bool PrepareConstantBuffer();
	bool PrepareSprite();
	bool PrepareResources();
	bool PrepareDescriptors();

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	Sprite              _sprite;
	std::vector<Texture> _colorBuffer;
	VertexBufferPtr     _vertexBuffer[FRAME_BUFFER_COUNT];
	MeshBufferPtr       _meshBuffer  [FRAME_BUFFER_COUNT];
	MatrixBufferPtr     _constantBuffer;
	
	RootSignatureComPtr _rootSignature = nullptr;
	PipelineStateComPtr _pipelineState = nullptr;
	static RootSignatureComPtr _defaultRootSignature;
	static PipelineStateComPtr _defaultPipelineState;
};


#endif