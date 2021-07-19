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
#include "GameCore/Include/Model/MMD/PMXFile.hpp"
#include "GameCore/Include/GameConstantBufferConfig.hpp"
#include <Windows.h>
#include <unordered_map>
#include <map>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class VMDFile;
class GameTimer;

using SceneGPUAddress  = D3D12_GPU_VIRTUAL_ADDRESS;

class PMXModel : public DefaultModel
{
	using VertexBuffer = std::unique_ptr<UploadBuffer<PMXVertex>>;
	using IndexBuffer = std::unique_ptr<UploadBuffer<UINT16>>;
	using MaterialBuffer = std::unique_ptr<UploadBuffer<PMXMaterial>>;
	using ModelObject = std::unique_ptr<UploadBuffer<ObjectConstants>>;
	using BoneBuffer = std::unique_ptr<UploadBuffer<PMXBoneParameter>>;
	using BoneQuaternionBuffer = std::unique_ptr<UploadBuffer<PMXBoneQuaternion>>;
	using BonePositionBuffer   = std::unique_ptr<UploadBuffer<PMXBoneLocalPosition>>;
	using BoneMatrix           = std::unique_ptr<std::vector<gm::Matrix4>>;
	using AnimationList = std::unordered_map<std::wstring, std::shared_ptr<VMDFile>>;

public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(const std::wstring& filePath);
	bool Update();
	bool Draw(SceneGPUAddress scene);

	bool StartAnimation(const std::wstring& motionName);
	bool AddMotion(const std::wstring& filePath, const std::wstring& motionName);
	bool UpdateMotion();
	bool UpdateMorph();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(const gm::Quaternion& rotation);
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
	bool PrepareBoneBuffer();
	bool PrepareBoneMap();
	bool PrepareBoneIK();
	bool PreparePMXObject();

	void WriteBoneParameterToBuffer();
	

	void UpdateBoneMatrices();
	void UpdateNodeAnimation(bool isAfterPhysics, int frameNo = 0);
	void UpdatePhysicsAnimation();
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	std::shared_ptr<PMXData>  _pmxData = nullptr;
	ModelObject               _modelObject = nullptr;

	/*-------------------------------------------------------------------
	-           Buffer and View
	---------------------------------------------------------------------*/
	VertexBuffer              _vertexBuffer[FRAME_BUFFER_COUNT];
	MaterialBuffer            _materialBuffer;
	BoneBuffer                _boneBuffer;
	BoneQuaternionBuffer      _boneQuaternionBuffer; // for sdef
	BonePositionBuffer        _bonePositionBuffer;   // for sdef
	std::vector<UINT>         _materialView;
	int _currentFrameIndex = 0;


	/*-------------------------------------------------------------------
	-           Motion Data
	---------------------------------------------------------------------*/
	bool                      _isAnimation = false;
	AnimationList             _motionData;
	const GameTimer* _gameTimer = nullptr;
	float                     _currentTime = 0;
	std::wstring              _currentMotionName;


	/*-------------------------------------------------------------------
	-           Object Property
	---------------------------------------------------------------------*/
	BoneMatrix                _boneMatrices; // Bone matrix affected by each instance.
	std::unique_ptr<std::vector<gm::Float3>> _bonePosition;
	std::unique_ptr<std::vector<gm::Float4>> _boneQuaternion;
	std::unique_ptr<PMXVertex[]>   _vertices;
	std::unique_ptr<std::map<std::string, PMXBoneNode>> _boneMap;
	std::unique_ptr<std::vector<PMXBoneNode*>> _boneNodeAddress;
	std::unique_ptr<std::vector<PMXBoneNode*>> _sortedBoneNodeAddress;
	std::unique_ptr<std::vector<PMXBoneIK>> _boneIKs;
};

#endif