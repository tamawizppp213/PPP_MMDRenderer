#pragma once
#ifndef DIRECTX12_CONFIG_HPP
#define DIRECTX12_CONFIG_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Define Config
//////////////////////////////////////////////////////////////////////////////////
#define FRAME_BUFFER_COUNT 2 // double buffer
#define VSYNC 0// 0: don't wait, 1: wait (60fps)
#define FIXED_FRAME_RATE (60.0f)

#define CBV_DESC_COUNT 1000
#define SRV_DESC_COUNT 3800
#define UAV_DESC_COUNT 1000
#define DSV_DESC_COUNT 100
#define RTV_DESC_COUNT 1000

#define OFF_SCREEN_TEXTURE_NUM 4
#define USE_HDR 
//////////////////////////////////////////////////////////////////////////////////
//                             Enum 
//////////////////////////////////////////////////////////////////////////////////
enum class RenderTargetType
{
	BackBuffer0,
	BackBuffer1,
	ColorBuffer,
	Shadowing,
	CountOfRenderTarget
};

#endif
