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
#include "GameMath/Include/GMColor.hpp"

using namespace gm;
//////////////////////////////////////////////////////////////////////////////////
//                            Implement
//////////////////////////////////////////////////////////////////////////////////
const UINT16 Sprite::Indices[] = { 0,1,2,2,1,3 };

#pragma region Public Function
/****************************************************************************
*                       CreateSpriteForTexture
*************************************************************************//**
*  @fn        void Sprite::CreateSpriteForTexture(const std::wstring& imagePath, const Float3& position, const Float2& rectSize, const Float4& uv)
*  @brief     Create Sprite 
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float2& rectSize, ([0] width, [1] height)
*  @param[in] const Float4& uv        (u[0], v[0], u[1], v[1])
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSpriteForTexture(const Float3& position, const Float2& rectSize, const Float4& uv, float radian)
{
	_spriteType = SpriteType::TEXTURE;
	CreateRect(position, rectSize, uv, radian);
}

/****************************************************************************
*                       CreateSpriteForTexture
*************************************************************************//**
*  @fn        void Sprite::CreateSpriteForTexture(const Float3& position, const Float2& rectSize, const Float2& u, const Float2& v)
*  @brief     Create Sprite
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float2& rectSize, ([0] width, [1] height)
*  @param[in] const Float2& u        (u[0], u[1])
*  @param[in] const Float2& v        (v[0], v[1])
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSpriteForTexture(const Float3& position, const Float2& rectSize, const Float2& u, const Float2& v, float radian)
{
	_spriteType = SpriteType::TEXTURE;
	CreateRect(position, rectSize, u, v, radian);
}

/****************************************************************************
*                       CreateSpriteForColor
*************************************************************************//**
*  @fn        void Sprite::CreateSpriteForColor(const Float3& position, const Float2& rectSize, const Float4& color)
*  @brief     Create Sprite
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float2& rectSize, ([0] width, [1] height)
*  @param[in] const Float4& color     (R,G,B,Alpha)
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSpriteForColor(const Float3& position, const Float2& rectSize, const Float4& color, float radian)
{
	_spriteType = SpriteType::COLOR;
	CreateRect(position, rectSize, color, radian);
}

/****************************************************************************
*                       CreateSprite
*************************************************************************//**
*  @fn        void Sprite::CreateSprite(const Float3& position, const Float2& rectSize, const Float4& color, const Float4& uv)
*  @brief     Create Sprite
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float2& rectSize, ([0] width, [1] height)
*  @param[in] const Float4& color     (R, G, B, Alpha)
*  @param[in] const Float4& uv        (u[0], v[0], u[1], v[1])
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSprite(const Float3& position, const Float2& rectSize, const Float4& color, const Float4& uv, float radian)
{
	_spriteType = SpriteType::TEXTURE;
	CreateRect(position, rectSize, color, uv, radian);
}

/****************************************************************************
*                       CreateSprite
*************************************************************************//**
*  @fn        void Sprite::CreateSprite(const Float3& position, const Float2& rectSize, const Float4& color, const Float2& u, const Float2& v)
*  @brief     Create Sprite
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float2& rectSize, ([0] width, [1] height)
*  @param[in] const Float4& color    (R,G,B, Alpha)
*  @param[in] const Float2& u        (u[0], u[1])
*  @param[in] const Float2& v        (v[0], v[1])
*  @return 　　void
*****************************************************************************/
void Sprite::CreateSprite(const Float3& position, const Float2& rectSize, const Float4& color, const Float2& u, const Float2& v, float radian)
{
	_spriteType = SpriteType::TEXTURE;
	CreateRect(position, rectSize, color, u, v, radian);
}
#pragma endregion Public Function

/****************************************************************************
*                       UpdateSprite
*************************************************************************//**
*  @fn        bool Sprite::UpdateSprite(const Float3& position, const Float4& color, const Float4& uv)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float4& color    (R,G,B, Alpha)
*  @param[in] const Float4& uv        (u[0], v[0], u[1], v[1])
*  @return 　　void
*****************************************************************************/
bool Sprite::UpdateSprite(const Float3& position, const Float4& color, const Float4& uv, float radian)
{
	if (_spriteType != SpriteType::TEXTURE)
	{
		MessageBox(NULL, L"Sprite type is not TEXTURE.", L"Warning", MB_ICONWARNING);
		return false;
	}

	CreateRect(position, _size, color, uv, radian);
	return true;
}

