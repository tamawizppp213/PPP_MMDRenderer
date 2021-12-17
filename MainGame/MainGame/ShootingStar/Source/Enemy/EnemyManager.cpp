//////////////////////////////////////////////////////////////////////////////////
///             @file   EnemyEmitter.cpp
///             @brief  Enemy Emitter
///             @author Toide Yutaro
///             @date   2021_12_08
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Enemy/EnemyManager.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyType.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyDefault.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyBlue.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyBoss.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyLightGreen.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyPurple.hpp"
#include "MainGame/ShootingStar/Include/Enemy/EnemyRed.hpp"
#include "MainGame/ShootingStar/Include/Player/Player.hpp"
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct EnemyEmitterConfig
{
	float       Time;
	gm::Vector3 Position;
	EnemyType   Type;
};

static EnemyEmitterConfig g_EnemyEmitterTable[] =
{
	{ 1.5f, {-0.66f, 1.5f, 0.0f}, EnemyType::Default },
	{ 1.5f, {-0.33f, 1.5f, 0.0f}, EnemyType::Blue },
	{ 1.5f, {0.0f  , 1.5f, 0.0f}, EnemyType::Default },
	{ 1.5f, {0.33f , 1.5f, 0.0f}, EnemyType::Blue },
	{ 1.5f, {0.66f , 1.5f, 0.0f}, EnemyType::Default },

	{ 8.0f, {-0.66f, 1.5f, 0.0f}, EnemyType::Blue},
	{ 8.0f, {-0.33f, 1.5f, 0.0f}, EnemyType::Default },
	{ 8.0f, {+0.00f, 1.5f, 0.0f}, EnemyType::Blue },
	{ 8.0f, {+0.33f, 1.5f, 0.0f}, EnemyType::Default },
	{ 8.0f, {+0.66f, 1.5f, 0.0f}, EnemyType::Blue },

	{ 23.0f, {-0.66f, 1.5f, 0.0f}, EnemyType::LightGreen },
	{ 23.0f, {0.66f , 1.5f, 0.0f}, EnemyType::LightGreen },
	{ 23.0f, {0.00f , 1.5f, 0.0f}, EnemyType::Blue },
	{ 27.0f, {0.33f, 1.5f, 0.0f}, EnemyType::Purple },
	{ 27.0f, {-0.33f, 1.5f, 0.0f}, EnemyType::Blue },

	{ 40.0f, {-0.30f, 1.5f, 0.0f},  EnemyType::Blue },
	{ 40.0f, {0.33f, 1.5f, 0.0f},  EnemyType::Blue },
	{ 40.0f, {0.60f, 1.5f, 0.0f},  EnemyType::Blue },
	{ 42.5f, {0.80f, 1.5f, 0.0f}, EnemyType::Purple },
	{ 42.5f, {-0.80f, 1.5f, 0.0f}, EnemyType::Red },
	{ 53.0f, {-0.50f, 1.5f, 0.0f}, EnemyType::Default },
	{ 54.0f, {0.00f, 1.5f, 0.0f}, EnemyType::Default },
	{ 55.0f, {0.40f, 1.5f, 0.0f}, EnemyType::Default },


	{ 63.0f, {-0.33f, 1.5f, 0.0f},  EnemyType::Blue },
	{ 63.0f, {0.00f, 1.5f, 0.0f},  EnemyType::LightGreen },
	{ 63.0f, {0.33f, 1.5f, 0.0f},  EnemyType::Blue },
	{ 64.0f, {-0.66f, 1.5f, 0.0f}, EnemyType::Red },

	{ 77.0f, {-0.70f, 1.5f, 0.0f}, EnemyType::Default },
	{ 78.0f, {-0.35f, 1.5f, 0.0f}, EnemyType::Default },
	{ 79.0f, {0.75f, 1.5f, 0.0f}, EnemyType::Default },
	{ 85.0f, {-0.66f, 1.5f, 0.0f}, EnemyType::LightGreen },
	{ 85.0f, {0.00f, 1.5f, 0.0f}, EnemyType::Purple },
	{ 85.0f, {0.66f, 1.5f, 0.0f}, EnemyType::LightGreen },
	{ 85.0f, {0.80f, 1.5f, 0.0f}, EnemyType::Red },

	{ 110.0f, {-0.30f, 1.5f, 0.0f},  EnemyType::Default },
};

std::vector<EnemyDefault>    g_enemies;
std::vector<EnemyBlue>       g_blueEnemies;
std::vector<EnemyBoss>       g_bossEnemies;
std::vector<EnemyLightGreen> g_greenEnemies;
std::vector<EnemyPurple>     g_purpleEnemies;
std::vector<EnemyRed>        g_redEnemies;
bool g_enableGenerate = true;

