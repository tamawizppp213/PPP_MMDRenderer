//////////////////////////////////////////////////////////////////////////////////
///             @file   Font.hpp
///             @brief  Font
///             @author Toide Yutaro
///             @date   2021_02_10
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Sprite/Font.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
std::unordered_map<FontType, FontInfo> FontLoader::FontTable = {};
namespace font
{
	bool CreateFontGennnokakuEnglish(std::unordered_map<FontType, FontInfo>& fontTable);
	bool CreateFontGennnokakuNumber(std::unordered_map<FontType, FontInfo>& fontTable);
}

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*							Initialize
*************************************************************************//**
*  @fn        bool FontLoader::Initialize()
*  @brief     Load Font
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool FontLoader::Initialize()
{
	if (!FontTable.empty())    { return true; }
	if (!LoadFonts(FontTable)) { return false; }
	return true;
}

/****************************************************************************
*							Finalize
*************************************************************************//**
*  @fn        bool FontLoader::Finalize()
*  @brief     Clear Font.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool FontLoader::Finalize()
{
	FontTable.clear();
	return true;
}

#pragma endregion Public Function

#pragma region Private Function
/****************************************************************************
*							LoadFonts
*************************************************************************//**
*  @fn        bool FontLoader::LoadFonts(std::unordered_map<FontType, FontInfo>& fontTable)
*  @brief     Load Font. 
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool FontLoader::LoadFonts(std::unordered_map<FontType, FontInfo>& fontTable)
{
	if (font::CreateFontGennnokakuEnglish(fontTable)){ return false; }
	if (font::CreateFontGennnokakuNumber(fontTable)) { return false; }

	return true;
}
#pragma endregion Private Function

#pragma region CreateFontConfig
namespace font
{
	bool CreateFontGennnokakuEnglish(std::unordered_map<FontType, FontInfo>& fontTable)
	{
		TextureLoader textureLoader;
		textureLoader.LoadTexture(L"Resources/Font/GennokakuEnglish.png", fontTable[FontType::GenNoKaku_Gothic_English].Texture);
		fontTable[FontType::GenNoKaku_Gothic_English].PixelSizePerChar = DirectX::XMFLOAT2(35.0f, 64.0f);
		fontTable[FontType::GenNoKaku_Gothic_English].ImagePixelWidth  = 3325.0f;
		fontTable[FontType::GenNoKaku_Gothic_English].FontType         = FontType::GenNoKaku_Gothic_English;
		return true;
	}

	bool CreateFontGennnokakuNumber(std::unordered_map<FontType, FontInfo>& fontTable)
	{
		TextureLoader textureLoader;
		textureLoader.LoadTexture(L"Resources/Font/GennnokakuNumber.png", fontTable[FontType::GenNoKaku_Gothic_Number].Texture);
		fontTable[FontType::GenNoKaku_Gothic_Number].PixelSizePerChar = DirectX::XMFLOAT2(51.2f, 64.0f);
		fontTable[FontType::GenNoKaku_Gothic_Number].ImagePixelWidth  = 512.0f;
		fontTable[FontType::GenNoKaku_Gothic_Number].FontType         = FontType::GenNoKaku_Gothic_Number;
		return true;
	}
}
#pragma endregion CreateFontConfig