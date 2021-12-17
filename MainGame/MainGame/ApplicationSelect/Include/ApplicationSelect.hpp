//////////////////////////////////////////////////////////////////////////////////
//              Title:  RenderTitle.hpp
//            Content:  CoreRenderer Scene 
//             Author:  Toide Yutaro
//             Create:  2020_12_11
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef APPLICATION_SELECT_HPP
#define APPLICATION_SELECT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Core/Include/Scene.hpp"
#include <vector>

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
class ApplicationSelect final : public Scene
{
	using TextRendererPtr = std::unique_ptr<TextRenderer>;
	using TextStringPtr   = std::unique_ptr<TextString>;
	using AudioSourcePtr  = std::unique_ptr<AudioSource>;
	struct RenderResource
	{
		std::unique_ptr<Sprite>  SpritePtr;
		std::unique_ptr<Texture> TexturePtr;
		RenderResource()
		{
			SpritePtr = std::make_unique<Sprite>();
			TexturePtr = std::make_unique<Texture>();
		}
	};
	enum class ApplicationTitle
	{
		ShootingStar,
		MMDRenderer,
		CountOfTitle
	};
public:

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	ApplicationSelect();
	~ApplicationSelect();
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
	void OnKeyboardInput() override;
	void DrawSprites();
	void SceneTransition();
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	SpriteRendererPtr _spriteRenderer = nullptr;
	TextRendererPtr   _textRenderer   = nullptr;
	
	RenderResource _rightArrow;
	RenderResource _leftArrow;
	std::vector<RenderResource> _titleScreen;
	AudioSourcePtr _decisionSound = nullptr;
	AudioSourcePtr _moveSound[8];

	int   _stageSelectIndex = 0;
	int   _moveSoundIndex = 0;
	float _localTimer = 0;       // to use text animation
};
#endif