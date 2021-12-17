//////////////////////////////////////////////////////////////////////////////////
///             @file   PlayerHP.hpp
///             @brief  Player HP UI
///             @author Toide Yutaro
///             @date   -----------------
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PLAYER_HP_UI_HPP
#define PLAYER_HP_UI_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/GameActor.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include "GameMath/Include/GMVector.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class UIPlayerHP : public GameActor
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static void ClearAllPlayerHP();
	static void GenerateUI(const gm::Vector3& startPosition, int hp, float heightSize);
	static bool ManageUI(int currentHP);
	static std::vector<UIPlayerHP*>& GetAllUI() { return _hpUI; }
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const Texture& GetTexture() const { return _texture; };
	const Sprite&  GetSprite () const { return _sprite; }
	void SetPosition(const gm::Vector3& position) { _transform.LocalPosition = position; }


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	UIPlayerHP(const gm::Vector3& position, float size);
	UIPlayerHP(const UIPlayerHP&)            = default;
	UIPlayerHP& operator=(const UIPlayerHP&) = default;
	UIPlayerHP(UIPlayerHP&&)                 = default;
	UIPlayerHP& operator=(UIPlayerHP&&)      = default;
	~UIPlayerHP() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Texture _texture;
	Sprite  _sprite;
	float   _size;

	static std::vector<UIPlayerHP*> _hpUI;
};
#endif