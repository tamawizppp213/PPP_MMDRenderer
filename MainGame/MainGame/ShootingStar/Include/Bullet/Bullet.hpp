//////////////////////////////////////////////////////////////////////////////////
///             @file   Bullet.hpp
///             @brief  Bullet
///             @author Toide Yutaro
///             @date   2021_12_07
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef BULLET_HPP
#define BULLET_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/GameActor.hpp"
#include "GameCore/Include/Collision/Collider.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include "GameMath/Include/GMVector.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class BulletType
{
	Player,
	EnemyBulletBlue,
	EnemyBulletGreen,
	CountOfBulletType
};

enum class BulletColor
{
	Red,
	Blue,
	Green,
	CountOfColorType
};
/****************************************************************************
*				  			Bullet
*************************************************************************//**
*  @class     Bullet
*  @brief     Player bullet class (in case use this class, you should use "new".)
*****************************************************************************/
class Bullet final : public GameActor
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static void     Initialize();
	static std::vector<Bullet*>& AllBullets() { return _bullets; }
	static std::vector<Bullet*>  AllActiveBullets();
	static std::vector<Bullet*>  AllActivePlayerBullets();
	static void     AllBulletsUpdate(GameTimer& gameTimer);
	static void     ClearAllBullets();
	static void     ActiveBullet(const gm::Vector3& position, const gm::Vector3& velocity,  BulletType type);
	static void     GenerateBullets(int bulletNum, BulletType type, BulletColor color);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	Collider2D&       GetColBox()       { return _colliderBox; }
	const Collider2D& GetColBox() const { return _colliderBox; }
	Texture&          GetTexture  ()  { return _texture; }
	Sprite &          GetSprite   ()  { return _sprite; }
	BulletType        GetBulletType() { return _bulletType; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Bullet(BulletType type, BulletColor color);
	Bullet(const Bullet&)            = default;
	Bullet& operator=(const Bullet&) = default;
	Bullet(Bullet&&)                 = default;
	Bullet& operator=(Bullet&&)      = default;
	~Bullet(){};


private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void Update(GameTimer& gameTimer);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Collider2D  _colliderBox;
	gm::Vector3 _speed;
	Sprite      _sprite;
	Texture     _texture;
	const float _bulletSize = 0.10f;
	BulletType  _bulletType;
	static std::vector<Bullet*> _bullets;

	static Texture _textureColorList[(int)BulletColor::CountOfColorType];
};

#endif