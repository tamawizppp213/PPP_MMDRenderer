//////////////////////////////////////////////////////////////////////////////////
///             @file   Enemy.hpp
///             @brief  Enemy
///             @author Toide Yutaro
///             @date   --------------
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef ENEMY_HPP
#define ENEMY_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/GameActor.hpp"
#include "GameCore/Include/Collision/Collider.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include "GameMath/Include/GMVector.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyType.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class Player;


/****************************************************************************
*				  			Enemy
*************************************************************************//**
*  @class     Enemy
*  @brief     Enemy base class
*****************************************************************************/
class Enemy : public GameActor
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void Initialize() = 0;
	virtual bool Generate(const gm::Vector3& position) = 0;
	virtual void Update(GameTimer& gameTimer, const Player& player) = 0;
	virtual void Finalize() = 0;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	Collider2D&       GetColBox()       { return _colliderBox; }
	const Collider2D& GetColBox() const { return _colliderBox; }
	const Texture& GetTexture() const { return _texture; }
	const Sprite& GetSprite() const { return _sprite; }

	EnemyType GetEnemyType()            { return _enemyType; }
	// speed
	const gm::Vector3& GetSpeed() const { return _speed; }
	void SetSpeed(const gm::Vector3& speed) { _speed = speed; }
	// HP
	int  GetHP()            { return _hp; }
	void SetHP(int hp)      { _hp = hp; }
	bool IsHPZero()         { return _hp <= 0; }
	void Damage(int damage) { _hp -= damage; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Enemy () = default ;
	Enemy(const Enemy&)            = default;
	Enemy& operator=(const Enemy&) = default;
	Enemy(Enemy&&)                 = default;
	Enemy& operator=(Enemy&&)      = default;
	~Enemy() = default;

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	virtual void Reset() = 0;
	void PushBackEnemy(Enemy* enemy) { _enemies.push_back(enemy); }
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	int         _hp;
	Collider2D  _colliderBox;
	gm::Vector3 _speed;
	float       _localTimer = 0.0f;
	Sprite      _sprite;
	Texture     _texture;
	EnemyType   _enemyType;
	float       _rotation;
	const float _enemySize = 0.3f;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	static std::vector<Enemy*> _enemies;
};

#endif