//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyManager.hpp
///             @brief  EnemyManager
///             @author Toide Yutaro
///             @date   -----------------
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef ENEMY_EMITTER_HPP
#define ENEMY_EMITTER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/GameTimer.hpp"
#include "GameMath/Include/GMVector.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyType.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////
struct Texture;
struct Sprite;
class Player;
class Enemy;
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class EnemyManager
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize();
	void Update(GameTimer& gameTimer, const Player& player);
	void GetDrawMaterial(EnemyType type, std::vector<Sprite>& sprite, Texture& texture);
	void Terminal();
	void GenerateEnemy(EnemyType type, const gm::Vector3& position);

	int GetEnemyDefaultCount() { return _enemyDefaultCount; }
	int GetEnemyBlueCount   () { return _enemyBlueCount; }
	int GetEnemyGreenCount  () { return _enemyGreenCount; }
	int GetEnemyPurpleCount () { return _enemyPurpleCount; }
	int GetEnemyRedCount    () { return _enemyRedCount; }
	int GetEnemyBossCount() { return _enemyBossCount; }
	int GetMaxEnemyAppearCount() { return _maxEmitterCount; }
	int GetCurrentEnemyAppearCount() { return _emitterCount; }

	Enemy* GetEnemy(EnemyType type, int index);
	static std::vector<Enemy*> GetActiveEnemies();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	static EnemyManager& Instance()
	{
		static EnemyManager enemyManager;
		return enemyManager;
	}

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	EnemyManager()                               = default;
	~EnemyManager()                              = default;
	EnemyManager(const EnemyManager&)            = delete;
	EnemyManager& operator=(const EnemyManager&) = delete;
	EnemyManager(EnemyManager&&)                 = delete;
	EnemyManager& operator= (EnemyManager&&)     = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	float _localTimer;
	int   _emitterCount;
	int   _maxEmitterCount;

	const int _enemyDefaultCount = 10;
	const int _enemyBlueCount    = 50;
	const int _enemyBossCount    = 2;
	const int _enemyGreenCount   = 50;
	const int _enemyPurpleCount  = 50;
	const int _enemyRedCount     = 50;

	
};

#endif