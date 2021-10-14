//////////////////////////////////////////////////////////////////////////////////
//            @file　　  TotalCollisionAlgorithm.hpp
//            @brief    Implement Total collision Algorithms
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_03
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

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
#ifndef TOTAL_COLLISION_ALGORITHM_HPP
#define TOTAL_COLLISION_ALGORITHM_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../BroadPhaseCollision/CollisionAlgorithm.hpp"
#include "../BroadPhaseCollision/BroadPhaseProxy.hpp"
#include "../BroadPhaseCollision/Dbvt.hpp"
#include "../CollisionShapes/TriangleCallback.hpp"
#include "../NarrowPhaseCollision/VoronoiSimplexSolver.hpp"
#include "HashedSimplePairCache.hpp"
#include "CollisionCreateFunction.hpp"
#include "CollisionDispatcher.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class PersistentManifold;
class ICollisionShape;
class ConvexPenetrationDepthSolver;
class VolonoiSimplexSolver;

using ShapePairCallback =  bool (*)(const ICollisionShape* pShape0, const ICollisionShape* pShape1);
extern ShapePairCallback g_CompoundChaildShapePairCallback;
extern ShapePairCallback g_CompoundCompoundChildShapePairCallback;
//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
#pragma region Box_Box
/****************************************************************************
*				  			 BoxBoxCollisionAlgorithm
*************************************************************************//**
*  @class      BoxBoxCollisionAlgorithm
*  @brief     temp
*****************************************************************************/
class BoxBoxCollisionAlgorithm : public IActivatingCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			int bbsize = sizeof(BoxBoxCollisionAlgorithm);
			void* ptr = ci.Dispatcher1->AllocateCollisionAlgorithm(bbsize);
			return new (ptr)BoxBoxCollisionAlgorithm(0, ci, body0Wrap, body1Wrap);
		}
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	BoxBoxCollisionAlgorithm(const CollisionAlgorithmConstructionInfo& ci) : IActivatingCollisionAlgorithm(ci) {};
	BoxBoxCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap);

	virtual ~BoxBoxCollisionAlgorithm();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	bool _ownManifold;
	PersistentManifold* _manifold;
};

#pragma endregion          Box_Box
#pragma region Sphere_Box
/****************************************************************************
*				  			 BoxBoxCollisionAlgorithm
*************************************************************************//**
*  @class      BoxBoxCollisionAlgorithm
*  @brief     temp
*****************************************************************************/
class SphereBoxCollisionAlgorithm : public IActivatingCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(SphereBoxCollisionAlgorithm));
			
			if (!Swapped)
			{
				return new (memory) SphereBoxCollisionAlgorithm(0, ci, body0Wrap, body1Wrap, false);
			}
			else
			{
				return new (memory) SphereBoxCollisionAlgorithm(0, ci, body0Wrap, body1Wrap, true);
			}
		}
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;
	bool  GetSphereDistance(const CollisionObjectWrapper* boxObjectWrap, gm::Vector3& pointOnBox, gm::Vector3& normal, float& penetrationDepth, const gm::Vector3& sphereCenter, float radius, float maxContactDistance);
	float GetSpherePenetration(gm::Vector3 const& boxHalfExtent, gm::Vector3 const& sphereRelativePosition, gm::Vector3& closestPoint, gm::Vector3& normal);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	SphereBoxCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, bool isSwapped);

	virtual ~SphereBoxCollisionAlgorithm();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	bool _ownManifold;
	PersistentManifold* _manifold;
	bool _isSwapped;
};
#pragma endregion       Sphere_Box
#pragma region   Sphere_Sphere
/****************************************************************************
*				  			 BoxBoxCollisionAlgorithm
*************************************************************************//**
*  @class      BoxBoxCollisionAlgorithm
*  @brief     temp
*****************************************************************************/
class SphereSphereCollisionAlgorithm : public IActivatingCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(SphereSphereCollisionAlgorithm));
			return new (memory)SphereSphereCollisionAlgorithm(0, ci, body0Wrap, body1Wrap);
		}
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	SphereSphereCollisionAlgorithm(const CollisionAlgorithmConstructionInfo& ci);
	SphereSphereCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap);

	virtual ~SphereSphereCollisionAlgorithm();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	bool _ownManifold;
	PersistentManifold* _manifold;
};
#pragma endregion    Sphere_Sphere
#pragma region Sphere_Triangle
/****************************************************************************
*				  			 BoxBoxCollisionAlgorithm
*************************************************************************//**
*  @class      BoxBoxCollisionAlgorithm
*  @brief     temp
*****************************************************************************/
class SphereTriangleCollisionAlgorithm : public IActivatingCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(SphereTriangleCollisionAlgorithm));
			return new (memory)SphereTriangleCollisionAlgorithm(ci.Manifold, ci, body0Wrap, body1Wrap, Swapped);
		}
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	SphereTriangleCollisionAlgorithm(const CollisionAlgorithmConstructionInfo& ci) : IActivatingCollisionAlgorithm(ci) {};
	SphereTriangleCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, bool swapped);

	virtual ~SphereTriangleCollisionAlgorithm();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	bool _ownManifold;
	PersistentManifold* _manifold;
	bool _swapped;
};

