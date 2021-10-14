//////////////////////////////////////////////////////////////////////////////////
///             @file   PMXConfig.hpp
///             @brief  PMXFile Config
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PMD_MODEL_HPP
#define PMD_MODEL_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/Model.hpp"
#include "GameCore/Include/Model/MMD/PMDFile.hpp"
#include "GameCore/Include/GameConstantBufferConfig.hpp"
#include <Windows.h>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class VMDFile;
class GameTimer;
using VertexBuffer     = std::unique_ptr<UploadBuffer<PMDVertex>>;
using IndexBuffer      = std::unique_ptr<UploadBuffer<UINT16>>;
using BoneBuffer       = std::unique_ptr<UploadBuffer<BoneParameter>>;
using BoneMatrix       = std::unique_ptr<std::vector<gm::Matrix4>>;
using MaterialBuffer   = std::unique_ptr<UploadBuffer<PMDMaterial>>;
using ModelObject      = std::unique_ptr<UploadBuffer<ObjectConstants>>;
using AnimationList    = std::unordered_map<std::wstring, std::shared_ptr<VMDFile>>;
using SceneGPUAddress  = D3D12_GPU_VIRTUAL_ADDRESS;
using LightGPUAddress  = D3D12_GPU_VIRTUAL_ADDRESS;

class PMDModel : public DefaultModel
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(const std::wstring& filePath);
	bool Update();
	bool Draw(SceneGPUAddress scene, LightGPUAddress light);

	bool StartAnimation(const std::wstring& motionName);
	bool AddMotion(const std::wstring& filePath, const std::wstring& motionName);
	bool UpdateMotion();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetWorldTimer(const GameTimer& gameTimer);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
protected:
	/****************************************************************************
	**                ProtectedFunction
	*****************************************************************************/
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();
	bool PrepareMaterialBuffer();
	bool PreparePMDObject();
	bool PrepareBoneBuffer();

	void WriteBoneParameterToBuffer();
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	std::shared_ptr<PMDData>  _pmdData     = nullptr;
	
	/*-------------------------------------------------------------------
	-           Motion Data
	---------------------------------------------------------------------*/
	AnimationList             _motionData;
	const GameTimer*          _gameTimer   = nullptr;
	float                     _currentTime = 0;
	std::wstring              _currentMotionName;

	/*-------------------------------------------------------------------
	-           Buffer and View
	---------------------------------------------------------------------*/
	VertexBuffer              _vertexBuffer[FRAME_BUFFER_COUNT];
	MaterialBuffer            _materialBuffer;
	BoneBuffer                _boneBuffer;
	std::vector<UINT>         _materialView;

	/*-------------------------------------------------------------------
	-           Object Property
	---------------------------------------------------------------------*/
	BoneMatrix                _boneMatrices; // Bone matrix affected by each instance.


};

#endif