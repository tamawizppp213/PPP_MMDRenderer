//////////////////////////////////////////////////////////////////////////////////
///             @file   PMXModel.cpp
///             @brief  PMXModel
///             @author Toide Yutaro
///             @date   2021_06_18
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/MMD/PMXModel.hpp"
#include "GameCore/Include/Model/ModelPipelineState.hpp"
#include "GameCore/Include/Model/ModelLoader.hpp"
#include "GameCore/Include/Model/MotionLoader.hpp"
#include "GameCore/Include/Model/MMD/VMDAnimation.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include <thread>
#include <chrono>
#include <d3dcompiler.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define PMX_FRAME_PER_SECOND 30

inline Float3& operator+=(Float3& a, const Float3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

PMXModel::~PMXModel() {};

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        bool PMXModel::Initialize()
*  @brief     Initialize (Load PMDData ...)
*  @param[in] const std::wstring& filePath
*  @param[in] const std::wstring& addDebugName
*  @return 　　bool
*****************************************************************************/
bool PMXModel::Initialize(const std::wstring& filePath, const std::wstring& addName)
{
	SetActorType(ActorType::PMX);
	/*-------------------------------------------------------------------
	-             Load PMD Data
	---------------------------------------------------------------------*/
	ModelPSOManager& modelManager = ModelPSOManager::Instance();
	ModelLoader modelLoader;
	modelLoader.Load3DModel(filePath, &_pmxData);

	/*-------------------------------------------------------------------
	-             Prepare Vertex Buffer
	---------------------------------------------------------------------*/
	if (!PrepareVertexBuffer()) { MessageBox(NULL, L" Vertex buffer cannot be prepared.", L"Warning", MB_ICONWARNING); return false; };

	/*-------------------------------------------------------------------
	-             Prepare Index Buffer
	---------------------------------------------------------------------*/
	if (!PrepareIndexBuffer()) { MessageBox(NULL, L" Index buffer cannot be prepared.", L"Warning", MB_ICONWARNING); return false; };

	/*-------------------------------------------------------------------
	-             Prepare Material Buffer
	---------------------------------------------------------------------*/
	if (!PrepareMaterialBuffer()) { MessageBox(NULL, L" Material buffer cannot be prepared.", L"Warning", MB_ICONWARNING); return false; };
	
	/*-------------------------------------------------------------------
	-             Prepare Material Buffer
	---------------------------------------------------------------------*/
	if (!PrepareBoneBuffer()) { MessageBox(NULL, L" Bone buffer cannot be prepared.", L"Warning", MB_ICONWARNING); return false; };

	/*-------------------------------------------------------------------
	-             Prepare PMX Object
	---------------------------------------------------------------------*/
	if (!PreparePMXObject()) { MessageBox(NULL, L"PMXObject cannot be prepared.", L"Warning", MB_ICONWARNING); return false; };

	/*-------------------------------------------------------------------
	-             Prepare PMX Bone
	---------------------------------------------------------------------*/
	if (!PrepareBoneMap()) { MessageBox(NULL, L"BoneMap cannot be prepared.", L"Warning", MB_ICONWARNING); return false; };
	if (!PrepareBoneIK()) { MessageBox(NULL, L"BoneIK cannot be prepared.", L"Warning", MB_ICONWARNING); return false; };
	
	/*-------------------------------------------------------------------
	-             Prepare PMX Physics and RigidBody and Joint
	---------------------------------------------------------------------*/
	if (!PreparePhysics()) { MessageBox(NULL, L"BoneIK cannot be prepared.", L"Warning", MB_ICONWARNING); return false; }

	return true;
}

/****************************************************************************
*                       Update 
*************************************************************************//**
*  @fn        bool PMXModel::Update()
*  @brief     Update motion
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::Update()
{
	UpdateTotalAnimation();
	UpdateGPUData();
	return true;
}

void PMXModel::Finalize()
{
	/*-------------------------------------------------------------------
	-            Clear Map
	---------------------------------------------------------------------*/
	_boneMap.get()->clear();
	_motionData.clear();

	/*-------------------------------------------------------------------
	-            Clear vector 
	---------------------------------------------------------------------*/
	_threads                     .clear(); _threads                     .shrink_to_fit();
	_materialView                .clear(); _materialView                .shrink_to_fit();
	_boneMatrices         .get()->clear(); _boneMatrices         .get()->shrink_to_fit();
	_boneNodeAddress      .get()->clear(); _boneNodeAddress      .get()->shrink_to_fit();
	_sortedBoneNodeAddress.get()->clear(); _sortedBoneNodeAddress.get()->shrink_to_fit();
	_rootBoneNodeNames           .clear(); _rootBoneNodeNames           .shrink_to_fit();
	_boneIKs              .get()->clear(); _boneIKs              .get()->shrink_to_fit();
	_semiStandardBoneMap         .clear(); _semiStandardBoneMap         .shrink_to_fit();

	/*-------------------------------------------------------------------
	-            Release smart pointer (敢えて明示的に開放してます)
	---------------------------------------------------------------------*/
	_vertices             .reset();
	_boneMatrices         .reset();
	_bonePosition         .reset();
	_boneQuaternion       .reset();
	_boneMap              .reset();
	_boneNodeAddress      .reset();
	_sortedBoneNodeAddress.reset();
	_boneIKs.reset();
	_pmxData.reset();

	/*-------------------------------------------------------------------
	-            Release Buffer
	---------------------------------------------------------------------*/
	for (auto& vertexBuffer : _vertexBuffer) { vertexBuffer.get()->Resource()->Release(); }
	_materialBuffer     .get()->Resource()->Release();
	_boneBuffer          .get()->Resource()->Release();


}
/****************************************************************************
*                       Draw
*************************************************************************//**
*  @fn        bool PMDModel::Draw()
*  @brief     DrawPMDModel
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::Draw(SceneGPUAddress scene, LightGPUAddress light)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	_currentFrameIndex                        = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[_currentFrameIndex].VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _meshBuffer[_currentFrameIndex].IndexBufferView();

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->SetGraphicsRootConstantBufferView(0, _modelObject.get()->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, scene);
	commandList->SetGraphicsRootConstantBufferView(3, light);
	commandList->SetGraphicsRootConstantBufferView(4, _boneBuffer.get()->Resource()->GetGPUVirtualAddress());

	/*-------------------------------------------------------------------
	-               Drawing process for each material
	---------------------------------------------------------------------*/
	auto address = _materialBuffer.get()->Resource()->GetGPUVirtualAddress();
	UINT offset  = 0;
	for (int i = 0; i < (int)_pmxData->GetMaterialCount(); ++i)
	{
		commandList->SetGraphicsRootConstantBufferView(2, address);
		commandList->SetGraphicsRootDescriptorTable(5, _pmxData->GetTextureList(i).Texture.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(6, _pmxData->GetTextureList(i).SphereMultiply.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(7, _pmxData->GetTextureList(i).SphereAddition.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(8, _pmxData->GetTextureList(i).ToonTexture.GPUHandler);
		commandList->DrawIndexedInstanced(_pmxData.get()->GetIndexCountForMaterial(i), 1, offset, _meshBuffer[_currentFrameIndex].BaseVertexLocation, 0);
		offset  += _pmxData.get()->GetIndexCountForMaterial(i);
		address += CalcConstantBufferByteSize(sizeof(PBRMaterial));
	}

	
	return true;
}

/****************************************************************************
*                       AddMotion
*************************************************************************//**
*  @fn        bool PMXModel::AddMotion()
*  @brief     Add Motion Data
*  @param[in] const std::wstring& filePath
*  @param[in] const std::wstring& motionName
*  @return 　　bool
*****************************************************************************/
bool PMXModel::AddMotion(const std::wstring& filePath, const std::wstring& motionName)
{
	MotionLoader motionLoader;
	motionLoader.LoadMotion(filePath, &_motionData[motionName]);

	return true;
}

/****************************************************************************
*                       StartAnimation
*************************************************************************//**
*  @fn        bool PMDModel::StartAnimation()
*  @brief     Start Animation
*  @param[in] const std::wstring& filePath
*  @param[in] const std::wstring& motionName
*  @return 　　bool
*****************************************************************************/
bool PMXModel::StartAnimation(const std::wstring& motionName)
{
	if (_gameTimer == nullptr)
	{
		MessageBox(NULL, L"Please set a gameTimer. \n So, please call back SetWorldTimer.", L"Warning", MB_ICONWARNING);
		return false;
	}

	if (!_motionData.contains(motionName))
	{
		MessageBox(NULL, L" Couldn't find motion name.", L"Warning", MB_ICONWARNING);
		return false;
	}
	
	/*-------------------------------------------------------------------
	-			Set Initialize Pose
	---------------------------------------------------------------------*/
	_currentMotionName = motionName;
	for (auto& boneMotion : _motionData[motionName]->GetMotionMap())
	{
		std::string boneName  = boneMotion.first;
		auto iteratorBoneNode = _pmxData->FindBoneNode(boneName);

		if (iteratorBoneNode == _pmxData->GetBoneNodeTable().end())
		{
			continue;
		}

		PMXBoneNode* boneNode      = _pmxData->GetBoneNode(boneMotion.first);
		Vector3      basePosition = boneNode->GetTranslate();
		Matrix4      boneTransform = 
			  Translation(basePosition)
			* RotationQuaternion(boneMotion.second[0].Quaternion)
			* Translation(basePosition);
		_boneMatrices.get()->at(boneNode->GetBoneIndex()) = boneTransform;
	}
	PMXBoneNode::RecursiveBoneMatrixMultiply(*_boneMatrices, _pmxData->GetBoneNode("全ての親"), MatrixIdentity());
	WriteBoneParameterToBuffer();

	_isAnimation = true;
	_currentTime = 0;
	return true;
}
#pragma region Property
void PMXModel::SetWorldTimer(const GameTimer& gameTimer)
{
	this->_gameTimer = &gameTimer;
}

#pragma endregion Property
#pragma endregion Public Function

#pragma region Protected Function
#pragma region Prepare
/****************************************************************************
*                       PrepareVertexBuffer
*************************************************************************//**
*  @fn        bool PMDModel::PrepareVertexBuffer()
*  @brief     Prepare Vertex Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::PrepareVertexBuffer()
{
	/*-------------------------------------------------------------------
	-			Prepare variables
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check PMD Vertex is existed
	---------------------------------------------------------------------*/
	if (_pmxData->GetVertex() == nullptr) { ::OutputDebugString(L"Can't read vertex data (vertex)"); return false; }
	int vertexCount = (int)_pmxData->GetVertexCount();

	/*-------------------------------------------------------------------
	-			Build CPU and GPU Vertex Buffer
	---------------------------------------------------------------------*/
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		/*-------------------------------------------------------------------
		-			Copy PMD Vertex Data To Mesh Buffer
		---------------------------------------------------------------------*/
		_vertexBuffer[i] = std::make_unique<UploadBuffer<PMXVertex>>(directX12.GetDevice(), vertexCount, false);

		const UINT vbByteSize = (UINT)vertexCount * sizeof(PMXVertex);

		/*-------------------------------------------------------------------
		-			Build CPU / GPU Vertex Buffer
		---------------------------------------------------------------------*/
		_meshBuffer[i].BaseVertexLocation   = 0;
		_meshBuffer[i].VertexBufferGPU      = _vertexBuffer[i]->Resource();
		_meshBuffer[i].VertexByteStride     = sizeof(PMXVertex);
		_meshBuffer[i].VertexBufferByteSize = vbByteSize;
		if (FAILED(D3DCreateBlob(vbByteSize, &_meshBuffer[i].VertexBufferCPU)))
		{
			::OutputDebugString(L"Can't create blob data (vertex)");
			return false;
		}

	}

	std::unique_ptr<PMXVertex[]> vertices = std::make_unique<PMXVertex[]>(vertexCount);
	std::memcpy(vertices.get(), (void*)_pmxData->GetVertex(), vertexCount * sizeof(PMXVertex));
	
	_vertices = std::move(vertices);
	return true;
}

