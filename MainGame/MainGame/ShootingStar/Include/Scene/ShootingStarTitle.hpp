//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShootingStarTitle.hpp
//            Content:  Title Scene 
//             Author:  Toide Yutaro
//             Create:  2020_12_02
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SHOOTING_STAR_TITLE_HPP
#define SHOOTING_STAR_TITLE_HPP

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
class ShootingStarTitle final : public Scene
{
	using AudioSourcePtr  = std::unique_ptr<AudioSource>;
	
public:

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	ShootingStarTitle();
	~ShootingStarTitle();
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
	AudioSourcePtr    _audioSource    = nullptr;
	AudioSourcePtr    _pressSound     = nullptr;
	RenderResource    _backUI;
};
#endif