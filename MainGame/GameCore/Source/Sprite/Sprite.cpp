//////////////////////////////////////////////////////////////////////////////////
///             @file    Sprite.cpp
///             @brief   Sprite struct
///             @author  Toide Yutaro
///             @date    2020_1_22
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Sprite/Sprite.hpp"
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include <DirectXColors.h>

//////////////////////////////////////////////////////////////////////////////////
//                            Implement
//////////////////////////////////////////////////////////////////////////////////
const UINT16 Sprite::Indices[] = { 0,1,2,2,1,3 };

#pragma region Public Function
/****************************************************************************
*                       CreateSpriteForTexture
*************************************************************************//**
*  @fn        void Sprite::CreateSpriteForTexture(const std::wstring& imagePath, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& uv)
*  @brief     Create Sprite 
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT2& rectSize, ([0] width, [1] height)
*  @param[in] const DirectX::XMFLOAT4& uv        (u[0], v[0], u[1], v[1])
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSpriteForTexture(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& uv)
{
	_spriteType = SpriteType::TEXTURE;
	CreateRect(position, rectSize, uv);
}

/****************************************************************************
*                       CreateSpriteForTexture
*************************************************************************//**
*  @fn        void Sprite::CreateSpriteForTexture(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
*  @brief     Create Sprite
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT2& rectSize, ([0] width, [1] height)
*  @param[in] const DirectX::XMFLOAT2& u        (u[0], u[1])
*  @param[in] const DirectX::XMFLOAT2& v        (v[0], v[1])
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSpriteForTexture(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
{
	_spriteType = SpriteType::TEXTURE;
	CreateRect(position, rectSize, u, v);
}

/****************************************************************************
*                       CreateSpriteForColor
*************************************************************************//**
*  @fn        void Sprite::CreateSpriteForColor(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color)
*  @brief     Create Sprite
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT2& rectSize, ([0] width, [1] height)
*  @param[in] const DirectX::XMFLOAT4& color     (R,G,B,Alpha)
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSpriteForColor(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color)
{
	_spriteType = SpriteType::COLOR;
	CreateRect(position, rectSize, color);
}

/****************************************************************************
*                       CreateSprite
*************************************************************************//**
*  @fn        void Sprite::CreateSprite(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& uv)
*  @brief     Create Sprite
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT2& rectSize, ([0] width, [1] height)
*  @param[in] const DirectX::XMFLOAT4& color     (R, G, B, Alpha)
*  @param[in] const DirectX::XMFLOAT4& uv        (u[0], v[0], u[1], v[1])
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSprite(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& uv)
{
	_spriteType = SpriteType::TEXTURE;
	CreateRect(position, rectSize, color, uv);
}

/****************************************************************************
*                       CreateSprite
*************************************************************************//**
*  @fn        void Sprite::CreateSprite(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
*  @brief     Create Sprite
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT2& rectSize, ([0] width, [1] height)
*  @param[in] const DirectX::XMFLOAT4& color    (R,G,B, Alpha)
*  @param[in] const DirectX::XMFLOAT2& u        (u[0], u[1])
*  @param[in] const DirectX::XMFLOAT2& v        (v[0], v[1])
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSprite(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
{
	_spriteType = SpriteType::TEXTURE;
	CreateRect(position, rectSize, color, u, v);
}
#pragma endregion Public Function

/****************************************************************************
*                       UpdateSprite
*************************************************************************//**
*  @fn        bool Sprite::UpdateSprite(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& uv)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT4& color    (R,G,B, Alpha)
*  @param[in] const DirectX::XMFLOAT4& uv        (u[0], v[0], u[1], v[1])
*  @return 　　void
*****************************************************************************/
bool Sprite::UpdateSprite(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& uv)
{
	if (_spriteType != SpriteType::TEXTURE)
	{
		MessageBox(NULL, L"Sprite type is not TEXTURE.", L"Warning", MB_ICONWARNING);
		return false;
	}

	CreateRect(position, _size, color, uv);
	return true;
}

