//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyBlue.cpp
///             @brief  EnemyBlue
///             @author Toide Yutaro
///             @date   ------------
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/EnemyBlue.hpp"
#include "MainGame/ShootingStar/Include/Bullet/Bullet.hpp"
#include "GameCore/Include/Screen.hpp"
#include "GameMath/Include/GMDistribution.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define DEFAULT_SPEED_Y (2.0f)

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
static RandomInt g_Random;

void EnemyBlue::Initialize()
{
	_actorType = ActorType::Sprite;
	_name      = "EnemyBlue";
	_isActive  = false;
	_transform.LocalPosition = Vector3(0.0f, 1.0f + _enemySize / 2.0f, 0.0f);
	_speed       = Vector3(0.0f, 0.5f, 0.0f);
	_accel       = Vector3(0.0f, 0.4f, 0.0f);
	_colliderBox = Collider2D::CreateRectangleCollider(Float2(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY()), _enemySize / Screen::GetAspectRatio(), _enemySize);
	_actionState = EnemyBlueActionState::None;
	_enemyType   = EnemyType::Default;
	_rotation    = GM_PI;
	_hp = _defaultHP;

	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/enemy.png", _texture);
	_sprite.CreateSprite(_transform.LocalPosition.ToFloat3(), Float2(_enemySize / Screen::GetAspectRatio(), _enemySize),Float4(0.4f,0.4f,1, 0), Float2(0, 1), Float2(0, 1), _rotation);

	g_Random.SetRange(0, 1);

	PushBackEnemy(this);
}

void EnemyBlue::Update(GameTimer& gameTimer, const Player& player)
{
	if (!IsActive()) { return; }

	_localTimer += gameTimer.DeltaTime();
	switch (_actionState)
	{
		case EnemyBlueActionState::Appear:
		{
			Appear(gameTimer);
			break;
		}
		case EnemyBlueActionState::Stop:
		{
			Stop(gameTimer);
			break;
		}
		case EnemyBlueActionState::Shot:
		{
			Shot(gameTimer);
			break;
		}

		default: break;
	}

	_colliderBox.centerPosition = _transform.LocalPosition.ToFloat3();
	_sprite.UpdateSprite(_transform.LocalPosition.ToFloat3(), Float4(0.4f, 0.4f, 1, 1.0f), Float2(0, 1), Float2(0, 1), _rotation);
}
void EnemyBlue::Reset()
{

}
bool EnemyBlue::Generate(const Vector3& position)
{
	if (!IsActive())
	{
		_actionState = EnemyBlueActionState::Appear;
		_transform.LocalPosition = position;
		_colliderBox.centerPosition = Float3(position.GetX(), position.GetY(), position.GetZ());
		_speed = Vector3(0.0f, 0.5f, 0.0f);
		_accel = Vector3(0.0f, 0.4f, 0.0f);
		_rotation = GM_PI;
		_sprite.UpdateSprite(_transform.LocalPosition.ToFloat3(), Float4(0.4f, 0.4f, 1, 1.0f), Float2(0, 1), Float2(0, 1), _rotation);
		_hp = _defaultHP;
		SetActive(true);
		return true;
	}
	else
	{
		return false;
	}
}
void EnemyBlue::Appear(GameTimer& gameTimer)
{
	_transform.LocalPosition -= _speed * gameTimer.DeltaTime();

	if (_transform.LocalPosition.GetY() <= 0.7f)
	{
		_transform.LocalPosition.SetY(0.7f);
		_actionState = EnemyBlueActionState::Stop;
		_localTimer = 0.0f;
	}
}
/****************************************************************************
*                       Stop
*************************************************************************//**
*  @fn        void EnemyDefault::Stop(GameTimer& gameTimer)
*  @brief     Stop enemy blue
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyBlue::Stop(GameTimer& gameTimer)
{
	_speed.SetY(0);
	if (_localTimer >= 1.5f)
	{
		_actionState = EnemyBlueActionState::Shot;
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
void EnemyBlue::Shot(GameTimer& gameTimer)
{
	float speed = 0.7f;
	for (int i = 0; i < 5; ++i)
	{
		Vector3 velocity;
		velocity.SetX(speed * Cos(_rotation - 2 * GM_PI / 3 + i * GM_PI / 16));
		velocity.SetY(speed * Sin(_rotation - 2 * GM_PI / 3 + i * GM_PI / 16));
		Bullet::ActiveBullet(_transform.LocalPosition, velocity, BulletType::EnemyBulletBlue);
	}
	
	_actionState = EnemyBlueActionState::Stop;
}