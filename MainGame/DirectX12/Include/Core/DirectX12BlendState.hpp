//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12BlendState.hpp
///             @brief  blend state
///             @author Toide Yutaro
///             @date   2022_07_12
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_BLENDSTATE_HPP
#define DIRECTX12_BLENDSTATE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12BaseStruct.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                       BlendStateType
*************************************************************************//**
*  @fn        enum class BlendStateType
*  @brief     Pixel color blend type (note: These types use off-screen rendering))
*****************************************************************************/
enum class BlendStateType
{
	None, 
	Normal,
	Addition,
	Subtraction,
	Multiplication,
	Division,
	Screen,
	Overlay,
	Dark,
	Light,
	ColorDark,
	ColorLight,
	BurnColor,
	BurnLinear,
	DodgeColor,
	DodgeLinear,
	SoftLight,
	HardLight,
	VividLight,
	LinearLight,
	PinLight,
	HardMix,
	AbsoluteSubstract,
	Exclusion,
	BrightExtract,
	ColorKeyAlpha,
	ColorTone,
	CountOfBlendStateType
};

/****************************************************************************
*                       FastBlendStateType
*************************************************************************//**
*  @fn        enum class FastBlendStateType 
*  @brief     Fast color blend type (non-offscreen rendering)
*****************************************************************************/
enum class FastBlendStateType
{
	None,
	Normal,
	Addition,
	Subtraction,
	Screen,
	CountOfFastBlendStateType
};

/****************************************************************************
*                       PostEffectBlendStateType
*************************************************************************//**
*  @fn        enum class PostEffectBlendStateType
*  @brief     Post Effect color blend.
*****************************************************************************/
enum class PostEffectBlendStateType
{
	// shader default 2D
	None,
	Monochrome,
	Invert,
	Sepia,
	GrayScale,
	Binary,

	// Special Effects
	Blur,
	CountOfBlendStateType
};


/****************************************************************************
*                       ShaderBlend
*************************************************************************//**
*  @fn        ShaderBlend
*  @brief     DrawPMDModel
*****************************************************************************/
struct ShaderBlendData
{
	BlendStateType Type;
	BlobComPtr     VertexShader;
	BlobComPtr     PixelShader;
};

struct FastShaderBlendData
{
	FastBlendStateType Type;
	BlobComPtr         VertexShader;
	BlobComPtr         PixelShader;
};

struct PostEffectShaderBlendData
{
	PostEffectBlendStateType Type;
	BlobComPtr VertexShader;
	BlobComPtr PixelShader;
};


void InitializeShaderBlendData();

ShaderBlendData           GetShaderBlendData(BlendStateType           type);
FastShaderBlendData       GetShaderBlendData(FastBlendStateType       type);
PostEffectShaderBlendData GetShaderBlendData(PostEffectBlendStateType type);

BLEND_DESC GetBlendDesc(BlendStateType           type);
BLEND_DESC GetBlendDesc(FastBlendStateType       type);
BLEND_DESC GetBlendDesc(PostEffectBlendStateType type);
#endif