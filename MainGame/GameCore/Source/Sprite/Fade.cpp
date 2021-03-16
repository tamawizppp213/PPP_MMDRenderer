//////////////////////////////////////////////////////////////////////////////////
///             @file   Fade.cpp
///             @brief  Fade 
///             @author Toide Yutaro
///             @date   2021_03_17
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12Config.hpp"
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
#include <d3dcompiler.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
namespace fade
{
	using namespace DirectX;
	using MatrixBuffer = std::shared_ptr<UploadBuffer<DirectX::XMMATRIX>>;
	using VertexBuffer = std::shared_ptr<UploadBuffer<VertexPositionNormalColorTexture>>;

	struct FadeBuffer
	{
		Texture                 Texture;
		XMMATRIX                ProjectionViewMatrix = DirectX::XMMatrixIdentity();
		MatrixBuffer            ConstantBuffer       = nullptr;
		std::vector<MeshBuffer> MeshBuffer;
		VertexBuffer            DynamicVertexBuffer[FRAME_BUFFER_COUNT];
	};

	static FadeBuffer g_FadeBuffer;
	static float g_CurrentTime;
	static bool  g_IsCreatedBuffer = false;
}

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
Fader::Fader()
{

}

Fader::~Fader()
{

}

/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        bool Fader::Initialize
*  @brief     Fade Initialize
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::Initialize()
{
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	_currentFadeState = FadeState::FADE_STATE_NONE;
	if (!PrepareSprite())         { return false; }
	if (fade::g_IsCreatedBuffer)  { return true; }
	if (!LoadWhiteTexture())      { return false; }
	if (!PrepareVertexBuffer())   { return false; }
	if (!PrepareIndexBuffer())    { return false; }
	if (!PrepareConstantBuffer()) { return false; }
	fade::g_IsCreatedBuffer = true;
	return true;
}

