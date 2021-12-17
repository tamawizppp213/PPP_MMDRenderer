//////////////////////////////////////////////////////////////////////////////////
///             @file   Enemy.cpp
///             @brief  Enemy
///             @author Toide Yutaro
///             @date   ----------------
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/EnemyDefault.hpp"
#include "MainGame/ShootingStar/Include/Bullet/Bullet.hpp"
#include "GameCore/Include/Screen.hpp"
#include "GameMath/Include/GMDistribution.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define DEFAULT_SPEED_Y (3.0f)
std::vector<Enemy*> Enemy::_enemies;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
RandomInt g_Random;

/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        void Enemy::Initialize()
*  @brief     Initialize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyDefault::Initialize()
{
	_actorType               = ActorType::Sprite;
	_name                    = "DefaultEnemy";
	_isActive                = false;
	_transform.LocalPosition = Vector3(0.0f, 1.0f + _enemySize / 2.0f, 0.0f);
	_speed                   = Vector3(0.0f, 0.5f, 0.0f);
	_accel                   = Vector3(0.0f, 0.04f, 0.0f);
	_colliderBox             = Collider2D::CreateRectangleCollider(Float2(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY()), _enemySize / Screen::GetAspectRatio(), _enemySize);
	_actionState             = EnemyDefaultActionState::None;
	_enemyType               = EnemyType::Default;
	_rotation = GM_PI ;

	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/enemy.png", _texture);
	_sprite.CreateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(_enemySize / Screen::GetAspectRatio(), _enemySize), Float2(0, 1), Float2(0, 1), _rotation);

	g_Random.SetRange(0, 1);

	PushBackEnemy(this);
}

/****************************************************************************
*                       Update
*************************************************************************//**
*  @fn        void Enemy::Update()
*  @brief     If IsActive is enable, Execute action
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyDefault::Update(GameTimer& gameTimer, const Player& player)
{
	if (!IsActive()) { return; }

	_localTimer += gameTimer.DeltaTime();
	switch (_actionState)
	{
		case EnemyDefaultActionState::Appear:
		{
			Appear(gameTimer);
			break;
		}
		case EnemyDefaultActionState::Stop:
		{
			Stop(gameTimer);
			if (_localTimer >= _waitTime)
			{
				Shot(gameTimer);
			}
			IsProceedAccelerate(gameTimer);
			break;
		}
		case EnemyDefaultActionState::Shot:
		{
			Shot(gameTimer);
			break;
		}
		case EnemyDefaultActionState::Accelerate:
		{
			Accelerator(gameTimer);
			break;
		}
		default: break;
	}

	_colliderBox.centerPosition = _transform.LocalPosition.ToFloat3();
	_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0, 1), Float2(0, 1), 1.0f, _rotation);
}
/****************************************************************************
*                       Reset
*************************************************************************//**
*  @fn        void EnemyDefault::Reset()
*  @brief     Reset 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyDefault::Reset()
{
	_isActive = false;
	_transform.LocalPosition = Vector3(0.0f, 1.0f + _enemySize / 2.0f, 0.0f);
	_speed                   = Vector3(0.0f, 0.5f, 0.0f);
	_accel                   = Vector3(0.0f, 0.4f, 0.0f);
	_colliderBox             = Collider2D::CreateRectangleCollider(Float2(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY()), _enemySize / Screen::GetAspectRatio(), _enemySize);
	_actionState             = EnemyDefaultActionState::None;
	_enemyType               = EnemyType::Default;
	_hp = _defaultHP;
}
/****************************************************************************
*                       Generate
*************************************************************************//**
*  @fn        void EnemyDefault::Generate(const Vector3& position)
*  @brief     Generate enemy
*  @param[in] const Vector3& position
*  @return 　　bool
*****************************************************************************/
bool EnemyDefault::Generate(const Vector3& position)
{
	if (!IsActive())
	{
		_actionState                = EnemyDefaultActionState::Appear;
		_transform.LocalPosition    = position;
		_colliderBox.centerPosition = Float3(position.GetX(), position.GetY(), position.GetZ());
		_speed                      = Vector3(0.0f, 0.5f, 0.0f);
		_accel                      = Vector3(0.0f, 0.04f, 0.0f);
		_rotation = GM_PI;
		_hp = _defaultHP;
		SetActive(true);
		_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0, 1), Float2(0, 1), 1.0f, _rotation);
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
*  @fn        void EnemyDefault::Appear(GameTimer& gameTimer)
*  @brief     Appear an enemy to the preDecided position/ 
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyDefault::Appear(GameTimer& gameTimer)
{
	_transform.LocalPosition -= _speed * gameTimer.DeltaTime();
	if (_transform.LocalPosition.GetY() <= 0.6f)
	{
		_transform.LocalPosition.SetY(0.6f);
		_actionState = EnemyDefaultActionState::Stop;
		_localTimer  = 0.0f;
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
void EnemyDefault::Stop(GameTimer& gameTimer)
{
	_speed.SetY(0.0f);
}
/****************************************************************************
*                       Accelerator 
*************************************************************************//**
*  @fn        void EnemyDefault::Accelerator(GameTimer& gameTimer)
*  @brief     Acclerate enemy speeds.If the enemy is out of the screen, it setActive will false. 
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyDefault::Accelerator(GameTimer& gameTimer)
{
	_speed                   += _accel * gameTimer.DeltaTime();
	_transform.LocalPosition -= _speed * gameTimer.DeltaTime();

	if (_transform.LocalPosition.GetY() < -1.0f - _enemySize / 2.0f)
	{
		_actionState = EnemyDefaultActionState::End;
		_isActive    = false;
		_speed       = Vector3(0.0f, DEFAULT_SPEED_Y, 0.0f);
	}

}
/****************************************************************************
*                       IsProceedAccelerate
*************************************************************************//**
*  @fn        void EnemyDefault::IsProceedAccelerate(GameTimer& gameTimer)
*  @brief     Before going to the accelerate state, you should check the wait time.
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyDefault::IsProceedAccelerate(GameTimer& gameTimer)
{
	if (_localTimer >= _waitTime)
	{
		_actionState = EnemyDefaultActionState::Accelerate;
		_localTimer = 0;
		
		if (g_Random.GetRandomValue())
		{
			_speed =  Normalize(Vector3(-0.3f, 0.8f, 0.0f));
		}
		else
		{
			_speed = Normalize(Vector3(0.3f, 0.8f, 0.0f));
		}
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
void EnemyDefault::Shot(GameTimer& gameTimer)
{
	Bullet::ActiveBullet(_transform.LocalPosition, Vector3(0.0f, 0.7f, 0.0f), BulletType::EnemyBulletBlue);
	_actionState = EnemyDefaultActionState::Accelerate;
}

