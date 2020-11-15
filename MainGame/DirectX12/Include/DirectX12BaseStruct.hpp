//////////////////////////////////////////////////////////////////////////////////
//              Title:  DirectX12BaseStruct.hpp
//            Content:  DirectX12 Struct for DirexctX12_Base.hpp 
//                      (For Studying)(Reference: Microsoft d3dx12.h) 
//             Author:  Toide Yutaro
//             Create:  2020_11_
//////////////////////////////////////////////////////////////////////////////////
#ifndef DIRECTX12_BASE_STRUCT_HPP
#define DIRECTX12_BASE_STRUCT_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <d3d12.h>
#include "DirectX12/Include/DirectX12Core.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                     Heap Property : D3D12_HEAP_PROPERTIES
//////////////////////////////////////////////////////////////////////////////////
struct HEAP_PROPERTY : public D3D12_HEAP_PROPERTIES
{
	HEAP_PROPERTY() {};

	// Heap property
	explicit HEAP_PROPERTY(const D3D12_HEAP_PROPERTIES& heapDesc) 
		: D3D12_HEAP_PROPERTIES(heapDesc) {};

	// Heap type: custom
	HEAP_PROPERTY(
		D3D12_CPU_PAGE_PROPERTY cpuPageProperty,
		D3D12_MEMORY_POOL memoryPoolPreference,
		UINT creationNodeMask = 1,
		UINT nodeMask = 1)
	{
		Type                 = D3D12_HEAP_TYPE_CUSTOM;
		CPUPageProperty      = cpuPageProperty;
		MemoryPoolPreference = memoryPoolPreference;
		CreationNodeMask     = creationNodeMask;
		VisibleNodeMask      = nodeMask;
	}

	// Heap type: Non-custom
	explicit HEAP_PROPERTY(
		D3D12_HEAP_TYPE type,
		UINT creationNodeMask = 1,
		UINT nodeMask = 1)
	{
		Type                 = type;
		CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		CreationNodeMask     = creationNodeMask;
		VisibleNodeMask      = nodeMask;
	}
	 
	// Return oneself pointer (unary operator )
	operator const D3D12_HEAP_PROPERTIES& () const { return *this; }

	bool IsCPUAccessible() const
	{
		return Type == D3D12_HEAP_TYPE_UPLOAD || Type == D3D12_HEAP_TYPE_READBACK || (Type == D3D12_HEAP_TYPE_CUSTOM &&
			(CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE || CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK));
	}
};

// Equal
inline bool operator==(const D3D12_HEAP_PROPERTIES& l, const D3D12_HEAP_PROPERTIES& r)
{
	return l.Type == r.Type && l.CPUPageProperty == r.CPUPageProperty &&
		l.MemoryPoolPreference == r.MemoryPoolPreference &&
		l.CreationNodeMask     == r.CreationNodeMask &&
		l.VisibleNodeMask      == r.VisibleNodeMask;
}

// Not Equal
inline bool operator!=(const D3D12_HEAP_PROPERTIES& l, const D3D12_HEAP_PROPERTIES& r)
{
	return !(l == r);
}

//////////////////////////////////////////////////////////////////////////////////
//                     Heap Desc : D3D12_HEAP_DESC
//////////////////////////////////////////////////////////////////////////////////
struct HEAP_DESC : public D3D12_HEAP_DESC
{
	HEAP_DESC() {};
	explicit HEAP_DESC(const D3D12_HEAP_DESC& heapDesc) :D3D12_HEAP_DESC(heapDesc) {};

	HEAP_DESC(
		UINT64 sizeInBytes,
		D3D12_HEAP_PROPERTIES heapProperties,
		UINT64 alignment = 0,
		D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE)
	{
		SizeInBytes = sizeInBytes;
		Properties  = heapProperties;
		Alignment   = alignment;
		Flags       = flags;
	}

	// The folowing constructors are used to make small changes tp the Heap Property 
	// Use HEAP_PROPERTY pointer
	HEAP_DESC(
		UINT64 sizeInBytes,
		D3D12_HEAP_TYPE type,
		UINT64 alignment = 0,
		D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE)
	{
		SizeInBytes = sizeInBytes;
		Properties  = HEAP_PROPERTY(type);
		Alignment   = alignment;
		Flags       = flags;
	}

