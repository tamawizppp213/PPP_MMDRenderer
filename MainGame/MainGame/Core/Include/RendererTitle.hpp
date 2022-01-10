//////////////////////////////////////////////////////////////////////////////////
//              Title:  RenderTitle.hpp
//            Content:  CoreRenderer Scene 
//             Author:  Toide Yutaro
//             Create:  2020_12_11
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef CORE_RENDERER_TITLE_HPP
#define CORE_RENDERER_TITLE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Core/Include/Scene.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class  TextRenderer;
struct TextString;
struct Sprite;
struct Texture;
class Fader;
//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class CoreRendererTitle final : public Scene
{
	using TextRendererPtr = std::unique_ptr<TextRenderer>;
	using TextStringPtr   = std::unique_ptr<TextString>;
	using FaderPtr        = std::unique_ptr<Fader>;
	struct RenderResource
	{
		std::unique_ptr<Sprite>  SpritePtr;
		std::unique_ptr<Texture> TexturePtr;
		RenderResource()
		{
			SpritePtr  = std::make_unique<Sprite>();
			TexturePtr = std::make_unique<Texture>();
		}
	};
public:

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	CoreRendererTitle();
	~CoreRendererTitle();
	void Initialize(GameTimer& gameTimer) override;
	void Draw()       override;
	void Update()     override;
	void Terminate()  override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool LoadMaterials(GameTimer& gameTimer) override;

	void UpdateTextAnimation();
	void DrawSprites();
	

	void OnKeyboardInput() override;
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	SpriteRendererPtr _spriteRenderer  = nullptr;
	TextRendererPtr   _textRenderer    = nullptr;
	RenderResource    _backGround;
	TextStringPtr     _creatorText = nullptr;
	TextStringPtr     _titleText       = nullptr;
	float             _localTimer      = 0;       // to use text animation
	FaderPtr          _faderPtr = nullptr;
	bool              _enableFadeIn = true;
	bool              _enableFadeOut = true;
	bool _firstUpdate = true;
};
#endif