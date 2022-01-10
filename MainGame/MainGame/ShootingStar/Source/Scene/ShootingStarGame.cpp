//////////////////////////////////////////////////////////////////////////////////
//              @file   ShootingStarGame.cpp
///             @brief  Shooting star game.cpp
///             @author Toide Yutaro
///             @date   2021_12_11
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Scene/ShootingStarGame.hpp"
#include "MainGame/ShootingStar/Include/Scene/ShootingStarResult.hpp"
#include "MainGame/ShootingStar/Include/Scene/ShootingStarGameOver.hpp"
#include "MainGame/ShootingStar/Include/Scene/ShootingStarGameClear.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "GameCore/Include/Collision/Collision.hpp"
#include "MainGame/Core/Include/SceneManager.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include "MainGame/ShootingStar/Include/Player/Player.hpp"
#include "MainGame/ShootingStar/Include/Player/PlayerHP.hpp"
#include "MainGame/ShootingStar/Include/Bullet/Bullet.hpp"
#include "MainGame/ShootingStar/Include/Bullet/DamageEffect.hpp"
#include "MainGame/ShootingStar/Include/Enemy/Enemy.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyManager.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyPurple.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyRed.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyBoss.hpp"
#include "GameCore/Include/Audio/AudioTable.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
ShootingStarGame::ShootingStarGame()
{
	_spriteRenderer = std::make_unique<SpriteRenderer>();
	_audioSource    = std::make_unique<AudioSource>();
	_player         = std::make_unique<Player>();

}

ShootingStarGame::~ShootingStarGame()
{

}

void ShootingStarGame::Initialize(GameTimer& gameTimer)
{
	Scene::Initialize(gameTimer);
	EnemyManager::Instance().Initialize();
	_directX12.CompleteInitialize();
}
/****************************************************************************
*                          Update
*************************************************************************//**
*  @fn        void MMDRendererTitle::Update()
*  @brief     Update scene objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarGame::Update()
{
	/*-------------------------------------------------------------------
	-           Check Input
	---------------------------------------------------------------------*/
	Scene::Update();
	/*-------------------------------------------------------------------
	-           Update BackGround
	---------------------------------------------------------------------*/
	UpdateBackGround();
	/*-------------------------------------------------------------------
	-           Update Player
	---------------------------------------------------------------------*/
	_player.get()->Update(*_gameTimer);
	/*-------------------------------------------------------------------
	-           Update Enemy
	---------------------------------------------------------------------*/
	EnemyManager::Instance().Update(*_gameTimer, *_player.get());
	/*-------------------------------------------------------------------
	-           Update Bullet
	---------------------------------------------------------------------*/
	Bullet::AllBulletsUpdate(*_gameTimer);
	/*-------------------------------------------------------------------
	-           Update DamageEffect
	---------------------------------------------------------------------*/
	DamageEffect::AllEffectsUpdate(*_gameTimer);
	/*-------------------------------------------------------------------
	-           CollisionDetections
	---------------------------------------------------------------------*/
	CollisionDetections();
	/*-------------------------------------------------------------------
	-           PlayerHP UI
	---------------------------------------------------------------------*/
	UIPlayerHP::ManageUI(_player.get()->GetHP());

	/*-------------------------------------------------------------------
	-           Check Scene transition
	---------------------------------------------------------------------*/
	if (_player.get()->HasMotionFinished()) { SceneManager::Instance().TransitScene(new ShootingStarGameClear(), *_gameTimer); }
	else if (_player.get()->GetHP() <= 0) { SceneManager::Instance().TransitScene(new ShootingStarGameOver(), *_gameTimer); }
}
/****************************************************************************
*                          Draw
*************************************************************************//**
*  @fn        void MMDRendererTitle::Draw()
*  @brief     Draw objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarGame::Draw()
{
	/*-------------------------------------------------------------------
	-           Clear screen
	---------------------------------------------------------------------*/
	_directX12.ClearScreen();
	/*-------------------------------------------------------------------
	-           Draw All Sprite
	---------------------------------------------------------------------*/
	DrawSprites();
	/*-------------------------------------------------------------------
	-           Call rendering complete
	---------------------------------------------------------------------*/
	_directX12.CompleteRendering();
}
/****************************************************************************
*                          Terminate
*************************************************************************//**
*  @fn        bool MMDRendererTitle::LoadMaterials(GameTimer& gameTimer)
*  @brief     This function is used in case scene transition
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarGame::Terminate()
{
	/*-------------------------------------------------------------------
	-           Clear Audio
	---------------------------------------------------------------------*/
	_audioSource.get()->Stop(); _audioSource.reset();
	AudioTableManager::Instance().ClearAudioTable();
	/*-------------------------------------------------------------------
	-           Clear 2DObject
	---------------------------------------------------------------------*/
	TextureTableManager::Instance().ClearTextureTable();
	UIPlayerHP  ::ClearAllPlayerHP();
	Bullet      ::ClearAllBullets();
	DamageEffect::ClearAllEffects();
	GameObject  ::ClearAllGameObjects();
	EnemyManager::Instance().Terminate();
	_player.get()->Finalize(); _player.reset();
	_backGround.Clear();
	_redArrow  .Clear();

	_spriteRenderer.get()->Finalize();
	_spriteRenderer.reset();

	_directX12.ResetCommandList();

}

