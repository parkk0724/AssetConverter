#pragma once

#include <DirectXMath.h>

namespace Math
{
	struct D3DVector2;
	struct D3DVector4;

	struct D3DVector3 : private DirectX::XMFLOAT3
	{
		using DirectX::XMFLOAT3::x;
		using DirectX::XMFLOAT3::y;
		using DirectX::XMFLOAT3::z;

		D3DVector3();
		D3DVector3(float x, float y, float z);
		D3DVector3(const D3DVector2& v, float z);
		D3DVector3(const D3DVector4& v);
			
		float Dot(const D3DVector3& rhs) const;
		const D3DVector3 Cross(const D3DVector3& rhs) const;
		float Length() const;
		float LengthSquare() const;

		const D3DVector3 operator+(const D3DVector3& rhs) const;
		const D3DVector3 operator-(const D3DVector3& rhs) const;
		const D3DVector3 operator*(float rhs) const;
		const D3DVector3 operator/(float rhs) const;
		D3DVector3& operator+=(const D3DVector3& rhs);
		D3DVector3& operator-=(const D3DVector3& rhs);
		D3DVector3& operator*=(float rhs);
		D3DVector3& operator/=(float rhs);

		operator DirectX::XMVECTOR();
		operator const DirectX::XMVECTOR() const;

	private:
		D3DVector3(DirectX::FXMVECTOR v);
	};

	const D3DVector3 operator*(float lhs, const D3DVector3& rhs);
	const D3DVector3 operator/(float lhs, const D3DVector3& rhs);
}