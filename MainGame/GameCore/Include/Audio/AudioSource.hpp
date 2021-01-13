//////////////////////////////////////////////////////////////////////////////////
///             @file   AudioSource.hpp
///             @brief  Default Audio class (for 2D)
///             @author Toide Yutaro
///             @date   2020_01/12
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef AUDIO_SOURCE_HPP
#define AUDIO_SOURCE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "AudioCore.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/*************************************************************************//**
*  @enum      SoundType
*****************************************************************************/
enum class SoundType
{
	BGM,
	BGS,
	ME,
	SE,
	NONE
};

/*************************************************************************//**
*  @class     AudioDefault
*  @brief     Audio Play Base Class
*****************************************************************************/
class AudioSource
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual bool Play();
	bool Stop();
	bool Replay();
	bool Pause();
	bool ExitLoop();
	bool LoadSound(const std::wstring& filePath, SoundType soundType, float volume = 1.0f);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool  IsLoop()    const;
	bool  IsPlaying();

#pragma region Pitch
	float GetPitch();
	void  SetPitch(float pitch);
	void  SetMaxPitch(float maxPitch);
	void  AdjustPitch(float diffPitch);
#pragma endregion Pitch

#pragma region Volume
	float GetVolume();
	bool  SetVolume(float volume);
	void  AdjustVolume(float diffVolume);
#pragma endregion Volume

	// In case 3D Audio, this parameter is invalid.
	bool SetPan(float pan);
	bool IsUseReverb(bool isReverb);
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	AudioSource();
	~AudioSource();
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	void FlushAudioData();
	bool IsExistedSourceVoice();
	bool IsRemainedSourceBufferQueue();
	bool LoadAudioClip(const std::wstring& filePath);
	bool CreateSourceVoice();
	bool CreateReverb();
	bool SelectSoundType(SoundType soundType);

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	std::shared_ptr<AudioClip>_audioClip = nullptr;
	IXAudio2SourceVoice* _sourceVoice = nullptr;
	SoundType _soundType = SoundType::NONE;
	bool  _isLoop;
	float _maxVolume;
	float _maxPitch;
	static float _volumeRatio[4];

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif