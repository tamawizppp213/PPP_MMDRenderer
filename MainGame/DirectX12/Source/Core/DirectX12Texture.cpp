//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12TextureTexture.cpp
///             @brief  .tga, .dds, ,png, .jpg, .bmp, .hdr, .sph, .spa
///             @author Toide Yutaro
///             @date   2020_12_06
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                              Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include "DirectX12/Include/Core/DirectX12Debug.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include <algorithm>
#include <iostream>
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

/****************************************************************************
*							  LoadTexture
*************************************************************************//**
*  @fn         void TextureLoader::LoadTexture(const std::wstring& filePath, ResourceComPtr& texture)
*  @brief      Load Texture (.tga, .dds, ,png, .jpg, .bmp, .hdr, .sph, .spa)
*  @param[out] DirectX12& directX12,
*  @param[in]  const std::wstring& filePath
*  @param[out] Texture& texture
*  @return 　　 int
*****************************************************************************/
void TextureLoader::LoadTexture(const std::wstring& filePath, Texture& texture, TextureType type)
{
	DirectX12& directX12 = DirectX12::Instance();
	/*-------------------------------------------------------------------
	-               If the file is loaded once, read from it
	---------------------------------------------------------------------*/
	if (_textureTableManager.Instance().TextureTable.find(filePath) != _textureTableManager.Instance().TextureTable.end())
	{
		texture = _textureTableManager.Instance().TextureTable[filePath];
		return;
	}

	/*-------------------------------------------------------------------
	-                Choose Extension and Load Texture Data
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

	CreateTextureFromImageData(directX12.GetDevice(), image, texture.Resource, isDiscrete);

	/*-------------------------------------------------------------------
	-                 Transmit texture buffer to GPU
	---------------------------------------------------------------------*/
	if (!isDiscrete)
	{
		ThrowIfFailed(texture.Resource->WriteToSubresource(
			0,
			nullptr,
			image->pixels,
			static_cast<UINT>(image->rowPitch),
			static_cast<UINT>(image->slicePitch)));
	}
	else
	{
		/*-------------------------------------------------------------------
		-                 Create Upload Buffer
		---------------------------------------------------------------------*/
		D3D12_HEAP_PROPERTIES heapProperty = HEAP_PROPERTY(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC   resourceDesc = RESOURCE_DESC::Buffer(
			AlignmentValue(static_cast<UINT>(image->rowPitch), static_cast<UINT>(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT)) * image->height);

		ResourceComPtr uploadBuffer = nullptr;
		ThrowIfFailed(directX12.GetDevice()->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf())));

		/*-------------------------------------------------------------------
		-                   Map Upload Buffer
		---------------------------------------------------------------------*/
		BYTE* mappedData = nullptr;
		uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));

		auto address = image->pixels;
		UINT32 height = isDXT ? static_cast<UINT32>(image->height / 4) : static_cast<UINT32>(image->height);

		for (UINT32 i = 0; i < height; ++i)
		{
			std::copy_n(address, image->rowPitch, mappedData);
			address    += image->rowPitch;
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
		dst.pResource        = texture.Resource.Get();

		/*-------------------------------------------------------------------
		-                Copy Texture Region
		---------------------------------------------------------------------*/
		directX12.GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &source, nullptr);

		/*-------------------------------------------------------------------
		-                Bariier Transition
		---------------------------------------------------------------------*/
		auto barrier = BARRIER::Transition(
			texture.Resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		directX12.GetCommandList()->ResourceBarrier(1, &barrier);

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
	-                    Create SRV Desc
	---------------------------------------------------------------------*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	switch (type)
	{
		case TextureType::Texture2D:
			srvDesc.Format                        = texture.Resource.Get()->GetDesc().Format;
			srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels           = 1;
			srvDesc.Texture2D.PlaneSlice          = 0;
			srvDesc.Texture2D.MostDetailedMip     = 0;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			break;
		case TextureType::TextureCube:
			srvDesc.Format                        = texture.Resource.Get()->GetDesc().Format;
			srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels           = 1;
			srvDesc.TextureCube.MostDetailedMip     = 0;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			break;
	}
	/*-------------------------------------------------------------------
	-                    Create SRV
	---------------------------------------------------------------------*/
	_textureTableManager.Instance().ID++;
	directX12.GetDevice()->CreateShaderResourceView(texture.Resource.Get(), &srvDesc,
		CPU_DESC_HANDLER(directX12.GetCPUCbvSrvUavHeapStart(), _textureTableManager.Instance().ID, directX12.GetCbvSrvUavDescriptorHeapSize()));

	/*-------------------------------------------------------------------
	-                    Describe texture infomation
	---------------------------------------------------------------------*/
	Texture addTexture;
	addTexture.Resource   = texture.Resource;
	addTexture.Format     = texture.Resource.Get()->GetDesc().Format;
	addTexture.GPUHandler = GPU_DESC_HANDLER(directX12.GetGPUCbvSrvUavHeapStart(), _textureTableManager.Instance().ID, directX12.GetCbvSrvUavDescriptorHeapSize());
	addTexture.ImageSize  = DirectX::XMFLOAT2(image->width, image->height);
	/*-------------------------------------------------------------------
	-                    Add texture table
	---------------------------------------------------------------------*/
	_textureTableManager.Instance().TextureTable[filePath] = addTexture;
	texture = addTexture;
}

/****************************************************************************
*					    CreateTextureFromImageData
*************************************************************************//**
*  @fn         void Texture::CreateTextureFromImageData(Device* device, const DirectX::Image* image, ResourceComPtr& textureBuffer, bool isDiscreteGPU)
*  @brief      Load Texture (.tga, .dds, ,png, .jpg, .bmp, .hdr, .sph, .spa)
*  @param[out] Device* device
*  @param[in]  const DirectX::Image* image
*  @param[out] ResourceComPtr& textureBuffer
*  @param[in]  bool isDiscreteGPU
*  @return 　　 void
*****************************************************************************/
void TextureLoader::CreateTextureFromImageData(Device* device, const DirectX::Image* image, ResourceComPtr& textureBuffer, bool isDiscreteGPU)
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
			IID_PPV_ARGS(textureBuffer.ReleaseAndGetAddressOf())))
	}
	else
	{
		ThrowIfFailed(device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(textureBuffer.ReleaseAndGetAddressOf())));
	}
	
}