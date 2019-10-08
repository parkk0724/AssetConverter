#pragma once

#include <DirectXMath.h>

namespace Math
{
	struct D3DVector4;

	struct D3DMatrix4x4 : private DirectX::XMFLOAT4X4
	{
		static const D3DMatrix4x4 Scale(float x, float y, float z);
		static const D3DMatrix4x4 RotateX(float angle);
		static const D3DMatrix4x4 RotateY(float angle);
		static const D3DMatrix4x4 RotateZ(float angle);
		static const D3DMatrix4x4 RotateAxis(const D3DVector4& axis, float angle);
		static const D3DMatrix4x4 Rotate(float x, float y, float z);
		static const D3DMatrix4x4 Translate(float x, float y, float z);

		D3DMatrix4x4();
		D3DMatrix4x4(float x1, float x2, float x3, float x4,
				float y1, float y2, float y3, float y4,
				float z1, float z2, float z3, float z4,
				float w1, float w2, float w3, float w4);

		float Determinant() const;
		const D3DMatrix4x4 Inverse() const;
		const D3DMatrix4x4 Transpose() const;

		const D3DMatrix4x4 operator+(const D3DMatrix4x4& rhs) const;
		const D3DMatrix4x4 operator-(const D3DMatrix4x4& rhs) const;
		const D3DMatrix4x4 operator*(const D3DMatrix4x4& rhs) const;
		const D3DMatrix4x4 operator*(float rhs) const;
		const D3DMatrix4x4 operator/(float rhs) const;
		D3DMatrix4x4& operator+=(const D3DMatrix4x4& rhs);
		D3DMatrix4x4& operator-=(const D3DMatrix4x4& rhs);
		D3DMatrix4x4& operator*=(const D3DMatrix4x4& rhs);
		D3DMatrix4x4& operator*=(float rhs);
		D3DMatrix4x4& operator/=(float rhs);

		float* operator[](size_t index);
		const float* operator[](size_t index) const;

		operator DirectX::XMMATRIX();
		operator const DirectX::XMMATRIX() const;

	private:
		D3DMatrix4x4(DirectX::FXMMATRIX matrix);
	};

	const D3DMatrix4x4 operator*(float lhs, const D3DMatrix4x4& rhs);
	const D3DMatrix4x4 operator/(float lhs, const D3DMatrix4x4& rhs);
}