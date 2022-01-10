//////////////////////////////////////////////////////////////////////////////////
///             @file   Font.hpp
///             @brief  Font
///             @author Toide Yutaro
///             @date   2021_02_10
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef FONT_HPP
#define FONT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include "GameMath/Include/GMVector.hpp"
#include "FontType.hpp"
#include <unordered_map>
#pragma warning(disable : 26495)

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                            Font
//////////////////////////////////////////////////////////////////////////////////
struct FontInfo
{
	FontType     FontType;
	gm::Float2   PixelSizePerChar;
	float        ImagePixelWidth; 
	Texture      Texture;
	~FontInfo()
	{
		Texture.Resource = nullptr;
	}
};

class FontLoader
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static bool Initialize();
	static bool Finalize();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	static std::unordered_map<FontType, FontInfo> FontTable;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	FontLoader() = default;
	FontLoader& operator=(const FontLoader&) = default;
	FontLoader(FontLoader&&)                 = default;
	FontLoader& operator=(FontLoader&&)      = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	static bool LoadFonts(std::unordered_map<FontType, FontInfo>& fontTable);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif