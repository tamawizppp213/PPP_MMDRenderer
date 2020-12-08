//////////////////////////////////////////////////////////////////////////////////
//              Title:  Test.hpp
//            Content:  Test
//             Author:  Toide Yutaro
//             Create:  2020_12_06
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef TEST_HPP
#define TEST_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Scene.hpp"
#include "DirectX12/Include/DirectX12Geometry.hpp"
#include "DirectX12/Include/DirectX12Buffer.hpp"
#include <vector>
#include <memory>



//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                           MotionTest Class
//////////////////////////////////////////////////////////////////////////////////
class Test final : public Scene
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	Test();
	~Test();
	void Initialize(const DirectX12& directX12) override;
	void Draw()       override;
	void Update()     override;
	void Terminate()  override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

protected:
	void LoadShaders()             override;
	void LoadGeometry()            override;
	void BuildPSOs()               override;
	void BuildRootSignature()      override;
	void BuildRenderItem()         override;
	void BuildFrameResources()     override;
	void BuildDescriptorHeap()     override;
	void BuildConstantBufferView() override;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	DirectX12           _directX12;
	DeviceComPtr        _device;
	CommandListComPtr   _commandList;
	RootSignatureComPtr _rootSignature  = nullptr;
	PipelineStateComPtr _pipelineState  = nullptr;
	BlobComPtr _vertexShader = {};
	BlobComPtr _pixelShader  = {};
	Resource*  _vertexBuffer = nullptr;
	Resource*  _indexBuffer  = nullptr;
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> _geometries;
};
#endif