#pragma endregion  Sphere_Triangle
#pragma region Convex_Convex
/****************************************************************************
*				  			 BoxBoxCollisionAlgorithm
*************************************************************************//**
*  @class      BoxBoxCollisionAlgorithm
*  @brief     temp
*****************************************************************************/
class ConvexConvexCollisionAlgorithm : public IActivatingCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		ConvexPenetrationDepthSolver* PdSolver;
		int NumPerturbationIterations;
		int MinimumPointsPerturbationThreshold;
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(ConvexConvexCollisionAlgorithm));
			return new (memory)ConvexConvexCollisionAlgorithm(ci.Manifold, ci, body0Wrap, body1Wrap, PdSolver, NumPerturbationIterations, MinimumPointsPerturbationThreshold);
		};
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;
	void  SetLowLevelofDetail(bool useLowLevel);
	const PersistentManifold* GetManifold() { return _manifold; }

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ConvexConvexCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, ConvexPenetrationDepthSolver* pdSolver, int numPerturbationIterations, int minimumPointsPerturbationThreshold);
	virtual ~ConvexConvexCollisionAlgorithm();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	ConvexPenetrationDepthSolver* _penetrationDepthSolver;
	gm::AlignedObjectArray<gm::Vector3> _worldVertsB1;
	gm::AlignedObjectArray<gm::Vector3> _worldVertsB2;
	bool _ownManifold;
	PersistentManifold* _manifold;
	bool _lowLevelOfDetail;

	int _numPerturbationIteration;
	int _minimumPointsPerturbationThreshold;
};

#pragma endregion    Convex_Convex
#pragma region    Convex_Concave
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class ConvexTriangleCallback : public TriangleCallback
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void ProcessTriangle(gm::Vector3* triangle, int partID, int triangleIndex);
	void ClearCache();
	void ClearWrapperData() { _convexBodyWrap = NULL; _triangleBodyWrap = NULL; }

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	int TriangleCount;
	PersistentManifold* ManifoldPtr;

	INLINE const gm::Vector3& GetAABBMin() const { return _aabbMin; }
	INLINE const gm::Vector3& GetAABBMax() const { return _aabbMax; }

	void SetTimeStepAndCounters(float collisionMarginTriangle, const DispatcherInfo& dispatchInfo, const CollisionObjectWrapper* convexBodyWrap, const CollisionObjectWrapper* triangleBodyWrap, ManifoldResult* resultOut);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	ConvexTriangleCallback(IDispatcher* dispatcher, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, bool isSwapped);
	~ConvexTriangleCallback();

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::Vector3 _aabbMin;
	gm::Vector3 _aabbMax;

	const CollisionObjectWrapper* _convexBodyWrap;
	const CollisionObjectWrapper* _triangleBodyWrap;

	ManifoldResult*       _resultOut;
	IDispatcher*          _dispatcher;
	const DispatcherInfo* _dispatchInfoPtr;
	float                 _collisionMarginTriangle;
};

