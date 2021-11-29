//////////////////////////////////////////////////////////////////////////////////
///             @file   Enemy.cpp
///             @brief  Enemy
///             @author Toide Yutaro
///             @date   2021_10_27
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/Enemy.hpp"
#include "GameCore/Include/Screen.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define ENEMY_SIZE 64

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                       Function
*************************************************************************//**
*  @fn        void Temp::Temp(void)
*  @brief     test
*  @param[in] test
*  @return Å@Å@void
*****************************************************************************/
void Enemy::Initialize()
{
	_transform.LocalPosition = Vector3(0.0f, 0.0f, 0.0f);
	_speed                   = Vector2(0.0f, 5.0f * Screen::GetOnePixelHeight());
	Float2 colSize           = Float2(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY());
	_colliderCircle          = Collider2D::CreateCircleCollider( colSize, ENEMY_SIZE * Screen::GetOnePixelWidth() / 2);
	_actionState             = ActionState::None;
	_localTimer.Start();
	_sprite.CreateSpriteForTexture(_transform.LocalPosition.ToFloat3(), 
		Float2(ENEMY_SIZE * Screen::GetOnePixelWidth(), ENEMY_SIZE * Screen::GetOnePixelHeight()), 
		Float2(0.0f, 1.0f),  Float2(0.0f, 1.0f));
	
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/ShootingStar/enemy.png", _texture);
}

void Enemy::Update()
{
	if (!IsActive()) { return; }

	switch (_actionState)
	{

	}
}