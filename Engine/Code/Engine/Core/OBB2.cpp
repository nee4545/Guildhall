#include "Engine/Core/OBB2.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cstdlib>

OBB2::OBB2( const OBB2& copyFrom )
{
	m_center = copyFrom.m_center;
	m_halfDimensions = copyFrom.m_halfDimensions;
	m_iBasis = copyFrom.m_iBasis;
}

OBB2::OBB2( const Vec2& center, const Vec2& fullDimensions, const Vec2& iBasisNormal/*=Vec2(1.f,0.f)*/ )
{
	m_center = center;
	m_halfDimensions = fullDimensions*0.5f;
	m_iBasis = iBasisNormal;
}

OBB2::OBB2( const Vec2& center, const Vec2& fullDimensions, float orientationDegrees )
{
	m_center= center;
	m_halfDimensions = fullDimensions*0.5f;
	Vec2 newVec =Vec2::MakeFromPolarDegrees(orientationDegrees);
	newVec.Normalize();
	m_iBasis = newVec;
}

OBB2::OBB2( const AABB2& asAxisAlignedBox, float orientationDegrees/*=0.f*/ )
{
	m_center = (asAxisAlignedBox.maxs - asAxisAlignedBox.mins)*0.5f + asAxisAlignedBox.mins;
	m_halfDimensions = (asAxisAlignedBox.maxs - asAxisAlignedBox.mins)*0.5f;
	Vec2 newVec =Vec2::MakeFromPolarDegrees( orientationDegrees );
	newVec.Normalize();
	m_iBasis = newVec;
}

bool OBB2::IsPointInside( const Vec2& point ) const
{
	//Get Point in Local Space
	Vec2 pLocal = point - m_center;
	float pi =  DotProduct2D(pLocal,m_iBasis);
	float pj =  DotProduct2D(pLocal,m_iBasis.GetRotated90Degrees());

	if( pi > m_halfDimensions.x|| pi < -m_halfDimensions.x || pj > m_halfDimensions.y || pj < -m_halfDimensions.y )
	{
		return false;
	}
	else
	{
		return true;
	}
}

const Vec2 OBB2::GetCenter() const
{
	return m_center;
}

const Vec2 OBB2::GetDimensions() const
{
	return m_halfDimensions * 2.f;
}

const Vec2 OBB2::GetIBasisNormal() const
{
	return m_iBasis;
}

const Vec2 OBB2::GetJBasisNormal() const
{
	return m_iBasis.GetRotated90Degrees();
}

float OBB2::GetOrientationDegrees() const
{
	return m_iBasis.GetAngleDegrees();
}

const Vec2 OBB2::GetNearestPoint( const Vec2& referencePoint ) const
{
	//Get Point in Local Space
	Vec2 pLocal = referencePoint - m_center;
	Vec2 jBasis = m_iBasis.GetRotated90Degrees();
	float pi =  DotProduct2D( pLocal, m_iBasis );
	float pj =  DotProduct2D( pLocal, m_iBasis.GetRotated90Degrees() );

	float ni = Clamp(pi,-m_halfDimensions.x,m_halfDimensions.x);
	float nj = Clamp(pj,-m_halfDimensions.y,m_halfDimensions.y);

	Vec2 n = m_center + (m_iBasis*ni) + (jBasis*nj);

	return n;
}

const Vec2 OBB2::GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const
{
	Vec2 rightVector = m_iBasis * m_halfDimensions.x;
	Vec2 upVector = m_iBasis.GetRotated90Degrees() * m_halfDimensions.y;

	Vec2 obbMins = m_center - rightVector - upVector;
	Vec2 obbMaxs = m_center + rightVector + upVector;

	float obbMinsLocalx = DotProduct2D(obbMins,m_iBasis);
	float obbMinsLocaly = DotProduct2D(obbMins,m_iBasis.GetRotated90Degrees());

	float obbMaxLocalx = DotProduct2D(obbMaxs,m_iBasis);
	float obbMaxLocaly = DotProduct2D(obbMaxs,m_iBasis.GetRotated90Degrees());

	float pointX= RangeMapFloat( obbMinsLocalx, obbMaxLocalx, 0.f, 1.f, uvCoordsZeroToOne.x );
	float pointY= RangeMapFloat( obbMinsLocaly, obbMaxLocaly, 0.f, 1.f, uvCoordsZeroToOne.y );

	Vec2 pointAtUV = obbMins + (m_iBasis*pointX) + (m_iBasis.GetRotated90Degrees()*pointY);

	return pointAtUV;

}

