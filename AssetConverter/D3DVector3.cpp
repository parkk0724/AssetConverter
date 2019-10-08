#include "D3DVector2.h"
#include "D3DVector3.h"
#include "D3DVector4.h"

using namespace DirectX;

namespace Math
{
	//
	// Public Functions
	//
	D3DVector3::D3DVector3()
		: XMFLOAT3(0.0f, 0.0f, 0.0f)
	{
	}

	D3DVector3::D3DVector3(float x, float y, float z)
		: XMFLOAT3(x, y, z)
	{
	}

	D3DVector3::D3DVector3(const D3DVector2& v, float z)
		: XMFLOAT3(v.x, v.y, z)
	{
	}

	D3DVector3::D3DVector3(const D3DVector4& v)
		: XMFLOAT3(v.x, v.y, v.z)
	{
	}

	float D3DVector3::Dot(const D3DVector3& rhs) const
	{
		XMVECTOR result = XMVector3Dot(*this, rhs);
		return XMVectorGetX(result);
	}

	const D3DVector3 D3DVector3::Cross(const D3DVector3& rhs) const
	{
		return XMVector3Cross(*this, rhs);
	}

	float D3DVector3::Length() const
	{
		XMVECTOR result = XMVector3Length(*this);
		return XMVectorGetX(result);
	}

	float D3DVector3::LengthSquare() const
	{
		XMVECTOR result = XMVector3LengthSq(*this);
		return XMVectorGetX(result);
	}

	const D3DVector3 D3DVector3::operator+(const D3DVector3& rhs) const
	{
		return XMVectorAdd(*this, rhs);
	}

	const D3DVector3 D3DVector3::operator-(const D3DVector3& rhs) const
	{
		return XMVectorSubtract(*this, rhs);
	}

	const D3DVector3 D3DVector3::operator*(float rhs) const
	{
		return XMVectorScale(*this, rhs);
	}

	const D3DVector3 D3DVector3::operator/(float rhs) const
	{
		return XMVectorScale(*this, 1 / rhs);
	}

	D3DVector3& D3DVector3::operator+=(const D3DVector3& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	D3DVector3& D3DVector3::operator-=(const D3DVector3& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	D3DVector3& D3DVector3::operator*=(float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	D3DVector3& D3DVector3::operator/=(float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	D3DVector3::operator XMVECTOR()
	{
		return XMLoadFloat3(this);
	}

	D3DVector3::operator const XMVECTOR() const
	{
		return XMLoadFloat3(this);
	}

	//
	// Private Member Functions
	//
	D3DVector3::D3DVector3(FXMVECTOR v)
	{
		XMStoreFloat3(this, v);
	}

	//
	// Global Functions
	//
	const D3DVector3 operator*(float lhs, const D3DVector3& rhs)
	{
		return rhs.operator*(lhs);
	}

	const D3DVector3 operator/(float lhs, const D3DVector3& rhs)
	{
		return rhs.operator/(lhs);
	}
}