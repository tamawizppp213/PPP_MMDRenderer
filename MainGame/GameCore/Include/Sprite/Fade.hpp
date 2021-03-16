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
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(); 
	void StartFadeIn(float inOutTime, float pauseTime);
	bool Draw(const GameTimer& gameTimer, const DirectX::XMMATRIX& projViewMatrix);
	bool AllowedGoToNextScene();
	bool Finalize();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetColor(const DirectX::XMFLOAT3& color);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Fader();
	~Fader();
private:
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
	Sprite             _sprite;
	DirectX::XMFLOAT4  _color;
	FadeState          _currentFadeState;
	float              _inOutTime   = 0.0f;
	float              _pauseTime   = 0.0f;
	bool               _allowedNextScene = false;

};

#endif