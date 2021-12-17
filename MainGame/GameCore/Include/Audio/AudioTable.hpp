//////////////////////////////////////////////////////////////////////////////////
///             @file   AudioTable.hpp
///             @brief  Audio Table (Singleton)
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef AUDIO_TABLE_HPP
#define AUDIO_TABLE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "AudioClip.hpp"
#include <unordered_map>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  		AudioTable (Singleton)
*************************************************************************//**
*  @class     AudioTable
*  @brief     Model Table
*****************************************************************************/
class AudioTableManager
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void ClearAudioTable() { AudioTable.clear(); }

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	std::unordered_map<std::wstring, std::shared_ptr<AudioClip>> AudioTable;

	/***************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static AudioTableManager& Instance()
	{
		static AudioTableManager audioTable;
		return audioTable;
	}
	// Prohibit move and copy.
	AudioTableManager(const AudioTableManager&)            = delete;
	AudioTableManager& operator=(const AudioTableManager&) = delete;
	AudioTableManager(AudioTableManager&&)                 = delete;
	AudioTableManager& operator=(AudioTableManager&&)      = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	AudioTableManager()  = default;
	~AudioTableManager() = default;

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

};

#endif