/****************************************************************************
*                       Draw
*************************************************************************//**
*  @fn        bool Fader::Draw(const GameTimer& gameTimer, const DirectX::XMMATRIX& projViewMatrix)
*  @brief     Draw fade
*  @param[in] const GameTimer& gameTimer,
*  @param[in] const DirectX::XMMATRIX projViewMatrix
*  @return 　　bool
*****************************************************************************/
bool Fader::Draw(const GameTimer& gameTimer, const DirectX::XMMATRIX& projViewMatrix)
{
	if (_currentFadeState == FadeState::FADE_STATE_NONE) { return true; }

	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	using namespace fade;
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	SpritePSOManager& spriteManager           = SpritePSOManager::Instance();
	int currentFrameIndex                     = directX12.GetCurrentFrameIndex();
	DescriptorHeapComPtr textureDescHeap      = directX12.GetCbvSrvUavHeap();
	SpriteType spriteType                     = SpriteType::TEXTURE;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = g_FadeBuffer.MeshBuffer[currentFrameIndex].VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = g_FadeBuffer.MeshBuffer[currentFrameIndex].IndexBufferView();
	auto viewPort     = directX12.GetViewport();
	auto scissorRects = directX12.GetScissorRect();
	auto rtv          = directX12.GetCurrentRenderTargetView();
	auto dsv          = directX12.GetDepthStencilView();
	
	/*-------------------------------------------------------------------
	-               Update fade
	---------------------------------------------------------------------*/
	g_FadeBuffer.ProjectionViewMatrix = projViewMatrix;
	g_FadeBuffer.ConstantBuffer->CopyStart();
	g_FadeBuffer.ConstantBuffer->CopyData(0, g_FadeBuffer.ProjectionViewMatrix);
	g_FadeBuffer.ConstantBuffer->CopyEnd();

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(spriteManager.GetRootSignature(spriteType).Get());
	commandList->SetPipelineState(spriteManager.GetPipelineState(spriteType).Get());
	ID3D12DescriptorHeap* heapList[] = { textureDescHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetGraphicsRootDescriptorTable(0, directX12.GetGPUCbvSrvUavHeapStart());
	commandList->SetGraphicsRootDescriptorTable(1, g_FadeBuffer.Texture.GPUHandler);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
	commandList->RSSetViewports(1, &viewPort);
	commandList->RSSetScissorRects(1, &scissorRects);

	/*-------------------------------------------------------------------
	-               Add vertex data
	---------------------------------------------------------------------*/
	UpdateFade(gameTimer);

	/*-------------------------------------------------------------------
	-               Draw vertex data
	---------------------------------------------------------------------*/
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	
	return true;
}

/****************************************************************************
*                       StartFadeIn
*************************************************************************//**
*  @fn        void Fader::StartFadeIn(float inOutTime, float pauseTime)
*  @brief     Fade in start
*  @param[in] float inOutTime,
*  @param[in] float pauseTime
*  @return bool
*****************************************************************************/
void Fader::StartFadeIn(float inOutTime, float pauseTime)
{
	_inOutTime          = inOutTime;
	_pauseTime          = pauseTime;
	_color.w            = 0.0f;
	fade::g_CurrentTime = 0.0f;
	_currentFadeState = FadeState::FADE_STATE_IN;
}

/****************************************************************************
*                       AllowedGoToNextScene
*************************************************************************//**
*  @fn        bool Fader::AllowedGoToNextScene()
*  @brief     return allowed go to next scene 
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::AllowedGoToNextScene()
{
	return _allowedNextScene;
}

/****************************************************************************
*                       Finalize
*************************************************************************//**
*  @fn        bool Fader::Finalize()
*  @brief     Finalize
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::Finalize()
{
	return true;
}

/****************************************************************************
*                       SetColor
*************************************************************************//**
*  @fn        bool Fader::SetColor(const DirectX::XMFLOAT3& color)
*  @brief     Set Fade Color
*  @param[in] const DirectX::XMFLOAT3& color
*  @return 　　void
*****************************************************************************/
void Fader::SetColor(const DirectX::XMFLOAT3& color)
{
	_color.x = color.x;
	_color.y = color.y;
	_color.z = color.z;
}
#pragma endregion Public Function

#pragma region Private Function
/****************************************************************************
*                       LoadWhiteTexture
*************************************************************************//**
*  @fn        bool Fader::LoadWhiteTexture()
*  @brief     Load White Texture
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::LoadWhiteTexture()
{
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/White.png", fade::g_FadeBuffer.Texture);
	
	return true;
}

/****************************************************************************
*                       PrepareVertexBuffer
*************************************************************************//**
*  @fn        bool Fader::PrepareVertexBuffer()
*  @brief     Prepare vertex buffer. (vertexNum: 4)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::PrepareVertexBuffer()
{
	using namespace fade;
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	DirectX12& directX12            = DirectX12::Instance();

	g_FadeBuffer.MeshBuffer.resize(FRAME_BUFFER_COUNT);
	for (int i = 0; i < g_FadeBuffer.MeshBuffer.size(); ++i)
	{
		VertexPositionNormalColorTexture* vertices = new VertexPositionNormalColorTexture[4];
		g_FadeBuffer.DynamicVertexBuffer[i] = std::make_shared<UploadBuffer<VertexPositionNormalColorTexture>>(directX12.GetDevice(), 4, false);

		/*-------------------------------------------------------------------
		-			Vertex data initialize
		---------------------------------------------------------------------*/
		g_FadeBuffer.DynamicVertexBuffer[i]->CopyStart();
		for (int j = 0; j < 4; ++j)
		{
			vertices[j].Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[j].Normal   = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[j].Color    = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			vertices[j].UV       = DirectX::XMFLOAT2(0.0f, 0.0f);
			g_FadeBuffer.DynamicVertexBuffer[i]->CopyData(j, vertices[j]);
		}
		g_FadeBuffer.DynamicVertexBuffer[i]->CopyEnd();

		const UINT vbByteSize = (UINT)4 * sizeof(VertexPositionNormalColorTexture);

		/*-------------------------------------------------------------------
		-			Build CPU / GPU Vertex Buffer
		---------------------------------------------------------------------*/
		g_FadeBuffer.MeshBuffer[i].BaseVertexLocation   = 0;
		g_FadeBuffer.MeshBuffer[i].VertexBufferGPU      = g_FadeBuffer.DynamicVertexBuffer[i]->Resource();
		g_FadeBuffer.MeshBuffer[i].VertexByteStride     = sizeof(VertexPositionNormalColorTexture);
		g_FadeBuffer.MeshBuffer[i].VertexBufferByteSize = vbByteSize;
		if (FAILED(D3DCreateBlob(vbByteSize, &g_FadeBuffer.MeshBuffer[i].VertexBufferCPU)))
		{
			::OutputDebugString(L"Can't create blob data (vertex)");
			return false;
		}
		delete[] vertices;
	}

	return true;
}

