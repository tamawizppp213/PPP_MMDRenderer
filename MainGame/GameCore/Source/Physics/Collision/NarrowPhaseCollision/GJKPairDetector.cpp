//////////////////////////////////////////////////////////////////////////////////
//            @file　　  VoronoiSimplexSolver.hpp
//            @brief    VoronoiSimplexSolver
//                      注意! 個人的な勉強のため, bulletのソースコードを引用して改変しています. 
//                        　　bullet使用時の注意書きを以下に記載致します. 
//            @author:  Partically Edit Bullet by Toide
//            @date  :  2021_10_12 (first checked)
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
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/GjkPairDetector.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/ConvexShape.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/VoronoiSimplexSolver.hpp"
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/PenetrationDepthSolver.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define REL_ERROR2 1.0e-6
float gGjkEpaPenetrationTolerance = 0.001f;

using namespace gm;

struct SupportVector
{
	Vector3 V;
	Vector3 V1;
	Vector3 V2;
};
struct Simplex
{
	SupportVector PS[4];
	int Last;
};

static Vector3 ccd_vec3_origin = Vector3(0, 0, 0);
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
GJKPairDetector::GJKPairDetector(const ConvexShape* objectA, const ConvexShape* objectB, VolonoiSimplexSolver* simplexSolver, ConvexPenetrationDepthSolver* penetrationDepthSolver)
	: _cachedSeparatingAxis(0.0f, 1.0f, 0.0f),
	_penetrationDepthSolver(penetrationDepthSolver),
	_simplexSolver(simplexSolver),
	_minkowskiA(objectA),
	_minkowskiB(objectB),
	_shapeTypeA(objectA->GetShapeType()),
	_shapeTypeB(objectB->GetShapeType()),
	_marginA(objectA->GetMargin()),
	_marginB(objectB->GetMargin()),
	_ignoreMargin(false),
	LastUsedMethod(-1),
	CatchDegeneracies(1),
	FixContactNormalDirection(1)
{
}
GJKPairDetector::GJKPairDetector(const ConvexShape* objectA, const ConvexShape* objectB, int shapeTypeA, int shapeTypeB, float marginA, float marginB, VolonoiSimplexSolver* simplexSolver, ConvexPenetrationDepthSolver* penetrationDepthSolver)
	: _cachedSeparatingAxis(0.0f, 1.0f, 0.0f),
	_penetrationDepthSolver(penetrationDepthSolver),
	_simplexSolver(simplexSolver),
	_minkowskiA(objectA),
	_minkowskiB(objectB),
	_shapeTypeA(shapeTypeA),
	_shapeTypeB(shapeTypeB),
	_marginA(marginA),
	_marginB(marginB),
	_ignoreMargin(false),
	LastUsedMethod(-1),
	CatchDegeneracies(1),
	FixContactNormalDirection(1)
{
}

#pragma region Static Function
static void ComputeSupport(const ConvexShape* convexA, const AffineTransform& localTransformA, const ConvexShape* convexB, const AffineTransform& localTransformB, const Vector3& direction, bool check2D, Vector3& supAWorld, Vector3& supBWorld, Vector3& aMinb)
{
	Vector3 separatingAxisInA = localTransformA.GetBasis() * direction;
	Vector3 separatingAxisInB = localTransformB.GetBasis() * (-direction);
	Vector3 pInANoMargin      = convexA->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInA);
	Vector3 qInBNoMargin      = convexB->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInB);
	Vector3 pInA              = pInANoMargin;
	Vector3 qInB              = qInBNoMargin;

	supAWorld = localTransformA * pInA;
	supBWorld = localTransformB * qInB;

	if (check2D)
	{
		supAWorld.SetZ(0.0f);
		supBWorld.SetZ(0.0f);
	}

	aMinb = supAWorld - supBWorld;
}
inline void SimplexInitialize(Simplex* simplex) { simplex->Last = -1; }
inline int  SimplexSize(const Simplex* simplex) { return simplex->Last + 1; }
inline const SupportVector* SimplexPoint(const Simplex* simplex, int index) { return &simplex->PS[index]; }
inline void  SupportCopy(SupportVector* d, const SupportVector* s) { *d = *s; }
inline void  Vector3Copy(Vector3* v, const Vector3* w)             { *v = *w;}
inline void  CCDVector3Addition(Vector3* v, const Vector3* w){ *v = *v + *w;}
inline void  CCDVector3Subtract(Vector3* v, const Vector3* w) { *v -= *w;}
inline void  Vector3Subtract2(Vector3* d, const Vector3* v, const Vector3* w) { *d = (*v) - (*w);}
inline float Vector3Dot(const Vector3* a, const Vector3* b) { return Dot(*a, *b);;}
inline float CCDVector3Dist2(const Vector3* a, const Vector3* b)
{
	Vector3 ab;
	Vector3Subtract2(&ab, a, b);
	return Vector3Dot(&ab, &ab);
}
inline void  Vector3Scale(Vector3* d, float k) { *d = k * (*d); }
inline void Vector3Cross(Vector3* d, const Vector3* a, const Vector3* b)
{
	d->SetX((a->GetY() * b->GetZ()) - (a->GetZ() * b->GetY()));
	d->SetY((a->GetZ() * b->GetX()) - (a->GetX() * b->GetZ()));
	d->SetZ((a->GetX() * b->GetY()) - (a->GetY() * b->GetX()));
}
inline void TripleCross(const Vector3* a, const Vector3* b, const Vector3* c, Vector3* d)
{
	Vector3 e;
	Vector3Cross(&e, a, b);
	Vector3Cross(d, &e, c);
}

