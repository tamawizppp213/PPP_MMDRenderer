xof 0303txt 0032
template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}


Mesh {
 11;
 -4.865921;-2.737605;-0.002522;,
 4.862751;-2.737605;-0.002522;,
 -4.865921;2.738283;-0.002522;,
 4.862751;2.738283;-0.002522;,
 -0.504782;0.565319;0.002400;,
 -0.504782;0.565319;0.002400;,
 -0.504782;0.565319;0.002400;,
 -0.504782;0.565319;0.002400;,
 -0.504782;0.565319;0.002400;,
 -0.504782;0.565319;0.002400;,
 -0.504782;0.565319;0.002400;;
 6;
 3;0,2,3;,
 3;3,1,0;,
 3;4,5,6;,
 3;6,7,4;,
 3;4,8,9;,
 3;9,10,4;;

 MeshNormals {
  11;
  0.000000;0.000000;-1.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;0.000000;0.000000;,
  0.000000;0.000000;0.000000;,
  0.000000;0.000000;0.000000;,
  0.000000;0.000000;0.000000;,
  0.000000;0.000000;0.000000;,
  0.000000;0.000000;0.000000;,
  0.000000;0.000000;0.000000;;
  6;
  3;0,2,3;,
  3;3,1,0;,
  3;4,5,6;,
  3;6,7,4;,
  3;4,8,9;,
  3;9,10,4;;
 }

 MeshTextureCoords {
  11;
  0.003167;0.994279;,
  0.997611;0.994279;,
  0.003167;0.003144;,
  0.997611;0.003144;,
  0.003167;0.994279;,
  0.003167;0.003144;,
  0.997611;0.003144;,
  0.997611;0.994279;,
  0.003167;0.003144;,
  0.997611;0.003144;,
  0.997611;0.994279;;
 }

 MeshMaterialList {
  1;
  6;
  0,
  0,
  0,
  0,
  0,
  0;

  Material {
   1.000000;1.000000;1.000000;0.700000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.250000;0.250000;0.250000;;

   TextureFilename {
    "tex\\\\screen.bmp";
   }
  }
 }
}