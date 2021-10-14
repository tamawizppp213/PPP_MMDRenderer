//////////////////////////////////////////////////////////////////////////////////
///             @file   Model.hpp
///             @brief  Model
///             @author Toide Yutaro
///             @date   2021_03_10
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MODEL_HPP
#define MODEL_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Config.hpp"
#include "DirectX12/Include/Core/DirectX12MeshBuffer.hpp"
#include "GameCore/Include/GameConstantBufferConfig.hpp"
#include "GameCore/Include/Core/GameActor.hpp"
#include "GameMath/Include/GMTransform.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

class DefaultModel : public GameActor
{
	using ModelObject = std::unique_ptr<UploadBuffer<ObjectConstants>>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	MeshBuffer& GetCurrentMeshBuffer();
	UploadBuffer<ObjectConstants>* GetModelWorldInfo() const { return _modelObject.get(); }

	void EnableAmbientOcculusionMap(bool enabled) { _worldInfo.TextureConfig = enabled; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DefaultModel()                               = default;
	DefaultModel(const DefaultModel&)            = default;
	DefaultModel& operator=(const DefaultModel&) = default;
	DefaultModel(DefaultModel&&)                 = default;
	DefaultModel& operator=(DefaultModel&&)      = default;
	~DefaultModel();
protected:
	/****************************************************************************
	**                ProtectedFunction
	*****************************************************************************/
	bool PrepareVertexBuffer() { return true; };
	bool PrepareIndexBuffer() { return true; };

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	std::wstring   _filePath;
	MeshBuffer     _meshBuffer[FRAME_BUFFER_COUNT];
	BlobComPtr     _vertexShader = nullptr;
	BlobComPtr     _pixelShader  = nullptr;

	/*-------------------------------------------------------------------
	-           Object Property
	---------------------------------------------------------------------*/
	ObjectConstants           _worldInfo;
	ModelObject               _modelObject = nullptr;
	
};
#endif