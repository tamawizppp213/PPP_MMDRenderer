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
#include "DirectX12BaseStruct.hpp"
#include "GameMath/Include/GMVector.hpp"
#include <DirectXTex/DirectXTex.h>
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
	gm::Float2        ImageSize;
};

enum class TextureType : UINT
{
	Unknown          = 0,
	Buffer           = 1,
	Texture1D        = 2,
	Texture1DArray   = 3,
	Texture2D        = 4,
	Texture2DArray   = 5,
	Texture2DMS      = 6,
	Texture2DMSArray = 7,
	Texture3D        = 8,
	TextureCube      = 9,
	TextureCubeArray = 10,
	RayTracing_Acceleration_Structure = 11
};

enum class SaveFormat
{
	BMP  = 1,
	JPEG = 2,
	PNG  = 3,
	TIFF = 4,
	GIF  = 5,
	WMP  = 6,
	ICO  = 7
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
	void ClearTextureTable()
	{
		TextureTable.clear(); ID = 0;
	}

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
	void LoadTexture(const std::wstring& filePath, Texture& texture, TextureType type = TextureType::Texture2D );

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
	void CreateTextureFromImageData(Device* device, const DirectX::Image* image, ResourceComPtr& textureBuffer, bool isDiscreteGPU, const DirectX::TexMetadata* metadata);
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	TextureTableManager& _textureTableManager = TextureTableManager::Instance();

};

#endif