/****************************************************************************
*                       PrepareIndexBuffer
*************************************************************************//**
*  @fn        bool PMXModel::PrepareIndexBuffer()
*  @brief     Prepare Index Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::PrepareIndexBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check Index Data
	---------------------------------------------------------------------*/
	if (_pmxData->GetIndex() == nullptr) { ::OutputDebugString(L"Can't read index data (vertex)"); return false; }
	int indexCount = (int)_pmxData->GetIndexCount();

	/*-------------------------------------------------------------------
	-			Build CPU / GPU Index Buffer
	---------------------------------------------------------------------*/
	const UINT ibByteSize              = (UINT)indexCount * sizeof(UINT32);
	_meshBuffer[0].IndexBufferGPU      = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), _pmxData->GetIndex(), ibByteSize, _meshBuffer[0].IndexBufferUploader).Resource();
	_meshBuffer[0].IndexFormat         = DXGI_FORMAT_R32_UINT;
	_meshBuffer[0].IndexBufferByteSize = ibByteSize;
	_meshBuffer[0].IndexCount          = indexCount;
	if (FAILED(D3DCreateBlob(ibByteSize, &_meshBuffer[0].IndexBufferCPU)))
	{
		::OutputDebugString(L"Can't create blob data (index)");
		return false;
	}


	/*-------------------------------------------------------------------
	-		Copy the index buffer by the amount of the frame buffer.
	---------------------------------------------------------------------*/
	for (int i = 1; i < FRAME_BUFFER_COUNT; ++i)
	{
		_meshBuffer[i].IndexBufferCPU      = _meshBuffer[0].IndexBufferCPU;
		_meshBuffer[i].IndexBufferGPU      = _meshBuffer[0].IndexBufferGPU;
		_meshBuffer[i].IndexBufferUploader = _meshBuffer[0].IndexBufferUploader;
		_meshBuffer[i].IndexCount          = _meshBuffer[0].IndexCount;
		_meshBuffer[i].IndexFormat         = _meshBuffer[0].IndexFormat;
		_meshBuffer[i].StartIndexLocation  = _meshBuffer[0].StartIndexLocation;
		_meshBuffer[i].IndexBufferByteSize = _meshBuffer[0].IndexBufferByteSize;
	}
	return true;
}

