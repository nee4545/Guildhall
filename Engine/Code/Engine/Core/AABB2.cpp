#include "Engine/Core/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>

AABB2::AABB2( const Vec2& mins, const Vec2& maxs )
{
	this->mins=mins;
	this->maxs=maxs;
}

AABB2::AABB2( float minX, float minY, float maxX, float maxY )
{
	mins.x=minX;
	mins.y=minY;

	maxs.x=maxX;
	maxs.y=maxY;
}

AABB2::AABB2( const AABB2& copyFrom )
{
	mins=copyFrom.mins;
	maxs=copyFrom.maxs;
}



AABB2::AABB2( int minX , int minY , int maxX , int maxY )
{
	mins.x = ( float ) minX;
	mins.y = ( float ) minY;
	maxs.x = ( float ) maxX;
	maxs.y = ( float ) maxY;
}

bool AABB2::IsPointInside( const Vec2& point ) const //does not include positions on edge
{
	if( (point.x>mins.x && point.x<maxs.x) && (point.y>mins.y &&point.y<maxs.y) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

const Vec2 AABB2::GetCenter() const
{
	return Vec2((maxs.x-mins.x)*0.5f+mins.x,(maxs.y-mins.y)*0.5f+mins.y);
}

const Vec2 AABB2::GetDimensions() const
{
	return Vec2(maxs.x-mins.x,maxs.y-mins.y);
}

const Vec2 AABB2::GetNearestPoint( const Vec2& referencePosition ) const
{

	if( referencePosition.x<mins.x && (referencePosition.y>mins.y && referencePosition.y<maxs.y) )
	{
		return Vec2(mins.x,referencePosition.y);
	}

	if( referencePosition.x>maxs.x && (referencePosition.y>mins.y && referencePosition.y<maxs.y) )
	{
		return Vec2(maxs.x,referencePosition.y);
	}

	if( referencePosition.y > maxs.y && (referencePosition.x>mins.x && referencePosition.x<maxs.x) )
	{
		return Vec2(referencePosition.x, maxs.y);
	}

	if( referencePosition.y < mins.y && (referencePosition.x>mins.x && referencePosition.x<maxs.x) )
	{
		return Vec2(referencePosition.x,mins.y);
	}

	if( referencePosition.x<=mins.x && (referencePosition.y>=maxs.y) )
	{
		return Vec2(mins.x,maxs.y);
	}

	if( referencePosition.x<=mins.x && referencePosition.y<=mins.y )
	{
		return Vec2(mins.x,mins.y);
	}

	if( referencePosition.x>=maxs.x && referencePosition.y>=maxs.y )
	{
		return Vec2(maxs.x,maxs.y);
	}

	if( referencePosition.x>=maxs.x && referencePosition.y<=mins.y )
	{
		return Vec2(maxs.x,mins.y);
	}

	return referencePosition;
}

const Vec2 AABB2::GetPointAtUV( const Vec2& uvCoorsZeroToOne ) const
{
	float pointX= RangeMapFloat(0.f,1.f,mins.x,maxs.x,uvCoorsZeroToOne.x);
	float pointY= RangeMapFloat(0.f,1.f,mins.y,maxs.y,uvCoorsZeroToOne.y);

	return Vec2(pointX,pointY);
}

const Vec2 AABB2::GetUVForPoint( const Vec2& point ) const
{
	float pointX= RangeMapFloat( mins.x, maxs.x, 0.f, 1.f, point.x );
	float pointY= RangeMapFloat( mins.y, maxs.y, 0.f, 1.f, point.y );

	return Vec2(pointX,pointY);
}

float AABB2::GetOuterRadius() const
{
	Vec2 dimensions = GetDimensions();

	if ( dimensions.x > dimensions.y )
	{
		return dimensions.x * 0.5f;
	}
	else
	{
		return dimensions.y * 0.5f;
	}
}

float AABB2::GetInnerRadius() const
{
	Vec2 dimensions = GetDimensions();

	if ( dimensions.x > dimensions.y )
	{
		return dimensions.x * 0.5f;
	}
	else
	{
		return dimensions.y * 0.5f;
	}
}

void AABB2::GetCornerPositions( Vec2* outFourPoints ) const
{
	outFourPoints[ 0 ] = Vec2( mins );
	outFourPoints[ 1 ] = Vec2( maxs.x , mins.y );
	outFourPoints[ 2 ] = Vec2( maxs );
	outFourPoints[ 3 ] = Vec2( mins.x , maxs.y );
}

AABB2 AABB2::GetBoxAtLeft( float fractionOfWidth , float additionalWidth /*= 0.f */ ) const
{
	Vec2 dimensions = GetDimensions();

	Vec2 min = mins;
	Vec2 max = Vec2( min.x + ( fractionOfWidth * dimensions.x )+additionalWidth , maxs.y );

	AABB2 toReturn = AABB2( min , max );
	return toReturn;
}

AABB2 AABB2::GetBoxAtRight( float fractionOfWidth , float additionalWidth /*= 0.f */ ) const
{
	Vec2 dimensions = GetDimensions();

	Vec2 max = maxs;
	Vec2 min = Vec2( maxs.x - additionalWidth - ( fractionOfWidth * dimensions.x ) , mins.y );

	AABB2 toReturn = AABB2( min , max );
	return toReturn;
}

AABB2 AABB2::GetBoxAtBottom( float fractionOfHeight , float additionalHeight /*= 0.f */ ) const
{
	Vec2 dimensions = GetDimensions();

	Vec2 min = mins;
	Vec2 max = Vec2( maxs.x , min.y + ( fractionOfHeight * dimensions.y ) + additionalHeight );

	AABB2 toReturn = AABB2( min , max );
	return toReturn;

}

AABB2 AABB2::GetBoxAtTop( float fractionOfHeight , float additionalHeight /*= 0.f */ ) const
{
	Vec2 dimensions = GetDimensions();

	Vec2 max = maxs;
	Vec2 min = Vec2( mins.x , max.y - additionalHeight - ( fractionOfHeight * dimensions.y ) );

	AABB2 toReturn = AABB2( min , max );
	return toReturn;
}

void AABB2::Translate( const Vec2& translation )
{
	mins+=translation;
	maxs+=translation;
}

void AABB2::SetCenter( const Vec2& newCenter )
{
 	float minx=newCenter.x-((maxs.x-mins.x)*0.5f);
	float miny=newCenter.y-((maxs.y-mins.y)*0.5f);

	float maxx=newCenter.x+((maxs.x-mins.x)*0.5f);
	float maxy=newCenter.y+((maxs.y-mins.y)*0.5f);

	mins.x=minx;
	mins.y=miny;
	maxs.x=maxx;
	maxs.y=maxy;
}

void AABB2::SetDimension( const Vec2& newDimensions ) 
{
	AABB2 temp=AABB2(Vec2(0.f,0.f),newDimensions);
	temp.SetCenter(this->GetCenter());

	this->mins=temp.mins;
	this->maxs=temp.maxs;

}

void AABB2::StretchToIncludePoint( const Vec2& point )
{
	if( point.x<=mins.x && (point.y<=maxs.y && point.y>=mins.y))
	{
		mins.x=point.x;
	}

	if( point.x>=maxs.x && (point.y<=maxs.y && point.y>=mins.y) )
	{
		maxs.x=point.x;
	}

	if( point.y<=mins.y && (point.x<=maxs.x && point.x>=mins.x) )
	{
		mins.y=point.y;
	}

	if( point.y>=maxs.y && (point.x<=maxs.x && point.x>=mins.x) )
	{
		maxs.y=point.y;
	}

	if( point.x<mins.x && (point.y<mins.y) )
	{
		mins=point;
	}

	if( point.x<mins.x && point.y>maxs.y )
	{
		mins.x=point.x;
		maxs.y=point.y;
	}

	if( point.x > maxs.x && point.y>maxs.y )
	{
		maxs=point;
	}

	if( point.x > maxs.x && point.y<mins.y )
	{
		maxs.x=point.x;
		mins.y=point.y;
	}

}

void AABB2::CarveBoxOffLeft( float fractionOfWidth , float additionalWidth /*= 0.f */ )
{
	Vec2 dimensions = GetDimensions();

	Vec2 min = mins;
	Vec2 max = Vec2( min.x + ( fractionOfWidth * dimensions.x ) + additionalWidth , maxs.y );

	AABB2 newAAbb = AABB2( min , max );
	*this = newAAbb;
}

void AABB2::CarveBoxOffRight( float fractionOfWidth , float additionalWidth /*= 0.f */ )
{
	Vec2 dimensions = GetDimensions();

	Vec2 max = maxs;
	Vec2 min = Vec2( maxs.x - additionalWidth - ( fractionOfWidth * dimensions.x ) , mins.y );

	AABB2 newAAbb = AABB2( min , max );
	*this = newAAbb;
}

void AABB2::CarveBoxOffTop( float fractionOfHeight , float additionalHeight /*= 0.f */ )
{
	Vec2 dimensions = GetDimensions();

	Vec2 max = maxs;
	Vec2 min = Vec2( mins.x , max.y - additionalHeight - ( fractionOfHeight * dimensions.y ) );

	AABB2 newAAbb = AABB2( min , max );
	*this = newAAbb;
}

void AABB2::CarveBoxOffBottom( float fractionOfHeight , float additionalHeight /*= 0.f */ )
{
	Vec2 dimensions = GetDimensions();

	Vec2 min = mins;
	Vec2 max = Vec2( maxs.x , min.y + ( fractionOfHeight * dimensions.y ) + additionalHeight );

	AABB2 newAAbb = AABB2( min , max );
	*this = newAAbb;
}

AABB2 AABB2::GetPercentageOfAABB( float percentage )
{
	AABB2 toReturn;
	Vec2 dimenstions = GetDimensions();
	float width = percentage * dimenstions.x;
	float height = percentage * dimenstions.y;

	toReturn.mins = Vec2( 0.f , 0.f );
	toReturn.maxs = Vec2( width , height );
	toReturn.SetCenter( GetCenter() );

	return toReturn;
}

void AABB2::operator=( const AABB2& assignFrom )
{
	mins=assignFrom.mins;
	maxs=assignFrom.maxs;
}

bool AABB2::operator==( const AABB2& compareWith ) const
{
	if( ((maxs.x-mins.x) == (compareWith.maxs.x-compareWith.mins.x)) && ((maxs.y-mins.y) == (compareWith.maxs.y-compareWith.mins.y)) )
	{
		return true;
	}
	else
		return false;
}
