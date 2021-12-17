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
namespace
{
	using namespace gm;
	Vector4 GetBezierParameter(int index, const vmd::VMDKeyFrame& keyFrame)
	{
		Vector4 result;
		result.SetX(float(keyFrame.Interpolation[4 * 0 + index]) / 127.0f);
		result.SetY(float(keyFrame.Interpolation[4 * 1 + index]) / 127.0f);
		result.SetZ(float(keyFrame.Interpolation[4 * 2 + index]) / 127.0f);
		result.SetW(float(keyFrame.Interpolation[4 * 3 + index]) / 127.0f);
		return result;
	}
}
#pragma region Public Function
VMDFile::~VMDFile()
{
	for (auto& motion : _motionMap)
	{
		motion.second.clear();
		motion.second.shrink_to_fit();
	}
	for (auto& morph : _morphingMap)
	{
		morph.second.clear();
		morph.second.shrink_to_fit();
	}
	_motionMap.clear();
	_cameras.clear();
	_cameras.shrink_to_fit();
	_iks.clear();
	_iks.shrink_to_fit();
	_shadows.clear();
	_shadows.shrink_to_fit();
	_lights.clear();
	_lights.shrink_to_fit();
}
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
	-             Load Motion (Key Frame)Data
	---------------------------------------------------------------------*/
	std::vector<vmd::VMDKeyFrame> keyFrames;
	keyFrames.resize(motionCount);
	
	for (auto& keyFrame : keyFrames)
	{
		fread_s(&keyFrame.BoneName     , sizeof(keyFrame.BoneName)     , sizeof(char)      , 15, filePtr);
		fread_s(&keyFrame.Frame        , sizeof(keyFrame.Frame)        , sizeof(UINT32)    , 1 , filePtr);
		fread_s(&keyFrame.Translation  , sizeof(keyFrame.Translation)  , sizeof(gm::Float3), 1 , filePtr);
		fread_s(&keyFrame.Quarternion  , sizeof(keyFrame.Quarternion)  , sizeof(gm::Float4), 1 , filePtr);
		fread_s(&keyFrame.Interpolation, sizeof(keyFrame.Interpolation), sizeof(UINT8)     , 64, filePtr);
	}

	/*-------------------------------------------------------------------
	-             Set KeyFrame Motion Map, (for Bone Rotation)
	---------------------------------------------------------------------*/
	for (auto& keyFrame : keyFrames)
	{
		std::string boneName = keyFrame.BoneName;
		_motionMap[boneName].emplace_back(
			keyFrame.Frame,
			gm::Quaternion(gm::Vector4(keyFrame.Quarternion)),
			gm::Vector3(keyFrame.Translation),
			GetBezierParameter(0, keyFrame),
			GetBezierParameter(1, keyFrame),
			GetBezierParameter(2, keyFrame),
			GetBezierParameter(3, keyFrame)
		);
		_animationDuration = max(keyFrame.Frame, _animationDuration);
	}

	/*-------------------------------------------------------------------
	-             Sort key frame data (for each bone)
	---------------------------------------------------------------------*/
	int count = 0;
	for (auto& motion : _motionMap)
	{
		std::sort(motion.second.begin(), motion.second.end(),
			[](const VMDKeyFrame& leftValue, const VMDKeyFrame& rightValue)
		{
			return leftValue.Frame < rightValue.Frame;
		});
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
	std::vector<vmd::VMDMorph> morphs;
	morphs.resize(blendShapeCount);
	for (auto& morph : morphs)
	{
		fread_s(&morph.Name  , sizeof(morph.Name  ), sizeof(char)  , 15, filePtr);
		fread_s(&morph.Frame , sizeof(morph.Frame ), sizeof(UINT32), 1 , filePtr);
		fread_s(&morph.Weight, sizeof(morph.Weight), sizeof(float) , 1 , filePtr);

		std::string morphName = morph.Name;
		_morphingMap[morphName].emplace_back(morph);
	}


	/*-------------------------------------------------------------------
	-             Sort key frame morphing data 
	---------------------------------------------------------------------*/
	int count = 0;
	for (auto& morph : _morphingMap)
	{
		std::sort(morph.second.begin(), morph.second.end(),
			[](const VMDKeyFrameMorph& leftValue, const VMDKeyFrameMorph& rightValue)
		{
			return leftValue.Frame < rightValue.Frame;
		});
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
		fread_s(&camera.Frame        , sizeof(camera.Frame)        , sizeof(UINT32)    , 1 , filePtr);
		fread_s(&camera.Distance     , sizeof(camera.Distance)     , sizeof(float)     , 1 , filePtr);
		fread_s(&camera.Interest     , sizeof(camera.Interest)     , sizeof(gm::Float3), 1 , filePtr);
		fread_s(&camera.Rotation     , sizeof(camera.Rotation)     , sizeof(gm::Float3), 1 , filePtr);
		fread_s(&camera.Interpolation, sizeof(camera.Interpolation), sizeof(UINT8)     , 24, filePtr);
		fread_s(&camera.ViewAngle    , sizeof(camera.ViewAngle)    , sizeof(UINT32)    , 1 , filePtr);
		fread_s(&camera.IsPerspective, sizeof(camera.IsPerspective), sizeof(UINT8)     , 1 , filePtr);
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
		fread_s(&light.Frame   , sizeof(light.Frame)   , sizeof(UINT32)    , 1, filePtr);
		fread_s(&light.Color   , sizeof(light.Color)   , sizeof(gm::Float3), 1, filePtr);
		fread_s(&light.Position, sizeof(light.Position), sizeof(gm::Float3), 1, filePtr);

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
	std::vector<vmd::VMDIK> iks;
	iks.resize(ikCount);
	for (auto& ik : iks)
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

		_iks.push_back(ik);
	}
	return true;
}


#pragma endregion Private Function