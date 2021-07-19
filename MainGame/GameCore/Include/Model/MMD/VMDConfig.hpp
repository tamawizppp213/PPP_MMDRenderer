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
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "GameMath/Include/GMQuaternion.hpp"
#include <unordered_map>
#include <Windows.h>
#include <array>
#include <string>
#include <vector>

#pragma warning(disable : 26495)
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
namespace vmd
{
#pragma pack(1)
	struct VMDHeader
	{
		char Header   [30];
		char ModelName[20];
	};

	struct VMDKeyFrame
	{
		char                  BoneName[15];
		UINT32                Frame;
		gm::Float3            Translation;
		gm::Float4            Quarternion;   
		std::array<UINT8, 64> Interpolation; // bezier
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
		gm::Float3            Interest;
		gm::Float3            Rotation;
		std::array<UINT8, 24> Interpolation;
		UINT32                ViewAngle;
		UINT8                 IsPerspective;
	};

	struct VMDLight
	{
		UINT32     Frame;
		gm::Float3 Color;
		gm::Float3 Position;
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

/****************************************************************************
*				  			VMDMotion
*************************************************************************//**
*  @class     VMDMotion
*  @brief     VMD Motion Data
*****************************************************************************/
class VMDKeyFrame
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	UINT32                     Frame;
	gm::Quaternion             Quaternion;
	gm::Vector3                Location;
	std::array<gm::Vector4, 4> ControlPointForBezier; // xyzRotate

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	VMDKeyFrame(UINT32 frame, const gm::Quaternion& quaternion, const gm::Vector3& location,
		const gm::Vector4& controlPointX, const gm::Vector4& controlPointY, const gm::Vector4& controlPointZ, const gm::Vector4& controlPointR )
	{
		Frame      = frame;
		Quaternion = quaternion;
		Location   = location;
		ControlPointForBezier[0] = controlPointX;
		ControlPointForBezier[1] = controlPointY;
		ControlPointForBezier[2] = controlPointZ;
		ControlPointForBezier[3] = controlPointR;
	}

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

};

/****************************************************************************
*				  			VMDMorph
*************************************************************************//**
*  @class     VMD Morph
*  @brief     VMD IK
*****************************************************************************/
class VMDKeyFrameMorph
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	UINT32 Frame;
	float  Weight;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	VMDKeyFrameMorph(const vmd::VMDMorph morph)
	{
		Frame  = morph.Frame;
		Weight = morph.Weight;
	}
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

};
/****************************************************************************
*				  			VMDIK
*************************************************************************//**
*  @class     VMDIK
*  @brief     VMD IK
*****************************************************************************/
class VMDIK
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	UINT32 FrameNo;
	UINT8  IsShow;
	std::unordered_map<std::string, bool> IKEnableTable;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	VMDIK(const vmd::VMDIK& ik)
	{
		FrameNo = ik.Frame;
		IsShow  = ik.IsShow;
		for (int i = 0; i < ik.IKInfo.size(); ++i)
		{
			IKEnableTable[ik.IKInfo[i].IKName] = ik.IKInfo[i].IsEnabled;
		}
	}
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	
};
#endif