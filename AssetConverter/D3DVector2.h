#pragma once

#include <DirectXMath.h>

namespace Math
{
	struct D3DVector3;
	struct D3DVector4;

	struct D3DVector2 : private DirectX::XMFLOAT2
	{
		using DirectX::XMFLOAT2::x;
		using DirectX::XMFLOAT2::y;

		D3DVector2();
		D3DVector2(float x, float y);
		D3DVector2(const D3DVector3& v);
		D3DVector2(const D3DVector4& v);

		float Dot(const D3DVector2& rhs) const;
		float Length() const;
		float LengthSquare() const;

		const D3DVector2 operator+(const D3DVector2& rhs) const;
		const D3DVector2 operator-(const D3DVector2& rhs) const;
		const D3DVector2 operator*(float rhs) const;
		const D3DVector2 operator/(float rhs) const;
		D3DVector2& operator+=(const D3DVector2& rhs);
		D3DVector2& operator-=(const D3DVector2& rhs);
		D3DVector2& operator*=(float rhs);
		D3DVector2& operator/=(float rhs);

		operator DirectX::XMVECTOR();
		operator const DirectX::XMVECTOR() const;

	private:
		D3DVector2(DirectX::FXMVECTOR v);
	};

	const D3DVector2 operator*(float lhs, const D3DVector2& rhs);
	const D3DVector2 operator/(float lhs, const D3DVector2& rhs);
}