//////////////////////////////////////////////////////////////////////////////////
///             @file   VMDFile.cpp
///             @brief  VMDFile 
///             @author Toide Yutaro
///             @date   2021_02_28
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/MMD/VMDAnimation.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region VMDBezier
/****************************************************************************
*							GetYFromXOnBezier
*************************************************************************//**
*  @fn        float VMDBezier::GetYFromXOnBezier(float x, const gm::Float4& controlPoints, UINT8 loop)
*  @brief     Get Easing y on bezier function
*  @param[in] void
*  @return Å@Å@float y
*****************************************************************************/
float VMDBezier::GetYFromXOnBezier(float x, const gm::Float4& controlPoints, UINT8 loop)
{
	if (controlPoints.x == controlPoints.y && controlPoints.z == controlPoints.w) { return x; }

	float t = x;
	const float k0 = 1 + 3 * controlPoints.x - 3 * controlPoints.z;
	const float k1 = 3 * controlPoints.z - 6 * controlPoints.x;
	const float k2 = 3 * controlPoints.x;

	constexpr float epsilon = 0.0005f;

	for (int i = 0; i < loop; ++i)
	{
		auto ft = k0 * t * t * t + k1 * t * t + k2 * t - x;

		if (ft <= epsilon && ft >= -epsilon)break;

		t -= ft / 2;
	}

	auto r = 1 - t;
	return t * t * t + 3 * t * t * r * controlPoints.w + 3 * t * r * r * controlPoints.y;
}

void VMDBezier::EasingKeyFrame(float time, VMDKeyFrame& keyFrame, float& rotationRatio, gm::Vector3& translationRatio)
{
	translationRatio.SetX(VMDBezier::GetYFromXOnBezier(time, keyFrame.ControlPointForBezier[0].ToFloat4()));
	translationRatio.SetY(VMDBezier::GetYFromXOnBezier(time, keyFrame.ControlPointForBezier[1].ToFloat4()));
	translationRatio.SetZ(VMDBezier::GetYFromXOnBezier(time, keyFrame.ControlPointForBezier[2].ToFloat4()));
	rotationRatio       = VMDBezier::GetYFromXOnBezier(time, keyFrame.ControlPointForBezier[3].ToFloat4());
}
#pragma endregion VMDBezier