/****************************************************************************
*                       PrepareMaterialBuffer
*************************************************************************//**
*  @fn        bool PMXModel::PrepareMaterialBuffer()
*  @brief     Prepare Index Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::PrepareMaterialBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check Material Data
	---------------------------------------------------------------------*/
	if (_pmxData->GetPBRMaterial() == nullptr) { ::OutputDebugString(L"Can't read material data"); return false; }
	int materialCount = (int)_pmxData->GetMaterialCount();

	/*-------------------------------------------------------------------
	-			Build Material Data
	---------------------------------------------------------------------*/
	_materialBuffer = std::make_unique<UploadBuffer<PBRMaterial>>(directX12.GetDevice(), materialCount, true);
	_materialBuffer->CopyStart();
	_materialBuffer->Resource()->SetName(L"PMXMaterial");
	for (int i = 0; i < materialCount; ++i)
	{
		_materialBuffer->CopyData(i, _pmxData->GetPBRMaterial()[i]);
	}
	_materialBuffer->CopyEnd();


	/*-------------------------------------------------------------------
	-			Create Constant Buffer View
	---------------------------------------------------------------------*/
	for (int i = 0; i < materialCount; ++i)
	{
		/*-------------------------------------------------------------------
		-			Create Constant Buffer View Descriptor
		---------------------------------------------------------------------*/
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = _materialBuffer.get()->Resource()->GetGPUVirtualAddress() + i * CalcConstantBufferByteSize(sizeof(PBRMaterial));
		cbvDesc.SizeInBytes    = CalcConstantBufferByteSize(sizeof(PBRMaterial));

		/*-------------------------------------------------------------------
		-			Create Constant Buffer View
		---------------------------------------------------------------------*/
		directX12.GetDevice()->CreateConstantBufferView(
			&cbvDesc, directX12.GetCPUResourceView(HeapType::CBV, directX12.IssueViewID(HeapType::CBV)));
	}

	return true;
}

/****************************************************************************
*                       PreparePMXObject
*************************************************************************//**
*  @fn        bool PMDModel::PreparePMXObject()
*  @brief     Prepare PMX Object
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::PreparePMXObject()
{
	/*-------------------------------------------------------------------
	-			Prepare variables
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	Device* device       = directX12.GetDevice();
	auto modelObject     = std::make_unique<UploadBuffer<ObjectConstants>>(device, 1, true);

	/*-------------------------------------------------------------------
	-			Set Modeldata
	---------------------------------------------------------------------*/
	_worldInfo.World            = gm::MatrixIdentityF();
	_worldInfo.TextureConfig    = 0;
	_worldInfo.Padding[0] = 0;
	_worldInfo.Padding[1] = 0;

	/*-------------------------------------------------------------------
	-			Copy Model object data
	---------------------------------------------------------------------*/
	modelObject->CopyStart();
	modelObject->CopyData(0, _worldInfo);
	modelObject->CopyEnd();

	/*-------------------------------------------------------------------
	-			Move Model object data
	---------------------------------------------------------------------*/
	_modelObject = std::move(modelObject);
	return true;
}