/****************************************************************************
*				  			 ConvexConcaveCollisionAlgorithm
*************************************************************************//**
*  @class     ConvexConcaveCollisionAlgorithm
*  @brief     temp
*****************************************************************************/
__declspec(align(16))
class ConvexConcaveCollisionAlgorithm : public IActivatingCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		virtual ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap)
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(ConvexConcaveCollisionAlgorithm));
			return new (memory) ConvexConcaveCollisionAlgorithm(ci, body0Wrap, body1Wrap, false);
		}
	};

	struct SwappedCreateFunction : public CollisionAlgorithmCreateFunction
	{
		virtual ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap)
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(ConvexConcaveCollisionAlgorithm));
			return new (memory) ConvexConcaveCollisionAlgorithm(ci, body0Wrap, body1Wrap, true);
		}
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;
	void  ClearCache();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	ConvexConcaveCollisionAlgorithm(const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, bool isSwapped);
	virtual ~ConvexConcaveCollisionAlgorithm();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	bool _isSwapped;
};
#pragma endregion   Convex_Cancave
#pragma region Convex_Plane
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class ConvexPlaneCollisionAlgorithm : public ICollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		int NumPerturbationIterations;
		int MinimumPointsPerturbationThreshold;

		CreateFunction() : NumPerturbationIterations(1), MinimumPointsPerturbationThreshold(0) {};

		ICollisionAlgorithm* CreateCollisionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap)
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(ConvexPlaneCollisionAlgorithm));
			if (!Swapped)
			{
				return new (memory) ConvexPlaneCollisionAlgorithm(0, ci, body0Wrap, body1Wrap, false, NumPerturbationIterations, MinimumPointsPerturbationThreshold);

			}
			else
			{
				return new (memory) ConvexPlaneCollisionAlgorithm(0, ci, body0Wrap, body1Wrap, true, NumPerturbationIterations, MinimumPointsPerturbationThreshold);
			}
		}
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  CollideSingleContact(const gm::Quaternion& perturbeRotation, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut);
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ConvexPlaneCollisionAlgorithm(PersistentManifold* manifold, CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrao, const CollisionObjectWrapper* body1Wrap, bool isSwapped, int numPerturbationIterations, int minimumPointsPerturbationThreshold);
	virtual ~ConvexPlaneCollisionAlgorithm();

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	bool                _ownManifold;
	PersistentManifold* _mainfold;
	bool                _isSwapped;
	int                 _numPertubationIterations;
	int                 _minimumPointPertubationThreshold;
};
#pragma endregion     Convex_Plane
#pragma region Compound 
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class CompoundCollisionAlgorithm : public IActivatingCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(CompoundCollisionAlgorithm));
			return new (memory) CompoundCollisionAlgorithm(ci, body0Wrap, body1Wrap, false);
		}
	};
	struct SwappedCreateFunction : public CollisionAlgorithmCreateFunction
	{
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(CompoundCollisionAlgorithm));
			return new (memory) CompoundCollisionAlgorithm(ci, body0Wrap, body1Wrap, true);
		}
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	ICollisionAlgorithm* GetChildAlgorithm(int n) const { return _childCollisionAlgorithms[n]; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	CompoundCollisionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, bool isSwapped);
	virtual ~CompoundCollisionAlgorithm();
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	void RemoveChildAlgorithms();
	void PreAllocateChildAlgorithms(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap);

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	gm::AlignedObjectArray<ICollisionAlgorithm*> _childCollisionAlgorithms;
	bool                _isSwapped;
	PersistentManifold* _sharedManifold;
	bool                _ownsManifold;
	int                 _compoundShapeRevision;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	NodeStack     _stack2;
	ManifoldArray _manifoldArray;
};
#pragma endregion         Compound
#pragma region CompoundCompound
class CompoundCollisionAlgorithm;
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class CompoundCompoundCollisionAlgorithm : public CompoundCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(CompoundCompoundCollisionAlgorithm));
			return new (memory) CompoundCompoundCollisionAlgorithm(ci, body0Wrap, body1Wrap, false);
		}
	};
	struct SwappedCreateFunction : public CollisionAlgorithmCreateFunction
	{
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(CompoundCompoundCollisionAlgorithm));
			return new (memory) CompoundCompoundCollisionAlgorithm(ci, body0Wrap, body1Wrap, true);
		}
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	CompoundCompoundCollisionAlgorithm(const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, bool isSwapped);
	virtual ~CompoundCompoundCollisionAlgorithm();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void RemoveChildAlgorithms();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	HashedSimplePairCache* _childCollisionAlgorithmCache;
	BtSimplePairArray      _removePairs;

	int _compoundShapeRevision0;
	int _compoundShapeRevision1;
};
#pragma endregion CompoundCompound
#pragma region Empty
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class EmptyAlgorithm : public ICollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		virtual ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap)
		{
			(void)body0Wrap; (void)body1Wrap;
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(EmptyAlgorithm));
			return new (memory) EmptyAlgorithm(ci);
		}
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override {};
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	EmptyAlgorithm(const CollisionAlgorithmConstructionInfo& ci);

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#pragma endregion            Empty
#pragma region Box2D_Box2D
/****************************************************************************
*				  			 Box2DBox2DCollisionAlgorithm
*************************************************************************//**
*  @class      Box2DBox2DCollisionAlgorithm
*  @brief      Box2D collision test algorithm (2021_10_10: first checked)
*****************************************************************************/
class Box2DBox2DCollisionAlgorithm : public IActivatingCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			int bbsize = sizeof(Box2DBox2DCollisionAlgorithm);
			void* ptr = ci.Dispatcher1->AllocateCollisionAlgorithm(bbsize);
			return new (ptr)Box2DBox2DCollisionAlgorithm(0, ci, body0Wrap, body1Wrap);
		}
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Box2DBox2DCollisionAlgorithm(const CollisionAlgorithmConstructionInfo& ci) : IActivatingCollisionAlgorithm(ci) {};
	Box2DBox2DCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap);

	virtual ~Box2DBox2DCollisionAlgorithm();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	bool _ownManifold;
	PersistentManifold* _manifold;
};
#pragma endregion Box2D_Box2D
#pragma region Colvex2D_Convex2D
/****************************************************************************
*				  			 BoxBoxCollisionAlgorithm
*************************************************************************//**
*  @class      BoxBoxCollisionAlgorithm
*  @brief     temp
*****************************************************************************/
class Convex2DConvex2DCollisionAlgorithm : public IActivatingCollisionAlgorithm
{
public:
	struct CreateFunction : public CollisionAlgorithmCreateFunction
	{
		ConvexPenetrationDepthSolver* PdSolver;
		VolonoiSimplexSolver*         SimplexSolver;
		int                           NumPerturbationIterations;
		int                           MinimumPointsPerturbationThreshold;

