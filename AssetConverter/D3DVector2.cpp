#include "D3DVector2.h"
#include "D3DVector3.h"
#include "D3DVector4.h"

using namespace DirectX;

namespace Math
{
	D3DVector2::D3DVector2()
		: XMFLOAT2(0.0f, 0.0f)
	{
	}

	D3DVector2::D3DVector2(float x, float y)
		: XMFLOAT2(x, y)
	{
	}

	D3DVector2::D3DVector2(const D3DVector3& v)
		: XMFLOAT2(v.x, v.y)
	{
	}

	D3DVector2::D3DVector2(const D3DVector4& v)
		: XMFLOAT2(v.x, v.y)
	{
	}

	float D3DVector2::Dot(const D3DVector2& rhs) const
	{
		XMVECTOR result = XMVector2Dot(*this, rhs);
		return XMVectorGetX(result);
	}

	float D3DVector2::Length() const
	{
		XMVECTOR result = XMVector2Length(*this);
		return XMVectorGetX(result);
	}

	float D3DVector2::LengthSquare() const
	{
		XMVECTOR result = XMVector2LengthSq(*this);
		return XMVectorGetX(result);
	}

	const D3DVector2 D3DVector2::operator+(const D3DVector2& rhs) const
	{
		return XMVectorAdd(*this, rhs);
	}

	const D3DVector2 D3DVector2::operator-(const D3DVector2& rhs) const
	{
		return XMVectorSubtract(*this, rhs);
	}

	const D3DVector2 D3DVector2::operator*(float rhs) const
	{
		return XMVectorScale(*this, rhs);
	}

	const D3DVector2 D3DVector2::operator/(float rhs) const
	{
		return XMVectorScale(*this, 1.0f / rhs);
	}

	D3DVector2& D3DVector2::operator+=(const D3DVector2& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	D3DVector2& D3DVector2::operator-=(const D3DVector2& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	D3DVector2& D3DVector2::operator*=(float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	D3DVector2& D3DVector2::operator/=(float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	D3DVector2::operator XMVECTOR()
	{
		return XMLoadFloat2(this);
	}

	D3DVector2::operator const XMVECTOR() const
	{
		return XMLoadFloat2(this);
	}

	//
	// Private Member Functions
	//
	D3DVector2::D3DVector2(FXMVECTOR v)
	{
		XMStoreFloat2(this, v);
	}

	//
	// Global Functions
	//
	const D3DVector2 operator*(float lhs, const D3DVector2& rhs)
	{
		return rhs.operator*(lhs);
	}

	const D3DVector2 operator/(float lhs, const D3DVector2& rhs)
	{
		return rhs.operator/(lhs);
	}

}