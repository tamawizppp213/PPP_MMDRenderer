//////////////////////////////////////////////////////////////////////////////////
///             @file   PMXConfig.hpp
///             @brief  PMXFile Config
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PMD_MODEL_HPP
#define PMD_MODEL_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/Model.hpp"
#include "GameCore/Include/GameConstantBufferConfig.hpp"
#include "DirectX12/Include/DirectX12PrimitiveGeometry.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class GameTimer;
struct PBRMaterial;

using SceneGPUAddress = D3D12_GPU_VIRTUAL_ADDRESS;
using LightGPUAddress = D3D12_GPU_VIRTUAL_ADDRESS;

class PrimitiveModel : public DefaultModel
{
	using VertexBuffer   = std::unique_ptr<UploadBuffer<VertexPositionNormalTexture>>;
	using MaterialBuffer = std::unique_ptr<UploadBuffer<PBRMaterial>>;
	using MaterialPtr    = std::unique_ptr<PBRMaterial>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void CreateRect      ();
	void CreateQuadrangle(float x, float y, float w, float h, float depth);
	void CreateBox       (bool isInvertNormal = false);
	void CreateSphere    (UINT32 sliceCount = 20, UINT32 stackCount = 20, bool isInvertNormal = false);
	void CreateGeoSphere (UINT32 numSubdivisions, bool isInvertNormal = false);
	void CreateCylinder  (float bottomRadius = 1.0f, float topRadius = 1.0f, UINT32 sliceCount = 20, UINT32 stackCount = 20, bool isInvertNormal = false);
	void CreateGrid      (UINT32 rows = 1, UINT32 columns = 1, bool isInvertNormal = false);
	
	void LoadTexture(const std::wstring& filePath);

	bool Update();
	bool Draw(SceneGPUAddress scene, LightGPUAddress light);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetWorldTimer(const GameTimer& gameTimer);

	/*-------------------------------------------------------------------
	-               Material Property
	---------------------------------------------------------------------*/
	void SetMaterial         (const PBRMaterial& material);
	void SetDiffuse          (const gm::Float4& diffuse);
	void SetSpecular         (const gm::Float3& specular);
	void SetRoughness        (float roughness);
	void SetEmissiveColor    (const gm::Float3& color);
	void SetEmissiveIntensity(float intensity);
	void SetAmbient          (const gm::Float3& ambient);
	void SetMetalness        (float metalness);

	/*-------------------------------------------------------------------
	-               Mesh Property
	---------------------------------------------------------------------*/
	int  GetVertexCount() { return (int)_meshData.Vertices.size(); }
	int  GetIndexCount()  { return (int)_meshData.Indices.size(); }
	UploadBuffer<PBRMaterial>* GetMaterialBuffer() const { return _materialBuffer.get(); }
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PrimitiveModel();
	PrimitiveModel(const PrimitiveModel&)            = default;
	PrimitiveModel& operator=(const PrimitiveModel&) = default;
	PrimitiveModel(PrimitiveModel&&)                 = default;
	PrimitiveModel& operator=(PrimitiveModel&&)      = default;
	~PrimitiveModel();
protected:
	/****************************************************************************
	**                ProtectedFunction
	*****************************************************************************/
	bool PrepareBuffers();
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();
	bool PrepareMaterialBuffer();
	bool PrepareObjectBuffer();

	bool UpdateGPUData();

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	MeshData       _meshData;
	VertexBuffer   _vertexBuffer[FRAME_BUFFER_COUNT];
	MaterialBuffer _materialBuffer;
	Texture        _texture;
	MaterialPtr    _material;
	int  _currentFrameIndex = 0;
	bool _isInitialized = false;
	
};

#endif