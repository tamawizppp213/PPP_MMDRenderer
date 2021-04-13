//////////////////////////////////////////////////////////////////////////////////
///             @file   CubeMap.hpp
///             @brief  CubeMap
///             @author Toide Yutaro
///             @date   2021_03_22
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/EnvironmentMap/CubeMap.hpp"
#include "GameCore/Include/EnvironmentMap/CubeMapPipeLineState.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/Core/DirectX12MeshBuffer.hpp"
#include "DirectX12/Include/DirectX12PrimitiveGeometry.hpp"
#include "GameCore/Include/GameConstantBufferConfig.hpp"
#include <d3dcompiler.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define SKY_SCALE 5000.0f

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Dynamic Cubemap
//////////////////////////////////////////////////////////////////////////////////
#pragma region DynamicCubemap
DynamicCubemap::DynamicCubemap(Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
	SetVariables(device, width, height, format);
}

#pragma region Public Function
/****************************************************************************
*							Initialize
*************************************************************************//**
*  @fn        bool DynamicCubemap::Initialize(Device* device, UINT width, UINT height, DXGI_FORMAT format)
*  @brief     Initialize
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool DynamicCubemap::Initialize(Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
	if (device == nullptr) { MessageBox(NULL, L"Device is nullptr. ", L"Warning", MB_ICONWARNING); return false; }
	SetVariables(device, width, height, format);
	return true;
}

/****************************************************************************
*							SetCubemapSize
*************************************************************************//**
*  @fn        bool DynamicCubemap::SetCubemapSize(UINT width, UINT height)
*  @brief     Set cubemap size
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool DynamicCubemap::SetCubemapSize(UINT width, UINT height)
{
	if ((_width != width) || (_height != height))
	{
		_width  = width;
		_height = height;

		if (!BuildTextureResource()) { return false; }
		if (!BuildDescriptors())     { return false; };
	}
	return true;
}
#pragma endregion Public Function


#pragma region Private Function
/****************************************************************************
*							BuildTextureResource
*************************************************************************//**
*  @fn        bool DynamicCubemap::BuildTextureResource()
*  @brief     Create Texture Resource
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool DynamicCubemap::BuildTextureResource()
{
	if (_device == nullptr) { return false; }
	/*-------------------------------------------------------------------
	-             Prepare Descriptor and heap property
	---------------------------------------------------------------------*/
	RESOURCE_DESC textureDesc = RESOURCE_DESC::Texture2D( _format, _width, _height, 
		6, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	HEAP_PROPERTY heapProperty(D3D12_HEAP_TYPE_DEFAULT);

	/*-------------------------------------------------------------------
	-             Send Resource to GPU 
	---------------------------------------------------------------------*/
	ThrowIfFailed(_device->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cubemap)));

	return true;
}

/****************************************************************************
*							BuildDescriptors
*************************************************************************//**
*  @fn        bool DynamicCubemap::BuildDescriptors()
*  @brief     Create SRV, RTV Desc
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool DynamicCubemap::BuildDescriptors()
{
	if (_device == nullptr) { return false; }

	/*-------------------------------------------------------------------
	-             Create Shader Resource View
	---------------------------------------------------------------------*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format                          = _format;
	srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip     = 0;
	srvDesc.TextureCube.MipLevels           = 1;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	_device->CreateShaderResourceView(_cubemap.Get(), &srvDesc, _cpuSrv);

	/*-------------------------------------------------------------------
	-             Create Render Target View (for each face)
	---------------------------------------------------------------------*/
	for (int i = 0; i < 6; ++i)
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension                  = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Format                         = _format;
		rtvDesc.Texture2DArray.MipSlice        = 0;
		rtvDesc.Texture2DArray.PlaneSlice      = 0;
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		rtvDesc.Texture2DArray.ArraySize       = 1;
		_device->CreateRenderTargetView(_cubemap.Get(), &rtvDesc, _cpuRtv[i]);
	}

	/*-------------------------------------------------------------------
	-             Create Depth Stencil View
	---------------------------------------------------------------------*/
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment        = 0;
	depthStencilDesc.Width            = _width;
	depthStencilDesc.Height           = _height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels        = 1;
	depthStencilDesc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;

	return true;
}

