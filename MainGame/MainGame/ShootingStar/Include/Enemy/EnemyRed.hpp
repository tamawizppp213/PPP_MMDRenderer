//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyRed.hpp
///             @brief  EnemyRed
///             @author Toide Yutaro
///             @date   2021_12_11
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef ENEMY_RED_HPP
#define ENEMY_RED_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/Enemy.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			Enemy
*************************************************************************//**
*  @class     Enemy
*  @brief     Enemy base class
*****************************************************************************/
class EnemyRed : public Enemy
{
public:
	enum class EnemyRedActionState
	{
		Appear,
		Stop,
		Move,
		Barrier,
		AttackPrepare,
		Attack,
		AttackAfter,
		End,
		None,
		CountOfState
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize() override;
	void Update(GameTimer& gameTimer, const Player& player) override;
	void Reset() override;
	bool Generate(const gm::Vector3& position) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool IsBarrier() { return _isBarrier; }
	Texture& GetBarrierTexture() { return _barrierTexture; }
	Sprite& GetBarrierSprite()   { return _barrierSprite; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	EnemyRed() = default;
	EnemyRed(const EnemyRed&)            = default;
	EnemyRed& operator=(const EnemyRed&) = default;
	EnemyRed(EnemyRed&&)                 = default;
	EnemyRed& operator=(EnemyRed&&)      = default;
	~EnemyRed()                          = default;

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
	void Move(GameTimer& gameTimer);
	void AttackPrepare(GameTimer& gameTimer, const Player& player);
	void Attack(GameTimer& gameTimer);
	void AttackAfter(GameTimer& gameTimer);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	EnemyRedActionState _actionState;
	int _moveCount = 0;
	float _lerpT = 0.0f;

	// barrier
	bool _isBarrier = false;
	Texture _barrierTexture;
	Sprite  _barrierSprite;
	int     _barrierIndex;

	// player
	gm::Vector3 _previousPlayerPosition;
	gm::Vector3 _previousEnemyPosition;

	const int _defaultHP = 100;
};

#endif