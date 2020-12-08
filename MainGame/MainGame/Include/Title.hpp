//////////////////////////////////////////////////////////////////////////////////
//              Title:  Title.hpp
//            Content:  Title Scene 
//             Author:  Toide Yutaro
//             Create:  2020_11_06
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef TITLE_HPP
#define TITLE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "Scene.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class Title final : public Scene
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	Title();
	~Title();
	void Initialize(const DirectX12& directX12) override;
	void Draw()       override;
	void Update()     override;
	void Terminate()  override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

protected:
	void LoadShaders()             override;
	void LoadGeometry()            override;
	void BuildPSOs()               override;
	void BuildRootSignature()      override;
	void BuildRenderItem()         override;
	void BuildFrameResources()     override;
	void BuildDescriptorHeap()     override;
	void BuildConstantBufferView() override;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	DirectX12 _directX12;

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/



};
#endif