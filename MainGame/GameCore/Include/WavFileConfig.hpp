//////////////////////////////////////////////////////////////////////////////////
///             @file   WavFileConfig.hpp
///             @brief  Wav File Config
///             @author Toide Yutaro
///             @date   2020_12_11
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef WAVE_FILE_CONFIG_HPP
#define WAVE_FILE_CONFIG_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <Windows.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define GROUND    32768.0
#define MAX_PLUS  65536.0
#define MAX_MINUS 0.0
#define MONO   1
#define STEREO 2


namespace audio
{
	/****************************************************************************
	*				  			MONO_PCM
	*************************************************************************//**
	*  @struct    MONO_PCM
	*  @brief     PCM data
	*****************************************************************************/
	struct MONO_PCM
	{
		UINT32  samplingFreq; 
		UINT32  bits;
		UINT32  dataLength;
		double* waveData;
	};

	/****************************************************************************
	*				  			Riff Chunk
	*************************************************************************//**
	*  @struct    RIFF_CHUNK
	*  @brief     Riff chunk for wav file loading
	*****************************************************************************/
	struct RIFF_CHUNK
	{
		UINT8  chunkID[4];
		UINT32 chunkSize;
		UINT8  chunkFormType[4];
	};

	/****************************************************************************
	*				  			FMT Chunk
	*************************************************************************//**
	*  @struct    FMT_CHUNK
	*  @brief     Fmt chunk for wav file loading
	*****************************************************************************/
	struct FMT_CHUNK
	{
		UINT8   chunkID[4];
		UINT32  chunkSize;
		UINT16  waveFormatType;
		UINT16  formatChannel;
		UINT32  samplesPerSecond;
		UINT32  bytesPerSecond;
		UINT16  blockSize;
		UINT16  bitsPerSample;
	};

	/****************************************************************************
	*				  			DATA Chunk
	*************************************************************************//**
	*  @struct    DATA_CHUNK
	*  @brief     Data chunk for wav file loading
	*****************************************************************************/
	struct DATA_CHUNK
	{
		UINT8  chunkID[4];
		UINT32 chunkSize;
		UINT16 data;
	};

	/****************************************************************************
	*				  			WAVE_FORMAT
	*************************************************************************//**
	*  @struct    WAVE_FORMAT
	*  @brief     wav file format
	*****************************************************************************/
	struct WAVE_FORMAT
	{
		RIFF_CHUNK riffChunk;
		FMT_CHUNK  fmtChunck;
		DATA_CHUNK dataChunk;
	};

}
#endif