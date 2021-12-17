//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyLightGreen.cpp
///             @brief  EnemyLightGreen
///             @author Toide Yutaro
///             @date   ------------------
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/EnemyLightGreen.hpp"
#include "MainGame/ShootingStar/Include/Bullet/Bullet.hpp"
#include "MainGame/ShootingStar/Include/Player/Player.hpp"
#include "GameCore/Include/Screen.hpp"
#include "GameMath/Include/GMDistribution.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define DEFAULT_SPEED_Y (0.5f)
static RandomInt g_Random;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        void EnemyLightGreen::Initialize()
*  @brief     Initialize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyLightGreen::Initialize()
{
	_actorType               = ActorType::Sprite;
	_name                    = "EnemyLightGreen";
	_isActive                = false;
	_transform.LocalPosition = Vector3(0.0f, 1.0f + _enemySize / 2.0f, 0.0f);
	_speed                   = Vector3(0.0f, DEFAULT_SPEED_Y, 0.0f);
	_colliderBox             = Collider2D::CreateRectangleCollider(Float2(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY()), _enemySize / Screen::GetAspectRatio(), _enemySize);
	_actionState             = EnemyLightGreenActionState::None;
	_enemyType               = EnemyType::LightGreen;
	_rotation                = GM_PI;
	_hp = _defaultHP;
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/enemyLightGreen.png", _texture);
	_sprite.CreateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(_enemySize/Screen::GetAspectRatio(), _enemySize), Float2(0, 1), Float2(0, 1), GM_PI);

	g_Random.SetRange(0, 1);

	PushBackEnemy(this);
}

void EnemyLightGreen::Update(GameTimer& gameTimer, const Player& player)
{
	if (!IsActive()) { return; }

	_localTimer += gameTimer.DeltaTime();
	switch (_actionState)
	{
	case EnemyLightGreenActionState::Appear:
	{
		Appear(gameTimer);
		break;
	}
	case EnemyLightGreenActionState::Stop:
	{
		Stop(gameTimer);
		break;
	}
	case EnemyLightGreenActionState::Look_Player:
	{
		LookPlayer(gameTimer, player);
		break;
	}
	case EnemyLightGreenActionState::Shot:
	{
		Shot(gameTimer);
		break;
	}

	default: break;
	}

	_colliderBox.centerPosition = _transform.LocalPosition.ToFloat3();
	_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0, 1), Float2(0, 1), 1.0f, _rotation);
}

void EnemyLightGreen::Reset()
{

}
bool EnemyLightGreen::Generate(const Vector3& position)
{
	if (!IsActive())
	{
		_actionState = EnemyLightGreenActionState::Appear;
		_transform.LocalPosition = position;
		_colliderBox.centerPosition = Float3(position.GetX(), position.GetY(), position.GetZ());
		_speed = Vector3(0.0f, DEFAULT_SPEED_Y, 0.0f);
		_rotation = GM_PI;
		_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0, 1), Float2(0, 1), 1.0f, _rotation);
		_hp = _defaultHP;
		SetActive(true);
		return true;
	}
	else
	{
		return false;
	}
}
/****************************************************************************
*                       Appear
*************************************************************************//**
*  @fn        void EnemyLightGreen::Appear(GameTimer& gameTimer)
*  @brief     Appear an enemy to the preDecided position/
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyLightGreen::Appear(GameTimer& gameTimer)
{
	_transform.LocalPosition -= _speed * gameTimer.DeltaTime();

	if (_transform.LocalPosition.GetY() <= 0.7f)
	{
		_transform.LocalPosition.SetY(0.7f);
		_actionState = EnemyLightGreenActionState::Stop;
		_localTimer = 0.0f;
	}
}
/****************************************************************************
*                       Stop
*************************************************************************//**
*  @fn        void EnemyDefault::Stop(GameTimer& gameTimer)
*  @brief     Stop enemy till wait time
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyLightGreen::Stop(GameTimer& gameTimer)
{
	_speed.SetY(0.0f);
	if (_localTimer >= 1.0f)
	{
		_actionState = EnemyLightGreenActionState::Look_Player;
		_localTimer  = 0.0f;
	}
}

void EnemyLightGreen::LookPlayer(GameTimer& gameTimer, const Player& player)
{
	// プレイヤー座標を取得する
	Vector3 temp = Normalize(player.GetTransform().LocalPosition - _transform.LocalPosition);
	_rotation = -ATan2(temp.GetY(), temp.GetX()) + GM_PI / 2;

	if (_localTimer >= 1.6f)
	{
		_actionState = EnemyLightGreenActionState::Shot;
		_localTimer = 0.0f;
	}

}
/****************************************************************************
*                       Shot
*************************************************************************//**
*  @fn        void EnemyDefault::Shot(GameTimer& gameTimer)
*  @brief     Shot enemy blue bullet
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyLightGreen::Shot(GameTimer& gameTimer)
{
	float speed = 1.4f;
	
	for (int i = 0; i < 5; ++i)
	{
		Vector3 velocity;
		velocity.SetX(speed * Cos(-_rotation - GM_PI / 2 + GM_PI / 8 - i * GM_PI / 16));
		velocity.SetY(speed * Sin(-_rotation - GM_PI / 2 + GM_PI / 8 - i * GM_PI / 16));
		Bullet::ActiveBullet(_transform.LocalPosition, velocity, BulletType::EnemyBulletGreen);
	}

	_actionState = EnemyLightGreenActionState::Stop;
	_localTimer  = 0.0f;
}