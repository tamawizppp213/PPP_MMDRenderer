//////////////////////////////////////////////////////////////////////////
///             @file   DirectX12ModelLoader.hpp
///             @brief  Load Model
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/MMD/VMDFile.hpp"
#include "GameCore/Include/Model/MotionLoader.hpp"
#include "GameCore/Include/File/FileUtility.hpp"
#include <stdexcept>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*                       Load3DModel
*************************************************************************//**
*  @fn        void ModelLoader::Load3DModel(const std::wstring& filePath, std::shared_ptr<PMDData>* pmdData)
*  @brief     LoadPMDData
*  @param[in] const std::wstring& filePath
*  @param[out]std::shared_ptr<PMDData>* pmdData
*  @return Å@Å@void
*****************************************************************************/
void MotionLoader::LoadMotion(const std::wstring& filePath, std::shared_ptr<VMDFile>* vmdData)
{
	/*-------------------------------------------------------------------
	-               If the file is loaded once, read from it
	---------------------------------------------------------------------*/
	if (_motionTableManager.Instance().MotionTableVMD.find(filePath) != _motionTableManager.Instance().MotionTableVMD.end())
	{
		*vmdData = _motionTableManager.Instance().MotionTableVMD[filePath];
		return;
	}

	/*-------------------------------------------------------------------
	-       Choose extension and load 3Dmodel data
	---------------------------------------------------------------------*/
	std::wstring extension = file:: GetExtension(filePath);
	if (extension == L"vmd")
	{
		// Load 3D model data (.pmd)
		std::shared_ptr<VMDFile> vmdDataPtr = std::make_shared<VMDFile>();
		vmdDataPtr->LoadVMDFile(filePath);

		// Add model table
		_motionTableManager.Instance().MotionTableVMD[filePath] = vmdDataPtr;
		*vmdData = vmdDataPtr;
	}
	else
	{
		auto error = std::invalid_argument("Invalid file path was specified.");
		std::cout << error.what() << std::endl;
	}

}