		CreateFunction(VolonoiSimplexSolver* simplexSolver, ConvexPenetrationDepthSolver* pdSolver);
		virtual ~CreateFunction();
		ICollisionAlgorithm* CreateCollsiionAlgorithm(CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap) override
		{
			void* memory = ci.Dispatcher1->AllocateCollisionAlgorithm(sizeof(Convex2DConvex2DCollisionAlgorithm));
			return new (memory) Convex2DConvex2DCollisionAlgorithm(ci.Manifold, ci, body0Wrap, body1Wrap, SimplexSolver, PdSolver, NumPerturbationIterations, MinimumPointsPerturbationThreshold);
		}
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void  ProcessCollision(const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	float CalculateTimeOfImpact(CollisionObject* body0, CollisionObject* body1, const DispatcherInfo& dispatchInfo, ManifoldResult* resultOut) override;
	void  GetAllContactManifolds(ManifoldArray& manifoldArray) override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const PersistentManifold* GetManifold() { return _manifold; }
	
	void SetLowLevelOfDetail(bool useLowLevel);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Convex2DConvex2DCollisionAlgorithm(PersistentManifold* manifold, const CollisionAlgorithmConstructionInfo& ci, const CollisionObjectWrapper* body0Wrap, const CollisionObjectWrapper* body1Wrap, VolonoiSimplexSolver* simplexSolver, ConvexPenetrationDepthSolver* pdSolver, int numPerturbationIterations, int minimumPointsPerturbationThreshold);
	virtual ~Convex2DConvex2DCollisionAlgorithm();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	VolonoiSimplexSolver _simplexSolver;
	ConvexPenetrationDepthSolver* _pdSolver;
	bool _ownManifold;
	PersistentManifold* _manifold;
	int _lowLevelOfDetail;
};

#pragma endregion Convex2D_Convex2D
#endif