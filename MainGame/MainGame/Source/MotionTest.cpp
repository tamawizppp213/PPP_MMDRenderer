//////////////////////////////////////////////////////////////////////////////////
//              Title:  MotionTest.hpp
//            Content:  MotionTest Scene 
//             Author:  Toide Yutaro
//             Create:  2020_11_07
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Include/MotionTest.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include "DirectX12/Include/DirectX12PrimitiveGeometry.hpp"
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "DirectX12/Include/Core/DirectX12BaseStruct.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <iostream>
#include <array>
#include <memory>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
const int numFrameResources = 3;
using namespace DirectX;


//////////////////////////////////////////////////////////////////////////////////
//                              Struct
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
MotionTest::MotionTest()
{

}

MotionTest::~MotionTest()
{

}
void MotionTest::Initialize(GameTimer& gameTimer)
{
	
	_directX12.ResetCommandList();

	TestLoadShader();
	TestPrimitiveTriangle();
	TestRootSignature();
	BuildPSOs();

}

void MotionTest::Update()
{

}

void MotionTest::Draw()
{
	_directX12.ClearScreen();

	_directX12.GetCommandList()->SetGraphicsRootSignature(_rootSignature.Get());
	_directX12.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//_directX12.GetCommandList()->IASetVertexBuffers(0, 1, &_geometries["rect"]->VertexBufferView());
	//_directX12.GetCommandList()->IASetIndexBuffer(&_geometries["rect"]->IndexBufferView());
	_directX12.GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	_directX12.CompleteRendering();
}


void MotionTest::Terminate()
{

}

#pragma region Protected Function
void MotionTest::LoadShaders()
{
	_shaders["standardVS"] = CompileShader(L"Shader\\ShaderColorTest.hlsl", nullptr, "VS", "vs_5_1");
	_shaders["opaquePS"]   = CompileShader(L"Shader\\ShaderColorTest.hlsl", nullptr, "PS", "ps_5_1");
}

void MotionTest::LoadGeometry()
{
	LoadPrimitiveGeometry();
}

void MotionTest::LoadTextures()
{

}

void MotionTest::BuildRootSignature()
{
	/*-------------------------------------------------------------------
	-	Root parameter can be a table, root descriptor or root constants.
	---------------------------------------------------------------------*/
	const int slot = 2;
	ROOT_PARAMETER slotRootParameter[slot];

	/*-------------------------------------------------------------------
	-				Create descriptor table of CBVs.
	---------------------------------------------------------------------*/

	DESCRIPTOR_RANGE cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	DESCRIPTOR_RANGE cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

	/*-------------------------------------------------------------------
	-			A root signature is an array of root parameters.
	---------------------------------------------------------------------*/
	ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.Init(slot, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSigDesc.Create(_directX12.GetDevice(), &_rootSignature);
	
}

void MotionTest::BuildPSOs()
{
	/*-------------------------------------------------------------------
	-			PSO for default objects
	---------------------------------------------------------------------*/
	D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultPsoDesc = _directX12.GetDefaultPSOConfig();
	defaultPsoDesc.pRootSignature = _rootSignature.Get();
	defaultPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(_shaders["TestVS"]->GetBufferPointer()),
		_shaders["TestVS"]->GetBufferSize()
	};
	defaultPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(_shaders["TestPS"]->GetBufferPointer()),
		_shaders["TestPS"]->GetBufferSize()
	};
	defaultPsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	ThrowIfFailed(_directX12.GetDevice()->CreateGraphicsPipelineState(&defaultPsoDesc, IID_PPV_ARGS(&_PSOs["default"])));

	/*-------------------------------------------------------------------
	-			PSO for wireframe objects
	---------------------------------------------------------------------*/
	D3D12_GRAPHICS_PIPELINE_STATE_DESC wireframePsoDesc = defaultPsoDesc;
	wireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	ThrowIfFailed(_directX12.GetDevice()->CreateGraphicsPipelineState(&wireframePsoDesc, IID_PPV_ARGS(&_PSOs["wireframe"])));
}

