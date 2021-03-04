#include "Vec3.hpp"
#include <cmath>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

Vec3::Vec3( const Vec3& copy )
{

	this->x=copy.x;
	this->y=copy.y;
	this->z=copy.z;
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
{
	this->x=initialX;
	this->y=initialY;
	this->z=initialZ;
}


Vec3::Vec3(const Vec2& vec , float zValue )
{
	x = vec.x;
	y = vec.y;
	z = zValue;
	
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + ( const Vec3& vecToAdd ) const
{
	Vec3 r= Vec3();
	r.x=this->x+vecToAdd.x;
	r.y=this->y+vecToAdd.y;
	r.z=this->z+vecToAdd.z;
	return Vec3( r.x, r.y ,r.z);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	Vec3 r= Vec3();
	r.x=this->x-vecToSubtract.x;
	r.y=this->y-vecToSubtract.y;
	r.z=this->z-vecToSubtract.z;
	return Vec3( r.x, r.y,r.z );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-() const
{
	return Vec3( -this->x, -this->y,-this->z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	Vec3 r= Vec3();
	r.x=this->x*uniformScale;
	r.y=this->y*uniformScale;
	r.z=this->z*uniformScale;
	return Vec3( r.x, r.y,r.z );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( const Vec3& vecToMultiply ) const
{
	Vec3 r= Vec3();
	r.x=this->x*vecToMultiply.x;
	r.y=this->y*vecToMultiply.y;
	r.y=this->z*vecToMultiply.z;
	return Vec3( r.x, r.y,r.z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	Vec3 r=Vec3();
	r.x=this->x/inverseScale;
	r.y=this->y/inverseScale;
	r.z=this->z/inverseScale;
	return Vec3( r.x, r.y,r.z );
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	this->x=copyFrom.x;
	this->y=copyFrom.y;
	this->z=copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	Vec3 r=Vec3();
	r.x=uniformScale*vecToScale.x;
	r.y=uniformScale*vecToScale.y;
	r.z=uniformScale*vecToScale.z;
	return Vec3( r.x, r.y, r.z );
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	if( this->x==compare.x && this->y==compare.y && this->z==compare.z )
	{
		return true;
	}
	else
		return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	if( this->x!=compare.x || this->y!=compare.y || this->z!=compare.z )
	{
		return true;
	}
	else
		return false;
}


float Vec3::GetLength() const
{
	return(sqrt((x*x)+(y*y)+(z*z)));
}

float Vec3::GetLengthXY() const
{
	return(sqrt((x*x)+(y*y)));
}

float Vec3::GetLengthSquared() const
{
	return(sqrt(((x*x)+(y*y)+(z*z))*((x*x)+(y*y)+(z*z))));
}

float Vec3::GetLengthXYSquared() const
{
	return(sqrt(((x*x)+(y*y))*((x*x)+(y*y))));
}

float Vec3::GetAngleAboutZDegrees() const
{
	float degrees = atan2( y, x )*180.f/3.1415926f;
	return(degrees);
}

float Vec3::GetAngleAboutZRadians() const
{
	return(atan2(y,x));
}

const Vec3 Vec3::GetRotatedAboutZDegrees( float deltaDegrees ) const
{
	Vec3 temp= Vec3(x,y,z);
	float r= sqrt( (x*x)+(y*y) );
	float thetaDegrees= atan2( y, x )*180.f/3.1415926f;
	thetaDegrees+=deltaDegrees;
	temp.x= r*CosDegrees( thetaDegrees );
	temp.y= r*SinDegrees( thetaDegrees );

	return temp;
}

const Vec3 Vec3::GetRotatedAboutZRadians( float deltaRadians ) const
{
	Vec3 temp= Vec3(x,y,z);
	float r= sqrt( (x*x)+(y*y) );
	float thetaRadians= atan2( y, x );
	thetaRadians+=deltaRadians;
	temp.x=r*cos( thetaRadians );
	temp.y=r*sin( thetaRadians );

	return temp;
}

const Vec3 Vec3::GetClamped( float maxLength ) const
{
	float r=sqrt( (x*x)+(y*y)+(z*z) );
	float theta= atan2( y, x );
	Vec3 temp= Vec3( x, y, z );

	if( r > maxLength )
	{
		temp.x=maxLength*cos( theta );
		temp.y=maxLength*sin( theta );
	}

	return temp;
}

const Vec3 Vec3::GetNormalized() const
{
	float r=sqrt( (x*x)+(y*y)+(z*z) );
	Vec3 temp= Vec3();

	if( r!=0.f )
	{
		float scale= 1.f/r;
		temp.x=scale*x;
		temp.y=scale*y;
		temp.z=scale*z;
	}

	return temp;
}

void Vec3::SetFromText( const char* asText )
{
	std::vector< std::string > splitStrings = SplitStringOnDelimiter( asText, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 3, Stringf( "Vec2 can't construct from improper string \"%s\"", asText ) );
	x = (float)atof( splitStrings[0].c_str() );
	y = (float)atof( splitStrings[1].c_str() );
	z = (float)atof( splitStrings[2].c_str() );
}
