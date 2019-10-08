#include "D3DQuaternion.h"

using namespace DirectX;

namespace Math
{
	D3DQuaternion::D3DQuaternion()
		: XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	{
	}

	D3DQuaternion::D3DQuaternion(float x, float y, float z, float w)
		: XMFLOAT4(x, y, z, w)
	{
	}

	D3DVector3 D3DQuaternion::ToEulerAngles() const
	{
		D3DVector3 angles;

		double a = 2.0 * (w * x + y * z);
		double b = 1.0 - 2.0 * (x * x + y * y);
		angles.x = atan2(a, b);

		double c = 2.0 * (w * y - z * x);
		angles.y = fabs(c) >= 1 ? copysign(XM_PI / 2, c) : asin(c);

		double d = 2.0 * (w * z + x * y);
		double e = 1.0 - 2.0 * (y * y + z * z);
		angles.z = atan2(d, e);

		return angles;
	}
	
	D3DQuaternion::operator DirectX::XMVECTOR()
	{
		return XMLoadFloat4(this);
	}

	D3DQuaternion::operator const DirectX::XMVECTOR() const
	{
		return XMLoadFloat4(this);
	}
}