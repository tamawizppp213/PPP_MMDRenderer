//////////////////////////////////////////////////////////////////////////////////
///             @file   Player.hpp
///             @brief  Player
///             @author Toide Yutaro
///             @date   2021_12_10
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PLAYER_HPP
#define PLAYER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/GameActor.hpp"
#include "GameCore/Include/Collision/Collider.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include "GameMath/Include/GMVector.hpp"
#include "GameCore/Include/Input/GameInput.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define PLAYER_SIZE (0.2f)

/****************************************************************************
*				  			Enemy
*************************************************************************//**
*  @class     Enemy
*  @brief     Enemy base class
*****************************************************************************/
class Player final : public GameActor
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize();
	void Update(GameTimer& gameTimer);

	void Damage();


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	Collider2D&       GetColBox()       { return _colliderBox; }
	const Collider2D& GetColBox() const { return _colliderBox; }
	Texture&          GetTexture  ()       { return _texture; }
	Sprite&           GetSprite   ()       { return _sprite; }
	
	int  GetHP()             { return _hp; }
	int  GetAttackPower()    { return _attack; }
	bool GetIsClear()        { return _isClear; }
	void SetIsClear(bool isClear) { _isClear = isClear; }
	bool HasMotionFinished() { return _hasMotionFinished; }
	bool HasDamaged       () { return _hasDamaged; }

	void SetAttack(int attackPower) { _attack = attackPower; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Player();
	Player(const Player&)            = default;
	Player& operator=(const Player&) = default;
	Player(Player&&)                 = default;
	Player& operator=(Player&&)      = default;
	~Player()                        = default;

protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Collider2D  _colliderBox;
	gm::Vector3 _speed;
	gm::Vector3 _accel;
	Sprite      _sprite;
	Texture     _texture;
	int         _hp;
	int         _attack;
	bool        _hasDamaged;
	bool        _hasMotionFinished;
	bool        _isClear;
	int _animationIndex = 0;

	const int  _defaultStartHP = 3;
	GameInput& _gameInput = GameInput::Instance();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	float _localTimer = 0;
};

#endif