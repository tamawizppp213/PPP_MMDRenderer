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
#define ASCII_START_CHAR 20 // start from ' '. 

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
bool TextRenderer::DrawString(FontType fontType, const TextString& text, const DirectX::XMFLOAT4& color, const DirectX::XMMATRIX& matrix)
{
	/*-------------------------------------------------------------------
	-              Variable definition
	---------------------------------------------------------------------*/
	using namespace DirectX;
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
		XMFLOAT3 centerPosition = XMFLOAT3(
			text.StartPosition.x + (i + 0.5f) * text.SizePerChar.x + i * text.Space,
			text.StartPosition.y - text.SizePerChar.y,
			0.0f);

		XMFLOAT2 u = XMFLOAT2(
			(string[(INT64)i]     - ASCII_START_CHAR) * (fontInfo->PixelSizePerChar.x / fontInfo->ImagePixelWidth),
			(string[(INT64)i + 1] - ASCII_START_CHAR) * (fontInfo->PixelSizePerChar.y / fontInfo->ImagePixelWidth));

		XMFLOAT2 v = XMFLOAT2(0.0f, 1.0f);

		spriteList[i].CreateSprite(centerPosition, text.SizePerChar, color, u, v);
	}
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
bool TextRenderer::DrawNumber(FontType fontType, const TextNumber& num, const DirectX::XMFLOAT4& color, const DirectX::XMMATRIX& matrix)
{
	/*-------------------------------------------------------------------
	-              Variable definition
	---------------------------------------------------------------------*/
	using namespace DirectX;
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
		XMFLOAT3 centerPosition = XMFLOAT3(
			num.StartPosition.x + (i + 0.5f) * num.SizePerDigit.x + i * num.Space,
			num.StartPosition.y - num.SizePerDigit.y,
			0.0f);

		XMFLOAT2 u = XMFLOAT2(
			values[i] * (fontInfo->PixelSizePerChar.x / fontInfo->ImagePixelWidth),
			values[(INT64)i + 1] * (fontInfo->PixelSizePerChar.y / fontInfo->ImagePixelWidth));

		XMFLOAT2 v = XMFLOAT2(0.0f, 1.0f);

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