//////////////////////////////////////////////////////////////////////////////////
///             @file   Bloom.hpp
///             @brief  Post Effect Bloom ()
///             @author Toide Yutaro
///             @date   2021_07_13
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef BLOOM_HPP
#define BLOOM_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Blur.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
class Bloom final : public PostEffect
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(int width, int height);
	bool OnResize(int newWidth, int newHeight) override;
	bool Draw()     override;
	bool Draw(Resource* renderTarget, D3D12_RESOURCE_STATES renderTargetState = D3D12_RESOURCE_STATE_COMMON);
	void Finalize() override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void UpdateBloomPower(float power);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	~Bloom() {};


protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	bool PrepareResources(int width, int height);
	bool PrepareDescriptors();
	bool PrepareRootSignature();
	bool PreparePipelineState();
	
	
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	PipelineStateComPtr _luminancePipeline = nullptr;
	Blur _blur[4];
	float _explosion = 10;
};

#endif