namespace enemy
{
	static void GenerateEnemyDefault(const Vector3& position);
	static void GenerateEnemyBlue(const Vector3& position);
	static void GenerateEnemyLightGreen(const Vector3& position);
	static void GenerateEnemyPurple(const Vector3& position);
	static void GenerateEnemyRed(const Vector3& position);
	static void GenerateEnemyBoss(const Vector3& position);
}

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
void EnemyManager::Initialize()
{
	_localTimer   = 0.0f;
	_emitterCount = 0;
	_maxEmitterCount = _countof(g_EnemyEmitterTable);
	g_enemies      .resize(_enemyDefaultCount);
	g_blueEnemies  .resize(_enemyBlueCount);
	g_bossEnemies  .resize(_enemyBossCount);
	g_greenEnemies .resize(_enemyGreenCount);
	g_purpleEnemies.resize(_enemyPurpleCount);
	g_redEnemies   .resize(_enemyRedCount);

	for (auto& enemy : g_enemies)
	{
		enemy.Initialize();
	}
	for (auto& enemy : g_blueEnemies)
	{
		enemy.Initialize();
	}
	for (auto& enemy : g_bossEnemies)
	{
		enemy.Initialize();
	}
	for (auto& enemy : g_greenEnemies)
	{
		enemy.Initialize();
	}
	for (auto& enemy : g_purpleEnemies)
	{
		enemy.Initialize();
	}
	for (auto& enemy : g_redEnemies)
	{
		enemy.Initialize();
	}

}

void EnemyManager::Update(GameTimer& gameTimer, const Player& player)
{
	if (_emitterCount < _countof(g_EnemyEmitterTable)) 
	{ 
		// “G‚Ì¶¬
		if (g_EnemyEmitterTable[_emitterCount].Time <= _localTimer)
		{
			GenerateEnemy(g_EnemyEmitterTable[_emitterCount].Type, g_EnemyEmitterTable[_emitterCount].Position);
		}
	}
	if (_maxEmitterCount == _emitterCount)
	{
		GenerateEnemy(EnemyType::Boss, Vector3(0, 1.5f, 0));
	}

	

	// “G‚ÌXV
	for (auto& enemy : g_enemies)
	{
		enemy.Update(gameTimer, player);
	}
	for (auto& enemy : g_blueEnemies)
	{
		enemy.Update(gameTimer, player);
	}
	for (auto& enemy : g_greenEnemies)
	{
		enemy.Update(gameTimer, player);
	}
	for (auto& enemy : g_purpleEnemies)
	{
		enemy.Update(gameTimer, player);
	}
	for (auto& enemy : g_redEnemies)
	{
		enemy.Update(gameTimer, player);
	}
	for (auto& enemy : g_bossEnemies)
	{
		enemy.Update(gameTimer, player);
	}
	_localTimer += gameTimer.DeltaTime();
}

void EnemyManager::Terminal()
{
	g_enemies      .clear();
	g_blueEnemies  .clear();
	g_bossEnemies  .clear();
	g_greenEnemies .clear();
	g_purpleEnemies.clear();
	g_redEnemies   .clear();
}

void EnemyManager::GenerateEnemy(EnemyType type, const gm::Vector3& position)
{
	switch (type)
	{
		case EnemyType::Default:
		{
			enemy::GenerateEnemyDefault(position);
			break;
		}
		case EnemyType::Blue:
		{
			enemy::GenerateEnemyBlue(position);
			break;
		}
		case EnemyType::LightGreen:
		{
			enemy::GenerateEnemyLightGreen(position);
			break;
		}
		case EnemyType::Purple:
		{
			enemy::GenerateEnemyPurple(position);
			break;
		}
		case EnemyType::Red:
		{
			enemy::GenerateEnemyRed(position);
			break;
		}
		case EnemyType::Boss:
		{
			enemy::GenerateEnemyBoss(position);
			break;
		}
	}
	_emitterCount++;
}

