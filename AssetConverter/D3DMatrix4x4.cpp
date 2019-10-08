#include "D3DMatrix4x4.h"
#include "D3DVector3.h"
#include "D3DVector4.h"

using namespace DirectX;

namespace Math
{
	const D3DMatrix4x4 D3DMatrix4x4::Scale(float x, float y, float z)
	{
		return XMMatrixScaling(x, y, z);
	}

	const D3DMatrix4x4 D3DMatrix4x4::RotateX(float angle)
	{
		return XMMatrixRotationX(angle);
	}

	const D3DMatrix4x4 D3DMatrix4x4::RotateY(float angle)
	{
		return XMMatrixRotationY(angle);
	}

	const D3DMatrix4x4 D3DMatrix4x4::RotateZ(float angle)
	{
		return XMMatrixRotationZ(angle);
	}

	const D3DMatrix4x4 D3DMatrix4x4::RotateAxis(const D3DVector4& axis, float angle)
	{
		XMVECTOR v = XMVectorSet(axis.x, axis.y, axis.z, axis.w);
		return XMMatrixRotationAxis(v, angle);
	}

	const D3DMatrix4x4 D3DMatrix4x4::Rotate(float x, float y, float z)
	{
		return XMMatrixRotationRollPitchYaw(x, y, z);
	}

	const D3DMatrix4x4 D3DMatrix4x4::Translate(float x, float y, float z)
	{
		return XMMatrixTranslation(x, y, z);
	}

	//
	// Public Member Functions
	//
	D3DMatrix4x4::D3DMatrix4x4()
	{
		XMMATRIX identity = XMMatrixIdentity();
		XMStoreFloat4x4(this, identity);
	}

	D3DMatrix4x4::D3DMatrix4x4(float x1, float x2, float x3, float x4,
						float y1, float y2, float y3, float y4,
						float z1, float z2, float z3, float z4,
						float w1, float w2, float w3, float w4)
		: XMFLOAT4X4(x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4, w1, w2, w3, w4)
	{
		
	}

	float D3DMatrix4x4::Determinant() const
	{
		XMVECTOR determinant = XMMatrixDeterminant(*this);
		return XMVectorGetX(determinant);
	}

	const D3DMatrix4x4 D3DMatrix4x4::Inverse() const
	{
		return XMMatrixInverse(nullptr, *this);
	}

	const D3DMatrix4x4 D3DMatrix4x4::Transpose() const
	{
		return XMMatrixTranspose(*this);
	}

	const D3DMatrix4x4 D3DMatrix4x4::operator+(const D3DMatrix4x4& rhs) const
	{
		XMMATRIX m1 = *this;
		XMMATRIX m2 = rhs;
		return m1 + m2;
	}

	const D3DMatrix4x4 D3DMatrix4x4::operator-(const D3DMatrix4x4& rhs) const
	{
		XMMATRIX m1 = *this;
		XMMATRIX m2 = rhs;
		return m1 - m2;
	}

	const D3DMatrix4x4 D3DMatrix4x4::operator*(const D3DMatrix4x4& rhs) const
	{
		XMMATRIX m1 = *this;
		XMMATRIX m2 = rhs;
		return XMMatrixMultiply(m1, m2);
	}

	const D3DMatrix4x4 D3DMatrix4x4::operator*(float rhs) const
	{
		XMMATRIX m = *this;
		return m * rhs;
	}

	const D3DMatrix4x4 D3DMatrix4x4::operator/(float rhs) const
	{
		XMMATRIX m = *this;
		return m / rhs;
	}

	D3DMatrix4x4& D3DMatrix4x4::operator+=(const D3DMatrix4x4& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	D3DMatrix4x4& D3DMatrix4x4::operator-=(const D3DMatrix4x4& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	D3DMatrix4x4& D3DMatrix4x4::operator*=(const D3DMatrix4x4& rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	D3DMatrix4x4& D3DMatrix4x4::operator*=(float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	D3DMatrix4x4& D3DMatrix4x4::operator/=(float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	float* D3DMatrix4x4::operator[](size_t index)
	{
		return m[index];
	}

	const float* D3DMatrix4x4::operator[](size_t index) const
	{
		return m[index];
	}

	D3DMatrix4x4::operator DirectX::XMMATRIX()
	{
		return XMLoadFloat4x4(this);
	}

	D3DMatrix4x4::operator const DirectX::XMMATRIX() const
	{
		return XMLoadFloat4x4(this);
	}

	//
	// Private Member Functions
	//
	D3DMatrix4x4::D3DMatrix4x4(DirectX::FXMMATRIX matrix)
	{
		XMStoreFloat4x4(this, matrix);
	}

	//
	// Global Functions
	//
	const D3DMatrix4x4 operator*(float lhs, const D3DMatrix4x4& rhs)
	{
		return rhs.operator*(lhs);
	}

	const D3DMatrix4x4 operator/(float lhs, const D3DMatrix4x4& rhs)
	{
		return rhs.operator/(lhs);
	}
}