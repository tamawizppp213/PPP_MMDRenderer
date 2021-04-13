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

/****************************************************************************
*                       Load3DModel
*************************************************************************//**
*  @fn        bool FBXData::Load3DModel(const std::wstring& filePath)
*  @brief     Load fbx 3d model
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
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

	/*-------------------------------------------------------------------
	-             Open FBX File
	---------------------------------------------------------------------*/
	if(!SetFBXConfig()){ MessageBox(NULL, L" The fbx config couldn't be set.", L"Warning", MB_ICONWARNING); }
	return true;
}


#pragma region Property


#pragma endregion Property
#pragma endregion Public Function

#pragma region Protected Function

#pragma endregion Protected Function

#pragma region Private Function
/****************************************************************************
*                       CreateFBXManager
*************************************************************************//**
*  @fn        bool FBXData::CreateFBXManager()
*  @brief     Create FBX Manager Class
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool FBXData::CreateFBXManager()
{
	/*-------------------------------------------------------------------
	-             Create Manager
	---------------------------------------------------------------------*/
	_fbxManager = FbxManager::Create();

	/*-------------------------------------------------------------------
	-             Nullptr Check
	---------------------------------------------------------------------*/
	if (_fbxManager == nullptr) { return false; }
	return true;
}

/****************************************************************************
*                       CreateFBXImporter
*************************************************************************//**
*  @fn        bool FBXData::CreateFBXImporter()
*  @brief     Create FBX Importer (fbx parser class)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool FBXData::CreateFBXImporter()
{
	/*-------------------------------------------------------------------
	-             Create Importer
	---------------------------------------------------------------------*/
	_fbxImporter = FbxImporter::Create(_fbxManager, "imp");

	/*-------------------------------------------------------------------
	-             Nullptr Check
	---------------------------------------------------------------------*/
	if (_fbxImporter == nullptr)
	{
		_fbxManager->Destroy();
		return false;
	}

	return true; 
}

/****************************************************************************
*                       CreateFBXScene
*************************************************************************//**
*  @fn        bool FBXData::CreateFBXScene()
*  @brief     Create FBX Scene (state after disassembly)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool FBXData::CreateFBXScene()
{
	/*-------------------------------------------------------------------
	-             Get FBX Scene
	---------------------------------------------------------------------*/
	_fbxScene    = FbxScene::Create(_fbxManager, "fbxscene");

	/*-------------------------------------------------------------------
	-             Nullptr Check
	---------------------------------------------------------------------*/
	if (_fbxScene == nullptr)
	{
		_fbxImporter->Destroy();
		_fbxManager->Destroy();
		return false;
	}
	return true;
}

/****************************************************************************
*                       OpenFBXFile
*************************************************************************//**
*  @fn        bool FBXData::OpenFBXFile(const std::wstring& filePath)
*  @brief     Open File
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
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

/****************************************************************************
*                       SetFBXConfig
*************************************************************************//**
*  @fn        bool FBXData::SetFBXConfig
*  @brief     Set fbx config (Convert DirectX coordinate)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool FBXData::SetFBXConfig()
{
	/*-------------------------------------------------------------------
	-             Righthand coordinate -> Left handed coordinate
	---------------------------------------------------------------------*/
	FbxAxisSystem axisSystem = _fbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::DirectX.ConvertScene(_fbxScene);

	/*-------------------------------------------------------------------
	-             Convert Mesh Triangle
	---------------------------------------------------------------------*/
	FbxGeometryConverter converter(_fbxManager);
	converter.Triangulate(_fbxScene, true);

	/*-------------------------------------------------------------------
	-             Get RootNode
	---------------------------------------------------------------------*/
	_fbxRootNode = _fbxScene->GetRootNode();

	
	return true;
}

bool FBXData::LoadFBXVertex()
{
	std::map<std::string, FbxNode*> nodeList;
	CollectMeshNode(_fbxRootNode, nodeList);

	
	return true;
}

/****************************************************************************
*                       CollectMeshNode
*************************************************************************//**
*  @fn        void FBXData::CollectMeshNode(FbxNode* node, std::map<std::string, FbxNode*>& nodeList)
*  @brief     Add mesh node
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void FBXData::CollectMeshNode(FbxNode* node, std::map<std::string, FbxNode*>& nodeList)
{
	/*-------------------------------------------------------------------
	-             Check whether the nodeType is eMesh. 
	---------------------------------------------------------------------*/
	for (int i = 0; i < node->GetNodeAttributeCount(); ++i)
	{
		FbxNodeAttribute* attribute = node->GetNodeAttributeByIndex(i);

		if (attribute->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
		{
			nodeList[node->GetName()] = node;
			break;
		}
	}

	/*-------------------------------------------------------------------
	-             Recursive Function
	---------------------------------------------------------------------*/
	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		CollectMeshNode(node->GetChild(i), nodeList);
	}
}
#pragma endregion Private Functoon