//////////////////////////////////////////////////////////////////////////////////
//              Title:  MotionTest.hpp
//            Content:  Motion Test Scene (For Debug) 
//             Author:  Toide Yutaro
//             Create:  2020_11_06
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MOTION_TEST_HPP
#define MOTION_TEST_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Scene.hpp"
#include "DirectX12/Include/DirectX12Geometry.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/DirectX12MathHelper.hpp"
#include "GameCore/Include/FrameResources.hpp"
#include <vector>
#include <unordered_map>
#include <memory>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct RenderItem
{
	RenderItem() = default;

	DirectX::XMFLOAT4X4 World   = DirectXMathHelper::Identity4X4();
	int NumFramesDirty = numFrameResources;
	UINT ObjectCBIndex = -1;

	MeshGeometry* Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount         = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation  = 0;
	const int numFrameResources = 3;
};

//////////////////////////////////////////////////////////////////////////////////
//                           MotionTest Class
//////////////////////////////////////////////////////////////////////////////////
class MotionTest final : public Scene
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	MotionTest();
	~MotionTest();
	void Initialize(GameTimer& gameTimer) override;
	void Draw()       override;
	void Update()     override;
	void Terminate()  override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

protected:
	void LoadShaders()             ;
	void LoadGeometry()            ;
	void LoadTextures()            ;
	void BuildPSOs()               ;
	void BuildRootSignature()      ;
	void BuildRenderItem()         ;
	void BuildFrameResources()     ;
	void BuildDescriptorHeap()     ;
	void BuildConstantBufferView() ;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void LoadPrimitiveGeometry();

	// DebugTest
	void TestPrimitiveTriangle();
	void TestLoadShader();
	void TestRootSignature();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	DirectX12& _directX12 = DirectX12::Instance();
	
	RootSignatureComPtr  _rootSignature = nullptr;
	DescriptorHeapComPtr _cbvHeap       = nullptr;
	std::unordered_map<std::string, BlobComPtr>                    _shaders;
	std::unordered_map<std::string, PipelineStateComPtr>           _PSOs;
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> _geometries;
	std::vector<std::unique_ptr<RenderItem>> _allRenderItems;
	std::vector<RenderItem*> _opaqueRItems;
	UINT _passCbvOffset = 0;
	UINT _cbvSrvUavDescriptorSize = 0;
	
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW  ibView;
	std::vector<std::unique_ptr<FrameResource>> _frameResources;
};
#endif