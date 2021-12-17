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
	{0.0f / 3.0f, 0.0f / 10.0f}, // É`ÉÉÅ[ÉW

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
	// Laser textureÇÃì«Ç›çûÇ›
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/EnemyBoss.png", _texture);
	_sprite.CreateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(1.4f, 0.7f), Float2(135.0f / 3354.0f, 965.0f / 3354.0f), Float2(0.0f, 540.0f / 1598.0f), _rotation);
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/bossLaserEffect.png", _laserTexture);
	_laserSprite.CreateSprite(_transform.LocalPosition.ToFloat3(), Float2(1.3f, 2.0f),Float4(1,1,1, 0.7f),
		Float2(g_LaserAnimation[_animationIndex].x, g_LaserAnimation[_animationIndex].x + 1.0f / 3.0f),
		Float2(g_LaserAnimation[_animationIndex].y, g_LaserAnimation[_animationIndex].y + 1.0f / 10.0f));

	_laserSound = std::make_unique<AudioSource>();
	_chargeSound = std::make_unique<AudioSource>();
	_laserSound.get()->LoadSound(L"Resources/Audio/ShootingStar/laserFire.wav", SoundType::SE, 1.0f);
	_chargeSound.get()->LoadSound(L"Resources/Audio/ShootingStar/charge.wav", SoundType::SE, 1.0f);

	PushBackEnemy(this);
	g_Random.SetRange(0, 4);
}

void EnemyBoss::Update(GameTimer& gameTimer, const Player& player)
{
	if (!IsActive()) 
	{
		if (_enableShotLaser) { _enableShotLaser = false; }
		if (_isCharge)        { _isCharge = false; }
		return; 
	}

	_localTimer += gameTimer.DeltaTime();
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

	_colliderBox.centerPosition = Float3(_transform.LocalPosition.GetX(), 0.5f, 0.0f);
	_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(135.0f / 3354.0f, 965.0f / 3354.0f), Float2(0.0f, 540.0f / 1598.0f), 1.0f, _rotation);
	if (_isCharge)
	{
		_laserSprite.CreateSprite(Float3(0.05f, 0.33f, 0.0f), Float2(0.8f, 0.8f), Float4(1, 1, 1, 0.7f),
			Float2(g_LaserAnimation[_animationIndex].x, g_LaserAnimation[_animationIndex].x + 1.0f / 3.0f),
			Float2(g_LaserAnimation[_animationIndex].y, g_LaserAnimation[_animationIndex].y + 1.0f / 10.0f));

	}
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