Enemy* EnemyManager::GetEnemy(EnemyType type, int index)
{
	switch (type)
	{
		case EnemyType::Default:
		{
			if (index >= GetEnemyDefaultCount()) { return nullptr; }
			return &g_enemies[index];
		}
		case EnemyType::Blue:
		{
			if (index >= GetEnemyBlueCount()) { return nullptr; }
			return &g_blueEnemies[index];
		}
		case EnemyType::LightGreen:
		{
			if (index >= GetEnemyGreenCount()) { return nullptr; }
			return &g_greenEnemies[index];
		}
		case EnemyType::Purple:
		{
			if (index >= GetEnemyPurpleCount()) { return nullptr; }
			return &g_purpleEnemies[index];
		}
		case EnemyType::Red:
		{
			if (index >= GetEnemyRedCount()) { return nullptr; }
			return &g_redEnemies[index];
		}
		case EnemyType::Boss:
		{
			if (index >= GetEnemyBossCount()) { return nullptr; }
			return &g_bossEnemies[index];
		}
	}
	_emitterCount++;
	return nullptr;
}

void EnemyManager::GetDrawMaterial(EnemyType type, std::vector<Sprite>& sprite, Texture& texture)
{
	sprite.clear();
	switch (type)
	{
		case EnemyType::Default:
		{
			for (auto& enemy : g_enemies)
			{
				if (enemy.IsActive())
				{
					sprite.push_back(enemy.GetSprite());
					texture = enemy.GetTexture();
				}
			}
			break;
		}
		case EnemyType::Blue:
		{
			for (auto& enemy : g_blueEnemies)
			{
				if (enemy.IsActive())
				{
					sprite.push_back(enemy.GetSprite());
					texture = enemy.GetTexture();
				}
			}
			break;
		}
		case EnemyType::LightGreen:
		{
			for (auto& enemy : g_greenEnemies)
			{
				if (enemy.IsActive())
				{
					sprite.push_back(enemy.GetSprite());
					texture = enemy.GetTexture();
				}
			}
			break;
		}
		case EnemyType::Purple:
		{
			for (auto& enemy : g_purpleEnemies)
			{
				if (enemy.IsActive())
				{
					sprite.push_back(enemy.GetSprite());
					texture = enemy.GetTexture();
				}
			}
			break;
		}
		case EnemyType::Red:
		{
			for (auto& enemy : g_redEnemies)
			{
				if (enemy.IsActive())
				{
					sprite.push_back(enemy.GetSprite());
					texture = enemy.GetTexture();
				}
			}
			break;
		}
		case EnemyType::Boss:
		{
			for (auto& enemy : g_bossEnemies)
			{
				if (enemy.IsActive())
				{
					sprite.push_back(enemy.GetSprite());
					texture = enemy.GetTexture();
				}
			}
			break;
		}
	}
	
}

std::vector<Enemy*> EnemyManager::GetActiveEnemies()
{
	std::vector<Enemy*> enemies;
	for (auto& enemy : g_enemies)
	{
		if (enemy.IsActive())
		{
			enemies.push_back(&enemy);
		}
	}

	for (auto& enemy : g_blueEnemies)
	{
		if (enemy.IsActive())
		{
			enemies.push_back(&enemy);
		}
	}

	for (auto& enemy : g_greenEnemies)
	{
		if (enemy.IsActive())
		{
			enemies.push_back(&enemy);
		}
	}

	for (auto& enemy : g_purpleEnemies)
	{
		if (enemy.IsActive())
		{
			enemies.push_back(&enemy);
		}
	}

	for (auto& enemy : g_redEnemies)
	{
		if (enemy.IsActive())
		{
			enemies.push_back(&enemy);
		}
	}
	for (auto& enemy : g_bossEnemies)
	{
		if (enemy.IsActive())
		{
			enemies.push_back(&enemy);
		}
	}
	return enemies;
}

void enemy::GenerateEnemyDefault(const Vector3& position)
{
	for (auto& enemy : g_enemies)
	{
		if (enemy.Generate(position)) { break; }
	}
}

void enemy::GenerateEnemyBlue(const Vector3& position)
{
	for (auto& enemy : g_blueEnemies)
	{
		if (enemy.Generate(position)) { break; }
	}
}

void enemy::GenerateEnemyLightGreen(const Vector3& position)
{
	for (auto& enemy : g_greenEnemies)
	{
		if (enemy.Generate(position)) { break; }
	}
}

void enemy::GenerateEnemyPurple(const Vector3& position)
{
	for (auto& enemy : g_purpleEnemies)
	{
		if (enemy.Generate(position)) { break; }
	}
}

void enemy::GenerateEnemyRed(const Vector3& position)
{
	for (auto& enemy : g_redEnemies)
	{
		if (enemy.Generate(position)) { break; }
	}
}
void enemy::GenerateEnemyBoss(const Vector3& position)
{
	for (auto& enemy : g_bossEnemies)
	{
		if (enemy.Generate(position)) { break; }
	}
}