//////////////////////////////////////////////////////////////////////////////////
///             @file   AudioReSource.cpp
///             @brief  AudioReSource.cpp
///             @author Toide Yutaro
///             @date   2021_01_03
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Audio/AudioClip.hpp"
#include "GameCore/Include/Audio/AudioTable.hpp"
#include "GameCore/Include/Audio/WavDecoder.hpp"
#include "GameCore/Include/FileUtility.hpp"


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
AudioClip::AudioClip()
{
	ZeroMemory(&_waveFormatEx, sizeof(_waveFormatEx));
}

AudioClip::~AudioClip()
{

}

/****************************************************************************
*                       LoadFromFile
*************************************************************************//**
*  @fn        bool AudioClip::LoadFromFile()
*  @brief     Load wav file
*  @param[in] const std::wstring& filePath
*  @return �@�@bool
*****************************************************************************/
bool AudioClip::LoadFromFile(const std::wstring& filePath)
{
	/*-------------------------------------------------------------------
	-    Select the appropriate sound loading function for each extension
	---------------------------------------------------------------------*/
	std::wstring extension = file::GetExtension(filePath);
	if (extension == L"wav")
	{
		WavDecoder wavDecoder;
		wavDecoder.LoadFromFile(filePath);
		_waveFormatEx = wavDecoder.GetFileFormatEx();
		_filePath     = wavDecoder.GetFilePath();
		_soundSize    = wavDecoder.GetWaveSize();
		_soundData    = wavDecoder.GetWaveData();

		return true;
	}
	else
	{
		MessageBox(NULL, L"The file format cannot be read.", L"Warning", MB_ICONWARNING);
		return false;
	}
}
/****************************************************************************
*                       Destroy
*************************************************************************//**
*  @fn        bool AudioClip::Destroy()
*  @brief     Destroy Audio Data
*  @param[in] void
*  @return �@�@void
*****************************************************************************/
void AudioClip::Destroy()
{
	
}

/****************************************************************************
*                       GetFilePath
*************************************************************************//**
*  @fn        const std::wstring& AudioClip::GetFilePath() const
*  @brief     Return file path
*  @param[in] void
*  @return    std::wstring&
*****************************************************************************/
const std::wstring& AudioClip::GetFilePath() const
{
	return _filePath;
}

/****************************************************************************
*                       GetSoundSize
*************************************************************************//**
*  @fn        const size_t AudioClip::GetSoundSize() const
*  @brief     Return sound size (only waveData Byte Size)
*  @param[in] void
*  @return    size_t
*****************************************************************************/
const size_t AudioClip::GetSoundSize() const
{
	return _soundSize;
}

/****************************************************************************
*                       GetSoundSize
*************************************************************************//**
*  @fn        const size_t AudioClip::GetSoundSize() const
*  @brief     Return sound size (only waveData Byte Size)
*  @param[in] void
*  @return    size_t
*****************************************************************************/
const WAVEFORMATEX& AudioClip::GetFileFormatEx() const
{
	return _waveFormatEx;
}
/****************************************************************************
*                       GetSoundData
*************************************************************************//**
*  @fn        const size_t AudioClip::GetSoundData() const
*  @brief     Return file path
*  @param[in] void
*  @return    const std::shared_ptr<BYTE[]>&
*****************************************************************************/
const std::shared_ptr<BYTE[]>& AudioClip::GetSoundData() const
{
	if (_soundData == nullptr)
	{
		MessageBox(NULL, L"_soundData is nullptr", L"Warning", MB_ICONWARNING);
	}
	return _soundData;
}
