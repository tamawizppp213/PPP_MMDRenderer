//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12ModelLoader.hpp
///             @brief  Load Model
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/Primitive/PrimitiveModel.hpp"
#include "GameCore/Include/Model/ModelMaterial.hpp"
#include "GameCore/Include/Model/ModelPipelineState.hpp"
#include <d3dcompiler.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
PrimitiveModel::PrimitiveModel()
{
	MaterialPtr material = std::make_unique<PBRMaterial>();
	SetActorType(ActorType::Primitive);
	_material = std::move(material);

}
PrimitiveModel::~PrimitiveModel()
{

}

#pragma region Public Function
void PrimitiveModel::CreateRect()
{
	_meshData = GeometryGenerator::Rect(1.0f, 1.0f, 1.0f);

	PrepareBuffers();
}
void PrimitiveModel::CreateQuadrangle(float x, float y, float w, float h, float depth)
{
	_meshData = GeometryGenerator::Quadrangle(x, y, w, h, depth);
	PrepareBuffers();
}
void PrimitiveModel::CreateBox(bool isInvertNormal)
{
	_meshData = GeometryGenerator::Box(1.0f, 1.0f, 1.0f, 0, isInvertNormal);
	PrepareBuffers();
}
void PrimitiveModel::CreateSphere(UINT32 sliceCount, UINT32 stackCount, bool isInvertNormal)
{
	_meshData = GeometryGenerator::Sphere(1.0f, sliceCount, stackCount, isInvertNormal);
	PrepareBuffers();
}
void PrimitiveModel::CreateGeoSphere(UINT32 numSubdivisions, bool isInvertNormal)
{
	_meshData = GeometryGenerator::GeoSphere(1.0f, numSubdivisions, isInvertNormal);
	PrepareBuffers();
}
void PrimitiveModel::CreateCylinder(float bottomRadius, float topRadius, UINT32 sliceCount, UINT32 stackCount, bool isInvertNormal)
{
	_meshData = GeometryGenerator::Cylinder(bottomRadius, topRadius, 1.0f, sliceCount, stackCount, isInvertNormal);
	PrepareBuffers();
}
void PrimitiveModel::CreateGrid(UINT32 rows, UINT32 columns, bool isInvertNormal)
{
	_meshData = GeometryGenerator::Grid(1.0f, 1.0f, rows, columns, isInvertNormal);
	PrepareBuffers();
}

void PrimitiveModel::LoadTexture(const std::wstring& filePath)
{
	TextureLoader textureLoader;
	textureLoader.LoadTexture(filePath, _texture);
}

bool PrimitiveModel::Update()
{
	UpdateGPUData();
	return true;
}

