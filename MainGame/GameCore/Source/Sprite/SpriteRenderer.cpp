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
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12Debug.hpp"
#include "GameCore/Include/Sprite/SpritePipeLineState.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/Core/DirectX12Config.hpp"
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include <d3dcompiler.h>
#include <iostream>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
const int SpriteRenderer::MaxSpriteCount = 1024;
SpriteRenderer::SpriteRenderer()
{
	_projectionViewMatrix = DirectX::XMMatrixIdentity();
}

SpriteRenderer::~SpriteRenderer()
{

}


bool SpriteRenderer::Initialize()
{
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	if (!PrepareVertexBuffer())   { return false; }
	if (!PrepareIndexBuffer())    { return false; }
	if (!PrepareConstantBuffer()) { return false; }

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
	auto rtv          = directX12.GetCurrentRenderTargetView();
	auto dsv          = directX12.GetDepthStencilView();
	auto viewport     = directX12.GetViewport();
	auto scissorRects = directX12.GetScissorRect();

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(spriteManager.GetRootSignature(spriteType).Get());
	commandList->SetPipelineState(spriteManager.GetPipelineState(spriteType).Get());
	ID3D12DescriptorHeap* heapList[] = {_textureDescHeap.Get()};
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetGraphicsRootDescriptorTable(0, directX12.GetGPUCbvSrvUavHeapStart());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0,1,&vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRects);
	return true;
}
bool SpriteRenderer::Draw(const std::vector<Sprite>& spriteList, const Texture& texture, const DirectX::XMMATRIX& matrix)
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
	CommandList* commandList        = directX12.GetCommandList();
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	int currentFrameIndex           = directX12.GetCurrentFrameIndex();
	SpriteType spriteType           = spriteList[0].GetSpriteType();
	_textureDescHeap                = directX12.GetCbvSrvUavHeap();
	auto rtv          = directX12.GetCurrentRenderTargetView();
	auto dsv          = directX12.GetDepthStencilView();
	auto viewport     = directX12.GetViewport();
	auto scissorRects = directX12.GetScissorRect();

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
	for (int i = 0; i < min(spriteList.size(), (INT64)MaxSpriteCount - (INT64)_spriteStackCount); ++i)
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
		vertices[j].Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertices[j].Normal   = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertices[j].Color    = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
		vertices[j].UV       = DirectX::XMFLOAT2(0.0f, 0.0f);
		_dynamicVertexBuffer[currentFrameIndex]->CopyData(j, vertices[j]);
	}
	_dynamicVertexBuffer[currentFrameIndex]->CopyEnd();
	delete[] vertices;
	return true;
}

bool SpriteRenderer::Finalize()
{

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
bool SpriteRenderer::PrepareVertexBuffer()
{
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	DirectX12& directX12            = DirectX12::Instance();

	_meshBuffer.resize(FRAME_BUFFER_COUNT);
	for (int i = 0; i < _meshBuffer.size(); ++i)
	{
		VertexPositionNormalColorTexture* vertices = new VertexPositionNormalColorTexture[4 * MaxSpriteCount];
		_dynamicVertexBuffer[i] = std::make_shared<UploadBuffer<VertexPositionNormalColorTexture>>(directX12.GetDevice(), MaxSpriteCount * 4, false);

		_dynamicVertexBuffer[i]->CopyStart();
		for (int j = 0; j < MaxSpriteCount * 4; ++j)
		{
			vertices[j].Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[j].Normal   = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[j].Color    = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
			vertices[j].UV       = DirectX::XMFLOAT2(0.0f, 0.0f);
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
bool SpriteRenderer::PrepareIndexBuffer()
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
	_meshBuffer[0].IndexBufferGPU      = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), indices.data(), ibByteSize, _meshBuffer[0].IndexBufferUploader).Resource();
	_meshBuffer[0].IndexFormat         = DXGI_FORMAT_R16_UINT;
	_meshBuffer[0].IndexBufferByteSize = ibByteSize;
	_meshBuffer[0].IndexCount          = indices.size();
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
bool SpriteRenderer::PrepareConstantBuffer()
{
	using namespace DirectX;
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Map Constant Buffer
	---------------------------------------------------------------------*/
	_constantBuffer = std::make_shared<UploadBuffer<XMMATRIX>>(directX12.GetDevice(), 1, true);
	_constantBuffer->CopyStart();
	_constantBuffer->CopyData(0, _projectionViewMatrix);
	_constantBuffer->CopyEnd();
	/*-------------------------------------------------------------------
	-			Build Constant Buffer View descriptor
	---------------------------------------------------------------------*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = _constantBuffer->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes    = CalcConstantBufferByteSize(sizeof(XMMATRIX));

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View 
	---------------------------------------------------------------------*/
	directX12.GetDevice()->CreateConstantBufferView(&cbvDesc, directX12.GetCPUCbvSrvUavHeapStart());
	return true;
}

#pragma endregion Private Function