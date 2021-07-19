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

void FrameResource::UpdateObjectConstants()
{

}

void FrameResource::UpdateSceneConstants(SceneConstants* scene, const Camera* camera)
{
	using namespace gm;
	if (camera == nullptr || scene == nullptr) { return; }

	Matrix4 view                      = camera->GetViewMatrix();
	Matrix4 projection                = camera->GetProjectionMatrix();
	Matrix4 viewProjection            = view * projection; 

	Vector4 viewDeterminant           = Determinant(view);
	Vector4 projectionDeterminant     = Determinant(projection);
	Vector4 viewProjectionDeterminant = Determinant(viewProjection);

	Matrix4 inverseView               = Inverse(viewDeterminant          , view);
	Matrix4 inverseProjection         = Inverse(projectionDeterminant    , projection);
	Matrix4 inverseViewProjection     = Inverse(viewProjectionDeterminant, viewProjection);
	// note: Texture and shadow related features will be added later.

	scene->View                  = view.ToFloat4x4();
	scene->InverseView           = inverseView.ToFloat4x4();
	scene->Projection            = projection .ToFloat4x4();
	scene->InverseProjection     = inverseProjection.ToFloat4x4();
	scene->ViewProjection        = viewProjection.ToFloat4x4();
	scene->InverseViewProjection = inverseViewProjection.ToFloat4x4();

	scene->EyePosition             = camera->GetPosition3f();
	scene->RenderTargetSize        = Float2((float)_screen.GetScreenWidth(), (float)_screen.GetScreenHeight());
	scene->InverseRenderTargetSize = Float2(1.0f / _screen.GetScreenWidth(), 1.0f / _screen.GetScreenHeight());
	scene->NearZ                   = camera->GetNearZ();
	scene->FarZ                    = camera->GetFarZ();
	scene->TotalTime               = _gameTimer->TotalTime();
	scene->DeltaTime               = _gameTimer->DeltaTime();

	auto currentSceneConstantBuffer = SceneConstantsBuffer.get();
	currentSceneConstantBuffer->CopyStart();
	currentSceneConstantBuffer->CopyData(0, *scene);
	currentSceneConstantBuffer->CopyEnd();
}

void FrameResource::UpdateMaterialConstants()
{

}