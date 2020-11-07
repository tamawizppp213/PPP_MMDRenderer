//////////////////////////////////////////////////////////////////////////////////
//              Title:  GameTimer.hpp
//            Content:  Time Measurement Function 
//             Author:  Toide Yutaro (Reference: 3DGame Programming with DirectX12)
//             Create:  2020_11_08
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/GameTimer.hpp"
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////

GameTimer::GameTimer() :
	_secondsPerCount(0.0), _deltaTime(-1.0), _baseTime(0), _pausedTime(0),
	_previousTime(0), _currentTime(0), _stopTime(0)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	_secondsPerCount = 1.0 / (double)countsPerSec;

	_stopped = true;

}
float GameTimer::TotalTime() const
{

	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance 
	// mStopTime - mBaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from mStopTime:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime
	if (_stopped)
	{
		return (float)(((_stopTime - _pausedTime) - _baseTime) * _secondsPerCount);
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrTime:  
	//
	//  (mCurrTime - mPausedTime) - mBaseTime 
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrTime
	else
	{
		return (float)(((_currentTime - _pausedTime) - _baseTime) * _secondsPerCount);
	}
}

float GameTimer::DeltaTime() const
{
	return _deltaTime;
}

void GameTimer::Reset()
{
	__int64 currentTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	_baseTime = currentTime;
	_previousTime = currentTime;
	_stopTime = 0;
	_stopped = false;

}

void GameTimer::Start()
{
	__int64 startTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (_stopped)
	{
		_pausedTime += (startTime - _stopTime);

		_previousTime = startTime;
		_stopTime = 0;
		_stopped = false;
	}
}

void GameTimer::Stop()
{
	if (!_stopped)
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		_stopTime = currentTime;
		_stopped = true;
	}

}

void GameTimer::Tick()
{
	if (_stopped)
	{
		_deltaTime = 0.0;
		return;
	}

	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	_currentTime = currentTime;

	// Time difference between this frame and the previous.
	_deltaTime = (_currentTime - _previousTime) * _secondsPerCount;

	// Prepare for next frame.
	_previousTime = _currentTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (_deltaTime < 0.0)
	{
		_deltaTime = 0.0;
	}
}