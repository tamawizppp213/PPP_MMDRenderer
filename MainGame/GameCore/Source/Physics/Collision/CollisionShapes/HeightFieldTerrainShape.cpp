//////////////////////////////////////////////////////////////////////////////////
///             @file   HeightFieldTerrainShape.hpp
///             @brief  HeightFieldTerrainShape
///             @author Partially edit by Toide
///             @date   2021_09_19
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
#include "../../../../Include/Physics/Collision/CollisionShapes/HeightFieldTerrainShape.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
namespace
{
	struct GridRaycastState
	{
		int X;  // Next quad coords
		int Z;
		int Prev_x;  // Previous quad coords
		int Prev_z;
		float Param;      // Exit param for previous quad
		float PrevParam;  // Enter param for previous quad
		float MaxDistanceFlat;
		float MaxDistance3d;
	};
}

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Static Function
static inline int GetQuantized(float x) 
{ 
	if (x < 0.0f) { return (int)(x - 0.5f); } 
	return (int)(x + 0.5f);
}
static HeightFieldTerrainShape::Range MinMaxRange(float a, float b, float c)
{
	if (a > b)
	{
		if (b > c)
			return HeightFieldTerrainShape::Range(c, a);
		else if (a > c)
			return HeightFieldTerrainShape::Range(b, a);
		else
			return HeightFieldTerrainShape::Range(b, c);
	}
	else
	{
		if (a > c)
			return HeightFieldTerrainShape::Range(c, b);
		else if (b > c)
			return HeightFieldTerrainShape::Range(a, b);
		else
			return HeightFieldTerrainShape::Range(a, c);
	}

}
template <typename ActionT>
//http://www.cse.yorku.ca/~amana/research/grid.pdf
void GridRaycast(ActionT& quadAction, const Vector3& beginPos, const Vector3& endPos, int indices[3])
{
	GridRaycastState rs;
	rs.MaxDistance3d = Distance(beginPos, endPos);
	if (rs.MaxDistance3d < 0.0001){return; }


	float rayDirectionFlatX = endPos.GetElement(indices[0]) - beginPos.GetElement(indices[0]);
	float rayDirectionFlatZ = endPos.GetElement(indices[2]) - beginPos.GetElement(indices[2]);
	rs.MaxDistanceFlat = Sqrt(rayDirectionFlatX * rayDirectionFlatX + rayDirectionFlatZ * rayDirectionFlatZ);

	if (rs.MaxDistanceFlat < 0.0001)
	{
		// Consider the ray vertical
		rayDirectionFlatX = 0;
		rayDirectionFlatZ = 0;
	}
	else
	{
		rayDirectionFlatX /= rs.MaxDistanceFlat;
		rayDirectionFlatZ /= rs.MaxDistanceFlat;
	}

	const int xiStep = rayDirectionFlatX > 0 ? 1 : rayDirectionFlatX < 0 ? -1 : 0;
	const int ziStep = rayDirectionFlatZ > 0 ? 1 : rayDirectionFlatZ < 0 ? -1 : 0;

	const float infinite = 9999999;
	const float paramDeltaX = xiStep != 0 ? 1.f / (float)Abs(rayDirectionFlatX) : infinite;
	const float paramDeltaZ = ziStep != 0 ? 1.f / (float)Abs(rayDirectionFlatZ) : infinite;

	// pos = param * dir
	float paramCrossX;  // At which value of `param` we will cross a x-axis lane?
	float paramCrossZ;  // At which value of `param` we will cross a z-axis lane?

	// paramCrossX and paramCrossZ are initialized as being the first cross
	// X initialization
	if (xiStep != 0)
	{
		if (xiStep == 1)
		{
			paramCrossX = (gm::Ceiling(beginPos.GetElement(indices[0])) - beginPos.GetElement(indices[0])) * paramDeltaX;
		}
		else
		{
			paramCrossX = (beginPos.GetElement(indices[0]) - Floor(beginPos.GetElement(indices[0]))) * paramDeltaX;
		}
	}
	else
	{
		paramCrossX = infinite;  // Will never cross on X
	}

	// Z initialization
	if (ziStep != 0)
	{
		if (ziStep == 1)
		{
			paramCrossZ = (Ceiling(beginPos.GetElement(indices[2])) - beginPos.GetElement(indices[2])) * paramDeltaZ;
		}
		else
		{
			paramCrossZ = (beginPos.GetElement(indices[2]) - Floor(beginPos.GetElement(indices[2]))) * paramDeltaZ;
		}
	}
	else
	{
		paramCrossZ = infinite;  // Will never cross on Z
	}

	rs.X = static_cast<int>(Floor(beginPos.GetElement(indices[0])));
	rs.Z = static_cast<int>(Floor(beginPos.GetElement(indices[2])));

	// Workaround cases where the ray starts at an integer position
	if (paramCrossX == 0.0)
	{
		paramCrossX += paramDeltaX;
		// If going backwards, we should ignore the position we would get by the above flooring,
		// because the ray is not heading in that direction
		if (xiStep == -1)
		{
			rs.X -= 1;
		}
	}

	if (paramCrossZ == 0.0)
	{
		paramCrossZ += paramDeltaZ;
		if (ziStep == -1)
			rs.Z -= 1;
	}

	rs.Prev_x = rs.X;
	rs.Prev_z = rs.Z;
	rs.Param = 0;

	while (true)
	{
		rs.Prev_x = rs.X;
		rs.Prev_z = rs.Z;
		rs.PrevParam = rs.Param;

		if (paramCrossX < paramCrossZ)
		{
			// X lane
			rs.X += xiStep;
			// Assign before advancing the param,
			// to be in sync with the initialization step
			rs.Param = paramCrossX;
			paramCrossX += paramDeltaX;
		}
		else
		{
			// Z lane
			rs.Z += ziStep;
			rs.Param = paramCrossZ;
			paramCrossZ += paramDeltaZ;
		}

		if (rs.Param > rs.MaxDistanceFlat)
		{
			rs.Param = rs.MaxDistanceFlat;
			quadAction(rs);
			break;
		}
		else
		{
			quadAction(rs);
		}
	}
}
#pragma endregion Static Function
#pragma region ProcessTrianglesAction
struct ProcessTrianglesAction
{
	const HeightFieldTerrainShape* Shape;
	bool FlipQuadEdges;
	bool UseDiamondSubdivision;
	int  Width;
	int  Length;
	TriangleCallback* Callback;