void EnemyBoss::Appear(GameTimer& gameTimer)
{
	_speed = Vector3(0.0f, DEFAULT_SPEED_Y, 0.0f);
	_transform.LocalPosition -= _speed * gameTimer.DeltaTime();

	if (_transform.LocalPosition.GetY() <= 0.75f)
	{
		_transform.LocalPosition.SetY(0.75f);
		_actionState = EnemyBossActionState::Stop;
		_localTimer = 0.0f;
	}
}
void EnemyBoss::Stop(GameTimer& gameTimer)
{
	_speed = Vector3(0, 0, 0);

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
void EnemyBoss::LaserPrepare(GameTimer& gameTimer)
{
	if (_localTimer >= g_LaserPatternTable[_animationIndex])
	{
		_animationIndex++;
		_localTimer = 0;
	}

	// âπê∫çƒê∂
	if (_animationIndex >= 1)
	{
		_actionState     = EnemyBossActionState::Laser;
		_localTimer      = 0;
		_isCharge        = false;
		_enableShotLaser = true;
		_laserLocalTimer = 0;
		_chargeSound.get()->Stop();
		_laserSound.get()->Play();
	}
}
void EnemyBoss::Laser(GameTimer& gameTimer, const Player& player)
{
	if (_localTimer >= g_LaserPatternTable[_animationIndex])
	{
		_animationIndex++;
		_localTimer = 0;
	}

	if (_laserStartTime < _laserLocalTimer && _laserLocalTimer < _laserEndTime)
	{
		float t = (_laserLocalTimer - _laserStartTime) / (_laserEndTime - _laserStartTime);
		_laserColBox.centerPosition = Lerp(Vector3(-0.7f, player.GetTransform().LocalPosition.GetY(), 0.0f), Vector3(0.7f, player.GetTransform().LocalPosition.GetY(), 0.0f), t).ToFloat3();
	}

	if (_localTimer >= _laserEndTime)
	{
		_enableShotLaser = false;
	}

	if (_animationIndex >= _countof(g_LaserPatternTable))
	{
		_actionState = EnemyBossActionState::Stop;
		_localTimer = 0;
		_enableShotLaser = false;
		_animationIndex = 0;
		_laserColBox.centerPosition = Float3(0, 0, 0);
		_laserLocalTimer = 0;
	}
	_laserLocalTimer += gameTimer.DeltaTime();
}
void EnemyBoss::LaserAfter(GameTimer& gameTimer)
{

}
void EnemyBoss::Shot(GameTimer& gameTimer)
{
	float speed = 1.0f;

	for (int i = 0; i < 5; ++i)
	{
		Vector3 velocity;
		velocity.SetX(speed * Cos(_rotation - 2 * GM_PI / 3 + i * GM_PI / 16));
		velocity.SetY(speed * Sin(_rotation - 2 * GM_PI / 3 + i * GM_PI / 16));
		Bullet::ActiveBullet(Vector3(0.25f, 0.5f, 0.0f), velocity, BulletType::EnemyBulletBlue);
		Bullet::ActiveBullet(Vector3(-0.2f, 0.5f, 0.0f), velocity, BulletType::EnemyBulletBlue);
	}

	_actionState = EnemyBossActionState::Stop;
	_localTimer  = 0.0f;
}
void EnemyBoss::ShotToPlayer(GameTimer& gameTimer, const Player& player)
{
	float speed     = 1.4f;
	Vector3 temp    = Normalize(player.GetTransform().LocalPosition - _transform.LocalPosition);
	float rotation  = -ATan2(temp.GetY(), temp.GetX()) + GM_PI / 2;
	for (int i = 0; i < 5; ++i)
	{
		Vector3 velocity;
		velocity.SetX(speed * Cos(-rotation - GM_PI / 2 + GM_PI / 8 - i * GM_PI / 16));
		velocity.SetY(speed * Sin(-rotation - GM_PI / 2 + GM_PI / 8 - i * GM_PI / 16));
		Bullet::ActiveBullet(Vector3(0.25f, 0.5f, 0.0f), velocity, BulletType::EnemyBulletGreen);
		Bullet::ActiveBullet(Vector3(-0.2f, 0.5f, 0.0f), velocity, BulletType::EnemyBulletGreen);
	}

	_actionState = EnemyBossActionState::Stop;
	_localTimer  = 0.0f;
}
void EnemyBoss::GenerateEnemy(GameTimer& gameTimer)
{
	g_Random.SetRange(0, (int)EnemyType::Red);
	EnemyType type = static_cast<EnemyType>(g_Random.GetRandomValue());
	EnemyManager::Instance().GenerateEnemy(type, Vector3(-0.5f, 1.5f, 0.0f));
	type = static_cast<EnemyType>(g_Random.GetRandomValue());
	EnemyManager::Instance().GenerateEnemy(type, Vector3(0.5f, 1.5f, 0.0f));

	_actionState = EnemyBossActionState::Stop;
	_localTimer  = 0;
}

void EnemyBoss::ProceedPrepareLaser()
{
	if (_localTimer >= _waitTime)
	{
		_actionState = EnemyBossActionState::LaserPrepare;
		_localTimer  = 0.0f;
		_isCharge    = true;
		// âπê∫ÇÃçƒê∂
	}
}
void EnemyBoss::ProceedGenerateEnemy()
{
	if (_localTimer >= _waitTime)
	{
		_actionState = EnemyBossActionState::GenerateEnemy;
		_localTimer  = 0.0f;
	}
}
void EnemyBoss::ProceedShotToPlayer()
{
	if (_localTimer >= _waitTime)
	{
		_actionState = EnemyBossActionState::ShotToPlayer;
		_localTimer  = 0;
	}
}
void EnemyBoss::ProceedShot()
{
	if (_localTimer >= _waitTime)
	{
		_actionState = EnemyBossActionState::Shot;
		_localTimer  = 0;
	}
}