/****************************************************************************
*							BuildDescriptors
*************************************************************************//**
*  @fn        bool DynamicCubemap::BuildDescriptors(CPU_DESC_HANDLER cpuSrv, GPU_DESC_HANDLER gpuSrv, CPU_DESC_HANDLER cpuRtv[6])
*  @brief     Create SRV, RTV Desc
*  @param[in] CPU_DESC_HANDLER 
*  @param[in] GPU_DESC_HANDLER
*  @param[in] CPU_DESC_HANDLER RTV
*  @return 　　bool
*****************************************************************************/
bool DynamicCubemap::BuildDescriptors(CPU_DESC_HANDLER cpuSrv, GPU_DESC_HANDLER gpuSrv, CPU_DESC_HANDLER cpuRtv[6])
{
	/*-------------------------------------------------------------------
	-             Prepare Desc hander (CPU_SRV, GPU_SRV, CPU_RTV[6])
	---------------------------------------------------------------------*/
	_cpuSrv = cpuSrv;
	_gpuSrv = gpuSrv;
	for (int i = 0; i < 6; ++i)
	{
		_cpuRtv[i] = cpuRtv[i];
	}

	/*-------------------------------------------------------------------
	-            Build Descriptors
	---------------------------------------------------------------------*/
	if (!BuildDescriptors()) { return false; };
	return true;

}


void DynamicCubemap::SetVariables(Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
	this->_device = device;
	this->_width  = width;
	this->_height = height;
	this->_format = format;

	_viewport     = { 0.0f, 0.0f, (float)width, (float)height };
	_scissorRect  = { 0,0,(int)width, (int)height };
}

/****************************************************************************
*							BuildCubeFaceCamera
*************************************************************************//**
*  @fn        bool Skybox::BuildCubeFaceCamera(const DirectX::XMFLOAT3& origin)
*  @brief     Build Cube Face Camera
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool DynamicCubemap::BuildCubeFaceCamera(const DirectX::XMFLOAT3& origin)
{
	using namespace DirectX;

	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(origin.x + 1.0f, origin.y, origin.z),
		XMFLOAT3(origin.x - 1.0f, origin.y, origin.z),
		XMFLOAT3(origin.x, origin.y + 1.0f, origin.z),
		XMFLOAT3(origin.x, origin.y - 1.0f, origin.z),
		XMFLOAT3(origin.x, origin.y, origin.z + 1.0f),
		XMFLOAT3(origin.x, origin.y, origin.z - 1.0f)
	};
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f,  0.0f),
		XMFLOAT3(0.0f, 1.0f,  0.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, +1.0f),
		XMFLOAT3(0.0f, 1.0f,  0.0f),
		XMFLOAT3(0.0f, 1.0f,  0.0f)
	};

	for (int i = 0; i < 6; ++i)
	{
		_camera[i].LookAt(origin, targets[i], ups[i]);
		_camera[i].SetLens(0.5f * XM_PI, 1.0f, 0.0f, 1000.0f);
		_camera[i].UpdateViewMatrix();
	}

	return true;
}
#pragma endregion Private Function
#pragma endregion Dynamic Cubemap

//////////////////////////////////////////////////////////////////////////////////
//                             Skybox
//////////////////////////////////////////////////////////////////////////////////
#pragma region Skybox
/****************************************************************************
*							Initialize
*************************************************************************//**
*  @fn        bool Skybox::Initialize(const std::wstring& filePath, const DirectX::XMFLOAT3& origin)
*  @brief     Skybox Initialize
*  @param[in] const std::wstring& filePath
*  @param[in] const DirectX::XMFLOAT3& origin coordinate
*  @return 　　bool
*****************************************************************************/
bool Skybox::Initialize(const std::wstring& texturePath)
{
	/*-------------------------------------------------------------------
	-            Load Cubemap
	---------------------------------------------------------------------*/
	CubemapPSOManager& cubemapManager = CubemapPSOManager::Instance();
	TextureLoader textureLoader;
	textureLoader.LoadTexture(texturePath, _texture, TextureType::TextureCube);
	/*-------------------------------------------------------------------
	-            Prepare Vertex and Index Buffer
	---------------------------------------------------------------------*/
	if (!PrepareVertexAndIndexBuffer()) { return false; }
	/*-------------------------------------------------------------------
	-            Prepare SkyObject
	---------------------------------------------------------------------*/
	if (!PrepareSkyObject())            { return false; }
	/*-------------------------------------------------------------------
	-            Prepare Material Constant Buffer
	---------------------------------------------------------------------*/
	if (!PrepareMaterialBuffer())      { return false; }

	return true;
}

