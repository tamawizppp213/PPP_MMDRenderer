//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShootingStarGame.hpp
//            Content:  Shooting Star Game
//             Author:  Toide Yutaro
//             Create:  2021_12_11
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SHOOTING_STAR_GAME_HPP
#define SHOOTING_STAR_GAME_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Core/Include/Scene.hpp"
#include "RenderResource.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class  AudioSource;
struct Sprite;
struct Texture;
class  Player;
class EnemyManager;

#define BACK_GROUND_WAIT_TIME (3.0f)
//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class ShootingStarGame final : public Scene
{
	using AudioSourcePtr = std::unique_ptr<AudioSource>;
	using PlayerPtr      = std::unique_ptr<Player>;

public:

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	ShootingStarGame() ;
	~ShootingStarGame();
	void Initialize(GameTimer& gameTimer) override;
	void Draw()       override;
	void Update()     override;
	void Terminate()  override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
protected:
	/****************************************************************************
	**                Protected Function
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
	PlayerPtr         _player      = nullptr;
	RenderResource    _redArrow;

	float _backGroundAnimationTime = 0.0f;
	bool  _hasGeneratedBullet = false;
	const int _bulletsBufferNum = 8;
	const int _enemyBulletsBufferNum = 100; // ê¬Ç∆óŒÇªÇÍÇºÇÍ
	const int _effectBufferNum = 8;
private:
	/****************************************************************************
	**                Pritected Function
	*****************************************************************************/
	void UpdateBackGround();
	void CollisionDetections();
};
#endif