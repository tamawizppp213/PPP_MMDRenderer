//////////////////////////////////////////////////////////////////////////////////
//            @file�@�@  DBVTBroadPhase.hpp
//            @brief    Broad phase collision detection object  
//                      ����! �l�I�ȕ׋��̂���, bullet�̃\�[�X�R�[�h�����p���ĉ��ς��Ă��܂�. 
//                        �@�@bullet�g�p���̒��ӏ������ȉ��ɋL�ڒv���܂�. 
//            @author:  Partially edit by Toide 
//            @date  :  2021_10_09 (first checked)
//////////////////////////////////////////////////////////////////////////////////

//
// btAxisSweep3.h
//
// Copyright (c) 2006 Simon Hobbs
//
// This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

#pragma once
#ifndef AXIS_SWEEP_HPP
#define AXIS_SWEEP_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "AxisSweepInternal.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Template Class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			AxisSweep
*************************************************************************//**
*  @class     AxisSweep
*  @brief     The btAxisSweep3 is an efficient implementation of the 3d axis sweep and prune broadphase.
              It uses arrays rather then lists for storage of the 3 axis. Also it operates using 16 bit integer coordinates instead of floats.
              For large worlds and many objects, use bt32BitAxisSweep3 or btDbvtBroadphase instead. bt32BitAxisSweep3 has higher precision and allows more then 16384 objects at the cost of more memory and bit of performance.
*****************************************************************************/
class AxisSweep : public AxisSweepInternal<unsigned short int>
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	AxisSweep(const gm::Vector3& worldAABBMin, const gm::Vector3& worldAabbMax, unsigned short int maxHandles = 16384, IOverlappingPairCache* pairCache = 0, bool disableRaycastAccelerator = false)
		:AxisSweepInternal<unsigned short int>(worldAABBMin, worldAabbMax, 0xfffe, 0xffff, maxHandles, pairCache, disableRaycastAccelerator)
	{
		assert(maxHandles > 1 && maxHandles < 32767);
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
};

class AxisSweep32Bit : public AxisSweepInternal<unsigned int>
{
	AxisSweep32Bit(const gm::Vector3& worldAABBMin, const gm::Vector3& worldAABBMax, unsigned int maxHandles = 1500000, IOverlappingPairCache* pairCache = 0, bool disableRaycastAccelerator = false)
		:AxisSweepInternal<unsigned int>(worldAABBMin, worldAABBMax, 0xfffffffe, 0x7fffffff, maxHandles, pairCache, disableRaycastAccelerator)
	{
		assert(maxHandles > 1 && maxHandles < 2147483647);
	}
};
#endif