/****************************************************************************
*                          LoadMaterials
*************************************************************************//**
*  @fn        bool MMDRendererTitle::LoadMaterials(GameTimer& gameTimer)
*  @brief     Load materials
*  @param[in] GameTimer& gameTimer
*  @return 　　bool
*****************************************************************************/
bool ShootingStarGame::LoadMaterials(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-           Prepare renderer
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Initialize();

	/*-------------------------------------------------------------------
	-           Prepare BackGround
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/milky-way.png", *_backGround.TexturePtr.get());
	_backGround.SpritePtr.get()->CreateSpriteForTexture(Float3(0, 0, 0), Float2(2, 2), Float2(0, 1), Float2(0, 1));

	/*-------------------------------------------------------------------
	-           Audio Source
	---------------------------------------------------------------------*/
	_audioSource.get()->LoadSound(L"Resources/Audio/ShootingStar/digitalworld.wav", SoundType::BGM, 0.5f);
	_audioSource.get()->Play();

	/*-------------------------------------------------------------------
	-           Player
	---------------------------------------------------------------------*/
	_player.get()->Initialize();
	UIPlayerHP::GenerateUI(Vector3(-1.0f + 0.1f / Screen::GetAspectRatio(), 0.85f, 0), _player.get()->GetHP(), 0.1f);
	/*-------------------------------------------------------------------
	-           Enemy
	---------------------------------------------------------------------*/

	/*-------------------------------------------------------------------
	-           Bullet
	---------------------------------------------------------------------*/
	Bullet::Initialize();
	Bullet::GenerateBullets(_bulletsBufferNum, BulletType::Player, BulletColor::Red);
	Bullet::GenerateBullets(_enemyBulletsBufferNum, BulletType::EnemyBulletBlue, BulletColor::Blue);
	Bullet::GenerateBullets(_enemyBulletsBufferNum, BulletType::EnemyBulletGreen, BulletColor::Green);
	
	/*-------------------------------------------------------------------
	-           Damage Effect
	---------------------------------------------------------------------*/
	DamageEffect::CreateEffects(_effectBufferNum);

	/*-------------------------------------------------------------------
	-           Laser direction arrow
	---------------------------------------------------------------------*/
	textureLoader.LoadTexture(L"Resources/Texture/Default/Arrow.png", *_redArrow.TexturePtr);
	_redArrow.SpritePtr.get()->CreateSprite(Float3(0, 0, 0), Float2(0.1f, 0.1f),Float4(1,0,0,1), Float2(0, 0), Float2(0, 0), 3 * GM_PI / 4);
	return true;
}

