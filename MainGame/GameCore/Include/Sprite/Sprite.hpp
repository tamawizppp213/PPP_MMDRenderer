//////////////////////////////////////////////////////////////////////////////////
///             @file   Sprite.hpp
///             @brief  Sprite
///             @author Toide Yutaro
///             @date   2021_01_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SPLITE_HPP
#define SPLITE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "GameCore/Include/Sprite/SpritePipeLineState.hpp"
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include <array>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*							 SpriteMesh
*************************************************************************//**
*  @struct    MeshData
*  @brief     MeshData
*****************************************************************************/
struct Sprite //: public MeshBuffer
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void CreateSpriteForTexture(
		const gm::Float3& position,
		const gm::Float2& rectSize,
		const gm::Float4& uv);
	void CreateSpriteForTexture(
		const gm::Float3& position,
		const gm::Float2& rectSize,
		const gm::Float2& u,
		const gm::Float2& v);
	void CreateSpriteForColor(
		const gm::Float3& position,
		const gm::Float2& rectSize,
		const gm::Float4& color);
	void CreateSprite(
		const gm::Float3& position,
		const gm::Float2& rectSize,
		const gm::Float4& color,
		const gm::Float4& uv);
	void CreateSprite(
		const gm::Float3& position,
		const gm::Float2& rectSize,
		const gm::Float4& color,
		const gm::Float2& u,
		const gm::Float2& v);
	bool UpdateSprite(
		const gm::Float3& position,
		const gm::Float4& color,
		const gm::Float4& uv);
	bool UpdateSprite(
		const gm::Float3& position,
		const gm::Float4& color,
		const gm::Float2& u,
		const gm::Float2& v);
	bool UpdateSpriteForColor(
		const gm::Float3& position,
		const gm::Float4& color);
	bool UpdateSpriteForTexture(
		const gm::Float3& position,
		const gm::Float4& uv,
		float alpha = 1.0f);
	bool UpdateSpriteForTexture(
		const gm::Float3& position,
		const gm::Float2& u,
		const gm::Float2& v,
		float alpha = 1.0f);
	
	

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	std::array<VertexPositionNormalColorTexture, 4> Vertices;
	static const UINT16 Indices[6];
	
	SpriteType GetSpriteType() const;
	void SetWidth(float width);
	void SetHeight(float height);
	void SetSize(const gm::Float2& rectSize);
	void SetSize(float width, float height);
	void SetAlpha(float alpha);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Sprite()                         = default;
	Sprite(const Sprite&)            = default;
	Sprite& operator=(const Sprite&) = default;
	Sprite(Sprite&&)                 = default;
	Sprite& operator=(Sprite&&)      = default;


private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void LoadTexture(const std::wstring& imagePath);
	void CreateRect(const gm::Float3& position, const gm::Float2& rectSize, const gm::Float4& color, const gm::Float4& uv);
	void CreateRect(const gm::Float3& position, const gm::Float2& rectSize, const gm::Float2& u    , const gm::Float2& v);
	void CreateRect(const gm::Float3& position, const gm::Float2& rectSize, const gm::Float4& color, const gm::Float2& u, const gm::Float2& v);
	void CreateRect(const gm::Float3& position, const gm::Float2& rectSize, const gm::Float4& color);
	
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	SpriteType        _spriteType;
	gm::Float2 _size;
	//float _angle  = 0.0f;
};

#endif