bool PrimitiveModel::Draw(SceneGPUAddress scene, LightGPUAddress light)
{
	Update();
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12          = DirectX12::Instance();
	CommandList* commandList      = directX12.GetCommandList();
	ModelPSOManager& modelManager = ModelPSOManager::Instance();
	_currentFrameIndex = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[_currentFrameIndex].VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _meshBuffer[_currentFrameIndex].IndexBufferView();


	/*-------------------------------------------------------------------
	-               Update matrix
	---------------------------------------------------------------------*/

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->SetGraphicsRootConstantBufferView(0, _modelObject.get()->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, scene);
	commandList->SetGraphicsRootConstantBufferView(2, _materialBuffer.get()->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, light);


	/*-------------------------------------------------------------------
	-               Drawing process for each material
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootDescriptorTable(4, _texture.GPUHandler);
	commandList->DrawIndexedInstanced((UINT)_meshData.Indices.size(), 1, _meshBuffer[_currentFrameIndex].StartIndexLocation, _meshBuffer[_currentFrameIndex].BaseVertexLocation, 0);
	return true;
}

#pragma endregion Public Function
#pragma region Protected Function
bool PrimitiveModel::PrepareBuffers()
{
	if (!PrepareVertexBuffer())   { return false; }
	if (!PrepareIndexBuffer())    { return false; }
	if (!PrepareMaterialBuffer()) { return false; }
	if (!PrepareObjectBuffer())   { return false; }
	LoadTexture(L"Resources/Texture/Default/White.png");
	return true;
}
bool PrimitiveModel::PrepareVertexBuffer()
{
	/*-------------------------------------------------------------------
	-			Prepare variables
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check PMD Vertex is existed
	---------------------------------------------------------------------*/
	int vertexCount    = static_cast<int>(_meshData.Vertices.size());
	int vertexByteSize = sizeof(VertexPositionNormalTexture);

	/*-------------------------------------------------------------------
	-			Build CPU and GPU Vertex Buffer
	---------------------------------------------------------------------*/
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		/*-------------------------------------------------------------------
		-			Copy PMD Vertex Data To Mesh Buffer
		---------------------------------------------------------------------*/
		_vertexBuffer[i] = std::make_unique<UploadBuffer<VertexPositionNormalTexture>>(directX12.GetDevice(), vertexCount, false);
		_vertexBuffer[i]->CopyStart();
		_vertexBuffer[i]->CopyTotalData(_meshData.Vertices.data(), (int)_meshData.Vertices.size());
		_vertexBuffer[i]->CopyEnd();
		const UINT vbByteSize = (UINT)vertexCount * vertexByteSize;

		/*-------------------------------------------------------------------
		-			Build CPU / GPU Vertex Buffer
		---------------------------------------------------------------------*/
		_meshBuffer[i].BaseVertexLocation   = 0;
		_meshBuffer[i].VertexBufferGPU      = _vertexBuffer[i]->Resource();
		_meshBuffer[i].VertexByteStride     = vertexByteSize;
		_meshBuffer[i].VertexBufferByteSize = vbByteSize;
		if (FAILED(D3DCreateBlob(vbByteSize, &_meshBuffer[i].VertexBufferCPU)))
		{
			::OutputDebugString(L"Can't create blob data (vertex)");
			return false;
		}

	}

	return true;
}
bool PrimitiveModel::PrepareIndexBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check Index Data
	---------------------------------------------------------------------*/
	int indexCount = (int)_meshData.Indices.size();

	/*-------------------------------------------------------------------
	-			Build CPU / GPU Index Buffer
	---------------------------------------------------------------------*/
	const UINT ibByteSize = (UINT)indexCount * sizeof(UINT32);
	_meshBuffer[0].IndexBufferGPU      = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), _meshData.Indices.data(), ibByteSize, _meshBuffer[0].IndexBufferUploader).Resource();
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
bool PrimitiveModel::PrepareMaterialBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Build Material Data
	---------------------------------------------------------------------*/
	_materialBuffer = std::make_unique<UploadBuffer<PBRMaterial>>(directX12.GetDevice(), 1, true);
	_materialBuffer->CopyStart();
	_materialBuffer->CopyData(0, *_material.get());
	_materialBuffer->Resource()->SetName(L"PrimitiveModelMaterial");
	_materialBuffer->CopyEnd();
	/*-------------------------------------------------------------------
	-			Create Constant Buffer View Descriptor
	---------------------------------------------------------------------*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = _materialBuffer.get()->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes    = CalcConstantBufferByteSize(sizeof(PBRMaterial));

	/*-------------------------------------------------------------------
	-			Create Constant Buffer View
	---------------------------------------------------------------------*/
	directX12.GetDevice()->CreateConstantBufferView(
		&cbvDesc, directX12.GetCPUResourceView(HeapType::CBV, directX12.IssueViewID(HeapType::CBV)));
	return true;
}
bool PrimitiveModel::PrepareObjectBuffer()
{
	/*-------------------------------------------------------------------
	-			Prepare variables
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	Device* device = directX12.GetDevice();
	auto modelObject = std::make_unique<UploadBuffer<ObjectConstants>>(device, 1, true);

	/*-------------------------------------------------------------------
	-			Set Modeldata
	---------------------------------------------------------------------*/
	_worldInfo.World = gm::MatrixIdentityF();
	_worldInfo.TextureConfig = 0;
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

bool PrimitiveModel::UpdateGPUData()
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
	-               Map Material Object
	---------------------------------------------------------------------*/
	_materialBuffer.get()->CopyStart();
	_materialBuffer.get()->CopyData(0, *_material.get());
	_materialBuffer.get()->CopyEnd();
	return true;
}


#pragma endregion Protected Function

#pragma region Property
void PrimitiveModel::SetMaterial(const PBRMaterial& material)
{
	*_material.get() = material;
}
void PrimitiveModel::SetDiffuse(const gm::Float4& diffuse)
{
	_material.get()->Diffuse = diffuse;
}
void PrimitiveModel::SetSpecular(const gm::Float3& specular)
{
	_material.get()->Specular = specular;
}
void PrimitiveModel::SetRoughness(float roughness)
{
	_material.get()->Roughness = roughness;
}
void PrimitiveModel::SetEmissiveColor(const gm::Float3& color)
{
	_material.get()->EmissiveColor = color;
}
void PrimitiveModel::SetEmissiveIntensity(float intensity)
{
	_material.get()->EmissiveIntensity = intensity;
}
void PrimitiveModel::SetAmbient(const gm::Float3& ambient)
{
	_material.get()->Ambient = ambient;
}
void PrimitiveModel::SetMetalness(float metalness)
{
	_material.get()->Metalness = metalness;
}
#pragma endregion Property