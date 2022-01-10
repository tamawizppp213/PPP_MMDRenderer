//////////////////////////////////////////////////////////////////////////////////
///             @file   SpriteRenderer.cpp
///             @brief  Sprite Renderer class 
///             @author Toide Yutaro
///             @date   2021_03_16
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "GameCore/Include/Sprite/SpritePipeLineState.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include <d3dcompiler.h>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
const int SpriteRenderer::MaxSpriteCount = 1024;
SpriteRenderer::SpriteRenderer()
{
	_projectionViewMatrix =gm::MatrixIdentity();
}

SpriteRenderer::~SpriteRenderer()
{
	
}


bool SpriteRenderer::Initialize(FastBlendStateType type, const std::wstring& addName)
{
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	
	// set name
	std::wstring name = L"";
	if (addName != L"") { name += addName; name += L"::"; }
	name += L"SpriteRenderer::";

	if (!PrepareRootSignature (name + L"RootSignature"))          { return false; }
	if (!PreparePipelineState (type, name + L"PipelineState"))    { return false; }
	if (!PrepareVertexBuffer  (name + L"VertexBuffer"))           { return false; }
	if (!PrepareIndexBuffer   (name + L"IndexBuffer"))            { return false; }
	if (!PrepareConstantBuffer(name + L"Matrix4::ConsantBuffer")) { return false; }

	return true;
}


bool SpriteRenderer::DrawStart()
{
	
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	SpritePSOManager& spriteManager           = SpritePSOManager::Instance();
	int currentFrameIndex                     = directX12.GetCurrentFrameIndex();
	SpriteType spriteType                     = SpriteType::TEXTURE;
	_textureDescHeap                          = directX12.GetCbvSrvUavHeap();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[currentFrameIndex].VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _meshBuffer[currentFrameIndex].IndexBufferView();

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(_rootSignature.Get());
	commandList->SetPipelineState(_pipelineState.Get());
	ID3D12DescriptorHeap* heapList[] = {_textureDescHeap.Get()};
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);

	commandList->SetGraphicsRootConstantBufferView(0, _constantBuffer.get()->Resource()->GetGPUVirtualAddress());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0,1,&vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	return true;
}

bool SpriteRenderer::Draw(const std::vector<Sprite>& spriteList, const Texture& texture, const gm::Matrix4& matrix)
{
	/*-------------------------------------------------------------------
	-               sprite count check
	---------------------------------------------------------------------*/
	if (_spriteStackCount + spriteList.size() > MaxSpriteCount)
	{
		MessageBox(NULL, L"The maximum number of sprites exceeded. \n If the maximum number is not exceeded, please check whether DrawEnd is being called.", L"Warning", MB_ICONWARNING);
		return false;
	}

	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12            = DirectX12::Instance();
	int currentFrameIndex           = directX12.GetCurrentFrameIndex();

	/*-------------------------------------------------------------------
	-               Check whether spriteList is empty 
	---------------------------------------------------------------------*/
	if (spriteList.empty())
	{
		return true;
	}

	/*-------------------------------------------------------------------
	-               Copy ProjectionViewMatrix
	---------------------------------------------------------------------*/
	_projectionViewMatrix = matrix;
	_constantBuffer->CopyStart();
	_constantBuffer->CopyData(0, _projectionViewMatrix);
	_constantBuffer->CopyEnd();


	/*-------------------------------------------------------------------
	-               Add vertex data 
	---------------------------------------------------------------------*/
	_dynamicVertexBuffer[currentFrameIndex]->CopyStart();
	for (int i = 0; i < min((INT64)spriteList.size(), (INT64)MaxSpriteCount - (INT64)_spriteStackCount); ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			_dynamicVertexBuffer[currentFrameIndex]->CopyData((i + _spriteStackCount) * 4 + j, spriteList[i].Vertices[j]);
		}
	}
	_dynamicVertexBuffer[currentFrameIndex]->CopyEnd();


	/*-------------------------------------------------------------------
	-               Count sprite num
	---------------------------------------------------------------------*/
	_drawCallNum++;
	_spriteStackCount += static_cast<int>(spriteList.size());
	_spriteCountList.push_back(static_cast<int>(spriteList.size()));
	_textures.push_back(texture);

	return true;
}

