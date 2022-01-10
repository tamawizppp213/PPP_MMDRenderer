//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyPurple.hpp
///             @brief  EnemyPurple
///             @author Toide Yutaro
///             @date   ----------------
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef ENEMY_PURPLE_HPP
#define ENEMY_PURPLE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/Enemy.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include <memory>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			Enemy
*************************************************************************//**
*  @class     Enemy
*  @brief     Enemy base class
*****************************************************************************/
class EnemyPurple : public Enemy
{
public:
	enum class EnemyPurpleActionState
	{
		Appear,
		Stop,
		Move,
		LaserPrepare,
		Laser,
		LaserAfter,
		End,
		None,
		CountOfState
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize() override;
	void Update(GameTimer& gameTimer, const Player& player) override;
	void Finalize() override;
	void Reset() override {};
	bool Generate(const gm::Vector3& position) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool EnableFireLaser() { return _enableFireLaser; }
	bool IsCharging     () { return _isCharge; }
	Texture& GetLaserTexture()  { return _laserTexture; }
	Sprite & GetLaserSprite()   { return _laserSprite; }
	Texture& GetChargeTexture() { return _chargeTexture; }
	Sprite & GetChargeSprite()  { return _chargeSprite; }
	Collider2D& GetLaserCollider() { return _laserCollider; }

	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	EnemyPurple() = default;
	EnemyPurple(const EnemyPurple&)            = default;
	EnemyPurple& operator=(const EnemyPurple&) = default;
	EnemyPurple(EnemyPurple&&)                 = default;
	EnemyPurple& operator=(EnemyPurple&&)      = default;
	~EnemyPurple()                             = default;

protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void Appear(GameTimer& gameTimer);
	void Stop(GameTimer& gameTimer);
	void Move(GameTimer& gameTimer, const Player& player);
	void LaserPrepare(GameTimer& gameTimer);
	void Laser(GameTimer& gameTimer);
	void LaserAfter(GameTimer& gameTimer, const Player& player);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	EnemyPurpleActionState _actionState;
	std::unique_ptr<AudioSource> _laserSound = nullptr;
	std::unique_ptr<AudioSource> _chargeSound = nullptr;

	// laser
	Texture _laserTexture;
	Sprite  _laserSprite;
	bool    _enableFireLaser = false;
	Collider2D _laserCollider;

	// charge
	Texture _chargeTexture;
	Sprite  _chargeSprite;
	float   _chargeRotation = 0.0f;
	bool    _isCharge = false;

	int _prepareLaserCounter = 0;
	
	const int _defaultHP = 200;
	float _lerpT = 0.0f;
};

#endif