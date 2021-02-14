//////////////////////////////////////////////////////////////////////////////////
///             @file   Text.hpp
///             @brief  Text 
///             @author Toide Yutaro
///             @date   2021_02_09
//////////////////////////////////////////////////////////////////////////////////
#pragma once 
#ifndef FADE_HPP
#define FADE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12MeshBuffer.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include <DirectXMath.h>
#include <vector>
#include <memory>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct Texture;

//////////////////////////////////////////////////////////////////////////////////
//                             Class
//////////////////////////////////////////////////////////////////////////////////
enum class FadeState
{
	FADE_STATE_NONE,
	FADE_STATE_IN,
	FADE_STATE_OUT
};

/****************************************************************************
*				  			Fade
*************************************************************************//**
*  @class     Fade
*  @brief     Fade
*****************************************************************************/
class Fader
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(); 
	bool Draw(const DirectX::XMMATRIX& projViewMatrix);
	bool Finalize();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetColor(const DirectX::XMFLOAT3& color);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool LoadWhiteTexture(); 
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();
	bool PrepareConstantBuffer();
	bool UpdateFade();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	DirectX::XMFLOAT4       _color;
	FadeState               _currentFadeState;
	DirectX::XMMATRIX       _projectionViewMatrix = DirectX::XMMatrixIdentity();
	Texture                 _texture;
	std::vector<MeshBuffer> _meshBuffer;
	std::shared_ptr<UploadBuffer<VertexPositionNormalColorTexture>> _dynamicVertexBuffer[FRAME_BUFFER_COUNT];
	std::shared_ptr<UploadBuffer<DirectX::XMMATRIX>> _constantBuffer;
};

#endif