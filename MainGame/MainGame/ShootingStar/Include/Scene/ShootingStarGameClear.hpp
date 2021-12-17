//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShootingStarGameClear.hpp
//            Content:  Game Clear Scene 
//             Author:  Toide Yutaro
//             Create:  2020_12_02
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SHOOTING_STAR_GAME_CLEAR_HPP
#define SHOOTING_STAR_GAME_CLEAR_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Core/Include/Scene.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class  AudioSource;
struct Sprite;
struct Texture;
//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class ShootingStarGameClear final : public Scene
{
	using AudioSourcePtr = std::unique_ptr<AudioSource>;

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
public:

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	ShootingStarGameClear();
	~ShootingStarGameClear();
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
	SpriteRendererPtr _spriteRenderer = nullptr;
	RenderResource    _backGround;
	AudioSourcePtr    _audioSource = nullptr;
};
#endif