	void exec(int x, int z) const
	{
		if (x < 0 || z < 0 || x >= Width || z >= Length) { return; }

		gm::Vector3 vertices[3];

		// TODO Since this is for raycasts, we could greatly benefit from an early exit on the first hit

		// Check quad
		if (FlipQuadEdges || (UseDiamondSubdivision && (((z + x) & 1) > 0)))
		{
			// First triangle
			Shape->GetVertex(x, z, vertices[0]);
			Shape->GetVertex(x + 1, z, vertices[1]);
			Shape->GetVertex(x + 1, z + 1, vertices[2]);
			Callback->ProcessTriangle(vertices, x, z);

			// Second triangle
			Shape->GetVertex(x, z, vertices[0]);
			Shape->GetVertex(x + 1, z + 1, vertices[1]);
			Shape->GetVertex(x, z + 1, vertices[2]);
			Callback->ProcessTriangle(vertices, x, z);
		}
		else
		{
			// First triangle
			Shape->GetVertex(x, z, vertices[0]);
			Shape->GetVertex(x, z + 1, vertices[1]);
			Shape->GetVertex(x + 1, z, vertices[2]);
			Callback->ProcessTriangle(vertices, x, z);

			// Second triangle
			Shape->GetVertex(x + 1, z, vertices[0]);
			Shape->GetVertex(x, z + 1, vertices[1]);
			Shape->GetVertex(x + 1, z + 1, vertices[2]);
			Callback->ProcessTriangle(vertices, x, z);
		}
	}

	void operator()(const GridRaycastState& bs) const
	{
		exec(bs.Prev_x, bs.Prev_z);
	}
};
#pragma endregion ProcessTrianglesAction
#pragma region ProcessVBoundsAction
struct ProcessVBoundsAction
{
	const AlignedObjectArray<HeightFieldTerrainShape::Range>& VBounds;
	int Width;
	int Length;
	int ChunkSize;

	Vector3 RayBegin;
	Vector3 RayEnd;
	Vector3 RayDirection;

