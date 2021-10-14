//////////////////////////////////////////////////////////////////////////////////
///             @file   JjkEpa.hpp
///             @brief  GJK _ EPA Algorithm class
///             @author Partially edit by Toide
///             @date   2021_10_14
//////////////////////////////////////////////////////////////////////////////////
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2008 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the
use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software in a
product, an acknowledgment in the product documentation would be appreciated
but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

/*
GJK-EPA collision solver by Nathanael Presson, 2008
*/

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "../../../../Include/Physics/Collision/NarrowPhaseCollision/GjkEpa2.hpp"
#include "../../../../Include/Physics/Collision/CollisionShapes/SphereShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

namespace gjkepa2_impl
{
#define GJK_MAX_ITERATIONS (128)

/*-------------------------------------------------------------------
-           GJK Parameter
---------------------------------------------------------------------*/
#ifdef  BT_USE_DOUBLE_PRECISION
#define GJK_ACCURACY ((btScalar)1e-12)
#define GJK_MIN_DISTANCE ((btScalar)1e-12)
#define GJK_DUPLICATED_EPS ((btScalar)1e-12)
#else
#define GJK_ACCURACY       (0.0001f)
#define GJK_MIN_DISTANCE   (0.0001f)
#define GJK_DUPLICATED_EPS (0.0001f)
#endif  //BT_USE_DOUBLE_PRECISION

#define GJK_SIMPLEX2_EPS (0.0f)
#define GJK_SIMPLEX3_EPS (0.0f)
#define GJK_SIMPLEX4_EPS (0.0f)


/*-------------------------------------------------------------------
-           EPA Parameter
---------------------------------------------------------------------*/
#define EPA_MAX_VERTICES   (128)
#define EPA_MAX_ITERATIONS (255)

#ifdef  BT_USE_DOUBLE_PRECISION
#define EPA_ACCURACY ((btScalar)1e-12)
#define EPA_PLANE_EPS ((btScalar)1e-14)
#define EPA_INSIDE_EPS ((btScalar)1e-9)
#else
#define EPA_ACCURACY ((Scalar)0.0001)
#define EPA_PLANE_EPS ((Scalar)0.00001)
#define EPA_INSIDE_EPS ((Scalar)0.01)
#endif

#define EPA_FALLBACK (10 * EPA_ACCURACY)
#define EPA_MAX_FACES (EPA_MAX_VERTICES * 2)

/*-------------------------------------------------------------------
-           Shorthands
---------------------------------------------------------------------*/
using UINT32 = unsigned int;
using UINT   = unsigned int;
using UINT8  = unsigned char;


//////////////////////////////////////////////////////////////////////////////////
//                               Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Minkowski Difference
/****************************************************************************
*				  			Minkowski Difference
*************************************************************************//**
*  @class     Minkowski Difference
*  @brief     Minkowski Difference support class
*****************************************************************************/
struct MinkowskiDifference
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void EnableMargin(bool enable)
	{
		if (enable) { Ls = &ConvexShape::LocalGetSupportVertexNonVirtual; }
		else        { Ls = &ConvexShape::LocalGetSupportVertexWithoutMarginNonVirtual; }
	}
	inline Vector3 Support0(const Vector3& d) const { return (((Shapes[0])->*(Ls))(d)); }
	inline Vector3 Support1(const Vector3& d) const { return (ToShape0 * ((Shapes[1])->*(Ls))(ToShape1 * d)); }
	inline Vector3 Support(const Vector3& d) const { return (Support0(d) - Support1(-d)); }
	Vector3 Support(const Vector3& d, UINT32 index) const
	{
		if (index) { return Support1(d); }
		else       { return Support0(d); }
	}

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const ConvexShape* Shapes[2];
	Matrix3            ToShape1;
	AffineTransform    ToShape0;

#ifdef __SPU__
	bool _enableMargin;
#else
	Vector3(ConvexShape::* Ls)(const Vector3&) const;
#endif
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	MinkowskiDifference() = default;
};

