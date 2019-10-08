#pragma once

#include <DirectXMath.h>

namespace Math
{
	struct D3DVector2;
	struct D3DVector3;
	struct D3DMatrix4x4;

	struct D3DVector4 : private DirectX::XMFLOAT4
	{
		using DirectX::XMFLOAT4::x;
		using DirectX::XMFLOAT4::y;
		using DirectX::XMFLOAT4::z;
		using DirectX::XMFLOAT4::w;

		D3DVector4();
		D3DVector4(float x, float y, float z, float w);
		D3DVector4(const D3DVector2& v, float z, float w);
		D3DVector4(const D3DVector3& v, float w);

		float Dot(const D3DVector4& rhs) const;
		const D3DVector4 Cross(const D3DVector4& v2, const D3DVector4& v3) const;
		float Length() const;
		float LengthSquare() const;

		const D3DVector4 operator+(const D3DVector4& rhs) const;
		const D3DVector4 operator-(const D3DVector4& rhs) const;
		const D3DVector4 operator*(float rhs) const;
		const D3DVector4 operator*(const D3DMatrix4x4& rhs) const;
		const D3DVector4 operator/(float rhs) const;
		D3DVector4& operator+=(const D3DVector4& rhs);
		D3DVector4& operator-=(const D3DVector4& rhs);
		D3DVector4& operator*=(float rhs);
		D3DVector4& operator/=(float rhs);

		operator DirectX::XMVECTOR();
		operator const DirectX::XMVECTOR() const;

	private:
		D3DVector4(DirectX::FXMVECTOR v);
	};

	const D3DVector4 operator*(float lhs, const D3DVector4& rhs);
	const D3DVector4 operator/(float lhs, const D3DVector4& rhs);
}