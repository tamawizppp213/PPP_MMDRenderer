//////////////////////////////////////////////////////////////////////////////////
///             @file   AudioClip.hpp
///             @brief  Audio Clip for audio play (.wav data)
///             @author Toide Yutaro
///             @date   2021_01_12
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef AUDIO_CLIP_HPP
#define AUDIO_CLIP_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "AudioCore.hpp"


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


/****************************************************************************
*				  		AudioClip
*************************************************************************//**
*  @class     Audio Clip
*  @brief     Save audio data (current .wav only)
*****************************************************************************/
class AudioClip
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool LoadFromFile(const std::wstring& filePath);
	void Destroy();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const std::wstring&            GetFilePath()     const;
	const size_t                   GetSoundSize()    const;
	const std::shared_ptr<BYTE[]>& GetSoundData()    const;
	const WAVEFORMATEX&            GetFileFormatEx() const;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	AudioClip();
	~AudioClip();
	AudioClip(const AudioClip&) = default;
	AudioClip& operator=(const AudioClip&) = default;
	AudioClip(AudioClip&&) = default;
	AudioClip& operator=(AudioClip&&) = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::wstring _filePath             = L"";
	size_t       _soundSize            = 0;
	std::shared_ptr<BYTE[]> _soundData = nullptr;
	WAVEFORMATEX _waveFormatEx;
};


#endif
