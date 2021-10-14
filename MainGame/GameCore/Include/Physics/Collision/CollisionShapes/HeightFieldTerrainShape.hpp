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
#pragma once
#ifndef HEIGHT_FIELD_TERRAIN_SHAPE_HPP
#define HEIGHT_FIELD_TERRAIN_SHAPE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "ConcaveShape.hpp"
#include "TriangleInfoMap.hpp"
#include "GameMath/Include/GMAlignedAllocatorArray.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         ConvexPointCloudSha@e
//////////////////////////////////////////////////////////////////////////////////
///btHeightfieldTerrainShape simulates a 2D heightfield terrain
/**
  The caller is responsible for maintaining the heightfield array; this
  class does not make a copy.

  The heightfield can be dynamic so long as the min/max height values
  capture the extremes (heights must always be in that range).

  The local origin of the heightfield is assumed to be the exact
  center (as determined by width and length and height, with each
  axis multiplied by the localScaling).

  \b NOTE: be careful with coordinates.  If you have a heightfield with a local
  min height of -100m, and a max height of +500m, you may be tempted to place it
  at the origin (0,0) and expect the heights in world coordinates to be
  -100 to +500 meters.
  Actually, the heights will be -300 to +300m, because bullet will re-center
  the heightfield based on its AABB (which is determined by the min/max
  heights).  So keep in mind that once you create a btHeightfieldTerrainShape
  object, the heights will be adjusted relative to the center of the AABB.  This
  is different to the behavior of many rendering engines, but is useful for
  physics engines.

  Most (but not all) rendering and heightfield libraries assume upAxis = 1
  (that is, the y-axis is "up").  This class allows any of the 3 coordinates
  to be "up".  Make sure your choice of axis is consistent with your rendering
  system.

  The heightfield heights are determined from the data type used for the
  heightfieldData array.

   - unsigned char: height at a point is the uchar value at the
	   grid point, multipled by heightScale.  uchar isn't recommended
	   because of its inability to deal with negative values, and
	   low resolution (8-bit).

   - short: height at a point is the short int value at that grid
	   point, multipled by heightScale.

   - float or dobule: height at a point is the value at that grid point.

  Whatever the caller specifies as minHeight and maxHeight will be honored.
  The class will not inspect the heightfield to discover the actual minimum
  or maximum heights.  These values are used to determine the heightfield's
  axis-aligned bounding box, multiplied by localScaling.

  For usage and testing see the TerrainDemo.
 */
/****************************************************************************
*				  			HeightFieldTerrainShape
*************************************************************************//**
*  @class     HeightFieldTerrainShape
*  @brief     HeightFieldTerrainShape
*****************************************************************************/
__declspec(align(16))
class HeightFieldTerrainShape :public ConcaveShape
{
public:
	struct Range
	{
		Range() {};
		Range(float min, float max) : Min(min), Max(max) {};

		bool Overlaps(const Range& other) const {return !(Min > other.Max || Max < other.Min);}
		float Min;
		float Max;
	};
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual void GetAABB(const gm::AffineTransform& transform, gm::Vector3& aabbMin, gm::Vector3& aabbMax) const override;
	virtual void ProcessAllTriangles(TriangleCallback* callback, const gm::Vector3& aabbMin, const gm::Vector3& aabbMax) const override;
	virtual void CalculateLocalInertia(float mass, gm::Vector3& inertia) const;
	void PerformRaycast(TriangleCallback* callback, const gm::Vector3& raySource, const gm::Vector3& rayTarget) const;
	void BuildAccelerator(int chunkSize = 16);
	void ClearAccelerator();


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void  GetVertex(int x, int y, gm::Vector3& vertex) const;
	int   GetUpAxis     () const { return _upAxis; }
	float GetUserValue3()  const { return _userValue3; }
	const struct TriangleInfoMap* GetTriangleInfoMap   () const { return _triangleInfoMap; }
	struct       TriangleInfoMap* GetTriangleInfoMap   ()       { return _triangleInfoMap; }
	const unsigned char*          GetHeightFieldRawData() const { return _heightFieldDataUnsignedChar; }

	void SetUseDiamondSubdivision(bool useDiamondSubdivision = true) { _useDiamondSubdivision = useDiamondSubdivision; }
	void SetUseZigzagSubdivision (bool useZigzagSubdivision  = true) { _useZigzagSubdivision; }///could help compatibility with Ogre heightfields. See https://code.google.com/p/bullet/issues/detail?id=625
	void SetFilpTriangleWinding  (bool flipTriangleWinding)          { _flipTriangleWinding = flipTriangleWinding; }
	void SetUserValue3           (float value)                       { _userValue3          = value; }
	void SetTriangleInfoMap(TriangleInfoMap* map) { _triangleInfoMap = map; }

