//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShootingStarTitle.hpp
//            Content:  Title Scene 
//             Author:  Toide Yutaro
//             Create:  2020_12_02
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SHOOTING_STAR_GAME_OVER_HPP
#define SHOOTING_STAR_GAME_OVER_HPP

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
//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class ShootingStarGameOver final : public Scene
{
	using AudioSourcePtr = std::unique_ptr<AudioSource>;

public:

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	ShootingStarGameOver();
	~ShootingStarGameOver();
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
	RenderResource    _backGround;
	AudioSourcePtr    _audioSource = nullptr;
};
#endif