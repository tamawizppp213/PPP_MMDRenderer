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
#include <vector>
#include <memory>
#include <DirectXMath.h>
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
	bool Initialize();
	bool DrawStart();
	bool Draw(const std::vector<Sprite>& spriteList, const Texture& texture, const DirectX::XMMATRIX& matrix);
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
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();
	bool PrepareConstantBuffer();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::vector<MeshBuffer> _meshBuffer;
	DescriptorHeapComPtr    _textureDescHeap = nullptr;
	std::vector<Texture>    _textures;
	std::vector<VertexPositionNormalColorTexture> _vertices;
	DirectX::XMMATRIX       _projectionViewMatrix;
	std::shared_ptr<UploadBuffer<VertexPositionNormalColorTexture>> _dynamicVertexBuffer[FRAME_BUFFER_COUNT];
	std::shared_ptr<UploadBuffer<DirectX::XMMATRIX>>                _constantBuffer = nullptr;
	
	int _spriteStackCount = 0;
	int _drawCallNum = 0;
	std::vector<int> _spriteCountList;
	static const int MaxSpriteCount;
};


#endif

