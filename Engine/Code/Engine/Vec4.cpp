#include "Engine/Math/Vec4.hpp"

const Vec4 Vec4::operator + ( const Vec4& vecToAdd ) const
{
	Vec4 r = Vec4();
	r.x = this->x + vecToAdd.x;
	r.y = this->y + vecToAdd.y;
	r.z = this->z + vecToAdd.z;
	r.w = this->w + vecToAdd.w;
	return Vec4( r.x , r.y , r.z, r.w );
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-( const Vec4& vecToSubtract ) const
{
	Vec4 r = Vec4();
	r.x = this->x - vecToSubtract.x;
	r.y = this->y - vecToSubtract.y;
	r.z = this->z - vecToSubtract.z;
	r.w = this->w - vecToSubtract.w;
	return Vec4( r.x , r.y , r.z, r.w );
}


//------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-() const
{
	return Vec4( -this->x , -this->y , -this->z, -this->w );
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*( float uniformScale ) const
{
	Vec4 r = Vec4();
	r.x = this->x * uniformScale;
	r.y = this->y * uniformScale;
	r.z = this->z * uniformScale;
	r.w = this->w * uniformScale;
	return Vec4( r.x , r.y , r.z, r.w );
}


//------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*( const Vec4& vecToMultiply ) const
{
	Vec4 r = Vec4();
	r.x = this->x * vecToMultiply.x;
	r.y = this->y * vecToMultiply.y;
	r.z = this->z * vecToMultiply.z;
	return Vec4( r.x , r.y , r.z, r.w );
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator/( float inverseScale ) const
{
	Vec4 r = Vec4();
	r.x = this->x / inverseScale;
	r.y = this->y / inverseScale;
	r.z = this->z / inverseScale;
	r.w = this->z / inverseScale;
	return Vec4( r.x , r.y , r.z, r.w );
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator+=( const Vec4& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator-=( const Vec4& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=( const Vec4& copyFrom )
{
	this->x = copyFrom.x;
	this->y = copyFrom.y;
	this->z = copyFrom.z;
	this->w = copyFrom.w;
}


//-----------------------------------------------------------------------------------------------
const Vec4 operator*( float uniformScale , const Vec4& vecToScale )
{
	Vec4 r = Vec4();
	r.x = uniformScale * vecToScale.x;
	r.y = uniformScale * vecToScale.y;
	r.z = uniformScale * vecToScale.z;
	r.w = uniformScale * vecToScale.w;
	return Vec4( r.x , r.y , r.z, r.w );
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==( const Vec4& compare ) const
{
	if ( this->x == compare.x && this->y == compare.y && this->z == compare.z && this->w == compare.w)
	{
		return true;
	}
	else
		return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator!=( const Vec4& compare ) const
{
	if ( this->x != compare.x || this->y != compare.y || this->z != compare.z || this->w!=compare.w)
	{
		return true;
	}
	else
		return false;
}