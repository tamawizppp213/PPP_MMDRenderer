//////////////////////////////////////////////////////////////////////////////////
//              Title:  MotionTest.hpp
//            Content:  MotionTest Scene 
//             Author:  Toide Yutaro
//             Create:  2020_11_07
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Include/Test.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include "DirectX12/Include/DirectX12PrimitiveGeometry.hpp"
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "DirectX12/Include/Core/DirectX12BaseStruct.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/DirectX12MathHelper.hpp"
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include "GameCore/Include/Model/ModelLoader.hpp"
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <iostream>
#include <array>
#include "GameCore/Include/Model/MMD/PMDModel.hpp"
#include "GameCore/Include/Audio/AudioSource3D.hpp"
#include "GameCore/Include/Sprite/Sprite.hpp"
#include "GameCore/Include/Model/FBX/FBXFile.hpp"
#include "GameCore/Include/Sprite/TextRenderer.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
float angle = 0.0f;
std::vector<Sprite> _spriteList;
std::vector<Sprite> _list;
std::vector<Sprite> _list2;
Texture test;
Texture test2;
Texture test3;
Fader _fader;
bool isStart = false;

//////////////////////////////////////////////////////////////////////////////////
//                              Struct
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
Test::Test()
{
	
}

Test::~Test()
{

}

void Test::Initialize(GameTimer& gameTimer)
{
	_gameTimer  = &gameTimer;
	_device      = _directX12.GetDevice();
	_commandList = _directX12.GetCommandList();

	_directX12.ResetCommandList();

	_spriteRenderer.Initialize();
	LoadTextures();

	// ÉÇÉìÉXÉ^Å[10ëÃÇ
	for (int i = 0; i < 10; ++i)
	{
		Sprite sprite;
		sprite.CreateSpriteForTexture(
			DirectX::XMFLOAT3((float)0.1 * i - 0.5f, (float)-0.5f, 0.0f),
			DirectX::XMFLOAT2(0.5f, 0.5f),
			DirectX::XMFLOAT2(0.0f, 1.0f),
			DirectX::XMFLOAT2(0.0f, 1.0f));
		_list2.push_back(sprite);
	}

	for (int i = 0; i < 10; ++i)
	{
		Sprite sprite;
		sprite.CreateSpriteForTexture(
			DirectX::XMFLOAT3((float)0.1 * i - 0.5f, (float)0.0f, 0.0f),
			DirectX::XMFLOAT2(0.5f, 0.5f),
			DirectX::XMFLOAT2(0.0f, 1.0f),
			DirectX::XMFLOAT2(0.0f, 1.0f));
		_list.push_back(sprite);
	}

	for (int i = 0; i < 10; ++i)
	{
		Sprite sprite;
		sprite.CreateSpriteForTexture(
			DirectX::XMFLOAT3((float)0.1 * i - 0.5f, (float)0.5f, 0.0f),
			DirectX::XMFLOAT2(0.5f, 0.5f),
			DirectX::XMFLOAT2(0.0f, 1.0f),
			DirectX::XMFLOAT2(0.0f, 1.0f));
		_spriteList.push_back(sprite);
	}

	_fader.Initialize();
	_fader.SetColor(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	_directX12.CompleteInitialize();
	_directX12.FlushCommandQueue();

	//PMDModel model;
	//model.Initialize(L"Resources/Model/Miku/HatsuneMiku.pmd");
	//FBXData fbx;
	//fbx.Load3DModel(L"Resources/Model/Wolf/Wolf_with_Animations.fbx");
}

void Test::Update()
{
	using namespace DirectX;
	angle += 1.0f;
	

}

void Test::Draw()
{
	_directX12.ClearScreen();
	if (!isStart) { _fader.StartFadeIn(2.0f, 10.0f); isStart = true; }
	
	_fader.Draw(*_gameTimer, matrix);
    _directX12.CompleteRendering();
}


void Test::Terminate()
{

}

#pragma region Protected Function
void Test::LoadShaders()
{
	_vertexShader = CompileShader(L"Shader\\ShaderTestVertex.hlsl", nullptr, "VSMain", "vs_5_1");
	_pixelShader  = CompileShader(L"Shader\\ShaderTestPixel.hlsl",  nullptr, "PSMain", "ps_5_1");
}

void Test::LoadGeometry()
{
	GeometryGenerator  geometryGenerator;
	MeshData rect  = geometryGenerator.Rect(2.0f, 2.0f, 1.0f);
	MeshData rect2 = geometryGenerator.Rect(1.0f, 1.0f, 1.0f);

	SubMeshGeometry rectSubmesh;
	rectSubmesh.IndexCount = (UINT)rect.Indices.size();
	rectSubmesh.StartIndexLocation = 0;
	rectSubmesh.BaseVertexLocation = 0;

	std::vector<VertexPositionNormalColorTexture> vertices(rect.Vertices.size());
	for (UINT64 i = 0; i < rect.Vertices.size(); ++i)
	{
		vertices[i].Position = rect.Vertices[i].Position;
		vertices[i].UV       = rect.Vertices[i].UV;
	}
	

	std::vector<UINT16> indices;
	indices.insert(indices.end(), std::begin(rect.Indices), std::end(rect.Indices));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(VertexPositionNormalColorTexture);
	const UINT ibByteSize = (UINT)indices.size()  * sizeof(UINT16);

	auto geometry = std::make_unique<MeshGeometry>();
	geometry->Name = "rect";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geometry->VertexBufferCPU));
	CopyMemory(geometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geometry->IndexBufferCPU));
	CopyMemory(geometry->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	// create vertex and index buffer
	geometry->VertexBufferGPU = DefaultBuffer(_directX12.GetDevice(), _directX12.GetCommandList(), vertices.data(), vbByteSize, geometry->VertexBufferUploader).Resource();
	geometry->IndexBufferGPU  = DefaultBuffer(_directX12.GetDevice(), _directX12.GetCommandList(), indices.data(),  ibByteSize, geometry->IndexBufferUploader ).Resource();

	// for vertex and index buffer view 
	geometry->VertexByteStride     = sizeof(VertexPositionNormalColorTexture);
	geometry->VertexBufferByteSize = vbByteSize;
	geometry->IndexFormat          = DXGI_FORMAT_R16_UINT;
	geometry->IndexBufferByteSize  = ibByteSize;

	geometry->DrawArgs["rect"] = rectSubmesh;

	_geometries[geometry->Name] = std::move(geometry);

}