/****************************************************************************
*                       PrepareBoneBuffer
*************************************************************************//**
*  @fn        bool PMDModel::PrepareBoneBuffer()
*  @brief     Prepare Bone Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::PrepareBoneBuffer()
{
	/*-------------------------------------------------------------------
	-			Initialize bone matrix
	---------------------------------------------------------------------*/
	std::vector<gm::Matrix4> boneMatrices;
	boneMatrices.resize(_pmxData->GetBoneCount());
	std::fill(boneMatrices.begin(), boneMatrices.end(), gm::MatrixIdentity());
	
	/*-------------------------------------------------------------------
	-			Prepare variables 
	---------------------------------------------------------------------*/
	auto& directX12   = DirectX12::Instance();
	Device* device    = directX12.GetDevice();
	auto boneObject   = std::make_unique<UploadBuffer<PMXBoneParameter>>(device, 1, true);
	
	PMXBoneParameter* boneParameter = new PMXBoneParameter();
	std::copy(boneMatrices.begin(), boneMatrices.end(), boneParameter->BoneMatrices);
	_boneMatrices = std::make_unique<std::vector<gm::Matrix4>>(boneMatrices);

	/*-------------------------------------------------------------------
	-			Copy bone object
	---------------------------------------------------------------------*/
	boneObject->CopyStart();
	boneObject->Resource()->SetName(L"Bone");
	boneObject->CopyData(0, *boneParameter);
	boneObject->CopyEnd();
	
	/*-------------------------------------------------------------------
	-			Move bone buffer
	---------------------------------------------------------------------*/
	_boneBuffer = std::move(boneObject);

	/*-------------------------------------------------------------------
	-			Create Constant Buffer View
	---------------------------------------------------------------------*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = _boneBuffer.get()->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes    = CalcConstantBufferByteSize(sizeof(PMXBoneParameter));
	
	device->CreateConstantBufferView(
		&cbvDesc, directX12.GetCPUResourceView(HeapType::CBV, directX12.IssueViewID(HeapType::CBV)));
	
	delete boneParameter;
	return true;
}
/****************************************************************************
*                       PrepareBoneMap
*************************************************************************//**
*  @fn        bool PMDModel::PrepareBoneMap()
*  @brief     Prepare Bone Map
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::PrepareBoneMap()
{
	/*-------------------------------------------------------------------
	-			Get Bone Map
	---------------------------------------------------------------------*/
	std::unique_ptr<std::map<std::string, PMXBoneNode>> bones    = std::make_unique<std::map<std::string, PMXBoneNode>>(_pmxData->CopyBoneNodeTable());
	std::unique_ptr<std::vector<PMXBoneNode*>> nodeAddress       = std::make_unique<std::vector<PMXBoneNode*>>(_pmxData->GetBoneCount());
	std::unique_ptr<std::vector<PMXBoneNode*>> sortedNodeAddress = std::make_unique<std::vector<PMXBoneNode*>>(_pmxData->GetBoneCount());
	for (int i = 0; i < _pmxData->GetBoneCount(); ++i)
	{
		auto& boneName = _pmxData->GetBoneNames()[i];
		auto& bone     = bones->at(boneName);

		/*-------------------------------------------------------------------
		-			parent
		---------------------------------------------------------------------*/
		if (bone.GetParent() != nullptr)
		{
			auto parentIndex = bone.GetParent()->GetBoneIndex();
			auto parentName  = _pmxData->GetBoneNames()[parentIndex];
			bone.SetParent(&bones->at(parentName));
		}

		/*-------------------------------------------------------------------
		-			child
		---------------------------------------------------------------------*/
		if (!bone.GetChildren().empty())
		{
			int count = 0;
			for (auto& child : bone.GetChildren())
			{
				auto childIndex = child->GetBoneIndex();
				auto childName  = _pmxData->GetBoneNames()[childIndex];
				bone.SetChild(&bones->at(childName), count);
				count++;
			}
		}

		/*-------------------------------------------------------------------
		-			append
		---------------------------------------------------------------------*/
		if (bone.GetAppendNode() != nullptr)
		{
			auto appendIndex = bone.GetParent()->GetBoneIndex();
			auto appendName  = _pmxData->GetBoneNames()[appendIndex];
			bone.SetAppendNode(&bones->at(appendName));
		}

		/*-------------------------------------------------------------------
		-			semi default bone 
		---------------------------------------------------------------------*/
		if (bones.get()->find(bone.GetBoneName() + "D") != bones.get()->end())
		{
			std::pair<int, int> pair;
			pair.first  = bones.get()->at(bone.GetBoneName() + "D").GetBoneIndex(); // "D"bone
			pair.second = bone.GetBoneIndex(); // non "D"
			_semiStandardBoneMap.push_back(pair);
		}

		/*-------------------------------------------------------------------
		-			ex bone setting
		---------------------------------------------------------------------*/
		if (bone.GetBoneName() == "右足先EX" || bone.GetBoneName() == "左足先EX")
		{
			//bone.EnableIK(false);
		}

		nodeAddress->at(i) = &bone;
		sortedNodeAddress->at(i) = &bone;
	}


	/*-------------------------------------------------------------------
	-			Sorted bone node address
	---------------------------------------------------------------------*/
	std::stable_sort(
		sortedNodeAddress.get()->begin(),
		sortedNodeAddress.get()->end(),
		[](PMXBoneNode* x, PMXBoneNode* y) { return x->GetDeformationDepth() < y->GetDeformationDepth(); }
	);

	_boneMap               = std::move(bones);
	_boneNodeAddress       = std::move(nodeAddress);
	_sortedBoneNodeAddress = std::move(sortedNodeAddress);

	for (const auto& node : *_sortedBoneNodeAddress.get())
	{
		if (node->GetParent() == nullptr)
		{
			_rootBoneNodeNames.push_back(node->GetBoneName());
		}
	}
	
	return true;
}
bool PMXModel::PrepareBoneIK()
{
	std::unique_ptr<std::vector<PMXBoneIK>> boneIK = std::make_unique<std::vector<PMXBoneIK>>(_pmxData->GetBoneIKCount());
	/*-------------------------------------------------------------------
	-			SaveIK
	---------------------------------------------------------------------*/
	for (int i = 0; i < boneIK.get()->size(); ++i)
	{
		auto  pmxFileIK   = _pmxData->GetBoneIK()[i]; // ikbone (in pmx file)
		auto& thisModelIK = boneIK.get()->at(i);
		std::string ikName = pmxFileIK.GetIKBoneNode()->GetBoneName();
		
		// ①　Dボーンが存在する場合, Dボーンを子とターゲットに設定する.
		// ②　IKボーンに対し, 変更前のEnableIKをfalse, 変更後をtrueに変更する. 
		std::string targetName = pmxFileIK.GetTargetBoneNode()->GetBoneName();
		for (auto& semiStandard : _semiStandardBoneMap)
		{
			if (semiStandard.second == _boneMap.get()->at(targetName).GetBoneIndex())
			{
				targetName = targetName + "D";
				_boneMap.get()->at(targetName).EnableIK(true);
				break;
			}
		}

		thisModelIK.SetIKBone    (&_boneMap.get()->at(ikName));
		thisModelIK.SetTargetBone(&_boneMap.get()->at(targetName));
		thisModelIK.SetIterationCount(pmxFileIK.GetIterationCount());
		thisModelIK.SetLimitAngle(pmxFileIK.GetAngleLimit());

		// save parent 
		if (pmxFileIK.GetIKParentBoneNode() != nullptr)
		{
			thisModelIK.SetIKParentBone(&_boneMap.get()->at(pmxFileIK.GetIKParentBoneNode()->GetBoneName()));
		}
		
		// save children
		std::vector<PMXIKChain> tempIK;
		tempIK.resize(pmxFileIK.GetChainLength());
		for (int j = 0; j < pmxFileIK.GetChainLength(); ++j)
		{
			std::string childName = pmxFileIK.GetChains()[j].IKBone->GetBoneName();
			for (auto& semiStandard : _semiStandardBoneMap)
			{
				if (semiStandard.second == _boneMap.get()->at(childName).GetBoneIndex())
				{
					childName = childName + "D";
					_boneMap.get()->at(childName).EnableIK(true);
					break;
				}
			}

			tempIK[j].IKBone      = &_boneMap.get()->at(childName);
			tempIK[j].EnableLimit = pmxFileIK.GetChains()[j].EnableLimit;
			tempIK[j].AngleMax    = pmxFileIK.GetChains()[j].AngleMax;
			tempIK[j].AngleMin    = pmxFileIK.GetChains()[j].AngleMin;
		}
		
		thisModelIK.SetIKChains(tempIK);
		
		if (ikName == "右つま先ＩＫ")
		{
			if (_boneMap.get()->find("右足先EX") != _boneMap.get()->end())
			{
				thisModelIK.InsertIKChain(0, &_boneMap.get()->at("右足先EX"), false, Float3(0,0,0), Float3(0,0,0));
			}
		}
		else if (ikName == "左つま先ＩＫ")
		{
			if (_boneMap.get()->find("左足先EX") != _boneMap.get()->end())
			{
				thisModelIK.InsertIKChain(0, &_boneMap.get()->at("左足先EX"), false, Float3(0, 0, 0), Float3(0, 0, 0));
			}
		}
	}

	for (int i = 0; i < boneIK.get()->size(); ++i)
	{
		auto   boneName = boneIK.get()->at(i).GetIKName();
		auto&  boneNode = _boneMap.get()->at(boneName);
		boneNode.SetBoneIK(&boneIK.get()->at(i));
	}

	_boneIKs = std::move(boneIK);
	return true;
}
void PMXModel::WriteBoneParameterToBuffer()
{
	PMXBoneParameter* boneParameter = new PMXBoneParameter();
	std::copy(_boneMatrices.get()->begin(), _boneMatrices.get()->end(), boneParameter->BoneMatrices);

	auto boneObject = _boneBuffer.get();
	boneObject->CopyStart();
	boneObject->CopyData(0, *boneParameter);
	boneObject->CopyEnd();

	delete boneParameter;
}
/****************************************************************************
*                       PreparePhysics
*************************************************************************//**
*  @fn        bool PMXModel::PreparePhysics()
*  @brief     Prepare Physics
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::PreparePhysics()
{
	if (!_physicsManager.Create()) { return false; }

	for (const auto& pmxRigidBody : GetPMXData()->GetRigidBodyList())
	{
		auto         rigidBody = _physicsManager.AddRigidBody();
		PMXBoneNode* node      = nullptr;
		if (pmxRigidBody.BoneIndex != -1)
		{
			node = _boneNodeAddress.get()->at(pmxRigidBody.BoneIndex);
		}
		if (!rigidBody->Create(pmxRigidBody, this, node))
		{
			return false;
		}
		_physicsManager.GetMMDPhysics()->AddRigidBody(rigidBody);
	}

	for (const auto& pmxJoint : GetPMXData()->GetJointList())
	{
		if (pmxJoint.RigidBodyIndex_A != -1 && pmxJoint.RigidBodyIndex_B != -1 && pmxJoint.RigidBodyIndex_A != pmxJoint.RigidBodyIndex_B)
		{
			auto         joint = _physicsManager.AddJoint();
			PMXBoneNode* node  = nullptr;
			auto rigidBodies   = _physicsManager.GetRigidBodies();
			bool result = joint->CreateJoint(pmxJoint, (*rigidBodies)[pmxJoint.RigidBodyIndex_A].get(), (*rigidBodies)[pmxJoint.RigidBodyIndex_B].get());
			if (!result)
			{
				return false;
			}
			_physicsManager.GetMMDPhysics()->AddJoint(joint);
		}
		
	}

	ResetPhysics();

	return true;
}

#pragma endregion Prepare
#pragma region Update 
/****************************************************************************
*                       UpdateBoneMatrices
*************************************************************************//**
*  @fn        bool PMXModel::UpdateBoneMatrices()
*  @brief     Update Bone matrix
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXModel::UpdateBoneMatrices()
{
	int count = 0;

	for (auto& bone : *_boneNodeAddress.get())
	{
		auto inverseBindMatrix = bone->GetInverseBindMatrix();
		auto globalMatrix      = bone->GetGlobalMatrix();
		_boneMatrices.get()->at(count) = inverseBindMatrix * globalMatrix;
		count++;
	}

}

/****************************************************************************
*                       UpdateNodeAnimation
*************************************************************************//**
*  @fn        bool PMXModel::UpdateNodeAnimation()
*  @brief     Update bone node animation
*  @param[in] bool isAfterPhisics
*  @param[in] int frameNo
*  @return 　　void
*****************************************************************************/
void PMXModel::UpdateNodeAnimation(bool isAfterPhysics, int frameNo)
{
	auto sortedBoneNodeAddress = _sortedBoneNodeAddress.get();

	for (auto bone : *sortedBoneNodeAddress)
	{
		if (bone->IsDeformationAfterPhysics() != isAfterPhysics) { continue; }
		bone->UpdateLocalMatrix();
	}

	for (auto bone : *sortedBoneNodeAddress)
	{
		if (bone->IsDeformationAfterPhysics() != isAfterPhysics) { continue; }
		if (bone->GetParent() == nullptr)
		{
			bone->UpdateSelfandChildMatrix();
		}
	}

	for (auto bone : *sortedBoneNodeAddress)
	{
		if (bone->IsDeformationAfterPhysics() != isAfterPhysics) { continue; }

		if (bone->GetAppendNode() != nullptr)
		{
			//bone->UpdateAppendMatrix();
			//bone->UpdateSelfandChildMatrix();
		}

		if (bone->GetBoneIK() != nullptr)
		{
			bone->GetBoneIK()->SolveIK(frameNo);
			bone->UpdateSelfandChildMatrix();	
		}
		
	}

	for (auto bone : *sortedBoneNodeAddress)
	{
		if (bone->IsDeformationAfterPhysics() != isAfterPhysics){ continue; }

		if (bone->GetParent() == nullptr)
		{
			bone->UpdateSelfandChildMatrix();
		}
	}
}
/****************************************************************************
*                       UpdatePhysicsAnimation
*************************************************************************//**
*  @fn        void PMXModel::UpdatePhysicsAnimation(float deltaTime)
*  @brief     Update physics animation
*  @param[in] float deltaTime
*  @return 　　void
*****************************************************************************/
void PMXModel::UpdatePhysicsAnimation(float deltaTime)
{
	PMXPhysicsManager* physicsManager = GetPMXPhysicsManager();
	MMDPhysics*        physics        = physicsManager->GetMMDPhysics();
	if (physics == nullptr) { return; }

	/*-------------------------------------------------------------------
	-                    Activate rigidBody
	---------------------------------------------------------------------*/
	auto rigidBodies = physicsManager->GetRigidBodies();
	for (auto& rigidBody : *rigidBodies)
	{
		rigidBody->SetActivation(true);
	}

	/*-------------------------------------------------------------------
	-                    Update physics
	---------------------------------------------------------------------*/
	physics->Update(deltaTime);

	for (auto& rigidBody : *rigidBodies)
	{
		rigidBody->ReflectGlobalMatrix();
	}
	for (auto& rigidBodies : *rigidBodies)
	{
		rigidBodies->CalculateLocalMatrix();
	}
	for (auto name : _rootBoneNodeNames)
	{
		_boneMap.get()->at(name).UpdateSelfandChildMatrix();
	}
}

