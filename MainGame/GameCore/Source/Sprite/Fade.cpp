//////////////////////////////////////////////////////////////////////////////////
///             @file   Fade.cpp
///             @brief  Fade 
///             @author Toide Yutaro
///             @date   2021_02_09
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12Config.hpp"
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include "GameCore/Include/Sprite/SpritePipeLineState.hpp"
#include "GameCore/Include/Sprite/Sprite.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include <d3dcompiler.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define MAX_FADE_SPRITE_COUNT 1

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
bool Fader::Initialize()
{
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	_currentFadeState = FadeState::FADE_STATE_NONE;
	if (!LoadWhiteTexture())      { return false; }
	if (!PrepareVertexBuffer())   { return false; }
	if (!PrepareIndexBuffer())    { return false; }
	if (!PrepareConstantBuffer()) { return false; }
	return true;
}

bool Fader::Draw(const DirectX::XMMATRIX& projViewMatrix)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                 = DirectX12::Instance();
	CommandList* commandList             = directX12.GetCommandList();
	SpritePSOManager& spriteManager      = SpritePSOManager::Instance();
	int currentFrameIndex                = directX12.GetCurrentFrameIndex();
	DescriptorHeapComPtr textureDescHeap = directX12.GetCbvSrvUavHeap();
	SpriteType spriteType                = SpriteType::TEXTURE;
	
	/*-------------------------------------------------------------------
	-               Update fade
	---------------------------------------------------------------------*/
	_projectionViewMatrix = projViewMatrix;
	_constantBuffer->CopyData(0, _projectionViewMatrix);

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(spriteManager.GetRootSignature(spriteType).Get());
	commandList->SetPipelineState(spriteManager.GetPipelineState(spriteType).Get());
	ID3D12DescriptorHeap* heapList[] = { textureDescHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetGraphicsRootDescriptorTable(0, directX12.GetGPUCbvSrvUavHeapStart());
	commandList->SetGraphicsRootDescriptorTable(1, _texture.GPUHandler);
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &_meshBuffer[currentFrameIndex].VertexBufferView());
	commandList->IASetIndexBuffer(&_meshBuffer[currentFrameIndex].IndexBufferView());
	commandList->OMSetRenderTargets(1, &directX12.GetCurrentRenderTargetView(), FALSE, &directX12.GetDepthStencilView());
	commandList->RSSetViewports(1, &directX12.GetViewport());
	commandList->RSSetScissorRects(1, &directX12.GetScissorRect());

	/*-------------------------------------------------------------------
	-               Add vertex data
	---------------------------------------------------------------------*/
	for (int i = 0; i < MAX_FADE_SPRITE_COUNT; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
		//	_dynamicVertexBuffer[currentFrameIndex]->CopyData((i * 4 + j, spriteList[i].Vertices[j]);
		}
	}

	/*-------------------------------------------------------------------
	-               Draw vertex data
	---------------------------------------------------------------------*/
	commandList->DrawIndexedInstanced((UINT)(6 * MAX_FADE_SPRITE_COUNT), 1, _meshBuffer[currentFrameIndex].StartIndexLocation, 0, 0);
	
	return true;
}

bool Fader::Finalize()
{
	return true;
}
#pragma endregion Public Function

#pragma region Private Function
bool Fader::LoadWhiteTexture()
{
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/White.png", _texture);
	
	return true;
}

bool Fader::PrepareVertexBuffer()
{
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	DirectX12& directX12            = DirectX12::Instance();

	_meshBuffer.resize(FRAME_BUFFER_COUNT);
	for (int i = 0; i < _meshBuffer.size(); ++i)
	{
		VertexPositionNormalColorTexture* vertices = new VertexPositionNormalColorTexture[4 * MAX_FADE_SPRITE_COUNT];
		_dynamicVertexBuffer[i] = std::make_shared<UploadBuffer<VertexPositionNormalColorTexture>>(directX12.GetDevice(), 4 * MAX_FADE_SPRITE_COUNT, false);

		/*-------------------------------------------------------------------
		-			Vertex data initialize
		---------------------------------------------------------------------*/
		for (int j = 0; j < 4 * MAX_FADE_SPRITE_COUNT; ++j)
		{
			vertices[j].Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[j].Normal   = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[j].Color    = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			vertices[j].UV       = DirectX::XMFLOAT2(0.0f, 0.0f);
			_dynamicVertexBuffer[i]->CopyData(j, vertices[j]);
		}

		const UINT vbByteSize = (UINT)MAX_FADE_SPRITE_COUNT* 4 * sizeof(VertexPositionNormalColorTexture);

		/*-------------------------------------------------------------------
		-			Build CPU / GPU Vertex Buffer
		---------------------------------------------------------------------*/
		_meshBuffer[i].BaseVertexLocation = 0;
		_meshBuffer[i].VertexBufferGPU = _dynamicVertexBuffer[i]->Resource();
		_meshBuffer[i].VertexByteStride = sizeof(VertexPositionNormalColorTexture);
		_meshBuffer[i].VertexBufferByteSize = vbByteSize;
		if (FAILED(D3DCreateBlob(vbByteSize, &_meshBuffer[i].VertexBufferCPU)))
		{
			::OutputDebugString(L"Can't create blob data (vertex)");
			return false;
		}
		delete[] vertices;
	}

	return true;
}

bool Fader::PrepareIndexBuffer()
{
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Create rect indices
	---------------------------------------------------------------------*/
	std::vector<UINT16> indices(MAX_FADE_SPRITE_COUNT * 6);
	UINT16 spriteIndex[] = { 0,1,3,1,2,3 };
	for (int i = 0; i < MAX_FADE_SPRITE_COUNT; ++i)
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
	_meshBuffer[0].IndexBufferGPU = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), indices.data(), ibByteSize, _meshBuffer[0].IndexBufferUploader).Resource();
	_meshBuffer[0].IndexFormat = DXGI_FORMAT_R16_UINT;
	_meshBuffer[0].IndexBufferByteSize = ibByteSize;
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

bool Fader::PrepareConstantBuffer()
{
	using namespace DirectX;
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Map Constant Buffer
	---------------------------------------------------------------------*/
	_constantBuffer = std::make_shared<UploadBuffer<XMMATRIX>>(directX12.GetDevice(), 1, true);
	_constantBuffer->CopyData(0, _projectionViewMatrix);

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View descriptor
	---------------------------------------------------------------------*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = _constantBuffer->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(XMMATRIX));

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View
	---------------------------------------------------------------------*/
	directX12.GetDevice()->CreateConstantBufferView(&cbvDesc, directX12.GetCPUCbvSrvUavHeapStart());
	return true;
}

bool Fader::UpdateFade()
{
	if (_currentFadeState == FadeState::FADE_STATE_IN)
	{

	}
	return true;
}
#pragma endregion Private Function