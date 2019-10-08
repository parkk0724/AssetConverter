#include "Math.h"

namespace Math
{
#if defined(Direct3D9)
	const float PI = DirectX::XM_PI;

	constexpr float ConvertToRadians(float degree)
	{
		return DirectX::XMConvertToRadians(degree);
	}

	constexpr float ConvertToDegrees(float radian)
	{
		return DirectX::XMConvertToDegrees(radian);
	}
#endif
}