//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyBlue.hpp
///             @brief  EnemyBlue
///             @author Toide Yutaro
///             @date   -----------------
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef ENEMY_BLUE_HPP
#define ENEMY_BLUE_HPP

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
class EnemyBlue : public Enemy
{
public:
	enum class EnemyBlueActionState
	{
		Appear,
		Stop,
		Shot,
		End,
		None,
		CountOfActionState
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize() override;
	void Update    (GameTimer& gameTimer, const Player& player) override;
	void Reset     () override;
	bool Generate(const gm::Vector3& position) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	EnemyBlue()                            = default;
	EnemyBlue(const EnemyBlue&)            = default;
	EnemyBlue& operator=(const EnemyBlue&) = default;
	EnemyBlue(EnemyBlue&&)                 = default;
	EnemyBlue& operator=(EnemyBlue&&)      = default;
	~EnemyBlue()                           = default;

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
	void Shot(GameTimer& gameTimer);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	EnemyBlueActionState _actionState;
	gm::Vector3 _accel;

	const float _defaultHP = 80;
};

#endif