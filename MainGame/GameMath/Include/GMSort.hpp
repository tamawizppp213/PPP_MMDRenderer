//////////////////////////////////////////////////////////////////////////////////
///             @file   GMSort.hpp
///             @brief  GMSortAlgorithm
///             @author Toide Yutaro
///             @date   2020_12_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GM_SORT_HPP
#define GM_SORT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>
#include <utility> 

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         
//////////////////////////////////////////////////////////////////////////////////
namespace gm
{
	/****************************************************************************
	*				  			Sort
	*************************************************************************//**
	*  @class     Sort
	*  @brief     Sort
	*****************************************************************************/
	template<typename T>
	class Sort
	{
	public:
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		static void InsertionSortAscend (T* array, int arraySize);
		static void InsertionSortDescend(T* array, int arraySize);
		static void BubbleSortAscend    (T* array, int arraySize);
		static void BubbleSortDescend   (T* array, int arraySize);
		static void SelectionSortAscend (T* array, int arraySize);
		static void SelectionSortDescend(T* array, int arraySize);
		static void ShellSortAscend     (T* array, int arraySize);
		static void ShellSortDescend    (T* array, int arraySize);
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
		
	};

#pragma region Public Function
	/****************************************************************************
	*							InsertionSortAscend
	*************************************************************************//**
	*  @fn        void Sort<T>::InsertionSortAscend(T* array, int arraySize)
	*  @brief     Insertion Sort (O(N ~ N^2))
	*  @param[out]T*  array
	*  @param[in] int arraySize
	*  @return 　　void
	*****************************************************************************/
	template<typename T> void Sort<T>::InsertionSortAscend(T* array, int arraySize)
	{
		int j;

		for (int i = 1; i < arraySize; ++i)
		{
			T temp = array[i];
			for (j = i - 1; j >= 0 && array[j] >  temp; j--)
			{
				array[j + 1] = array[j];
			}

			array[j + 1] = temp;
		}
	}

	/****************************************************************************
	*							InsertionSortDescend
	*************************************************************************//**
	*  @fn        void Sort<T>::InsertionSortDescend(T* array, int arraySize)
	*  @brief     Insertion Sort (O(N ~ N^2))
	*  @param[out]T* array
	*  @param[in] int arraySize
	*  @return 　　void
	*****************************************************************************/
	template<typename T> void Sort<T>::InsertionSortDescend(T* array, int arraySize)
	{
		int j;

		for (int i = 1; i < arraySize; ++i)
		{
			T temp = array[i];
			for (j = i - 1; j >= 0 && array[j] < temp; j--)
			{
				array[j + 1] = array[j];
			}

			array[j + 1] = temp;
		}
	}
	/****************************************************************************
	*							BubbleSortAscend
	*************************************************************************//**
	*  @fn        void Sort<T>::BubbleSortDescend(T* array, int arraySize)
	*  @brief     Bubble Sort (O(N^2))
	*  @param[out]T* array
	*  @param[in] int arraySize
	*  @return 　　void
	*****************************************************************************/
	template<typename T> void Sort<T>::BubbleSortAscend(T* array, int arraySize)
	{
		bool isSwap = true;
		for (int i = 0; isSwap; ++i)
		{
			isSwap = false;

			for (int j = arraySize - 1; j >= i + 1; --j)
			{
				if (array[j] < array[j - 1])
				{
					std::swap(array[j], array[j - 1]);
					isSwap = true;
				}
			}
		}
	}
	/****************************************************************************
	*							BubbleSortDescend
	*************************************************************************//**
	*  @fn        void Sort<T>::InsertionSortDescend(T* array, int arraySize)
	*  @brief     Insertion Sort (O(N^2))
	*  @param[out]T* array
	*  @param[in] int arraySize
	*  @return 　　void
	*****************************************************************************/
	template<typename T> void Sort<T>::BubbleSortDescend(T* array, int arraySize)
	{
		bool isSwap = true;
		for (int i = 0; isSwap; ++i)
		{
			isSwap = false;

			for (int j = arraySize - 1; j >= i + 1; --j)
			{
				if (array[j] > array[j - 1])
				{
					std::swap(array[j], array[j - 1]);
					isSwap = true;
				}
			}
		}
	}
	/****************************************************************************
	*							SelectionSortAscend
	*************************************************************************//**
	*  @fn        void Sort<T>::SelectionSortAscend(T* array, int arraySize)
	*  @brief     Selection Sort (O(N^2)) !note: This sort is instable and has a bug. 
	*  @param[out]T* array
	*  @param[in] int arraySize
	*  @return 　　void
	*****************************************************************************/
	template<typename T> void Sort<T>::SelectionSortAscend(T* array, int arraySize)
	{
		int i, j, temp, min_jIndex;
		for (i = 0; i < arraySize - 1; i++)
		{
			min_jIndex = i;
			for (j = i + 1; j < arraySize; j++)
			{
				// get min_jindex
				if (array[j] < array[min_jIndex])
				{
					min_jIndex = j;
				}
				
				// swap
				temp              = array[i];
				array[i]          = array[min_jIndex];
				array[min_jIndex] = temp;
			}
		}
	}
	/****************************************************************************
	*							SelectionSortDescend
	*************************************************************************//**
	*  @fn        void Sort<T>::SelectionSortDescend(T* array, int arraySize)
	*  @brief     Selection Sort (O(N^2))!note: This sort is instable and has a bug.
	*  @param[out]T* array
	*  @param[in] int arraySize
	*  @return 　　void
	*****************************************************************************/
	template<typename T> void Sort<T>::SelectionSortDescend(T* array, int arraySize)
	{
		int i, j, temp, max_jIndex;
		for (i = 0; i < arraySize - 1; i++)
		{
			max_jIndex = i;
			for (j = i + 1; j < arraySize; j++)
			{
				// get min_jindex
				if (array[j] > array[max_jIndex])
				{
					max_jIndex = j;
				}

				// swap
				temp              = array[i];
				array[i]          = array[max_jIndex];
				array[max_jIndex] = temp;
			}
		}
	}

