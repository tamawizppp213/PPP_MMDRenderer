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

//////////////////////////////////////////////////////////////////////////////////
//								Class 
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			GameActor
*************************************************************************//**
*  @class    GameActor (Updatable Object)
*  @brief    GameObject (base class in the all object class)
*****************************************************************************/
class Component
{
	friend class GameObject;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	std::string GetTag() const { return _tag; }
	GameObject& GetGameObject() { return *_gameObject; }

	/****************************************************************************
	**               Constructor and Destructor
	*****************************************************************************/
	Component();
	Component(const Component&)            = default;
	Component& operator=(const Component&) = default;
	Component(Component&&)                 = default;
	Component& operator=(Component&&)      = default;
	~Component() {};

protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

private:
	GameObject* _gameObject;
	std::string _tag;
};

#endif