/****************************************************************************
*                       UpdateSprite
*************************************************************************//**
*  @fn        bool Sprite::UpdateSprite(const Float& position, const Float4& color, const Float2& u, const Float2& v)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float4& color    (R,G,B, Alpha)
*  @param[in] const Float2& u        (u[0], u[1])
*  @param[in] const Float2& v        (v[0], v[1])
*  @return 　　void
*****************************************************************************/
bool Sprite::UpdateSprite(const Float3& position, const Float4& color, const Float2& u, const Float2& v, float radian)
{
	if (_spriteType != SpriteType::TEXTURE)
	{
		MessageBox(NULL, L"Sprite type is not TEXTURE.", L"Warning", MB_ICONWARNING);
		return false;
	}

	CreateRect(position, _size, color, u, v, radian);
	return true;
}

/****************************************************************************
*                       UpdateSpriteForColor
*************************************************************************//**
*  @fn        bool Sprite::UpdateSpriteForColor(const Float3& position, const Float4& color)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float4& color    (R,G,B, Alpha)
*  @return 　　bool
*****************************************************************************/
bool Sprite::UpdateSpriteForColor(const Float3& position, const Float4& color, float radian)
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
*  @fn        bool Sprite::UpdateSpriteForTexture(const Float3& position, const Float4& uv, float alpha = 1.0f)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float2& u        (u[0], u[1])
*  @param[in] const Float2& v        (v[0], v[1])
*  @param[in] float alpha = 1.0f
*  @return 　　bool
******************s***********************************************************/
bool Sprite::UpdateSpriteForTexture(const Float3& position, const Float4& uv, float alpha, float radian)
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
*  @fn        bool Sprite::UpdateSpriteForTexture(const Float3& position, const Float2& u, const Float2& v, float alpha = 1.0f)
*  @brief     Update Sprite Config. It needs to be called before Draw.
*  @param[in] const Float3& position, World rect center postion (x,y,z)
*  @param[in] const Float2& u        (u[0], u[1])
*  @param[in] const Float2& v        (v[0], v[1])
*  @param[in] float alpha = 1.0f
*  @return 　　bool
*****************************************************************************/
bool Sprite::UpdateSpriteForTexture(const Float3& position, const Float2& u, const Float2& v, float alpha, float radian)
{
	if (_spriteType != SpriteType::TEXTURE)
	{
		MessageBox(NULL, L"Sprite type is not TEXTURE.", L"Warning", MB_ICONWARNING);
		return false;
	}

	CreateRect(position, _size, Float4(1.0f, 1.0f, 1.0f, alpha), u, v, radian);
	return true;
}
#pragma region Private Function
/****************************************************************************
*                       CreateRect
*************************************************************************//**
*  @fn        void Sprite::CreateRect(const Float3& position, const Float2& rectSize, const Float4& color, const Float4& uv)
*  @brief     Create Rectangle
*  @param[in] const Float3& position (rect center position)
*  @param[in] const Float2& rectSize
*  @param[in] const Float4& color
*  @param[in] const Float4& uv
*  @return 　　void
*****************************************************************************/
void Sprite::CreateRect(const Float3& position, const Float2& rectSize, const Float4& color, const Float4& uv, float radian)
{
	using Vertex = VertexPositionNormalColorTexture;

	/*-------------------------------------------------------------------
	-              Get rect info
	-------------------------t-------------------------------------------*/
	_size = rectSize;
	float w2 = rectSize.x / 2;
	float h2 = rectSize.y / 2;

	/*-------------------------------------------------------------------
	-              Get rotation info
	-------------------------t-------------------------------------------*/
	float fSin, fCos;
	DirectX::XMScalarSinCos(&fSin, &fCos, radian);

	/*-------------------------------------------------------------------
	-              Create RectPosition
	-------------------------t-------------------------------------------*/
	std::array<Float3, 4> positions;
	positions[0] = Float3(position.x - w2 * fCos - h2 * fSin, position.y + w2 * fSin - h2 * fCos, position.z);
	positions[1] = Float3(position.x - w2 * fCos + h2 * fSin, position.y + w2 * fSin + h2 * fCos, position.z);
	positions[2] = Float3(position.x + w2 * fCos + h2 * fSin, position.y - w2 * fSin + h2 * fCos, position.z);
	positions[3] = Float3(position.x + w2 * fCos - h2 * fSin, position.y - w2 * fSin - h2 * fCos, position.z);


	/*-------------------------------------------------------------------
	-              Create Normal Vector
	---------------------------------------------------------------------*/
	Float3 faceEdge_0, faceEdge_1, faceNormal;
	faceEdge_0 = Vector3(positions[3] - positions[0]).ToFloat3();
	faceEdge_1 = Vector3(positions[1] - positions[0]).ToFloat3();
	faceNormal = Normalize(Cross(Vector3(faceEdge_0), Vector3(faceEdge_1))).ToFloat3();

	/*-------------------------------------------------------------------
	-              Create UV Vector
	---------------------------------------------------------------------*/
	Float2 uvs[] = { Float2(uv.x, uv.w), Float2(uv.x, uv.y), Float2(uv.z, uv.y), Float2(uv.z,uv.w) };

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
*  @fn        void Sprite::CreateRect(const Float3& position, const Float2& rectSize, const Float2& u, const Float2& v)
*  @brief     Create Rectangle
*  @param[in] const Float3& position (rect center position)
*  @param[in] const Float2& rectSize
*  @param[in] const Float2& u
*  @param[in] const Float2& v
*  @return 　　void
*****************************************************************************/
void Sprite::CreateRect(const Float3& position, const Float2& rectSize, const Float2& u, const Float2& v, float radian)
{
	using Vertex = VertexPositionNormalColorTexture;

	/*-------------------------------------------------------------------
	-              Get rect info
	-------------------------t-------------------------------------------*/
	_size = rectSize;
	float w2 = rectSize.x / 2;
	float h2 = rectSize.y / 2;

	/*-------------------------------------------------------------------
	-              Get rotation info
	-------------------------t-------------------------------------------*/
	float fSin, fCos;
	DirectX::XMScalarSinCos(&fSin, &fCos, radian);

	/*-------------------------------------------------------------------
	-              Create RectPosition
	-------------------------t-------------------------------------------*/
	std::array<Float3, 4> positions;
	positions[0] = Float3(position.x - w2 * fCos - h2 * fSin, position.y + w2 * fSin - h2 * fCos, position.z);
	positions[1] = Float3(position.x - w2 * fCos + h2 * fSin, position.y + w2 * fSin + h2 * fCos, position.z);
	positions[2] = Float3(position.x + w2 * fCos + h2 * fSin, position.y - w2 * fSin + h2 * fCos, position.z);
	positions[3] = Float3(position.x + w2 * fCos - h2 * fSin, position.y - w2 * fSin - h2 * fCos, position.z);

	/*-------------------------------------------------------------------
	-              Create Normal Vector
	---------------------------------------------------------------------*/
	Float3 faceEdge_0, faceEdge_1, faceNormal;
	faceEdge_0 = Vector3(positions[3] - positions[0]).ToFloat3();
	faceEdge_1 = Vector3(positions[1] - positions[0]).ToFloat3();
	faceNormal = Normalize(Cross(Vector3(faceEdge_0), Vector3(faceEdge_1))).ToFloat3();

	/*-------------------------------------------------------------------
	-              Create Color Vector
	---------------------------------------------------------------------*/
	Float4 color;
	color = color::White;

	/*-------------------------------------------------------------------
	-              Create UV Vector
	---------------------------------------------------------------------*/
	Float2 uvs[] = { Float2(u.x, v.y), Float2(u.x, v.x), Float2(u.y, v.x), Float2(u.y,v.y) };

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
*  @fn        void Sprite::CreateRect(const Float3& position, const Float2& rectSize, const Float4& color, const Float2& u, const Float2& v)
*  @brief     Create Rectangle
*  @param[in] const Float3& position (rect center position)
*  @param[in] const Float2& rectSize
*  @param[in] const Float4& color
*  @param[in] const Float2& u
*  @param[in] const Float2& v
*  @return 　　void
*****************************************************************************/
void Sprite::CreateRect(const Float3& position, const Float2& rectSize, const Float4& color, const Float2& u, const Float2& v, float radian)
{
	using Vertex = VertexPositionNormalColorTexture;

	/*-------------------------------------------------------------------
	-              Get rect info
	-------------------------t-------------------------------------------*/
	_size    = rectSize;
	float w2 = rectSize.x / 2;
	float h2 = rectSize.y / 2;

	/*-------------------------------------------------------------------
	-              Get rotation info
	-------------------------t-------------------------------------------*/
	float fSin, fCos;
	DirectX::XMScalarSinCos(&fSin, &fCos, radian);

	/*-------------------------------------------------------------------
	-              Create RectPosition
	-------------------------t-------------------------------------------*/
	std::array<Float3, 4> positions;
	positions[0] = Float3(position.x - w2 * fCos - h2 * fSin, position.y + w2 * fSin - h2 * fCos, position.z);
	positions[1] = Float3(position.x - w2 * fCos + h2 * fSin, position.y + w2 * fSin + h2 * fCos, position.z);
	positions[2] = Float3(position.x + w2 * fCos + h2 * fSin, position.y - w2 * fSin + h2 * fCos, position.z);
	positions[3] = Float3(position.x + w2 * fCos - h2 * fSin, position.y - w2 * fSin - h2 * fCos, position.z);

	/*-------------------------------------------------------------------
	-              Create Normal Vector
	---------------------------------------------------------------------*/
	Float3 faceEdge_0, faceEdge_1, faceNormal;
	faceEdge_0 = Vector3(positions[3] - positions[0]).ToFloat3();
	faceEdge_1 = Vector3(positions[1] - positions[0]).ToFloat3();
	faceNormal = Normalize(Cross(Vector3(faceEdge_0), Vector3(faceEdge_1))).ToFloat3();

	/*-------------------------------------------------------------------
	-              Create UV Vector
	---------------------------------------------------------------------*/
	Float2 uvs[] = { Float2(u.x, v.y), Float2(u.x, v.x), Float2(u.y, v.x), Float2(u.y,v.y) };

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
*  @fn        void Sprite::CreateRect(const Float3& position, const Float2& rectSize, const Float4& color)
*  @brief     Create Rectangle
*  @param[in] const Float3& position (rect center position)
*  @param[in] const Float2& rectSize ([0]: width, [1] height)
*  @param[in] const Float4& color    ( R, G, B, Alpha)
*  @return 　　void
*****************************************************************************/
void Sprite::CreateRect(const Float3& position, const Float2& rectSize, const Float4& color, float radian)
{
	using namespace DirectX;
	using Vertex = VertexPositionNormalColorTexture;

	/*-------------------------------------------------------------------
	-              Get rect info
	-------------------------t-------------------------------------------*/
	_size = rectSize;
	float w2 = rectSize.x / 2;
	float h2 = rectSize.y / 2;

	/*-------------------------------------------------------------------
	-              Get rotation info
	-------------------------t-------------------------------------------*/
	float fSin, fCos;
	DirectX::XMScalarSinCos(&fSin, &fCos, radian);

	/*-------------------------------------------------------------------
	-              Create RectPosition
	-------------------------t-------------------------------------------*/
	std::array<Float3, 4> positions;
	positions[0] = Float3(position.x - w2 * fCos - h2 * fSin, position.y + w2 * fSin - h2 * fCos, position.z);
	positions[1] = Float3(position.x - w2 * fCos + h2 * fSin, position.y + w2 * fSin + h2 * fCos, position.z);
	positions[2] = Float3(position.x + w2 * fCos + h2 * fSin, position.y - w2 * fSin + h2 * fCos, position.z);
	positions[3] = Float3(position.x + w2 * fCos - h2 * fSin, position.y - w2 * fSin - h2 * fCos, position.z);

	/*-------------------------------------------------------------------
	-              Create Normal Vector
	---------------------------------------------------------------------*/
	Float3 faceEdge_0, faceEdge_1, faceNormal;
	faceEdge_0 = Vector3(positions[3] - positions[0]).ToFloat3();
	faceEdge_1 = Vector3(positions[1] - positions[0]).ToFloat3();
	faceNormal = Normalize(Cross(Vector3(faceEdge_0), Vector3(faceEdge_1))).ToFloat3();

	/*-------------------------------------------------------------------
	-              Create UV Vector
	---------------------------------------------------------------------*/
	Float2 uvs[] = { Float2(0.0f, 1.0f), Float2(0.0f, 0.0f), Float2(1.0f, 0.0f), Float2(1.0f,1.0f) };

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

void Sprite::SetSize(const Float2& rectSize)
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