//////////////////////////////////////////////////////////////////////////////////
//            @file　　  CollisionCreateFunction.hpp
//            @brief    CollisionCreateFunction  
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Pertially Edit By Toide
//            @date  :  2021_09_29
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

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
#ifndef COLLISION_CREATE_FUNCTION_HPP
#define COLLISION_CREATE_FUNCTION_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
class ICollisionAlgorithm;
class CollisionObject;
struct CollisionObjectWrapper;
struct CollisionAlgorithmConstructionInfo;

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Struct
//////////////////////////////////////////////////////////////////////////////////
struct CollisionAlgorithmCreateFunction
{
	bool Swapped;

	CollisionAlgorithmCreateFunction() : Swapped(false) {};
	virtual ~CollisionAlgorithmCreateFunction() {};

	virtual ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo&, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap)
	{
		(void)body0Wrap;
		(void)body1Wrap;
		return 0;
	}
};
#endif