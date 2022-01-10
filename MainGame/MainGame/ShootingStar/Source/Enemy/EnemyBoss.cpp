//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyBoss.cpp
///             @brief  EnemyBoss
///             @author Toide Yutaro
///             @date   --------------
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/EnemyBoss.hpp"
#include "MainGame/ShootingStar/Include/Bullet/Bullet.hpp"
#include "MainGame/ShootingStar/Include/Player/Player.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyManager.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include "GameCore/Include/Screen.hpp"
#include "GameMath/Include/GMDistribution.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define DEFAULT_SPEED_Y (1.0f)

static Float2 g_LaserAnimation[] =
{
	{0.0f / 3.0f, 0.0f / 10.0f}, // チャージ

	{0.0f / 3.0f, 0.0f / 10.0f},
	{1.0f / 3.0f, 0.0f / 10.0f},
	{2.0f / 3.0f, 0.0f / 10.0f},

	{0.0f / 3.0f, 1.0f / 10.0f},
	{1.0f / 3.0f, 1.0f / 10.0f},
	{2.0f / 3.0f, 1.0f / 10.0f},

	{0.0f / 3.0f, 2.0f / 10.0f},
	{1.0f / 3.0f, 2.0f / 10.0f},
	{2.0f / 3.0f, 2.0f / 10.0f},

	{0.0f / 3.0f, 3.0f / 10.0f},
	{1.0f / 3.0f, 3.0f / 10.0f},
	{2.0f / 3.0f, 3.0f / 10.0f},

	{0.0f / 3.0f, 4.0f / 10.0f},
	{1.0f / 3.0f, 4.0f / 10.0f},
	{2.0f / 3.0f, 4.0f / 10.0f},

	{0.0f / 3.0f, 5.0f / 10.0f},
	{1.0f / 3.0f, 5.0f / 10.0f},
	{2.0f / 3.0f, 5.0f / 10.0f},

	{0.0f / 3.0f, 6.0f / 10.0f},
	{1.0f / 3.0f, 6.0f / 10.0f},
	{2.0f / 3.0f, 6.0f / 10.0f},

	{0.0f / 3.0f, 7.0f / 10.0f},
	{1.0f / 3.0f, 7.0f / 10.0f},
	{2.0f / 3.0f, 7.0f / 10.0f},

	{0.0f / 3.0f, 8.0f / 10.0f},
	{1.0f / 3.0f, 8.0f / 10.0f},
	{2.0f / 3.0f, 8.0f / 10.0f},

	{0.0f / 3.0f, 9.0f / 10.0f},
	{1.0f / 3.0f, 9.0f / 10.0f},
	{2.0f / 3.0f, 9.0f / 10.0f},
};
static float g_LaserPatternTable[] =
{
	3.5f,
	0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,
	0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,
	0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,0.033f,
};
static RandomInt g_Random;
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                      Initialize
*************************************************************************//**
*  @fn        void EnemyBoss::Initialize()
*  @brief     Initialize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyBoss::Initialize()
{
	_actorType               = ActorType::Sprite;
	_name                    = "EnemyBoss";
	_isActive                = false;
	_transform.LocalPosition = Vector3(0.0f, 1.0f + _enemySize / 2.0f, 0.0f);
	_speed                   = Vector3(0.0f, DEFAULT_SPEED_Y, 0.0f);
	_colliderBox             = Collider2D::CreateRectangleCollider(Float2(_transform.LocalPosition.GetX(), 0.45f), _enemySize / Screen::GetAspectRatio(), _enemySize);
	_laserColBox             = Collider2D::CreateRectangleCollider(Float2(-0.7f, -0.85f), 0.2f / Screen::GetAspectRatio(), 0.5f);
	_actionState             = EnemyBossActionState::None;
	_enemyType               = EnemyType::Boss;
	_rotation = GM_PI;
	_isCharge        = false;
	_enableShotLaser = false;
	_hp = _defaultHP;

	/*-------------------------------------------------------------------
	-          Load Texture
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/EnemyBoss.png", _texture);
	_sprite.CreateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(1.4f, 0.7f), Float2(135.0f / 3354.0f, 965.0f / 3354.0f), Float2(0.0f, 540.0f / 1598.0f), _rotation);
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/bossLaserEffect.png", _laserTexture);
	_laserSprite.CreateSprite(_transform.LocalPosition.ToFloat3(), Float2(1.3f, 2.0f),Float4(1,1,1, 0.7f),
		Float2(g_LaserAnimation[_animationIndex].x, g_LaserAnimation[_animationIndex].x + 1.0f / 3.0f),
		Float2(g_LaserAnimation[_animationIndex].y, g_LaserAnimation[_animationIndex].y + 1.0f / 10.0f));

	/*-------------------------------------------------------------------
	-          Load Sound
	---------------------------------------------------------------------*/
	_laserSound = std::make_unique<AudioSource>();
	_chargeSound = std::make_unique<AudioSource>();
	_laserSound.get()->LoadSound(L"Resources/Audio/ShootingStar/laserFire.wav", SoundType::SE, 1.0f);
	_chargeSound.get()->LoadSound(L"Resources/Audio/ShootingStar/charge.wav", SoundType::SE, 1.0f);

	PushBackEnemy(this);
	g_Random.SetRange(0, 4);
}
/****************************************************************************
*                      Finalize
*************************************************************************//**
*  @fn        void EnemyBoss::Finalize()
*  @brief     Finalize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyBoss::Finalize()
{
	_texture     .Resource = nullptr;
	_laserTexture.Resource = nullptr;

	_laserSound.reset();
	_chargeSound.reset();
}
/****************************************************************************
*                      Update 
*************************************************************************//**
*  @fn        void EnemyBoss::Update(GameTimer& gameTimer, const Player& player)
*  @brief     Update enemy action
*  @param[in] GameTimer& gameTimer
*  @param[in] Player& player
*  @return 　　void
*****************************************************************************/
void EnemyBoss::Update(GameTimer& gameTimer, const Player& player)
{
	/*-------------------------------------------------------------------
	-          Active Check
	---------------------------------------------------------------------*/
	if (!IsActive()) 
	{
		if (_enableShotLaser) { _enableShotLaser = false; }
		if (_isCharge)        { _isCharge = false; }
		return; 
	}
	/*-------------------------------------------------------------------
	-          Update Local Timer
	---------------------------------------------------------------------*/
	_localTimer += gameTimer.DeltaTime();
	/*-------------------------------------------------------------------
	-          Decide Action State
	---------------------------------------------------------------------*/
	switch (_actionState)
	{
		case EnemyBossActionState::Appear:
		{
			Appear(gameTimer);
			break;
		}
		case EnemyBossActionState::Stop:
		{
			Stop(gameTimer);
			break;
		}
		case EnemyBossActionState::GenerateEnemy:
		{
			GenerateEnemy(gameTimer);
			break;
		}
		case EnemyBossActionState::LaserPrepare:
		{
			_chargeSound.get()->Play();
			LaserPrepare(gameTimer);
			break;
		}
		case EnemyBossActionState::Laser:
		{
			Laser(gameTimer, player);
			break;
		}
		case EnemyBossActionState::Shot:
		{
			Shot(gameTimer);
			break;
		}
		case EnemyBossActionState::ShotToPlayer:
		{
			ShotToPlayer(gameTimer, player);
			break;
		}
		default: break;
	}
	/*-------------------------------------------------------------------
	-          Update Collider Box
	---------------------------------------------------------------------*/
	_colliderBox.centerPosition = Float3(_transform.LocalPosition.GetX(), 0.5f, 0.0f);
	/*-------------------------------------------------------------------
	-          Update Sprite
	---------------------------------------------------------------------*/
	_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(135.0f / 3354.0f, 965.0f / 3354.0f), Float2(0.0f, 540.0f / 1598.0f), 1.0f, _rotation);
	/*-------------------------------------------------------------------
	-          Update charge action (in case Charge action state)
	---------------------------------------------------------------------*/
	if (_isCharge)
	{
		_laserSprite.CreateSprite(Float3(0.042f, 0.33f, 0.0f), Float2(0.8f, 0.8f), Float4(1, 1, 1, 0.7f),
			Float2(g_LaserAnimation[_animationIndex].x, g_LaserAnimation[_animationIndex].x + 1.0f / 3.0f),
			Float2(g_LaserAnimation[_animationIndex].y, g_LaserAnimation[_animationIndex].y + 1.0f / 10.0f));
	}
	/*-------------------------------------------------------------------
	-          Update Laser Action (in case Laser action state)
	---------------------------------------------------------------------*/
	if (_enableShotLaser)
	{
		_laserSprite.CreateSprite(Float3(0.1f, 0.0f, 0.0f), Float2(1.5f, 2.0f), Float4(1, 1, 1, 0.7f),
			Float2(g_LaserAnimation[_animationIndex].x, g_LaserAnimation[_animationIndex].x + 1.0f / 3.0f),
			Float2(g_LaserAnimation[_animationIndex].y, g_LaserAnimation[_animationIndex].y + 1.0f / 10.0f));
	}
}

