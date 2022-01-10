//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyRed.cpp
///             @brief  EnemyRed
///             @author Toide Yutaro
///             @date   2021_12_08
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/EnemyRed.hpp"
#include "MainGame/ShootingStar/Include/Player/Player.hpp"
#include "GameCore/Include/Screen.hpp"
#include "GameMath/Include/GMDistribution.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define DEFAULT_SPEED_Y    (0.8f)
#define ENEMY_TARGET_SPEED (1.9f)
#define STOP_POSITION_Y    (0.7f)       // 立ち止まる場所
#define PROCEED_MOVE_ACTION_TIME (1.5f) // MoveActionStateに移行する時間
#define MOVE_ACTION_COUNT        (5)    // 横移動
static RandomInt g_Random;

static Float2 g_Barrier[] =
{
	{0.0f / 5.0f, 0.0f},
	{1.0f / 5.0f, 0.0f},
	{2.0f / 5.0f, 0.0f},
	{3.0f / 5.0f, 0.0f},
	{4.0f / 5.0f, 0.0f},

	{0.0f / 5.0f, 0.5f}, // 5
	{1.0f / 5.0f, 0.5f},
	{2.0f / 5.0f, 0.5f},
	{3.0f / 5.0f, 0.5f},
	{4.0f / 5.0f, 0.5f},
	{3.0f / 5.0f, 0.5f},
	{2.0f / 5.0f, 0.5f},
	{1.0f / 5.0f, 0.5f}  // 12
};

static float g_BarrierPatternTable[] =
{
	0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f
};
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                      Initialize
*************************************************************************//**
*  @fn        void EnemyRed::Initialize()
*  @brief     Initialize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyRed::Initialize()
{
	_actorType = ActorType::Sprite;
	_name      = "EnemyRed";
	_isActive  = false;
	_transform.LocalPosition = Vector3(0.0f, 1.0f + _enemySize / 2.0f, 0.0f);
	_speed       = Vector3(0.0f, 0.8f, 0.0f);
	_colliderBox = Collider2D::CreateRectangleCollider(Float2(_transform.LocalPosition.GetX(), _transform.LocalPosition.GetY()), _enemySize / Screen::GetAspectRatio(), _enemySize);
	_actionState = EnemyRedActionState::None;
	_enemyType   = EnemyType::Red;
	_rotation    = GM_PI;
	_hp = _defaultHP;

	/*-------------------------------------------------------------------
	-           Load Texture
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/enemy.png", _texture);
	_sprite.CreateSprite(_transform.LocalPosition.ToFloat3(), Float2(_enemySize / Screen::GetAspectRatio(), _enemySize),Float4(1.0f,0.4f,0.4f, 1.0f), Float2(0, 1), Float2(0, 1), _rotation);
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/barrier.png", _barrierTexture);
	_barrierSprite.CreateSprite(_transform.LocalPosition.ToFloat3(), Float2(_enemySize / Screen::GetAspectRatio() + 0.2f, _enemySize + 0.2f), Float4(1.0f, 1.0f, 1.0f, 1.0f), Float2(0, 1), Float2(0, 1), 0);

	g_Random.SetRange(0, 4);

	PushBackEnemy(this);
}
/****************************************************************************
*                      Finalize
*************************************************************************//**
*  @fn        void EnemyRed::Finalize()
*  @brief     Finalize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void EnemyRed::Finalize()
{
	_texture.Resource->Release();
	_barrierTexture.Resource->Release();
}
/****************************************************************************
*                      Update
*************************************************************************//**
*  @fn        void EnemyRed::Update(GameTimer& gameTimer, const Player& player)
*  @brief     Update action
*  @param[in] GameTimer& gameTimer
*  @param[in] Player& player
*  @return 　　void
*****************************************************************************/
void EnemyRed::Update(GameTimer& gameTimer, const Player& player)
{
	/*-------------------------------------------------------------------
	-           Active Check
	---------------------------------------------------------------------*/
	if (!IsActive()) 
	{
		if (_isBarrier) { _isBarrier = false; }
		return; 
	}

	/*-------------------------------------------------------------------
	-           Update Local Timer
	---------------------------------------------------------------------*/
	_localTimer += gameTimer.DeltaTime();

	/*-------------------------------------------------------------------
	-           Decide Action State
	---------------------------------------------------------------------*/
	switch (_actionState)
	{
		case EnemyRedActionState::Appear:
		{
			Appear(gameTimer);
			break;
		}
		case EnemyRedActionState::Stop:
		{
			Stop(gameTimer);
			break;
		}
		case EnemyRedActionState::Move:
		{
			Move(gameTimer);
			break;
		}
		case EnemyRedActionState::AttackPrepare:
		{
			AttackPrepare(gameTimer, player);
			break;
		}
		case EnemyRedActionState::Attack:
		{
			Attack(gameTimer);
			break;
		}
		case EnemyRedActionState::AttackAfter:
		{
			AttackAfter(gameTimer);
			break;
		}
		default: break;
	}
	/*-------------------------------------------------------------------
	-           Update Collider
	---------------------------------------------------------------------*/
	_colliderBox.centerPosition = _transform.LocalPosition.ToFloat3();
	/*-------------------------------------------------------------------
	-           Update Sprite
	---------------------------------------------------------------------*/
	_sprite.UpdateSprite(_transform.LocalPosition.ToFloat3(), Float4(1.0f, 0.4f, 0.4f, 1.0f), Float2(0, 1), Float2(0, 1), _rotation);
	/*-------------------------------------------------------------------
	-           Update Barrier Action
	---------------------------------------------------------------------*/
	if (_isBarrier)
	{
		_barrierSprite.UpdateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(g_Barrier[_barrierIndex].x, g_Barrier[_barrierIndex].x + 0.2f),
			Float2(g_Barrier[_barrierIndex].y, g_Barrier[_barrierIndex].y + 0.5f), 0.8f, 0.0f);
	}
}

