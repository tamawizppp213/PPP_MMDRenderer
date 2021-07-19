//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12BlendState.cpp
///             @brief  blend state
///             @author Toide Yutaro
///             @date   2022_07_08
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12BlendState.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct ShaderEntryPoint
{
	BlendStateType Type;
	std::string    VSName;
	std::string    PSName;
};

struct FastShaderEntryPoint
{
	FastBlendStateType Type;
	std::string VSName;
	std::string PSName;
};

struct PostEffectEntryPoint
{
	PostEffectBlendStateType Type;
	std::string VSName;
	std::string PSName;
};
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
static const std::wstring g_ShaderPath     = L"Shader\\ShaderDefault2D.hlsl";
static const std::wstring g_TestVertexPath = L"Shader\\ShaderTestVertex.hlsl";
static const std::wstring g_TestPixelPath  = L"Shader\\ShaderTestPixel.hlsl";
static const std::wstring g_BlurPixelPath  = L"Shader\\Effect\\ShaderBlur.hlsl";
static const ShaderEntryPoint g_EntryPointList[(int)BlendStateType::CountOfBlendStateType] =
{
	{BlendStateType::None             , "VSMain" , "PSMain"},
	{BlendStateType::Normal           , "VSMain" , "PSNormal"},
	{BlendStateType::Addition         , "VSBlend", "PSAddition"},
	{BlendStateType::Subtraction      , "VSBlend", "PSSubtraction"},
	{BlendStateType::Multiplication   , "VSBlend", "PSMultiplication"},
	{BlendStateType::Division         , "VSBlend", "PSDivision"},
	{BlendStateType::Screen           , "VSBlend", "PSScreen"},
	{BlendStateType::Overlay          , "VSBlend", "PSOverlay"},
	{BlendStateType::Dark             , "VSBlend", "PSDark"},
	{BlendStateType::Light            , "VSBlend", "PSLight"},
	{BlendStateType::ColorDark        , "VSBlend", "PSColorDark"},
	{BlendStateType::ColorLight       , "VSBlend", "PSColorLight"},
	{BlendStateType::BurnColor        , "VSBlend", "PSBurnColor"},
	{BlendStateType::BurnLinear       , "VSBlend", "PSBurnLinear"},
	{BlendStateType::DodgeColor       , "VSBlend", "PSDodgeColor"},
	{BlendStateType::DodgeLinear      , "VSBlend", "PSDodgeLinear"},
	{BlendStateType::SoftLight        , "VSBlend", "PSSoftLight"},
	{BlendStateType::HardLight        , "VSBlend", "PSHardLight"},
	{BlendStateType::VividLight       , "VSBlend", "PSVividLight"},
	{BlendStateType::LinearLight      , "VSBlend", "PSLinearLight"},
	{BlendStateType::PinLight         , "VSBlend", "PSPinLight"},
	{BlendStateType::HardMix          , "VSBlend", "PSHardMix"},
	{BlendStateType::AbsoluteSubstract, "VSBlend", "PSAbsoluteSubtract"},
	{BlendStateType::Exclusion        , "VSBlend", "PSExclusion"},
	{BlendStateType::BrightExtract    , "VSBlend", "PSBrightExtract"},
	{BlendStateType::ColorKeyAlpha    , "VSBlend", "PSColorKeyAlpha"},
	{BlendStateType::ColorTone        , "VSBlend", "PSColorTone"},
};

