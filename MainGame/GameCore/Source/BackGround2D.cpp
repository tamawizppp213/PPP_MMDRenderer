//////////////////////////////////////////////////////////////////////////////////
//              Title:  BackGround2D.hpp
//            Content:  Back Ground2D
//             Author:  Toide Yutaro
//             Create:  2020_12_24
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/BackGround2D.hpp"
#include "DirectX12/Include/DirectX12Texture.hpp"
#include "DirectX12/Include/DirectX12Shader.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                            Implement
//////////////////////////////////////////////////////////////////////////////////
void BackGround2D::Initialize(const std::vector<std::wstring>& imagePath, DirectX12& directX12)
{
	/*-------------------------------------------------------------------
	-               Device and CommandList Initialize
	---------------------------------------------------------------------*/
	_directX12 = directX12;

	/*-------------------------------------------------------------------
	-               Resize Vector
	---------------------------------------------------------------------*/
	ResizeVectors(imagePath);

	/*-------------------------------------------------------------------
	-               LoadTexture
	---------------------------------------------------------------------*/
	LoadTextures(imagePath);

	/*-------------------------------------------------------------------
	-               LoadShader
	---------------------------------------------------------------------*/
	LoadShaders();
}

void BackGround2D::Update()
{

}

void BackGround2D::Draw()
{
	for (int i = 0; i < _textureBuffers.size(); ++i)
	{
		_directX12.GetCommandList()->IASetVertexBuffers(0, 1, &_meshGeometry[i].VertexBufferView());
		_directX12.GetCommandList()->IASetIndexBuffer(&_meshGeometry[i].IndexBufferView());
		_directX12.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_directX12.GetCommandList()->DrawIndexedInstanced(_meshGeometry[i].IndexBufferByteSize / sizeof(UINT16), 1, 0, 0, 0);
	}
}


void BackGround2D::Terminate()
{

}

#pragma region Private Function

void BackGround2D::ResizeVectors(const std::vector<std::wstring>& imagePath)
{
	_textureBuffers.resize(imagePath.size());
	_meshGeometry.resize(imagePath.size());
	_subMeshGeometry.resize(imagePath.size());
}

void BackGround2D::LoadTextures(const std::vector<std::wstring>& imagePath)
{
	Texture texture;
	for (int i = 0; i < imagePath.size(); ++i)
	{
		texture.LoadTexture(_directX12, imagePath[i], _textureBuffers[i]);
	}
}

void BackGround2D::LoadShaders()
{
	_vertexShader = CompileShader(L"Shader\\ShaderTestVertex.hlsl", nullptr, "VSMain", "vs_5_1");
	_pixelShader  = CompileShader(L"Shader\\ShaderTestPixel.hlsl", nullptr, "PSMain", "ps_5_1");
}
#pragma endregion Private Functio