bool SpriteRenderer::DrawEnd()
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12     = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex    = directX12.GetCurrentFrameIndex();

	/*-------------------------------------------------------------------
	-                 Draw
	---------------------------------------------------------------------*/
	int spriteCounter = 0;
	for (int i = 0; i < _drawCallNum; ++i)
	{
		commandList->SetGraphicsRootDescriptorTable(1, _textures[i].GPUHandler);
		commandList->DrawIndexedInstanced((UINT)(6 * _spriteCountList[i]), 1, 6 * spriteCounter, 0, 1);
		spriteCounter += _spriteCountList[i];
	}

	/*-------------------------------------------------------------------
	-               Reset Stack Count 
	---------------------------------------------------------------------*/
	_spriteStackCount = 0;
	_drawCallNum      = 0;
	spriteCounter     = 0;
	_spriteCountList.clear();
	_textures.clear();
	_vertices.clear();
	
	/*-------------------------------------------------------------------
	-               Reset VertexData
	---------------------------------------------------------------------*/
	VertexPositionNormalColorTexture* vertices = new VertexPositionNormalColorTexture[4 * MaxSpriteCount];
	_dynamicVertexBuffer[currentFrameIndex]->CopyStart();
	for (int j = 0; j < 4 * min(_spriteStackCount, MaxSpriteCount); ++j)
	{
		vertices[j].Position = Float3(0.0f, 0.0f, 0.0f);
		vertices[j].Normal   = Float3(0.0f, 0.0f, 0.0f);
		vertices[j].Color    = Float4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[j].UV       = Float2(0.0f, 0.0f);
		_dynamicVertexBuffer[currentFrameIndex]->CopyData(j, vertices[j]);
	}
	_dynamicVertexBuffer[currentFrameIndex]->CopyEnd();
	delete[] vertices;
	return true;
}

bool SpriteRenderer::Finalize()
{
	/*-------------------------------------------------------------------
	-           Clear MeshBuffer
	---------------------------------------------------------------------*/
	for (auto& buffer  : _meshBuffer) { buffer.Dispose(); }
	_meshBuffer.clear(); _meshBuffer.shrink_to_fit();
	/*-------------------------------------------------------------------
	-           Clear texture resource
	---------------------------------------------------------------------*/
	for (auto& texture : _textures) 
	{ 
		if (texture.Resource)
		{
			texture.Resource = nullptr;
		}
	}
	_textures.clear(); _textures.shrink_to_fit();
	_textureDescHeap = nullptr;
	/*-------------------------------------------------------------------
	-           Clear Vertices
	---------------------------------------------------------------------*/
	_vertices.clear(); _vertices.shrink_to_fit();
	for (auto& buffer : _dynamicVertexBuffer) 
	{ 
		buffer.get()->Resource()->Release();
		buffer.reset();
	}
	
	/*-------------------------------------------------------------------
	-           Clear ConstantBuffer
	---------------------------------------------------------------------*/
	_constantBuffer.get()->Resource()->Release();
	_constantBuffer.reset();
	/*-------------------------------------------------------------------
	-           Clear SpriteCountList
	---------------------------------------------------------------------*/
	_spriteCountList.clear(); _spriteCountList.shrink_to_fit();

	/*-------------------------------------------------------------------
	-           Clear PipelineState
	---------------------------------------------------------------------*/
	_rootSignature = nullptr;
	_pipelineState = nullptr;
	return true;
}
#pragma endregion Public Function

#pragma region Private Function



/****************************************************************************
*                       PrepareVertexBuffer
*************************************************************************//**
*  @fn        bool SpriteRenderer::PrepareVertexBuffer()
*  @brief     Prepare vertex buffer by the amount of the maxSpriteCount
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool SpriteRenderer::PrepareVertexBuffer(const std::wstring& name)
{
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	DirectX12& directX12            = DirectX12::Instance();

	_meshBuffer.resize(FRAME_BUFFER_COUNT);
	for (int i = 0; i < _meshBuffer.size(); ++i)
	{
		VertexPositionNormalColorTexture* vertices = new VertexPositionNormalColorTexture[4 * MaxSpriteCount];
		_dynamicVertexBuffer[i] = std::make_unique<UploadBuffer<VertexPositionNormalColorTexture>>(directX12.GetDevice(), MaxSpriteCount * 4, false,name);

		_dynamicVertexBuffer[i]->CopyStart();
		for (int j = 0; j < MaxSpriteCount * 4; ++j)
		{
			vertices[j].Position = Float3(0.0f, 0.0f, 0.0f);
			vertices[j].Normal   = Float3(0.0f, 0.0f, 0.0f);
			vertices[j].Color    = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			vertices[j].UV       = Float2(0.0f, 0.0f);
			_dynamicVertexBuffer[i]->CopyData(j, vertices[j]);
		}
		_dynamicVertexBuffer[i]->CopyEnd();
		const UINT vbByteSize = (UINT)MaxSpriteCount * 4 * sizeof(VertexPositionNormalColorTexture);

		/*-------------------------------------------------------------------
		-			Build CPU / GPU Vertex Buffer
		---------------------------------------------------------------------*/
		_meshBuffer[i].BaseVertexLocation   = 0;
		_meshBuffer[i].VertexBufferGPU      = _dynamicVertexBuffer[i]->Resource();
		_meshBuffer[i].VertexByteStride     = sizeof(VertexPositionNormalColorTexture);
		_meshBuffer[i].VertexBufferByteSize = vbByteSize;
		if (FAILED(D3DCreateBlob(vbByteSize, &_meshBuffer[i].VertexBufferCPU)))
		{
			::OutputDebugString(L"Can't create blob data (vertex)");
			return false;
		}
		//CopyMemory(_meshBuffer[i].VertexBufferCPU->GetBufferPointer(), vertices, vbByteSize);
		delete[] vertices;
	}


	return true;
}

