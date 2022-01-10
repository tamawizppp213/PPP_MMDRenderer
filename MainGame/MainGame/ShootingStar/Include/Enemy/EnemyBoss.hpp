//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyBoss.hpp
///             @brief  EnemyBoss
///             @author Toide Yutaro
///             @date   ----------------
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef ENEMY_BOSS_HPP
#define ENEMY_BOSS_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/Enemy.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class AudioSource;
/****************************************************************************
*				  			Enemy
*************************************************************************//**
*  @class     Enemy
*  @brief     Enemy base class
*****************************************************************************/
class EnemyBoss : public Enemy
{
	using AudioSourcePtr = std::unique_ptr<AudioSource>;
public:
	enum class EnemyBossActionState
	{
		Appear,
		Stop,
		GenerateEnemy,
		LaserPrepare,
		Laser,
		LaserAfter,
		Shot,
		ShotToPlayer,
		End,
		None,
		CountOfState
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize() override;
	void Update    (GameTimer& gameTimer, const Player& player) override;
	void Finalize() override;
	void Reset     () override;
	bool Generate(const gm::Vector3& position) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool     EnableShotLaser() { return _enableShotLaser; }
	bool     IsCharging     () { return _isCharge; }
	Texture& GetLaserTexture()     { return _laserTexture; }
	Sprite&  GetLaserSprite ()       { return _laserSprite; }
	Collider2D& GetLaserCollider() { return _laserColBox; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	EnemyBoss()                            = default;
	EnemyBoss(const EnemyBoss&)            = default;
	EnemyBoss& operator=(const EnemyBoss&) = default;
	EnemyBoss(EnemyBoss&&)                 = default;
	EnemyBoss& operator=(EnemyBoss&&)      = default;
	~EnemyBoss()                           = default;

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
	void LaserPrepare(GameTimer& gameTimer);
	void Laser(GameTimer& gameTimer, const Player& player);
	void LaserAfter(GameTimer& gameTimer);
	void Shot(GameTimer& gameTimer);
	void ShotToPlayer(GameTimer& gameTimer, const Player& player);
	void GenerateEnemy(GameTimer& gameTimer);

	void ProceedPrepareLaser();
	void ProceedGenerateEnemy();
	void ProceedShotToPlayer();
	void ProceedShot();
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	EnemyBossActionState _actionState;
	float _waitTime       = 2.5f;
	int   _animationIndex = 0;

	// Laser
	bool    _isCharge        = false;
	bool    _enableShotLaser = false;
	Texture _laserTexture;
	Sprite  _laserSprite;
	float   _laserStartTime = 0.1f;
	float   _laserEndTime   = 0.6f;
	Collider2D _laserColBox;
	float _laserLocalTimer = 0.0f;
	
	AudioSourcePtr _laserSound;
	AudioSourcePtr _chargeSound;

	const int _defaultHP = 1000;
};

#endif