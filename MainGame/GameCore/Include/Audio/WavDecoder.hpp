//////////////////////////////////////////////////////////////////////////////////
///             @file   WaveFile.hpp
///             @brief  Wave File Write and Read
///             @author Toide Yutaro
///             @date   2020_12_11
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef WAVE_FILE_HPP
#define WAVE_FILE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <Windows.h>
#include <memory>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/*************************************************************************//**
*  @class     WavFile
*  @brief     Wave File write and read
*****************************************************************************/
class WavDecoder
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool LoadFromFile(const std::wstring& filePath);

	const WAVEFORMATEX&            GetFileFormatEx() const;
	const std::wstring&            GetFilePath()     const;
	const std::shared_ptr<BYTE[]>& GetWaveData()     const;
	const size_t                   GetWaveSize()     const;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	WavDecoder();
	~WavDecoder() = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool Open(const std::wstring& filePath);
	bool CreateWaveFormatEx(size_t formatChunk);
	bool CreateWaveData(size_t dataSize);
	bool CheckChunk(MMCKINFO* chunkNext, MMCKINFO* chunkPrev, char c0, char c1, char c2, char c3, UINT flag);
	bool Close();
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	HMMIO _handle                     = nullptr;
	std::shared_ptr<BYTE[]> _waveData = nullptr;
	size_t _waveDataSize              = 0;
	std::wstring _filePath            = L"";
	WAVEFORMATEX _waveFormatEx;
};

#endif