//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12PMDFile.hpp
///             @brief  PMDFile Loader
///             @author Toide Yutaro
///             @date   2020_12_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_PMD_FILE_HPP
#define DIRECTX12_PMD_FILE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12PMDConfig.hpp"
#include "DirectX12ModelFile.hpp"
#include <d3d12.h>
#include <vector>
#include <iostream>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			PMDFile
*************************************************************************//**
*  @class     PMDData
*  @brief     PMD Data
*****************************************************************************/
class PMDData final : public ModelData
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Load3DModel(const std::wstring& filePath) override;
	
#pragma region Property
	const std::vector<PMDVertex>& GetVertex() const { return _vertices; }
	const PMDVertex& GetVertex(int index)     const { return _vertices[index]; }
	UINT32 GetVertexCount()   override;
	UINT32 GetIndexCount()    override;
	UINT32 GetMaterialCount() override;
#pragma endregion Property
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	pmd::PMDHeader                _header;
	std::vector<PMDVertex>        _vertices;
	std::vector<UINT16>           _indices;
	std::vector<pmd::PMDMaterial> _materials;

};

#endif