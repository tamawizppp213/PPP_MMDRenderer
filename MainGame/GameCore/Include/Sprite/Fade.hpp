//////////////////////////////////////////////////////////////////////////////////
///             @file   Fade.hpp
///             @brief  Fade
///                     ‡@ Initialize
///                     ‡A StartFadeIn(call once)
///                     ‡B Draw
///                     ‡C Check scene change (AllowedGoToNextScene) 
///             @author Toide Yutaro
///             @date   2021_03_17
//////////////////////////////////////////////////////////////////////////////////
#pragma once 
#ifndef FADE_HPP
#define FADE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Sprite/Sprite.hpp"
#include "GameMath/Include/GMMatrix.hpp"
#include "DirectX12/Include/Core/DirectX12MeshBuffer.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include <memory>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct Texture;
class GameTimer;

//////////////////////////////////////////////////////////////////////////////////
//                             Class
//////////////////////////////////////////////////////////////////////////////////
enum class FadeState
{
	FADE_STATE_NONE,
	FADE_STATE_IN,
	FADE_STATE_PAUSE,
	FADE_STATE_OUT
};

/****************************************************************************
*				  			Fade
*************************************************************************//**
*  @class     Fade
*  @brief     Fade
*****************************************************************************/
class Fader
{
	using MatrixBuffer = std::unique_ptr<UploadBuffer<gm::Matrix4>>;
	using VertexBuffer = std::unique_ptr<UploadBuffer<VertexPositionNormalColorTexture>>;

public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(); 
	void StartFadeInAndOut(float inTime, float outTime, float pauseTime);
	void StartFadeIn (float inTime);
	void StartFadeOut(float outTime);
	bool Draw(const GameTimer& gameTimer, const gm::Matrix4& projViewMatrix);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetSpriteSize(float width = 2.0f, float height = 2.0f); // default NDC
	void SetColor(const gm::Float3& color);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Fader();
	Fader(const Fader&)            = default;
	Fader& operator=(const Fader&) = default;
	Fader(Fader&&)                 = default;
	Fader& operator=(Fader&&)      = default;
	~Fader();
protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool LoadWhiteTexture(); 
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();
	bool PrepareConstantBuffer();
	bool PrepareSprite();
	bool UpdateFade     (const GameTimer& gameTimer);
	bool FadeInAction   (const GameTimer& gameTimer);
	bool FadePauseAction(const GameTimer& gameTimer);
	bool FadeOutAction  (const GameTimer& gameTimer);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Sprite      _sprite;
	gm::Float4  _color;
	FadeState   _currentFadeState;
	float       _inTime      = 0.0f;
	float       _outTime     = 0.0f;
	float       _pauseTime   = 0.0f;
	Texture     _texture;
	MatrixBuffer            _constantBuffer = nullptr;
	std::vector<MeshBuffer> _meshBuffer;
	VertexBuffer            _vertexBuffer[FRAME_BUFFER_COUNT];
	float                   _localTimer = 0.0f;
	bool                    _isInitialized = false;
	bool _isInAndOutFade = false;
};

#endif