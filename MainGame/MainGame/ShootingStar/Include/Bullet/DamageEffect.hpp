//////////////////////////////////////////////////////////////////////////////////
///             @file   DamageEffect.hpp
///             @brief  Damage effect
///             @author Toide Yutaro
///             @date   2021_12_09
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DAMAGE_EFFECT_HPP
#define DAMAGE_EFFECT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/GameActor.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include "GameMath/Include/GMVector.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
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
class DamageEffect : public GameActor
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static std::vector<DamageEffect*> AllEffects() { return _damageEffects; }
	static void AllEffectsUpdate(GameTimer& gameTimer);
	static void ClearAllEffects();
	static void ActiveEffect(const gm::Vector3& position);
	static void CreateEffects(int effectNum);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	Texture& GetTexture() { return _texture; }
	Sprite & GetSprite() { return _sprite; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DamageEffect();
	DamageEffect(const DamageEffect&)            = default;
	DamageEffect& operator=(const DamageEffect&) = default;
	DamageEffect(DamageEffect&&)                 = default;
	DamageEffect& operator=(DamageEffect&&)      = default;
	~DamageEffect() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void Update(GameTimer& gameTimer);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Sprite _sprite;
	Texture _texture;
	int     _animationIndex = 0;
	float   _localTimer = 0;
	std::unique_ptr<AudioSource> _audio;
	static std::vector<DamageEffect*> _damageEffects;
};

#endif