/****************************************************************************
*                       UpdateSprite
*************************************************************************//**
*  @fn        bool Sprite::UpdateSprite(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT4& color    (R,G,B, Alpha)
*  @param[in] const DirectX::XMFLOAT2& u        (u[0], u[1])
*  @param[in] const DirectX::XMFLOAT2& v        (v[0], v[1])
*  @return 　　void
*****************************************************************************/
bool Sprite::UpdateSprite(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
{
	if (_spriteType != SpriteType::TEXTURE)
	{
		MessageBox(NULL, L"Sprite type is not TEXTURE.", L"Warning", MB_ICONWARNING);
		return false;
	}

	CreateRect(position, _size, color, u, v);
	return true;
}

/****************************************************************************
*                       UpdateSpriteForColor
*************************************************************************//**
*  @fn        bool Sprite::UpdateSpriteForColor(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT4& color    (R,G,B, Alpha)
*  @return 　　bool
*****************************************************************************/
bool Sprite::UpdateSpriteForColor(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color)
{
	if (_spriteType != SpriteType::COLOR)
	{
		MessageBox(NULL, L"Sprite type is not COLOR.", L"Warning", MB_ICONWARNING);
		return false;
	}

	CreateRect(position, _size, color);
	return true;
}

/****************************************************************************
*                       UpdateSpriteForTexture
*************************************************************************//**
*  @fn        bool Sprite::UpdateSpriteForTexture(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& uv, float alpha = 1.0f)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT2& u        (u[0], u[1])
*  @param[in] const DirectX::XMFLOAT2& v        (v[0], v[1])
*  @param[in] float alpha = 1.0f
*  @return 　　bool
******************s***********************************************************/
bool Sprite::UpdateSpriteForTexture(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& uv, float alpha)
{
	if (_spriteType != SpriteType::TEXTURE)
	{
		MessageBox(NULL, L"Sprite type is not TEXTURE.", L"Warning", MB_ICONWARNING);
		return false;
	}

	CreateRect(position, _size, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha), uv);
	return true;
}

/****************************************************************************
*                       UpdateSpriteForTexture
*************************************************************************//**
*  @fn        bool Sprite::UpdateSpriteForTexture(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v, float alpha = 1.0f)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const DirectX::XMFLOAT3& position, World rect center postion (x,y,z)
*  @param[in] const DirectX::XMFLOAT2& u        (u[0], u[1])
*  @param[in] const DirectX::XMFLOAT2& v        (v[0], v[1])
*  @param[in] float alpha = 1.0f
*  @return 　　bool
*****************************************************************************/
bool Sprite::UpdateSpriteForTexture(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v, float alpha)
{
	if (_spriteType != SpriteType::TEXTURE)
	{
		MessageBox(NULL, L"Sprite type is not TEXTURE.", L"Warning", MB_ICONWARNING);
		return false;
	}

	CreateRect(position, _size, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha), u, v);
	return true;
}
#pragma region Private Function
/****************************************************************************
*                       CreateRect
*************************************************************************//**
*  @fn        void Sprite::CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& uv)
*  @brief     Create Rectangle
*  @param[in] const DirectX::XMFLOAT3& position (rect center position)
*  @param[in] const DirectX::XMFLOAT2& rectSize
*  @param[in] const DirectX::XMFLOAT4& color
*  @param[in] const DirectX::XMFLOAT4& uv
*  @return 　　void
*****************************************************************************/
void Sprite::CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& uv)
{
	using namespace DirectX;
	using Vertex = VertexPositionNormalColorTexture;

	/*-------------------------------------------------------------------
	-              Create RectPosition
	-------------------------t-------------------------------------------*/
	_size    = rectSize;
	float w2 = rectSize.x / 2;
	float h2 = rectSize.y / 2;
	std::array<XMFLOAT3, 4> positions;
	positions[0] = XMFLOAT3(position.x - w2, position.y - h2, position.z);
	positions[1] = XMFLOAT3(position.x - w2, position.y + h2, position.z);
	positions[2] = XMFLOAT3(position.x + w2, position.y + h2, position.z);
	positions[3] = XMFLOAT3(position.x + w2, position.y - h2, position.z);

	/*-------------------------------------------------------------------
	-              Create Normal Vector
	---------------------------------------------------------------------*/
	XMFLOAT3 faceEdge_0, faceEdge_1, faceNormal;
	XMStoreFloat3(&faceEdge_0, XMLoadFloat3(&positions[3]) - XMLoadFloat3(&positions[0]));
	XMStoreFloat3(&faceEdge_1, XMLoadFloat3(&positions[1]) - XMLoadFloat3(&positions[0]));
	XMStoreFloat3(&faceNormal, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&faceEdge_0), XMLoadFloat3(&faceEdge_1))));

	/*-------------------------------------------------------------------
	-              Create UV Vector
	---------------------------------------------------------------------*/
	XMFLOAT2 uvs[] = { XMFLOAT2(uv.x, uv.w), XMFLOAT2(uv.x, uv.y), XMFLOAT2(uv.z, uv.y), XMFLOAT2(uv.z,uv.w) };

	/*-------------------------------------------------------------------
	-              Set Verteices
	---------------------------------------------------------------------*/
	for (int i = 0; i < 4; ++i)
	{
		Vertices[i] = Vertex(positions[i], faceNormal, color, uvs[i]);
	}
}