/****************************************************************************
*                          OnKeyboardInput
*************************************************************************//**
*  @fn        void MainBattle::OnKeyboardInput()
*  @brief     Keyboard Input
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarGame::OnKeyboardInput()
{
	// 弾の発射処理
	if (_gameInput.GetKeyboard().IsTrigger(DIK_SPACE))
	{
		Bullet::ActiveBullet(_player.get()->GetTransform().LocalPosition, Vector3(0, -2.0f, 0.0f), BulletType::Player);
	}
}

/****************************************************************************
*                          DrawSprites
*************************************************************************//**
*  @fn        void MMDRendererTitle::DrawSprites()
*  @brief     Draw sprite (backGround and text)
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarGame::DrawSprites()
{
	/*-------------------------------------------------------------------
	-           Prepare draw
	---------------------------------------------------------------------*/
	std::vector<Sprite> backGround, player, enemies, laser, charge, barrier, damage, hp, bossLaser, laserInstruction;
	backGround.push_back(*_backGround.SpritePtr.get());
	_spriteRenderer.get()->DrawStart();

	/*-------------------------------------------------------------------
	-           PlayerHP UI
	---------------------------------------------------------------------*/
	if (_player.get()->GetHP() > 0)
	{
		Texture texture;
		for (auto& ui : UIPlayerHP::GetAllUI())
		{
			if (ui->IsActive())
			{
				hp.push_back(ui->GetSprite());
				texture = ui->GetTexture();
			}
		}

		if (hp.size() != 0) _spriteRenderer.get()->Draw(hp, texture, MatrixIdentity());
	}


	/*-------------------------------------------------------------------
	-            Hit Animation
	---------------------------------------------------------------------*/
	if (DamageEffect::AllEffects().size() != 0)
	{
		Texture texture;
		for (auto& effect : DamageEffect::AllEffects())
		{
			if (effect->IsActive())
			{
				texture = effect->GetTexture();
				damage.push_back(effect->GetSprite());
			}
		}
		if (damage.size() != 0) _spriteRenderer.get()->Draw(damage, texture, MatrixIdentity());
	}

	/*-------------------------------------------------------------------
	-           Boss Laser
	---------------------------------------------------------------------*/
	Texture bossLaserTexture;
	
	for (int i = 0; i < EnemyManager::Instance().GetEnemyBossCount(); ++i)
	{
		EnemyBoss* enemy = (EnemyBoss*)EnemyManager::Instance().GetEnemy(EnemyType::Boss, i);
		if (enemy->EnableShotLaser() || enemy->IsCharging())
		{
			bossLaser.push_back(enemy->GetLaserSprite());
			bossLaserTexture = enemy->GetLaserTexture();
		}
		if (enemy->IsCharging())
		{
			Float3 temp = Float3(enemy->GetTransform().LocalPosition.GetX() + 0.15f, enemy->GetTransform().LocalPosition.GetY() -0.22f, enemy->GetTransform().LocalPosition.GetZ());
			_redArrow.SpritePtr.get()->UpdateSprite(temp, Float4(1,0,0,1), Float2(0, 1), Float2(0, 1), 3 * GM_PI / 4.0f);
			laserInstruction.push_back(*_redArrow.SpritePtr.get());
		}

	}
	if (bossLaser.size() != 0) { _spriteRenderer.get()->Draw(bossLaser, bossLaserTexture, MatrixIdentity()); }
	if (laserInstruction.size() != 0) { _spriteRenderer.get()->Draw(laserInstruction, *_redArrow.TexturePtr.get(), MatrixIdentity()); }

	/*-------------------------------------------------------------------
	-            Bullet
	---------------------------------------------------------------------*/
	if (Bullet::AllBullets().size() != 0)
	{
		for (int i = 0; i < (int)BulletType::CountOfBulletType; ++i)
		{
			std::vector<Sprite> bullets;
			Texture texture;
			for (auto& bullet : Bullet::AllBullets())
			{
				if (bullet->GetBulletType() != (BulletType)i) { continue; }
				if (bullet->IsActive())
				{
					texture = bullet->GetTexture();
					bullets.push_back(bullet->GetSprite());
				}
			}
			if(bullets.size() != 0)_spriteRenderer.get()->Draw(bullets, texture, MatrixIdentity());
		}
		
	}
	
	/*-------------------------------------------------------------------
	-           Charge
	---------------------------------------------------------------------*/
	Texture chargeTexture;
	for (int i = 0; i < EnemyManager::Instance().GetEnemyPurpleCount(); ++i)
	{
		EnemyPurple* enemy = (EnemyPurple*)EnemyManager::Instance().GetEnemy(EnemyType::Purple, i);
		if (enemy->IsCharging())
		{
			charge.push_back(enemy->GetChargeSprite());
			chargeTexture = enemy->GetChargeTexture();
		}

	}
	if (charge.size() != 0) { _spriteRenderer.get()->Draw(charge, chargeTexture, MatrixIdentity()); }

	/*-------------------------------------------------------------------
	-           Laser
	---------------------------------------------------------------------*/
	Texture laserTexture;
	for (int i = 0; i < EnemyManager::Instance().GetEnemyPurpleCount(); ++i)
	{
		EnemyPurple* enemy = (EnemyPurple*)EnemyManager::Instance().GetEnemy(EnemyType::Purple, i);
		if (enemy->EnableFireLaser())
		{
			laser.push_back(enemy->GetLaserSprite());
			laserTexture = enemy->GetLaserTexture();
		}

	}
	if (laser.size() != 0) { _spriteRenderer.get()->Draw(laser, laserTexture, MatrixIdentity()); }

	/*-------------------------------------------------------------------
	-           Barrier
	---------------------------------------------------------------------*/
	Texture barrierTexture;
	for (int i = 0; i < EnemyManager::Instance().GetEnemyRedCount(); ++i)
	{
		EnemyRed* enemy = (EnemyRed*)EnemyManager::Instance().GetEnemy(EnemyType::Red, i);
		if (enemy->IsBarrier())
		{
			barrier.push_back(enemy->GetBarrierSprite());
			barrierTexture = enemy->GetBarrierTexture();
		}

	}
	if (barrier.size() != 0) { _spriteRenderer.get()->Draw(barrier, barrierTexture, MatrixIdentity()); }

	
	/*-------------------------------------------------------------------
	-            Player
	---------------------------------------------------------------------*/
	player.push_back(_player.get()->GetSprite());
	_spriteRenderer.get()->Draw(player, _player.get()->GetTexture(), MatrixIdentity());
	/*-------------------------------------------------------------------
	-            Enemy
	---------------------------------------------------------------------*/
	Texture texture;
	EnemyManager::Instance().GetDrawMaterial(EnemyType::Default, enemies, texture);
	if(enemies.size() > 0)_spriteRenderer.get()->Draw(enemies, texture, MatrixIdentity());
	EnemyManager::Instance().GetDrawMaterial(EnemyType::Blue, enemies, texture);
	if (enemies.size() > 0)_spriteRenderer.get()->Draw(enemies, texture, MatrixIdentity());
	EnemyManager::Instance().GetDrawMaterial(EnemyType::LightGreen, enemies, texture);
	if (enemies.size() > 0)_spriteRenderer.get()->Draw(enemies, texture, MatrixIdentity());
	EnemyManager::Instance().GetDrawMaterial(EnemyType::Purple, enemies, texture);
	if (enemies.size() > 0)_spriteRenderer.get()->Draw(enemies, texture, MatrixIdentity());
	EnemyManager::Instance().GetDrawMaterial(EnemyType::Red, enemies, texture);
	if (enemies.size() > 0)_spriteRenderer.get()->Draw(enemies, texture, MatrixIdentity());
	EnemyManager::Instance().GetDrawMaterial(EnemyType::Boss, enemies, texture);
	if (enemies.size() > 0)_spriteRenderer.get()->Draw(enemies, texture, MatrixIdentity());
	/*-------------------------------------------------------------------
	-           BackGroundTexture
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Draw(backGround, *_backGround.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->DrawEnd();

}

void ShootingStarGame::UpdateBackGround()
{
	_backGroundAnimationTime += _gameTimer->DeltaTime();
	if (_backGroundAnimationTime >= BACK_GROUND_WAIT_TIME)
	{
		_backGroundAnimationTime = 0.0f;
	}

	_backGround.SpritePtr.get()->UpdateSpriteForTexture(
		Float3(0, 0, 0), Float2(0.0f, 1.0f),
		Float2(_backGroundAnimationTime / BACK_GROUND_WAIT_TIME + 0.5f, _backGroundAnimationTime / BACK_GROUND_WAIT_TIME), 1.0f);

}

void ShootingStarGame::CollisionDetections()
{
	auto enemies = EnemyManager::Instance().GetActiveEnemies();
	auto bullets = Bullet::AllActiveBullets();
	auto playerBullets = Bullet::AllActivePlayerBullets();
	/*-------------------------------------------------------------------
	-           Player Bullet vs Enemy
	---------------------------------------------------------------------*/
	for (auto& enemy : enemies)
	{
		for (auto& bullet : playerBullets)
		{
			if (Collision::OnCollision2D(bullet->GetColBox(), enemy->GetColBox()))
			{
				if (enemy->GetEnemyType() == EnemyType::Boss && static_cast<EnemyBoss*>(enemy)->IsCharging())
				{
					bullet->SetActive(false);
					enemy->Damage(_player.get()->GetAttackPower());
					DamageEffect::ActiveEffect(Vector3(enemy->GetTransform().LocalPosition + Vector3(0.05f, -0.3f, 0)));
					if (enemy->IsHPZero())
					{
						_player.get()->SetIsClear(true);
					}
				}
				else if(enemy->GetEnemyType() == EnemyType::Boss){}
				else
				{
					bullet->SetActive(false);
					enemy->Damage(_player.get()->GetAttackPower());
					DamageEffect::ActiveEffect(enemy->GetTransform().LocalPosition);
				}
				if (enemy->IsHPZero())
				{
					enemy->SetActive(false);
				}
			}
		}
	}

	/*-------------------------------------------------------------------
	-           Player Bullet vs Enemy Bullet
	---------------------------------------------------------------------*/
	for (auto& bullet : bullets)
	{
		if (bullet->GetBulletType() == BulletType::Player) { continue; };
		for (auto& playerBullet : playerBullets)
		{
			if (Collision::OnCollision2D(bullet->GetColBox(), playerBullet->GetColBox()))
			{
				bullet->SetActive(false);
				playerBullet->SetActive(false);
			}
		}
	}

	if (!_player.get()->HasDamaged() && !_player.get()->GetIsClear())
	{
		/*-------------------------------------------------------------------
		-           Enemy Bullet vs Player
		---------------------------------------------------------------------*/
		for (auto& bullet : bullets)
		{
			if (bullet->GetBulletType() == BulletType::Player) { continue; }
			if (Collision::OnCollision2D(bullet->GetColBox(), _player.get()->GetColBox()))
			{
				_player.get()->Damage();
				bullet->SetActive(false);
				DamageEffect::ActiveEffect(_player.get()->GetTransform().LocalPosition);
				break;
			}
		}

		/*-------------------------------------------------------------------
		-           Player vs Enemy
		---------------------------------------------------------------------*/
		for (auto& enemy : enemies)
		{
			if (Collision::OnCollision2D(_player.get()->GetColBox(), enemy->GetColBox()))
			{
				_player.get()->Damage();
				enemy->SetActive(false);
				DamageEffect::ActiveEffect(_player.get()->GetTransform().LocalPosition);
				break;
			}
		}

		/*-------------------------------------------------------------------
		-           Player vs EnemyPurple Laser
		---------------------------------------------------------------------*/
		for (int i = 0; i < EnemyManager::Instance().GetEnemyPurpleCount(); ++i)
		{
			EnemyPurple* enemy = (EnemyPurple*)EnemyManager::Instance().GetEnemy(EnemyType::Purple, i);
			if (enemy->IsActive() && enemy->EnableFireLaser())
			{
 				if (Collision::OnCollision2D(_player.get()->GetColBox(), enemy->GetLaserCollider()))
				{
					_player.get()->Damage();
					DamageEffect::ActiveEffect(_player.get()->GetTransform().LocalPosition);
				}
			}
		}

		/*-------------------------------------------------------------------
		-           Player vs EnemyBoss Laser
		---------------------------------------------------------------------*/
		for (int i = 0; i < EnemyManager::Instance().GetEnemyBossCount(); ++i)
		{
			EnemyBoss* enemy = (EnemyBoss*)EnemyManager::Instance().GetEnemy(EnemyType::Boss, i);
			if (enemy->IsActive() && enemy->EnableShotLaser())
			{
				if (Collision::OnCollision2D(_player.get()->GetColBox(), enemy->GetLaserCollider()))
				{
					_player.get()->Damage();
					DamageEffect::ActiveEffect(_player.get()->GetTransform().LocalPosition);
				}
			}
		}

	}
}