	HEAP_DESC(
		UINT64 sizeInBytes,
		D3D12_CPU_PAGE_PROPERTY cpuPageProperty,
		D3D12_MEMORY_POOL memoryPoolPreference,
		UINT64 alignment = 0,
		D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE)
	{
		SizeInBytes = sizeInBytes;
		Properties  = HEAP_PROPERTY(cpuPageProperty, memoryPoolPreference);
		Alignment   = alignment;
		Flags       = flags;
	}

	HEAP_DESC(
		const D3D12_RESOURCE_ALLOCATION_INFO& resAllocInfo,
		D3D12_HEAP_PROPERTIES properties,
		D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE)
	{
		SizeInBytes = resAllocInfo.SizeInBytes;
		Properties  = properties;
		Alignment   = resAllocInfo.Alignment;
		Flags       = flags;
	}
	HEAP_DESC(
		const D3D12_RESOURCE_ALLOCATION_INFO& resAllocInfo,
		D3D12_HEAP_TYPE type,
		D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE)
	{
		SizeInBytes = resAllocInfo.SizeInBytes;
		Properties  = HEAP_PROPERTY(type);
		Alignment   = resAllocInfo.Alignment;
		Flags       = flags;
	}

	HEAP_DESC(
		const D3D12_RESOURCE_ALLOCATION_INFO& resAllocInfo,
		D3D12_CPU_PAGE_PROPERTY cpuPageProperty,
		D3D12_MEMORY_POOL memoryPoolPreference,
		D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE)
	{
		SizeInBytes = resAllocInfo.SizeInBytes;
		Properties  = HEAP_PROPERTY(cpuPageProperty, memoryPoolPreference);
		Alignment   = resAllocInfo.Alignment;
		Flags       = flags;
	}
};

inline bool operator==(const D3D12_HEAP_DESC& l, const D3D12_HEAP_DESC& r)
{
	return l.SizeInBytes == r.SizeInBytes &&
		   l.Properties  == r.Properties  &&
		   l.Alignment   == r.Alignment   &&
		   l.Flags       == r.Flags;
}
inline bool operator!=(const D3D12_HEAP_DESC& l, const D3D12_HEAP_DESC& r)
{
	return !(l == r);
}

//////////////////////////////////////////////////////////////////////////////////
//                      BARRIRER 
//////////////////////////////////////////////////////////////////////////////////
struct BARRIER : public D3D12_RESOURCE_BARRIER
{
	BARRIER() {};
	explicit BARRIER(const D3D12_RESOURCE_BARRIER& barrier) :D3D12_RESOURCE_BARRIER(barrier) {};

	/****************************************************************************
	**                    Transition
	*****************************************************************************/
	static inline BARRIER Transition(
		_In_ ID3D12Resource* pResource,
		D3D12_RESOURCE_STATES before,
		D3D12_RESOURCE_STATES after,
		UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
		D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE)
	{
		BARRIER result;
		ZeroMemory(&result, sizeof(result));
		D3D12_RESOURCE_BARRIER& barrier = result;

		result.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		result.Flags = flags;

		barrier.Transition.pResource   = pResource;
		barrier.Transition.StateBefore = before;
		barrier.Transition.StateAfter  = after;
		barrier.Transition.Subresource = subResource;
	}

	/****************************************************************************
	**                    Aliasing
	*****************************************************************************/
	static inline BARRIER Aliasing(
		_In_ ID3D12Resource* before,
		_In_ ID3D12Resource* after)
	{
		BARRIER result;
		ZeroMemory(&result, sizeof(result));

		D3D12_RESOURCE_BARRIER& barrier = result;
		result.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;

		barrier.Aliasing.pResourceBefore = before;
		barrier.Aliasing.pResourceAfter  = after;
		return result;
	}

	/****************************************************************************
	**                    Unordered Access View (UAV)
	*****************************************************************************/
	static inline BARRIER UAV(
		_In_ ID3D12Resource* pResource)
	{
		BARRIER result;
		ZeroMemory(&result, sizeof(result));

		D3D12_RESOURCE_BARRIER& barrier = result;
		result.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		
		barrier.UAV.pResource = pResource;
		return result;
	}

	operator const D3D12_RESOURCE_BARRIER& () const { return *this; }
};


#endif