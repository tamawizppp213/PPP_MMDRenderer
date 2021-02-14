//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12PMDFile.cpp
///             @brief  PMDFile 
///             @author Toide Yutaro
///             @date   2020_12_
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/DirectX12PMDFile.hpp"
#include "DirectX12/Include/Core/DirectX12Debug.hpp"
#include <iostream>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
void PMDData::Load3DModel(const std::wstring& filePath)
{
	using namespace pmd;

	/*-------------------------------------------------------------------
	-             Open File
	---------------------------------------------------------------------*/
	FILE* filePtr = OpenFile(filePath);

	/*-------------------------------------------------------------------
	-             Load Header Data
	---------------------------------------------------------------------*/
	fread_s(&_header, sizeof(_header), sizeof(pmd::PMDHeader), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Vertex Data
	---------------------------------------------------------------------*/
	fread_s(&_vertexCount, sizeof(_vertexCount), sizeof(UINT32), 1, filePtr);
	_vertices.resize(_vertexCount);
	fread_s(_vertices.data(), sizeof(pmd::PMDVertex) * _vertices.size(), sizeof(pmd::PMDVertex) * _vertices.size(), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Index Data
	---------------------------------------------------------------------*/
	fread_s(&_indexCount, sizeof(_indexCount), sizeof(UINT32), 1, filePtr);
	_indices.resize(_indexCount);
	fread_s(_indices.data(), _indices.size(), _indices.size(), 1, filePtr);

	
	/*-------------------------------------------------------------------
	-             Load Material Data
	---------------------------------------------------------------------*/
	//fread_s(&_materialCount, sizeof(_materialCount), sizeof(UINT32), 1, filePtr);
	//_materials.resize(_materialCount);
	//fread_s(_materials.data(), sizeof(PMDMaterial), sizeof(PMDMaterial), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Close File
	---------------------------------------------------------------------*/
	fclose(filePtr);
}


#pragma region Property
UINT32 PMDData::GetVertexCount()
{
	return this->_vertexCount;
}

UINT32 PMDData::GetIndexCount()
{
	return this->_indexCount;
}

UINT32 PMDData::GetMaterialCount()
{
	return this->_materialCount;
}

#pragma endregion Property
#pragma endregion Public Function

#pragma region Protected Function

#pragma endregion Protected Function

#pragma region Private Function

#pragma endregion Private Functoon