	virtual const gm::Vector3& GetLocalScaling()       const override;
	virtual void SetLocalScaling(const gm::Vector3& scaling) override;
	virtual const char* GetName() const { return "HEIGHTFIELD"; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DECLARE_ALIGNED_ALLOCATOR;
	HeightFieldTerrainShape(int heightStickWidth, int heightStickLength,
		const float* heightFieldData, float heightScale,
		float minHeight, float maxHeight, int upAxis, bool flipQuedEdges);
	HeightFieldTerrainShape(int heightStickWidth, int heightStickLength,
		const double* heightFieldData, float heightScale,
		float minHeight, float maxHeight, int upAxis, bool flipQuedEdges);
	HeightFieldTerrainShape(int heightStickWidth, int heightStickLength,
		const short* heightFieldData, float heightScale,
		float minHeight, float maxHeight, int upAxis, bool flipQuedEdges);
	HeightFieldTerrainShape(int heightStickWidth, int heightStickLength,
		const unsigned char* heightFieldData, float heightScale,
		float minHeight, float maxHeight, int upAxis, bool flipQuedEdges);
	HeightFieldTerrainShape(int heightStickWidth, int heightStickLength,
		const void* heightFieldData, float heightScale,
		float minHeight, float maxHeight, int upAxis, PhyScalarType type, bool flipQuedEdges);
	virtual ~HeightFieldTerrainShape();

protected:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void Initialize(int heightStickWidth, int heightStickLength,
		const void* heightFieldData, float heightScale,
		float minHeight, float maxHeight, int upAxis, PhyScalarType heightDataType, bool flipQuadEdges);
	virtual float GetRawHeightFieldValue(int x, int y) const;
	void          QuantizeWithClamp(int* out, const gm::Vector3& point, int isMax) const;

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	gm::Vector3 _localAABBMin;
	gm::Vector3 _localAABBMax;
	gm::Vector3 _localOrigin;

	int _heightStickWidth;
	int _heightStickLength;
	float _minHeight;
	float _maxHeight;
	float _width;
	float _length;
	float _heightScale;
	union
	{
		const unsigned char* _heightFieldDataUnsignedChar;
		const short* _heightFieldDataShort;
		const float* _heightFieldDataFloat;
		const double* _heightFieldDataDouble;
		const void* _heightFieldDataUnknown;
	};

	PhyScalarType _heightDataType;
	bool _flipQuadEdges;
	bool _useDiamondSubdivision;
	bool _useZigzagSubdivision;
	bool _flipTriangleWinding;
	int _upAxis;

	gm::Vector3 _localScaling;

	gm::AlignedObjectArray<Range> _boundsGrid;
	int _boundGridWidth;
	int _boundGridLength;
	int _boundChunkSize;

	float _userValue3;

	TriangleInfoMap* _triangleInfoMap;
};
///btHeightfieldTerrainShape simulates a 2D heightfield terrain
/**
  The caller is responsible for maintaining the heightfield array; this
  class does not make a copy.

  The heightfield can be dynamic so long as the min/max height values
  capture the extremes (heights must always be in that range).

  The local origin of the heightfield is assumed to be the exact
  center (as determined by width and length and height, with each
  axis multiplied by the localScaling).

  \b NOTE: be careful with coordinates.  If you have a heightfield with a local
  min height of -100m, and a max height of +500m, you may be tempted to place it
  at the origin (0,0) and expect the heights in world coordinates to be
  -100 to +500 meters.
  Actually, the heights will be -300 to +300m, because bullet will re-center
  the heightfield based on its AABB (which is determined by the min/max
  heights).  So keep in mind that once you create a btHeightfieldTerrainShape
  object, the heights will be adjusted relative to the center of the AABB.  This
  is different to the behavior of many rendering engines, but is useful for
  physics engines.

  Most (but not all) rendering and heightfield libraries assume upAxis = 1
  (that is, the y-axis is "up").  This class allows any of the 3 coordinates
  to be "up".  Make sure your choice of axis is consistent with your rendering
  system.

  The heightfield heights are determined from the data type used for the
  heightfieldData array.

   - unsigned char: height at a point is the uchar value at the
	   grid point, multipled by heightScale.  uchar isn't recommended
	   because of its inability to deal with negative values, and
	   low resolution (8-bit).

   - short: height at a point is the short int value at that grid
	   point, multipled by heightScale.

   - float or dobule: height at a point is the value at that grid point.

  Whatever the caller specifies as minHeight and maxHeight will be honored.
  The class will not inspect the heightfield to discover the actual minimum
  or maximum heights.  These values are used to determine the heightfield's
  axis-aligned bounding box, multiplied by localScaling.

  For usage and testing see the TerrainDemo.
 */
#endif