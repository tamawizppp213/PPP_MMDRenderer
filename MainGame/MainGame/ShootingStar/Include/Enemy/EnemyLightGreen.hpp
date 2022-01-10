//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyLightGreen.hpp
///             @brief  EnemyLightGreen
///             @author Toide Yutaro
///             @date   ---------------
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef ENEMY_LIGHT_GREEN_HPP
#define ENEMY_LIGHT_GREEN_HPP

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
class EnemyLightGreen : public Enemy
{
public:
	enum class EnemyLightGreenActionState
	{
		Appear,
		Stop,
		Look_Player,
		Shot,
		End,
		None,
		CountOfState
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize() override;
	void Update    (GameTimer& gameTimer,const Player& player) override;
	void Finalize() override;
	void Reset     () override;
	bool Generate(const gm::Vector3& position) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	EnemyLightGreen()                                  = default;
	EnemyLightGreen(const EnemyLightGreen&)            = default;
	EnemyLightGreen& operator=(const EnemyLightGreen&) = default;
	EnemyLightGreen(EnemyLightGreen&&)                 = default;
	EnemyLightGreen& operator=(EnemyLightGreen&&)      = default;
	~EnemyLightGreen()                                 = default;

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
	void LookPlayer(GameTimer& gameTimer, const Player& player);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	
	EnemyLightGreenActionState _actionState;
	const int _defaultHP = 100;

};

#endif