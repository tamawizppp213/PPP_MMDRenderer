//////////////////////////////////////////////////////////////////////////////////
///             @file   Enemy.hpp
///             @brief  Enemy
///             @author Toide Yutaro
///             @date   2021_10_27
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
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

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
	virtual void Initialize();
	virtual void Update();
	virtual void Terminate();
	virtual void Reset();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	Collider2D&       GetColCircle()       { return _colliderCircle; }
	const Collider2D& GetColCircle() const { return _colliderCircle; }

	static int  GetTotalEnemyCount();
	static bool ClearAllEnemy();

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Enemy ()                       = default;
	Enemy(const Enemy&)            = default;
	Enemy& operator=(const Enemy&) = default;
	Enemy(Enemy&&)                 = default;
	Enemy& operator=(Enemy&&)      = default;
	~Enemy() = default;

protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Collider2D  _colliderCircle;
	gm::Vector2 _speed;
	GameTimer   _localTimer;
	Sprite      _sprite;
	Texture     _texture;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool Appear();
	bool Stop();
	bool Accelerator;
	bool Shot();
	bool IsProceedAccelerate();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	enum class ActionState
	{
		Appear,
		Stop,
		Shot,
		Accelerate,
		End,
		None,
		CountOfState
	};
	ActionState _actionState;
};

#endif