/****************************************************************************
*                       UpdateGPUData
*************************************************************************//**
*  @fn        bool PMXModel::UpdateGPUData()
*  @brief     Copy to GPU Constant Buffer Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::UpdateGPUData()
{
	/*-------------------------------------------------------------------
	-               Map Model Object
	---------------------------------------------------------------------*/
	_worldInfo.World = _transform.GetFloat4x4();
	auto modelObject = _modelObject.get();
	modelObject->CopyStart();
	modelObject->CopyData(0, _worldInfo);
	modelObject->CopyEnd();

	/*-------------------------------------------------------------------
	-               Map Bone Matrix
	---------------------------------------------------------------------*/
	WriteBoneParameterToBuffer();

	auto morphVertex = _vertices.get();
	auto vertexBuffer = _vertexBuffer[_currentFrameIndex].get();
	vertexBuffer->CopyStart();
	vertexBuffer->CopyTotalData(morphVertex, (int)_pmxData->GetVertexCount());
	vertexBuffer->CopyEnd();
	return true;
}

/****************************************************************************
*                       UpdateTotalAnimation
*************************************************************************//**
*  @fn        void PMXModel::UpdateTotalAnimation()
*  @brief     Update morph and motion animation
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXModel::UpdateTotalAnimation()
{
	if (!_isAnimation) { return; }

	_currentTime  += _gameTimer->DeltaTime(); // [s]
	UINT32 frameNo = CalculateAnimationFrameNo();
	UpdateMorph (frameNo);
	UpdateMotion(frameNo);
}


bool PMXModel::UpdateMorph(UINT32 frameNo)
{
	if (!_motionData[_currentMotionName]->IsExistedMorphingMap()) { return false; }
	SetUpParallelUpdate();

	std::chrono::system_clock::time_point start, end;
	start = std::chrono::system_clock::now();
	/*-------------------------------------------------------------------
	-               Update Motion Data
	---------------------------------------------------------------------*/
	auto vertices = _vertices.get();
	std::memcpy(vertices, (void*)_pmxData->GetVertex(), _pmxData->GetVertexCount() * sizeof(PMXVertex));

	for (auto& morphMotion : _motionData[_currentMotionName]->GetMorphingMap())
	{
		std::string morphName = morphMotion.first;
		auto iteratorMorph = _pmxData->FindMorph(morphName);

		if (iteratorMorph == _pmxData->GetMorphingMap().end())
		{
			continue;
		}

		auto& morph = iteratorMorph->second;
		std::vector<VMDKeyFrameMorph> keyFrames = morphMotion.second;

		// For each motion, select the obe closest to the desired frame number
		auto reverseIterator = std::find_if(keyFrames.rbegin(), keyFrames.rend(),
			[frameNo](const VMDKeyFrameMorph& keyFrame)
		{
			return keyFrame.Frame <= frameNo;
		});
		if (reverseIterator == keyFrames.rend()) { continue; } //If there's no match, skip it.

		/*-------------------------------------------------------------------
		-               Calculate Morphing
		---------------------------------------------------------------------*/
		auto iterator = reverseIterator.base();
		if (iterator != keyFrames.end())
		{
			float t = static_cast<float>(frameNo - reverseIterator->Frame) / static_cast<float>(iterator->Frame - reverseIterator->Frame);
			t = Lerp(reverseIterator->Weight, iterator->Weight, t);

			for (int i = 0; i < iteratorMorph->second.PositionMorphs.size(); ++i)
			{
				auto index        = iteratorMorph->second.PositionMorphs[i].VertexIndex;
				auto& morphVertex = iteratorMorph->second.PositionMorphs[i].Position;

				Vector3 temp = Vector3(vertices[index].Vertex.Position) + Vector3(morphVertex) * t;
				vertices[index].Vertex.Position = temp.ToFloat3();
			}

		}
		else
		{
			float t = reverseIterator->Weight;

			size_t numVertexCount = iteratorMorph->second.PositionMorphs.size() / _parallelUpdateCount;
			std::vector<UpdateRange> ranges(_parallelUpdateCount);
			// calculate vertex offset
			int count = 0;
			for (auto& range : ranges)
			{
				range.VertexCount = numVertexCount;
				range.VertexOffset = numVertexCount * count;
				count++;
			}


			// multi thread vertex morph
			for (int i = 0; i < _threads.size(); ++i)
			{
				_threads[i] = std::thread([i, &ranges, &iteratorMorph, t, &vertices]
				{
					for (int j = 0; j < ranges[i].VertexCount; ++j)
					{
						auto index = iteratorMorph->second.PositionMorphs[j + ranges[i].VertexOffset].VertexIndex;
						auto& morphVertex = iteratorMorph->second.PositionMorphs[j + ranges[i].VertexOffset].Position;
						Vector3 temp = Vector3(morphVertex) * t + Vector3(vertices[index].Vertex.Position);
						vertices[index].Vertex.Position = temp.ToFloat3();
					}
				});
			}

			// single threads
			/*for (int i = 0; i < iteratorMorph->second.PositionMorphs.size(); ++i)
			{
				auto index = iteratorMorph->second.PositionMorphs[i].VertexIndex;
				auto& morphVertex = iteratorMorph->second.PositionMorphs[i].Position;
				Vector3 temp = Vector3(morphVertex) * t + Vector3(vertices[index].Vertex.Position);
				vertices[index].Vertex.Position = temp.ToFloat3();
			}*/

			for (auto& thread : _threads) { thread.join(); }
		}

	}

	return true;
}

