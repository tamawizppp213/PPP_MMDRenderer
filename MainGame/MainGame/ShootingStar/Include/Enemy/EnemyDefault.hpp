//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyDefault.hpp
///             @brief  EnemyDefault
///             @author Toide Yutaro
///             @date   ---------------
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef ENEMY_DEFAULT_HPP
#define ENEMY_DEFAULT_HPP

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
class EnemyDefault : public Enemy
{
public:
	enum class EnemyDefaultActionState
	{
		Appear,
		Stop,
		Shot,
		Accelerate,
		End,
		None,
		CountOfState
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize() override;
	bool Generate(const gm::Vector3& position) override;
	void Update(GameTimer& gameTimer, const Player& player) override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	Collider2D&       GetColBox()       { return _colliderBox; }
	const Collider2D& GetColBox() const { return _colliderBox; }


	/*************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	EnemyDefault() {};
	EnemyDefault(const EnemyDefault&)            = default;
	EnemyDefault& operator=(const EnemyDefault&) = default;
	EnemyDefault(EnemyDefault&&)                 = default;
	EnemyDefault& operator=(EnemyDefault&&)      = default;
	~EnemyDefault() = default;

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	virtual void Reset() override;
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void Appear             (GameTimer& gameTimer);
	void Stop               (GameTimer& gameTimer);
	void Accelerator        (GameTimer& gameTimer);
	void Shot               (GameTimer& gameTimer);
	void IsProceedAccelerate(GameTimer& gameTimer);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	EnemyDefaultActionState _actionState;
	gm::Vector3 _accel;
	const float _waitTime = 0.5f;
	const int _defaultHP = 40;
};

#endif