	int* Indices;
	ProcessTrianglesAction ProcessTriangles;

	ProcessVBoundsAction(const AlignedObjectArray<HeightFieldTerrainShape::Range>& bnd, int* indices)
		: VBounds(bnd),
		Indices(indices)
	{
	}
	void operator()(const GridRaycastState& rs) const
	{
		int x = rs.Prev_x;
		int z = rs.Prev_z;

		if (x < 0 || z < 0 || x >= Width || z >= Length)
		{
			return;
		}

		const HeightFieldTerrainShape::Range chunk = VBounds[x + z * Width];

		Vector3 enterPos;
		Vector3 exitPos;

		if (rs.MaxDistanceFlat > 0.0001)
		{
			Scalar flatTo3d     = ChunkSize * rs.MaxDistance3d / rs.MaxDistanceFlat;
			Scalar enterParam3d = rs.PrevParam * flatTo3d;
			Scalar exitParam3d  = rs.Param * flatTo3d;
			enterPos            = RayBegin + RayDirection * enterParam3d;
			exitPos             = RayBegin + RayDirection * exitParam3d;

			// We did enter the flat projection of the AABB,
			// but we have to check if we intersect it on the vertical axis
			if (enterPos.GetY() > chunk.Max && exitPos.GetElement(Indices[1]) > chunk.Max)
			{
				return;
			}
			if (enterPos.GetY() < chunk.Min && exitPos.GetElement(Indices[1]) < chunk.Min)
			{
				return;
			}
		}
		else
		{
			// Consider the ray vertical
			// (though we shouldn't reach this often because there is an early check up-front)
			enterPos = RayBegin;
			exitPos = RayEnd;
		}

		GridRaycast(ProcessTriangles, enterPos, exitPos, Indices);
		// Note: it could be possible to have more than one grid at different levels,
		// to do this there would be a branch using a pointer to another ProcessVBoundsAction
	}
};
#pragma endregion ProcessVBoundsAction
HeightFieldTerrainShape::HeightFieldTerrainShape(int heightStickWidth, int heightStickLength, const float* heightFieldData, float heightScale, float minHeight, float maxHeight, int upAxis, bool flipQuedEdges)
{
	_userValue3      = 0;
	_triangleInfoMap = 0;
	Initialize(heightStickWidth, heightStickLength, heightFieldData, 1, minHeight, maxHeight, upAxis, PhyScalarType::Float, flipQuedEdges);
}
HeightFieldTerrainShape::HeightFieldTerrainShape(int heightStickWidth, int heightStickLength,const double* heightFieldData, float heightScale, float minHeight, float maxHeight, int upAxis, bool flipQuedEdges):_userValue3(0), _triangleInfoMap(0)
{
	Initialize(heightStickWidth, heightStickLength, heightFieldData, 1, minHeight, maxHeight, upAxis, PhyScalarType::Double, flipQuedEdges);
}
HeightFieldTerrainShape::HeightFieldTerrainShape(int heightStickWidth, int heightStickLength, const short* heightFieldData, float heightScale, float minHeight, float maxHeight, int upAxis, bool flipQuedEdges) : _userValue3(0), _triangleInfoMap(0)
{
	Initialize(heightStickWidth, heightStickLength, heightFieldData, 1, minHeight, maxHeight, upAxis, PhyScalarType::Short, flipQuedEdges);
}
HeightFieldTerrainShape::HeightFieldTerrainShape(int heightStickWidth, int heightStickLength,const unsigned char* heightFieldData, float heightScale, float minHeight, float maxHeight, int upAxis, bool flipQuedEdges)
{
	Initialize(heightStickWidth, heightStickLength, heightFieldData, 1, minHeight, maxHeight, upAxis, PhyScalarType::Uchar, flipQuedEdges);
}
HeightFieldTerrainShape::HeightFieldTerrainShape(int heightStickWidth, int heightStickLength, const void* heightFieldData, float heightScale, float minHeight, float maxHeight, int upAxis, PhyScalarType type,  bool flipQuedEdges) : _userValue3(0), _triangleInfoMap(0)
{
#ifdef BT_USE_DOUBLE_PRECISION
	if (type == PhyScalarType::Float) type = PhyScalarType::Double;
#endif
	Initialize(heightStickWidth, heightStickLength, heightFieldData, 1, minHeight, maxHeight, upAxis, type, flipQuedEdges);
}
HeightFieldTerrainShape::~HeightFieldTerrainShape()
{
	ClearAccelerator();
}

