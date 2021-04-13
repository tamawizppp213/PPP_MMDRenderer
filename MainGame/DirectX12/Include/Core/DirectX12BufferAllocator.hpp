//////////////////////////////////////////////////////////////////////////////////
//              Title:  DirectX12BufferView.hpp
//            Content:  CSU Buffer View Manager 
//             Author:  Toide Yutaro
//             Create:  2020_11_23
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_BUFFER_VIEW_HPP
#define DIRECTX12_BUFFER_VIEW_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12BaseStruct.hpp"
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define INVALID_ID -1

//////////////////////////////////////////////////////////////////////////////////
//                             Class 
//////////////////////////////////////////////////////////////////////////////////
enum class HeapType
{
	RTV,
	DSV,
	CBV,
	SRV,
	UAV,
	HEAP_TYPE_COUNT
};
/****************************************************************************
*				  		Resource View
*************************************************************************//**
*  @class     Resource View
*  @brief     Resource View
*****************************************************************************/
class ResourceAllocator
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	inline CPU_DESC_HANDLER GetCPUDescHandler(UINT offsetIndex = 0) const 
	{ 
		if(offsetIndex > _maxDescriptorCount){ MessageBox(NULL, L"The number of IDs has exceeded the expected number. ", L"Warning", MB_ICONWARNING); }
		return CPU_DESC_HANDLER(_cpuHeapPtr, offsetIndex, _descriptorSize); 
	}
	inline GPU_DESC_HANDLER GetGPUDescHandler(UINT offsetIndex = 0) const 
	{ 
		if (offsetIndex > _maxDescriptorCount) { MessageBox(NULL, L"The number of IDs has exceeded the expected number. ", L"Warning", MB_ICONWARNING); }
		return GPU_DESC_HANDLER(_gpuHeapPtr, offsetIndex, _descriptorSize); 
	}

	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	inline UINT IssueID()
	{
		if (_currentID + 1 > _maxDescriptorCount) { MessageBox(NULL, L"The number of IDs has exceeded the expected number. ", L"Warning", MB_ICONWARNING); }
		
		_currentID++; return _currentID; 
	}
	inline UINT GetHeapSize()  const { return _maxDescriptorCount * _descriptorSize; }
	inline void SetResourceAllocator(UINT maxDescriptorCount, UINT descriptorSize, D3D12_CPU_DESCRIPTOR_HANDLE cpuHeapPtr, D3D12_GPU_DESCRIPTOR_HANDLE gpuHeapPtr)
	{
		_maxDescriptorCount  = maxDescriptorCount;
		_descriptorSize      = descriptorSize;
		_cpuHeapPtr          = cpuHeapPtr;
		_gpuHeapPtr          = gpuHeapPtr;
		_currentID           = INVALID_ID;
	}

	/****************************************************************************
	**                Constructor
	*****************************************************************************/
	// Prohibit copy and move
	ResourceAllocator()                                    = default;
	ResourceAllocator(const ResourceAllocator&)            = delete;
	ResourceAllocator& operator=(const ResourceAllocator&) = delete;
	ResourceAllocator(ResourceAllocator&&)                 = delete;
	ResourceAllocator& operator=(ResourceAllocator&&)      = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE _cpuHeapPtr         = D3D12_CPU_DESCRIPTOR_HANDLE();
	D3D12_GPU_DESCRIPTOR_HANDLE _gpuHeapPtr         = D3D12_GPU_DESCRIPTOR_HANDLE();
	UINT                        _descriptorSize     = 0;
	UINT                        _maxDescriptorCount = 0;
	UINT                        _currentID          = INVALID_ID;
}; 

class RenderTargetViewAllocator    final : public ResourceAllocator{};
class DepthStencilViewAllocator    final : public ResourceAllocator{};
class ConstantBufferViewAllocator  final : public ResourceAllocator{};
class ShaderResourceViewAllocator  final : public ResourceAllocator{};
class UnorderedAccessViewAllocator final : public ResourceAllocator{};
#endif