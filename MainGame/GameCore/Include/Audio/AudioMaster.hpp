//////////////////////////////////////////////////////////////////////////////////
///             @file   AudioMaster.hpp
///             @brief  AudioMaster.hpp
///             @author Toide Yutaro
///             @date   2021_01_03
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef AUDIO_MASTER_HPP
#define AUDIO_MASTER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "AudioTable.hpp"
#include "AudioCore.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			SoundManager
*************************************************************************//**
*  @class     SoundManager
*  @brief     temp
*****************************************************************************/
class AudioMaster
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize();
	void Finalize();
	void ClearSound();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	IXAudio2* GetAudioInterface() const;
	IXAudio2MasteringVoice* GetMasteringVoice() const;
	X3DAUDIO_HANDLE& GetX3DAudioInterface() const;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static AudioMaster& Instance()
	{
		static AudioMaster audioMaster;
		return audioMaster;
	}
	AudioMaster(const AudioMaster&)            = delete;
	AudioMaster& operator=(const AudioMaster&) = delete;
	AudioMaster(AudioMaster&&)                 = delete;
	AudioMaster& operator=(AudioMaster&&)      = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	AudioMaster()  = default;
	~AudioMaster() = default;

	bool CreateXAudio2();
	bool CreateMasterVoice();
	bool CreateX3DAudio();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	static IXAudio2* _xAudio;
	static IXAudio2MasteringVoice* _masterVoice;
	static X3DAUDIO_HANDLE _x3dAudio;

	UINT32 _debugFlag = 0;

	AudioTableManager& _audioTable = AudioTableManager::Instance();
};
#endif