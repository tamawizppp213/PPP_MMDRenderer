//////////////////////////////////////////////////////////////////////////////////
///             @file   VMDFile.cpp
///             @brief  VMDFile 
///             @author Toide Yutaro
///             @date   2021_02_28
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/MMD/VMDFile.hpp"
#include "GameCore/Include/File/FileUtility.hpp"
#include <filesystem>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*							LoadVMDFile
*************************************************************************//**
*  @fn        bool VMDFile::LoadVMDFile(const std::wstring& filePath)
*  @brief     Load VMD File
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VMDFile::LoadVMDFile(const std::wstring& filePath)
{
	using namespace vmd;

	/*-------------------------------------------------------------------
	-             Open File
	---------------------------------------------------------------------*/
	FILE* filePtr = file::OpenFile(filePath);
	std::filesystem::path directory = filePath;
	_directory = directory.parent_path().string();

	/*-------------------------------------------------------------------
	-             Load Header Data
	---------------------------------------------------------------------*/
	if (!LoadVMDHeader(filePtr)) { MessageBox(NULL, L" VMDHeader cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load Motion Data
	---------------------------------------------------------------------*/
	if (!LoadVMDMotion(filePtr)) { MessageBox(NULL, L" VMDMotion cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load Header Data
	---------------------------------------------------------------------*/
	if (!LoadVMDBlendShape(filePtr)) { MessageBox(NULL, L" VMDBlendShape cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load Camera Data
	---------------------------------------------------------------------*/
	if (!LoadVMDCamera(filePtr)) { MessageBox(NULL, L" VMDCamera cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load Light Data
	---------------------------------------------------------------------*/
	if (!LoadVMDLight(filePtr)) { MessageBox(NULL, L" VMDLight cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load Shadow Data
	---------------------------------------------------------------------*/
	if (!LoadVMDShadow(filePtr)) { MessageBox(NULL, L" VMDShadow cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load IK Data
	---------------------------------------------------------------------*/
	if (!LoadVMDIK(filePtr)) { MessageBox(NULL, L" VMDIK cannot be read.", L"Warning", MB_ICONWARNING); };
	return true;
}
#pragma endregion Public Function

#pragma region Private Function
/****************************************************************************
*							LoadVMDHeader
*************************************************************************//**
*  @fn        bool VMDFile::LoadVMDHeader(FILE* file)
*  @brief     Load VMD Header
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VMDFile::LoadVMDHeader(FILE* filePtr)
{
	fread_s(&_header, sizeof(_header), sizeof(vmd::VMDHeader), 1, filePtr);
	return true;
}

/****************************************************************************
*							LoadVMDMotion
*************************************************************************//**
*  @fn        bool VMDFile::LoadVMDMotion(FILE* file)
*  @brief     Load VMD Motion
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VMDFile::LoadVMDMotion(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Motion Count
	---------------------------------------------------------------------*/
	UINT32 motionCount = 0;
	fread_s(&motionCount, sizeof(motionCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Motion Data
	---------------------------------------------------------------------*/
	_motions.resize(motionCount);
	for (auto& motion : _motions)
	{
		fread_s(&motion, sizeof(motion), sizeof(vmd::VMDMotion), 1, filePtr);
	}
	return true;
}

/****************************************************************************
*							LoadVMDBlendShape
*************************************************************************//**
*  @fn        bool VMDFile::LoadVMDBlendShape(FILE* file)
*  @brief     Load VMD BlendShape
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VMDFile::LoadVMDBlendShape(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load BlendShape Count
	---------------------------------------------------------------------*/
	UINT32 blendShapeCount = 0;
	fread_s(&blendShapeCount, sizeof(blendShapeCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load BlendShape Data
	---------------------------------------------------------------------*/
	_morphs.resize(blendShapeCount);
	for (auto& morph : _morphs)
	{
		fread_s(&morph, sizeof(morph), sizeof(vmd::VMDMorph), 1, filePtr);
	}
	return true;
}

/****************************************************************************
*							LoadVMDCamera
*************************************************************************//**
*  @fn        bool VMDFile::LoadVMDCamera(FILE* file)
*  @brief     Load VMD Camera
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VMDFile::LoadVMDCamera(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Camera Count
	---------------------------------------------------------------------*/
	UINT32 cameraCount = 0;
	fread_s(&cameraCount, sizeof(cameraCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Camera Data
	---------------------------------------------------------------------*/
	_cameras.resize(cameraCount);
	for (auto& camera : _cameras)
	{
		fread_s(&camera, sizeof(camera), sizeof(vmd::VMDCamera), 1, filePtr);
	}
	return true;
}

/****************************************************************************
*							LoadVMDLight
*************************************************************************//**
*  @fn        bool VMDFile::LoadVMDLight(FILE* file)
*  @brief     Load VMD Light
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VMDFile::LoadVMDLight(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Light Count
	---------------------------------------------------------------------*/
	UINT32 lightCount = 0;
	fread_s(&lightCount, sizeof(lightCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Light Data
	---------------------------------------------------------------------*/
	_lights.resize(lightCount);
	for (auto& light : _lights)
	{
		fread_s(&light, sizeof(light), sizeof(vmd::VMDLight), 1, filePtr);
	}
	return true;
}

/****************************************************************************
*							LoadVMDShadow
*************************************************************************//**
*  @fn        bool VMDFile::LoadVMDShadow(FILE* file)
*  @brief     Load VMD Shadow
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VMDFile::LoadVMDShadow(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Shadow Count
	---------------------------------------------------------------------*/
	UINT32 shadowCount = 0;
	fread_s(&shadowCount, sizeof(shadowCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Shadow Data
	---------------------------------------------------------------------*/
	_shadows.resize(shadowCount);
	for (auto& shadow : _shadows)
	{
		fread_s(&shadow, sizeof(shadow), sizeof(vmd::VMDShadow), 1, filePtr);
	}
	return true;
}

/****************************************************************************
*							LoadVMDIK
*************************************************************************//**
*  @fn        bool VMDFile::LoadVMDIK(FILE* file)
*  @brief     Load VMD IK
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool VMDFile::LoadVMDIK(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load IK Count
	---------------------------------------------------------------------*/
	UINT32 ikCount = 0;
	fread_s(&ikCount, sizeof(ikCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load IK Data
	---------------------------------------------------------------------*/
	_iks.resize(ikCount);
	for (auto& ik : _iks)
	{
		/*-------------------------------------------------------------------
		-             Load Frame, and IsShow Data
		---------------------------------------------------------------------*/
		fread_s(&ik.Frame , sizeof(ik.Frame) , sizeof(UINT32), 1, filePtr);
		fread_s(&ik.IsShow, sizeof(ik.IsShow), sizeof(UINT8) , 1, filePtr);

		/*-------------------------------------------------------------------
		-             Load IK Info Count
		---------------------------------------------------------------------*/
		UINT32 ikInfoCount = 0;
		fread_s(&ikInfoCount, sizeof(ikInfoCount), sizeof(UINT32), 1, filePtr);

		/*-------------------------------------------------------------------
		-             Load IK Infomation 
		---------------------------------------------------------------------*/
		ik.IKInfo.resize(ikInfoCount);
		for (auto& ikInfo : ik.IKInfo)
		{
			fread_s(&ikInfo.IKName   , sizeof(ikInfo.IKName)   , sizeof(char) , 20, filePtr);
			fread_s(&ikInfo.IsEnabled, sizeof(ikInfo.IsEnabled), sizeof(UINT8), 1, filePtr);
		}
	}
	return true;
}
#pragma endregion Private Function