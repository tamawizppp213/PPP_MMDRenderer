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
#include "DirectX12/Include/DirectX12PrimitiveGeometry.hpp"
#include "GameCore/Include/GameConstantBufferConfig.hpp"
#include "GameCore/Include/GameTimer.hpp"
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
*  @fn        bool DynamicCubemap::Initialize(Device* device, UINT width, UINT height, const DirectX::XMFLOAT3& origin, DXGI_FORMAT format)
*  @brief     Initialize
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool DynamicCubemap::Initialize(Device* device, UINT width, UINT height, const DirectX::XMFLOAT3& origin, DXGI_FORMAT format)
{
	if (device == nullptr) { MessageBox(NULL, L"Device is nullptr. ", L"Warning", MB_ICONWARNING); return false; }
	SetVariables(device, width, height, format);
	BuildCubeFaceCamera(origin);
	return true;
}

/****************************************************************************
*							Update
*************************************************************************//**
*  @fn        bool DynamicCubemap::Update()
*  @brief     Update
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool DynamicCubemap::Update(SceneGPUAddress scene, GameTimer& gameTimer)
{
	using namespace DirectX;

	for (int i = 0; i < 6; ++i)
	{
		SceneConstants cubemapConstants;
		XMMATRIX view                      = _camera[i].GetViewMatrix();
		XMMATRIX projection                = _camera[i].GetProjectionMatrix();
		XMMATRIX viewProjection            = XMMatrixMultiply(view, projection);

		XMVECTOR viewDeterminant           = XMMatrixDeterminant(view);
		XMVECTOR projectionDeterminant     = XMMatrixDeterminant(projection);
		XMVECTOR viewProjectionDeterminant = XMMatrixDeterminant(viewProjection);

		XMMATRIX inverseView               = XMMatrixInverse(&viewDeterminant          , view);
		XMMATRIX inverseProjection         = XMMatrixInverse(&projectionDeterminant    , projection);
		XMMATRIX inverseViewProjection     = XMMatrixInverse(&viewProjectionDeterminant, viewProjection);
		// note: Texture and shadow related features will be added later.

		XMStoreFloat4x4(&cubemapConstants.View                 , XMMatrixTranspose(view));
		XMStoreFloat4x4(&cubemapConstants.InverseView          , XMMatrixTranspose(inverseView));
		XMStoreFloat4x4(&cubemapConstants.Projection           , XMMatrixTranspose(projection));
		XMStoreFloat4x4(&cubemapConstants.InverseProjection    , XMMatrixTranspose(inverseProjection));
		XMStoreFloat4x4(&cubemapConstants.ViewProjection       , XMMatrixTranspose(viewProjection));
		XMStoreFloat4x4(&cubemapConstants.InverseViewProjection, XMMatrixTranspose(inverseViewProjection));

		Screen screen;
		cubemapConstants.EyePosition             = _camera[i].GetPosition3f();
		cubemapConstants.RenderTargetSize        = XMFLOAT2((float)_width, (float)_height);
		cubemapConstants.InverseRenderTargetSize = XMFLOAT2(1.0f / _width, 1.0f / _height);
		cubemapConstants.NearZ                   = _camera[i].GetNearZ();
		cubemapConstants.FarZ                    = _camera[i].GetFarZ();
		cubemapConstants.TotalTime               = gameTimer.TotalTime();
		cubemapConstants.DeltaTime               = gameTimer.DeltaTime();

		/*auto currentSceneConstantBuffer = SceneConstantsBuffer.get();
		currentSceneConstantBuffer->CopyStart();
		currentSceneConstantBuffer->CopyData(0, *scene);
		currentSceneConstantBuffer->CopyEnd();*/
	}
	return true;
}

bool DynamicCubemap::Draw()
{
	
	return true;
}

bool DynamicCubemap::Finalize()
{
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

		if (!BuildResource()) { return false; }
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
bool DynamicCubemap::BuildResource()
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
	using namespace gm;

	Vector3 targets[6] =
	{
		Vector3(origin.x + 1.0f, origin.y, origin.z),
		Vector3(origin.x - 1.0f, origin.y, origin.z),
		Vector3(origin.x, origin.y + 1.0f, origin.z),
		Vector3(origin.x, origin.y - 1.0f, origin.z),
		Vector3(origin.x, origin.y, origin.z + 1.0f),
		Vector3(origin.x, origin.y, origin.z - 1.0f)
	};
	Vector3 ups[6] =
	{
		Vector3(0.0f, 1.0f,  0.0f),
		Vector3(0.0f, 1.0f,  0.0f),
		Vector3(0.0f, 0.0f, -1.0f),
		Vector3(0.0f, 0.0f, +1.0f),
		Vector3(0.0f, 1.0f,  0.0f),
		Vector3(0.0f, 1.0f,  0.0f)
	};

	for (int i = 0; i < 6; ++i)
	{
		_camera[i].LookAt(Vector3(origin), targets[i], ups[i]);
		_camera[i].SetLens(0.5f * DirectX::XM_PI, 1.0f, 0.0f, 1000.0f);
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
	skyData.World = gm::Scaling(SKY_SCALE, SKY_SCALE, SKY_SCALE).ToFloat4x4();

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