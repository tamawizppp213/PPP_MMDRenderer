//////////////////////////////////////////////////////////////////////////////////
//              Title:  DirectX12Core.hpp
//            Content:  DirectX12 Alias, Forward Declaration
//             Author:  Toide Yutaro
//             Create:  2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_CORE_HPP
#define DIRECTX12_CORE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

//////////////////////////////////////////////////////////////////////////////////
//                         Forward Declaration
//////////////////////////////////////////////////////////////////////////////////
class DirectX12;

//////////////////////////////////////////////////////////////////////////////////
//                         Alias 
//////////////////////////////////////////////////////////////////////////////////
using Microsoft::WRL::ComPtr;
using Device       = ID3D12Device;
using Factory      = IDXGIFactory4;
using Adapter      = IDXGIAdapter;
using Output       = IDXGIOutput;
using CommandList  = ID3D12GraphicsCommandList5;
using CommandQueue = ID3D12CommandQueue;
using Resource     = ID3D12Resource;

using DeviceComPtr           = ComPtr<ID3D12Device>;
using AdapterComPtr          = ComPtr<IDXGIAdapter>;
using FactoryComPtr          = ComPtr<IDXGIFactory7>;
using SwapchainComPtr        = ComPtr<IDXGISwapChain4>;
using CommandAllocatorComPtr = ComPtr<ID3D12CommandAllocator>;
using CommandQueueComPtr     = ComPtr<ID3D12CommandQueue>;
using CommandListComPtr      = ComPtr<ID3D12GraphicsCommandList5>;
using FenceComPtr            = ComPtr<ID3D12Fence1>;
using ResourceComPtr         = ComPtr<ID3D12Resource1>;
using DescriptorHeapComPtr   = ComPtr<ID3D12DescriptorHeap>;
using PipelineStateComPtr    = ComPtr<ID3D12PipelineState>;
using DebugComPtr            = ComPtr<ID3D12Debug>;

#endif DirectX12_CORE_HPP

