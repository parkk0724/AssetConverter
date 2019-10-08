#include "D3DVector2.h"
#include "D3DVector3.h"
#include "D3DVector4.h"
#include "D3DMatrix4x4.h"

using namespace DirectX;

namespace Math
{
	//
	// Public Member Functions
	//
	D3DVector4::D3DVector4()
		: XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	D3DVector4::D3DVector4(float x, float y, float z, float w)
		: XMFLOAT4(x, y, z, w)
	{
	}

	D3DVector4::D3DVector4(const D3DVector2& v, float z, float w)
		: XMFLOAT4(v.x, v.y, z, w)
	{
	}

	D3DVector4::D3DVector4(const D3DVector3& v, float w)
		: XMFLOAT4(v.x, v.y, v.z, w)
	{
	}

	float D3DVector4::Dot(const D3DVector4& rhs) const
	{
		XMVECTOR result = XMVector4Dot(*this, rhs);
		return XMVectorGetX(result);
	}

	const D3DVector4 D3DVector4::Cross(const D3DVector4& v2, const D3DVector4& v3) const
	{
		return XMVector4Cross(*this, v2, v3);
	}

	float D3DVector4::Length() const
	{
		XMVECTOR result = XMVector4Length(*this);
		return XMVectorGetX(result);
	}

	float D3DVector4::LengthSquare() const
	{
		XMVECTOR result = XMVector4LengthSq(*this);
		return XMVectorGetX(result);
	}

	const D3DVector4 D3DVector4::operator+(const D3DVector4& rhs) const
	{
		return XMVectorAdd(*this, rhs);
	}

	const D3DVector4 D3DVector4::operator-(const D3DVector4& rhs) const
	{
		return XMVectorSubtract(*this, rhs);
	}

	const D3DVector4 D3DVector4::operator*(float rhs) const
	{
		return XMVectorScale(*this, rhs);
	}

	const D3DVector4 D3DVector4::operator*(const D3DMatrix4x4& rhs) const
	{
		return XMVector4Transform(*this, rhs);
	}

	const D3DVector4 D3DVector4::operator/(float rhs) const
	{
		return XMVectorScale(*this, 1.0f / rhs);
	}

	D3DVector4& D3DVector4::operator+=(const D3DVector4& rhs)
	{
		XMVECTOR result = XMVectorAdd(*this, rhs);
		XMStoreFloat4(this, result);
		return *this;
	}

	D3DVector4& D3DVector4::operator-=(const D3DVector4& rhs)
	{
		XMVECTOR result = XMVectorSubtract(*this, rhs);
		XMStoreFloat4(this, result);
		return *this;
	}

	D3DVector4& D3DVector4::operator*=(float rhs)
	{
		XMVECTOR result = XMVectorScale(*this, rhs);
		XMStoreFloat4(this, result);
		return *this;
	}

	D3DVector4& D3DVector4::operator/=(float rhs)
	{
		XMVECTOR result = XMVectorScale(*this, 1.0f / rhs);
		XMStoreFloat4(this, result);
		return *this;
	}

	D3DVector4::operator XMVECTOR()
	{
		return XMLoadFloat4(this);
	}

	D3DVector4::operator const XMVECTOR() const
	{
		return XMLoadFloat4(this);
	}

	//
	// Private Member Functions
	//
	D3DVector4::D3DVector4(FXMVECTOR v)
	{
		XMStoreFloat4(this, v);
	}

	//
	// Global Functions
	//
	const D3DVector4 operator*(float lhs, const D3DVector4& rhs)
	{
		return rhs.operator*(lhs);
	}

	const D3DVector4 operator/(float lhs, const D3DVector4& rhs)
	{
		return rhs.operator/(lhs);
	}
}