//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12FBXFile.cpp
///             @brief  FBXFile 
///             @author Toide Yutaro
///             @date   2020_12_
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/FBX/FBXFile.hpp"
#include "GameCore/Include/File/FileUtility.hpp"

#ifdef _DEBUG
#pragma comment(lib, "Pluguins/fbxsdk/debug/libfbxsdk-md.lib")
#pragma comment(lib, "Pluguins/fbxsdk/debug/zlib-md.lib")
#pragma comment(lib, "Pluguins/fbxsdk/debug/libxml2-md.lib")
#else
#pragma comment(lib, "Pluguins/fbxsdk/release/libfbxsdk-md.lib")
#pragma comment(lib, "Pluguins/fbxsdk/release/zlib-md.lib")
#pragma comment(lib, "Pluguins/fbxsdk/release/libxml2-md.lib")
#endif
//#pragma comment(lib, "libfbxsdk.lib")
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
FBXData::~FBXData()
{
	_fbxImporter->Destroy();
	_fbxScene->Destroy();
	_fbxManager->Destroy();
}

bool FBXData::Load3DModel(const std::wstring& filePath)
{
	/*-------------------------------------------------------------------
	-             Create FBX Manager
	---------------------------------------------------------------------*/
	if(!CreateFBXManager()){ MessageBox(NULL, L" FBX Manager coundn't be created.", L"Warning", MB_ICONWARNING); }

	/*-------------------------------------------------------------------
	-             Create FBX Importer
	---------------------------------------------------------------------*/
	if (!CreateFBXImporter()) { MessageBox(NULL, L" FBX Importer coundn't be created.", L"Warning", MB_ICONWARNING); }

	/*-------------------------------------------------------------------
	-             Create FBX Scene
	---------------------------------------------------------------------*/
	if (!CreateFBXScene()) { MessageBox(NULL, L" FBX Scene coundn't be created.", L"Warning", MB_ICONWARNING); }
	
	/*-------------------------------------------------------------------
	-             Open FBX File
	---------------------------------------------------------------------*/
	if(!OpenFBXFile(filePath)){ MessageBox(NULL, L" The file path couldn't be read.", L"Warning", MB_ICONWARNING); }


	return true;
}


#pragma region Property


#pragma endregion Property
#pragma endregion Public Function

#pragma region Protected Function

#pragma endregion Protected Function

#pragma region Private Function
bool FBXData::CreateFBXManager()
{
	_fbxManager = FbxManager::Create();
	if (_fbxManager == nullptr) 
	{ 
		return false; 
	}
	return true;
}

bool FBXData::CreateFBXImporter()
{
	_fbxImporter = FbxImporter::Create(_fbxManager, "imp");
	if (_fbxImporter == nullptr)
	{
		_fbxManager->Destroy();
		return false;
	}
	return true; 
}

bool FBXData::CreateFBXScene()
{
	_fbxScene = FbxScene::Create(_fbxManager, "fbxscene");
	if (_fbxScene == nullptr)
	{
		_fbxImporter->Destroy();
		_fbxManager->Destroy();
		return false;
	}
	return true;
}

bool FBXData::OpenFBXFile(const std::wstring& filePath)
{
	fbxsdk::FbxString fileName(file::WStringToString(filePath).c_str());
	if (!_fbxImporter->Initialize(fileName.Buffer(), -1, _fbxManager->GetIOSettings()))
	{
		MessageBox(NULL, L" FBX Importer cannot be initialized.", L"Warning", MB_ICONWARNING);
		_fbxImporter->Destroy();
		_fbxScene->Destroy();
		_fbxManager->Destroy();
		return false;
	}
	if (!_fbxImporter->Import(_fbxScene))
	{
		MessageBox(NULL, L" FBX Scene cannot be created.", L"Warning", MB_ICONWARNING);
		_fbxImporter->Destroy();
		_fbxScene->Destroy();
		_fbxManager->Destroy();
		return false;
	}
	return true;

}

bool FBXData::LoadFBXVertex()
{
	FbxGeometryConverter converter(_fbxManager);
	converter.Triangulate(_fbxScene, true);


	return true;
}
#pragma endregion Private Functoon