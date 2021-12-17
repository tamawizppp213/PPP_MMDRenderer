//////////////////////////////////////////////////////////////////////////////////
//              @file   PlayerHP.cpp
///             @brief  Heart UI
///             @author Toide Yutaro
///             @date   -
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Player/PlayerHP.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
std::vector<UIPlayerHP*> UIPlayerHP::_hpUI;

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
UIPlayerHP::UIPlayerHP(const Vector3& position, float size)
{
	_actorType = ActorType::Sprite;
	_name      = "PlayerHPUI";
	_isActive  = true;
	_transform.LocalPosition = position;
	_size = size;
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/heart.png", _texture);
	_sprite.CreateSpriteForTexture(_transform.LocalPosition.ToFloat3(), Float2(_size / Screen::GetAspectRatio(), _size), Float2(0, 1), Float2(0, 1));

	_hpUI.push_back(this);
}

void UIPlayerHP::ClearAllPlayerHP()
{
	for (int i = 0; i < _hpUI.size(); ++i)
	{
		Destroy(_hpUI[i]);
	}
	_hpUI.clear();
}

void UIPlayerHP::GenerateUI(const Vector3& startPosition, int hp, float widthSize)
{
	for (int i = 0; i < hp; ++i)
	{
		Vector3 temp = startPosition + Vector3((float)i * widthSize / Screen::GetAspectRatio(), 0, 0);
		UIPlayerHP* hp = new UIPlayerHP(temp, widthSize); // delete‚ÍDestroy‚ª’S“–‚·‚é. (‚¿‚á‚ñ‚ÆŒÄ‚Ño‚·‚±‚Æ.)
	}
}

bool UIPlayerHP::ManageUI(int currentHP)
{
	if (currentHP > _hpUI.size()) { return false; }
	int deleteCount = (int)_hpUI.size() - currentHP;
	for (int i = 0; i < _hpUI.size(); ++i)
	{
		_hpUI[i]->SetActive(true);
	}
	for (int i = 0; i < deleteCount; ++i)
	{
		_hpUI[_hpUI.size() - 1 - i]->SetActive(false);
	}
	return true;
}

