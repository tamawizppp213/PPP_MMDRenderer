//////////////////////////////////////////////////////////////////////////////////
///             @file   Blur.hpp
///             @brief  Post Effect Blur (use compute shader, Gaussian blur)
///             @author Toide Yutaro
///             @date   2021_07_13
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DEPTH_OF_FIELD_HPP
#define DEPTH_OF_FIELD_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Blur.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
namespace dof
{
	struct BlurParameter
	{
		float TextureSize[2];
		float Radius;
	};

	struct DofClipSize
	{
		float NearClip;
		float FarClip;
	};
}
//////////////////////////////////////////////////////////////////////////////////
//                            Class
//////////////////////////////////////////////////////////////////////////////////
// Gaussian Blur
class Dof final : public PostEffect 
{
	using BlurParameterBuffer = std::unique_ptr<UploadBuffer<dof::BlurParameter>>;
	using DofClipSizeBuffer = std::unique_ptr<UploadBuffer<dof::DofClipSize>>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(int width, int height);
	bool OnResize(int newWidth, int newHeight) override;
	bool Draw()     override;
	bool Draw(Resource* renderTarget, Resource* zPrepass, D3D12_RESOURCE_STATES renderTargetState = D3D12_RESOURCE_STATE_COMMON);
	void Finalize() override;
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void UpdateBlurEffectParameter(int width, int height, float radius = 8.0f);
	void UpdateDofClipSize(float nearClip = 0.2f, float farClip = 0.5f);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Dof()                      = default;
	Dof(const Dof&)            = default;
	Dof& operator=(const Dof&) = default;
	Dof(Dof&&)                 = default;
	Dof& operator=(Dof&&)      = default;
	~Dof() = default;

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	bool PrepareResources(int width , int height);
	bool PrepareDescriptors();
	bool PrepareRootSignature();
	bool PreparePipelineState();
	bool PrepareBlurConstantBuffer(int width, int height, float radius = 8.0f);
	bool PrepareDofConfigBuffer(float nearClip = 0.2f, float farClip = 0.5f);

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	PipelineStateComPtr _verticalBlurPipeline = nullptr;
	PipelineStateComPtr _rhomboidBlurPipeline = nullptr;
	BlurParameterBuffer _blurBuffer;
	DofClipSizeBuffer   _dofBuffer;
};
#endif