static ShaderBlendData g_ShaderData[(int)BlendStateType::CountOfBlendStateType] =
{
	{BlendStateType::None             , nullptr, nullptr},
	{BlendStateType::Normal           , nullptr, nullptr},
	{BlendStateType::Addition         , nullptr, nullptr},
	{BlendStateType::Subtraction      , nullptr, nullptr},
	{BlendStateType::Multiplication   , nullptr, nullptr},
	{BlendStateType::Division         , nullptr, nullptr},
	{BlendStateType::Screen           , nullptr, nullptr},
	{BlendStateType::Overlay          , nullptr, nullptr},
	{BlendStateType::Dark             , nullptr, nullptr},
	{BlendStateType::Light            , nullptr, nullptr},
	{BlendStateType::ColorDark        , nullptr, nullptr},
	{BlendStateType::ColorLight       , nullptr, nullptr},
	{BlendStateType::BurnColor        , nullptr, nullptr},
	{BlendStateType::BurnLinear       , nullptr, nullptr},
	{BlendStateType::DodgeColor       , nullptr, nullptr},
	{BlendStateType::DodgeLinear      , nullptr, nullptr},
	{BlendStateType::SoftLight        , nullptr, nullptr},
	{BlendStateType::HardLight        , nullptr, nullptr},
	{BlendStateType::VividLight       , nullptr, nullptr},
	{BlendStateType::LinearLight      , nullptr, nullptr},
	{BlendStateType::PinLight         , nullptr, nullptr},
	{BlendStateType::HardMix          , nullptr, nullptr},
	{BlendStateType::AbsoluteSubstract, nullptr, nullptr},
	{BlendStateType::Exclusion        , nullptr, nullptr},
	{BlendStateType::BrightExtract    , nullptr, nullptr},
	{BlendStateType::ColorKeyAlpha    , nullptr, nullptr},
	{BlendStateType::ColorTone        , nullptr, nullptr},

};

static FastShaderEntryPoint g_FastEntryPointList[(int)FastBlendStateType::CountOfFastBlendStateType] =
{
	{FastBlendStateType::None       , "VSMain", "PSMain"},
	{FastBlendStateType::Normal     , "VSMain", "PSMain"},
	{FastBlendStateType::Addition   , "VSMain", "PSMain"},
	{FastBlendStateType::Subtraction, "VSMain", "PSMain"},
	{FastBlendStateType::Screen     , "VSMain", "PSMain"}
};

static FastShaderBlendData g_FastShaderData[(int)FastBlendStateType::CountOfFastBlendStateType] =
{
	{FastBlendStateType::None       , nullptr, nullptr},
	{FastBlendStateType::Normal     , nullptr, nullptr},
	{FastBlendStateType::Addition   , nullptr, nullptr},
	{FastBlendStateType::Subtraction, nullptr, nullptr},
	{FastBlendStateType::Screen     , nullptr, nullptr}
};

static PostEffectEntryPoint g_EffectEntryPointList[(int)PostEffectBlendStateType::CountOfBlendStateType] =
{
	{PostEffectBlendStateType::None      , "VSMain" , "PSMain"},
	{PostEffectBlendStateType::Monochrome, "VSBlend", "PSMonochrome"},
	{PostEffectBlendStateType::Invert    , "VSBlend", "PSInvert"},
	{PostEffectBlendStateType::Sepia     , "VSBlend", "PSSepia"},
	{PostEffectBlendStateType::GrayScale , "VSBlend", "PSGrayScale"},
	{PostEffectBlendStateType::Binary    , "VSBlend", "PSBinary"},
	{PostEffectBlendStateType::Blur      , "VSBlend", "PSMain"}
};

static PostEffectShaderBlendData g_EffectShaderData[(int)PostEffectBlendStateType::CountOfBlendStateType] =
{
	{PostEffectBlendStateType::None      , nullptr, nullptr},
	{PostEffectBlendStateType::Monochrome, nullptr, nullptr},
	{PostEffectBlendStateType::Invert    , nullptr, nullptr},
	{PostEffectBlendStateType::Sepia     , nullptr, nullptr},
	{PostEffectBlendStateType::GrayScale , nullptr, nullptr},
	{PostEffectBlendStateType::Binary    , nullptr, nullptr},
	{PostEffectBlendStateType::Blur      , nullptr, nullptr}
};

