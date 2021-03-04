#include "Engine/Math/Vec2.hpp"
#include <cmath>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy ): x( 123.f ), y( 456.f ){

	this->x=copy.x;
	this->y=copy.y;
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY ): x( 123.f ), y( 456.f )
{
	this->x=initialX;
	this->y=initialY;
}


Vec2::Vec2( const IntVec2& copyFrom )
{
	x = ( float ) copyFrom.x;
	y = ( float ) copyFrom.y;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	Vec2 r= Vec2();
	r.x=this->x+vecToAdd.x;
	r.y=this->y+vecToAdd.y;
	return Vec2( r.x, r.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	Vec2 r= Vec2();
	r.x=this->x-vecToSubtract.x;
	r.y=this->y-vecToSubtract.y;
	return Vec2( r.x, r.y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( -this->x, -this->y);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	Vec2 r= Vec2();
	r.x=this->x*uniformScale;
	r.y=this->y*uniformScale;
	return Vec2( r.x, r.y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	Vec2 r= Vec2();
	r.x=this->x*vecToMultiply.x;
	r.y=this->y*vecToMultiply.y;
	return Vec2( r.x,r.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	Vec2 r=Vec2();
	r.x=this->x/inverseScale;
	r.y=this->y/inverseScale;
	return Vec2( r.x, r.y );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	this->x=copyFrom.x;
	this->y=copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	Vec2 r=Vec2();
	r.x=uniformScale*vecToScale.x;
	r.y=uniformScale*vecToScale.y;
	return Vec2( r.x, r.y);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	if( this->x==compare.x && this->y==compare.y )
	{
		return true;
	}
	else
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	if( this->x!=compare.x || this->y!=compare.y )
	{
		return true;
	}
	else
		return false;
}

float Vec2::GetLength() const
{
	return(sqrt((x*x)+(y*y)));
}

float Vec2::GetLengthSquared() const
{
	return(sqrt((x*x)+(y*y))*sqrt((x*x)+(y*y)));
}

float Vec2::GetAngleDegrees() const
{
	float degrees = atan2( y, x )*180.f/3.1415926f;
	return(degrees);
	
}

float Vec2::GetAngleRadians() const
{
	return(atan2(y,x));
}

const Vec2 Vec2::GetRotatedRadians( float deltaRadians ) const
{
	Vec2 temp= Vec2(0.0f,0.0f);
	float r= sqrt((x*x)+(y*y));
	float thetaRadians= atan2(y,x);
	thetaRadians+=deltaRadians;
	temp.x=r*cos(thetaRadians);
	temp.y=r*sin(thetaRadians);

	return temp;
}

const Vec2 Vec2::GetRotatedDegrees( float deltaDegrees ) const
{
	Vec2 temp= Vec2( 0.0f, 0.0f );
	float r= sqrt( (x*x)+(y*y) );
	float thetaDegrees= atan2(y,x)*180.f/3.1415926f;
	thetaDegrees+=deltaDegrees;
	temp.x= r*CosDegrees(thetaDegrees);
	temp.y= r*SinDegrees(thetaDegrees);

	return temp;
}

const Vec2 Vec2::GetRotated90Degrees() const
{
	return(GetRotatedDegrees(90.0f));
}

const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return(GetRotatedDegrees(-90.f));
}

const Vec2 Vec2::GetNormalized() const
{
	float r=sqrt((x*x)+(y*y));
	Vec2 temp= Vec2();

	if( r!=0.f )
	{
		float scale= 1.f/r;
		temp.x=scale*x;
		temp.y=scale*y;

	}

	return temp;

	
	/*float theta = atan2(y,x);
	float tempX = cos(theta);
	float tempY = sin(theta);
	return(Vec2(tempX,tempY));*/

}

const Vec2 Vec2::GetClamped(float maxLength) const
{
	float r=sqrt( (x*x)+(y*y) );
	float theta= atan2(y,x);
	Vec2 temp= Vec2( x, y );

	if( r > maxLength )
	{
		temp.x=maxLength*cos(theta);
		temp.y=maxLength*sin(theta);
	}

	return temp;
}

const Vec2 Vec2::MakeFromPolarDegrees( float directionDegrees, float length )
{
	Vec2 vecToReturn=Vec2();

	vecToReturn.x=length*CosDegrees(directionDegrees);
	vecToReturn.y=length*SinDegrees(directionDegrees);

	return vecToReturn;
	  
}


const Vec2 Vec2::MakeFromPolarRadians( float directionRadians, float length )
{
	Vec2 vecToReturn=Vec2();

	vecToReturn.x=length*cos( directionRadians );
	vecToReturn.y=length*sin( directionRadians );

	return vecToReturn;
}


void Vec2::SetAngleDegrees( float newAngleDegress )
{
	float r= sqrt((x*x)+(y*y));

	x= r * CosDegrees(newAngleDegress);
	y= r * SinDegrees(newAngleDegress);
}

void Vec2::SetAngleRadians( float newAngleRadians )
{
	float r= sqrt((x*x)+(y*y));

	x= r * cos(newAngleRadians);
	y= r * sin(newAngleRadians);
}

void Vec2::SetPolarDegrees( float newAngleDegrees, float newLength )
{
	x = newLength * CosDegrees(newAngleDegrees);
	y = newLength * SinDegrees(newAngleDegrees);
}

void Vec2::SetPolarRadians( float newAngleRadians, float newLength )
{
	x = newLength * cos(newAngleRadians);
	y = newLength * sin(newAngleRadians);
} 

void Vec2::RotateDegrees( float deltaDegrees )
{
	float r= sqrt( (x*x)+(y*y) );
	float thetaDegrees= atan2( y, x )*180.f/3.1415926f;
	thetaDegrees+=deltaDegrees;
	x= r*CosDegrees( thetaDegrees );
	y= r*SinDegrees( thetaDegrees );

}  

void Vec2::RotateRadians( float deltaRadians )
{
	float r= sqrt( (x*x)+(y*y) );
	float thetaRadians= atan2( y, x );
	thetaRadians+=deltaRadians;
	x=r*cos( thetaRadians );
	y=r*sin( thetaRadians );
}

void Vec2::Rotate90Degrees()
{
	this->RotateDegrees(90.f);
}

void Vec2::RotateMinus90Degrees()
{
	this->RotateDegrees(-90.f);
}

void Vec2::SetLength(float newLength)
{
	float theta= atan2(y,x);

	x= newLength*cos(theta);
	y= newLength*sin(theta);
}

void Vec2::ClampLength( float maxLength )
{
	float r=sqrt( (x*x)+(y*y) );
	float theta= atan2( y, x );

	if( r > maxLength )
	{
		x=maxLength*cos( theta );
		y=maxLength*sin( theta );
	}

}

void Vec2::Normalize()
{
	float r=sqrtf( (x*x)+(y*y) );

	if( r!=0.f )
	{
		float scale= 1.f/r;
		x*=scale;
		y*=scale;

	}
}

float Vec2::NormalizeAndGetPreviousLength()
{
	float r=sqrtf( (x*x)+(y*y) );

	if( r!=0.f )
	{
		float scale= 1.f/r;
		x*=scale;
		y*=scale;

	}

	return r;
}

void Vec2::Reflect( const Vec2& normalVector )
{
	Vec2 normalCopy = normalVector.GetNormalized();
	*this = *this - (2 * (DotProduct2D( *this, normalCopy)*normalCopy));
	
}

const Vec2 Vec2::GetReflected( const Vec2& normalVector ) const
{
	Vec2 normalCopy = normalVector.GetNormalized();
	return  *this - (2 * (DotProduct2D( *this, normalCopy)*normalCopy));
}

void Vec2::SetFromText( const char* asText )
{
	std::vector< std::string > splitStrings = SplitStringOnDelimiter( asText, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 2, Stringf( "Vec2 can't construct from improper string \"%s\"", asText ) );
	x = (float)atof( splitStrings[0].c_str() );
	y = (float)atof( splitStrings[1].c_str() );
}






