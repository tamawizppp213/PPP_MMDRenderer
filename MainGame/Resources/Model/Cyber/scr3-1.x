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
 -4.864326;-2.737917;-0.002380;,
 4.864344;-2.737917;-0.002380;,
 -4.864326;2.737972;-0.002380;,
 4.864344;2.737972;-0.002380;,
 -0.503188;0.565006;0.002541;,
 -0.503188;0.565006;0.002541;,
 -0.503188;0.565006;0.002541;,
 -0.503188;0.565006;0.002541;,
 -0.503188;0.565006;0.002541;,
 -0.503188;0.565006;0.002541;,
 -0.503188;0.565006;0.002541;;
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
  0.499810;0.994430;,
  0.741856;0.994430;,
  0.499810;0.757121;,
  0.741856;0.757121;,
  0.499810;0.994430;,
  0.499810;0.757121;,
  0.741856;0.757121;,
  0.741856;0.994430;,
  0.499810;0.757121;,
  0.741856;0.757121;,
  0.741856;0.994430;;
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
   1.000000;1.000000;1.000000;0.900000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   1.000000;1.000000;1.000000;;

   TextureFilename {
    "tex\\\\screen.bmp";
   }
  }
 }
}