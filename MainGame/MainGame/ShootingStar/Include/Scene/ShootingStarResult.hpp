//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShootingStarResult.hpp
//            Content:  Result Scene 
//             Author:  Toide Yutaro
//             Create:  2020_12_04
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SHOOTING_STAR_RESULT_HPP
#define SHOOTING_STAR_RESULT_HPP

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
class ShootingStarResult final : public Scene
{
	using AudioSourcePtr  = std::unique_ptr<AudioSource>;

public:

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	ShootingStarResult();
	~ShootingStarResult();
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
};
#endif