inline int ccdEq(float _a, float _b)
{
	float ab;
	float a, b;

	ab = Abs(_a - _b);
	if (Abs(ab) < FLT_EPSILON)
		return 1;

	a = Abs(_a);
	b = Abs(_b);
	if (b > a)
	{
		return ab < FLT_EPSILON* b;
	}
	else
	{
		return ab < FLT_EPSILON* a;
	}
}

float ccdVector3X(const Vector3* v)
{
	return v->GetX();
}
float ccdVector3Y(const Vector3* v)
{
	return v->GetY();
}
float ccdVector3Z(const Vector3* v)
{
	return v->GetZ();
}
inline int btVec3Eq(const Vector3* a, const Vector3* b)
{
	return ccdEq(ccdVector3X(a), ccdVector3X(b)) && ccdEq(ccdVector3Y(a), ccdVector3Y(b)) && ccdEq(ccdVector3Z(a), ccdVector3Z(b));
}
inline void SimplexAdd(Simplex* s, const SupportVector* v)
{
	// here is no check on boundaries in sake of speed
	++s->Last;
	SupportCopy(s->PS + s->Last, v);
}
inline void SimplexSet(Simplex* s, size_t pos, const SupportVector* a)
{
	SupportCopy(s->PS + pos, a);
}
inline void SimplexSetSize(Simplex* s, int size){ s->Last = size - 1;}
inline const SupportVector* ccdSimplexLast(const Simplex* s)
{
	return SimplexPoint(s, s->Last);
}
inline int   ccdSign(float val)
{
	if (Abs(val) < FLT_EPSILON)
	{
		return 0;
	}
	else if (val < 0.0f)
	{
		return -1;
	}
	return 1;
}
inline float Vector3PointSegmentDist2(const Vector3* P, const Vector3* x0, const Vector3* b, Vector3* witness)
{
	float dist, t;
	Vector3 d, a;

	// direction of segment
	Vector3Subtract2(&d, b, x0);

	// precompute vector from P to x0
	Vector3Subtract2(&a, x0, P);

	t = -1.0f * Vector3Dot(&a, &d);
	t /= Vector3Dot(&d, &d);

	if (t < 0.0f || Abs(t) < FLT_EPSILON)
	{
		dist = CCDVector3Dist2(x0, P);
		if (witness)
			Vector3Copy(witness, x0);
	}
	else if (t > 1.0f || ccdEq(t, 1.0f))
	{
		dist = CCDVector3Dist2(b, P);
		if (witness)
			Vector3Copy(witness, b);
	}
	else
	{
		if (witness)
		{
			Vector3Copy(witness, &d);
			Vector3Scale(witness, t);
			CCDVector3Addition(witness, x0);
			dist = CCDVector3Dist2(witness, P);
		}
		else
		{
			// recycling variables
			Vector3Scale(&d, t);
			CCDVector3Addition(&d, &a);
			dist = Vector3Dot(&d, &d);
		}
	}

	return dist;
}
float btVec3PointTriDist2(const Vector3* P, const Vector3* x0, const Vector3* B, const Vector3* C, Vector3* witness)
{
	// Computation comes from analytic expression for triangle (x0, B, C)
	//      T(s, t) = x0 + s.d1 + t.d2, where d1 = B - x0 and d2 = C - x0 and
	// Then equation for distance is:
	//      D(s, t) = | T(s, t) - P |^2
	// This leads to minimization of quadratic function of two variables.
	// The solution from is taken only if s is between 0 and 1, t is
	// between 0 and 1 and t + s < 1, otherwise distance from segment is
	// computed.

	Vector3 d1, d2, a;
	double u, v, w, p, q, r;
	double s, t, dist, dist2;
	Vector3 witness2;

	Vector3Subtract2(&d1, B, x0);
	Vector3Subtract2(&d2, C, x0);
	Vector3Subtract2(&a, x0, P);

	u = Vector3Dot(&a, &a);
	v = Vector3Dot(&d1, &d1);
	w = Vector3Dot(&d2, &d2);
	p = Vector3Dot(&a, &d1);
	q = Vector3Dot(&a, &d2);
	r = Vector3Dot(&d1, &d2);

	s = (q * r - w * p) / (w * v - r * r);
	t = (-s * r - q) / w;

	if ((Abs(s) < FLT_EPSILON || s > 0.0f) && (ccdEq(s, 1.0f) || s < 1.0f) && (Abs(t) < FLT_EPSILON || t > 0.0f) && (ccdEq(t, 1.0f) || t <1.0f) && (ccdEq(t + s, 1.0f) || t + s < 1.0f))
	{
		if (witness)
		{
			Vector3Scale(&d1, s);
			Vector3Scale(&d2, t);
			Vector3Copy(witness, x0);
			CCDVector3Addition(witness, &d1);
			CCDVector3Addition(witness, &d2);
			dist = CCDVector3Dist2(witness, P);
		}
		else
		{
			dist = s * s * v;
			dist += t * t * w;
			dist += 2.0f * s * t * r;
			dist += 2.0f * s * p;
			dist += 2.0f * t * q;
			dist += u;
		}
	}
	else
	{
		dist  = Vector3PointSegmentDist2(P, x0, B, witness);
		dist2 = Vector3PointSegmentDist2(P, x0, C, &witness2);
		if (dist2 < dist)
		{
			dist = dist2;
			if (witness)
				Vector3Copy(witness, &witness2);
		}

		dist2 = Vector3PointSegmentDist2(P, B, C, &witness2);
		if (dist2 < dist)
		{
			dist = dist2;
			if (witness)
				Vector3Copy(witness, &witness2);
		}
	}

	return dist;
}