/****************************************************************************
*                       PrepareIndexBuffer
*************************************************************************//**
*  @fn        bool Fader::PrepareIndexBuffer()
*  @brief     Prepare index buffer. (indexNum: 6)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::PrepareIndexBuffer()
{
	using namespace fade;
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Create rect indices
	---------------------------------------------------------------------*/
	std::vector<UINT16> indices(6);
	UINT16 spriteIndex[] = { 0,1,3,1,2,3 };
	for (int i = 0; i < 6; ++i)
	{
		indices[i] = spriteIndex[i];
	}

	/*-------------------------------------------------------------------
	-			Build CPU / GPU Index Buffer
	---------------------------------------------------------------------*/
	const UINT ibByteSize = (UINT)indices.size() * sizeof(UINT16);
	g_FadeBuffer.MeshBuffer[0].IndexBufferGPU = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), indices.data(), ibByteSize, g_FadeBuffer.MeshBuffer[0].IndexBufferUploader).Resource();
	g_FadeBuffer.MeshBuffer[0].IndexFormat = DXGI_FORMAT_R16_UINT;
	g_FadeBuffer.MeshBuffer[0].IndexBufferByteSize = ibByteSize;
	if (FAILED(D3DCreateBlob(ibByteSize, &g_FadeBuffer.MeshBuffer[0].IndexBufferCPU)))
	{
		::OutputDebugString(L"Can't create blob data (index)");
		return false;
	}
	CopyMemory(g_FadeBuffer.MeshBuffer[0].IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	/*-------------------------------------------------------------------
	-		Copy the index buffer by the amount of the frame buffer.
	---------------------------------------------------------------------*/
	for (int i = 1; i < g_FadeBuffer.MeshBuffer.size(); ++i)
	{
		g_FadeBuffer.MeshBuffer[i].IndexBufferCPU      = g_FadeBuffer.MeshBuffer[0].IndexBufferCPU;
		g_FadeBuffer.MeshBuffer[i].IndexBufferGPU      = g_FadeBuffer.MeshBuffer[0].IndexBufferGPU;
		g_FadeBuffer.MeshBuffer[i].IndexBufferUploader = g_FadeBuffer.MeshBuffer[0].IndexBufferUploader;
		g_FadeBuffer.MeshBuffer[i].IndexCount          = g_FadeBuffer.MeshBuffer[0].IndexCount;
		g_FadeBuffer.MeshBuffer[i].IndexFormat         = g_FadeBuffer.MeshBuffer[0].IndexFormat;
		g_FadeBuffer.MeshBuffer[i].StartIndexLocation  = g_FadeBuffer.MeshBuffer[0].StartIndexLocation;
		g_FadeBuffer.MeshBuffer[i].IndexBufferByteSize = g_FadeBuffer.MeshBuffer[0].IndexBufferByteSize;
	}
	return true;
}

/****************************************************************************
*                       PrepareConstantBuffer
*************************************************************************//**
*  @fn        bool Fader::PrepareConstantBuffer()
*  @brief     Prepare constant buffer. 
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::PrepareConstantBuffer()
{
	using namespace DirectX;
	using namespace fade;
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Map Constant Buffer
	---------------------------------------------------------------------*/
	g_FadeBuffer.ConstantBuffer = std::make_shared<UploadBuffer<XMMATRIX>>(directX12.GetDevice(), 1, true);
	g_FadeBuffer.ConstantBuffer->CopyStart();
	g_FadeBuffer.ConstantBuffer->CopyData(0, g_FadeBuffer.ProjectionViewMatrix);
	g_FadeBuffer.ConstantBuffer->CopyEnd();

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View descriptor
	---------------------------------------------------------------------*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = g_FadeBuffer.ConstantBuffer->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(XMMATRIX));

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View
	---------------------------------------------------------------------*/
	directX12.GetDevice()->CreateConstantBufferView(&cbvDesc, directX12.GetCPUCbvSrvUavHeapStart());
	return true;
}

