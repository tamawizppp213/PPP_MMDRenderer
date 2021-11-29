//////////////////////////////////////////////////////////////////////////////////
///             @file   PMXConfig.hpp
///             @brief  PMXFile Config
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PMX_MODEL_HPP
#define PMX_MODEL_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/Model.hpp"
#include "GameCore/Include/Model/MMD/PMXFile.hpp"
#include "GameCore/Include/GameConstantBufferConfig.hpp"
#include <future>
#include <Windows.h>
#include <unordered_map>
#include <map>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class VMDFile;
class GameTimer;

using SceneGPUAddress  = D3D12_GPU_VIRTUAL_ADDRESS;
using LightGPUAddress  = D3D12_GPU_VIRTUAL_ADDRESS;
struct UpdateRange
{
	int VertexOffset;
	int VertexCount;
};

class PMXPhysicsManager
{
	using RigidBodyPtr = std::unique_ptr<PMXRigidBody>;
	using JointPtr     = std::unique_ptr<PMXJoint>;
public:
	PMXPhysicsManager();
	~PMXPhysicsManager();

	bool Create();
	MMDPhysics*   GetMMDPhysics();
	PMXRigidBody* AddRigidBody();
	PMXJoint*     AddJoint();

	std::vector<RigidBodyPtr>* GetRigidBodies() { return &_rigidBodies; }
	std::vector<JointPtr>*     GetJoints() { return &_joints; };
private:
	std::unique_ptr<MMDPhysics> _mmdPhysics;
	std::vector<RigidBodyPtr>   _rigidBodies;
	std::vector<JointPtr>       _joints;
};

class PMXModel : public DefaultModel
{
	using VertexBuffer         = std::unique_ptr<UploadBuffer<PMXVertex>>;
	using IndexBuffer          = std::unique_ptr<UploadBuffer<UINT32>>;
	using MaterialBuffer       = std::unique_ptr<UploadBuffer<PBRMaterial>>;
	using BoneBuffer           = std::unique_ptr<UploadBuffer<PMXBoneParameter>>;
	using BoneQuaternionBuffer = std::unique_ptr<UploadBuffer<PMXBoneQuaternion>>;
	using BonePositionBuffer   = std::unique_ptr<UploadBuffer<PMXBoneLocalPosition>>;
	using BoneMatrix           = std::unique_ptr<std::vector<gm::Matrix4>>;
	using AnimationList = std::unordered_map<std::wstring, std::shared_ptr<VMDFile>>;


public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual bool Initialize(const std::wstring& filePath);
	virtual bool Update();
	virtual bool Draw(SceneGPUAddress scene, LightGPUAddress light);

	bool StartAnimation(const std::wstring& motionName);
	bool PauseAnimation(); 
	bool StopAnimation();
	bool AddMotion     (const std::wstring& filePath, const std::wstring& motionName);
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetWorldTimer(const GameTimer& gameTimer);
	void ResetAnimationTimer()  { _currentTime = 0; };
	PMXData* GetPMXData() const { return _pmxData.get(); }
	UploadBuffer<PMXBoneParameter>* GetBoneBuffer      () const  { return _boneBuffer.get(); }
	UploadBuffer<PBRMaterial     >* GetMaterialBuffer  ()  const { return _materialBuffer.get(); }
	const std::vector<std::string>& GetRootBoneNodeName() const  { return _rootBoneNodeNames; }
	PMXBoneNode* GetRootBoneNode(std::string rootBoneName)       {  return &_boneMap.get()->at(rootBoneName);}
	PMXPhysicsManager* GetPMXPhysicsManager() { return &_physicsManager; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMXModel()                           = default;
	PMXModel(const PMXModel&)            = default;
	PMXModel& operator=(const PMXModel&) = default;
	PMXModel(PMXModel&&)                 = default;
	PMXModel& operator=(PMXModel&&)      = default;
	~PMXModel();
protected:
	/****************************************************************************
	**                ProtectedFunction
	*****************************************************************************/
#pragma region Prepare
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();
	bool PrepareMaterialBuffer();
	bool PrepareBoneBuffer();
	bool PrepareBoneMap();
	bool PrepareBoneIK();
	bool PreparePMXObject();
	bool PreparePhysics();
#pragma endregion Prepare
#pragma region Update 
	void UpdateTotalAnimation(); // morph, motion
	bool UpdateMotion(UINT32 frameNo);
	bool UpdateMorph (UINT32 frameNo);
	void UpdateBoneMatrices();
	void UpdateBoneNodeTransform(UINT32 frameNo);
	void UpdateNodeAnimation(bool isAfterPhysics, int frameNo = 0);
	void UpdatePhysicsAnimation(float deltaTime);
	void ResetPhysics();
	virtual bool UpdateGPUData();
	void SetUpParallelUpdate();

#pragma endregion Update
#pragma region Bone Function
	
	void ClearBoneMatrices();
	void WriteBoneParameterToBuffer();
#pragma endregion Bone Function
	
	UINT32 CalculateAnimationFrameNo();

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	std::shared_ptr<PMXData>  _pmxData = nullptr;

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
	int _parallelUpdateCount = 0;
	std::vector<std::thread> _threads;

	/*-------------------------------------------------------------------
	-           Motion Data
	---------------------------------------------------------------------*/
	bool                      _isAnimation = false;
	AnimationList             _motionData;
	const GameTimer*          _gameTimer = nullptr;
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
	std::unique_ptr<std::vector<PMXBoneNode*>>          _boneNodeAddress;
	std::unique_ptr<std::vector<PMXBoneNode*>>          _sortedBoneNodeAddress;
	std::vector<std::string>                            _rootBoneNodeNames; //操作中心と全ての親ボーン
	std::unique_ptr<std::vector<PMXBoneIK>>             _boneIKs;

	std::vector<std::pair<int, int>> _semiStandardBoneMap;
	PMXPhysicsManager                _physicsManager;
};

#endif