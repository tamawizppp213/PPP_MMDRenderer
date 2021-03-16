//////////////////////////////////////////////////////////////////////////////////
//              Title:  FrameResources.hpp
//            Content:  Per frame resource manager
//             Author:  Toide Yutaro (Reference: 3DGame Programming with DirectX12)
//             Create:  2021_03_12
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "GameCore/Include/FrameResources.hpp"
#include "GameCore/Include/Camera.hpp"
#include "GameCore/Include/Screen.hpp"
#include "GameCore/Include/GameTimer.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
FrameResource::FrameResource(const FrameResourceCounter& frameResourceCounter, const GameTimer& gameTimer)
{
	DirectX12& directX12 = DirectX12::Instance();
	Device*    device    = directX12.GetDevice();

	this->ObjectConstantsBuffer     = std::make_unique<UploadBuffer<ObjectConstants>>    (device, frameResourceCounter.MaxObjectCount    , true);
	this->SceneConstantsBuffer      = std::make_unique<UploadBuffer<SceneConstants>>     (device, frameResourceCounter.MaxSceneCount     , true);
	this->MaterialConstantsBuffer   = std::make_unique<UploadBuffer<MaterialConstants>>  (device, frameResourceCounter.MaxMaterialCount  , true);
	this->SceneLightConstantsBuffer = std::make_unique<UploadBuffer<SceneLightConstants>>(device, frameResourceCounter.MaxSceneLightCount, true);
	this->_gameTimer                = &gameTimer;
	
}

void FrameResource::UpdateSceneConstants(SceneConstants* scene, const Camera* camera)
{
	using namespace DirectX;
	if (camera == nullptr || scene == nullptr) { return; }

	XMMATRIX view                      = camera->GetViewMatrix();
	XMMATRIX projection                = camera->GetProjectionMatrix();
	XMMATRIX viewProjection            = XMMatrixMultiply(view, projection);

	XMVECTOR viewDeterminant           = XMMatrixDeterminant(view);
	XMVECTOR projectionDeterminant     = XMMatrixDeterminant(projection);
	XMVECTOR viewProjectionDeterminant = XMMatrixDeterminant(viewProjection);

	XMMATRIX inverseView               = XMMatrixInverse(&viewDeterminant          , view);
	XMMATRIX inverseProjection         = XMMatrixInverse(&projectionDeterminant    , projection);
	XMMATRIX inverseViewProjection     = XMMatrixInverse(&viewProjectionDeterminant, viewProjection);
	// note: Texture and shadow related features will be added later.

	XMStoreFloat4x4(&scene->View                 , XMMatrixTranspose(view));
	XMStoreFloat4x4(&scene->InverseView          , XMMatrixTranspose(inverseView));
	XMStoreFloat4x4(&scene->Projection           , XMMatrixTranspose(projection));
	XMStoreFloat4x4(&scene->InverseProjection    , XMMatrixTranspose(inverseProjection));
	XMStoreFloat4x4(&scene->ViewProjection       , XMMatrixTranspose(viewProjection));
	XMStoreFloat4x4(&scene->InverseViewProjection, XMMatrixTranspose(inverseViewProjection));

	Screen screen;
	scene->EyePosition             = camera->GetPosition3f();
	scene->RenderTargetSize        = XMFLOAT2(screen.GetScreenWidth(), screen.GetScreenHeight());
	scene->InverseRenderTargetSize = XMFLOAT2(1.0f / screen.GetScreenWidth(), 1.0f / screen.GetScreenHeight());
	scene->NearZ                   = camera->GetNearZ();
	scene->FarZ                    = camera->GetFarZ();
	scene->TotalTime               = _gameTimer->TotalTime();
	scene->DeltaTime               = _gameTimer->DeltaTime();

	auto currentSceneConstantBuffer = SceneConstantsBuffer.get();
	currentSceneConstantBuffer->CopyStart();
	currentSceneConstantBuffer->CopyData(0, *scene);
	currentSceneConstantBuffer->CopyEnd();

}