#include "Engine/Math/IntVec2.hpp"
#include <cmath>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

IntVec2::IntVec2( const IntVec2& copyFrom )
{
	x=copyFrom.x;
	y=copyFrom.y;
}

IntVec2::IntVec2( int initialX, int initialY )
{
	x=initialX;
	y=initialY;
}

float IntVec2::GetLength() const
{
	float length=0;
	length=(float)sqrt(x*x+y*y);

	return length;
}

int IntVec2::GetLengthSquared() const
{
	return(x*x+y*y);
}

int IntVec2::GetTaxicabLength() const
{
	return(abs(x)+abs(y));
}

float IntVec2::GetOrientationRadians() const
{
	return((float)atan2(y,x));

}

float IntVec2::GetOrientationDegrees() const
{
	float degrees =(float) atan2( y, x )*180.f/3.1415926f;
	return(degrees);
}

const IntVec2 IntVec2::GetRotated90Degrees() const
{
	int newX=-y;
	int newY=x;

	return IntVec2(newX,newY);
}

const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	int newX= y;
	int newY= -x;

	return IntVec2(newX,newY);
}

void IntVec2::Rotate90Degrees()
{
	int x1=-y;
	int y1=x;

	x=x1;
	y=y1;
}

void IntVec2::RotateMinus90Degrees()
{
	
	int x1=y;
	int y1=-x;

	x=x1;
	y=y1;
}

const IntVec2 IntVec2::operator-=( const IntVec2& vecToSubtract ) const
{
	int newX= x-vecToSubtract.x;
	int newY= y-vecToSubtract.y;

	return(IntVec2(newX,newY));
}

const IntVec2 IntVec2::operator-() const
{
	return(IntVec2(-x,-y));
}

const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	IntVec2 r;
	r.x = this->x - vecToSubtract.x;
	r.y = this->y - vecToSubtract.y;
	return r;
}

void IntVec2::operator=( const IntVec2& copyFrom )
{
	x=copyFrom.x;
	y=copyFrom.y;
}

void IntVec2::operator*=( const IntVec2& vecToMultiply )
{
	x*=vecToMultiply.x;
	y*=vecToMultiply.y;
}

const IntVec2 IntVec2::operator*( int uniformScale ) const
{
	int newX=x*uniformScale;
	int newY=y*uniformScale;

	return(IntVec2(newX,newY));
}

const IntVec2 IntVec2::operator*( const IntVec2& vecToMultiply ) const
{
	int newX= x*vecToMultiply.x;
	int newY= y*vecToMultiply.y;

	return(IntVec2(newX,newY));
}

void IntVec2::SetFromText( const char* asText )
{
	std::vector< std::string > splitStrings = SplitStringOnDelimiter( asText, ',' );

	if ( splitStrings.size() == 1 )
	{
		splitStrings = SplitStringOnDelimiter( asText , '~' );
	}

	GUARANTEE_OR_DIE( splitStrings.size() == 2, Stringf( "Vec2 can't construct from improper string \"%s\"", asText ) );
	x = atoi( splitStrings[0].c_str() );
	y = atoi( splitStrings[1].c_str() );
}

void IntVec2::operator-=( const IntVec2& vecToSubtract )
{
	x-=vecToSubtract.x;
	y-=vecToSubtract.y;
}

void IntVec2::operator+=( const IntVec2& vecToAdd )
{
	x+=vecToAdd.x;
	y+=vecToAdd.y;
}

const IntVec2 IntVec2:: operator+=( const IntVec2& vecToAdd ) const
{
	int newX= x+vecToAdd.x;
	int newY= y+vecToAdd.y;

	return(IntVec2(newX,newY));
}

bool IntVec2::operator!=( const IntVec2& compare ) const
{
	if( x!=compare.x || y!=compare.y )
	{
		return true;
	}
	else
		return false;
}

bool IntVec2::operator==( const IntVec2& compare ) const
{
	if( x==compare.x && y==compare.y )
	{
		return true;
	}
	else
	{
		return false;
	}
}

const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale )
{
	IntVec2 vecToReturn= IntVec2(0,0);
	vecToReturn.x=uniformScale*vecToScale.x;
	vecToReturn.y=uniformScale*vecToScale.y;

	return vecToReturn;
}










