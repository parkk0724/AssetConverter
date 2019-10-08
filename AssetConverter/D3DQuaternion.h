#pragma once

#include "D3DVector3.h"

namespace Math
{
	struct D3DQuaternion : private DirectX::XMFLOAT4
	{
		using DirectX::XMFLOAT4::x;
		using DirectX::XMFLOAT4::y;
		using DirectX::XMFLOAT4::z;
		using DirectX::XMFLOAT4::w;

		D3DQuaternion();
		D3DQuaternion(float x, float y, float z, float w);

		D3DVector3 ToEulerAngles() const;

		operator DirectX::XMVECTOR();
		operator const DirectX::XMVECTOR() const;
	};
}