const Vec2 OBB2::GetUVForPoint( const Vec2& point ) const
{
	Vec2 rightVector = m_iBasis * m_halfDimensions.x;
	Vec2 upVector = m_iBasis.GetRotated90Degrees() * m_halfDimensions.y;

	Vec2 obbMins = m_center - rightVector - upVector;
	Vec2 obbMaxs = m_center + rightVector + upVector;

	Vec2 pLocal = point - m_center;

	float pi =  DotProduct2D( pLocal, m_iBasis );
	float pj =  DotProduct2D( pLocal, m_iBasis.GetRotated90Degrees() );

	float obbMinsLocalx = DotProduct2D( obbMins, m_iBasis );
	float obbMinsLocaly = DotProduct2D( obbMins, m_iBasis.GetRotated90Degrees() );

	float obbMaxLocalx = DotProduct2D( obbMaxs, m_iBasis );
	float obbMaxLocaly = DotProduct2D( obbMaxs, m_iBasis.GetRotated90Degrees() );

	float pointX= RangeMapFloat(  0.f, 1.f,obbMinsLocalx,obbMaxLocalx, pi );
	float pointY= RangeMapFloat(  0.f, 1.f, obbMinsLocaly,obbMaxLocaly,pj );

	return Vec2(pointX,pointY);
}

void OBB2::GetCornerPositions( Vec2* out_fourPoints ) const
{

	Vec2 rightVector = m_iBasis * m_halfDimensions.x;
	Vec2 upVector = m_iBasis.GetRotated90Degrees()*m_halfDimensions.y;

	out_fourPoints[ 0 ] = m_center - rightVector - upVector; //bottom left
	out_fourPoints[ 1 ] = m_center + rightVector - upVector; //bottom right
	out_fourPoints[ 2 ] = m_center + rightVector + upVector; //top right
	out_fourPoints[ 3 ] = m_center - rightVector + upVector; //top left
	
}

float OBB2::GetOuterRadius() const
{
	if ( m_halfDimensions.x >= m_halfDimensions.y )
	{
		return m_halfDimensions.x;
	}
	else
	{
		return m_halfDimensions.y;
	}
}

float OBB2::GetInnerRadius() const
{
	if ( m_halfDimensions.x <= m_halfDimensions.y )
	{
		return m_halfDimensions.x;
	}
	else
	{
		return m_halfDimensions.y;
	}
}

void OBB2::Translate( const Vec2& translation )
{
	m_center+=translation;
}

void OBB2::SetCenter( const Vec2& newCenter )
{
	m_center = newCenter;
}

void OBB2::SetDimensions( const Vec2& newDimensions )
{
	m_halfDimensions = newDimensions*0.5f;
}

void OBB2::SetOrientationDegrees( float newAbsoluteOrientationDegrees )
{
	m_iBasis = Vec2::MakeFromPolarDegrees(newAbsoluteOrientationDegrees);
}

void OBB2::RotateByDegrees( float relativeOrientationDegrees )
{
	float currentDegrees = m_iBasis.GetAngleDegrees();
	currentDegrees+=relativeOrientationDegrees;

	m_iBasis = Vec2::MakeFromPolarDegrees(currentDegrees);
}

void OBB2::StretchToIncludePoint( const Vec2& point )
{
	if( IsPointInside( point ) )
	{
		return;
	}

	Vec2 nearestPoint = GetNearestPoint( point );
	Vec2 deltaIncrement = ( point - nearestPoint ) *0.5f; 

	Vec2 jBasis = m_iBasis.GetRotated90Degrees();
	float pi = DotProduct2D( deltaIncrement , m_iBasis );
	float pj = DotProduct2D( deltaIncrement , jBasis );
	Vec2 pointLocalCoords( pi , pj );
	m_center += deltaIncrement;											 
	m_halfDimensions += ( Vec2(abs(pointLocalCoords.x),abs(pointLocalCoords.y)) );	 
}

void OBB2::Fix()
{
	m_halfDimensions.x = abs( m_halfDimensions.x );
	m_halfDimensions.y = abs( m_halfDimensions.y );

	if ( m_iBasis == Vec2( 0.f , 0.f ) )
	{
		m_iBasis = Vec2( 1.f , 0.f );
	}

	m_iBasis.Normalize();
}

void OBB2::operator=( const OBB2& assignFrom )
{
	m_center = assignFrom.m_center;
	m_iBasis = assignFrom.m_iBasis;
	m_halfDimensions = assignFrom.m_halfDimensions;
}

