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
#include "DirectX12/Include/DirectX12Shader.hpp"
#include "DirectX12/Include/DirectX12PrimitiveGeometry.hpp"
#include "DirectX12/Include/DirectX12VertexTypes.hpp"
#include "DirectX12/Include/DirectX12BaseStruct.hpp"
#include "DirectX12/Include/DirectX12Buffer.hpp"
#include "DirectX12/Include/DirectX12MathHelper.hpp"
#include "GameMath/Include/GMSort.hpp"
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <iostream>
#include <array>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                              Struct
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
Test::Test()
{
	/*
	int array[] = { 8,5,104, 19,1,29, 16 , 74, 24,52,14};
	gm::Sort<int>::ShellSortDescend(array, 11);
	for (int i = 0; i < 11; ++i)
	{
		std::cout << array[i] << std::endl;
	}
	*/
}

Test::~Test()
{

}

void Test::Initialize(const DirectX12& directX12)
{
	_directX12   = directX12;
	_device      = _directX12.GetDevice();
	_commandList = _directX12.GetCommandList();

	_directX12.ResetCommandList();

	BuildRootSignature();
	LoadShaders();
	LoadGeometry();
	BuildPSOs();

	_directX12.CompleteInitialize();
	_directX12.FlushCommandQueue();

}

void Test::Update()
{

}

void Test::Draw()
{
	_directX12.ClearScreen();
	_directX12.GetCommandList()->SetGraphicsRootSignature(_rootSignature.Get());
	_directX12.GetCommandList()->SetPipelineState(_pipelineState.Get());
	_directX12.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_directX12.GetCommandList()->IASetVertexBuffers(0, 1, &_geometries["rect"]->VertexBufferView());
	_directX12.GetCommandList()->IASetIndexBuffer(&_geometries["rect"]->IndexBufferView());
	_directX12.GetCommandList()->DrawIndexedInstanced(_geometries["rect"]->IndexBufferByteSize / sizeof(UINT16), 1, 0, 0, 0);
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
	MeshData rect = geometryGenerator.Rect(1.0f, 1.0f, 1.0f);

	SubMeshGeometry rectSubmesh;
	rectSubmesh.IndexCount = (UINT)rect.Indices.size();
	rectSubmesh.StartIndexLocation = 0;
	rectSubmesh.BaseVertexLocation = 0;

	std::vector<VertexPositionTexture> vertices(rect.Vertices.size());
	for (UINT64 i = 0; i < rect.Vertices.size(); ++i)
	{
		vertices[i].Position = rect.Vertices[i].Position;
	}
	vertices[0].UV = DirectX::XMFLOAT2(0.0f,1.0f);
	vertices[1].UV = DirectX::XMFLOAT2(0.0f, 0.0f);
	vertices[2].UV = DirectX::XMFLOAT2(1.0f, 0.0f);
	vertices[3].UV = DirectX::XMFLOAT2(1.0f, 1.0f);

	std::vector<UINT16> indices;
	indices.insert(indices.end(), std::begin(rect.Indices), std::end(rect.Indices));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(VertexPositionTexture);
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
	geometry->VertexByteStride     = sizeof(VertexPositionTexture);
	geometry->VertexBufferByteSize = vbByteSize;
	geometry->IndexFormat          = DXGI_FORMAT_R16_UINT;
	geometry->IndexBufferByteSize  = ibByteSize;

	geometry->DrawArgs["rect"] = rectSubmesh;

	_geometries[geometry->Name] = std::move(geometry);

}

void Test::BuildRootSignature()
{
	// A root signature is a collection of descriptor tables 
	// (which feeds data other than vertices to the shader).

	// build empty root signature
	ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSignatureDesc.Create(_device.Get(), &_rootSignature);
}

void Test::BuildPSOs()
{
	/*-------------------------------------------------------------------
	-			PSO for default objects
	---------------------------------------------------------------------*/
	D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultPsoDesc = _directX12.GetDefaultPSOConfig();
	defaultPsoDesc.InputLayout = VertexPositionTexture::InputLayout;
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
	defaultPsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
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

}
void Test::BuildConstantBufferView()
{

}
#pragma endregion Protected Function

#pragma region Private Function


#pragma endregion Private Function