//////////////////////////////////////////////////////////////////////////////////
///             @file   VMDConfig.hpp
///             @brief  CPDFile Config
///             @author Toide Yutaro
///             @date   2021_02_27
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef VMD_CONFIG_HPP
#define VMD_CONFIG_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <DirectXMath.h>
#include <Windows.h>
#include <array>
#include <string>
#include <vector>
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#pragma warning(disable : 26495)
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
namespace vmd
{
	using namespace DirectX;
#pragma pack(1)
	struct VMDHeader
	{
		char Header   [30];
		char ModelName[20];
	};

	struct VMDMotion
	{
		char                  BoneName[15];
		UINT32                Frame;
		XMFLOAT3              Translation;
		XMFLOAT4              Quarternion;
		std::array<UINT8, 64> Interpolation;
	};

	struct VMDMorph
	{
		char   Name[15];
		UINT32 Frame;
		float  Weight;
	};

	struct VMDCamera
	{
		UINT32                Frame;
		float                 Distance;
		XMFLOAT3              Interest;
		XMFLOAT3              Rotation;
		std::array<UINT8, 24> Interpolation;
		UINT32                ViewAngle;
		UINT8                 IsPerspective;
	};

	struct VMDLight
	{
		UINT32   Frame;
		XMFLOAT3 Color;
		XMFLOAT3 Position;
	};

	enum class VMDShadowType : UINT8
	{
		Off,
		Mode1,
		Mode2
	};

	struct VMDShadow
	{
		UINT32        Frame;
		VMDShadowType ShadowType;
		float         Distance;
	};

	struct VMDIKInfo
	{
		char  IKName[20];
		UINT8 IsEnabled;
	};

	struct VMDIK
	{
		UINT32                 Frame;
		UINT8                  IsShow;
		std::vector<VMDIKInfo> IKInfo;
	};
#pragma pack()
}
#endif