/****************************************************************************
*                       PrepareSprite
*************************************************************************//**
*  @fn        bool Fader::PrepareSprite()
*  @brief     Prepare sprite
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::PrepareSprite()
{
	using namespace DirectX;
	_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	_sprite.CreateSprite(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT2(2.0f, 2.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
		XMFLOAT2(0.0f, 1.0f), 
		XMFLOAT2(0.0f, 1.0f));
	
	return true;
}

/****************************************************************************
*                       UpdateFade
*************************************************************************//**
*  @fn        bool Fader::UpdateFade(const GameTimer& gameTimer)
*  @brief     Fade Action 
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::UpdateFade(const GameTimer& gameTimer)
{
	DirectX12& directX12 = DirectX12::Instance();
	int currentFrameIndex = directX12.GetCurrentFrameIndex();

	/*-------------------------------------------------------------------
	-			Fade Action
	---------------------------------------------------------------------*/
	switch (_currentFadeState)
	{
		case FadeState::FADE_STATE_IN:
		{
			FadeInAction(gameTimer);
			break;
		}
		case FadeState::FADE_STATE_PAUSE:
		{
			FadePauseAction(gameTimer);
			break;
		}
		case FadeState::FADE_STATE_OUT:
		{
			FadeOutAction(gameTimer);
			break;
		}
		default:
		{
			break;
		}
	}

	/*-------------------------------------------------------------------
	-			Copy Vertex Data
	---------------------------------------------------------------------*/
	fade::g_FadeBuffer.DynamicVertexBuffer[currentFrameIndex]->CopyStart();
	for (int i = 0; i < 4; ++i)
	{
		_sprite.Vertices[i].Color = _color;
		fade::g_FadeBuffer.DynamicVertexBuffer[currentFrameIndex]->CopyData(i, _sprite.Vertices[i]);
	}

	fade::g_FadeBuffer.DynamicVertexBuffer[currentFrameIndex]->CopyEnd();
	return true;
}

/****************************************************************************
*                       FadeInAction
*************************************************************************//**
*  @fn        bool Fader::FadeInAction(const GameTimer& gameTimer)
*  @brief     Fade In Action
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::FadeInAction(const GameTimer& gameTimer)
{
	if (_currentFadeState != FadeState::FADE_STATE_IN) { return false; }

	/*-------------------------------------------------------------------
	-			 Check if the fade-in is complete. 
	---------------------------------------------------------------------*/
	if (_color.w >= 1.0f)
	{
		_color.w = 1.0f;
		_currentFadeState = FadeState::FADE_STATE_PAUSE;
	}

	/*-------------------------------------------------------------------
	-			 Update alpha
	---------------------------------------------------------------------*/
	_color.w += (2.0f / (_inOutTime)) * gameTimer.DeltaTime();

	return true;
}

/****************************************************************************
*                       FadePauseAction
*************************************************************************//**
*  @fn        bool Fader::FadePauseAction(const GameTimer& gameTimer)
*  @brief     Fade Pause Action
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::FadePauseAction(const GameTimer& gameTimer)
{
	if (_currentFadeState != FadeState::FADE_STATE_PAUSE) { return false; }
	_allowedNextScene = true;

	/*-------------------------------------------------------------------
	-			 Check if the fade-pause is complete.
	---------------------------------------------------------------------*/
	if (fade::g_CurrentTime >= _pauseTime)
	{
		fade::g_CurrentTime = 0.0f;
		_currentFadeState = FadeState::FADE_STATE_OUT;
	}

	/*-------------------------------------------------------------------
	-			 Update alpha
	---------------------------------------------------------------------*/
	fade::g_CurrentTime += gameTimer.DeltaTime();

	return true;
}

/****************************************************************************
*                       FadeOutAction
*************************************************************************//**
*  @fn        bool Fader::FadeOutAction(const GameTimer& gameTimer)
*  @brief     Fade Out Action
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Fader::FadeOutAction(const GameTimer& gameTimer)
{
	if (_currentFadeState != FadeState::FADE_STATE_OUT) { return false; }
	
	/*-------------------------------------------------------------------
	-			 Check if the fade-out is complete.
	---------------------------------------------------------------------*/
	if (_color.w <= 0.0f)
	{
		_color.w = 0.0f;
		_currentFadeState = FadeState::FADE_STATE_NONE;
	}

	/*-------------------------------------------------------------------
	-			 Update alpha
	---------------------------------------------------------------------*/
	_color.w -= (2.0f / (_inOutTime)) * gameTimer.DeltaTime();
	return true;
}
#pragma endregion Private Function