/****************************************************************************
*                       CreateRect
*************************************************************************//**
*  @fn        void Sprite::CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
*  @brief     Create Rectangle
*  @param[in] const DirectX::XMFLOAT3& position (rect center position)
*  @param[in] const DirectX::XMFLOAT2& rectSize
*  @param[in] const DirectX::XMFLOAT2& u
*  @param[in] const DirectX::XMFLOAT2& v
*  @return 　　void
*****************************************************************************/
void Sprite::CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
{
	using namespace DirectX;
	using Vertex = VertexPositionNormalColorTexture;

	/*-------------------------------------------------------------------
	-              Create RectPosition
	-------------------------t-------------------------------------------*/
	_size    = rectSize;
	float w2 = rectSize.x / 2;
	float h2 = rectSize.y / 2;
	std::array<XMFLOAT3, 4> positions;
	positions[0] = XMFLOAT3(position.x - w2, position.y - h2, position.z);
	positions[1] = XMFLOAT3(position.x - w2, position.y + h2, position.z);
	positions[2] = XMFLOAT3(position.x + w2, position.y + h2, position.z);
	positions[3] = XMFLOAT3(position.x + w2, position.y - h2, position.z);

	/*-------------------------------------------------------------------
	-              Create Normal Vector
	---------------------------------------------------------------------*/
	XMFLOAT3 faceEdge_0, faceEdge_1, faceNormal;
	XMStoreFloat3(&faceEdge_0, XMLoadFloat3(&positions[3]) - XMLoadFloat3(&positions[0]));
	XMStoreFloat3(&faceEdge_1, XMLoadFloat3(&positions[1]) - XMLoadFloat3(&positions[0]));
	XMStoreFloat3(&faceNormal, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&faceEdge_0), XMLoadFloat3(&faceEdge_1))));

	/*-------------------------------------------------------------------
	-              Create Color Vector
	---------------------------------------------------------------------*/
	XMFLOAT4 color;
	XMStoreFloat4(&color, Colors::White);

	/*-------------------------------------------------------------------
	-              Create UV Vector
	---------------------------------------------------------------------*/
	XMFLOAT2 uvs[] = { XMFLOAT2(u.x, v.y), XMFLOAT2(u.x, v.x), XMFLOAT2(u.y, v.x), XMFLOAT2(u.y,v.y) };

	/*-------------------------------------------------------------------
	-              Set Verteices
	---------------------------------------------------------------------*/
	for (int i = 0; i < 4; ++i)
	{
		Vertices[i] = Vertex(positions[i], faceNormal, color, uvs[i]);
	}
}

/****************************************************************************
*                       CreateRect
*************************************************************************//**
*  @fn        void Sprite::CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
*  @brief     Create Rectangle
*  @param[in] const DirectX::XMFLOAT3& position (rect center position)
*  @param[in] const DirectX::XMFLOAT2& rectSize
*  @param[in] const DirectX::XMFLOAT4& color
*  @param[in] const DirectX::XMFLOAT2& u
*  @param[in] const DirectX::XMFLOAT2& v
*  @return 　　void
*****************************************************************************/
void Sprite::CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v)
{
	using namespace DirectX;
	using Vertex = VertexPositionNormalColorTexture;

	/*-------------------------------------------------------------------
	-              Create RectPosition
	-------------------------t-------------------------------------------*/
	_size    = rectSize;
	float w2 = rectSize.x / 2;
	float h2 = rectSize.y / 2;
	std::array<XMFLOAT3, 4> positions;
	positions[0] = XMFLOAT3(position.x - w2, position.y - h2, position.z);
	positions[1] = XMFLOAT3(position.x - w2, position.y + h2, position.z);
	positions[2] = XMFLOAT3(position.x + w2, position.y + h2, position.z);
	positions[3] = XMFLOAT3(position.x + w2, position.y - h2, position.z);

	/*-------------------------------------------------------------------
	-              Create Normal Vector
	---------------------------------------------------------------------*/
	XMFLOAT3 faceEdge_0, faceEdge_1, faceNormal;
	XMStoreFloat3(&faceEdge_0, XMLoadFloat3(&positions[3]) - XMLoadFloat3(&positions[0]));
	XMStoreFloat3(&faceEdge_1, XMLoadFloat3(&positions[1]) - XMLoadFloat3(&positions[0]));
	XMStoreFloat3(&faceNormal, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&faceEdge_0), XMLoadFloat3(&faceEdge_1))));

	/*-------------------------------------------------------------------
	-              Create UV Vector
	---------------------------------------------------------------------*/
	XMFLOAT2 uvs[] = { XMFLOAT2(u.x, v.y), XMFLOAT2(u.x, v.x), XMFLOAT2(u.y, v.x), XMFLOAT2(u.y,v.y) };

	/*-------------------------------------------------------------------
	-              Set Verteices
	---------------------------------------------------------------------*/
	for (int i = 0; i < 4; ++i)
	{
		Vertices[i] = Vertex(positions[i], faceNormal, color, uvs[i]);
	}
}

