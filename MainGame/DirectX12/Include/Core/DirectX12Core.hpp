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
using Device           = ID3D12Device8;
using Factory          = IDXGIFactory4;
using Adapter          = IDXGIAdapter;
using Output           = IDXGIOutput;
using CommandList      = ID3D12GraphicsCommandList5;
using CommandQueue     = ID3D12CommandQueue;
using CommandAllocator = ID3D12CommandAllocator;
using SwapChain        = IDXGISwapChain4;
using Resource         = ID3D12Resource;
using PSODesc          = D3D12_GRAPHICS_PIPELINE_STATE_DESC;
using RootSignature    = ID3D12RootSignature;
using PipelineState    = ID3D12PipelineState;
using DescriptorHeap   = ID3D12DescriptorHeap;

using DeviceComPtr           = ComPtr<ID3D12Device8>;
using AdapterComPtr          = ComPtr<IDXGIAdapter1>;
using FactoryComPtr          = ComPtr<IDXGIFactory7>;
using SwapchainComPtr        = ComPtr<IDXGISwapChain4>;
using OutputComPtr           = ComPtr<IDXGIOutput6>;
using CommandAllocatorComPtr = ComPtr<ID3D12CommandAllocator>;
using CommandQueueComPtr     = ComPtr<ID3D12CommandQueue>;
using CommandListComPtr      = ComPtr<ID3D12GraphicsCommandList5>;
using FenceComPtr            = ComPtr<ID3D12Fence>;
using ResourceComPtr         = ComPtr<ID3D12Resource1>;
using DescriptorHeapComPtr   = ComPtr<ID3D12DescriptorHeap>;
using PipelineStateComPtr    = ComPtr<ID3D12PipelineState>;
using RootSignatureComPtr    = ComPtr<ID3D12RootSignature>;
using BlobComPtr             = ComPtr<ID3DBlob>;
using DebugComPtr            = ComPtr<ID3D12Debug>;

#endif DirectX12_CORE_HPP