/****************************************************************************
*							Draw
*************************************************************************//**
*  @fn        bool Skybox::Draw()
*  @brief     Set Skybox Draw
*  @param[in] void
*  @return 　　void
*****************************************************************************/
bool Skybox::Draw(SceneGPUAddress scene)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	CubemapPSOManager& cubemapManager         = CubemapPSOManager::Instance();
	int currentFrameIndex                     = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _sphere[currentFrameIndex]->VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _sphere[currentFrameIndex]->IndexBufferView();
	auto rtv          = directX12.GetCPUResourceView(HeapType::RTV, currentFrameIndex);
	auto dsv          = directX12.GetCPUResourceView(HeapType::DSV, 0);
	auto skyObject    = _skyObject.get();
	/*-------------------------------------------------------------------
	-               Update matrix
	---------------------------------------------------------------------*/

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(cubemapManager.GetRootSignature(CubemapType::SKYBOX));
	commandList->SetPipelineState(cubemapManager.GetPipelineState(CubemapType::SKYBOX));
	ID3D12DescriptorHeap* heapList[] = { directX12.GetCbvSrvUavHeap() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
	commandList->SetGraphicsRootConstantBufferView(0, skyObject->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, scene);
	commandList->SetGraphicsRootConstantBufferView(2, _material->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(3, _texture.GPUHandler);
	commandList->DrawIndexedInstanced(_sphere[currentFrameIndex]->IndexCount, 1, _sphere[currentFrameIndex]->StartIndexLocation, _sphere[currentFrameIndex]->BaseVertexLocation, 0);

	return true;
}

/****************************************************************************
*							Finalize
*************************************************************************//**
*  @fn        bool Skybox::Finalize()
*  @brief     Set Skybox Origin
*  @param[in] void
*  @return 　　void
*****************************************************************************/
bool Skybox::Finalize()
{
	_sphere.clear();
	return true;
}


/****************************************************************************
*							SetSkyboxMaterial
*************************************************************************//**
*  @fn        bool Skybox::SetSkyboxMaterial()
*  @brief     Set Skybox Material 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Skybox::SetSkyboxMaterial(MaterialConstants& materialInfo)
{
	auto material = _material.get();
	material->CopyStart();
	material->CopyData(0, materialInfo);
	material->CopyEnd();
}
#pragma region Private Function
/****************************************************************************
*							PrepareVertexBuffer
*************************************************************************//**
*  @fn        bool Skybox::PrepareVertexBuffer()
*  @brief     Prepare Sphere Vertex Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Skybox::PrepareVertexAndIndexBuffer()
{
	/*-------------------------------------------------------------------
	-            Prepare Variables
	---------------------------------------------------------------------*/
	DirectX12&        directX12   = DirectX12::Instance();
	CommandList*      commandList = directX12.GetCommandList();
	GeometryGenerator geometryGenerator;
	_sphere.resize(FRAME_BUFFER_COUNT);

	/*-------------------------------------------------------------------
	-            Create Sphere Mesh
	---------------------------------------------------------------------*/
	MeshData sphereMesh = geometryGenerator.Sphere(0.5f, 20, 20, false);

	for (int i = 0; i < _sphere.size(); ++i)
	{
		/*-------------------------------------------------------------------
		-            Calcurate Buffer Size
		---------------------------------------------------------------------*/
		auto sphere               = std::make_unique<MeshBuffer>();
		auto vertexBufferByteSize = (UINT)sphereMesh.Vertices.size() * sizeof(VertexPositionNormalTexture);
		auto indexBufferByteSize  = (UINT)sphereMesh.Indices.size()  * sizeof(UINT16);
	
		/*-------------------------------------------------------------------
		-            Set Vertex Buffer
		---------------------------------------------------------------------*/
		ThrowIfFailed(D3DCreateBlob(vertexBufferByteSize, &sphere->VertexBufferCPU));
		CopyMemory(sphere->VertexBufferCPU->GetBufferPointer(), sphereMesh.Vertices.data(), vertexBufferByteSize);
		sphere->VertexBufferGPU      = DefaultBuffer(directX12.GetDevice(), commandList, sphereMesh.Vertices.data(), vertexBufferByteSize, sphere->VertexBufferUploader).Resource();
		sphere->VertexByteStride     = sizeof(VertexPositionNormalTexture);
		sphere->VertexBufferByteSize = (UINT)vertexBufferByteSize;
		sphere->BaseVertexLocation   = 0;

		/*-------------------------------------------------------------------
		-            Set Index Buffer
		---------------------------------------------------------------------*/
		ThrowIfFailed(D3DCreateBlob(indexBufferByteSize, &sphere->IndexBufferCPU));
		CopyMemory(sphere->IndexBufferCPU->GetBufferPointer(), sphereMesh.Indices.data(), indexBufferByteSize);
		sphere->IndexBufferGPU      = DefaultBuffer(directX12.GetDevice(), commandList, sphereMesh.Indices.data(), indexBufferByteSize, sphere->IndexBufferUploader).Resource();
		sphere->IndexFormat         = DXGI_FORMAT_R16_UINT;
		sphere->IndexBufferByteSize = (UINT)indexBufferByteSize;
		sphere->StartIndexLocation  = 0;
		sphere->IndexCount          = (UINT)sphereMesh.Indices.size();

		_sphere[i] = std::move(sphere);
	}
	
	return true;
}



