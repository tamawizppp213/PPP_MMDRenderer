#include "MainGame/Include/Scene.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"

Scene::Scene()
{
	FaderPtr fader = std::make_unique<Fader>();
	SpriteRendererPtr spriteRenderer = std::make_unique<SpriteRenderer>();

	_fader          = std::move(fader);
	_spriteRenderer = std::move(spriteRenderer);
	
}

Scene::~Scene()
{

}