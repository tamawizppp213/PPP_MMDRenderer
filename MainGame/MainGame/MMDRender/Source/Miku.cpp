//////////////////////////////////////////////////////////////////////////////////
//              @file   Miku.cpp
///             @brief  Miku
///             @author Toide Yutaro
///             @date   2021_11_29
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/MMDRender/Include/Miku.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                      Initialize
*************************************************************************//**
*  @fn        void Miku::Initialize()
*  @brief     Initialize
*  @param[in] const std::wstrintg& filePath
*  @return 　　void
*****************************************************************************/
bool Miku::Initialize(const std::wstring& filePath, const std::wstring& addName)
{
	if (!PMXModel::Initialize(filePath)) { return true; };
	_pbrMaterials = _pmxData.get()->CopyPBRMaterials();
	InitializeMaterialConfig();
	return true;
}
/****************************************************************************
*                      Finalize
*************************************************************************//**
*  @fn        void Miku::Finalize()
*  @brief     Finalize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Miku::Finalize()
{
	PMXModel::Finalize();
	_pbrMaterials.clear(); _pbrMaterials.shrink_to_fit();
}
/****************************************************************************
*                      UpdateGPUData
*************************************************************************//**
*  @fn        bool Miku::UpdateGPUData()
*  @brief     Update GPU Resources
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Miku::UpdateGPUData()
{
	/*-------------------------------------------------------------------
	-               Update Default PMXModel Infomation
	---------------------------------------------------------------------*/
	PMXModel::UpdateGPUData();
	/*-------------------------------------------------------------------
	-                  Update Material Buffer
	---------------------------------------------------------------------*/
	_materialBuffer.get()->CopyStart();
	for (int i = 0; i < GetPMXData()->GetMaterialCount(); ++i)
	{
		_materialBuffer.get()->CopyData(i, _pbrMaterials[i]);
	}
	_materialBuffer.get()->CopyEnd();
	return true;
}
/****************************************************************************
*                      InitializeMaterialConfig
*************************************************************************//**
*  @fn        void Miku::InitializeMaterialConfig()
*  @brief     Adjust Miku Material  
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Miku::InitializeMaterialConfig()
{
	/*-------------------------------------------------------------------
	-                      body01
	---------------------------------------------------------------------*/
	_pbrMaterials[0].Roughness         = 0.433f;
	_pbrMaterials[0].EmissiveIntensity = 3.9f;

	/*-------------------------------------------------------------------
	-                      Face
	---------------------------------------------------------------------*/
	_pbrMaterials[1].EmissiveIntensity = 0.6f;
	_pbrMaterials[4].EmissiveIntensity = 0.45f;
	_pbrMaterials[1].Roughness         = 0.38f;

	/*-------------------------------------------------------------------
	-                   Around Eye
	---------------------------------------------------------------------*/
	_pbrMaterials[16].Metalness = 0.15f;

	/*-------------------------------------------------------------------
	-                    Hair
	---------------------------------------------------------------------*/
	_pbrMaterials[19].EmissiveIntensity = 8;
	_pbrMaterials[20].EmissiveIntensity = 8;
	_pbrMaterials[22].EmissiveIntensity = 8;
	_pbrMaterials[19].Roughness = 0.3f;
	_pbrMaterials[20].Roughness = 0.3f;
	_pbrMaterials[22].Roughness = 0.3f;

	/*-------------------------------------------------------------------
	-                     Cloths
	---------------------------------------------------------------------*/
	_pbrMaterials[11].Metalness = 0.04f;
	_pbrMaterials[11].Roughness = 0.35f;
	_pbrMaterials[13].Metalness = 0.04f;
	_pbrMaterials[13].Roughness = 0.35f;
	_pbrMaterials[14].Metalness = 0.04f;
	_pbrMaterials[14].Roughness = 0.35f;
	_pbrMaterials[24].Metalness = 1;
	_pbrMaterials[24].Roughness = 0.05f;
	_pbrMaterials[26].Metalness = 1;
	_pbrMaterials[26].Roughness = 0.05f;
	_pbrMaterials[28].Metalness = 1;
	_pbrMaterials[28].Roughness = 0.05f;
	_pbrMaterials[28].EmissiveIntensity = 64;
	_pbrMaterials[18].Metalness = 1;
	_pbrMaterials[18].Roughness = 0.05f;
	_pbrMaterials[18].EmissiveIntensity = 60;
	_pbrMaterials[29].Metalness         = 1;
	_pbrMaterials[29].Roughness         = 0.05f;
	_pbrMaterials[29].EmissiveIntensity = 60;
}