void EnemyBoss::Reset()
{

}
/****************************************************************************
*                      Generate
*************************************************************************//**
*  @fn        void EnemyBoss::Generate(const Vector3& position)
*  @brief     Generate enemy
*  @param[in] Vector3& position
*  @return 　　void
*****************************************************************************/
bool EnemyBoss::Generate(const Vector3& position)
{
	if (!IsActive())
	{
		_actionState                = EnemyBossActionState::Appear;
		_transform.LocalPosition    = position;
		_colliderBox.centerPosition = Float3(position.GetX(), position.GetY(), position.GetZ());
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
*                      Appear
*************************************************************************//**
*  @fn        void EnemyBoss::Appear(GameTimer& gameTimer)
*  @brief     Appear action state
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyBoss::Appear(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-          Update enemy position
	---------------------------------------------------------------------*/
	_speed = Vector3(0.0f, DEFAULT_SPEED_Y, 0.0f);
	_transform.LocalPosition -= _speed * gameTimer.DeltaTime();
	/*-------------------------------------------------------------------
	-          Proceed to the Stop action state
	---------------------------------------------------------------------*/
	if (_transform.LocalPosition.GetY() <= 0.75f)
	{
		_transform.LocalPosition.SetY(0.75f);
		_actionState = EnemyBossActionState::Stop;
		_localTimer = 0.0f;
	}
}
/****************************************************************************
*                      Stop
*************************************************************************//**
*  @fn        void EnemyBoss::Stop(GameTimer& gameTimer)
*  @brief     Stop action state
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyBoss::Stop(GameTimer& gameTimer)
{
	_speed = Vector3(0, 0, 0);
	/*-------------------------------------------------------------------
	-          Decide action state
	---------------------------------------------------------------------*/
	if (_localTimer >= _waitTime)
	{
		g_Random.SetRange(0, 4);
		int state = g_Random.GetRandomValue();
		if      (state == 0) { ProceedShot(); }
		else if (state == 1) { ProceedGenerateEnemy(); }
		else if (state == 2) { ProceedPrepareLaser(); }
		else if (state == 3) { ProceedShotToPlayer(); }
		_localTimer = 0.0f;
	}
}
/****************************************************************************
*                      LaserPrepare
*************************************************************************//**
*  @fn        void EnemyBoss::LaserPrepare(GameTimer& gameTimer)
*  @brief     LaserPrepare action state
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyBoss::LaserPrepare(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-          Update Laser Animation
	---------------------------------------------------------------------*/
	if (_localTimer >= g_LaserPatternTable[_animationIndex])
	{
		_animationIndex++;
		_localTimer = 0;
	}

	/*-------------------------------------------------------------------
	-          Proceed to the Laser action state
	---------------------------------------------------------------------*/
	if (_animationIndex >= 1)
	{
		_actionState     = EnemyBossActionState::Laser;
		_localTimer      = 0;
		_isCharge        = false;
		_enableShotLaser = true;
		_laserLocalTimer = 0;
		/*-------------------------------------------------------------------
		-          Play laser sound
		---------------------------------------------------------------------*/
		_chargeSound.get()->Stop();
		_laserSound.get()->Play();
	}
}
/****************************************************************************
*                      Laser
*************************************************************************//**
*  @fn        void EnemyBoss::Laser(GameTimer& gameTimer, const Player& player)
*  @brief     Laser action state
*  @param[in] GameTimer& gameTimer
*  @param[in] Player& player
*  @return 　　void
*****************************************************************************/
void EnemyBoss::Laser(GameTimer& gameTimer, const Player& player)
{
	/*-------------------------------------------------------------------
	-          Update Laser Animation
	---------------------------------------------------------------------*/
	if (_localTimer >= g_LaserPatternTable[_animationIndex])
	{
		_animationIndex++;
		_localTimer = 0;
	}

	/*-------------------------------------------------------------------
	-          Update Laser Collider
	---------------------------------------------------------------------*/
	if (_laserStartTime < _laserLocalTimer && _laserLocalTimer < _laserEndTime)
	{
		float t = (_laserLocalTimer - _laserStartTime) / (_laserEndTime - _laserStartTime);
		_laserColBox.centerPosition = Lerp(Vector3(-0.7f, player.GetTransform().LocalPosition.GetY(), 0.0f), Vector3(0.7f, player.GetTransform().LocalPosition.GetY(), 0.0f), t).ToFloat3();
	}
	/*-------------------------------------------------------------------
	-          Stop Laser Action in case exceed LaserEndTime
	---------------------------------------------------------------------*/
	if (_localTimer >= _laserEndTime)
	{
		_enableShotLaser = false;
	}
	/*-------------------------------------------------------------------
	-          Proceed to the Stop action state
	---------------------------------------------------------------------*/
	if (_animationIndex >= _countof(g_LaserPatternTable))
	{
		_actionState = EnemyBossActionState::Stop;
		_localTimer = 0;
		_enableShotLaser = false;
		_animationIndex = 0;
		_laserColBox.centerPosition = Float3(0, 0, 0);
		_laserLocalTimer = 0;
	}
	/*-------------------------------------------------------------------
	-          Update laser local timer
	---------------------------------------------------------------------*/
	_laserLocalTimer += gameTimer.DeltaTime();
}
/****************************************************************************
*                      LaserAfter
*************************************************************************//**
*  @fn        void EnemyBoss::LaserAfter(GameTimer& gameTimer)
*  @brief     LaserAfter action state
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyBoss::LaserAfter(GameTimer& gameTimer)
{

}
/****************************************************************************
*                      Shot
*************************************************************************//**
*  @fn        void EnemyBoss::Shot(GameTimer& gameTimer)
*  @brief     Shot action state
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyBoss::Shot(GameTimer& gameTimer)
{
	float speed = 1.0f;
	/*-------------------------------------------------------------------
	-          Shot action 5way * 2
	---------------------------------------------------------------------*/
	for (int i = 0; i < 5; ++i)
	{
		Vector3 velocity;
		velocity.SetX(speed * Cos(_rotation - 2 * GM_PI / 3 + i * GM_PI / 16));
		velocity.SetY(speed * Sin(_rotation - 2 * GM_PI / 3 + i * GM_PI / 16));
		Bullet::ActiveBullet(Vector3(0.25f, 0.5f, 0.0f), velocity, BulletType::EnemyBulletBlue);
		Bullet::ActiveBullet(Vector3(-0.2f, 0.5f, 0.0f), velocity, BulletType::EnemyBulletBlue);
	}
	/*-------------------------------------------------------------------
	-          Proceed to the stop action state
	---------------------------------------------------------------------*/
	_actionState = EnemyBossActionState::Stop;
	_localTimer  = 0.0f;
}
/****************************************************************************
*                      ShotToPlayer
*************************************************************************//**
*  @fn        void EnemyBoss::ShotToPlayer(GameTimer& gameTimer, const Player& player)
*  @brief     ShotToPlayer action state
*  @param[in] GameTimer& gameTimer
*  @param[in] Player& player
*  @return 　　void
*****************************************************************************/
void EnemyBoss::ShotToPlayer(GameTimer& gameTimer, const Player& player)
{
	/*-------------------------------------------------------------------
	-          Look to the player 
	---------------------------------------------------------------------*/
	float speed     = 1.4f;
	Vector3 temp    = Normalize(player.GetTransform().LocalPosition - _transform.LocalPosition);
	float rotation  = -ATan2(temp.GetY(), temp.GetX()) + GM_PI / 2;
	/*-------------------------------------------------------------------
	-          Shot action 5way * 2
	---------------------------------------------------------------------*/
	for (int i = 0; i < 5; ++i)
	{
		Vector3 velocity;
		velocity.SetX(speed * Cos(-rotation - GM_PI / 2 + GM_PI / 8 - i * GM_PI / 16));
		velocity.SetY(speed * Sin(-rotation - GM_PI / 2 + GM_PI / 8 - i * GM_PI / 16));
		Bullet::ActiveBullet(Vector3(0.25f, 0.5f, 0.0f), velocity, BulletType::EnemyBulletGreen);
		Bullet::ActiveBullet(Vector3(-0.2f, 0.5f, 0.0f), velocity, BulletType::EnemyBulletGreen);
	}
	/*-------------------------------------------------------------------
	-          Proceed to the stop action state
	---------------------------------------------------------------------*/
	_actionState = EnemyBossActionState::Stop;
	/*-------------------------------------------------------------------
	-          Update local timer
	---------------------------------------------------------------------*/
	_localTimer  = 0.0f;
}
/****************************************************************************
*                      GenerateEnemy
*************************************************************************//**
*  @fn        void EnemyBoss::GenerateEnemy(GameTimer& gameTimer)
*  @brief     Generate enemy action state
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyBoss::GenerateEnemy(GameTimer& gameTimer)
{

	g_Random.SetRange(0, (int)EnemyType::Red);
	/*-------------------------------------------------------------------
	-          Generate 2 enemies
	---------------------------------------------------------------------*/
	EnemyType type = static_cast<EnemyType>(g_Random.GetRandomValue());
	EnemyManager::Instance().GenerateEnemy(type, Vector3(-0.5f, 1.5f, 0.0f));
	type = static_cast<EnemyType>(g_Random.GetRandomValue());
	EnemyManager::Instance().GenerateEnemy(type, Vector3(0.5f, 1.5f, 0.0f));
	/*-------------------------------------------------------------------
	-          Proceed to the Stop action state
	---------------------------------------------------------------------*/
	_actionState = EnemyBossActionState::Stop;
	_localTimer  = 0;
}
/****************************************************************************
*                      ProceedPrepareLaser
*************************************************************************//**
*  @fn        void EnemyBoss::ProceedPrepareLaser()
*  @brief     Proceed to the PrepareLaser action state
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyBoss::ProceedPrepareLaser()
{
	/*-------------------------------------------------------------------
	-          Proceed to the LaserPrepare action state
	---------------------------------------------------------------------*/
	if (_localTimer >= _waitTime)
	{
		_actionState = EnemyBossActionState::LaserPrepare;
		_localTimer  = 0.0f;
		_isCharge    = true;
		_chargeSound.get()->Play();
	}
}
/****************************************************************************
*                      ProceedGenerateEnemy
*************************************************************************//**
*  @fn        void EnemyBoss::ProceedGenerateEnemy()
*  @brief     Proceed to the GenerateEnemy action state
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyBoss::ProceedGenerateEnemy()
{
	/*-------------------------------------------------------------------
	-          Proceed to the GenerateEnemy action state
	---------------------------------------------------------------------*/
	if (_localTimer >= _waitTime)
	{
		_actionState = EnemyBossActionState::GenerateEnemy;
		_localTimer  = 0.0f;
	}
}
/****************************************************************************
*                      ProceedShotToPlayer
*************************************************************************//**
*  @fn        void EnemyBoss::ProceedShotToPlayer()
*  @brief     Proceed to the shot to player action state
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyBoss::ProceedShotToPlayer()
{
	/*-------------------------------------------------------------------
	-          Proceed to the ShotToPlayer action state
	---------------------------------------------------------------------*/
	if (_localTimer >= _waitTime)
	{
		_actionState = EnemyBossActionState::ShotToPlayer;
		_localTimer  = 0;
	}
}
/****************************************************************************
*                      ProceedShot
*************************************************************************//**
*  @fn        void EnemyBoss::ProceedShot()
*  @brief     Proceed to the shot action
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyBoss::ProceedShot()
{
	/*-------------------------------------------------------------------
	-          Proceed to the Shot action state
	---------------------------------------------------------------------*/
	if (_localTimer >= _waitTime)
	{
		_actionState = EnemyBossActionState::Shot;
		_localTimer  = 0;
	}
}