void MotionTest::BuildRenderItem()
{
	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRitem->ObjectCBIndex      = 0;
	boxRitem->Geo                = _geometries["shapeGeo"].get();
	boxRitem->PrimitiveType      = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount         = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	_allRenderItems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World              = gm::MatrixIdentityF();
	gridRitem->ObjectCBIndex      = 1;
	gridRitem->Geo                = _geometries["shapeGeo"].get();
	gridRitem->PrimitiveType      = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount         = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	_allRenderItems.push_back(std::move(gridRitem));

	UINT objCBIndex = 2;
	for (int i = 0; i < 5; ++i)
	{
		auto leftCylRitem     = std::make_unique<RenderItem>();
		auto rightCylRitem    = std::make_unique<RenderItem>();
		auto leftSphereRitem  = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		XMMATRIX leftCylWorld  = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld  = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
		leftCylRitem->ObjectCBIndex      = objCBIndex++;
		leftCylRitem->Geo                = _geometries["shapeGeo"].get();
		leftCylRitem->PrimitiveType      = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount         = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
		rightCylRitem->ObjectCBIndex      = objCBIndex++;
		rightCylRitem->Geo                = _geometries["shapeGeo"].get();
		rightCylRitem->PrimitiveType      = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount         = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		leftSphereRitem->ObjectCBIndex      = objCBIndex++;
		leftSphereRitem->Geo                = _geometries["shapeGeo"].get();
		leftSphereRitem->PrimitiveType      = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount         = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->ObjectCBIndex      = objCBIndex++;
		rightSphereRitem->Geo                = _geometries["shapeGeo"].get();
		rightSphereRitem->PrimitiveType      = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount         = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		_allRenderItems.push_back(std::move(leftCylRitem));
		_allRenderItems.push_back(std::move(rightCylRitem));
		_allRenderItems.push_back(std::move(leftSphereRitem));
		_allRenderItems.push_back(std::move(rightSphereRitem));
	}
	for (auto& e : _allRenderItems)
	{
		_opaqueRItems.push_back(e.get());
	}

}

void MotionTest::BuildFrameResources()
{
	for (int i = 0; i < numFrameResources; ++i)
	{
		//_frameResources.push_back(std::make_unique<FrameResource>(_directX12.GetDevice(),
		//	1, (UINT)_allRenderItems.size()));
	}
}

void MotionTest::BuildDescriptorHeap()
{
	UINT objectCount    = (UINT)_opaqueRItems.size();
	UINT numDescriptors = (objectCount + 1) * numFrameResources;
	_passCbvOffset      = objectCount * numFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask       = 0;
	ThrowIfFailed(_directX12.GetDevice()->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&_cbvHeap)));
}