/****************************************************************************
*							PrepareMaterialBuffer
*************************************************************************//**
*  @fn        bool Skybox::PrepareMaterialBuffer()
*  @brief     Build Material Constant Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Skybox::PrepareMaterialBuffer()
{
	using namespace DirectX;
	/*-------------------------------------------------------------------
	-			Prepare variables
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	Device* device       = directX12.GetDevice();
	auto material        = std::make_unique<UploadBuffer<MaterialConstants>>(device, 1, true);
	MaterialConstants    materialInfo; // sphere


	/*-------------------------------------------------------------------
	-			Copy Sky object data
	---------------------------------------------------------------------*/
	material->CopyStart();
	material->CopyData(0, materialInfo);
	material->CopyEnd();

	/*-------------------------------------------------------------------
	-			Move sky object data
	---------------------------------------------------------------------*/
	_material = std::move(material);
	return true;
}

/****************************************************************************
*							PrepareSkyObject
*************************************************************************//**
*  @fn        bool Skybox::PrepareSkyObject()
*  @brief     Build Material Constant Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Skybox::PrepareSkyObject()
{
	using namespace DirectX;
	/*-------------------------------------------------------------------
	-			Prepare variables
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	Device* device       = directX12.GetDevice();
	auto skyObject       = std::make_unique<UploadBuffer<ObjectConstants>>(device, 1, true);

	/*-------------------------------------------------------------------
	-			Set Skydata
	---------------------------------------------------------------------*/
	ObjectConstants skyData; // sphere
	XMStoreFloat4x4(&skyData.World, XMMatrixScaling(SKY_SCALE, SKY_SCALE, SKY_SCALE));
	skyData.TextureTransform = DirectXMathHelper::Identity4X4();

	/*-------------------------------------------------------------------
	-			Copy Sky object data
	---------------------------------------------------------------------*/
	skyObject->CopyStart();
	skyObject->CopyData(0, skyData);
	skyObject->CopyEnd();

	/*-------------------------------------------------------------------
	-			Move sky object data
	---------------------------------------------------------------------*/
	_skyObject = std::move(skyObject);
	return true;
}

#pragma endregion Private Function
#pragma endregion Skybox