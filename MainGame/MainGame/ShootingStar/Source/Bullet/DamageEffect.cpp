//////////////////////////////////////////////////////////////////////////////////
///             @file   DamageEffect.cpp
///             @brief  DamageEffect
///             @author Toide Yutaro
///             @date   -----------------
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Bullet/DamageEffect.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include "GameCore/Include/Screen.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
std::vector<DamageEffect*> DamageEffect::_damageEffects;

static float g_AnimationPatternTable[9] =
{
	0.05f,0.05f,0.05f,0.05f,0.05f,0.05f,0.05f,0.05f,1.0f / 60.0f
};
static int g_AnimationPattern[9] =
{
	0,1,2,3,4,5,6,7,8
};

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
DamageEffect::DamageEffect()
{
	_actorType = ActorType::Sprite;
	_name      = "DamageEffect";
	_transform = Transform();
	_isActive  = false;
	_animationIndex = 0;
	
	/*-------------------------------------------------------------------
	-          Load texture
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/explosion.png", _texture);
	_sprite.CreateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0.2f / Screen::GetAspectRatio(), 0.2f), Float2(0, 1), Float2(0, 1));

	/*-------------------------------------------------------------------
	-          Load sound
	---------------------------------------------------------------------*/
	_audio = std::make_unique<AudioSource>();
	_audio.get()->LoadSound(L"Resources/Audio/ShootingStar/bomb01.wav", SoundType::SE);

	_damageEffects.push_back(this);
}

void DamageEffect::CreateEffects(int effectNum)
{
	for (int i = 0; i < effectNum; ++i)
	{
		new DamageEffect(); // deleteはDestroy時に行う.
	}
}

void DamageEffect::AllEffectsUpdate(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-          Execute All effect update
	---------------------------------------------------------------------*/
	for (auto it = _damageEffects.begin(); it != _damageEffects.end(); ++it)
	{
		(*it)->Update(gameTimer);
	}
}

void DamageEffect::Update(GameTimer& gameTimer)
{
	if (!_isActive) { return; }

	_localTimer += gameTimer.DeltaTime();
	if (_localTimer >= g_AnimationPatternTable[g_AnimationPattern[_animationIndex]])
	{
		_localTimer = 0;
		_animationIndex++;
	}

	if (_animationIndex == 9)
	{
		_animationIndex = 0;
		SetActive(false);
		_localTimer = 0;
	}

	_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0.125f * g_AnimationPattern[_animationIndex], 0.125f * (g_AnimationPattern[_animationIndex] + 1)), Float2(0, 1));
}

/****************************************************************************
*                       ActiveBullet
*************************************************************************//**
*  @fn        void Bullet::ActiveBullet(const gm::Vector3& position)
*  @brief     アクティブでないものを見つけてActiveにする.
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DamageEffect::ActiveEffect(const gm::Vector3& position)
{
	for (int i = 0; i < _damageEffects.size(); ++i)
	{
		if (_damageEffects[i]->IsActive()) { continue; }

		_damageEffects[i]->SetActive(true);
		_damageEffects[i]->GetTransform().LocalPosition = position;
		_damageEffects[i]->_animationIndex = 0;
		_damageEffects[i]->_sprite.UpdateSpriteForTexture(_damageEffects[i]->_transform.LocalPosition.ToFloat3(), Float2(0.125f * g_AnimationPattern[_damageEffects[i]->_animationIndex], 0.125f * (g_AnimationPattern[_damageEffects[i]->_animationIndex] + 1)), Float2(0, 1));
		_damageEffects[i]->_audio.get()->Play();
		break;
	}
}
/****************************************************************************
*                       ClearAllEffects
*************************************************************************//**
*  @fn        void DamageEffect::ClearAllEffects()
*  @brief     Clear all effects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DamageEffect::ClearAllEffects()
{
	for (int i = 0; i < _damageEffects.size(); ++i)
	{
		Destroy(_damageEffects[i]);
	}
	_damageEffects.clear();
	_damageEffects.shrink_to_fit();
}