/****************************************************************************
*                       PrepareIndexBuffer
*************************************************************************//**
*  @fn        bool SpriteRenderer::PrepareIndexBuffer()
*  @brief     Prepare index buffer by the amount of the maxSpriteCount
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool SpriteRenderer::PrepareIndexBuffer(const std::wstring& name)
{
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Create rect indices
	---------------------------------------------------------------------*/
	std::vector<UINT16> indices(MaxSpriteCount * 6);
	UINT16 spriteIndex[] = { 0,1,3,1,2,3 };
	for (int i = 0; i < MaxSpriteCount; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			indices[(UINT64)6 * i + j] = i * 4 + spriteIndex[j];
		}
	}

	/*-------------------------------------------------------------------
	-			Build CPU / GPU Index Buffer
	---------------------------------------------------------------------*/
	const UINT ibByteSize = (UINT)indices.size() * sizeof(UINT16);
	_meshBuffer[0].IndexBufferGPU      = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), indices.data(), ibByteSize, _meshBuffer[0].IndexBufferUploader, name).Resource();
	_meshBuffer[0].IndexFormat         = DXGI_FORMAT_R16_UINT;
	_meshBuffer[0].IndexBufferByteSize = ibByteSize;
	_meshBuffer[0].IndexCount          = (UINT)indices.size();
	if (FAILED(D3DCreateBlob(ibByteSize, &_meshBuffer[0].IndexBufferCPU)))
	{
		::OutputDebugString(L"Can't create blob data (index)");
		return false;
	}
	CopyMemory(_meshBuffer[0].IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);
	
	/*-------------------------------------------------------------------
	-		Copy the index buffer by the amount of the frame buffer.
	---------------------------------------------------------------------*/
	for (int i = 1; i < _meshBuffer.size(); ++i)
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
*                       PrepareConstantBuffer
*************************************************************************//**
*  @fn        bool SpriteRenderer::PrepareConstantBuffer()
*  @brief     Prepare constant buffer by the amount of the maxSpriteCount
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool SpriteRenderer::PrepareConstantBuffer(const std::wstring& name)
{
	using namespace DirectX;
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Map Constant Buffer
	---------------------------------------------------------------------*/

	_constantBuffer = std::make_unique<UploadBuffer<Matrix4>>(directX12.GetDevice(), 1, true, name);
	_constantBuffer->CopyStart();
	_constantBuffer->CopyData(0, _projectionViewMatrix);
	_constantBuffer->CopyEnd();
	/*-------------------------------------------------------------------
	-			Build Constant Buffer View descriptor
	---------------------------------------------------------------------*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = _constantBuffer->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes    = CalcConstantBufferByteSize(sizeof(Matrix4));

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View 
	---------------------------------------------------------------------*/
	directX12.GetDevice()->CreateConstantBufferView(&cbvDesc, directX12.GetCPUResourceView(HeapType::CBV, directX12.IssueViewID(HeapType::CBV)));
	return true;
}

bool SpriteRenderer::PrepareRootSignature(const std::wstring& name)
{
	/*-------------------------------------------------------------------
	-			Build texture table
	---------------------------------------------------------------------*/
	DESCRIPTOR_RANGE textureTable[2];
	textureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0); // b0
	textureTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // s0

	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[2];
	rootParameter[0].InitAsConstantBufferView(0,D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[1].InitAsDescriptorTable(1, &textureTable[1], D3D12_SHADER_VISIBILITY_PIXEL);

	/*-------------------------------------------------------------------
	-			Build sampler desc
	---------------------------------------------------------------------*/
	auto samplerDesc = GetStaticSamplers(); // linearsampler

	/*-------------------------------------------------------------------
	-			 Build root parameter
	---------------------------------------------------------------------*/
	// A root signature is a collection of descriptor tables 
	// (which feeds data other than vertices to the shader).
	ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, (UINT)samplerDesc.size(), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &_rootSignature);
	_rootSignature->SetName(name.c_str());
	return true;
}

bool SpriteRenderer::PreparePipelineState(FastBlendStateType type, const std::wstring& name)
{

	/*-------------------------------------------------------------------
	-			 Build PSO 
	---------------------------------------------------------------------*/
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
	pipeLineState.InputLayout    = VertexPositionNormalColorTexture::InputLayout;
	pipeLineState.pRootSignature = _rootSignature.Get();
	pipeLineState.VS =
	{
		reinterpret_cast<BYTE*>(GetShaderBlendData(type).VertexShader->GetBufferPointer()),
		GetShaderBlendData(type).VertexShader->GetBufferSize()
	};
	pipeLineState.PS =
	{
		reinterpret_cast<BYTE*>(GetShaderBlendData(type).PixelShader->GetBufferPointer()),
		GetShaderBlendData(type).PixelShader->GetBufferSize()
	};
	pipeLineState.BlendState = GetBlendDesc(type);
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipeLineState, IID_PPV_ARGS(&_pipelineState)));

	_pipelineState->SetName(name.c_str());

	return true;

}
#pragma endregion Private Function