#pragma region Public Function
/****************************************************************************
*						GetAABB
*************************************************************************//**
*  @fn        void HeightFieldTerrainShape::GetAABB(const gm::Transform& t, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
*  @brief     Get aabb
*  @param[in] Transform& transform
*  @param[out] Vector3 aabbMin
*  @param[out] Vector3 aabbMax
*  @return 　　void
*****************************************************************************/
void HeightFieldTerrainShape::GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const
{
	Vector3 halfExtents = (_localAABBMax - _localAABBMin) * _localScaling * 0.5f;

	// Calculate origin
	Vector3 localOrigin(0, 0, 0);
	localOrigin.SetElement(_upAxis, (_minHeight + _maxHeight) * 0.5f);
	localOrigin = localOrigin * _localScaling;

	gm::Matrix3 abs_b = Absolute(transform.GetBasis());
	Vector3 center = transform.GetTranslation();
	Vector3 extent = Vector3(Dot(halfExtents, abs_b.GetX()), Dot(halfExtents, abs_b.GetY()), Dot(halfExtents, abs_b.GetZ()));
	extent        += Vector3(GetMargin(), GetMargin(), GetMargin());

	aabbMin = center - extent;
	aabbMax = center + extent;
}
/****************************************************************************
*						ProcessAllTriangles
*************************************************************************//**
*  @fn        void HeightFieldTerrainShape::ProcessAllTriangles(TriangleCallback* callback, const Vector3& aabbMin, const Vector3& aabbMax) const
*  @brief     Process all trinangles
*  @param[in] TriangleCallback* callback
*  @param[in] Vector3 aabbMin
*  @param[in] Vector3 aabbMax
*  @return 　　void
*****************************************************************************/
void HeightFieldTerrainShape::ProcessAllTriangles(TriangleCallback* callback, const Vector3& aabbMin, const Vector3& aabbMax) const
{
	Vector3 localAabbMin = aabbMin * Vector3(1.f / _localScaling.GetX(), 1.f / _localScaling.GetY(), 1.f / _localScaling.GetZ());
	Vector3 localAabbMax = aabbMax * Vector3(1.f / _localScaling.GetX(), 1.f / _localScaling.GetY(), 1.f / _localScaling.GetZ());

	// account for local origin
	localAabbMin += _localOrigin;
	localAabbMax += _localOrigin;

	//quantize the aabbMin and aabbMax, and adjust the start/end ranges
	int quantizedAabbMin[3];
	int quantizedAabbMax[3];
	QuantizeWithClamp(quantizedAabbMin, localAabbMin, 0);
	QuantizeWithClamp(quantizedAabbMax, localAabbMax, 1);

	// expand the min/max quantized values
	// this is to catch the case where the input aabb falls between grid points!
	for (int i = 0; i < 3; ++i)
	{
		quantizedAabbMin[i]--;
		quantizedAabbMax[i]++;
	}

	int startX = 0;
	int endX = _heightStickWidth - 1;
	int startJ = 0;
	int endJ = _heightStickLength - 1;

	switch (_upAxis)
	{
		case 0:
		{
			if (quantizedAabbMin[1] > startX)
				startX = quantizedAabbMin[1];
			if (quantizedAabbMax[1] < endX)
				endX = quantizedAabbMax[1];
			if (quantizedAabbMin[2] > startJ)
				startJ = quantizedAabbMin[2];
			if (quantizedAabbMax[2] < endJ)
				endJ = quantizedAabbMax[2];
			break;
		}
		case 1:
		{
			if (quantizedAabbMin[0] > startX)
				startX = quantizedAabbMin[0];
			if (quantizedAabbMax[0] < endX)
				endX = quantizedAabbMax[0];
			if (quantizedAabbMin[2] > startJ)
				startJ = quantizedAabbMin[2];
			if (quantizedAabbMax[2] < endJ)
				endJ = quantizedAabbMax[2];
			break;
		};
		case 2:
		{
			if (quantizedAabbMin[0] > startX)
				startX = quantizedAabbMin[0];
			if (quantizedAabbMax[0] < endX)
				endX = quantizedAabbMax[0];
			if (quantizedAabbMin[1] > startJ)
				startJ = quantizedAabbMin[1];
			if (quantizedAabbMax[1] < endJ)
				endJ = quantizedAabbMax[1];
			break;
		}

		default:
		{
			assert(0);
		}
	}

	// TODO If m_vboundsGrid is available, use it to determine if we really need to process this area

	const Range aabbUpRange(aabbMin.GetElement(_upAxis), aabbMax.GetElement(_upAxis));
	for (int j = startJ; j < endJ; j++)
	{
		for (int x = startX; x < endX; x++)
		{
			Vector3 vertices[3];
			int indices[3] = { 0, 1, 2 };
			if (_flipTriangleWinding)
			{
				indices[0] = 2;
				indices[2] = 0;
			}

			if (_flipQuadEdges || (_useDiamondSubdivision && !((j + x) & 1)) || (_useZigzagSubdivision && !(j & 1)))
			{
				GetVertex(x, j, vertices[indices[0]]);
				GetVertex(x, j + 1, vertices[indices[1]]);
				GetVertex(x + 1, j + 1, vertices[indices[2]]);

				// Skip triangle processing if the triangle is out-of-AABB.
				Range upRange = MinMaxRange(vertices[0].GetElement(_upAxis), vertices[1].GetElement(_upAxis), vertices[2].GetElement(_upAxis));

				if (upRange.Overlaps(aabbUpRange))
					callback->ProcessTriangle(vertices, 2 * x, j);

				// already set: getVertex(x, j, vertices[indices[0]])

				// equivalent to: getVertex(x + 1, j + 1, vertices[indices[1]]);
				vertices[indices[1]] = vertices[indices[2]];

				GetVertex(x + 1, j, vertices[indices[2]]);
				upRange.Min = Min(upRange.Min, vertices[indices[2]].GetElement(_upAxis));
				upRange.Max = Max(upRange.Max, vertices[indices[2]].GetElement(_upAxis));

				if (upRange.Overlaps(aabbUpRange))
					callback->ProcessTriangle(vertices, 2 * x + 1, j);
			}
			else
			{
				GetVertex(x, j, vertices[indices[0]]);
				GetVertex(x, j + 1, vertices[indices[1]]);
				GetVertex(x + 1, j, vertices[indices[2]]);

				// Skip triangle processing if the triangle is out-of-AABB.
				Range upRange = MinMaxRange(vertices[0].GetElement(_upAxis), vertices[1].GetElement(_upAxis), vertices[2].GetElement(_upAxis));

				if (upRange.Overlaps(aabbUpRange))
					callback->ProcessTriangle(vertices, 2 * x, j);

				// already set: getVertex(x, j + 1, vertices[indices[1]]);

				// equivalent to: getVertex(x + 1, j, vertices[indices[0]]);
				vertices[indices[0]] = vertices[indices[2]];

				GetVertex(x + 1, j + 1, vertices[indices[2]]);
				upRange.Min = Min(upRange.Min, vertices[indices[2]].GetElement(_upAxis));
				upRange.Max = Max(upRange.Max, vertices[indices[2]].GetElement(_upAxis));

				if (upRange.Overlaps(aabbUpRange))
					callback->ProcessTriangle(vertices, 2 * x + 1, j);
			}
		}
	}
}
/****************************************************************************
*						CalculateLocalInertia
*************************************************************************//**
*  @fn        void HeightFieldTerrainShape::CalculateLocalInertia(float mass, gm::Vector3& inertia) const
*  @brief     Calculate local inertia
*  @param[in] float mass
*  @param[out] Vector3 inertia
*  @return 　　void
*****************************************************************************/
void HeightFieldTerrainShape::CalculateLocalInertia(float mass, Vector3& inertia) const
{
	inertia = Vector3(0.0f, 0.0f, 0.0f);
}
/****************************************************************************
*						PerformRaycast
*************************************************************************//**
*  @fn        void HeightFieldTerrainShape::PerformRaycast(TriangleCallback* callback, const gm::Vector3& raySource, const gm::Vector3& rayTarget) const
*  @brief     Throw rayCast
*  @param[in] TriangleCallback* callback
*  @param[in] raySource
*  @param[in] rayTarget
*  @return 　　void
*****************************************************************************/
void HeightFieldTerrainShape::PerformRaycast(TriangleCallback* callback, const gm::Vector3& raySource, const gm::Vector3& rayTarget) const
{
	Vector3 beginPosition = raySource / _localScaling;
	Vector3 endPosition   = rayTarget / _localScaling;
	beginPosition        += _localOrigin;
	endPosition          += _localOrigin;

	ProcessTrianglesAction processTriangles;
	processTriangles.Shape                 = this;
	processTriangles.FlipQuadEdges         = _flipQuadEdges;
	processTriangles.UseDiamondSubdivision = _useDiamondSubdivision;
	processTriangles.Callback              = callback;
	processTriangles.Width                 = _heightStickWidth - 1;
	processTriangles.Length                = _heightStickLength - 1;

	int indices[3] = { 0,1,2 };
	if (_upAxis == 2) { indices[1] = 2; indices[2] = 1; }
	int iBeginX = static_cast<int> (Floor(beginPosition.GetElement(indices[0])));
	int iBeginZ = static_cast<int> (Floor(beginPosition.GetElement(indices[2])));
	int iEndX   = static_cast<int> (Floor(endPosition.GetElement(indices[0])));
	int iEndZ   = static_cast<int> (Floor(endPosition.GetElement(indices[2])));

	if (iBeginX == iEndX && iBeginZ == iEndZ)
	{
		processTriangles.exec(iBeginX, iEndZ);
		return;
	}

	if (_boundsGrid.size() == 0)
	{
		GridRaycast(processTriangles, beginPosition, endPosition, &indices[0]);
	}
	else
	{
		Vector3 rayDiffrence = endPosition - beginPosition;
		float flatDistance2 = rayDiffrence.GetElement(indices[0]) * rayDiffrence.GetElement(indices[0]) + rayDiffrence.GetElement(indices[2]) * rayDiffrence.GetElement(indices[2]);
		if (flatDistance2 < _boundChunkSize * _boundChunkSize)
		{
			GridRaycast(processTriangles, beginPosition, endPosition, &indices[0]);
		}

		ProcessVBoundsAction processVBounds(_boundsGrid, &indices[0]);
		processVBounds.Width            = _boundGridWidth;
		processVBounds.Length           = _boundGridLength;
		processVBounds.RayBegin         = beginPosition;
		processVBounds.RayEnd           = endPosition;
		processVBounds.RayDirection     = Normalize(rayDiffrence);
		processVBounds.ProcessTriangles = processTriangles;
		processVBounds.ChunkSize        = _boundChunkSize;

		GridRaycast(processVBounds, beginPosition / _boundChunkSize, endPosition / _boundChunkSize, indices);
	}
}
/****************************************************************************
*						BuildAccelerator
*************************************************************************//**
*  @fn        void HeightFieldTerrainShape::BuildAccelerator()
*  @brief     Build bounds grid
*  @param[in] int chunkSize
*  @return 　　void
*****************************************************************************/
void HeightFieldTerrainShape::BuildAccelerator(int chunkSize)
{
	if (chunkSize <= 0) { ClearAccelerator(); return; }

	_boundChunkSize = chunkSize;
	int chunkX = _heightStickWidth / chunkSize;
	int chunkZ = _heightStickLength / chunkSize;

	if (_heightStickWidth % chunkSize > 0 ) { ++chunkX; }
	if (_heightStickLength % chunkSize > 0) { ++chunkZ; }

	if (_boundGridWidth != chunkX || _boundGridLength != chunkZ)
	{
		ClearAccelerator();
		_boundGridWidth = chunkX;
		_boundGridLength = chunkZ;
	}
	if (chunkX == 0 || chunkZ == 0) { return; }

	// This data structure is only reallocated if the required size changed
	_boundsGrid.resize(chunkX * chunkZ);

	// Compute min and max height for all chunks
	for (int cz = 0; cz < chunkZ; ++cz)
	{
		int z0 = cz * chunkSize;

		for (int cx = 0; cx < chunkX; ++cx)
		{
			int x0 = cx * chunkSize;

			Range r;

			r.Min = GetRawHeightFieldValue(x0, z0);
			r.Max = r.Min;

			// Compute min and max height for this chunk.
			// We have to include one extra cell to account for neighbors.
			// Here is why:
			// Say we have a flat terrain, and a plateau that fits a chunk perfectly.
			//
			//   Left        Right
			// 0---0---0---1---1---1
			// |   |   |   |   |   |
			// 0---0---0---1---1---1
			// |   |   |   |   |   |
			// 0---0---0---1---1---1
			//           x
			//
			// If the AABB for the Left chunk did not share vertices with the Right,
			// then we would fail collision tests at x due to a gap.
			//
			for (int z = z0; z < z0 + chunkSize + 1; ++z)
			{
				if (z >= _heightStickLength) { continue; }

				for (int x = x0; x < x0 + chunkSize + 1; ++x)
				{
					if (x >= _heightStickWidth) { continue; }
					float height = GetRawHeightFieldValue(x, z);

					if (height < r.Min)
					{
						r.Min = height;
					}
					else if (height > r.Max)
					{
						r.Max = height;
					}
				}
			}

			_boundsGrid[cx + cz * chunkX] = r;
		}
	}
}
/****************************************************************************
*						ClearAccelerator
*************************************************************************//**
*  @fn        void HeightFieldTerrainShape::ClearAccelerator()
*  @brief     Clear bounds grid
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void HeightFieldTerrainShape::ClearAccelerator()
{
	_boundsGrid.clear();
}
/****************************************************************************
*						GetVertex
*************************************************************************//**
*  @fn        void  HeightFieldTerrainShape::GetVertex(int x, int y, gm::Vector3& vertex) const
*  @brief     Get vertex
*  @param[in] int x
*  @param[in] int y
*  @param[out] Vector3 vertex
*  @return 　　void
*****************************************************************************/
void  HeightFieldTerrainShape::GetVertex(int x, int y, Vector3& vertex) const
{
	assert(x >= 0); assert(y >= 0); assert(x < _heightStickWidth);  assert(y < _heightStickLength);

	float height = GetRawHeightFieldValue(x, y);
	switch (_upAxis)
	{
		case 0: { vertex = Vector3( height - _localOrigin.GetX(), (-_width / 2.0f) + x, (-_length / 2.0f) + y); break; }
		case 1: { vertex = Vector3( (-_width / 2.0f) + x, height - _localOrigin.GetY(), (-_length / 2.0f) + y); break; };
		case 2: { vertex = Vector3( (-_width / 2.0f) + x, (-_length / 2.0f) + y, height - _localOrigin.GetZ()); break; }
		default: { assert(0); }
	}

	vertex *= _localScaling;
}
/****************************************************************************
*						GetLocalScaling
*************************************************************************//**
*  @fn        const Vector3& HeightFieldTerrainShape::GetLocalScaling() const
*  @brief     Get local scaling
*  @param[in] void
*  @return 　　const Vector3&
*****************************************************************************/
const Vector3& HeightFieldTerrainShape::GetLocalScaling() const
{
	return _localScaling;
}
/****************************************************************************
*						SetLocalScaling
*************************************************************************//**
*  @fn        void HeightFieldTerrainShape::SetLocalScaling(const gm::Vector3& scaling)
*  @brief     Set local scaling
*  @param[in] Vector3 scaling
*  @return 　　void
*****************************************************************************/
void HeightFieldTerrainShape::SetLocalScaling(const gm::Vector3& scaling)
{
	_localScaling = scaling;
}