void PMXModel::UpdateBoneNodeTransform(UINT32 frameNo)
{
	auto boneMap = _boneMap.get();

	/*-------------------------------------------------------------------
	-               Update Motion Data
	---------------------------------------------------------------------*/
	for (auto& boneMotion : _motionData[_currentMotionName]->GetMotionMap())
	{
		std::string              boneName = boneMotion.first;
		std::vector<VMDKeyFrame> keyFrames = boneMotion.second;

		// Find bone node
		const auto iteratorBoneNode = boneMap->find(boneName);
		if (iteratorBoneNode == boneMap->end()) { continue; }

		/*-------------------------------------------------------------------
		-     Find bone node
		---------------------------------------------------------------------*/
		auto& boneNode = iteratorBoneNode->second;

		/*-------------------------------------------------------------------
		-      For each motion, select the closest to the desired frame number
		---------------------------------------------------------------------*/
		auto reverseIterator = std::find_if(keyFrames.rbegin(), keyFrames.rend(),
			[frameNo](const VMDKeyFrame& keyFrame)
		{
			return keyFrame.Frame <= frameNo;
		});
		if (reverseIterator == keyFrames.rend()) { continue; } //If there's no match, skip it.

		/*-------------------------------------------------------------------
		-               Calculate rotation
		---------------------------------------------------------------------*/
		boneNode.LoadInitialSRT();
		Quaternion rotation = reverseIterator->Quaternion;
		Vector3 offset = reverseIterator->Location;
		auto    iterator = reverseIterator.base();

		if (iterator != keyFrames.end())
		{
			Vector3  translationRatio;
			float    rotationRatio;
			float t = static_cast<float>(frameNo - reverseIterator->Frame) / static_cast<float>(iterator->Frame - reverseIterator->Frame);
			VMDBezier::EasingKeyFrame(t, *reverseIterator, rotationRatio, translationRatio);
			rotation = Slerp(reverseIterator->Quaternion, iterator->Quaternion, rotationRatio);
			offset = Lerp(offset, iterator->Location, translationRatio);
		}

		/*-------------------------------------------------------------------
		-               Bone Transformation
		---------------------------------------------------------------------*/
		boneNode.SetAnimateRotate(rotation);
		boneNode.SetAnimateTranslate(offset);

	}
}
bool PMXModel::UpdateMotion(UINT32 frameNo)
{
	if (!_isAnimation) { return false; }

	ClearBoneMatrices();
	UpdateBoneNodeTransform(frameNo); // set the bone node of the target frame.
	UpdateNodeAnimation(false);
	//UpdatePhysicsAnimation(_gameTimer->DeltaTime());
	UpdateNodeAnimation(true);
	UpdateBoneMatrices();

	return true;
}

