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

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Physics/Collision/CollisionDispatch/TotalCollisionDetector.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

using namespace gm;
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region SphereTriangle
SphereTriangleDetector::SphereTriangleDetector(SphereShape* sphere, TriangleShape* triangle, float contactBreakingThreshold)
{
	_sphere                   = sphere;
	_triangle                 = triangle;
	_contactBreakingThreshold = contactBreakingThreshold;
}
void SphereTriangleDetector::GetClosestPoints(const ClosestPointInput& input, Result& output, IDebugDraw* debugDraw, bool swapResult)
{
	/*-------------------------------------------------------------------
	-           Define Variables
	---------------------------------------------------------------------*/
	(void)debugDraw;
	const AffineTransform& transformA = input.TransformA;
	const AffineTransform& transformB = input.TransformB;
	Vector3 point;
	Vector3 normal;
	float   timeOfImpact = 1.0f;
	float   depth        = 0.0f;
	//AffineTransform sphereInTransform = transformB.


}
bool SphereTriangleDetector::Collide(const Vector3& sphereCenter, Vector3& point, Vector3& resultNormal, float& depth, float& timeOfImpact, float contactBreakingThreshold)
{
	return true;
}
bool SphereTriangleDetector::PointInTriangle(const gm::Vector3 vertices[], const gm::Vector3& normal, gm::Vector3* p)
{
	return true;
}
bool SphereTriangleDetector::FaceContains(const Vector3& p, const Vector3* vertices, Vector3& normal)
{
	return true;
}

#pragma endregion SphereTriangle
#pragma region BoxBox
#pragma region Define
#define dDOTpq(a, b, p, q) ((a)[0] * (b)[0] + (a)[p] * (b)[q] + (a)[2 * (p)] * (b)[2 * (q)])
#define dInfinity FLT_MAX
static Scalar dDOT(const   Scalar* a, const Scalar* b) { return dDOTpq(a, b, 1, 1); }
static Scalar dDOT44(const Scalar* a, const Scalar* b) { return dDOTpq(a, b, 4, 4); }
static Scalar dDOT41(const Scalar* a, const Scalar* b) { return dDOTpq(a, b, 4, 1); }
static Scalar dDOT14(const Scalar* a, const Scalar* b) { return dDOTpq(a, b, 1, 4); }
#define dMULTIPLYOP1_331(A, op, B, C)   \
	{                                   \
		(A)[0] op dDOT41((B), (C));     \
		(A)[1] op dDOT41((B + 1), (C)); \
		(A)[2] op dDOT41((B + 2), (C)); \
	}

#define dMULTIPLYOP0_331(A, op, B, C) \
	{                                 \
		(A)[0] op dDOT((B), (C));     \
		(A)[1] op dDOT((B + 4), (C)); \
		(A)[2] op dDOT((B + 8), (C)); \
	}

#define dMULTIPLY1_331(A, B, C) dMULTIPLYOP1_331(A, =, B, C)
#define dMULTIPLY0_331(A, B, C) dMULTIPLYOP0_331(A, =, B, C)
typedef Scalar dMatrix3[4 * 3];
#pragma endregion Define

void dLineClosestApproach(const Vector3& pa, const Vector3& ua, const Vector3& pb, const Vector3& ub, Scalar* alpha, Scalar* beta)
{
	//Vector3 p;
	//p.SetX(pb.GetX() - pa.GetX());
	//p.SetY(pb.GetY() - pa.GetY());
	//p.SetZ(pb.GetZ() - pa.GetZ());
	//Scalar uaub = dDOT(ua, ub);
	//Scalar q1   = dDOT(ua, p);
	//Scalar q2   = -dDOT(ub, p);
	//Scalar d = 1 - uaub * uaub;
	//if (d <= btScalar(0.0001f))
	//{
	//	// @@@ this needs to be made more robust
	//	*alpha = 0;
	//	*beta = 0;
	//}
	//else
	//{
	//	d = 1.f / d;
	//	*alpha = (q1 + uaub * q2) * d;
	//	*beta = (uaub * q1 + q2) * d;
	//}
}
BoxBoxDetector::BoxBoxDetector(const BoxShape* box1, const BoxShape* box2)
{
	Box1 = box1;
	Box2 = box2;
}
void BoxBoxDetector::GetClosestPoints(const ClosestPointInput& input, Result& output, IDebugDraw* debugDraw, bool swapResult)
{

}


#pragma endregion BoxBox