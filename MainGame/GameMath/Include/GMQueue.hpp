//////////////////////////////////////////////////////////////////////////////////
///             @file   GMQueue.hpp
///             @brief  GM Simple Queue
///             @author Toide Yutaro
///             @date   2020_12_13
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GM_QUEUE_HPP
#define GM_QUEUE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         
//////////////////////////////////////////////////////////////////////////////////
namespace gm
{
	/****************************************************************************
	*				  			Queue
	*************************************************************************//**
	*  @class     Queue
	*  @brief     Simple Queue
	*****************************************************************************/
	template<typename T>
	class Queue
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		void Reserve(int reserveNum)
		{
			_queue.reserve(reserveNum);
		}

		bool IsEmpty() const
		{
			return _startPosition == _queue.size();
		}

		void Enqueue(const T& data)
		{
			_queue.push_back(data);
		}

		T& Dequeue()
		{
			_startPosition++;
			return _queue[_startPosition-1];
		}

		T& Front()
		{
			return _queue[_startPosition];
		}

		int QueueCount()
		{
			return _queue.size() - _startPosition;
		}

		void Clear()
		{
			_queue.clear();
			_startPosition = 0;
		}
		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/


	private:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
		std::vector<T> _queue;
		int _startPosition = 0;
	};
}
#endif