/****************************************************************************
*                       CalculateAnimationFrameNo
*************************************************************************//**
*  @fn        UINT32 PMXModel::CalculateAnimationFrameNo()
*  @brief     Calculate current animation frame number 
*  @param[in] void
*  @return 　　UINT32
*****************************************************************************/
UINT32 PMXModel::CalculateAnimationFrameNo()
{
	UINT32 frameNo = static_cast<UINT>(PMX_FRAME_PER_SECOND * _currentTime);

	if (frameNo > _motionData[_currentMotionName]->GetAnimationDuration())
	{
		frameNo = 0;
		_currentTime = 0;
	}
	return frameNo;
}

void PMXModel::SetUpParallelUpdate()
{
	if (_parallelUpdateCount == 0) 
	{ 
		_parallelUpdateCount = std::thread::hardware_concurrency();
	}

	size_t maxParallelCount = max(size_t(16), size_t(std::thread::hardware_concurrency()));
	if (_parallelUpdateCount > maxParallelCount) { _parallelUpdateCount = 16; }

}
#pragma endregion Update
#pragma region Clear
void PMXModel::ClearBoneMatrices()
{
	std::fill(_boneMatrices.get()->begin(), _boneMatrices.get()->end(), MatrixIdentity());
	for (auto& bone : *_sortedBoneNodeAddress.get())
	{
		bone->BeforeUpdateMatrix();
	}
}
/****************************************************************************
*                       ResetPhysics
*************************************************************************//**
*  @fn        void PMXModel::ResetPhysics()
*  @brief     Reset physics
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXModel::ResetPhysics()
{
	/*-------------------------------------------------------------------
	-             Prepare physics manager
	---------------------------------------------------------------------*/
	PMXPhysicsManager* physicsManager = GetPMXPhysicsManager();
	MMDPhysics*        physics        = physicsManager->GetMMDPhysics();
	if (physics == nullptr) { return; }

	/*-------------------------------------------------------------------
	-             Reset RigidBody
	---------------------------------------------------------------------*/
	auto rigidBodies = physicsManager->GetRigidBodies();
	for (auto& rigidBody : *rigidBodies)
	{
		rigidBody->SetActivation(false);
		rigidBody->ResetTransform();
	}

	/*-------------------------------------------------------------------
	-             Update physics
	---------------------------------------------------------------------*/
	physics->Update(1.0f / FIXED_FRAME_RATE);

	for (auto& rigidBody : *rigidBodies)
	{
		rigidBody->ReflectGlobalMatrix();
	}
	
	for (auto& rigidBody : *rigidBodies)
	{
		rigidBody->CalculateLocalMatrix();
	}

	for (auto& name : _rootBoneNodeNames)
	{
		_boneMap.get()->at(name).UpdateSelfandChildMatrix();
	}

	for (auto& rigidBody : *rigidBodies)
	{
		rigidBody->Reset(physics);
	}
}
#pragma endregion Clear
#pragma endregion Protected Function