void Test::LoadTextures()
{
	TextureLoader texture;
	texture.LoadTexture(L"Resources/Texture/Test.jpg", _texture);
	texture.LoadTexture(L"Resources/Texture/textest.png", test2);
	texture.LoadTexture(L"Resources/Texture/Alora.jpg", test);
	texture.LoadTexture(L"Resources/Texture/Title.png", test3);
}

void Test::BuildRootSignature()
{
	/*-------------------------------------------------------------------
	-			Build texture table
	---------------------------------------------------------------------*/
	DESCRIPTOR_RANGE textureTable[2];
	textureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
	textureTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0);

	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[2];
	rootParameter[0].InitAsDescriptorTable(1, &textureTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameter[1].InitAsDescriptorTable(1, &textureTable[1], D3D12_SHADER_VISIBILITY_VERTEX);

	/*-------------------------------------------------------------------
	-			Build sampler desc
	---------------------------------------------------------------------*/
	STATIC_SAMPLER_DESC samplerDesc;
	samplerDesc.Init(0, D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	/*-------------------------------------------------------------------
	-			 Build root parameter 
	---------------------------------------------------------------------*/
	// A root signature is a collection of descriptor tables 
    // (which feeds data other than vertices to the shader).
	ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(2, rootParameter, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSignatureDesc.Create(_device.Get(), &_rootSignature);

}

void Test::BuildPSOs()
{
	/*-------------------------------------------------------------------
	-			PSO for default objects
	---------------------------------------------------------------------*/
	D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultPsoDesc = _directX12.GetDefaultPSOConfig();
	defaultPsoDesc.InputLayout = VertexPositionNormalColorTexture::InputLayout;
	defaultPsoDesc.pRootSignature = _rootSignature.Get();
	defaultPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(_vertexShader->GetBufferPointer()),
		_vertexShader->GetBufferSize()
	};
	defaultPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(_pixelShader->GetBufferPointer()),
		_pixelShader->GetBufferSize()
	};
	ThrowIfFailed(_directX12.GetDevice()->CreateGraphicsPipelineState(&defaultPsoDesc, IID_PPV_ARGS(&_pipelineState)));

}

void Test::BuildRenderItem()
{

}

void Test::BuildFrameResources()
{

}
void Test::BuildDescriptorHeap()
{
	using namespace DirectX;

	/*-------------------------------------------------------------------
	-			Build basic desc
	---------------------------------------------------------------------*/
	D3D12_DESCRIPTOR_HEAP_DESC basicDesc;
	basicDesc.NumDescriptors = 2;
	basicDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	basicDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	basicDesc.NodeMask       = 0;

	ThrowIfFailed(_device->CreateDescriptorHeap(&basicDesc, IID_PPV_ARGS(&_basicDescHeap)));

	/*-------------------------------------------------------------------
	-			Build texture desc
	---------------------------------------------------------------------*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format                        = _texture.Resource.Get()->GetDesc().Format;
	srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels           = 1;
	srvDesc.Texture2D.PlaneSlice          = 0;
	srvDesc.Texture2D.MostDetailedMip     = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	
	/*-------------------------------------------------------------------
	-			Build constant buffer desc
	---------------------------------------------------------------------*/
	//XMMATRIX matrix = DirectX::XMMatrixIdentity();
	_constantBuffer = std::make_unique<UploadBuffer<XMMATRIX>>(_device.Get(), 1, true);
	_constantBuffer->CopyStart();
	_constantBuffer->CopyData(0, matrix);
	_constantBuffer->CopyEnd();
	UINT cbSize     = CalcConstantBufferByteSize(sizeof(XMMATRIX));

	// Calcurate constant buffer offset
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = _constantBuffer->Resource()->GetGPUVirtualAddress();
	int baseIndex = 0;
	cbAddress    += (UINT64)baseIndex * cbSize;
	
	/*-------------------------------------------------------------------
	-			Build Constant Buffer View desc
	---------------------------------------------------------------------*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes    = cbSize;

	/*-------------------------------------------------------------------
	-			Build  View 
	---------------------------------------------------------------------*/
	auto basicHeapHandle = _basicDescHeap.Get()->GetCPUDescriptorHandleForHeapStart();
	_device->CreateShaderResourceView(
		_texture.Resource.Get(),
		&srvDesc,
		basicHeapHandle);

	basicHeapHandle.ptr += _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_device->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
}
void Test::BuildConstantBufferView()
{
	using namespace DirectX;

}
#pragma endregion Protected Function

#pragma region Private Function
void Test::LoadModels()
{
	ModelLoader modelLoader;
	modelLoader.Load3DModel(L"Resources/Model/Miku/HatsuneMiku.pmd", &_miku);
	modelLoader.Load3DModel(L"Resources/Model/Cube/Cube.obj", &_cube);
}

#pragma endregion Private Function