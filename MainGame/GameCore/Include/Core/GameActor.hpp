//////////////////////////////////////////////////////////////////////////////////
///             @file   ZPrepass.hpp
///             @brief  ZPrepass Class
///             @author Toide Yutaro
///             @date   2021_07_30
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GAME_ACTOR_HPP
#define GAME_ACTOR_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameObject.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class ActorType
{
	PMX,
	Primitive,
	Sprite,
	CountOfActorType
};

//////////////////////////////////////////////////////////////////////////////////
//								Class 
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			GameActor
*************************************************************************//**
*  @class    GameActor (Updatable Object)
*  @brief    GameObject (base class in the all object class)
*****************************************************************************/
class GameActor : public GameObject
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	int GetActorType() { return static_cast<int>(_actorType); }

	/****************************************************************************
	**               Constructor and Destructor
	*****************************************************************************/
	GameActor();
	GameActor(const GameActor&)            = default;
	GameActor& operator=(const GameActor&) = default;
	GameActor(GameActor&&)                 = default;
	GameActor& operator=(GameActor&&)      = default;
	~GameActor() {};

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	void SetActorType(ActorType type) { _actorType = type; }

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	ActorType _actorType;

private:

};

#endif