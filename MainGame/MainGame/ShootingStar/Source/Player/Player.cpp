//////////////////////////////////////////////////////////////////////////////////
///             @file   Player.cpp
///             @brief  Player
///             @author Toide Yutaro
///             @date   ------------------------
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Player/Player.hpp"
#include "GameCore/Include/Screen.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

static float g_DamageTime[] = { 0.2f, 0.2f,0.2f, 0.2f ,0.2f, 0.2f ,0.2f, 0.2f ,0.2f, 0.2f,2.0f / 60 };
static float g_DamageAlpha[] = { 0.3f, 1.0f,0.3f, 1.0f, 0.3f, 1.0f, 0.3f, 1.0f, 0.3f, 1.0f, 1.0f };
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
Player::Player()
{
}

void Player::Initialize()
{
	_actorType               = ActorType::Sprite;
	_transform.LocalPosition = Vector3(0.0f, -0.85f, 0.0f);
	_speed                   = Vector3(0.5f, 0.0f, 0.0f);
	_accel                   = Vector3(0.0f, 0.10f, 0.0f);
	_colliderBox             = Collider2D::CreateRectangleCollider(Float2(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY()), PLAYER_SIZE / (2.0f * Screen::GetAspectRatio()), PLAYER_SIZE / 2.0f);
	_hp                      = _defaultStartHP;
	_hasDamaged              = false;
	_hasMotionFinished       = false;
	_attack                  = 40;
	_isClear                 = false;

	TextureLoader texturLoader;
	texturLoader.LoadTexture(L"Resources/Texture/ShootingStar/player.png", _texture);
	_sprite.CreateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(PLAYER_SIZE, PLAYER_SIZE), Float2(0, 1), Float2(0, 1));
}

void Player::Update(GameTimer& gameTimer)
{
	if (!IsActive()) { return; }

	_localTimer += gameTimer.DeltaTime();
	/*-------------------------------------------------------------------
	-          Whether to perform clear motion or not
	---------------------------------------------------------------------*/
	if (!_isClear)
	{
		Vector3 position = _transform.LocalPosition;
		/*-------------------------------------------------------------------
		-          Input check
		---------------------------------------------------------------------*/
		if (_gameInput.GetKeyboard().IsPress(DIK_LEFT))
		{
			position.SetX(_transform.LocalPosition.GetX() - _speed.GetX() * gameTimer.DeltaTime());
		}
		if (_gameInput.GetKeyboard().IsPress(DIK_RIGHT))
		{
			position.SetX(_transform.LocalPosition.GetX() + _speed.GetX() * gameTimer.DeltaTime());
		}

		/*-------------------------------------------------------------------
		-          Screen range check
		---------------------------------------------------------------------*/
		if (position.GetX() < (-1.0f + PLAYER_SIZE / 2.0f))
		{
			position.SetX(-1.0f + PLAYER_SIZE / 2.0f);
		}
		if (position.GetX() > (1.0f - PLAYER_SIZE / 2))
		{
			position.SetX(1.0f - PLAYER_SIZE / 2.0f);
		}

		/*-------------------------------------------------------------------
		-         Update position and collider
		---------------------------------------------------------------------*/
		_transform.LocalPosition = position;
		_colliderBox.SetCenterPosition(Float2(position.GetX(), position.GetY()));
		/*-------------------------------------------------------------------
		-         Update sprite
		---------------------------------------------------------------------*/
		if (_hasDamaged)
		{
			if (g_DamageTime[_animationIndex] <= _localTimer)
			{
				_localTimer = 0;
				_animationIndex++;
			}
			if (_animationIndex == _countof(g_DamageTime))
			{
				_hasDamaged = false;
				_animationIndex = 0;
				_localTimer = 0;
			}
			_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0, 1), Float2(0, 1), g_DamageAlpha[_animationIndex]);
		}
		else
		{
			_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0, 1), Float2(0, 1), 1.0f);
		}
	}
	else
	{
		/*-------------------------------------------------------------------
		-          Proceed player
		---------------------------------------------------------------------*/
		_speed.SetX(0.0f);
		_speed                   += _accel * gameTimer.DeltaTime();
		_transform.LocalPosition += _speed * gameTimer.DeltaTime();
		if (_transform.LocalPosition.GetY() >= 1.0f + PLAYER_SIZE)
		{
			_hasMotionFinished = true;
		}
		/*-------------------------------------------------------------------
		-         Update sprite
		---------------------------------------------------------------------*/
		_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0, 1), Float2(0, 1), 1.0f);
	}
}

void Player::Damage()
{	
	_hasDamaged = true;
	_hp--;
	_localTimer = 0;
	if (_hp <= 0)
	{
		SetActive(false);
	}
}