	/****************************************************************************
	*							ShellSortAscend
	*************************************************************************//**
	*  @fn        void Sort<T>::ShellSortAscend(T* array, int arraySize)
	*  @brief     Shell Sort (O(N^1.25))(step: g_{n+1} = 3g_n +1).
	*  @param[out]T* array
	*  @param[in] int arraySize
	*  @return 　　void
	*****************************************************************************/
	template<typename T> void Sort<T>::ShellSortAscend(T* array, int arraySize)
	{
		int j, temp, step = 1;

		while (3 * step + 1 <= arraySize / 9)
		{
			step = 3 * step + 1;
		}

		while (step >= 1)
		{
			for (int i = step; i < arraySize; ++i)
			{
				j = i;
				while ((j >= step) && (array[j - step] > array[j]))
				{
					temp            = array[j - step];
					array[j - step] = array[j];
					array[j]        = temp;

					j -= step;
				}
			}
			step /= 3;
		}
	}
	/****************************************************************************
	*							ShellSortDescend
	*************************************************************************//**
	*  @fn        void Sort<T>::ShellSortDescend(T* array, int arraySize)
	*  @brief     Shell Sort (O(N^1.25))(step: g_{n+1} = 3g_n +1).
	*  @param[out]T* array
	*  @param[in] int arraySize
	*  @return 　　void
	*****************************************************************************/
	template<typename T> void Sort<T>::ShellSortDescend(T* array, int arraySize)
	{
		int j, temp, step = 1;

		while (3 * step + 1 <= arraySize / 9)
		{
			step = 3 * step + 1;
		}

		while (step >= 1)
		{
			for (int i = step; i < arraySize; ++i)
			{
				j = i;
				while ((j >= step) && (array[j - step] < array[j]))
				{
					temp            = array[j - step];
					array[j - step] = array[j];
					array[j]        = temp;

					j -= step;
				}
			}
			step /= 3;
		}
	}
#pragma endregion Public Function
#pragma region Private Function

#pragma endregion Private Function
}

#endif