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
#include "GameCore/Include/Model/MMD/VMDFile.hpp"
#include "GameCore/Include/GameTimer.hpp"
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
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        bool PMXModel::Initialize()
*  @brief     Initialize (Load PMDData ...)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXModel::Initialize(const std::wstring& filePath)
{
	/*-------------------------------------------------------------------
	-             Load PMD Data
	---------------------------------------------------------------------*/
	ModelPSOManager& modelManager = ModelPSOManager::Instance();
	ModelLoader modelLoader;
	modelLoader.Load3DModel(filePath, &_pmxData);

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
	-             Prepare Material Buffer
	---------------------------------------------------------------------*/
	if (!PrepareBoneBuffer()) { MessageBox(NULL, L" Bone buffer cannot be prepared.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Prepare PMDObject
	---------------------------------------------------------------------*/
	if (!PreparePMXObject())      { MessageBox(NULL, L"PMXObject cannot be prepared.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Prepare PMDObject
	---------------------------------------------------------------------*/
	if (!PrepareBoneMap()) { MessageBox(NULL, L"BoneMap cannot be prepared.", L"Warning", MB_ICONWARNING); };
	if(!PrepareBoneIK()) { MessageBox(NULL, L"BoneIK cannot be prepared.", L"Warning", MB_ICONWARNING); };
	return true;
}

bool PMXModel::Update()
{
	if (_isAnimation) 
	{
		_currentTime += _gameTimer->DeltaTime(); // [s]
		if (_motionData[_currentMotionName]->IsExistedMorphingMap()) { UpdateMorph(); }
		UpdateMotion();
	}
	return true;
}


bool PMXModel::UpdateMorph()
{

	/*-------------------------------------------------------------------
	-               Calculate Frame Number
	---------------------------------------------------------------------*/
	UINT32 frameNo = PMX_FRAME_PER_SECOND * _currentTime;
	if (frameNo > _motionData[_currentMotionName]->GetAnimationDuration())
	{
		frameNo      = 0;
		_currentTime = 0;
	}
	
	/*-------------------------------------------------------------------
	-               Update Motion Data
	---------------------------------------------------------------------*/
	auto vertices = _vertices.get();
	std::memcpy(vertices, (void*)_pmxData->GetVertex(), _pmxData->GetVertexCount() * sizeof(PMXVertex));
	for (auto& morphMotion : _motionData[_currentMotionName]->GetMorphingMap())
	{
		std::string morphName  = morphMotion.first;
		auto iteratorMorph = _pmxData->FindMorph(morphName);

		if (iteratorMorph == _pmxData->GetMorphingMap().end())
		{
			continue;
		}

		auto& morph                          = iteratorMorph->second;
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
				auto index                      = iteratorMorph->second.PositionMorphs[i].VertexIndex;
				auto& morphVertex               = iteratorMorph->second.PositionMorphs[i].Position;
				
                Vector3 temp                    = Vector3(vertices[index].Vertex.Position) + Vector3(morphVertex) * t;
				vertices[index].Vertex.Position = temp.ToFloat3();
			}
			
		}
		else
		{
			float t = reverseIterator->Weight;

			for (int i = 0; i < iteratorMorph->second.PositionMorphs.size(); ++i)
			{
				auto index                      = iteratorMorph->second.PositionMorphs[i].VertexIndex;
				auto& morphVertex               = iteratorMorph->second.PositionMorphs[i].Position;
				Vector3 temp                    = Vector3(morphVertex) * t + Vector3(vertices[index].Vertex.Position);
				vertices[index].Vertex.Position = temp.ToFloat3();
			}
		}
	
	}

	return true;
}

bool PMXModel::UpdateMotion()
{
	/*-------------------------------------------------------------------
	-               Calculate Frame Number
	---------------------------------------------------------------------*/
	UINT32 frameNo = PMX_FRAME_PER_SECOND * _currentTime;

	if (frameNo > _motionData[_currentMotionName]->GetAnimationDuration())
	{
		frameNo      = 0;
		_currentTime = 0;
	}
	/*-------------------------------------------------------------------
	-               Clear Matrix
	---------------------------------------------------------------------*/
	std::fill(_boneMatrices.get()->begin(), _boneMatrices.get()->end(), MatrixIdentity());

	auto* boneMap             = _boneMap.get();
	auto boneNodeAddressList = _boneNodeAddress.get();
	
	/*-------------------------------------------------------------------
	-               Update Motion Data
	---------------------------------------------------------------------*/
	for (auto& boneMotion : _motionData[_currentMotionName]->GetMotionMap())
	{
		std::string boneName  = boneMotion.first;

		auto iteratorBoneNode = boneMap->find(boneName);

		if (iteratorBoneNode == boneMap->end())
		{
			continue;
		}

		auto& boneNode               = iteratorBoneNode->second;
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
		boneNode.LoadInitialSRT();
		Quaternion rotation;
		Vector3 offset   = reverseIterator->Location;
		auto iterator    = reverseIterator.base();
		if (iterator != keyFrames.end())
		{
			float t  = static_cast<float>(frameNo - reverseIterator->Frame) / static_cast<float>(iterator->Frame - reverseIterator->Frame);
			t        = VMDFile::GetYFromXOnBezier(t, iterator->ControlPointForBezier[3].ToFloat4(), 12);
			Vector3  bezier;
			bezier.SetX(VMDFile::GetYFromXOnBezier(t, iterator->ControlPointForBezier[0].ToFloat4(), 12));
			bezier.SetY(VMDFile::GetYFromXOnBezier(t, iterator->ControlPointForBezier[1].ToFloat4(), 12));
			bezier.SetZ(VMDFile::GetYFromXOnBezier(t, iterator->ControlPointForBezier[2].ToFloat4(), 12));

			rotation = Slerp(reverseIterator->Quaternion, iterator->Quaternion, t);
			offset   = offset + bezier * (iterator->Location - offset);
			
		}
		else
		{
			rotation = reverseIterator->Quaternion;
		}

		
		/*-------------------------------------------------------------------
		-               Bone Transformation
		---------------------------------------------------------------------*/
		boneNode.SetAnimateRotate(rotation);
		boneNode.SetAnimateTranslate(offset);

	}

	for (auto bone : *_sortedBoneNodeAddress.get())
	{
		bone->BeforeUpdateMatrix();
	}
	UpdateNodeAnimation(false);
	UpdatePhysicsAnimation();
	UpdateNodeAnimation(true);
	
	//PMXBoneIK::SolveIK(frameNo, _pmxData.get(), *_boneNodeAddress.get(), _motionData[_currentMotionName].get());
	for (int i = 0; i < _pmxData->GetBoneCount(); ++i)
	{
		auto inverseBindMatrix     = boneNodeAddressList->at(i)->GetInverseBindMatrix();
		auto globalMatrix          = boneNodeAddressList->at(i)->GetGlobalMatrix();
		_boneMatrices.get()->at(i) = inverseBindMatrix * globalMatrix;
	}
	
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
bool PMXModel::Draw(SceneGPUAddress scene)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	ModelPSOManager& modelManager             = ModelPSOManager::Instance();
	_currentFrameIndex                     = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[_currentFrameIndex].VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _meshBuffer[_currentFrameIndex].IndexBufferView();


	/*-------------------------------------------------------------------
	-               Update matrix
	---------------------------------------------------------------------*/

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(modelManager.GetRootSignature(ModelType::PMX));
	commandList->SetPipelineState(modelManager.GetPipelineState(ModelType::PMX));
	ID3D12DescriptorHeap* heapList[] = { directX12.GetCbvSrvUavHeap() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->SetGraphicsRootConstantBufferView(0, _modelObject.get()->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, scene);
	commandList->SetGraphicsRootConstantBufferView(3, _boneBuffer.get()->Resource()->GetGPUVirtualAddress());

	/*-------------------------------------------------------------------
	-               Drawing process for each material
	---------------------------------------------------------------------*/
	auto address = _materialBuffer.get()->Resource()->GetGPUVirtualAddress();
	UINT offset  = 0;
	for (int i = 0; i < (int)_pmxData->GetMaterialCount(); ++i)
	{
		commandList->SetGraphicsRootConstantBufferView(2, address);
		commandList->SetGraphicsRootDescriptorTable(4, _pmxData->GetTextureList(i).Texture.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(5, _pmxData->GetTextureList(i).SphereMultiply.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(6, _pmxData->GetTextureList(i).SphereAddition.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(7, _pmxData->GetTextureList(i).ToonTexture.GPUHandler);
		commandList->DrawIndexedInstanced(_pmxData.get()->GetIndexCountForMaterial(i), 1, offset, _meshBuffer[_currentFrameIndex].BaseVertexLocation, 0);
		offset  += _pmxData.get()->GetIndexCountForMaterial(i);
		address += CalcConstantBufferByteSize(sizeof(PMXMaterial));
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

	auto morphVertex  = _vertices.get();
	auto vertexBuffer = _vertexBuffer[_currentFrameIndex].get();
	vertexBuffer->CopyStart();
	vertexBuffer->CopyTotalData(morphVertex, _pmxData->GetVertexCount());
	vertexBuffer->CopyEnd();
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
		Vector3       basePosition  = boneNode->GetTranslate();
		Matrix4      boneTransform = 
			  Translation(basePosition)
			* RotationQuaternion(boneMotion.second[0].Quaternion)
			* Translation(basePosition);
		_boneMatrices.get()->at(boneNode->GetBoneIndex()) = boneTransform;
	}
	PMXBoneNode::RecursiveBoneMatrixMultiply(*_boneMatrices, _pmxData->GetBoneNode("全ての親"), MatrixIdentity());
	WriteBoneParameterToBuffer();

	_isAnimation = true;
	return true;
}
#pragma region Property
void PMXModel::SetPosition(float x, float y, float z)
{
	_transform.LocalPosition = gm::Vector3(x, y, z);
}

void PMXModel::SetScale(float x, float y, float z)
{
	_transform.LocalScale = gm::Vector3(x, y, z);
}

void PMXModel::SetRotation(const gm::Quaternion& rotation)
{
	_transform.LocalRotation = rotation;
}

void PMXModel::SetWorldTimer(const GameTimer& gameTimer)
{
	this->_gameTimer = &gameTimer;
}
#pragma endregion Property
#pragma endregion Public Function

#pragma region Protected Function
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
	if (_pmxData->GetMaterial() == nullptr) { ::OutputDebugString(L"Can't read material data"); return false; }
	int materialCount = (int)_pmxData->GetMaterialCount();

	/*-------------------------------------------------------------------
	-			Build Material Data
	---------------------------------------------------------------------*/
	_materialBuffer = std::make_unique<UploadBuffer<PMXMaterial>>(directX12.GetDevice(), materialCount, true);
	_materialBuffer->CopyStart();
	_materialBuffer->Resource()->SetName(L"PMXMaterial");
	for (int i = 0; i < materialCount; ++i)
	{
		_materialBuffer->CopyData(i, _pmxData->GetMaterial()[i]);
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
		cbvDesc.BufferLocation = _materialBuffer.get()->Resource()->GetGPUVirtualAddress() + i * CalcConstantBufferByteSize(sizeof(PMXMaterial));
		cbvDesc.SizeInBytes    = CalcConstantBufferByteSize(sizeof(PMXMaterial));

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
	_worldInfo.TextureTransform = gm::MatrixIdentityF();

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
		
		thisModelIK.SetIKBone(&_boneMap.get()->at(pmxFileIK.GetIKBoneNode()->GetBoneName()));
		thisModelIK.SetTargetBone(&_boneMap.get()->at(pmxFileIK.GetTargetBoneNode()->GetBoneName()));
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
			tempIK[j].IKBone      = &_boneMap.get()->at(pmxFileIK.GetChains()[j].IKBone->GetBoneName());
			tempIK[j].EnableLimit = pmxFileIK.GetChains()[j].EnableLimit;
			tempIK[j].AngleMax    = pmxFileIK.GetChains()[j].AngleMax;
			tempIK[j].AngleMin    = pmxFileIK.GetChains()[j].AngleMin;
		}
		thisModelIK.SetIKChains(tempIK);
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

void PMXModel::UpdateBoneMatrices()
{
	auto boneNodeAddress = _boneNodeAddress.get();
	for (int i = 0; i < boneNodeAddress->size(); ++i)
	{
		PMXBoneNode* bone = boneNodeAddress->at(i);	
		if (bone->GetParent() == nullptr) { continue; }
		bone->UpdateSelfandChildMatrix();
	}
}

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
			bone->UpdateAppendMatrix();
			bone->UpdateSelfandChildMatrix();
		}

		if (bone->GetBoneIK() != nullptr)
		{
			PMXBoneIK* boneIK = bone->GetBoneIK();
			boneIK->SolveIK(frameNo, _motionData[_currentMotionName].get());
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

void PMXModel::UpdatePhysicsAnimation()
{

}
#pragma endregion Protected Function