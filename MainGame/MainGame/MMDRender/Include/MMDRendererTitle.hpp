//////////////////////////////////////////////////////////////////////////////////
//              Title:  MMDRenderTitle.hpp
//            Content:  Title Scene 
//             Author:  Toide Yutaro
//             Create:  2020_11_06
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MMD_RENDERER_TITLE_HPP
#define MMD_RENDERER_TITLE_HPP

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
class AudioSource;
//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class MMDRendererTitle final : public Scene
{
	using TextRendererPtr = std::unique_ptr<TextRenderer>;
	using TextStringPtr   = std::unique_ptr<TextString>;
	using AudioSourcePtr = std::unique_ptr<AudioSource>;
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
	MMDRendererTitle();
	~MMDRendererTitle();
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
	SpriteRendererPtr _spriteRenderer     = nullptr;
	TextRendererPtr   _textRenderer       = nullptr;
	RenderResource    _backGround;
	RenderResource    _backUI;
	RenderResource    _titleBack;
	RenderResource    _click;
	TextStringPtr     _pressButtonText    = nullptr;
	TextStringPtr     _titleText          = nullptr;
	float             _localTimer         = 0;       // to use text animation
	bool              _animationOn        = false;

	AudioSourcePtr _pressSound = nullptr;

};
#endif