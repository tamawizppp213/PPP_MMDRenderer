//////////////////////////////////////////////////////////////////////////////////
///             @file   PMDModel.cpp
///             @brief  PMDModel
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "GameCore/Include/Model/MMD/PMDModel.hpp"
#include "GameCore/Include/Model/ModelLoader.hpp"
#include <d3dcompiler.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        bool PMDModel::Initialize()
*  @brief     Initialize (Load PMDData ...)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDModel::Initialize(const std::wstring& filePath)
{
	/*-------------------------------------------------------------------
	-             Load PMD Data
	---------------------------------------------------------------------*/
	ModelLoader modelLoader;
	modelLoader.Load3DModel(filePath, &_pmdData);

	/*-------------------------------------------------------------------
	-             Prepare Vertex Buffer
	---------------------------------------------------------------------*/
	if (!PrepareVertexBuffer()) { MessageBox(NULL, L" Vertex buffer cannot be prepared.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Prepare Index Buffer
	---------------------------------------------------------------------*/
	if (!PrepareIndexBuffer()) { MessageBox(NULL, L" Index buffer cannot be prepared.", L"Warning", MB_ICONWARNING); };

	return true;
}

/****************************************************************************
*                       PrepareVertexBuffer
*************************************************************************//**
*  @fn        bool PMDModel::PrepareVertexBuffer()
*  @brief     Prepare Vertex Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDModel::PrepareVertexBuffer()
{
	/*-------------------------------------------------------------------
	-			Prepare variables
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check PMD Vertex is existed
	---------------------------------------------------------------------*/
	if (_pmdData->GetVertex().empty()) { ::OutputDebugString(L"Can't read vertex data (vertex)"); return false; }
	int vertexCount = _pmdData->GetVertex().size();

	/*-------------------------------------------------------------------
	-			Build CPU and GPU Vertex Buffer
	---------------------------------------------------------------------*/
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		/*-------------------------------------------------------------------
		-			Copy PMD Vertex Data To Mesh Buffer
		---------------------------------------------------------------------*/
		_vertexBuffer[i] = std::make_shared<UploadBuffer<PMDVertex>>(directX12.GetDevice(), vertexCount, false);
		for (int j = 0; j < vertexCount; ++j)
		{
			_vertexBuffer[i]->CopyData(j, _pmdData->GetVertex()[j]);
		}

		const UINT vbByteSize = (UINT)vertexCount* sizeof(PMDVertex);

		/*-------------------------------------------------------------------
		-			Build CPU / GPU Vertex Buffer
		---------------------------------------------------------------------*/
		_meshBuffer[i].BaseVertexLocation   = 0;
		_meshBuffer[i].VertexBufferGPU      = _vertexBuffer[i]->Resource();
		_meshBuffer[i].VertexByteStride     = sizeof(PMDVertex);
		_meshBuffer[i].VertexBufferByteSize = vbByteSize;
		if (FAILED(D3DCreateBlob(vbByteSize, &_meshBuffer[i].VertexBufferCPU)))
		{
			::OutputDebugString(L"Can't create blob data (vertex)");
			return false;
		}

	}

	return true;
}

/****************************************************************************
*                       PrepareIndexBuffer
*************************************************************************//**
*  @fn        bool PMDModel::PrepareIndexBuffer()
*  @brief     Prepare Index Buffer
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDModel::PrepareIndexBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Check Index Data
	---------------------------------------------------------------------*/
	if (_pmdData->GetIndex().empty()) { ::OutputDebugString(L"Can't read index data (vertex)"); return false; }
	int indexCount = _pmdData->GetIndex().size();

	/*-------------------------------------------------------------------
	-			Build CPU / GPU Index Buffer
	---------------------------------------------------------------------*/
	const UINT ibByteSize = (UINT)indexCount * sizeof(UINT16);
	_meshBuffer[0].IndexBufferGPU      = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), _pmdData->GetIndex().data(), ibByteSize, _meshBuffer[0].IndexBufferUploader).Resource();
	_meshBuffer[0].IndexFormat         = DXGI_FORMAT_R16_UINT;
	_meshBuffer[0].IndexBufferByteSize = ibByteSize;
	_meshBuffer[0].IndexCount          = indexCount;
	if (FAILED(D3DCreateBlob(ibByteSize, &_meshBuffer[0].IndexBufferCPU)))
	{
		::OutputDebugString(L"Can't create blob data (index)");
		return false;
	}
	
	
	/*-------------------------------------------------------------------
	-		Copy the index buffer by the amount of the frame buffer.
	---------------------------------------------------------------------*/
	for (int i = 1; i < FRAME_BUFFER_COUNT; ++i)
	{
		_meshBuffer[i].IndexBufferCPU      = _meshBuffer[0].IndexBufferCPU;
		_meshBuffer[i].IndexBufferGPU      = _meshBuffer[0].IndexBufferGPU;
		_meshBuffer[i].IndexBufferUploader = _meshBuffer[0].IndexBufferUploader;
		_meshBuffer[i].IndexCount          = _meshBuffer[0].IndexCount;
		_meshBuffer[i].IndexFormat         = _meshBuffer[0].IndexFormat;
		_meshBuffer[i].StartIndexLocation  = _meshBuffer[0].StartIndexLocation;
		_meshBuffer[i].IndexBufferByteSize = _meshBuffer[0].IndexBufferByteSize;
	}
	return true;
}