using TShape = MinkowskiDifference;
#pragma endregion Minkowski Difference
#pragma region GJK Calculate
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class GJK
{
public:
	/****************************************************************************
	**                Struct , Enum
	*****************************************************************************/
	struct SV      { Vector3 D, W; };
	struct Simplex { SV* C[4]; float P[4]; UINT32 Rank; };
	enum class EStatus
	{
		Valid,
		Inside,
		Failed
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void    Initialize();
	EStatus Evaluate(const TShape& shapeArg, const Vector3& guess);
	bool    EncloseOrigin();
	
	void GetSupport(const Vector3& d, SV& sv) const;
	void RemoveVertices(Simplex& simplex) { Free[NFree++] = simplex.C[--simplex.Rank]; }
	void AppendVertices(Simplex& simplex, const Vector3& vector);

	static float ProjectOrigin(const Vector3& a, const Vector3& b, Scalar* w, UINT& m);
	static float ProjectOrigin(const Vector3& a, const Vector3& b, const Vector3& c, Scalar* w, UINT& m);
	static float ProjectOrigin(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, Scalar* w, UINT& m);

	static float Det(const Vector3& a, const Vector3& b, const Vector3& c)
	{
		return (a.GetY() * b.GetZ() * c.GetX() + a.GetZ() * b.GetX() * c.GetY() -
			a.GetX() * b.GetZ() * c.GetY() - a.GetY() * b.GetX() * c.GetZ() +
			a.GetX() * b.GetY() * c.GetZ() - a.GetZ() * b.GetY() * c.GetX());
	}
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	TShape   Shape;
	Vector3  Ray;
	float    Distance;
	Simplex  Simplices[2];
	SV       Store[4];
	SV*      Free[4];
	UINT     NFree;
	UINT     Current;
	Simplex* SimplexPtr;
	EStatus  Status;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	GJK()
	{
		Initialize();
	}

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#pragma region Implement
/****************************************************************************
*						Initialize
*************************************************************************//**
*  @fn        void GJK::Initialize()
*  @brief     Initialize 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void GJK::Initialize()
{
	Ray      = Vector3(0,0,0);
	NFree    = 0;
	Status   = EStatus::Failed;
	Current  = 0;
	Distance = 0;
}
/****************************************************************************
*						Evaluate
*************************************************************************//**
*  @fn        GJK::EStatus GJK::Evaluate(const TShape& shapeArg, const Vector3& guess)
*  @brief     Evaluate (return EStatus)
*  @param[in] TShape = MinkowskiDifference
*  @param[in] Vector3 guess
*  @return 　　EStatus
*****************************************************************************/
GJK::EStatus GJK::Evaluate(const TShape& shapeArg, const Vector3& guess)
{
	UINT    iterations = 0;
	float   sqrtDist   = 0;
	float   alpha      = 0;
	Vector3 lastW[4];
	UINT    clastW = 0;

	/*-------------------------------------------------------------------
	-           Initialize solver
	---------------------------------------------------------------------*/
	Free[0]  = &Store[0]; Free[1] = &Store[1]; Free[2] = &Store[2]; Free[3] = &Store[3];
	NFree    = 4;
	Current  = 0;
	Status   = EStatus::Valid;
	Shape    = shapeArg;
	Distance = 0;

	/*-------------------------------------------------------------------
	-           Initialize simplex
	---------------------------------------------------------------------*/
	Simplices[0].Rank = 0;
	Ray = guess;
	const float sqrl = NormSquared(Ray);
	AppendVertices(Simplices[0], sqrl > 0 ? -Ray : Vector3(1, 0, 0));
	Simplices[0].P[0] = 1;
	Ray = Simplices[0].C[0]->W;

	sqrtDist = sqrl;
	lastW[0] = lastW[1] = lastW[2] = lastW[3] = Ray;

	/*-------------------------------------------------------------------
	-           Loop
	---------------------------------------------------------------------*/
	do
	{
		const UINT next = 1 - Current;
		Simplex& cs = Simplices[Current];
		Simplex& ns = Simplices[next];

		/*-------------------------------------------------------------------
		-           Check zero
		---------------------------------------------------------------------*/
		const float rl = Norm(Ray);
		if (rl < GJK_MIN_DISTANCE)
		{
			Status = EStatus::Inside; break; // Touching or  inside
		}

		/*-------------------------------------------------------------------
		-           Append new vertices in -vertex direction
		---------------------------------------------------------------------*/
		AppendVertices(cs, -Ray);
		const Vector3& w = cs.C[cs.Rank - 1]->W;
		bool found = false;
		for (UINT i = 0; i < 4; ++i)
		{
			if (NormSquared(w - lastW[i]) < GJK_DUPLICATED_EPS)
			{
				found = true; break;
			}
		}

		if (found)
		{
			RemoveVertices(Simplices[Current]); break; // return old simplex
		}
		else
		{
			lastW[clastW = ((clastW + 1) & 3)] = w; // update last w
		}

		/*-------------------------------------------------------------------
		-           Check for termination
		---------------------------------------------------------------------*/
		const float omega = Dot(Ray, w) / rl;
		alpha = Max(omega, alpha);
		if (((rl - alpha) - (GJK_ACCURACY * rl)) <= 0)
		{ /* Return old simplex				*/
			RemoveVertices(Simplices[Current]); break;
		}
		/*-------------------------------------------------------------------
		-           Reduce simplex
		---------------------------------------------------------------------*/

		Scalar weights[4];
		UINT  mask = 0;
		switch (cs.Rank)
		{
			case 2: { sqrtDist = ProjectOrigin(cs.C[0]->W, cs.C[1]->W, weights, mask); break; }
			case 3: { sqrtDist = ProjectOrigin(cs.C[0]->W, cs.C[1]->W, cs.C[2]->W , weights, mask); break; }
			case 4: { sqrtDist = ProjectOrigin(cs.C[0]->W, cs.C[1]->W, cs.C[2]->W ,cs.C[3]->W, weights, mask); break; }
		}

		if (sqrtDist >= 0)
		{
			ns.Rank = 0;
			Ray = Vector3(0, 0, 0);
			Current = next;
			for (UINT i = 0, ni = cs.Rank; i < ni; ++i)
			{
				if (mask & (1 << i))
				{
					ns.C[ns.Rank] = cs.C[i];
					ns.P[ns.Rank++] = weights[i];
					Ray += cs.C[i]->W * weights[i];
				}
				else
				{
					Free[NFree++] = cs.C[i];
				}
			}
			if (mask == 15) Status = EStatus::Inside;
		}
		else
		{
			RemoveVertices(Simplices[Current]); break;
		}
		Status = ((++iterations) < GJK_MAX_ITERATIONS) ? Status : EStatus::Failed;

	} while (Status == EStatus::Valid);

	SimplexPtr = &Simplices[Current];

	switch (Status)
	{
		case EStatus::Valid:  { Distance = Norm(Ray); break; }
		case EStatus::Inside: { Distance = 0; break; }
		default: { break; }
	}
	return Status;
}
/****************************************************************************
*						EncloseOrigin
*************************************************************************//**
*  @fn        bool GJK::EncloseOrigin()
*  @brief     --------
*  @param[in] Vector3 d
*  @param[in] SV sv
*  @return 　　bool
*****************************************************************************/
bool GJK::EncloseOrigin()
{
	switch (SimplexPtr->Rank)
	{
	case 1:
	{
		for (UINT i = 0; i < 3; ++i)
		{
			Vector3 axis = Vector3(0, 0, 0);
			axis.SetElement(i, 1);
			AppendVertices(*SimplexPtr, axis);
			if (EncloseOrigin()) return (true);
			RemoveVertices(*SimplexPtr);
			AppendVertices(*SimplexPtr, -axis);
			if (EncloseOrigin()) return (true);
			RemoveVertices(*SimplexPtr);
		}
	}
	break;
	case 2:
	{
		const Vector3 d = SimplexPtr->C[1]->W - SimplexPtr->C[0]->W;
		for (UINT i = 0; i < 3; ++i)
		{
			Vector3 axis = Vector3(0, 0, 0);
			axis.SetElement(i ,1);
			const Vector3 p = Cross(d, axis);
			if (NormSquared(p) > 0.0f)
			{
				AppendVertices(*SimplexPtr, p);
				if (EncloseOrigin()) { return (true); }
				RemoveVertices(*SimplexPtr);
				AppendVertices(*SimplexPtr, -p);
				if (EncloseOrigin()) return (true);
				RemoveVertices(*SimplexPtr);
			}
		}
	}
	break;
	case 3:
	{
		const Vector3 n = Cross(SimplexPtr->C[1]->W - SimplexPtr->C[0]->W,
			SimplexPtr->C[2]->W - SimplexPtr->C[0]->W);
		if (NormSquared(n) > 0.0f)
		{
			AppendVertices(*SimplexPtr, n);
			if (EncloseOrigin()) return (true);
			RemoveVertices(*SimplexPtr);
			AppendVertices(*SimplexPtr, -n);
			if (EncloseOrigin()) return (true);
			RemoveVertices(*SimplexPtr);
		}
	}
	break;
	case 4:
	{
		if (Abs(Det(SimplexPtr->C[0]->W - SimplexPtr->C[3]->W,
			SimplexPtr->C[1]->W - SimplexPtr->C[3]->W,
			SimplexPtr->C[2]->W - SimplexPtr->C[3]->W)) > 0.0f)
			return (true);
	}
	break;
	}
	return (false);
}
/****************************************************************************
*						GetSupport
*************************************************************************//**
*  @fn        void GJK::GetSupport(const Vector3& d, SV& sv) const
*  @brief     Get support plane
*  @param[in] Vector3 d
*  @param[in] SV sv
*  @return 　　void
*****************************************************************************/
void GJK::GetSupport(const Vector3& d, SV& sv) const
{
	sv.D = d / Norm(d);
	sv.W = Shape.Support(sv.D);
}
/****************************************************************************
*						AppendVertices
*************************************************************************//**
*  @fn        void GJK::AppendVertices(GJK::Simplex& simplex, const Vector3& vector)
*  @brief     Append simplex vertices
*  @param[in] GJK::Simplex simplex
*  @param[in] Vector3 vector
*  @return 　　void
*****************************************************************************/
void GJK::AppendVertices(GJK::Simplex& simplex, const Vector3& vector)
{
	simplex.P[simplex.Rank] = 0;
	simplex.C[simplex.Rank] = Free[--NFree];
	GetSupport(vector, *simplex.C[simplex.Rank++]);
}
/****************************************************************************
*						ProjectOrigin
*************************************************************************//**
*  @fn        float GJK::ProjectOrigin(const Vector3& a, const Vector3& b, Scalar* w, UINT& m)
*  @brief     Project origin
*  @param[in] GJK::Simplex simplex
*  @param[in] Vector3 vector
*  @return 　　void
*****************************************************************************/
float GJK::ProjectOrigin(const Vector3& a, const Vector3& b, Scalar* w, UINT& m)
{
	const Vector3 d = b - a;
	const Scalar  l = NormSquared(d);
	if ((float)l > GJK_SIMPLEX2_EPS)
	{
		const float t = ((float)l > 0 ? (float)-Dot(a, d) /(float) l : 0.0f);
		if      (t >= 1) { w[0] = 0; w[1] = 1; m = 2; return NormSquared(b); }
		else if (t <= 0) { w[0] = 1; w[1] = 0; m = 1; return NormSquared(a); }
		else             { w[0] = 1.0f - (w[1] = t); m = 3; return (NormSquared(a + d * t)); }
	}
	return -1;
}
float GJK::ProjectOrigin(const Vector3& a, const Vector3& b, const Vector3& c, Scalar* w, UINT& m)
{
	static const UINT imd3  [] = { 1,2,0 };
	const Vector3*    vertex[] = { &a, &b, &c };
	const Vector3     dl    [] = { a - b, b - c, c - a };
	const Vector3     n        = Cross(dl[0], dl[1]);
	const Scalar      l        = NormSquared(n);

	if (l > GJK_SIMPLEX3_EPS)
	{
		Scalar minDistance = -1;
		Scalar subw[2]     = { 0.0f, 0.0f };
		UINT   subm        = 0;

		for (UINT i = 0; i < 3; ++i)
		{
			if (Dot(*vertex[i], Cross(dl[i], n)) > 0.0f)
			{
				const UINT j = imd3[i];
				const Scalar subd(ProjectOrigin(*vertex[i], *vertex[j], subw, subm));
				if (((float)minDistance < 0.0f) || ((float)subd < minDistance))
				{
					minDistance = subd;
					m = static_cast<UINT>(((subm & 1) ? 1 << i : 0) + ((subm & 2) ? 1 << j : 0));
					w[i] = subw[0];
					w[j] = subw[1];
					w[imd3[j]] = 0;
				}
			}
		}
		if (minDistance < 0.0f)
		{
			const Scalar  d = Dot(a, n);
			const Scalar  s = Sqrt(l);
			const Vector3 p = n * (d / l);
			minDistance = NormSquared(p);
			m = 7;
			w[0] = Norm(Cross(dl[1], b - p)) / s;
			w[1] = Norm(Cross(dl[2], c - p)) / s;
			w[2] = 1.0f - (w[0] + w[1]);
		}
		return minDistance;
	}
	return (-1);
}
float GJK::ProjectOrigin(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, Scalar* w, UINT& m)
{
	static const UINT imd3[] = { 1, 2, 0 };
	const Vector3* vertex [] = { &a, &b, &c, &d };
	const Vector3  dl     [] = { a - d, b - d, c - d };
	const Scalar   vl        = Det(dl[0], dl[1], dl[2]);
	const float    ng        = (vl * Dot(a, Cross(b - c, a - b))) <= 0.0f;

	if (ng && (Abs(vl) > GJK_SIMPLEX4_EPS))
	{
		Scalar minDistance = -1.0f;
		Scalar subw[3]     = { 0.0f, 0.0f, 0.0f };
		UINT   subm        = 0;
		
		for (UINT i = 0; i < 3; ++i)
		{
			const UINT   j = imd3[i];
			const Scalar s = vl * Dot(d, Cross(dl[i], dl[i]));
			if (s > 0.0f)
			{
				const Scalar subd = ProjectOrigin(*vertex[i], *vertex[i], d, subw, subm);
				if ((minDistance < 0.0f) || ((float)subd < minDistance))
				{
					minDistance = subd;
					m = static_cast<UINT>((subm & 1 ? 1 << i : 0) + (subm & 2 ? 1 << j : 0) + (subm & 4 ? 8 : 0));
					w[i] = subw[0];
					w[j] = subw[1];
					w[imd3[j]] = 0;
					w[3] = subw[2];
				}
			}
		}

		if (minDistance < 0.0f)
		{
			minDistance = 0.0f;
			m           = 15;
			w[0]        = Det(c, b, d) / vl;
			w[1]        = Det(a, c, d) / vl;
			w[2]        = Det(b, a, d) / vl;
			w[3]        = 1.0f - (w[0] + w[1] + w[2]);
		}
		return minDistance;

	}
	return (-1);
}

#pragma endregion Implement
#pragma endregion GJK Calculate
#pragma region EPA
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/

	struct EPA
	{
	public:
		using SV = GJK::SV;
		struct Face
		{
			Vector3 N;
			Scalar  D;
			SV* C[3];
			Face* F[3];
			Face* L[2];
			UINT8   E[3];
			UINT8   Pass;
		};
		struct List
		{
			Face* Root;
			UINT32 Count;
			List() : Root(0), Count(0) {};
		};
		struct Horizon
		{
			Face* CF;
			Face* FF;
			UINT  NF;
			Horizon() : CF(0), FF(0), NF(0) {};
		};
		enum class EStatus
		{
			Valid,
			Touching,
			Degenerated,
			NonConvex,
			InvalidHull,
			OutOfFaces,
			OutOfVertices,
			AccuraryReached,
			FallBack,
			Failed
		};
		/****************************************************************************
		**                Public Function
		*****************************************************************************/
		void    Initialize();
		EStatus Evaluate(GJK& gjk, const Vector3& guess);
		bool    GetEdgeDistance(Face* face, SV* a, SV* b, Scalar& distance);
		Face* NewFace(SV* a, SV* b, SV* c, bool forced);
		Face* FindBest();
		bool    Expand(UINT pass, SV* w, Face* f, UINT e, Horizon& horizon);

		static inline void Bind(Face* fa, UINT ea, Face* fb, UINT eb)
		{
			fa->E[ea] = (UINT)eb;
			fa->F[ea] = fb;
			fb->E[eb] = (UINT)ea;
			fb->F[eb] = fa;
		}
		static inline void Append(List& list, Face* face)
		{
			face->L[0] = 0;
			face->L[1] = list.Root;
			if (list.Root)
			{
				list.Root->L[0] = face;
			}
			list.Root = face;
			++list.Count;
		}
		static inline void Remove(List& list, Face* face)
		{
			if (face->L[1]) { face->L[1]->L[0] = face->L[0]; }
			if (face->L[0]) { face->L[0]->L[1] = face->L[1]; }
			if (face == list.Root) { list.Root = face->L[1]; }
			--list.Count;
		}
		/****************************************************************************
		**                Public Member Variables
		*****************************************************************************/
		EStatus      Status;
		GJK::Simplex Result;
		Vector3      Normal;
		Scalar       Depth;
		SV           SVStore[EPA_MAX_VERTICES];
		Face         FCStore[EPA_MAX_FACES];
		UINT         NextSV;
		List         Hull;
		List         Stock;

		/****************************************************************************
		**                Constructor and Destructor
		*****************************************************************************/
		EPA() { Initialize(); }
	private:
		/****************************************************************************
		**                Private Function
		*****************************************************************************/

		/****************************************************************************
		**                Private Member Variables
		*****************************************************************************/
	};

	void         EPA::Initialize()
	{
		Status = EStatus::Failed;
		Normal = Vector3(0, 0, 0);
		Depth = 0;
		NextSV = 0;
		for (UINT i = 0; i < EPA_MAX_FACES; ++i) { Append(Stock, &FCStore[EPA_MAX_FACES - i - 1]); }
	}
	EPA::EStatus EPA::Evaluate(GJK& gjk, const Vector3& guess)
	{
		GJK::Simplex& simplex = *gjk.SimplexPtr;
		if ((simplex.Rank > 1) && gjk.EncloseOrigin())
		{
			/*-------------------------------------------------------------------
			-           Clear up
			---------------------------------------------------------------------*/
			while (Hull.Root)
			{
				Face* face = Hull.Root;
				Remove(Hull, face);
				Append(Stock, face);
			}
			Status = EStatus::Valid;
			NextSV = 0;

			/*-------------------------------------------------------------------
			-           Orient simplex
			---------------------------------------------------------------------*/
			if (gjk.Det(simplex.C[0]->W - simplex.C[3]->W, simplex.C[1]->W - simplex.C[3]->W, simplex.C[2]->W - simplex.C[3]->W) < 0)
			{
				std::swap(simplex.C[0], simplex.C[1]);
				std::swap(simplex.P[0], simplex.P[1]);
			}

			/*-------------------------------------------------------------------
			-           Build initial hull
			---------------------------------------------------------------------*/
			Face* tetra[] = { NewFace(simplex.C[0], simplex.C[1], simplex.C[2], true), NewFace(simplex.C[1], simplex.C[0], simplex.C[3], true), NewFace(simplex.C[2], simplex.C[1], simplex.C[3], true), NewFace(simplex.C[0], simplex.C[2], simplex.C[3], true) };
			if (Hull.Count == 4)
			{
				Face* best = FindBest();
				Face  outer = *best;
				UINT  pass = 0;
				UINT  iterations = 0;
				Bind(tetra[0], 0, tetra[1], 0);
				Bind(tetra[0], 1, tetra[2], 0);
				Bind(tetra[0], 2, tetra[3], 0);
				Bind(tetra[1], 1, tetra[3], 2);
				Bind(tetra[1], 2, tetra[2], 1);
				Bind(tetra[2], 2, tetra[3], 1);
				Status = EStatus::Valid;

				for (; iterations < EPA_MAX_ITERATIONS; ++iterations)
				{
					if (NextSV < EPA_MAX_VERTICES)
					{
						Horizon horizon;
						SV* w = &SVStore[NextSV++];
						bool valid = true;
						best->Pass = (UINT8)(++pass);

						gjk.GetSupport(best->N, *w);
						const Scalar wdist = Dot(best->N, w->W) - best->D;
						if ((float)wdist > EPA_ACCURACY)
						{
							for (UINT j = 0; (j < 3) && valid; ++j)
							{
								valid &= Expand(pass, w, best->F[j], best->E[j], horizon);
							}
							if (valid && (horizon.NF >= 3))
							{
								Bind(horizon.CF, 1, horizon.FF, 2);
								Remove(Hull, best);
								Append(Stock, best);
								best = FindBest();
								outer = *best;
							}
							else
							{
								Status = EStatus::InvalidHull;
								break;
							}
						}
						else
						{
							Status = EStatus::AccuraryReached;
							break;
						}
					}
					else
					{
						Status = EStatus::OutOfVertices;
						break;
					}
				}
				const Vector3 projection = outer.N * outer.D;
				Normal = outer.N;
				Depth = outer.D;
				Result.Rank = 3;
				Result.C[0] = outer.C[0];
				Result.C[1] = outer.C[1];
				Result.C[2] = outer.C[2];
				Result.P[0] = Norm(Cross(outer.C[1]->W - projection, outer.C[2]->W - projection));
				Result.P[1] = Norm(Cross(outer.C[2]->W - projection, outer.C[0]->W - projection));
				Result.P[2] = Norm(Cross(outer.C[0]->W - projection, outer.C[1]->W - projection));
				const Scalar sum = Result.P[0] + Result.P[1] + Result.P[2];
				Result.P[0] /= sum;
				Result.P[1] /= sum;
				Result.P[2] /= sum;
				return (Status);
			}
		}
		Status = EStatus::FallBack;
		Normal = -guess;
		const Scalar nl = Norm(Normal);
		if (nl > 0.0f)
			Normal = Normal / nl;
		else
			Normal = Vector3(1, 0, 0);
		Depth = 0;
		Result.Rank = 1;
		Result.C[0] = simplex.C[0];
		Result.P[0] = 1;
		return (Status);
	}
	bool         EPA::GetEdgeDistance(Face* face, SV* a, SV* b, Scalar& distance)
	{
		const Vector3 ba = b->W - a->W;
		const Vector3 n_ab = Cross(ba, face->N);  // Outward facing edge normal direction, on triangle plane
		const Scalar a_dot_nab = Dot(a->W, n_ab);  // Only care about the sign to determine inside/outside, so not normalization required

		if (a_dot_nab < 0.0f)
		{
			// Outside of edge a->b
			const Scalar ba_l2 = NormSquared(ba);
			const Scalar a_dot_ba = Dot(a->W, ba);
			const Scalar b_dot_ba = Dot(b->W, ba);

			if (a_dot_ba > 0.0f)
			{
				// Pick distance vertex a
				distance = Norm(a->W);
			}
			else if (b_dot_ba < 0.0f)
			{
				// Pick distance vertex b
				distance = Norm(b->W);
			}
			else
			{
				// Pick distance to edge a->b
				const Scalar a_dot_b = Dot(a->W, b->W);
				distance = Sqrt(Max((NormSquared(a->W) * NormSquared(b->W) - a_dot_b * a_dot_b) / ba_l2, (Scalar)0));
			}

			return true;
		}

		return false;
	}
	EPA::Face* EPA::NewFace(SV* a, SV* b, SV* c, bool forced)
	{
		if (Stock.Root)
		{
			Face* face = Stock.Root;
			Remove(Stock, face);
			Append(Hull, face);
			face->Pass = 0;
			face->C[0] = a;
			face->C[1] = b;
			face->C[2] = c;
			face->N = Cross(b->W - a->W, c->W - a->W);
			const Scalar l = Norm(face->N);
			const bool v = (float)l > EPA_ACCURACY;

			if (v)
			{
				if (!(GetEdgeDistance(face, a, b, face->D) || GetEdgeDistance(face, b, c, face->D) || GetEdgeDistance(face, c, a, face->D)))
				{
					// Origin projects to the interior of the triangle
					// Use distance to triangle plane
					face->D = Dot(a->W, face->N) / l;
				}

				face->N /= l;
				if ((float)forced || ((float)face->D >= -EPA_PLANE_EPS)) { return face; }
				else { Status = EStatus::NonConvex; }
			}
			else
			{
				Status = EStatus::Degenerated;
			}

			Remove(Hull, face);
			Append(Stock, face);
			return 0;
		}
		Status = Stock.Root ? EStatus::OutOfVertices : EStatus::OutOfFaces;
		return 0;
	}
	EPA::Face* EPA::FindBest()
	{
		Face* minf = Hull.Root;
		Scalar mind = minf->D * minf->D;
		for (Face* f = minf->L[1]; f; f = f->L[1])
		{
			const Scalar sqd = f->D * f->D;
			if ((float)sqd < mind)
			{
				minf = f;
				mind = sqd;
			}
		}
		return (minf);
	}
	bool EPA::Expand(UINT pass, SV* w, Face* f, UINT e, Horizon& horizon)
	{
		static const UINT i1m3[] = { 1, 2, 0 };
		static const UINT i2m3[] = { 2, 0, 1 };
		if (f->Pass != pass)
		{
			const UINT e1 = i1m3[e];
			if ((float)(Dot(f->N, w->W) - f->D) < -EPA_PLANE_EPS)
			{
				Face* nf = NewFace(f->C[e1], f->C[e], w, false);
				if (nf)
				{
					Bind(nf, 0, f, e);
					if (horizon.CF)
						Bind(horizon.CF, 1, nf, 2);
					else
						horizon.FF = nf;
					horizon.CF = nf;
					++horizon.NF;
					return (true);
				}
			}
			else
			{
				const UINT e2 = i2m3[e];
				f->Pass = (UINT8)pass;
				if (Expand(pass, w, f->F[e1], f->E[e1], horizon) &&
					Expand(pass, w, f->F[e2], f->E[e2], horizon))
				{
					Remove(Hull, f);
					Append(Stock, f);
					return (true);
				}
			}
		}
		return (false);
	}

}

using namespace gjkepa2_impl;
static void Initialize(const ConvexShape* shape0, const AffineTransform& worldTransform0, const ConvexShape* shape1, const AffineTransform& worldTransform1, GjkEpaSolver2::Result& results, TShape& shape, bool withMargins)
{
	results.Witnesses[0] = results.Witnesses[1] = Vector3(0, 0, 0);
	results.Status       = GjkEpaSolver2::Result::Separated;
	shape.Shapes[0]      = shape0;
	shape.Shapes[1]      = shape1;
	shape.ToShape1       = Transpose(gm::HadamaldProduct(worldTransform1.GetBasis(), worldTransform0.GetBasis()));
	shape.ToShape0       = AffineTransformUtil::InverseTimes(worldTransform0, worldTransform1);
}
#pragma endregion EPA
/****************************************************************************
*						RemoveVertex
*************************************************************************//**
*  @fn        void VolonoiSimplexSolver::RemoveVertex(int index)
*  @brief     Remove vertex is targeted by index
*  @param[in] int index
*  @return 　　void
*****************************************************************************/
#pragma region GJKEPASolver
int GjkEpaSolver2::StackSizeRequirement() { return sizeof(GJK) + sizeof(EPA); }
bool  GjkEpaSolver2::Distance(const ConvexShape* shape0, const AffineTransform& worldTransform0, const ConvexShape* shape1, const AffineTransform& worldTransform1, const Vector3& guess, Result& results)
{
	TShape shape;
	Initialize(shape0, worldTransform0, shape1, worldTransform1, results, shape, false);

	GJK gjk;
	GJK::EStatus gjkStatus = gjk.Evaluate(shape, guess);
	if (gjkStatus == GJK::EStatus::Valid)
	{
		Vector3 w0 = Vector3(0, 0, 0); 
		Vector3 w1 = Vector3(0, 0, 0);
		for (UINT i = 0; i < gjk.SimplexPtr->Rank; ++i)
		{
			const Scalar p = gjk.SimplexPtr->P[i];
			w0 += shape.Support(gjk.SimplexPtr->C[i]->D, 0) * p;
			w1 += shape.Support(-gjk.SimplexPtr->C[i]->D, 1) * p;
		}
		results.Witnesses[0] = worldTransform0 * w0;
		results.Witnesses[1] = worldTransform1 * w1;
		results.Normal = w0 - w1;
		results.Distance = Norm(results.Normal);
		results.Normal = results.Normal /  (results.Distance > GJK_MIN_DISTANCE ? results.Distance : 1);
		return true;
	}
	else
	{
		results.Status = (gjkStatus == GJK::EStatus::Inside) ? Result::Penetrating : Result::GJK_Failed;
		return false;
	}
}
bool  GjkEpaSolver2::Penetration(const ConvexShape* shape0, const AffineTransform& worldTransform0, const ConvexShape* shape1, const AffineTransform& worldTransform1, const Vector3& guess, Result& results, bool useMargins)
{
	TShape shape;
	Initialize(shape0, worldTransform0, shape1, worldTransform1, results, shape, useMargins);
	
	GJK gjk;
	GJK::EStatus gjkStatus = gjk.Evaluate(shape, -guess);
	switch (gjkStatus)
	{
		case GJK::EStatus::Inside:
		{
			EPA epa;
			EPA::EStatus epa_status = epa.Evaluate(gjk, -guess);
			if (epa_status != EPA::EStatus::Failed)
			{
				Vector3 w0 = Vector3(0, 0, 0);
				for (UINT i = 0; i < epa.Result.Rank; ++i)
				{
					w0 += shape.Support(epa.Result.C[i]->D, 0) * epa.Result.P[i];
				}
				results.Status = Result::Penetrating;
				results.Witnesses[0] = worldTransform0 * w0;
				results.Witnesses[1] = worldTransform0 * (w0 - epa.Normal * epa.Depth);
				results.Normal   = -epa.Normal;
				results.Distance = -epa.Depth;
				return (true);
			}
			else
			{
				results.Status = Result::EPA_Failed;
				break;
			}
		}
		case GJK::EStatus::Failed:
		{
			results.Status = Result::GJK_Failed;
			break;
		}
		default:
		{
			break;
		}
	}
	return (false);
}
float GjkEpaSolver2::SignedDistance(const Vector3& position, float margin, const ConvexShape* shape0, const AffineTransform& worldTransform, Result& results)
{
	TShape shape;
	SphereShape shape1(margin);
	AffineTransform wtrs1(Quaternion(Vector4(0,0,0,1)), position);
	Initialize(shape0, worldTransform, &shape1, wtrs1, results, shape, false);
	GJK gjk;
	GJK::EStatus gjkStatus = gjk.Evaluate(shape, Vector3(1, 1, 1));
	if (gjkStatus == GJK::EStatus::Valid)
	{
		Vector3 w0 = Vector3(0, 0, 0);
		Vector3 w1 = Vector3(0, 0, 0);
		for (UINT i = 0; i < gjk.SimplexPtr->Rank; ++i)
		{
			const Scalar p = gjk.SimplexPtr->P[i];
			w0 += shape.Support(gjk.SimplexPtr->C[i]->D, 0) * p;
			w1 += shape.Support(-gjk.SimplexPtr->C[i]->D, 1) * p;
		}
		results.Witnesses[0] = worldTransform * w0;
		results.Witnesses[1] = worldTransform * w1;
		const Vector3 delta = results.Witnesses[1] -
			results.Witnesses[0];
		const Scalar margin = shape0->GetMarginNonVirtual() + shape1.GetMarginNonVirtual();
		const Scalar length = Norm(delta);
		results.Normal = delta / length;
		results.Witnesses[0] += results.Normal * margin;
		results.Distance = length - margin;
		return results.Distance;
	}
	else
	{
		if (gjkStatus == GJK::EStatus::Inside)
		{
			if (Penetration(shape0, worldTransform, &shape1, wtrs1, gjk.Ray, results))
			{
				const Vector3 delta = results.Witnesses[0] -
					results.Witnesses[1];
				const Scalar length = Norm(delta);
				if (length >= FLT_EPSILON)
					results.Normal = delta / length;
				return (-length);
			}
		}
	}
	return (FLT_MAX);
}
bool  GjkEpaSolver2::SignedDistance(const ConvexShape* shape0, const gm::AffineTransform& worldTransform0, const ConvexShape* shape1, const AffineTransform& worldTransform1, const Vector3& guess, Result& results)
{
	if (!Distance(shape0, worldTransform0, shape1, worldTransform1, guess, results))
		return (Penetration(shape0, worldTransform0, shape1, worldTransform1, guess, results, false));
	else
		return (true);
}
#pragma endregionGJKEPASolver