/****************************************************************************
*                       CreateRect
*************************************************************************//**
*  @fn        void Sprite::CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color)
*  @brief     Create Rectangle
*  @param[in] const DirectX::XMFLOAT3& position (rect center position)
*  @param[in] const DirectX::XMFLOAT2& rectSize ([0]: width, [1] height)
*  @param[in] const DirectX::XMFLOAT4& color    ( R, G, B, Alpha)
*  @return 　　void
*****************************************************************************/
void Sprite::CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color)
{
	using namespace DirectX;
	using Vertex = VertexPositionNormalColorTexture;

	/*-------------------------------------------------------------------
	-              Create RectPosition
	-------------------------t-------------------------------------------*/
	_size    = rectSize;
	float w2 = rectSize.x / 2;
	float h2 = rectSize.y / 2;
	std::array<XMFLOAT3, 4> positions;
	positions[0] = XMFLOAT3(position.x - w2, position.y - h2, position.z);
	positions[1] = XMFLOAT3(position.x - w2, position.y + h2, position.z);
	positions[2] = XMFLOAT3(position.x + w2, position.y + h2, position.z);
	positions[3] = XMFLOAT3(position.x + w2, position.y - h2, position.z);

	/*-------------------------------------------------------------------
	-              Create Normal Vector
	---------------------------------------------------------------------*/
	XMFLOAT3 faceEdge_0, faceEdge_1, faceNormal;
	XMStoreFloat3(&faceEdge_0, XMLoadFloat3(&positions[3]) - XMLoadFloat3(&positions[0]));
	XMStoreFloat3(&faceEdge_1, XMLoadFloat3(&positions[1]) - XMLoadFloat3(&positions[0]));
	XMStoreFloat3(&faceNormal, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&faceEdge_0), XMLoadFloat3(&faceEdge_1))));

	/*-------------------------------------------------------------------
	-              Create UV Vector
	---------------------------------------------------------------------*/
	XMFLOAT2 uvs[] = { XMFLOAT2(0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT2(1.0f,1.0f) };

	/*-------------------------------------------------------------------
	-              Set Verteices
	---------------------------------------------------------------------*/
	for (int i = 0; i < 4; ++i)
	{
		Vertices[i] = Vertex(positions[i], faceNormal, color, uvs[i]);
	}
}


#pragma region Property

void Sprite::SetWidth(float width)
{
	if (width < 0.0f) { width = 0.0f; }
	_size.x = width;
}

void Sprite::SetHeight(float height)
{
	if (height < 0.0f) { height = 0.0f; }
	_size.y = height;
}

void Sprite::SetSize(const DirectX::XMFLOAT2& rectSize)
{
	_size = rectSize;
}

void Sprite::SetSize(float width, float height)
{
	_size.x = width;
	_size.y = height;
}


void Sprite::SetAlpha(float alpha)
{
	if (alpha > 1.0f) { alpha = 1.0f; }
	if (alpha < 0.0f) { alpha = 0.0f; }

	for (int i = 0; i < Vertices.size(); ++i)
	{
		Vertices[i].Color.w = alpha;
	}
}

SpriteType Sprite::GetSpriteType() const
{
	return _spriteType;
}
#pragma endregion Property
#pragma endregion Private Function