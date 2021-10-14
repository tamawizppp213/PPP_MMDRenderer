//////////////////////////////////////////////////////////////////////////////////
///             @file   BulletThread.hpp
///             @brief  Thread utilities for bullet function
///             @author Partially edit by toide
///             @date   2021_10_10
//////////////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2003-2014 Erwin Coumans  http://bullet.googlecode.com

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#pragma once
#ifndef BULLET_THREAD_HPP
#define BULLET_THREAD_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameMath/Include/GMVector.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define BT_THREAD_SAFE (1)
#if defined(_MSC_VER) && _MSC_VER >= 1600
#define BT_OVERRIDE override
#endif
#ifndef BT_OVERRIDE
#define BT_OVERRIDE
#endif

const unsigned int BT_MAX_THREAD_COUNT = 64;

namespace bt
{
	bool IsMainThread();
	bool ThreadsAreRunning();
	unsigned int GetCurrentThreadIndex();
	void ResetThreadIndexCounter();

	//////////////////////////////////////////////////////////////////////////////////
	//                          Class
	//////////////////////////////////////////////////////////////////////////////////

	/****************************************************************************
	*				  			TemplateClass
	*************************************************************************//**
	*  @class     TemplateClass
	*  @brief     temp
	*****************************************************************************/
	class SpinMutex
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		void Lock();
		void UnLock();
		bool TryLock();
		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		SpinMutex() { _lock = 0; }

	private:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
		int _lock;
	};

	INLINE void MutexLock(SpinMutex* mutex)
	{
#if BT_THREAD_SAFE
		mutex->Lock();
#else
		(void)mutex;
#endif
	}
	INLINE void MutexUnLock(SpinMutex* mutex)
	{
#if BT_THREAD_SAFE
		mutex->UnLock();
#else
		(void)mutex;
#endif
	}

	INLINE bool MutexTryLock(SpinMutex* mutex)
	{
#if BT_THREAD_SAFE
		return mutex->TryLock();
#else
		(void)mutex;
		return true;
#endif
	}

	class IParallelForBody
	{
	public:
		virtual ~IParallelForBody() {};
		virtual void ForLoop(int begin, int end) const = 0;
	};
	class IParallelSumBody
	{
	public:
		virtual ~IParallelSumBody() {};
		virtual float SumLoop(int begin, int end) const = 0;
	};


	class ITaskScheduler
	{
	public:
		ITaskScheduler(const char* name);
		virtual ~ITaskScheduler() {};
		const char* GetName() const { return _name; }
		virtual int GetMaxThreadsCount() const = 0;
		virtual int GetThreadsCount() const = 0;
		virtual void SetThreadsCount(int threadCount)= 0;
		virtual void ParallelFor(int begin, int end, int grainSize, const IParallelForBody& body) = 0;
		virtual float ParallelSum(int begin, int end, int grainSize, const IParallelSumBody& body) = 0;
		virtual void SleepWorkerThreadsHint() {};

		virtual void Activate();
		virtual void DeActivate();

	protected:
		const char* _name;
		unsigned int _savedThreadCounter;
		bool _isActive;
	};

	void SetTaskScheduler(ITaskScheduler* ts);
	ITaskScheduler* GetTaskScheduler();           // get the current task scheduler
	ITaskScheduler* GetSequentialTaskScheduler(); // get non-threaded task scheduler
	ITaskScheduler* CreateDefaultTaskScheduler(); // create a default task scheduler
	ITaskScheduler* GetOpenMPTaskScheduler();     // get OpenMP task scheduler
	ITaskScheduler* GetTBBTaskScheduler();        // get Intel TBB task scheduler
	ITaskScheduler* GetPPLTaskScheduler();        // get PPL task scheduler
	void ParallelFor(int begin, int end, int grainSize, const IParallelForBody& body);
	float ParallelSum(int begin, int end, int grainSize, const IParallelSumBody& body);
}
#endif