//////////////////////////////////////////////////////////////////////////////////
///             @file   SpriteRenderer.hpp
///             @brief  Sprite Renderer
///                     ‡@ Initialize
///                     ‡A Create Sprite -> std::vector<Sprite> pushback
///                     ‡B Sprite Update
///                     ‡C DrawStart() -> Draw() -> DrawEnd()
///             @author Toide Yutaro
///             @date   2021_01_20
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SPLITE_RENDERER_HPP
#define SPLITE_RENDERER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12MeshBuffer.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "Sprite.hpp"
#include "GameMath/Include/GMMatrix.hpp"
#include <memory>
#include "GameCore/Include/Effect/PostEffect.hpp"


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*					SpriteRenderer
*************************************************************************//**
*  @struct    SpriteRenderer
*  @brief     SpriteRenderer
*****************************************************************************/
class SpriteRenderer
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(FastBlendStateType type = FastBlendStateType::Normal);
	bool DrawStart();
	bool Draw(const std::vector<Sprite>& spriteList, const Texture& texture, const gm::Matrix4& matrix);
	bool DrawEnd();
	bool Finalize();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	SpriteRenderer();
	~SpriteRenderer();

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool PrepareRootSignature();
	bool PreparePipelineState(FastBlendStateType type);
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();
	bool PrepareConstantBuffer();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	PipelineStateComPtr     _pipelineState = nullptr;
	RootSignatureComPtr     _rootSignature = nullptr;
	std::vector<MeshBuffer> _meshBuffer;
	DescriptorHeapComPtr    _textureDescHeap = nullptr;
	std::vector<Texture>    _textures;
	std::vector<VertexPositionNormalColorTexture> _vertices;
	gm::Matrix4       _projectionViewMatrix;
	std::shared_ptr<UploadBuffer<VertexPositionNormalColorTexture>> _dynamicVertexBuffer[FRAME_BUFFER_COUNT];
	std::shared_ptr<UploadBuffer<gm::Matrix4>> _constantBuffer = nullptr;
	
	int _spriteStackCount = 0;
	int _drawCallNum = 0;
	std::vector<int> _spriteCountList;
	static const int MaxSpriteCount;
};


#endif

