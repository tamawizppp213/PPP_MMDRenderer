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
	bool Initialize();
	bool OnResize() override;
	bool Draw()     override;

	void UpdateWeightsTable(float sigma);
	void UpdateBlurSampling(int xBlurWidthDivision, int xBlurHeightDivision, int yBlurWidthDivision, int yBlurHeightDivision); // substitute for division count;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Blur()                       = default;
	Blur(const Blur&)            = default;
	Blur& operator=(const Blur&) = default;
	Blur(Blur&&)                 = default;
	Blur& operator=(Blur&&)      = default;
	~Blur() = default;

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	bool PrepareResources();
	bool PrepareDescriptors();
	bool PrepareBlurParameters();
	bool PrepareTextureSizeBuffer();
	bool PrepareRootSignature();
	bool PreparePipelineState();

	void ExecuteFirstBarrier(DirectX12& directX12, CommandList* commandList);
	void ExecuteSecondBarrier(DirectX12& directX12, CommandList* commandList);
	void ExecuteThirdBarrier(DirectX12& directX12, CommandList* commandList);
	void ExecuteFourthBarrier(DirectX12& directX12, CommandList* commandList);
	void ExecuteFifthBarrier(DirectX12& directX12, CommandList* commandList);
	void ExecuteFinalBarrier(DirectX12& directX12, CommandList* commandList);
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