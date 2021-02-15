//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12Texture.hpp
///             @brief  .tga, .dds, ,png, .jpg, .bmp, .hdr
///             @author Toide Yutaro
///             @date   2020_12_06
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_TEXTURE_HPP
#define DIRECTX12_TEXTURE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12Core.hpp"
#include "DirectX12/Include/Core/DirectX12BaseStruct.hpp"
#include <DirectXTex/DirectXTex.h>
#include <string>
#include <unordered_map>
#pragma warning(disable : 26495)

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct Texture
{
	ResourceComPtr    Resource;
	DXGI_FORMAT       Format;
	GPU_DESC_HANDLER  GPUHandler;
	DirectX::XMFLOAT2 ImageSize;
};

/****************************************************************************
*				  		TextureTable (Singleton) 
*************************************************************************//**
*  @class     TextureTableManager
*  @brief     Texture Table 
*****************************************************************************/
class TextureTableManager
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	std::unordered_map<std::wstring, Texture> TextureTable;
	int ID = 0;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static TextureTableManager& Instance()
	{
		static TextureTableManager textureTable;
		return textureTable;
	}
	// Prohibit move and copy.
	// Singleton answer (public or private): https://stackoverflow.com/questions/18931133/must-a-deleted-constructor-be-private
	TextureTableManager(const TextureTableManager&)            = delete;
	TextureTableManager& operator=(const TextureTableManager&) = delete;
	TextureTableManager(TextureTableManager&&)                 = delete;
	TextureTableManager& operator=(TextureTableManager&&)      = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	TextureTableManager()  = default;
	~TextureTableManager() = default;
	
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	
};

/****************************************************************************
*				  			    Texture
*************************************************************************//**
*  @class     Texture
*  @brief     Texture Manager .tga, .dds, ,png, .jpg, .bmp, .hdr
*****************************************************************************/
class TextureLoader
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void LoadTexture(const std::wstring& filePath, Texture& texture);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void CreateTextureFromImageData(Device* device, const DirectX::Image* image, ResourceComPtr& textureBuffer, bool isDiscreteGPU);
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	TextureTableManager& _textureTableManager = TextureTableManager::Instance();

};

#endif