static int btDoSimplex2(Simplex* simplex, Vector3* dir)
{
	const SupportVector* A, * B;
	Vector3 AB, AO, tmp;
	float dot;

	// get last added as A
	A = ccdSimplexLast(simplex);
	// get the other point
	B = SimplexPoint(simplex, 0);
	// compute AB oriented segment
	Vector3Subtract2(&AB, &B->V, &A->V);
	// compute AO vector
	Vector3Copy(&AO, &A->V);
	Vector3Scale(&AO, -1.0f);

	// dot product AB . AO
	dot = Vector3Dot(&AB, &AO);

	// check if origin doesn't lie on AB segment
	Vector3Cross(&tmp, &AB, &AO);
	if ((float)Abs(Vector3Dot(&tmp, &tmp)) < FLT_EPSILON && dot > 0.0f)
	{
		return 1;
	}

	// check if origin is in area where AB segment is
	if (Abs(dot) < FLT_EPSILON || dot < 0.0f)
	{
		// origin is in outside are of A
		SimplexSet(simplex, 0, A);
		SimplexSetSize(simplex, 1);
		Vector3Copy(dir, &AO);
	}
	else
	{
		// origin is in area where AB segment is

		// keep simplex untouched and set direction to
		// AB x AO x AB
		TripleCross(&AB, &AO, &AB, dir);
	}

	return 0;
}