#pragma region  Physics Manager
PMXPhysicsManager::PMXPhysicsManager() {};
PMXPhysicsManager::~PMXPhysicsManager()
{
	for (auto& joint : _joints)
	{
		_mmdPhysics->RemoveJoint(joint.get());
	}
	_joints.clear();

	for (auto& rigidBody : _rigidBodies)
	{
		_mmdPhysics->RemoveRigidBody(rigidBody.get());
	}
	_rigidBodies.clear();
	_mmdPhysics.reset();
}
/****************************************************************************
*                       Create
*************************************************************************//**
*  @fn        bool PMXPhysicsManager::Create()
*  @brief     Create MMDPhysics
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXPhysicsManager::Create()
{
	_mmdPhysics = std::make_unique<MMDPhysics>();
	return _mmdPhysics->Create();
}
/****************************************************************************
*                      GetMMDPhysics
*************************************************************************//**
*  @fn        MMDPhysics* PMXPhysicsManager::GetMMDPhysics()
*  @brief     Get MMDPhysics pointer
*  @param[in] void
*  @return 　　MMDPhysics*
*****************************************************************************/
MMDPhysics* PMXPhysicsManager::GetMMDPhysics()
{
	return _mmdPhysics.get();
}
/****************************************************************************
*                      AddRigidBody
*************************************************************************//**
*  @fn        PMXRigidBody* PMXPhysicsManager::AddRigidBody()
*  @brief     Add rigidBody
*  @param[in] void
*  @return 　　PMXRigidBody*
*****************************************************************************/
PMXRigidBody* PMXPhysicsManager::AddRigidBody()
{
	assert(_mmdPhysics != nullptr);
	auto rigidBody = std::make_unique<PMXRigidBody>();
	auto result    = rigidBody.get();
	_rigidBodies.emplace_back(std::move(rigidBody));

	return result;
}
/****************************************************************************
*                      AddJoint
*************************************************************************//**
*  @fn        PMXJoint* PMXPhysicsManager::AddJoint()
*  @brief     Add joint
*  @param[in] void
*  @return 　　PMXJoint*
*****************************************************************************/
PMXJoint* PMXPhysicsManager::AddJoint()
{
	assert(_mmdPhysics != nullptr);
	auto joint  = std::make_unique<PMXJoint>();
	auto result = joint.get();
	_joints.emplace_back(std::move(joint));

	return result;
}
#pragma endregion Physics Manager