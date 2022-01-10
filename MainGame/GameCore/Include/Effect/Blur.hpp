//////////////////////////////////////////////////////////////////////////////////
///             @file   Blur.hpp
///             @brief  Post Effect Blur (use compute shader, Gaussian blur)
///             @author Toide Yutaro
///             @date   2021_07_13
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef BLUR_HPP
#define BLUR_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "PostEffect.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
namespace blur
{
	struct BlurParameter
	{
		gm::Float4 Weights[2];
	};

	struct TextureSizeParameter
	{
		int OriginalTexture[2];
		int XBlurTexture[2];
		int YBlurTexture[2];
	};
}

//////////////////////////////////////////////////////////////////////////////////
//                            Class
//////////////////////////////////////////////////////////////////////////////////
// Gaussian Blur
class Blur final : public PostEffect 
{
	using BlurParameters = std::unique_ptr<UploadBuffer<blur::BlurParameter>>;
	using TextureSizeCB = std::unique_ptr<UploadBuffer<blur::TextureSizeParameter>>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(int width, int height, DXGI_FORMAT format);
	bool OnResize(int newWidth, int newHeight) override;
	bool Draw()     override;
	bool Draw(Resource* renderTarget, D3D12_RESOURCE_STATES renderTargetState = D3D12_RESOURCE_STATE_COMMON);
	void Finalize() override;
	void UpdateWeightsTable(float sigma);
	void UpdateBlurSampling(int xBlurWidthDivision, int xBlurHeightDivision, int yBlurWidthDivision, int yBlurHeightDivision); // substitute for division count;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	ColorBuffer& GetHalfDownSamplingColorBuffer() { return _colorBuffer[_colorBuffer.size() - 2]; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Blur()                       = default;
	Blur(const Blur&)            = default;
	Blur& operator=(const Blur&) = default;
	Blur(Blur&&)                 = default;
	Blur& operator=(Blur&&)      = default;
	~Blur() {  }

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	bool PrepareResources(int width, int height, DXGI_FORMAT format);
	bool PrepareDescriptors();
	bool PrepareBlurParameters();
	bool PrepareTextureSizeBuffer(int width, int height);
	bool PrepareRootSignature();
	bool PreparePipelineState();

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	BlurParameters _weightBuffer;
	TextureSizeCB  _textureSizeBuffer;
	blur::TextureSizeParameter _textureSizeParameter;

	PipelineStateComPtr _xBlurPipeline = nullptr;
	PipelineStateComPtr _yBlurPipeline = nullptr;

};
#endif