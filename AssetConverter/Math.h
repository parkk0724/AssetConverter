#pragma once

#ifdef Direct3D
#include "D3DVector2.h"
#include "D3DVector3.h"
#include "D3DVector4.h"

#include "D3DMatrix4x4.h"

#include "D3DQuaternion.h"
#endif

namespace Math
{
#ifdef Direct3D
	using Vector2 = D3DVector2;
	using Vector3 = D3DVector3;
	using Vector4 = D3DVector4;

	using Matrix4x4 = D3DMatrix4x4;

	using Quaternion = D3DQuaternion;
#endif

	extern const float PI;

	constexpr float ConvertToRadians(float degree);
	constexpr float ConvertToDegrees(float radian);
}