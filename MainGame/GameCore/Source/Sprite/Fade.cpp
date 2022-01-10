//////////////////////////////////////////////////////////////////////////////////
///             @file   Fade.cpp
///             @brief  Fade 
///             @author Toide Yutaro
///             @date   2021_03_17
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
#include <d3dcompiler.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#pragma region Public Function
Fader::Fader()
{

}

Fader::~Fader()
{
	
	for (auto& buffer : _meshBuffer) { buffer.Dispose(); }
	for (auto& buffer : _vertexBuffer) { buffer.get()->Resource()->Release(); buffer.reset(); }
	_constantBuffer.get()->Resource()->Release();
	_constantBuffer.reset();
	_meshBuffer.clear();
	_meshBuffer.shrink_to_fit();
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
	if (_isInitialized) { return true; }
	if (!PrepareSprite())         { return false; }
	if (!LoadWhiteTexture())      { return false; }
	if (!PrepareVertexBuffer())   { return false; }
	if (!PrepareIndexBuffer())    { return false; }
	if (!PrepareConstantBuffer()) { return false; }
	_isInitialized = true;
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
bool Fader::Draw(const GameTimer& gameTimer, const Matrix4& projViewMatrix)
{
	if (_currentFadeState == FadeState::FADE_STATE_NONE) { return true; }

	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	SpritePSOManager& spriteManager           = SpritePSOManager::Instance();
	int currentFrameIndex                     = directX12.GetCurrentFrameIndex();
	DescriptorHeapComPtr textureDescHeap      = directX12.GetCbvSrvUavHeap();
	SpriteType spriteType                     = SpriteType::TEXTURE;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[currentFrameIndex].VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _meshBuffer[currentFrameIndex].IndexBufferView();
	
	/*-------------------------------------------------------------------
	-               Update fade
	---------------------------------------------------------------------*/
	_constantBuffer->CopyStart();
	_constantBuffer->CopyData(0, projViewMatrix);
	_constantBuffer->CopyEnd();

	/*-------------------------------------------------------------------
	-               Add vertex data
	---------------------------------------------------------------------*/
	UpdateFade(gameTimer);

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(spriteManager.GetRootSignature(spriteType).Get());
	commandList->SetPipelineState(spriteManager.GetPipelineState(spriteType).Get());
	ID3D12DescriptorHeap* heapList[] = { textureDescHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetGraphicsRootConstantBufferView(0, _constantBuffer.get()->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(1, _texture.GPUHandler);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);

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
void Fader::StartFadeInAndOut(float inTime, float outTime, float pauseTime)
{
	_inTime             = inTime;
	_outTime            = outTime;
	_pauseTime          = pauseTime;
	_color.w            = 0.0f;
	_localTimer         = 0.0f;
	_currentFadeState = FadeState::FADE_STATE_IN;
	_isInAndOutFade = true;
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
void Fader::StartFadeIn(float inTime)
{
	_inTime           = inTime;
	_color.w          = 0.0f;
	_localTimer       = 0.0f;
	_currentFadeState = FadeState::FADE_STATE_IN;
	_isInAndOutFade   = false;
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
void Fader::StartFadeOut(float outTime)
{
	_outTime          = outTime;
	_color.w          = 1.0f;
	_localTimer       = 0.0f;
	_currentFadeState = FadeState::FADE_STATE_OUT;
	_isInAndOutFade   = false;
}

/****************************************************************************
*                       SetColor
*************************************************************************//**
*  @fn        bool Fader::SetColor(const Float3& color)
*  @brief     Set Fade Color
*  @param[in] const Float& color
*  @return 　　void
*****************************************************************************/
void Fader::SetColor(const Float3& color)
{
	_color.x = color.x;
	_color.y = color.y;
	_color.z = color.z;
}

/****************************************************************************
*                       SetSpriteSize
*************************************************************************//**
*  @fn        void Fader::SetSpriteSize(float width, float height)
*  @brief     Set sprite size
*  @param[in] float width
*  @param[in] float height
*  @return 　　void
*****************************************************************************/
void Fader::SetSpriteSize(float width, float height)
{
	_color = Float4(0.0f, 0.0f, 0.0f, 0.0f);
	_sprite.CreateSprite(
		Float3(0.0f, 0.0f, 0.0f),
		Float2(width, height),
		Float4(0.0f, 0.0f, 0.0f, 0.0f),
		Float2(0.0f, 1.0f),
		Float2(0.0f, 1.0f));
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
	textureLoader.LoadTexture(L"Resources/Texture/Default/White.png", _texture);
	
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
	SpritePSOManager& spriteManager = SpritePSOManager::Instance();
	DirectX12& directX12            = DirectX12::Instance();

	_meshBuffer.resize(FRAME_BUFFER_COUNT);
	for (int i = 0; i < _meshBuffer.size(); ++i)
	{
		VertexPositionNormalColorTexture* vertices = new VertexPositionNormalColorTexture[4];
		_vertexBuffer[i] = std::make_unique<UploadBuffer<VertexPositionNormalColorTexture>>(directX12.GetDevice(), 4, false, L"Fader::VertexBuffer");

		/*-------------------------------------------------------------------
		-			Vertex data initialize
		---------------------------------------------------------------------*/
		_vertexBuffer[i].get()->CopyStart();
		for (int j = 0; j < 4; ++j)
		{
			vertices[j].Position = gm::Float3(0.0f, 0.0f, 0.0f);
			vertices[j].Normal   = gm::Float3(0.0f, 0.0f, 0.0f);
			vertices[j].Color    = gm::Float4(0.0f, 0.0f, 0.0f, 0.0f);
			vertices[j].UV       = gm::Float2(0.0f, 0.0f);
			_vertexBuffer[i].get()->CopyData(j, vertices[j]);
		}
		_vertexBuffer[i].get()->CopyEnd();

		const UINT vbByteSize = (UINT)4 * sizeof(VertexPositionNormalColorTexture);

		/*-------------------------------------------------------------------
		-			Build CPU / GPU Vertex Buffer
		---------------------------------------------------------------------*/
		_meshBuffer[i].BaseVertexLocation   = 0;
		_meshBuffer[i].VertexBufferGPU      = _vertexBuffer[i]->Resource();
		_meshBuffer[i].VertexByteStride     = sizeof(VertexPositionNormalColorTexture);
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
	_meshBuffer[0].IndexBufferGPU      = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), indices.data(), ibByteSize, _meshBuffer[0].IndexBufferUploader, L"Fader::IndexBuffer").Resource();
	_meshBuffer[0].IndexFormat         = DXGI_FORMAT_R16_UINT;
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
	using namespace gm;
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Map Constant Buffer
	---------------------------------------------------------------------*/
	_constantBuffer = std::make_unique<UploadBuffer<Matrix4>>(directX12.GetDevice(), 1, true, L"Fader::ConstantBuffer");
	_constantBuffer->CopyStart();
	_constantBuffer->CopyData(0, MatrixIdentity());
	_constantBuffer->CopyEnd();

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View descriptor
	---------------------------------------------------------------------*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = _constantBuffer->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(Matrix4));

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View
	---------------------------------------------------------------------*/
	directX12.GetDevice()->CreateConstantBufferView(&cbvDesc, directX12.GetCPUResourceView(HeapType::CBV, directX12.IssueViewID(HeapType::CBV)));
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
	_color = Float4(0.0f, 0.0f, 0.0f, 0.0f);
	_sprite.CreateSprite(
		Float3(0.5f, 0.0f, 0.0f),
		Float2(2.0f, 2.0f),
		Float4(0.0f, 0.0f, 0.0f, 0.0f),
		Float2(0.0f, 1.0f), 
		Float2(0.0f, 1.0f));
	
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
	_vertexBuffer[currentFrameIndex]->CopyStart();
	for (int i = 0; i < 4; ++i)
	{
		_sprite.Vertices[i].Color = _color;
		_vertexBuffer[currentFrameIndex]->CopyData(i, _sprite.Vertices[i]);
	}

	_vertexBuffer[currentFrameIndex]->CopyEnd();
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
		if (_isInAndOutFade)
		{
			_currentFadeState = FadeState::FADE_STATE_PAUSE;
		}
		else
		{
			_currentFadeState = FadeState::FADE_STATE_NONE;
		}
		return true;
	}

	/*-------------------------------------------------------------------
	-			 Update alpha
	---------------------------------------------------------------------*/
	_color.w += (1.0f / (_inTime)) * gameTimer.DeltaTime();

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

	/*-------------------------------------------------------------------
	-			 Check if the fade-pause is complete.
	---------------------------------------------------------------------*/
	if (_localTimer >= _pauseTime)
	{
		_localTimer = 0.0f;
		_currentFadeState = FadeState::FADE_STATE_OUT;
		return true;
	}

	/*-------------------------------------------------------------------
	-			 Update alpha
	---------------------------------------------------------------------*/
	_localTimer += gameTimer.DeltaTime();

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
		return true;
	}

	/*-------------------------------------------------------------------
	-			 Update alpha
	---------------------------------------------------------------------*/
	_color.w -= 1.0f / _outTime * gameTimer.DeltaTime();
	return true;
}
#pragma endregion Private Function