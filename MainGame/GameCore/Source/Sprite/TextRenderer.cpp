//////////////////////////////////////////////////////////////////////////////////
///             @file   Font.hpp
///             @brief  Font
///             @author Toide Yutaro
///             @date   2021_02_10
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Sprite/TextRenderer.hpp"
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define ASCII_START_CHAR 32 // start from ' '. 
using namespace gm;
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*							Initialize
*************************************************************************//**
*  @fn        bool TextRenderer::Initialize()
*  @brief     Load Font. (One call somewhere is enough)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool TextRenderer::Initialize()
{
	FontLoader fontLoader;
	if (!fontLoader.Initialize())      { return false; }
	if (!SpriteRenderer::Initialize()) { return false; }
	return true;
}

bool TextRenderer::DrawStart()
{
	if (!SpriteRenderer::DrawStart()) { return false; }
	return true;
}
/****************************************************************************
*							DrawString
*************************************************************************//**
*  @fn        bool TextRenderer::DrawString(FontType fontType, const TextString& text, const DirectX::XMFLOAT4& color, const DirectX::XMMATRIX& matrix)
*  @brief     Draw String
*  @param[in] FontType fontType
*  @param[in] const TextString& text
*  @param[in] const DirectX::XMFLOAT4& color
*  @param[in] const DirectX::XMMATRIX matrix(projectionView)
*  @return 　　bool
*****************************************************************************/
bool TextRenderer::DrawString(FontType fontType, const TextString& text, const Float4& color, const Matrix4& matrix)
{
	/*-------------------------------------------------------------------
	-              Variable definition
	---------------------------------------------------------------------*/
	FontLoader           fontLoader;
	std::vector<char>    string(text.String.length());
	std::vector<Sprite>  spriteList(text.String.length());
	FontInfo* fontInfo = &fontLoader.FontTable[fontType];

	/*-------------------------------------------------------------------
	-              Push_back the char 
	---------------------------------------------------------------------*/
	for (int i = 0; i < text.String.length(); ++i)
	{
		string[i] = static_cast<char>(text.String[i]);
	}

	/*-------------------------------------------------------------------
	-              Create Sprite (DirectX Coordinates)
	---------------------------------------------------------------------*/
	for (int i = 0; i < text.String.length(); ++i)
	{
		Float3 centerPosition = Float3(
			text.StartPosition.x + (i + 0.5f) * text.SizePerChar.x + i * text.Space,
			text.StartPosition.y - text.SizePerChar.y,
			0.0f);

		Float2 u = Float2(
			(string[i]      - ASCII_START_CHAR) * (fontInfo->PixelSizePerChar.x / fontInfo->ImagePixelWidth),
			(string[i] + 1  - ASCII_START_CHAR) * (fontInfo->PixelSizePerChar.x / fontInfo->ImagePixelWidth));

		Float2 v = Float2(0.0f, 1.0f);

		spriteList[i].CreateSprite(centerPosition, text.SizePerChar, color, u, v);
	}
	SpriteRenderer::Draw(spriteList, fontInfo->Texture, matrix);
	return true;
}

/****************************************************************************
*							DrawNumber
*************************************************************************//**
*  @fn        bool TextRenderer::DrawNumber(FontType fontType, const TextNumber& num, const DirectX::XMFLOAT4& color, const DirectX::XMMATRIX& matrix)
*  @brief     Draw Number
*  @param[in] FontType fontType
*  @param[in] const TextNumber& num
*  @param[in] const DirectX::XMFLOAT4& color
*  @param[in] const DirectX::XMMATRIX matrix(projectionView)
*  @return 　　bool
*****************************************************************************/
bool TextRenderer::DrawNumber(FontType fontType, const TextNumber& num, const Float4& color, const Matrix4& matrix)
{
	/*-------------------------------------------------------------------
	-              Variable definition
	---------------------------------------------------------------------*/
	FontLoader           fontLoader;
	std::vector<int>     values(num.Digit);
	std::vector<Sprite>  spriteList(num.Digit);
	FontInfo* fontInfo = &fontLoader.FontTable[fontType];

	/*-------------------------------------------------------------------
	-              Push_back the value of each digit.
	---------------------------------------------------------------------*/
	int tempValue = num.Number;
	for (int i = num.Digit - 1; i >= 0; --i)
	{
		values[i] = (tempValue % 10);
		tempValue = tempValue / 10;
	}

	/*-------------------------------------------------------------------
	-              Create Sprite (DirectX Coordinates)
	---------------------------------------------------------------------*/
	for (int i = 0; i < num.Digit; ++i)
	{
		Float3 centerPosition = Float3(
			num.StartPosition.x + (i + 0.5f) * num.SizePerDigit.x + i * num.Space,
			num.StartPosition.y - num.SizePerDigit.y,
			0.0f);

		Float2 u = Float2(
			values[i]       * (fontInfo->PixelSizePerChar.x / fontInfo->ImagePixelWidth),
			(values[i] + 1) * (fontInfo->PixelSizePerChar.x / fontInfo->ImagePixelWidth));

		Float2 v = Float2(0.0f, 1.0f);

		spriteList[i].CreateSprite(centerPosition, num.SizePerDigit, color, u, v);
	}

	SpriteRenderer::Draw(spriteList, fontInfo->Texture, matrix);
	return true;
}

/****************************************************************************
*							DrawEnd
*************************************************************************//**
*  @fn        bool TextRenderer::DrawEnd()
*  @brief     Called at the end of each frame
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool TextRenderer::DrawEnd()
{
	if (!SpriteRenderer::DrawEnd()) { return false; }
	return true;
}

/****************************************************************************
*							Finalize
*************************************************************************//**
*  @fn        bool TextRenderer::Finalize()
*  @brief     Clear Font. (One call somewhere is enough)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool TextRenderer::Finalize()
{
	FontLoader fontLoader;
	if (!fontLoader.Finalize()) { return false; }
	return true;
}