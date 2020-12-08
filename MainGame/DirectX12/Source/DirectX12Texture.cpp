//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12TextureTexture.cpp
///             @brief  .tga, .dds, ,png, .jpg, .bmp, .hdr, .sph, .spa
///             @author Toide Yutaro
///             @date   2020_12_06
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                              Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/DirectX12Texture.hpp"
#include "DirectX12/Include/DirectX12Debug.hpp"
#include "DirectX12/Include/DirectX12BaseStruct.hpp"
#include "DirectX12/Include/DirectX12Buffer.hpp"
#include "DirectX12/Include/DirectX12Base.hpp"
#include <DirectXTex/DirectXTex.h>
#include <algorithm>

#ifdef _DEBUG
#pragma comment(lib, "Pluguins/DirectXTexd.lib")
#else 
#pragma comment(lib, "Pluguins/DirectXTex.lib")
#endif
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Texture 
//////////////////////////////////////////////////////////////////////////////////
using namespace DirectX;
void Texture::LoadTexture(DirectX12& directX12, const std::wstring& filePath, ResourceComPtr& buffer)
{
	/*-------------------------------------------------------------------
	-               If the file is loaded once, read from it
	---------------------------------------------------------------------*/
	if (g_textureTable.find(filePath) != g_textureTable.end())
	{
		buffer = g_textureTable[filePath];
		return;
	}

	/*-------------------------------------------------------------------
	-                Choose extension and Load Texture Data
	---------------------------------------------------------------------*/
	TexMetadata  metaData     = {};
	ScratchImage scratchImage = {};
	bool isDXT                = false;
	std::wstring extension    = GetExtension(filePath);
	/*-------------------------------------------------------------------
	-    Select the appropriate texture loading function for each extension
	---------------------------------------------------------------------*/
	
	if (extension == L"tga")
	{
		ThrowIfFailed(LoadFromTGAFile(filePath.c_str(), TGA_FLAGS_NONE, &metaData, scratchImage));
	}
	else if (extension == L"dds")
	{
		ThrowIfFailed(LoadFromDDSFile(filePath.c_str(),DDS_FLAGS_NONE, &metaData, scratchImage));
		isDXT = true;
	}
	else if (extension == L"hdr")
	{
		ThrowIfFailed(LoadFromHDRFile(filePath.c_str(), &metaData, scratchImage));
	}
	else
	{
		ThrowIfFailed(LoadFromWICFile(filePath.c_str(), WIC_FLAGS_NONE, &metaData, scratchImage));
	}
	auto image      = scratchImage.GetImage(0, 0, 0);
	bool isDiscrete = true;

	CreateTextureFromImageData(directX12.GetDevice(), image, buffer, isDiscrete);

	if (!isDiscrete)
	{
		ThrowIfFailed(buffer->WriteToSubresource(
			0,
			nullptr,
			image->pixels,
			static_cast<UINT>(image->rowPitch),
			static_cast<UINT>(image->slicePitch)));
	}
	else
	{
		D3D12_HEAP_PROPERTIES heapProperty = HEAP_PROPERTY(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDesc = RESOURCE_DESC::Buffer(
			AlignmentValue(static_cast<UINT>(image->rowPitch), static_cast<UINT>(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT)) * image->height);

		ResourceComPtr uploadBuffer = nullptr;
		ThrowIfFailed(directX12.GetDevice()->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf())));

		BYTE* mappedData = nullptr;
		uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(mappedData));

		auto address = image->pixels;
		UINT32 height = isDXT ? static_cast<UINT32>(image->height / 4) : static_cast<UINT32>(image->height);

		for (UINT32 i = 0; i < height; ++i)
		{
			std::copy_n(address, image->rowPitch, mappedData);
			address += image->rowPitch;
			mappedData += AlignmentValue(
				static_cast<UINT>(image->rowPitch),
				D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
		}

		uploadBuffer->Unmap(0, nullptr);

		D3D12_TEXTURE_COPY_LOCATION source = {}, dst = {};
		source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		source.pResource = uploadBuffer.Get();
		source.PlacedFootprint.Offset = 0;
		source.PlacedFootprint.Footprint.Width    = static_cast<UINT>(image->width);
		source.PlacedFootprint.Footprint.Height   = static_cast<UINT>(image->height);
		source.PlacedFootprint.Footprint.RowPitch =
			AlignmentValue(static_cast<UINT>(image->rowPitch), static_cast<UINT>(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
		source.PlacedFootprint.Footprint.Depth    = static_cast<UINT>(metaData.depth);
		source.PlacedFootprint.Footprint.Format   = image->format;

		dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;
		dst.pResource        = buffer.Get();

		/*-------------------------------------------------------------------
		-                Copy Texture Region
		---------------------------------------------------------------------*/
		directX12.GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &source, nullptr);

		/*-------------------------------------------------------------------
		-                Bariier Transition
		---------------------------------------------------------------------*/
		directX12.GetCommandList()->ResourceBarrier(1, &BARRIER::Transition(
			buffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		/*-------------------------------------------------------------------
		-                Execute Command List
		---------------------------------------------------------------------*/
		directX12.GetCommandList()->Close();
		ID3D12CommandList* commandLists[] = { directX12.GetCommandList() };
		directX12.GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
		directX12.FlushCommandQueue();
		directX12.ResetCommandList();
	}

	/*-------------------------------------------------------------------
	-                    Add texture table
	---------------------------------------------------------------------*/
	g_textureTable[filePath] = buffer;
}

void Texture::CreateTextureFromImageData(Device* device, const DirectX::Image* image, ResourceComPtr& buffer, bool isDiscreteGPU)
{
	D3D12_HEAP_PROPERTIES heapProperty = {};
	
	if (isDiscreteGPU)
	{
		heapProperty = HEAP_PROPERTY(D3D12_HEAP_TYPE_DEFAULT);
	}
	else
	{
		heapProperty.Type                 = D3D12_HEAP_TYPE_CUSTOM;
		heapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProperty.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProperty.CreationNodeMask     = 0;
		heapProperty.VisibleNodeMask      = 0;
	}

	/*-------------------------------------------------------------------
	-             Setting the final write destination resource
	---------------------------------------------------------------------*/
	D3D12_RESOURCE_DESC resourceDesc = RESOURCE_DESC::Texture2D(image->format, (UINT64)image->width, (UINT)image->height);

	if (isDiscreteGPU)
	{
		ThrowIfFailed(device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(buffer.ReleaseAndGetAddressOf())))
	}
	else
	{
		ThrowIfFailed(device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(buffer.ReleaseAndGetAddressOf())));
	}
	
}