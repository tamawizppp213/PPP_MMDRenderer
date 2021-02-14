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
#include <vector>
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
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT2& rectSize,
		const DirectX::XMFLOAT4& uv);
	void CreateSpriteForTexture(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT2& rectSize,
		const DirectX::XMFLOAT2& u,
		const DirectX::XMFLOAT2& v);
	void CreateSpriteForColor(const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT2& rectSize,
		const DirectX::XMFLOAT4& color);
	void CreateSprite(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT2& rectSize,
		const DirectX::XMFLOAT4& color,
		const DirectX::XMFLOAT4& uv);
	void CreateSprite(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT2& rectSize,
		const DirectX::XMFLOAT4& color,
		const DirectX::XMFLOAT2& u,
		const DirectX::XMFLOAT2& v);
	bool UpdateSprite(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT4& color,
		const DirectX::XMFLOAT4& uv);
	bool UpdateSprite(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT4& color,
		const DirectX::XMFLOAT2& u,
		const DirectX::XMFLOAT2& v);
	bool UpdateSpriteForColor(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT4& color);
	bool UpdateSpriteForTexture(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT4& uv,
		float alpha = 1.0f);
	bool UpdateSpriteForTexture(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT2& u,
		const DirectX::XMFLOAT2& v,
		float alpha = 1.0f);
	
	

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	std::array<VertexPositionNormalColorTexture, 4> Vertices;
	static const UINT16 Indices[6];
	
	SpriteType GetSpriteType() const;
	void SetWidth(float width);
	void SetHeight(float height);
	void SetSize(const DirectX::XMFLOAT2& rectSize);
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
	void CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& uv);
	void CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v);
	void CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& u, const DirectX::XMFLOAT2& v);
	void CreateRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& rectSize, const DirectX::XMFLOAT4& color);
	
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	SpriteType        _spriteType;
	DirectX::XMFLOAT2 _size;
	//float _angle  = 0.0f;
};

#endif