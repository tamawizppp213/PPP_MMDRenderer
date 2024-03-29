//////////////////////////////////////////////////////////////////////////////////
//              @file   ShootingStarGame.cpp
///             @brief  Shooting star game.cpp
///             @author Toide Yutaro
///             @date   2021_12_11
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Scene/RenderResource.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
void RenderResource::Clear()
{
	TexturePtr.get()->Resource = nullptr;
	TexturePtr.reset();
	SpritePtr.reset();
}