void InitializeShaderBlendData()
{
	for (int i = 0; i < (int)BlendStateType::CountOfBlendStateType; ++i)
	{
		g_ShaderData[i].VertexShader = CompileShader(g_ShaderPath, nullptr, g_EntryPointList[i].VSName, "vs_5_1");
		g_ShaderData[i].PixelShader  = CompileShader(g_ShaderPath, nullptr, g_EntryPointList[i].PSName, "ps_5_1");
	}

	for (int i = 0; i < (int)FastBlendStateType::CountOfFastBlendStateType; ++i)
	{
		g_FastShaderData[i].VertexShader = CompileShader(g_TestVertexPath, nullptr, g_FastEntryPointList[i].VSName, "vs_5_1");
		g_FastShaderData[i].PixelShader  = CompileShader(g_TestPixelPath, nullptr, g_FastEntryPointList[i].PSName, "ps_5_1");
	}

	for (int i = 0; i <= (int)PostEffectBlendStateType::Binary; ++i)
	{
		g_EffectShaderData[i].VertexShader = CompileShader(g_ShaderPath, nullptr, g_EffectEntryPointList[i].VSName, "vs_5_1");
		g_EffectShaderData[i].PixelShader  = CompileShader(g_ShaderPath, nullptr, g_EffectEntryPointList[i].PSName, "ps_5_1");
	}
	

}

ShaderBlendData GetShaderBlendData(BlendStateType type)
{
	return g_ShaderData[(int)type];
}

FastShaderBlendData GetShaderBlendData(FastBlendStateType type)
{
	return g_FastShaderData[(int)type];
}

PostEffectShaderBlendData GetShaderBlendData(PostEffectBlendStateType type)
{
	return g_EffectShaderData[(int)type];
}

BLEND_DESC GetBlendDesc(BlendStateType type)
{
	BLEND_DESC blendDesc = BLEND_DESC(D3D12_DEFAULT);
	blendDesc.AlphaToCoverageEnable         = true;
	blendDesc.IndependentBlendEnable        = false;
	blendDesc.RenderTarget[0].BlendEnable   = true;
	blendDesc.RenderTarget[0].LogicOpEnable = false;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	switch (type)
	{
		case BlendStateType::None:
		{
			// あとで半透明からOverWrite用に変更する.
			blendDesc.RenderTarget[0].BlendOp   = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend  = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		}
		default:
		{
			blendDesc.RenderTarget[0].BlendOp   = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend  = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		}
	}
	
	return blendDesc;
}

BLEND_DESC GetBlendDesc(PostEffectBlendStateType type)
{
	BLEND_DESC blendDesc = BLEND_DESC(D3D12_DEFAULT);
	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].LogicOpEnable = false;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	switch (type)
	{
		case PostEffectBlendStateType::None:
		{
			// あとで半透明からOverWrite用に変更する.
			blendDesc.RenderTarget[0].BlendOp   = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend  = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		}
		default:
		{
			blendDesc.RenderTarget[0].BlendOp   = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend  = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		}
	}

	return blendDesc;
}


BLEND_DESC GetBlendDesc(FastBlendStateType type)
{
	BLEND_DESC blendDesc = BLEND_DESC(D3D12_DEFAULT);
	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].LogicOpEnable = false;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	switch (type)
	{
		case FastBlendStateType::None:
		{
			blendDesc.RenderTarget[0].BlendOp   = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend  = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		}
		case FastBlendStateType::Normal:
		{
			blendDesc.RenderTarget[0].BlendOp   = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend  = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		}
		case FastBlendStateType::Addition:
		{
			blendDesc.RenderTarget[0].BlendOp   = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend  = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		}
		case FastBlendStateType::Subtraction:
		{
			blendDesc.RenderTarget[0].BlendOp   = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend  = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		}
		case FastBlendStateType::Screen:
		{
			blendDesc.RenderTarget[0].BlendOp   = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend  = D3D12_BLEND_INV_DEST_COLOR;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		}
	
	}

	blendDesc.RenderTarget[0].SrcBlendAlpha  = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha   = D3D12_BLEND_OP_ADD;
	return blendDesc;
}