void EnemyRed::Reset()
{

}
/****************************************************************************
*                      Generate
*************************************************************************//**
*  @fn        bool EnemyRed::Generate(const Vector3& position)
*  @brief     Finalize
*  @param[in] Vector3& screenPosition (in NDC Space)
*  @return 　　bool
*****************************************************************************/
bool EnemyRed::Generate(const Vector3& position)
{
	if (!IsActive())
	{
		_actionState                = EnemyRedActionState::Appear;
		_transform.LocalPosition    = position;
		_colliderBox.centerPosition = Float3(position.GetX(), position.GetY(), position.GetZ());
		_speed                      = Vector3(0.0f, 0.8f, 0.0f);
		_rotation                   = GM_PI;
		_hp                         = _defaultHP;
		_lerpT                      = 0;
		_isBarrier                  = false;
		_barrierSprite.CreateSprite(_transform.LocalPosition.ToFloat3(), Float2(_enemySize / Screen::GetAspectRatio() + 0.2f, _enemySize + 0.2f), Float4(1.0f, 1.0f, 1.0f, 1.0f), Float2(0, 1), Float2(0, 1), 0);
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
void EnemyRed::Appear(GameTimer& gameTimer)
{
	_transform.LocalPosition -= _speed * gameTimer.DeltaTime();

	/*-------------------------------------------------------------------
	-          Proceed to the stop position
	---------------------------------------------------------------------*/
	if (_transform.LocalPosition.GetY() <= STOP_POSITION_Y)
	{
		_transform.LocalPosition.SetY(STOP_POSITION_Y);
		_actionState = EnemyRedActionState::Stop;
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
void EnemyRed::Stop(GameTimer& gameTimer)
{
	_speed.SetY(0);
	if (_localTimer >= PROCEED_MOVE_ACTION_TIME)
	{
		_actionState = EnemyRedActionState::Move;
		_localTimer = 0.0f;
	}
}
/****************************************************************************
*                       Move
*************************************************************************//**
*  @fn        void EnemyRed::Move(GameTimer& gameTimer)
*  @brief     Random appear move action
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyRed::Move(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-         Move Random Position
	---------------------------------------------------------------------*/
	if (_localTimer >= 0.75f)
	{
		_transform.LocalPosition.SetX((float)(g_Random.GetRandomValue()) * 0.4f - 0.75f);
		_moveCount++;
		_localTimer = 0;
	}
	/*-------------------------------------------------------------------
	-          Proceed to the attack prepare state
	---------------------------------------------------------------------*/
	if (_moveCount == 5)
	{
		_actionState = EnemyRedActionState::AttackPrepare;
		_localTimer  = 0.0f;
		_moveCount   = 0;
		_isBarrier   = true;
	}
}
/****************************************************************************
*                       AttackPrepare
*************************************************************************//**
*  @fn        void EnemyRed::AttackPrepare(GameTimer& gameTimer, const Player& player)
*  @brief     Attack prepare motion (decide attack position)
*  @param[in] GameTimer& gameTimer
*  @param[in] Player player
*  @return 　　void
*****************************************************************************/
void EnemyRed::AttackPrepare(GameTimer& gameTimer, const Player& player)
{
	/*-------------------------------------------------------------------
	-          Barrier Animation
	---------------------------------------------------------------------*/
	if (_localTimer >= g_BarrierPatternTable[_barrierIndex])
	{
		_barrierIndex++;
		if (_barrierIndex >= 13)
		{
			_barrierIndex = 5;
		}
		_localTimer = 0;
	}
	/*-------------------------------------------------------------------
	-          Proceed to the Attack Motion
	---------------------------------------------------------------------*/
	if (_barrierIndex >= 12)
	{
		_previousPlayerPosition = player.GetTransform().LocalPosition;
		_previousEnemyPosition  = _transform.LocalPosition;
		_actionState            = EnemyRedActionState::Attack;
		_localTimer             = 0;
	}
}
/****************************************************************************
*                      Attack
*************************************************************************//**
*  @fn        void EnemyRed::Attack(GameTimer& gameTimer)
*  @brief     Attack Action
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyRed::Attack(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-         Barrier Animation
	---------------------------------------------------------------------*/
	if (_localTimer >= g_BarrierPatternTable[_barrierIndex])
	{
		_barrierIndex++;
		if (_barrierIndex >= 13)
		{
			_barrierIndex = 5;
		}
		_localTimer = 0;
	}

	/*-------------------------------------------------------------------
	-          Update Lerp
	---------------------------------------------------------------------*/
	_lerpT += ENEMY_TARGET_SPEED * gameTimer.DeltaTime();
	/*-------------------------------------------------------------------
	-          Update Position
	---------------------------------------------------------------------*/
	_transform.LocalPosition = Lerp(_previousEnemyPosition, _previousPlayerPosition, _lerpT);
	/*-------------------------------------------------------------------
	-          Proceed to the AttackAfter
	---------------------------------------------------------------------*/
	if (_lerpT >= 1.0f)
	{
		_actionState = EnemyRedActionState::AttackAfter;
		_localTimer  = 0;
		_lerpT       = 0;
	}
}
/****************************************************************************
*                      AttackAfter
*************************************************************************//**
*  @fn        void EnemyRed::AttackAfter(GameTimer& gameTimer)
*  @brief     Attack After Action
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void EnemyRed::AttackAfter(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-          Reset Barrier Animation
	---------------------------------------------------------------------*/
	if (_localTimer >= g_BarrierPatternTable[_barrierIndex])
	{
		_barrierIndex++;
		if (_barrierIndex >= 13)
		{
			_barrierIndex = 5;
		}
		_localTimer = 0;
	}
	/*-------------------------------------------------------------------
	-          Update Lerp
	---------------------------------------------------------------------*/
	_lerpT += ENEMY_TARGET_SPEED * gameTimer.DeltaTime();
	/*-------------------------------------------------------------------
	-          Return to the previous position
	---------------------------------------------------------------------*/
	_transform.LocalPosition = Lerp(_previousPlayerPosition, _previousEnemyPosition, _lerpT);

	/*-------------------------------------------------------------------
	-          Proceed to the move state
	---------------------------------------------------------------------*/
	if (_lerpT >= 1.0f)
	{
		_actionState  = EnemyRedActionState::Move;
		_localTimer   = 0.0f;
		_lerpT        = 0;
		_barrierIndex = 0;
		_isBarrier    = false;
	}
}