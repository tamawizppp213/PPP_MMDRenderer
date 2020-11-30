//////////////////////////////////////////////////////////////////////////////////
//              Title:  DirectX12PrimitiveGeometry.cpp
//            Content:  DirectX12 2D or 3D Primitive Geometry
//             Author:  Toide Yutaro
//             Create:  2020_11_23
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/DirectX12PrimitiveGeometry.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using Vertex = VertexPositionNormalTexture;
using namespace DirectX;

//////////////////////////////////////////////////////////////////////////////////
//                            Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*							     Rect
*************************************************************************//**
*  @fn         void GeometryGenerator::Rect(float width, float height)
*  @brief      Create rect vertex and index data (Center is origin)
*  @param[in]  float width
*  @param[in]  float height
*  @param[in]  float depth
*  @return �@�@ MeshData
*****************************************************************************/
MeshData GeometryGenerator::Rect(float width, float height, float depth)
{
	MeshData meshData;

	float w2 = width  / 2;
	float h2 = height / 2;

	meshData.Vertices.resize(4);
	meshData.Indices.resize(6);

	meshData.Vertices[0] = Vertex(XMFLOAT3(-w2, -h2, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));
	meshData.Vertices[1] = Vertex(XMFLOAT3(-w2, +h2, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
	meshData.Vertices[2] = Vertex(XMFLOAT3(+w2, +h2, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	meshData.Vertices[3] = Vertex(XMFLOAT3(+w2, -h2, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));

	meshData.Indices[0] = 0;
	meshData.Indices[1] = 1;
	meshData.Indices[2] = 2;

	meshData.Indices[0] = 0;
	meshData.Indices[1] = 2;
	meshData.Indices[2] = 3;
	return meshData;
}

/****************************************************************************
*							Quadrangle
*************************************************************************//**
*  @fn         void GeometryGenerator::Quadrangle(float x, float y, float w, float h, float depth)
*  @brief      Create Quadrangle vertex and index data 
*  @param[in]  float x
*  @param[in]  float y
*  @param[in]  float z
*  @param[in]  float h
*  @param[in]  float depth
*  @return �@�@ MeshData
*****************************************************************************/
MeshData GeometryGenerator::Quadrangle(float x, float y, float w, float h, float depth)
{
	MeshData meshData;

	meshData.Vertices.resize(4);
	meshData.Indices .resize(6);

	meshData.Vertices[0] = Vertex(XMFLOAT3(x    , y - h, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));
	meshData.Vertices[1] = Vertex(XMFLOAT3(x    , y    , depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
	meshData.Vertices[2] = Vertex(XMFLOAT3(x + w, y    , depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	meshData.Vertices[3] = Vertex(XMFLOAT3(x + w, y - h, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));

	meshData.Indices[0] = 0;
	meshData.Indices[1] = 1;
	meshData.Indices[2] = 2;

	meshData.Indices[0] = 0;
	meshData.Indices[1] = 2;
	meshData.Indices[2] = 3;

	return meshData;
}

/****************************************************************************
*							Box
*************************************************************************//**
*  @fn         void GeometryGenerator::Box(float width, float height, float depth, UINT32 numSubdivisions, bool isInvertNormal)
*  @brief      Create Box vertex and index data (Box center is origin)
*  @param[in]  float width
*  @param[in]  float height
*  @param[in]  float depth
*  @param[in]  UINT32 numSubdivisions
*  @param[in]  bool isInvertNormal
*  @return �@�@ MeshData
*****************************************************************************/
MeshData GeometryGenerator::Box(float width, float height, float depth, UINT32 numSubdivisions, bool isInvertNormal)
{
	// set half width, height and depth
	constexpr int faceCount = 6;
	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * height;

	MeshData meshData;
	Vertex   v[faceCount * 4];
	/*-------------------------------------------------------------------
	-					  Fill Vertex data
	---------------------------------------------------------------------*/
	// Fill in the front face vertex data.
	v[0]  = Vertex(XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));
	v[1]  = Vertex(XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
	v[2]  = Vertex(XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	v[3]  = Vertex(XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));

	// Fill in the back face vertex data.
	v[4]  = Vertex(XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(1.0f, 1.0f));
	v[5]  = Vertex(XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(0.0f, 1.0f));
	v[6]  = Vertex(XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(0.0f, 0.0f));
	v[7]  = Vertex(XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(1.0f, 0.0f));

	// Fill in the top face vertex data.
	v[8]  = Vertex(XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	v[9]  = Vertex(XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	v[10] = Vertex(XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	v[11] = Vertex(XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));

	// Fill in the bottom face vertex data.
	v[12] = Vertex(XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	v[13] = Vertex(XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	v[14] = Vertex(XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	v[15] = Vertex(XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));

	// Fill in the left face vertex data.
	v[16] = Vertex(XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	v[17] = Vertex(XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	v[18] = Vertex(XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	v[19] = Vertex(XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));

	// Fill in the right face vertex data.
	v[20] = Vertex(XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	v[21] = Vertex(XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	v[22] = Vertex(XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	v[23] = Vertex(XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));

	meshData.Vertices.assign(&v[0], &v[24]);
	/*-------------------------------------------------------------------
	-					     Set index data
	---------------------------------------------------------------------*/
	UINT32 index[faceCount * 6];
	// Fill front-> back -> top -> bottom -> left -> right
	for (int i = 0; i < faceCount; ++i)
	{
		index[6 * i + 0] = 4 * i + 0;
		index[6 * i + 1] = 4 * i + 1;
		index[6 * i + 2] = 4 * i + 2;
		index[6 * i + 3] = 4 * i + 0;
		index[6 * i + 4] = 4 * i + 2;
		index[6 * i + 5] = 4 * i + 3;
	}

	meshData.Indices.assign(&index[0], &index[faceCount * 6]);

	/*-------------------------------------------------------------------
	-					     SubDivide 
	---------------------------------------------------------------------*/
	numSubdivisions = std::min<UINT16>(numSubdivisions, 6u);
	for (UINT16 i = 0; i < numSubdivisions; ++i)
	{
		SubDivide(meshData);
	}

	/*-------------------------------------------------------------------
	-					  Invert Normal Vector
	---------------------------------------------------------------------*/
	if (isInvertNormal)
	{
		IsInvertNormal(meshData);
	}

	return meshData;
}

/****************************************************************************
*							Sphere
*************************************************************************//**
*  @fn         void GeometryGenerator::Sphere(float radius, UINT32 sliceCount, UINT32 stackCount, bool isInvertNormal)
*  @brief      Create Sphere vertex and index data (Sphere center is origin)
*  @param[in]  float radius
*  @param[in]  UINT32 sliceCount
*  @param[in]  UINT32 stackCount
*  @param[in]  bool isInvertNormal
*  @return �@�@ MeshData
*****************************************************************************/
MeshData GeometryGenerator::Sphere(float radius, UINT32 sliceCount, UINT32 stackCount, bool isInvertNormal)
{
	MeshData meshData;

	Vertex topVertex    = Vertex(XMFLOAT3(0.0f, +radius, 0.0f), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	Vertex bottomVertex = Vertex(XMFLOAT3(0.0f, -radius, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));

	/*-------------------------------------------------------------------
	-					  Fill Vertex data
	---------------------------------------------------------------------*/
	meshData.Vertices.push_back(topVertex);

	float phiStep   = XM_PI / stackCount;
	float thetaStep = 2.0f * XM_PI / sliceCount;

	for (UINT16 i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Verteies of ring
		for (UINT16 j = 0; j < sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex vertex;

			vertex.Position.x = radius * sinf(phi) * cosf(theta);
			vertex.Position.y = radius * cosf(phi);
			vertex.Position.z = radius * sinf(phi) * cosf(theta);

			XMVECTOR pos = XMLoadFloat3(&vertex.Position);
			XMStoreFloat3(&vertex.Normal, XMVector3Normalize(pos));

			vertex.UV.x = theta / XM_2PI;
			vertex.UV.y = phi   / XM_PI;

			meshData.Vertices.push_back(vertex);
		}
	}

	meshData.Vertices.push_back(bottomVertex);

	/*-------------------------------------------------------------------
	-					    Set index data
	---------------------------------------------------------------------*/
	for (UINT16 i = 1; i <= sliceCount; ++i)
	{
		meshData.Indices.push_back(0);
		meshData.Indices.push_back(i + 1);
		meshData.Indices.push_back(i);
	}

	UINT16 baseIndex = 1;
	UINT16 ringVertexCount = sliceCount + 1;
	for (UINT16 i = 0; i < stackCount - 2; ++i)
	{
		for (UINT16 j = 0; j < sliceCount; ++j)
		{
			meshData.Indices.push_back(baseIndex + i * ringVertexCount + j);
			meshData.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			meshData.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			meshData.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	// South pole vertex was added last.
	UINT16 southPoleIndex = (UINT16)meshData.Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT16 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices.push_back(southPoleIndex);
		meshData.Indices.push_back(baseIndex + i);
		meshData.Indices.push_back(baseIndex + i + 1);
	}

	if (isInvertNormal)
	{
		IsInvertNormal(meshData);
	}
	return meshData;
}

/****************************************************************************
*							GeoSphere
*************************************************************************//**
*  @fn         void GeometryGenerator::GeoSphere(float radius, UINT32 numSubdivisions)
*  @brief      Create geoSphere vertex and index data (GeoSphere center is origin)
*  @param[in]  float radius
*  @param[in]  UINT32 numSubdivisions
*  @param[in]  bool isInvertNormal
*  @return �@�@ MeshData
*****************************************************************************/
MeshData GeometryGenerator::GeoSphere(float radius, UINT32 numSubdivisions, bool isInvertNormal)
{
	MeshData meshData;

	numSubdivisions = std::min<UINT16>(numSubdivisions, 6u);;

	// Approximate a sphere by tessellating an icosahedron
	const float X = 0.525731f;
	const float Z = 0.850651f;

	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};

	UINT16 k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	meshData.Vertices.resize(12);
	meshData.Indices.assign(&k[0], &k[60]);

	for (UINT16 i = 0; i < 12; ++i)
	{
		meshData.Vertices[i].Position = pos[i];
	}

	for (UINT16 i = 0; i < numSubdivisions; ++i)
	{
		SubDivide(meshData);
	}

	for (UINT16 i = 0; i < meshData.Vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&meshData.Vertices[i].Position));
		XMVECTOR p = radius * n;

		XMStoreFloat3(&meshData.Vertices[i].Position, p);
		XMStoreFloat3(&meshData.Vertices[i].Normal, n);

		//Derive texture coordinates from spherical coordinates.
		float theta = atan2f(meshData.Vertices[i].Position.z, meshData.Vertices[i].Position.x);

		if (theta < 0.0f)
		{
			theta += XM_2PI;
		}

		float phi = acosf(meshData.Vertices[i].Position.y / radius);

		meshData.Vertices[i].UV.x = theta / XM_2PI;
		meshData.Vertices[i].UV.y = phi / XM_PI;

	}

	if (isInvertNormal)
	{
		IsInvertNormal(meshData);
	}
	return meshData;
}

/****************************************************************************
*							Cylinder
*************************************************************************//**
*  @fn         void GeometryGenerator::Cylinder(float bottomRadius, float topRadius, float height, UINT32 sliceCount, UINT32 stackCount, bool isInvertNormal)
*  @brief      Create Cylinder vertex and index data (Cylinder center is origin)
*  @param[in]  float bottomRadius
*  @param[in]  float topRadius
*  @param[in]  float height
*  @param[in]  UINT32 sliceCount
*  @param[in]  UINT32 stackCount
*  @param[in]  bool isInvertNormal
*  @return �@�@ MeshData
*****************************************************************************/
MeshData GeometryGenerator::Cylinder(float bottomRadius, float topRadius, float height, UINT32 sliceCount, UINT32 stackCount, bool isInvertNormal)
{
	MeshData meshData;

	float stackHeight = height / stackCount;
	float radiusStep  = (topRadius - bottomRadius) / stackCount;
	UINT32 ringCount  = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT32 i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		// vertices of ring
		float dTheta = 2.0f * XM_PI / sliceCount;
		for (UINT16 j = 0; j <= sliceCount; ++j)
		{
			Vertex vertex;

			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			vertex.Position = XMFLOAT3(r * c, y, r * s);
			vertex.UV.x = (float)j / sliceCount;
			vertex.UV.y = 1.0f - (float)i / stackCount;

			float dr = bottomRadius - topRadius;
			XMFLOAT3 bitangent(dr * c, -height, dr * s);

			XMVECTOR T = XMLoadFloat3(&XMFLOAT3(-s, 0.0f, c));
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
			XMStoreFloat3(&vertex.Normal, N);

			meshData.Vertices.push_back(vertex);
		}
	}

	UINT16 ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (UINT16 i = 0; i < stackCount; ++i)
	{
		for (UINT16 j = 0; j < sliceCount; ++j)
		{
			meshData.Indices.push_back(i * ringVertexCount + j);
			meshData.Indices.push_back((i + 1) * ringVertexCount + j);
			meshData.Indices.push_back((i + 1) * ringVertexCount + j + 1);

			meshData.Indices.push_back(i * ringVertexCount + j);
			meshData.Indices.push_back((i + 1) * ringVertexCount + j + 1);
			meshData.Indices.push_back(i * ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);

	if (isInvertNormal)
	{
		IsInvertNormal(meshData);
	}

	return meshData;
}

/****************************************************************************
*							Grid
*************************************************************************//**
*  @fn         void GeometryGenerator::Grid(float width, float depth, UINT32 rows, UINT32 columns, bool isInvertNormal)
*  @brief      Create grid vertex and index data (Grid center is origin)
*  @param[in]  float width
*  @param[in]  float depth
*  @param[in]  UINT32 rows
*  @param[in]  UINT32 columns
*  @param[in]  bool isInvertNormal
*  @return �@�@ MeshData
*****************************************************************************/
MeshData GeometryGenerator::Grid(float width, float depth, UINT32 rows, UINT32 columns, bool isInvertNormal)
{
	MeshData meshData;

	UINT16 vertexCount = rows * columns;
	UINT16 faceCount   = (rows - 1) * (columns - 1) * 2;

	/*-------------------------------------------------------------------
	-					  Fill Vertex data
	---------------------------------------------------------------------*/
	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (columns - 1);
	float dz = depth / (rows - 1);

	float du = 1.0f / (columns - 1);
	float dv = 1.0f / (rows - 1);

	meshData.Vertices.resize(vertexCount);
	for (UINT16 i = 0; i < rows; ++i)
	{
		float z = halfDepth - i * dz;
		for (UINT16 j = 0; j < columns; ++j)
		{
			float x = -halfWidth + j * dx;

			meshData.Vertices[(UINT64)i * columns + j].Position = XMFLOAT3(x, 0.0f, z);
			meshData.Vertices[(UINT64)i * columns + j].Normal   = XMFLOAT3(0.0f, 1.0f, 0.0f);

			meshData.Vertices[(UINT64)i * columns + j].UV.x = j * du;
			meshData.Vertices[(UINT64)i * columns + j].UV.y = i * dv;
		}
	}

	meshData.Indices.resize((UINT64)faceCount * 3);

	/*-------------------------------------------------------------------
	-					    Set Index data
	---------------------------------------------------------------------*/
	UINT16 k = 0;
	for (UINT16 i = 0; i < rows - 1; ++i)
	{
		for (UINT16 j = 0; j < columns - 1; ++j)
		{
			meshData.Indices[(UINT64)k    ] = (i    ) * columns + (j    );
			meshData.Indices[(UINT64)k + 1] = (i    ) * columns + (j + 1);
			meshData.Indices[(UINT64)k + 2] = (i + 1) * columns + (j    );

			meshData.Indices[(UINT64)k + 3] = (i + 1) * columns + (j    );
			meshData.Indices[(UINT64)k + 4] = (i    ) * columns + (j + 1);
			meshData.Indices[(UINT64)k + 5] = (i + 1) * columns + (j + 1);

			k += 6;
		}
	}

	if (isInvertNormal)
	{
		IsInvertNormal(meshData);
	}

	return meshData;
}

#pragma region Private Function
/****************************************************************************
*							IsInvertNormal
*************************************************************************//**
*  @fn         void GeometryGenerator::IsInvertNormal(MeshData& meshData)
*  @brief      Invert meshData normal vector
*  @param[out] MeshData& meshData
*  @return �@�@ void
*****************************************************************************/
void GeometryGenerator::IsInvertNormal(MeshData& meshData)
{
	for (auto it = meshData.Vertices.begin(); it != meshData.Vertices.end(); ++it)
	{
		it->Normal.x = -it->Normal.x;
		it->Normal.y = -it->Normal.y;
		it->Normal.z = -it->Normal.z;
	}
}

/****************************************************************************
*							SubDivide
*************************************************************************//**
*  @fn         void GeometryGenerator::SubDivide(MeshData& meshData)
*  @brief      Divide each mesh 
*  @param[out] MeshData& meshData
*  @return �@�@ void
*****************************************************************************/
void GeometryGenerator::SubDivide(MeshData& meshData)
{
	using Vertex = VertexPositionNormalTexture;

	// Save a copy of the input geometry
	MeshData inputCopy = meshData;

	meshData.Vertices.resize(0);
	meshData.Indices .resize(0);

	/*-------------------------------------------------------------------
	-					 SubDivide Figure
	---------------------------------------------------------------------*/
	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2
	/*-------------------------------------------------------------------*/

	/*-------------------------------------------------------------------
	-						SubDivide 
	---------------------------------------------------------------------*/
	UINT16 numTris = (UINT16)inputCopy.Indices.size() / 3;
	for (UINT16 i = 0; i < numTris; ++i)
	{
		/*-------------------------------------------------------------------
		-						Set Vertex
		---------------------------------------------------------------------*/
		Vertex vertex0 = inputCopy.Vertices[inputCopy.Indices[(UINT64)i * 3 + 0]];
		Vertex vertex1 = inputCopy.Vertices[inputCopy.Indices[(UINT64)i * 3 + 1]];
		Vertex vertex2 = inputCopy.Vertices[inputCopy.Indices[(UINT64)i * 3 + 2]];

		/*-------------------------------------------------------------------
		-						Set MidPoint
		---------------------------------------------------------------------*/
		Vertex mid0 = MidPoint(vertex0, vertex1);
		Vertex mid1 = MidPoint(vertex1, vertex2);
		Vertex mid2 = MidPoint(vertex0, vertex2);

		/*-------------------------------------------------------------------
		-						Add new geometry
		---------------------------------------------------------------------*/
		// vertex
		meshData.Vertices.push_back(vertex0); // index: 0
		meshData.Vertices.push_back(vertex1); // index: 1
		meshData.Vertices.push_back(vertex2); // index: 2
		meshData.Vertices.push_back(mid0);    // index: 3
		meshData.Vertices.push_back(mid1);    // index: 4
		meshData.Vertices.push_back(mid2);    // index: 5 

		// index
		meshData.Indices.push_back(i * 6 + 0);
		meshData.Indices.push_back(i * 6 + 3);
		meshData.Indices.push_back(i * 6 + 5);

		meshData.Indices.push_back(i * 6 + 3);
		meshData.Indices.push_back(i * 6 + 4);
		meshData.Indices.push_back(i * 6 + 5);

		meshData.Indices.push_back(i * 6 + 5);
		meshData.Indices.push_back(i * 6 + 4);
		meshData.Indices.push_back(i * 6 + 2);

		meshData.Indices.push_back(i * 6 + 3);
		meshData.Indices.push_back(i * 6 + 1);
		meshData.Indices.push_back(i * 6 + 4);

	}
}

VertexPositionNormalTexture GeometryGenerator::MidPoint(const VertexPositionNormalTexture& v0, const VertexPositionNormalTexture& v1)
{
	/*-------------------------------------------------------------------
	-						Load vertex data
	---------------------------------------------------------------------*/
	XMVECTOR p0 = XMLoadFloat3(&v0.Position);
	XMVECTOR p1 = XMLoadFloat3(&v1.Position);
	XMVECTOR n0 = XMLoadFloat3(&v0.Normal);
	XMVECTOR n1 = XMLoadFloat3(&v1.Normal);
	XMVECTOR t0 = XMLoadFloat2(&v0.UV);
	XMVECTOR t1 = XMLoadFloat2(&v1.UV);

	/*-------------------------------------------------------------------
	-						Calcurate mid point
	---------------------------------------------------------------------*/
	XMVECTOR pos    = 0.5f * (p0 + p1);
	XMVECTOR normal = XMVector3Normalize(0.5f * (n0 + n1));
	XMVECTOR uv     = 0.5f * (t0 + t1);

	/*-------------------------------------------------------------------
	-						Set vertex data
	---------------------------------------------------------------------*/
	Vertex vertex;
	XMStoreFloat3(&vertex.Position, pos);
	XMStoreFloat3(&vertex.Normal, normal);
	XMStoreFloat2(&vertex.UV, uv);

	return vertex;
}

/****************************************************************************
*							BuildCylinderTopCap
*************************************************************************//**
*  @fn         void GeometryGenerator::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT32 sliceCount, UINT32 stackCount, MeshData& meshData)
*  @brief      Create Cylinder top cap vertex and index data (GeoSphere center is origin)
*  @param[in]  float bottomRadius,
*  @param[in]  float topRadius,
*  @param[in]  float height,
*  @param[in]  UINT32 sliceCount,
*  @param[in]  UINT32 stackCount,
*  @param[out] MeshData& meshData,
*  @return �@�@ void
*****************************************************************************/
void GeometryGenerator::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT32 sliceCount, UINT32 stackCount, MeshData& meshData)
{
	UINT16 baseIndex = (UINT16)meshData.Vertices.size();

	float y      = 0.5f * height;
	float dTheta = 2.0f * XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (UINT16 i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(Vertex(XMFLOAT3(x, y, z), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(u, v)));

	}
	meshData.Vertices.push_back(Vertex(XMFLOAT3(0.0f, y, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 0.5f)));

	UINT16 centerIndex = (UINT16)meshData.Vertices.size() - 1;

	for (UINT16 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices.push_back(centerIndex);
		meshData.Indices.push_back(baseIndex + i + 1);
		meshData.Indices.push_back(baseIndex + i);
	}
}

/****************************************************************************
*							BuildCylinderBottomCap
*************************************************************************//**
*  @fn         void GeometryGenerator::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT32 sliceCount, UINT32 stackCount, MeshData& meshData)
*  @brief      Create Cylinder bottom cap vertex and index data (GeoSphere center is origin)
*  @param[in]  float bottomRadius,
*  @param[in]  float topRadius,
*  @param[in]  float height,
*  @param[in]  UINT32 sliceCount,
*  @param[in]  UINT32 stackCount,
*  @param[out] MeshData& meshData,
*  @return �@�@ void
*****************************************************************************/
void GeometryGenerator::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT32 sliceCount, UINT32 stackCount, MeshData& meshData)
{
	UINT16 baseIndex = (UINT16)meshData.Vertices.size();

	float y = -0.5f * height;
	float dTheta = 2.0f * XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (UINT16 i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(Vertex(XMFLOAT3(x, y, z), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(u, v)));

	}
	meshData.Vertices.push_back(Vertex(XMFLOAT3(0.0f, y, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.5f, 0.5f)));

	UINT16 centerIndex = (UINT16)meshData.Vertices.size() - 1;

	for (UINT16 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices.push_back(centerIndex);
		meshData.Indices.push_back(baseIndex + i);
		meshData.Indices.push_back(baseIndex + i + 1);
	}
}

#pragma endregion Private Function