static int btDoSimplex3(Simplex* simplex, Vector3* dir)
{
	const SupportVector* A, * B, * C;
	Vector3 AO, AB, AC, ABC, tmp;
	float dot, dist;

	// get last added as A
	A = ccdSimplexLast(simplex);
	// get the other points
	B = SimplexPoint(simplex, 1);
	C = SimplexPoint(simplex, 0);

	// check touching contact
	dist = btVec3PointTriDist2(&ccd_vec3_origin, &A->V, &B->V, &C->V, 0);
	if (Abs(dist) < FLT_EPSILON)
	{
		return 1;
	}

	// check if triangle is really triangle (has area > 0)
	// if not simplex can't be expanded and thus no itersection is found
	if (btVec3Eq(&A->V, &B->V) || btVec3Eq(&A->V, &C->V))
	{
		return -1;
	}

	// compute AO vector
	Vector3Copy(&AO, &A->V);
	Vector3Scale(&AO, -1.0f);

	// compute AB and AC segments and ABC vector (perpendircular to triangle)
	Vector3Subtract2(&AB, &B->V, &A->V);
	Vector3Subtract2(&AC, &C->V, &A->V);
	Vector3Cross(&ABC, &AB, &AC);

	Vector3Cross(&tmp, &ABC, &AC);
	dot = Vector3Dot(&tmp, &AO);
	if ((Abs(dot) < FLT_EPSILON) || dot > 0.0f)
	{
		dot = Vector3Dot(&AC, &AO);
		if ((Abs(dot) < FLT_EPSILON) || dot > 0.0f)
		{
			// C is already in place
			SimplexSet(simplex, 1, A);
			SimplexSetSize(simplex, 2);
			TripleCross(&AC, &AO, &AC, dir);
		}
		else
		{
			dot = Vector3Dot(&AB, &AO);
			if ((Abs(dot) < FLT_EPSILON) || dot > 0.0f)
			{
				SimplexSet(simplex, 0, B);
				SimplexSet(simplex, 1, A);
				SimplexSetSize(simplex, 2);
				TripleCross(&AB, &AO, &AB, dir);
			}
			else
			{
				SimplexSet(simplex, 0, A);
				SimplexSetSize(simplex, 1);
				Vector3Copy(dir, &AO);
			}
		}
	}
	else
	{
		Vector3Cross(&tmp, &AB, &ABC);
		dot = Vector3Dot(&tmp, &AO);
		if ((Abs(dot) < FLT_EPSILON) || dot > 0.0f)
		{
			dot = Vector3Dot(&AB, &AO);
			if ((Abs(dot) < FLT_EPSILON) || dot > 0.0f)
			{
				SimplexSet(simplex, 0, B);
				SimplexSet(simplex, 1, A);
				SimplexSetSize(simplex, 2);
				TripleCross(&AB, &AO, &AB, dir);
			}
			else
			{
				SimplexSet(simplex, 0, A);
				SimplexSetSize(simplex, 1);
				Vector3Copy(dir, &AO);
			}
		}
		else
		{
			dot = Vector3Dot(&ABC, &AO);
			if ((Abs(dot) < FLT_EPSILON) || dot > 0.0f)
			{
				Vector3Copy(dir, &ABC);
			}
			else
			{
				SupportVector tmp;
				SupportCopy(&tmp, C);
				SimplexSet(simplex, 0, B);
				SimplexSet(simplex, 1, &tmp);

				Vector3Copy(dir, &ABC);
				Vector3Scale(dir, -1.0f);
			}
		}
	}

	return 0;
}

