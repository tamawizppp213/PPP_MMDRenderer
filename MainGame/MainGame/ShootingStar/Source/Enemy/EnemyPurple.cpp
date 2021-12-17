//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyPurple.cpp
///             @brief  EnemyPurple
///             @author Toide Yutaro
///             @date   2021_12_03
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/EnemyPurple.hpp"
#include "MainGame/ShootingStar/Include/Player/Player.hpp"
#include "MainGame/ShootingStar/Include/Bullet/Bullet.hpp"
#include "GameCore/Include/Screen.hpp"
#include "GameMath/Include/GMDistribution.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define DEFAULT_SPEED_Y (1.0f)

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
static RandomInt g_Random;


void EnemyPurple::Initialize()
{
	_actorType               = ActorType::Sprite;
	_name                    = "EnemyPurple";
	_isActive                = false;
	_transform.LocalPosition = Vector3(0.0f, 1.0f + _enemySize / 2.0f, 0.0f);
	_speed                   = Vector3(0.0f, DEFAULT_SPEED_Y, 0.0f);
	_colliderBox             = Collider2D::CreateRectangleCollider(Float2(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY()), _enemySize / Screen::GetAspectRatio(), _enemySize);
	_actionState             = EnemyPurpleActionState::None;
	_enemyType               = EnemyType::Purple;
	_rotation = GM_PI;
	_hp = _defaultHP;
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/enemy.png", _texture);
	_sprite.CreateSprite(_transform.LocalPosition.ToFloat3(), Float2(_enemySize / Screen::GetAspectRatio(), _enemySize), Float4(1.0f, 0.7f, 1.0f, 1.0f), Float2(0, 1), Float2(0, 1), _rotation);
	g_Random.SetRange(0, 1);

	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/Laser.png", _laserTexture);
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/shoot.png", _chargeTexture);
	_laserSprite .CreateSprite(Float3(0,0,0), Float2(0.2f, 2.0f), Float4(1.0f, 1.0f, 1.0f, 1.0f), Float2(0, 1), Float2(0, 1), 0);
	_chargeSprite.CreateSprite(Float3(0, 0, 0), Float2(0.1f/Screen::GetAspectRatio(), 0.1f), Float4(1.0f, 1.0f, 1.0f, 1.0f), Float2(0, 1), Float2(0, 1), 0);
	_laserCollider = Collider2D::CreateRectangleCollider(Float2(0, 0), 0.2f, 2.0f);

	_laserSound = std::make_unique<AudioSource>();
	_chargeSound = std::make_unique<AudioSource>();
	_laserSound .get()->LoadSound(L"Resources/Audio/ShootingStar/laserFire.wav", SoundType::SE, 1.0f);
	_chargeSound.get()->LoadSound(L"Resources/Audio/ShootingStar/charge.wav"   , SoundType::SE, 1.0f);

	_lerpT = 0;
	PushBackEnemy(this);
}