void MotionTest::BuildConstantBufferView()
{
	UINT objectCBByteSize = CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT objectCount      = (UINT)_opaqueRItems.size();

	for (int frameIndex = 0; frameIndex < numFrameResources; ++frameIndex)
	{
		auto objectCB = _frameResources[frameIndex]->ObjectConstantsBuffer->Resource();
		for (UINT i = 0; i < objectCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();
			cbAddress += (UINT64)i * objectCBByteSize;

			int heapIndex = frameIndex * objectCount + i;
			auto handle   = CPU_DESC_HANDLER(_cbvHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, _cbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes    = objectCBByteSize;
			_directX12.GetDevice()->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	UINT passCBByteSize = CalcConstantBufferByteSize(sizeof(SceneConstants));

	// Last three descriptors are the pass CBVs for each frame resource.
	for (int frameIndex = 0; frameIndex < numFrameResources; ++frameIndex)
	{
		auto passCB = _frameResources[frameIndex]->SceneConstantsBuffer->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		// Offset to the pass cbv in the descriptor heap.
		int heapIndex = _passCbvOffset + frameIndex;
		auto handle   = CPU_DESC_HANDLER(_cbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, _cbvSrvUavDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		_directX12.GetDevice()->CreateConstantBufferView(&cbvDesc, handle);
	}
	
}
#pragma endregion Protected Function

#pragma region Private Function
void MotionTest::LoadPrimitiveGeometry()
{
	/*-------------------------------------------------------------------
	-						Create Geometry
	---------------------------------------------------------------------*/
	GeometryGenerator   geometryGenerator;
	MeshData box      = geometryGenerator.Box(1.0f, 1.0f, 1.0f, 3, false);
	MeshData grid     = geometryGenerator.Grid(20.0f, 30.0f, 60, 40, false);
	MeshData sphere   = geometryGenerator.Sphere(0.5f, 20, 20, false);
	MeshData cylinder = geometryGenerator.Cylinder(0.5f, 0.3f, 3.0f, 20, 20, false);

	/*-------------------------------------------------------------------
	-					   Set Vertex Offset
	---------------------------------------------------------------------*/
	UINT boxVertexOffset      = 0;
	UINT gridVertexOffset     = (UINT)box.Vertices.size();
	UINT sphereVertexOffset   = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	/*-------------------------------------------------------------------
	-					   Set Index Offset
	---------------------------------------------------------------------*/
	UINT boxIndexOffset       = 0;
	UINT gridIndexOffset      = (UINT)box.Indices.size();
	UINT sphereIndexOffset    = gridIndexOffset + (UINT)grid.Indices.size();
	UINT cylinderIndexOffset  = sphereIndexOffset + (UINT)sphere.Indices.size();

	/*-------------------------------------------------------------------
	-					   Create Submesh
	---------------------------------------------------------------------*/
	SubMeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubMeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubMeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubMeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	/*-------------------------------------------------------------------
	-					   set total vertex
	---------------------------------------------------------------------*/
	auto totalVertexCount = box.Vertices.size() + grid.Vertices.size() 
		           + sphere.Vertices.size() + cylinder.Vertices.size();

	/*-------------------------------------------------------------------
	-					   set vertex data
	---------------------------------------------------------------------*/
	std::vector<VertexPositionColor> vertices(totalVertexCount);
	UINT k = 0;

	for (UINT64 i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = box.Vertices[i].Position;
		vertices[k].Color    = DirectX::XMFLOAT4(DirectX::Colors::DarkGreen);
	}

	for (UINT64 i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = grid.Vertices[i].Position;
		vertices[k].Color    = DirectX::XMFLOAT4(DirectX::Colors::ForestGreen);
	}

	for (UINT64 i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = sphere.Vertices[i].Position;
		vertices[k].Color    = DirectX::XMFLOAT4(DirectX::Colors::Crimson);
	}

	for (UINT64 i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = cylinder.Vertices[i].Position;
		vertices[k].Color    = DirectX::XMFLOAT4(DirectX::Colors::SteelBlue);
	}

	/*-------------------------------------------------------------------
	-					   set index data
	---------------------------------------------------------------------*/
	std::vector<UINT16> indices;
	indices.insert(indices.end(), std::begin(box.Indices     ), std::end(box.Indices));
	indices.insert(indices.end(), std::begin(grid.Indices    ), std::end(grid.Indices));
	indices.insert(indices.end(), std::begin(sphere.Indices  ), std::end(sphere.Indices));
	indices.insert(indices.end(), std::begin(cylinder.Indices), std::end(cylinder.Indices));

	/*-------------------------------------------------------------------
	-					   get vertex and index byte size
	---------------------------------------------------------------------*/
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(VertexPositionColor);
	const UINT ibByteSize = (UINT)indices.size()  * sizeof(UINT16);

	/*-------------------------------------------------------------------
	-					   set mesh geometry
	---------------------------------------------------------------------*/
	auto geometry = std::make_unique<MeshGeometry>();
	geometry->Name = "geometry";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geometry->VertexBufferCPU));
	CopyMemory(geometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geometry->IndexBufferCPU));
	CopyMemory(geometry->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	// create vertex and index buffer
	geometry->VertexBufferGPU = DefaultBuffer(_directX12.GetDevice(), _directX12.GetCommandList(), vertices.data(), vbByteSize, geometry->VertexBufferUploader).Resource();
	geometry->IndexBufferGPU  = DefaultBuffer(_directX12.GetDevice(), _directX12.GetCommandList(), indices.data(),  ibByteSize, geometry->IndexBufferUploader).Resource();

	// for vertex and index buffer view 
	geometry->VertexByteStride     = sizeof(VertexPositionColor);
	geometry->VertexBufferByteSize = vbByteSize;
	geometry->IndexFormat          = DXGI_FORMAT_R16_UINT;
	geometry->IndexBufferByteSize  = ibByteSize;

	geometry->DrawArgs["box"]      = boxSubmesh;
	geometry->DrawArgs["grid"]     = gridSubmesh;
	geometry->DrawArgs["sphere"]   = sphereSubmesh;
	geometry->DrawArgs["cylinder"] = cylinderSubmesh;

	_geometries[geometry->Name] = std::move(geometry);
}

void MotionTest::TestLoadShader()
{
	_shaders["TestVS"] = CompileShader(L"Shader\\ShaderColorTest.hlsl", nullptr, "BasicVS", "vs_5_1");
	_shaders["TestPS"] = CompileShader(L"Shader\\ShaderColorTest.hlsl", nullptr, "BasicPS", "ps_5_1");
}

void MotionTest::TestPrimitiveTriangle()
{
	GeometryGenerator  geometryGenerator;
	MeshData rect = geometryGenerator.Rect(1.0f, 1.0f, 1.0f);

	SubMeshGeometry rectSubmesh;
	rectSubmesh.IndexCount = (UINT)rect.Indices.size();
	rectSubmesh.StartIndexLocation = 0;
	rectSubmesh.BaseVertexLocation = 0;

	std::vector<VertexPosition> vertices(rect.Vertices.size());
	for (UINT64 i = 0; i < rect.Vertices.size(); ++i)
	{
		vertices[i].Position = rect.Vertices[i].Position;
	}

	std::vector<UINT16> indices;
	indices.insert(indices.end(), std::begin(rect.Indices), std::end(rect.Indices));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(VertexPosition);
	const UINT ibByteSize = (UINT)indices.size()  * sizeof(UINT16);

	auto geometry = std::make_unique<MeshGeometry>();
	geometry->Name = "rect";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geometry->VertexBufferCPU));
	CopyMemory(geometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geometry->IndexBufferCPU));
	CopyMemory(geometry->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	// create vertex and index buffer
	geometry->VertexBufferGPU = DefaultBuffer(_directX12.GetDevice(), _directX12.GetCommandList(), vertices.data(), vbByteSize, geometry->VertexBufferUploader).Resource();
	geometry->IndexBufferGPU  = DefaultBuffer(_directX12.GetDevice(), _directX12.GetCommandList(), indices.data(), ibByteSize, geometry->IndexBufferUploader).Resource();

	// for vertex and index buffer view 
	geometry->VertexByteStride = sizeof(VertexPosition);
	geometry->VertexBufferByteSize = vbByteSize;
	geometry->IndexFormat = DXGI_FORMAT_R16_UINT;
	geometry->IndexBufferByteSize = ibByteSize;

	geometry->DrawArgs["rect"] = rectSubmesh;

	_geometries[geometry->Name] = std::move(geometry);

}

void MotionTest::TestRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* rootSigBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob));
	ThrowIfFailed(_directX12.GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&_rootSignature)));
	rootSigBlob->Release();
}
#pragma endregion Private Function