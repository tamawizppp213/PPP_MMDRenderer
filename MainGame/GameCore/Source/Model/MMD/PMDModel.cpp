//////////////////////////////////////////////////////////////////////////////////
///             @file   PMDModel.cpp
///             @brief  PMDModel
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/MMD/PMDModel.hpp"
#include "GameCore/Include/Model/ModelPipelineState.hpp"
#include "GameCore/Include/Model/ModelLoader.hpp"
#include "GameCore/Include/Model/MotionLoader.hpp"
#include "GameCore/Include/Model/MMD/VMDAnimation.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include <d3dcompiler.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define PMD_FRAME_PER_SECOND 30
using namespace gm;
namespace pmd
{
	Texture g_whiteTexture;
}

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        bool PMDModel::Initialize()
*  @brief     Initialize (Load PMDData ...)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDModel::Initialize(const std::wstring& filePath)
{
	/*-------------------------------------------------------------------
	-             Load PMD Data
	---------------------------------------------------------------------*/
	ModelPSOManager& modelManager = ModelPSOManager::Instance();
	ModelLoader modelLoader;
	modelLoader.Load3DModel(filePath, &_pmdData);

	/*-------------------------------------------------------------------
	-             Prepare Vertex Buffer
	---------------------------------------------------------------------*/
	if (!PrepareVertexBuffer()) { MessageBox(NULL, L" Vertex buffer cannot be prepared.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Prepare Index Buffer
	---------------------------------------------------------------------*/
	if (!PrepareIndexBuffer()) { MessageBox(NULL, L" Index buffer cannot be prepared.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Prepare Material Buffer
	---------------------------------------------------------------------*/
	if (!PrepareMaterialBuffer()) { MessageBox(NULL, L" Material buffer cannot be prepared.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Prepare PMDObject
	---------------------------------------------------------------------*/
	if (!PrepareBoneBuffer()) { MessageBox(NULL, L"Bone Buffer cannot be prepared.", L"Warning", MB_ICONWARNING); };
	/*-------------------------------------------------------------------
	-             Prepare PMDObject
	---------------------------------------------------------------------*/
	if (!PreparePMDObject())      { MessageBox(NULL, L"PMDObject cannot be prepared.", L"Warning", MB_ICONWARNING); };

	return true;
}

bool PMDModel::Update()
{

	return true;
}


bool PMDModel::UpdateMotion()
{
	/*-------------------------------------------------------------------
	-               Calculate Frame Number
	---------------------------------------------------------------------*/
	_currentTime += _gameTimer->DeltaTime(); // [s]

	UINT32 frameNo = static_cast<UINT32>(PMD_FRAME_PER_SECOND * _currentTime);

	if (frameNo > _motionData[_currentMotionName]->GetAnimationDuration())
	{
		frameNo      = 0;
		_currentTime = 0;
	}
	/*-------------------------------------------------------------------
	-               Clear Matrix
	---------------------------------------------------------------------*/
	std::fill(_boneMatrices.get()->begin(), _boneMatrices.get()->end(), MatrixIdentity());

	/*-------------------------------------------------------------------
	-               Update Motion Data
	---------------------------------------------------------------------*/
	for (auto& boneMotion : _motionData[_currentMotionName]->GetMotionMap())
	{
		std::string boneName  = boneMotion.first;
		auto iteratorBoneNode = _pmdData->FindBoneNode(boneName);

		if (iteratorBoneNode == _pmdData->GetBoneNodeTable().end())
		{
			continue;
		}

		PMDBoneNode boneNode               = iteratorBoneNode->second;
		std::vector<VMDKeyFrame> keyFrames = boneMotion.second;

		// For each motion, select the obe closest to the desired frame number
		auto reverseIterator = std::find_if(keyFrames.rbegin(), keyFrames.rend(),
			[frameNo](const VMDKeyFrame& keyFrame)
		{
			return keyFrame.Frame <= frameNo;
		});
		if (reverseIterator == keyFrames.rend()) { continue; } //If there's no match, skip it.

		/*-------------------------------------------------------------------
		-               Calculate rotation
		---------------------------------------------------------------------*/
		Matrix4 rotation = MatrixIdentity();
		Vector3 offset   = reverseIterator->Location;
		auto iterator    = reverseIterator.base();
		if (iterator != keyFrames.end())
		{
			float t  = static_cast<float>(frameNo - reverseIterator->Frame) / static_cast<float>(iterator->Frame - reverseIterator->Frame);
			t        = VMDBezier::GetYFromXOnBezier(t, iterator->ControlPointForBezier[3].ToFloat4(), 12);
			Vector3  bezier;
			bezier.SetX(VMDBezier::GetYFromXOnBezier(t, iterator->ControlPointForBezier[0].ToFloat4(), 12));
			bezier.SetY(VMDBezier::GetYFromXOnBezier(t, iterator->ControlPointForBezier[1].ToFloat4(), 12));
			bezier.SetZ(VMDBezier::GetYFromXOnBezier(t, iterator->ControlPointForBezier[2].ToFloat4(), 12));

			rotation = RotationQuaternion(Slerp(reverseIterator->Quaternion, iterator->Quaternion, t));
			offset   = offset +  bezier * (iterator->Location - offset);
		}
		else
		{
			rotation = RotationQuaternion(reverseIterator->Quaternion);
		}

		/*-------------------------------------------------------------------
		-               Bone Transformation
		---------------------------------------------------------------------*/
		Float3  basePosition  = boneNode.GetBasePosition();
		Matrix4 boneTransform =
			Translation(-basePosition.x, -basePosition.y, -basePosition.z)
			* rotation
			* Translation(basePosition.x, basePosition.y, basePosition.z);
		_boneMatrices.get()->at(boneNode.GetBoneIndex()) = boneTransform * Translation(offset);
	}
	PMDBoneNode::RecursiveBoneMatrixMultiply(*_boneMatrices, _pmdData->GetBoneNode("センター"), MatrixIdentity());
	PMDBoneIK::SolveIK(frameNo, _pmdData.get(), *_boneMatrices, _motionData[_currentMotionName].get());
	return true;
}
/****************************************************************************
*                       Draw
*************************************************************************//**
*  @fn        bool PMDModel::Draw()
*  @brief     DrawPMDModel
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDModel::Draw(SceneGPUAddress scene, LightGPUAddress light)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	ModelPSOManager& modelManager             = ModelPSOManager::Instance();
	int currentFrameIndex                     = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[currentFrameIndex].VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _meshBuffer[currentFrameIndex].IndexBufferView();


	/*-------------------------------------------------------------------
	-               Update matrix
	---------------------------------------------------------------------*/

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(modelManager.GetRootSignature(ModelType::PMD));
	commandList->SetPipelineState(modelManager.GetPipelineState(ModelType::PMD));
	ID3D12DescriptorHeap* heapList[] = { directX12.GetCbvSrvUavHeap() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
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
	for (int i = 0; i < (int)_pmdData->GetMaterialCount(); ++i)
	{
		commandList->SetGraphicsRootConstantBufferView(2, address);
		commandList->SetGraphicsRootDescriptorTable(5, _pmdData->GetTextureList(i).Texture.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(6, _pmdData->GetTextureList(i).SphereMultiply.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(7, _pmdData->GetTextureList(i).SphereAddition.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(8, _pmdData->GetTextureList(i).ToonTexture.GPUHandler);
		commandList->DrawIndexedInstanced(_pmdData.get()->GetIndexCountForMaterial(i), 1, offset, _meshBuffer[currentFrameIndex].BaseVertexLocation, 0);
		offset  += _pmdData.get()->GetIndexCountForMaterial(i);
		address += CalcConstantBufferByteSize(sizeof(PMDMaterial));
	}

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

	return true;
}

/****************************************************************************
*                       AddMotion
*************************************************************************//**
*  @fn        bool PMDModel::AddMotion()
*  @brief     Add Motion Data
*  @param[in] const std::wstring& filePath
*  @param[in] const std::wstring& motionName
*  @return 　　bool
*****************************************************************************/
bool PMDModel::AddMotion(const std::wstring& filePath, const std::wstring& motionName)
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
bool PMDModel::StartAnimation(const std::wstring& motionName)
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
		auto iteratorBoneNode = _pmdData->FindBoneNode(boneName);

		if (iteratorBoneNode == _pmdData->GetBoneNodeTable().end())
		{
			continue;
		}

		PMDBoneNode* boneNode      = _pmdData->GetBoneNode(boneMotion.first);
		Float3       basePosition  = boneNode->GetBasePosition();
		Matrix4      boneTransform = 
			  Translation(-basePosition.x, -basePosition.y, -basePosition.z)
			* RotationQuaternion(boneMotion.second[0].Quaternion)
			* Translation(basePosition.x, basePosition.y, basePosition.z);
		_boneMatrices.get()->at(boneNode->GetBoneIndex()) = boneTransform;
	}
	PMDBoneNode::RecursiveBoneMatrixMultiply(*_boneMatrices, _pmdData->GetBoneNode("センター"), MatrixIdentity());
	WriteBoneParameterToBuffer();
	return true;
}
#pragma region Property

void PMDModel::SetWorldTimer(const GameTimer& gameTimer)
{
	this->_gameTimer = &gameTimer;
}
#pragma endregion Property
#pragma endregion Public Function
#pragma region Private Function
/****************************************************************************
*                       PrepareVertexBuffer
*************************************************************************//**
*  @fn        bool PMDModel::PrepareVertexBuffer()
*  @brief     Prepare Vertex Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDModel::PrepareVertexBuffer()
{
	/*-------------------------------------------------------------------
	-			Prepare variables
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check PMD Vertex is existed
	---------------------------------------------------------------------*/
	if (_pmdData->GetVertex() == nullptr) { ::OutputDebugString(L"Can't read vertex data (vertex)"); return false; }
	int vertexCount = (int)_pmdData->GetVertexCount();

	/*-------------------------------------------------------------------
	-			Build CPU and GPU Vertex Buffer
	---------------------------------------------------------------------*/
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		/*-------------------------------------------------------------------
		-			Copy PMD Vertex Data To Mesh Buffer
		---------------------------------------------------------------------*/
		_vertexBuffer[i] = std::make_unique<UploadBuffer<PMDVertex>>(directX12.GetDevice(), vertexCount, false);
		_vertexBuffer[i]->CopyStart();
		for (int j = 0; j < vertexCount; ++j)
		{
			_vertexBuffer[i]->CopyData(j, _pmdData->GetVertex()[j]);
		}
		_vertexBuffer[i]->CopyEnd();

		const UINT vbByteSize = (UINT)vertexCount* sizeof(PMDVertex);

		/*-------------------------------------------------------------------
		-			Build CPU / GPU Vertex Buffer
		---------------------------------------------------------------------*/
		_meshBuffer[i].BaseVertexLocation   = 0;
		_meshBuffer[i].VertexBufferGPU      = _vertexBuffer[i]->Resource();
		_meshBuffer[i].VertexByteStride     = sizeof(PMDVertex);
		_meshBuffer[i].VertexBufferByteSize = vbByteSize;
		if (FAILED(D3DCreateBlob(vbByteSize, &_meshBuffer[i].VertexBufferCPU)))
		{
			::OutputDebugString(L"Can't create blob data (vertex)");
			return false;
		}

	}

	return true;
}

/****************************************************************************
*                       PrepareIndexBuffer
*************************************************************************//**
*  @fn        bool PMDModel::PrepareIndexBuffer()
*  @brief     Prepare Index Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDModel::PrepareIndexBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check Index Data
	---------------------------------------------------------------------*/
	if (_pmdData->GetIndex() == nullptr) { ::OutputDebugString(L"Can't read index data (vertex)"); return false; }
	int indexCount = (int)_pmdData->GetTotalIndexCount();

	/*-------------------------------------------------------------------
	-			Build CPU / GPU Index Buffer
	---------------------------------------------------------------------*/
	const UINT ibByteSize = (UINT)indexCount * sizeof(UINT16);
	_meshBuffer[0].IndexBufferGPU      = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), _pmdData->GetIndex(), ibByteSize, _meshBuffer[0].IndexBufferUploader).Resource();
	_meshBuffer[0].IndexFormat         = DXGI_FORMAT_R16_UINT;
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
*  @fn        bool PMDModel::PrepareMaterialBuffer()
*  @brief     Prepare Index Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDModel::PrepareMaterialBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check Material Data
	---------------------------------------------------------------------*/
	if (_pmdData->GetMaterial() == nullptr) { ::OutputDebugString(L"Can't read material data"); return false; }
	int materialCount = (int)_pmdData->GetMaterialCount();

	/*-------------------------------------------------------------------
	-			Build Material Data
	---------------------------------------------------------------------*/
	_materialBuffer = std::make_unique<UploadBuffer<PMDMaterial>>(directX12.GetDevice(), materialCount, true);
	_materialBuffer->CopyStart();
	_materialBuffer->Resource()->SetName(L"PMDMaterial");
	for (int i = 0; i < materialCount; ++i)
	{
		_materialBuffer->CopyData(i, _pmdData->GetMaterial()[i]);
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
		cbvDesc.BufferLocation = _materialBuffer.get()->Resource()->GetGPUVirtualAddress() + i * CalcConstantBufferByteSize(sizeof(PMDMaterial));
		cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(PMDMaterial));

		/*-------------------------------------------------------------------
		-			Create Constant Buffer View
		---------------------------------------------------------------------*/
		directX12.GetDevice()->CreateConstantBufferView(
			&cbvDesc, directX12.GetCPUResourceView(HeapType::CBV, directX12.IssueViewID(HeapType::CBV)));
	}

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
bool PMDModel::PrepareBoneBuffer()
{
	/*-------------------------------------------------------------------
	-			Initialize bone matrix
	---------------------------------------------------------------------*/
	std::vector<gm::Matrix4> boneMatrices;
	boneMatrices.resize(_pmdData->GetBoneCount());
	std::fill(boneMatrices.begin(), boneMatrices.end(), gm::MatrixIdentity());
	
	/*-------------------------------------------------------------------
	-			Prepare variables 
	---------------------------------------------------------------------*/
	auto& directX12   = DirectX12::Instance();
	Device* device    = directX12.GetDevice();
	auto boneObject   = std::make_unique<UploadBuffer<BoneParameter>>(device, 1, true);
	
	BoneParameter* boneParameter = new BoneParameter();
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
	cbvDesc.SizeInBytes    = CalcConstantBufferByteSize(sizeof(BoneParameter));
	
	device->CreateConstantBufferView(
		&cbvDesc, directX12.GetCPUResourceView(HeapType::CBV, directX12.IssueViewID(HeapType::CBV)));
	
	delete boneParameter;
	return true;
}

/****************************************************************************
*                       PreparePMDObject
*************************************************************************//**
*  @fn        bool PMDModel::PreparePMDObject()
*  @brief     Prepare PMD Object
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDModel::PreparePMDObject()
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

void PMDModel::WriteBoneParameterToBuffer()
{
	BoneParameter* boneParameter = new BoneParameter();
	std::copy(_boneMatrices.get()->begin(), _boneMatrices.get()->end(), boneParameter->BoneMatrices);

	auto boneObject = _boneBuffer.get();
	boneObject->CopyStart();
	boneObject->CopyData(0, *boneParameter);
	boneObject->CopyEnd();
	delete boneParameter;
}

#pragma endregion Private Fucntion

#pragma region other

#pragma endregion other