void EnemyPurple::Update(GameTimer& gameTimer, const Player& player)
{
	if (!IsActive()) 
	{ 
		if (_enableFireLaser) { _enableFireLaser = false; }
		if (_isCharge) { _isCharge = false; }
		return; 
	}

	_localTimer += gameTimer.DeltaTime();
	switch (_actionState)
	{
		case EnemyPurpleActionState::Appear:
		{
			Appear(gameTimer);
			break;
		}
		case EnemyPurpleActionState::Stop:
		{
			Stop(gameTimer);
			break;
		}
		case EnemyPurpleActionState::Move:
		{
			Move(gameTimer, player);
			break;
		}
		case EnemyPurpleActionState::LaserPrepare:
		{
			LaserPrepare(gameTimer);
			break;
		}
		case EnemyPurpleActionState::Laser:
		{
			Laser(gameTimer);
			break;
		}
		case EnemyPurpleActionState::LaserAfter:
		{
			LaserAfter(gameTimer, player);
			break;
		}
		default: break;
	}

	_colliderBox.centerPosition = _transform.LocalPosition.ToFloat3();
	_sprite.UpdateSprite(_transform.LocalPosition.ToFloat3(), Float4(1.0f, 0.7f, 1.0f, 1.0f), Float2(0, 1), Float2(0, 1), _rotation);
	
}
bool EnemyPurple::Generate(const Vector3& position)
{
	if (!IsActive())
	{
		_actionState = EnemyPurpleActionState::Appear;
		_transform.LocalPosition = position;
		_colliderBox.centerPosition = Float3(position.GetX(), position.GetY(), position.GetZ());
		_speed = Vector3(0.0f, DEFAULT_SPEED_Y, 0.0f);
		_rotation = GM_PI;
		_hp = _defaultHP;
		_sprite.UpdateSprite(_transform.LocalPosition.ToFloat3(), Float4(1.0f, 0.7f, 1.0f, 1.0f), Float2(0, 1), Float2(0, 1), _rotation);
		_lerpT = 0;
		_enableFireLaser = false;
		_laserSprite.CreateSprite(Float3(0, 0, 0), Float2(0.2f, 2.0f), Float4(1.0f, 1.0f, 1.0f, 1.0f), Float2(0, 1), Float2(0, 1), 0);
		_chargeSprite.CreateSprite(Float3(0, 0, 0), Float2(0.1f / Screen::GetAspectRatio(), 0.1f), Float4(1.0f, 1.0f, 1.0f, 1.0f), Float2(0, 1), Float2(0, 1), 0);
		SetActive(true);
		return true;
	}
	else
	{
		return false;
	}
}
void EnemyPurple::Appear(GameTimer& gameTimer)
{
	_transform.LocalPosition -= _speed * gameTimer.DeltaTime();

	if (_transform.LocalPosition.GetY() <= 0.75f)
	{
		_transform.LocalPosition.SetY(0.75f);
		_actionState = EnemyPurpleActionState::Stop;
		_localTimer = 0.0f;
	}
}
void EnemyPurple::Stop(GameTimer& gameTimer)
{
	_speed.SetY(0);
	if (_localTimer >= 1.5f)
	{
		_actionState = EnemyPurpleActionState::Move;
		_localTimer = 0.0f;
	}
}
void EnemyPurple::Move(GameTimer& gameTimer, const Player& player)
{
	_transform.LocalPosition.SetX(player.GetTransform().LocalPosition.GetX());
	if (_localTimer >= 1.0f)
	{
		Bullet::ActiveBullet(_transform.LocalPosition, Vector3(0, 1.0f, 0.0f), BulletType::EnemyBulletBlue);
		_localTimer = 0.0f;
		_prepareLaserCounter++;
	}
	if (_prepareLaserCounter == 4)
	{
		_actionState = EnemyPurpleActionState::LaserPrepare;
		_localTimer  = 0.0f;
		_isCharge    = true;
		_chargeSound.get()->Play();
		
	}
}
void EnemyPurple::LaserPrepare(GameTimer& gameTimer)
{
	_chargeRotation += 0.1f;
	_chargeSprite.UpdateSpriteForTexture(Float3(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY() - 0.1f, _transform.LocalPosition.GetZ()), Float2(207.0f / 256.0f, 217.0f / 256.0f), Float2(105.0f / 256.0f, 115.0f / 256.0f), 1.0f, _chargeRotation);
	if (_localTimer >= 1.2f)
	{
		_chargeSound.get()->Stop();
		_laserSound .get()->Play();
	}
	if (_localTimer >= 3.0f)
	{
		_isCharge        = false;
		_actionState     = EnemyPurpleActionState::Laser;
		_localTimer      = 0;
		_enableFireLaser = true;
		_chargeRotation  = 0.0f;
		_laserSprite.UpdateSpriteForTexture(Float3(_transform.LocalPosition.GetX(), - 0.3f, 0), Float2(0, 1), Float2(0, 1));
		_laserCollider.centerPosition = Float3(_transform.LocalPosition.GetX(), -0.7f, 0);

	}
}
void EnemyPurple::Laser(GameTimer& gameTimer)
{
	// ƒŒ[ƒU[‚¾‚·.
	if (_localTimer > 5.0f)
	{
		_actionState     = EnemyPurpleActionState::LaserAfter;
		_localTimer      = 0.0f;
		_lerpT           = 0.0f;
		_enableFireLaser = false;
	}
}
void EnemyPurple::LaserAfter(GameTimer& gameTimer, const Player& player)
{
	_lerpT += 0.004f;
	_transform.LocalPosition = Lerp(_transform.LocalPosition, Vector3(player.GetTransform().LocalPosition.GetX(), _transform.LocalPosition.GetY(), 0.0f), _lerpT);
	if (_lerpT >= 1.0f)
	{
		_localTimer  = 0;
		_actionState = EnemyPurpleActionState::Move;
		_lerpT       = 0.0f;
	}
}