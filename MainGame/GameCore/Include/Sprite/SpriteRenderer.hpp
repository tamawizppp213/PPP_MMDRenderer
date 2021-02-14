//////////////////////////////////////////////////////////////////////////////////
///             @file   SpriteRenderer.hpp
///             @brief  Sprite Renderer
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
	std::shared_ptr<UploadBuffer<VertexPositionNormalColorTexture>> _dynamicVertexBuffer[FRAME_BUFFER_COUNT];
	std::shared_ptr<UploadBuffer<DirectX::XMMATRIX>> _constantBuffer = nullptr;
	DescriptorHeapComPtr    _textureDescHeap = nullptr;
	DirectX::XMMATRIX       _projectionViewMatrix;
	
	int _spriteStackCount = 0;
	int _drawCallNum = 0;
	std::vector<int> _spriteCountList;
	static const int MaxSpriteCount;
};


#endif