static int btDoSimplex4(Simplex* simplex, Vector3* dir)
{
	const SupportVector* A, * B, * C, * D;
	Vector3 AO, AB, AC, AD, ABC, ACD, ADB;
	int B_on_ACD, C_on_ADB, D_on_ABC;
	int AB_O, AC_O, AD_O;
	float dist;

	// get last added as A
	A = ccdSimplexLast(simplex);
	// get the other points
	B = SimplexPoint(simplex, 2);
	C = SimplexPoint(simplex, 1);
	D = SimplexPoint(simplex, 0);

	// check if tetrahedron is really tetrahedron (has volume > 0)
	// if it is not simplex can't be expanded and thus no intersection is
	// found
	dist = btVec3PointTriDist2(&A->V, &B->V, &C->V, &D->V, 0);
	if (Abs(dist) < FLT_EPSILON)
	{
		return -1;
	}

	// check if origin lies on some of tetrahedron's face - if so objects
	// intersect
	dist = btVec3PointTriDist2(&ccd_vec3_origin, &A->V, &B->V, &C->V, 0);
	if (Abs(dist) < FLT_EPSILON)
		return 1;
	dist = btVec3PointTriDist2(&ccd_vec3_origin, &A->V, &C->V, &D->V, 0);
	if (Abs(dist) < FLT_EPSILON)
		return 1;
	dist = btVec3PointTriDist2(&ccd_vec3_origin, &A->V, &B->V, &D->V, 0);
	if (Abs(dist) < FLT_EPSILON)
		return 1;
	dist = btVec3PointTriDist2(&ccd_vec3_origin, &B->V, &C->V, &D->V, 0);
	if (Abs(dist) < FLT_EPSILON)
		return 1;

	// compute AO, AB, AC, AD segments and ABC, ACD, ADB normal vectors
	Vector3Copy(&AO, &A->V);
	Vector3Scale(&AO, -1.0f);
	Vector3Subtract2(&AB, &B->V, &A->V);
	Vector3Subtract2(&AC, &C->V, &A->V);
	Vector3Subtract2(&AD, &D->V, &A->V);
	Vector3Cross(&ABC, &AB, &AC);
	Vector3Cross(&ACD, &AC, &AD);
	Vector3Cross(&ADB, &AD, &AB);

	// side (positive or negative) of B, C, D relative to planes ACD, ADB
	// and ABC respectively
	B_on_ACD = ccdSign(Vector3Dot(&ACD, &AB));
	C_on_ADB = ccdSign(Vector3Dot(&ADB, &AC));
	D_on_ABC = ccdSign(Vector3Dot(&ABC, &AD));

	// whether origin is on same side of ACD, ADB, ABC as B, C, D
	// respectively
	AB_O = ccdSign(Vector3Dot(&ACD, &AO)) == B_on_ACD;
	AC_O = ccdSign(Vector3Dot(&ADB, &AO)) == C_on_ADB;
	AD_O = ccdSign(Vector3Dot(&ABC, &AO)) == D_on_ABC;

	if (AB_O && AC_O && AD_O)
	{
		// origin is in tetrahedron
		return 1;
		// rearrange simplex to triangle and call btDoSimplex3()
	}
	else if (!AB_O)
	{
		// B is farthest from the origin among all of the tetrahedron's
		// points, so remove it from the list and go on with the triangle
		// case

		// D and C are in place
		SimplexSet(simplex, 2, A);
		SimplexSetSize(simplex, 3);
	}
	else if (!AC_O)
	{
		// C is farthest
		SimplexSet(simplex, 1, D);
		SimplexSet(simplex, 0, B);
		SimplexSet(simplex, 2, A);
		SimplexSetSize(simplex, 3);
	}
	else
	{  // (!AD_O)
		SimplexSet(simplex, 0, C);
		SimplexSet(simplex, 1, B);
		SimplexSet(simplex, 2, A);
		SimplexSetSize(simplex, 3);
	}

	return btDoSimplex3(simplex, dir);
}

static int btDoSimplex(Simplex* simplex, Vector3* dir)
{
	if (SimplexSize(simplex) == 2)
	{
		// simplex contains segment only one segment
		return btDoSimplex2(simplex, dir);
	}
	else if (SimplexSize(simplex) == 3)
	{
		// simplex contains triangle
		return btDoSimplex3(simplex, dir);
	}
	else
	{  // btSimplexSize(simplex) == 4
		// tetrahedron - this is the only shape which can encapsule origin
		// so btDoSimplex4() also contains test on it
		return btDoSimplex4(simplex, dir);
	}
}
#pragma endregion Static Function