#pragma endregion Public Function
#pragma region Private Function
void HeightFieldTerrainShape::Initialize(int heightStickWidth, int heightStickLength, const void* heightFieldData, float heightScale, float minHeight, float maxHeight, int upAxis, PhyScalarType heightDataType, bool flipQuadEdges)
{
	// validation
	assert(heightStickWidth > 1);   // && "bad width");
	assert(heightStickLength > 1);  // && "bad length");
	assert(heightFieldData);        // && "null heightfield data");
	// btAssert(heightScale) -- do we care?  Trust caller here
	assert(minHeight <= maxHeight);                                    // && "bad min/max height");
	assert(upAxis >= 0 && upAxis < 3);                                 // && "bad upAxis--should be in range [0,2]");
	assert(heightDataType != PhyScalarType::Uchar || heightDataType != PhyScalarType::Float || heightDataType != PhyScalarType::Double || heightDataType != PhyScalarType::Short);  // && "Bad height data type enum");

	// initialize member variables
	_shapeType              = (int)BroadPhaseNativeTypes::Terrain_Shape;
	_heightStickWidth       = heightStickWidth;
	_heightStickLength      = heightStickLength;
	_minHeight              = minHeight;
	_maxHeight              = maxHeight;
	_width                  = (heightStickWidth - 1);
	_length                 =(heightStickLength - 1);
	_heightScale            = heightScale;
	_heightFieldDataUnknown = heightFieldData;
	_heightDataType         = heightDataType;
	_flipQuadEdges          = flipQuadEdges;
	_useDiamondSubdivision  = false;
	_useZigzagSubdivision   = false;
	_flipTriangleWinding    = false;
	_upAxis                 = upAxis;
	_localScaling           = Vector3(1.0f, 1.0f, 1.0f);

	_boundChunkSize  = 0;
	_boundGridWidth  = 0;
	_boundGridLength = 0;

	// determine min/max axis-aligned bounding box (aabb) values
	switch (_upAxis)
	{
	case 0:
	{
		_localAABBMin = Vector3(_minHeight, 0, 0);
		_localAABBMax = Vector3(_maxHeight, _width, _length);
		break;
	}
	case 1:
	{
		_localAABBMin = Vector3(0, _minHeight, 0);
		_localAABBMax = Vector3(_width, _maxHeight, _length);
		break;
	};
	case 2:
	{
		_localAABBMin = Vector3(0, 0, _minHeight);
		_localAABBMax = Vector3(_width, _length, _maxHeight);
		break;
	}
	default:
	{
		//need to get valid m_upAxis
		assert(0);  // && "Bad m_upAxis");
	}
	}

	// remember origin (defined as exact middle of aabb)
	_localOrigin = 0.5f * (_localAABBMin + _localAABBMax);
}
/****************************************************************************
*						GetRawHeightFieldValue
*************************************************************************//**
*  @fn        float HeightFieldTerrainShape::GetRawHeightFieldValue(int x, int y) const
*  @brief     This returns the "raw" (user's initial) height, not the actual height.
*             The actual height needs to be adjusted to be relative to the center of the heigthfield's AABB
*  @param[in] int x
*  @param[in] int y
*  @return 　　float 
*****************************************************************************/
float HeightFieldTerrainShape::GetRawHeightFieldValue(int x, int y) const
{
	float value = 0.0f;
	switch (_heightDataType)
	{
		case PhyScalarType::Float : { value = _heightFieldDataFloat [(y * _heightStickWidth) + x]; break; }
		case PhyScalarType::Double: { value = _heightFieldDataDouble[(y * _heightStickWidth) + x]; break; }
		case PhyScalarType::Uchar: { unsigned char heigthFieldValue = _heightFieldDataUnsignedChar[(y * _heightStickWidth) + x]; value = heigthFieldValue * _heightScale; break; }
		case PhyScalarType::Short: { short         heigthFieldValue = _heightFieldDataShort       [(y * _heightStickWidth) + x]; value = heigthFieldValue * _heightScale; break; }
		default: { assert("Bad _heightDataType"); }
	}
	return value;
}
/****************************************************************************
*						GetRawHeightFieldValue
*************************************************************************//**
*  @fn        QuantizeWithClamp
*  @brief     This routine is basically determining the gridpoint indices for a given
  input vector, answering the question: "which gridpoint is closest to the provided point?".
  "with clamp" means that we restrict the point to be in the heightfield's axis-aligned bounding box.
*  @param[out] int* out 
*  @param[in] Vector3 point
*  @param[in] isMax
*  @return 　　void
*****************************************************************************/
void HeightFieldTerrainShape::QuantizeWithClamp(int* out, const gm::Vector3& point, int isMax) const
{
	Vector3 clampedPoint(point);
	clampedPoint = Max(clampedPoint, _localAABBMin);
	clampedPoint = Min(clampedPoint, _localAABBMax);

	out[0] = GetQuantized(clampedPoint.GetX());
	out[1] = GetQuantized(clampedPoint.GetY());
	out[2] = GetQuantized(clampedPoint.GetZ());
}

#pragma endregion Private Function