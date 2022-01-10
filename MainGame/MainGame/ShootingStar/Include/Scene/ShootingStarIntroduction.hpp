//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShootingStarTitle.hpp
//            Content:  Title Scene 
//             Author:  Toide Yutaro
//             Create:  2020_12_02
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SHOOTING_STAR_INTRODUCTION_HPP
#define SHOOTING_STAR_INTRODUCTION_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Core/Include/Scene.hpp"
#include "RenderResource.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class  AudioSource;
struct Sprite;
struct Texture;
class  TextRenderer;
struct TextString;
//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class ShootingStarIntroduction final : public Scene
{
	using AudioSourcePtr  = std::unique_ptr<AudioSource>;
	using TextRendererPtr = std::unique_ptr<TextRenderer>;
	using TextStringPtr   = std::unique_ptr<TextString>;

public:

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	ShootingStarIntroduction();
	~ShootingStarIntroduction();
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
	void DrawSprites();

	void OnKeyboardInput() override;
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	TextRendererPtr   _textRenderer   = nullptr;
	RenderResource    _backGround;
	AudioSourcePtr    _audioSource    = nullptr;
	std::vector<TextStringPtr> _howToManipulate;
	TextStringPtr _title           = nullptr;
	TextStringPtr _clickButtonText = nullptr;
};
#endif