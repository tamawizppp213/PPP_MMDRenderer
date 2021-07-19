//////////////////////////////////////////////////////////////////////////////////
///             @file   CubeMap.hpp
///             @brief  CubeMap
///             @author Toide Yutaro
///             @date   2021_03_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_CUBE_MAP_HPP
#define DIRECTX12_CUBE_MAP_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/Core/DirectX12MeshBuffer.hpp"
#include "GameCore/Include/Camera.hpp"
#include <memory>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using  SceneGPUAddress = D3D12_GPU_VIRTUAL_ADDRESS;
struct ObjectConstants;
struct SceneConstants;
struct MaterialConstants;
class  GameTimer;

//////////////////////////////////////////////////////////////////////////////////
//                             Class
//////////////////////////////////////////////////////////////////////////////////
enum class CubemapFace : int
{
	Right  = 0,
	Left   = 1,
	Top    = 2,
	Bottom = 3,
	Front  = 4,
	Back   = 5
};

/****************************************************************************
*				  			StaticCubemap
*************************************************************************//**
*  @class     StaticCubemap
*  @brief     Creating a cube map where the reflection does not change in each frame
*****************************************************************************/
class StaticCubemap
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(const std::wstring& filePath);
	bool Draw();
	bool Terminal();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	StaticCubemap()  = default;
	~StaticCubemap() = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool BuildRootSignature();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

};

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class DynamicCubemap
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(Device* device, UINT width, UINT height, const DirectX::XMFLOAT3& origin, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	bool Update(SceneGPUAddress scene, GameTimer& gameTimer);
	bool Draw();
	bool Finalize();


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool SetCubemapSize(UINT width, UINT height);
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DynamicCubemap() = default;
	DynamicCubemap(Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	DynamicCubemap(const DynamicCubemap& cubemap)            = delete; // prohibit copy
	DynamicCubemap& operator=(const DynamicCubemap& cubemap) = delete; // prohibit copy
	
	~DynamicCubemap() = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool BuildResource();
	bool BuildDescriptors();
	bool BuildDescriptors(CPU_DESC_HANDLER cpuSrv, GPU_DESC_HANDLER gpuSrv, CPU_DESC_HANDLER cpuRtv[6]);
	void SetVariables(Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	bool BuildCubeFaceCamera(const DirectX::XMFLOAT3& origin);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Device* _device = nullptr;
	UINT _width;
	UINT _height;

	DXGI_FORMAT    _format = DXGI_FORMAT_R8G8B8A8_UNORM;

	CPU_DESC_HANDLER _cpuSrv;
	GPU_DESC_HANDLER _gpuSrv;
	CPU_DESC_HANDLER _cpuRtv[6];

	ResourceComPtr _cubemap = nullptr;
	Camera  _camera[6];
};

class Skybox
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(const std::wstring& texturePath);
	bool Draw(SceneGPUAddress scene);
	bool Finalize();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetSkyboxMaterial(MaterialConstants& materialInfo);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Skybox()  = default;
	~Skybox() = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool PrepareVertexAndIndexBuffer();
	bool PrepareMaterialBuffer();
	bool PrepareSkyObject();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Texture _texture;
	Resource* _skyResource = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>>  _skyObject =  nullptr;
	std::unique_ptr<UploadBuffer<MaterialConstants>> _material = nullptr;
	std::vector<std::unique_ptr<MeshBuffer>> _sphere;
};

#endif