#pragma region GJKPairDetector
void GJKPairDetector::GetClosestPoints(const ClosestPointInput& input, Result& output, class IDebugDraw* debugDraw, bool swapResults)
{
	(void)swapResults;
	GetClosestPointsNonVirtual(input, output, debugDraw);
}
void GJKPairDetector::GetClosestPointsNonVirtual(const ClosestPointInput& input, Result& output, class IDebugDraw* debugDraw)
{
	_cachedSeparatingDistance = 0.f;

	float distance = 0.0f;
	Vector3 normalInB(0.0f, 0.0f, 0.0f);

	Vector3 pointOnA, pointOnB;
	AffineTransform localTransA = input.TransformA;
	AffineTransform localTransB = input.TransformB;
	Vector3 positionOffset = (localTransA.GetTranslation() + localTransB.GetTranslation()) * 0.5f;
	localTransA.SetTranslation(localTransA.GetTranslation() - positionOffset);
	localTransB.SetTranslation(localTransB.GetTranslation() - positionOffset);

	bool check2d = _minkowskiA->IsConvex2D() && _minkowskiB->IsConvex2D();

	float marginA = _marginA;
	float marginB = _marginB;


	//for CCD we don't use margins
	if (_ignoreMargin)
	{
		marginA = 0.0f;
		marginB = 0.0f;
	}

	CurrentIteration = 0;
	int gGjkMaxIter = 1000;  //this is to catch invalid input, perhaps check for #NaN?
	_cachedSeparatingAxis = Vector3(0, 1, 0);

	bool isValid = false;
	bool checkSimplex = false;
	bool checkPenetration = true;
	DegenerateSimplex = 0;

	LastUsedMethod = -1;
	int status = -2;
	Vector3 orgNormalInB(0, 0, 0);
	float   margin = marginA + marginB;

	//we add a separate implementation to check if the convex shapes intersect
	//See also "Real-time Collision Detection with Implicit Objects" by Leif Olvang
	//Todo: integrate the simplex penetration check directly inside the Bullet btVoronoiSimplexSolver
	//and remove this temporary code from libCCD
	//this fixes issue https://github.com/bulletphysics/bullet3/issues/1703
	//note, for large differences in shapes, use double precision build!
	{
		float squaredDistance = FLT_MAX;
		float delta =(0.0f);

		Simplex  simplex1;
		Simplex* simplex = &simplex1;
		SimplexInitialize(simplex);

		Vector3 dir(1, 0, 0);

		{
			Vector3 lastSupV;
			Vector3 supAworld;
			Vector3 supBworld;
			ComputeSupport(_minkowskiA, localTransA, _minkowskiB, localTransB, dir, check2d, supAworld, supBworld, lastSupV);

			SupportVector last;
			last.V  = lastSupV;
			last.V1 = supAworld;
			last.V2 = supBworld;

			SimplexAdd(simplex, &last);

			dir = -lastSupV;

			// start iterations
			for (int iterations = 0; iterations < gGjkMaxIter; iterations++)
			{
				// obtain support point
				ComputeSupport(_minkowskiA, localTransA, _minkowskiB, localTransB, dir, check2d, supAworld, supBworld, lastSupV);

				// check if farthest point in Minkowski difference in direction dir
				// isn't somewhere before origin (the test on negative dot product)
				// - because if it is, objects are not intersecting at all.
				float delta = Dot(lastSupV, dir);
				if (delta < 0)
				{
					//no intersection, besides margin
					status = -1;
					break;
				}

				// add last support vector to simplex
				last.V  = lastSupV;
				last.V1 = supAworld;
				last.V2 = supBworld;

				SimplexAdd(simplex, &last);

				// if btDoSimplex returns 1 if objects intersect, -1 if objects don't
				// intersect and 0 if algorithm should continue

				Vector3 newDir;
				int do_simplex_res = btDoSimplex(simplex, &dir);

				if (do_simplex_res == 1)
				{
					status = 0;  // intersection found
					break;
				}
				else if (do_simplex_res == -1)
				{
					// intersection not found
					status = -1;
					break;
				}

				if (Abs(Vector3Dot(&dir, &dir)) < FLT_EPSILON)
				{
					// intersection not found
					status = -1;
				}

				if (NormSquared(dir) < FLT_EPSILON)
				{
					//no intersection, besides margin
					status = -1;
					break;
				}

				if (NormSquared(dir) < FLT_EPSILON * FLT_EPSILON)
				{
					// intersection not found
					status = -1;
					break;
				}
			}
		}

		_simplexSolver->Reset();
		if (1)
		{
			for (;;)
			{
				Vector3 separatingAxisInA = localTransA.GetBasis() * (-_cachedSeparatingAxis);
				Vector3 separatingAxisInB = localTransB.GetBasis() * _cachedSeparatingAxis;

				Vector3 pInA = _minkowskiA->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInA);
				Vector3 qInB = _minkowskiB->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInB);

				Vector3 pWorld = localTransA * pInA;
				Vector3 qWorld = localTransB * qInB;

				if (check2d)
				{
					pWorld.SetZ(0.f);
					qWorld.SetZ(0.f);
				}

				Vector3 w = pWorld - qWorld;
				delta = Dot(_cachedSeparatingAxis, w);

				// potential exit, they don't overlap
				if ((delta > 0.0f) && (delta * delta > squaredDistance * input.MaximumDistanceSquared))
				{
					DegenerateSimplex = 10;
					checkSimplex = true;
					//checkPenetration = false;
					break;
				}

				//exit 0: the new point is already in the simplex, or we didn't come any closer
				if (_simplexSolver->InSimplex(w))
				{
					DegenerateSimplex = 1;
					checkSimplex = true;
					break;
				}
				// are we getting any closer ?
				float f0 = squaredDistance - delta;
				float f1 = squaredDistance * REL_ERROR2;

				if (f0 <= f1)
				{
					if (f0 <= 0.0f)
					{
						DegenerateSimplex = 2;
					}
					else
					{
						DegenerateSimplex = 11;
					}
					checkSimplex = true;
					break;
				}

				//add current vertex to simplex
				_simplexSolver->AddVertex(w, pWorld, qWorld);
				Vector3 newCachedSeparatingAxis;

				//calculate the closest point to the origin (update vector v)
				if (!_simplexSolver->Closest(newCachedSeparatingAxis))
				{
					DegenerateSimplex = 3;
					checkSimplex = true;
					break;
				}

				if ((float)NormSquared(newCachedSeparatingAxis) < REL_ERROR2)
				{
					_cachedSeparatingAxis = newCachedSeparatingAxis;
					DegenerateSimplex = 6;
					checkSimplex = true;
					break;
				}

				float previousSquaredDistance = squaredDistance;
				squaredDistance = NormSquared(newCachedSeparatingAxis);
#if 0
				///warning: this termination condition leads to some problems in 2d test case see Bullet/Demos/Box2dDemo
				if (squaredDistance > previousSquaredDistance)
				{
					m_degenerateSimplex = 7;
					squaredDistance = previousSquaredDistance;
					checkSimplex = false;
					break;
				}
#endif  //

				//redundant m_simplexSolver->compute_points(pointOnA, pointOnB);

				//are we getting any closer ?
				if (previousSquaredDistance - squaredDistance <= FLT_EPSILON * previousSquaredDistance)
				{
					//				m_simplexSolver->backup_closest(m_cachedSeparatingAxis);
					checkSimplex = true;
					DegenerateSimplex = 12;

					break;
				}

				_cachedSeparatingAxis = newCachedSeparatingAxis;

				//degeneracy, this is typically due to invalid/uninitialized worldtransforms for a btCollisionObject
				if (CurrentIteration++ > gGjkMaxIter)
				{
					break;
				}

				bool check = (!_simplexSolver->FullSimplex());
				//bool check = (!m_simplexSolver->fullSimplex() && squaredDistance > SIMD_EPSILON * m_simplexSolver->maxVertex());

				if (!check)
				{
					//do we need this backup_closest here ?
					//				m_simplexSolver->backup_closest(m_cachedSeparatingAxis);
					DegenerateSimplex = 13;
					break;
				}
			}

			if (checkSimplex)
			{
				_simplexSolver->ComputePoints(pointOnA, pointOnB);
				normalInB = _cachedSeparatingAxis;

				float lenSqr = NormSquared(_cachedSeparatingAxis);

				//valid normal
				if (lenSqr < REL_ERROR2)
				{
					DegenerateSimplex = 5;
				}
				if (lenSqr > FLT_EPSILON * FLT_EPSILON)
				{
					float rlen = 1.0f / Sqrt(lenSqr);
					normalInB = normalInB * rlen;  //normalize

					float s = Sqrt(squaredDistance);

					assert(s > 0.0f);
					pointOnA -= _cachedSeparatingAxis * (marginA / s);
					pointOnB += _cachedSeparatingAxis * (marginB / s);
					distance = (1.0f / rlen) - margin;
					isValid = true;
					orgNormalInB = normalInB;

					LastUsedMethod = 1;
				}
				else
				{
					LastUsedMethod = 2;
				}
			}
		}

		bool catchDegeneratePenetrationCase =
			(CatchDegeneracies && _penetrationDepthSolver && DegenerateSimplex && ((distance + margin) < gGjkEpaPenetrationTolerance));

		//if (checkPenetration && !isValid)
		if ((checkPenetration && (!isValid || catchDegeneratePenetrationCase)) || (status == 0))
		{
			//penetration case

			//if there is no way to handle penetrations, bail out
			if (_penetrationDepthSolver)
			{
				// Penetration depth case.
				Vector3 tmpPointOnA, tmpPointOnB;

				_cachedSeparatingAxis = Vector3(0,0,0);

				bool isValid2 = _penetrationDepthSolver->CalculatePenetrationDepth( *_simplexSolver, _minkowskiA, _minkowskiB, localTransA, localTransB, _cachedSeparatingAxis, tmpPointOnA, tmpPointOnB,debugDraw);

				if (NormSquared(_cachedSeparatingAxis))
				{
					if (isValid2)
					{
						Vector3 tmpNormalInB = tmpPointOnB - tmpPointOnA;
						float lenSqr = NormSquared(tmpNormalInB);
						if (lenSqr <= (FLT_EPSILON * FLT_EPSILON))
						{
							tmpNormalInB = _cachedSeparatingAxis;
							lenSqr = NormSquared(_cachedSeparatingAxis);
						}

						if (lenSqr > (FLT_EPSILON * FLT_EPSILON))
						{
							tmpNormalInB /= Sqrt(lenSqr);
							float distance2 = - Norm((tmpPointOnA - tmpPointOnB));
							LastUsedMethod = 3;
							//only replace valid penetrations when the result is deeper (check)
							if (!isValid || (distance2 < distance))
							{
								distance = distance2;
								pointOnA = tmpPointOnA;
								pointOnB = tmpPointOnB;
								normalInB = tmpNormalInB;
								isValid = true;
							}
							else
							{
								LastUsedMethod = 8;
							}
						}
						else
						{
							LastUsedMethod = 9;
						}
					}
					else

					{
						///this is another degenerate case, where the initial GJK calculation reports a degenerate case
						///EPA reports no penetration, and the second GJK (using the supporting vector without margin)
						///reports a valid positive distance. Use the results of the second GJK instead of failing.
						///thanks to Jacob.Langford for the reproduction case
						///http://code.google.com/p/bullet/issues/detail?id=250

						if (NormSquared(_cachedSeparatingAxis) > 0.0f)
						{
							float distance2 = Norm((tmpPointOnA - tmpPointOnB)) - margin;
							//only replace valid distances when the distance is less
							if (!isValid || (distance2 < distance))
							{
								distance = distance2;
								pointOnA = tmpPointOnA;
								pointOnB = tmpPointOnB;
								pointOnA -= _cachedSeparatingAxis * marginA;
								pointOnB += _cachedSeparatingAxis * marginB;
								normalInB = _cachedSeparatingAxis;
								normalInB = Normalize(normalInB);

								isValid = true;
								LastUsedMethod = 6;
							}
							else
							{
								LastUsedMethod = 5;
							}
						}
					}
				}
				else
				{
					//printf("EPA didn't return a valid value\n");
				}
			}
		}
	}

	if (isValid && ((distance < 0) || (distance * distance < input.MaximumDistanceSquared)))
	{
		_cachedSeparatingAxis = normalInB;
		_cachedSeparatingDistance = distance;
		if (1)
		{
			///todo: need to track down this EPA penetration solver degeneracy
			///the penetration solver reports penetration but the contact normal
			///connecting the contact points is pointing in the opposite direction
			///until then, detect the issue and revert the normal

			float d2 = 0.f;
			{
				Vector3 separatingAxisInA = localTransA.GetBasis() * (-orgNormalInB);
				Vector3 separatingAxisInB = localTransB.GetBasis() * orgNormalInB;

				Vector3 pInA = _minkowskiA->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInA);
				Vector3 qInB = _minkowskiB->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInB);

				Vector3 pWorld = localTransA * pInA;
				Vector3 qWorld = localTransB * qInB;
				Vector3 w = pWorld - qWorld;
				d2 = Dot(orgNormalInB, w) - margin;
			}

			float d1 = 0;
			{
				Vector3 separatingAxisInA = localTransA.GetBasis() * (normalInB);
				Vector3 separatingAxisInB = localTransB.GetBasis() * (-normalInB);

				Vector3 pInA = _minkowskiA->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInA);
				Vector3 qInB = _minkowskiB->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInB);

				Vector3 pWorld = localTransA * pInA;
				Vector3 qWorld = localTransB * qInB;
				Vector3 w = pWorld - qWorld;
				d1 = Dot(-normalInB, w) - margin;
			}
			float d0 = 0.f;
			{
				Vector3 separatingAxisInA = input.TransformA.GetBasis() * (-normalInB);
				Vector3 separatingAxisInB = input.TransformB.GetBasis() * (normalInB);

				Vector3 pInA = _minkowskiA->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInA);
				Vector3 qInB = _minkowskiB->LocalGetSupportVertexWithoutMarginNonVirtual(separatingAxisInB);

				Vector3 pWorld = localTransA * pInA;
				Vector3 qWorld = localTransB * qInB;
				Vector3 w = pWorld - qWorld;
				d0 = Dot(normalInB, w) - margin;
			}

			if (d1 > d0)
			{
				LastUsedMethod = 10;
				normalInB = ( -1) * normalInB;
			}

			if (NormSquared(orgNormalInB))
			{
				if (d2 > d0 && d2 > d1 && d2 > distance)
				{
					normalInB = orgNormalInB;
					distance = d2;
				}
			}
		}

		output.AddContactPoint(
			normalInB,
			pointOnB + positionOffset,
			distance);
	}
	else
	{
		//printf("invalid gjk query\n");
	}
}
#pragma endregion GJKPairDetector
