//////////////////////////////////////////////////////////////////////////////////
///             @file   Bullet.cpp
///             @brief  Bullet
///             @author Toide Yutaro
///             @date   ----------------
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Bullet/Bullet.hpp"
#include "GameCore/Include/Screen.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
std::vector<Bullet*> Bullet::_bullets;
Texture Bullet::_textureColorList[(int)BulletColor::CountOfColorType];
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
Bullet::Bullet(BulletType type, BulletColor color)
{
	_actorType      = ActorType::Sprite;
	_name           = type == BulletType::Player ? "PlayerBullet" : "EnemyBullet";;
	_transform      = Transform();
	_speed          = Vector3(0.0f, -2.0f, 0.0f);
	_colliderBox = Collider2D::CreateRectangleCollider(Float2(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY()), _bulletSize / Screen::GetAspectRatio(), _bulletSize);
	_isActive       = false;
	_bulletType     = type;

	/*-------------------------------------------------------------------
	-          Load texture
	---------------------------------------------------------------------*/
	_texture = _textureColorList[(int)color];
	_sprite.CreateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(_bulletSize / Screen::GetAspectRatio(), _bulletSize), Float2(0, 1), Float2(0, 1));

	_bullets.push_back(this);

}
void Bullet::Initialize()
{
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/shootPlayer.png", _textureColorList[(int)BulletColor::Red]);
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/shootEnemy.png", _textureColorList[(int)BulletColor::Blue]);
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/shootGreen.png", _textureColorList[(int)BulletColor::Green]);
}

void Bullet::GenerateBullets(int bulletNum, BulletType type, BulletColor color)
{
	for (int i = 0; i < bulletNum; ++i)
	{
		new Bullet(type, color); //deleteはDestroyが担当する. (ちゃんと呼び出すこと.)
	}
}
void Bullet::AllBulletsUpdate(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-          Execute All bullet update 
	---------------------------------------------------------------------*/
	for (auto it = _bullets.begin(); it != _bullets.end(); ++it)
	{
		(*it)->Update(gameTimer);
	}
}
std::vector<Bullet*> Bullet::AllActiveBullets()
{
	std::vector<Bullet*> bullets;
	for (auto& bullet : _bullets)
	{
		if (bullet->IsActive())
		{
			bullets.push_back(bullet);
		}
	}
	return bullets;
}
std::vector<Bullet*> Bullet::AllActivePlayerBullets()
{
	std::vector<Bullet*> bullets;
	for (auto& bullet : _bullets)
	{
		if (bullet->IsActive() && bullet->GetBulletType() == BulletType::Player)
		{
			bullets.push_back(bullet);
		}
	}
	return bullets;
}
/****************************************************************************
*                       Update
*************************************************************************//**
*  @fn        void Bullet::Update(GameTimer& gameTimer)
*  @brief     Update 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Bullet::Update(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-         Whether to  Active 
	---------------------------------------------------------------------*/
	if (!IsActive()) { return; }

	/*-------------------------------------------------------------------
	-          Update position
	---------------------------------------------------------------------*/
	Vector3 position = _transform.LocalPosition;
	position        -= _speed * gameTimer.DeltaTime();
	
	/*-------------------------------------------------------------------
	-         Update position and collider (現状初期比率で固定する.)
	---------------------------------------------------------------------*/
	_transform.LocalPosition = position;
	_colliderBox.SetCenterPosition(Float2(position.GetX(), position.GetY()));

	/*-------------------------------------------------------------------
	-         Update sprite
	---------------------------------------------------------------------*/
	_sprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(0, 1), Float2(0, 1), 1.0f);

	/*-------------------------------------------------------------------
	-         Delete check
	---------------------------------------------------------------------*/
	if (position.GetY() >= (_bulletSize / 2.0f + 1.0f) || position.GetY() <= -1.0f - _bulletSize / 2.0f)
	{
		_isActive = false;
		_transform.LocalPosition    = Vector3(0, 0, 0);
		_colliderBox.centerPosition = _transform.LocalPosition.ToFloat3();
		_speed = Vector3(0, 0, 0);
	}

}
/****************************************************************************
*                       ActiveBullet
*************************************************************************//**
*  @fn        void Bullet::ActiveBullet(const gm::Vector3& position)
*  @brief     アクティブでないものを見つけてActiveにする.
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Bullet::ActiveBullet(const gm::Vector3& position, const gm::Vector3& velocity, BulletType type)
{
	for (int i = 0; i < _bullets.size(); ++i)
	{
		if (_bullets[i]->_bulletType != type) { continue; }
		if (_bullets[i]->IsActive()) { continue; }
		
		_bullets[i]->SetActive(true);
		_bullets[i]->GetTransform().LocalPosition = position;
		_bullets[i]->_speed = velocity;
		break;
	}
}
/****************************************************************************
*                       ClearAllBullets
*************************************************************************//**
*  @fn        void Bullet::ClearAllBullets()
*  @brief     Clear all bullets
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Bullet::ClearAllBullets()
{
	for (int i = 0; i < _bullets.size(); ++i)
	{
		Destroy(_bullets[i]);
	}
	_bullets.clear();
	_bullets.shrink_to_fit();
}