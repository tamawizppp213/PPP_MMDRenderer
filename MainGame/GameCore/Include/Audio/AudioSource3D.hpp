//////////////////////////////////////////////////////////////////////////////////
///             @file   AudioSource.hpp
///             @brief  AudioSource.hpp
///             @author Toide Yutaro
///             @date   2021_01_13
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef AUDIO_SOURCE_3D_HPP
#define AUDIO_SOURCE_3D_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "AudioCore.hpp"
#include "AudioSource.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			AudioListener
*************************************************************************//**
*  @struct     AudioListener
*  @brief     Audio Listener (for 3D sound)
*****************************************************************************/
struct AudioListener
{
	DirectX::XMFLOAT3 Front;
	DirectX::XMFLOAT3 Up;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Velocity;
};

/****************************************************************************
*				  			AudioEmitter
*************************************************************************//**
*  @struct    AudioEmitter
*  @brief     Audio Emitter (for 3D sound)
*****************************************************************************/
struct AudioEmitter : AudioListener
{
	float InnerRadius;
	float InnerRadiusAngle;

	UINT32 ChannelCount;
	float ChannelRadius;

	float CurveDistanceScaler;
	float DopplerScaler;
};

/****************************************************************************
*				  			Audio3DParameter
*************************************************************************//**
*  @struct    Audio3DParameter
*  @brief     struct for 3D sound play
*****************************************************************************/
struct Audio3DParameter
{
	int   SourceChannel;
	int   DestinationChannel;
	float Matrix[12];
	float Doppler;
	float EmitterToListenerAngle;
	float EmitterToListenerDistance;
	float EmitterVelocity;
	float ListenerVelocity;
	float ReverbLevel;
	float LPFReverbCoefficient;
	float LPFDirectCoefficient;
};

/****************************************************************************
*				  			AudioSource
*************************************************************************//**
*  @class     AudioSource
*  @brief     Audio Source (for 3D sound)
*****************************************************************************/
class AudioSource3D : public AudioSource
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual bool Play() override;
	bool ApplyChange();

	static Audio3DParameter& Calculate3DSound(
		const X3DAUDIO_LISTENER* listener,
		const X3DAUDIO_EMITTER* emitter);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetListenerFront(const DirectX::XMFLOAT3& front);
	void SetListenerUp(const DirectX::XMFLOAT3& top);
	void SetListenerPosition(const DirectX::XMFLOAT3& position);
	void SetListenerVelocity(const DirectX::XMFLOAT3& velocity);

	void SetEmitterFront(const DirectX::XMFLOAT3& front);
	void SetEmitterUp(const DirectX::XMFLOAT3& top);
	void SetEmitterPosition(const DirectX::XMFLOAT3& position);
	void SetEmitterVelocity(const DirectX::XMFLOAT3& velocity);
	void SetEmitterDopplerLevel(float level);
	void SetEmitterRadius(float radius);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	AudioSource3D();
	~AudioSource3D();
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	AudioEmitter  _emitter;
	AudioListener _listener;
	bool _using3DSound = true;
};

#endif
