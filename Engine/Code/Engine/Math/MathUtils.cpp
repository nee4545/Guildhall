#include "MathUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Polygon2D.hpp"
#include <cmath>
#include <vector>


float ConvertRadiansToDegrees( float radians )
{
	return(radians*180.f/3.1415926f);
}

float ConvertDegreesToRadians( float degrees )
{
	return (degrees*3.1415926f/180.f);
}


float CosDegrees( float degrees )
{
	float radians=ConvertDegreesToRadians(degrees);
	return cos(radians);
}

float SinDegrees( float degrees )
{
	float radians=ConvertDegreesToRadians(degrees);
	return sin(radians);
}

float Atan2Degrees( float y, float x )
{
	return atan2(y,x) * 180.0f/3.1415926f;
}

float GetShortestAngularDisplacement( float fromDegrees, float toDegrees )
{
	fromDegrees=fmod(fromDegrees,360.f);
	toDegrees=fmod(toDegrees,360.f);
	float angularDisplacement=toDegrees-fromDegrees;

	if( abs(angularDisplacement)<=180.f )
	{
		return angularDisplacement;
	}
	else
	{
		if(angularDisplacement<0 )
		{ 
		return 360.f+angularDisplacement;
		}
		else
		{ 
			angularDisplacement=angularDisplacement-360.f;
			return angularDisplacement;
		}
	}


}

float GetTurnedToward( float currentDegrees, float goalDegrees, float maxDeltaDegrees )
{
	float angularDisplacement=GetShortestAngularDisplacement(currentDegrees,goalDegrees);

	if( abs(angularDisplacement)<maxDeltaDegrees )
	{
		
		return goalDegrees;
	}
	else
	{ 
	   angularDisplacement=Clamp(angularDisplacement,-maxDeltaDegrees,maxDeltaDegrees);
	   return currentDegrees+angularDisplacement;
	}
}

float GetDistance2D(const Vec2& positionA,const Vec2& positionB)
{
	return(sqrt((positionB.x-positionA.x)*(positionB.x-positionA.x)+(positionB.y-positionA.y)*(positionB.y-positionA.y)));
}

float GetDistanceSquared2D( const Vec2& positionA, const Vec2& positionB )
{
	return((positionB.x-positionA.x)*(positionB.x-positionA.x)+(positionB.y-positionA.y)*(positionB.y-positionA.y));
}

float GetDistance3D( const Vec3& positionA, const Vec3& positionB )
{
	return(sqrt((positionB.x-positionA.x)*(positionB.x-positionA.x)+(positionB.y-positionA.y)*(positionB.y-positionA.y)+(positionB.z-positionA.z)*(positionB.z-positionA.z)));
}

float GetDistanceSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	return((positionB.x-positionA.x)*(positionB.x-positionA.x)+(positionB.y-positionA.y)*(positionB.y-positionA.y)+(positionB.z-positionA.z)*(positionB.z-positionA.z));
}

float GetDistanceXY3D( const Vec3& positionA, const Vec3& positionB )
{
	return(sqrt((positionB.x-positionA.x)*(positionB.x-positionA.x)+(positionB.y-positionA.y)*(positionB.y-positionA.y)));
}
float GetDistanceXYSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	return((positionB.x-positionA.x)*(positionB.x-positionA.x)+(positionB.y-positionA.y)*(positionB.y-positionA.y));
}

int GetTaxicabDistance2D( const IntVec2& positionA, const IntVec2& positionB )
{
	return (abs(positionB.x-positionA.x)+abs(positionB.y-positionA.y));
}

bool DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB )
{
	float dx = centerB.x-centerA.x;
	float dy = centerB.y-centerA.y;
    float d = sqrt((dx*dx)+(dy*dy));

	if( (radiusA+radiusB) >= d )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB )
{
	float dx= abs( centerB.x-centerA.x );
	float dy= abs( centerB.y-centerA.y );
	float dz= abs(centerB.z-centerA.z);

	float d= sqrt((dx*dx)+(dy*dy)+(dz*dz));

	if( (radiusA+radiusB) >= d )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DoOBBAndOBBOverlap2D( const OBB2& box1 , const OBB2& box2 )
{

	Vec2 box1CornerPoints[ 4 ];
	Vec2 box2CornerPoints[ 4 ];

	box1.GetCornerPositions( box1CornerPoints );
	box2.GetCornerPositions( box2CornerPoints );

	Vec2 axisTocheck = box1.GetIBasisNormal();

	FloatRange projectedPointsBox1 = GetRangeOnProjectedAxis( 4 , box1CornerPoints , Vec2( 0.f , 0.f ) , axisTocheck );
	FloatRange projectedPointsBox2 = GetRangeOnProjectedAxis( 4 , box2CornerPoints , Vec2( 0.f , 0.f ) , axisTocheck );

	if ( !projectedPointsBox1.DoesOverlap( projectedPointsBox2 ) )
	{
		return false;
	}

	axisTocheck = box1.GetJBasisNormal();

	projectedPointsBox1 = GetRangeOnProjectedAxis( 4 , box1CornerPoints , Vec2( 0.f , 0.f ) , axisTocheck );
	projectedPointsBox2 = GetRangeOnProjectedAxis( 4 , box2CornerPoints , Vec2( 0.f , 0.f ) , axisTocheck );

	if ( !projectedPointsBox1.DoesOverlap( projectedPointsBox2 ) )
	{
		return false;
	}

	axisTocheck = box2.GetIBasisNormal();


	projectedPointsBox1 = GetRangeOnProjectedAxis( 4 , box1CornerPoints , Vec2( 0.f , 0.f ) , axisTocheck );
	projectedPointsBox2 = GetRangeOnProjectedAxis( 4 , box2CornerPoints , Vec2( 0.f , 0.f ) , axisTocheck );

	if ( !projectedPointsBox1.DoesOverlap( projectedPointsBox2 ) )
	{
		return false;
	}

	axisTocheck = box2.GetJBasisNormal();

	projectedPointsBox1 = GetRangeOnProjectedAxis( 4 , box1CornerPoints , Vec2( 0.f , 0.f ) , axisTocheck );
	projectedPointsBox2 = GetRangeOnProjectedAxis( 4 , box2CornerPoints , Vec2( 0.f , 0.f ) , axisTocheck );

	if ( !projectedPointsBox1.DoesOverlap( projectedPointsBox2 ) )
	{
		return false;
	}


	return true;
}

bool DoDiscAndPolygonOverlap( const Vec2& discCenter , float discRadius , const Polygon2D& polygon )
{
	Vec2 nearestPoint = polygon.GetClosestPoint( discCenter );

	if ( IsPointInsideDisc2D( nearestPoint , discCenter , discRadius ) )
	{
		return true;
	}

	return false;

}



const Vec2 TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation )
{
	Vec2 transformedVec= position;
	
	transformedVec.x*= uniformScale;
	transformedVec.y*= uniformScale;

	transformedVec.RotateDegrees(rotationDegrees);

	transformedVec.x+=translation.x;
	transformedVec.y+=translation.y;

	return transformedVec;

}


const Vec2 TransformPosition2D( const Vec2& position, const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation )
{
	Vec2 transformedVec = Vec2(0.f,0.f);
	transformedVec.x = position.x*iBasis.x + position.y*jBasis.x + translation.x;
	transformedVec.y = position.x*iBasis.y + position.y*jBasis.y + translation.y;

	return transformedVec;
}

const Vec3 TransformPosition3DXY( const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXY )
{
	Vec3 transformedVec = position;

	transformedVec.x*= scaleXY;
	transformedVec.y*= scaleXY;
	//transformedVec.z*= scaleXY;

	transformedVec=transformedVec.GetRotatedAboutZDegrees(zRotationDegrees);

	transformedVec.x+=translationXY.x;
	transformedVec.y+=translationXY.y;
	

	return transformedVec;
}

const Vec3 TransformPosition3DXY( const Vec3& position, const Vec2& iBasisXY, const Vec2& jBasisXY, const Vec2& translationXY )
{
	Vec3 transformedVec = Vec3( 0.f, 0.f,0.f );

	transformedVec.x = position.x*iBasisXY.x + position.y*jBasisXY.x + translationXY.x;
	transformedVec.y = position.x*iBasisXY.y + position.y*jBasisXY.y + translationXY.y;
	transformedVec.z = position.z;

	return transformedVec;
}

float Interpolate( float a, float b, float fraction0fb )
{
	return a + fraction0fb *(b-a);
}

float RangeMapFloat( float inBegin, float inEnd, float outBegin, float outEnd, float inValue )
{
	float inDisplacement= inValue-inBegin;
	float inRange=inEnd-inBegin;
	float fraction= inDisplacement/inRange;
	float outRange= outEnd-outBegin;
	float outDisplacement= fraction*outRange;
	float outValue = outBegin+outDisplacement;

	return outValue;
}

float ClampZeroToOne( float value )
{
	{
		if( value<0 )
		{
			value=0.f;
			return value;
		}
		if( value>1 )
		{
			value=1.f;
			return value;
		}

		return value;
	}
}

float Clamp( float value, float min, float max )
{
	if( value<min )
	{
		return min;
	}

	if( value>max )
	{
		return max;
	}

	return value;
}

float Round( float value )
{
	if( value-RoundDownToInt( value )>=0.5f )
	{
		return (float)RoundDownToInt( value )+1.f;
	}
	else
		return (float)RoundDownToInt(value);
}

int RoundDownToInt( float value )
{
	if( value<0 )
	{
		return (int)value-1;
	}
	return (int)value;
}

int RoundToNearestInt( float value )
{
	if( value-RoundDownToInt( value )>=0.5f )
	{
		return RoundDownToInt(value)+1;
	}
	else
		return RoundDownToInt(value);
}

void PushDiscOutOFAABB2D( Vec2& center, float discRadius,const AABB2& aabb )
{
	Vec2 nearestPoint=aabb.GetNearestPoint(center);
	PushDiscOutOfPoint2D(center,discRadius,nearestPoint);

}


void PushDiscOutOfPoint2D( Vec2& center, float discRadius, const Vec2& point )
{
	if( GetDistance2D( center, point )>=discRadius )
	{
		return;
	}

	Vec2 displacement=center-point;

	displacement.Normalize();

	Vec2 radiusVec= displacement*discRadius;

	Vec2 moveOut=radiusVec-(center-point);

	center+=moveOut;
	
}

const Vec2 GetNearstPointOnDisc2D( const Vec2& referencePos, const Vec2& discCenter, float discRadius )
{
	if( IsPointInsideDisc2D( referencePos, discCenter, discRadius ) )
	{
		return referencePos;
	}

	Vec2 displacement = referencePos - discCenter;
	Vec2 fwd = displacement.GetNormalized();
	
	return discCenter + fwd*discRadius;
}

void PushDIscOutOfEachOther2D( Vec2& centerDisc1, float discRadius1, Vec2& centerDisc2, float discRadius2 )
{
	if( !DoDiscsOverlap( centerDisc1, discRadius1, centerDisc2, discRadius2 ) )
	{
		return;
	}

	Vec2 displacement=centerDisc2-centerDisc1;
	displacement.Normalize();
	float overlapDistance= discRadius1+discRadius2 -(centerDisc2-centerDisc1).GetLength();
	Vec2 correction=displacement*(overlapDistance*0.5f);
	centerDisc2+=correction;
	centerDisc1-=correction;
}



void PushMobileDiscOutOfFixedDisc( Vec2& mobileCenter, float mobileRadius,const Vec2& fixedCenter, float fixedRadius )
{
	if( !DoDiscsOverlap( mobileCenter, mobileRadius, fixedCenter, fixedRadius ) )
	{
		return;
	}

	Vec2 displacement= mobileCenter-fixedCenter;
	displacement.Normalize();
	float overlapDistance = mobileRadius+fixedRadius -(mobileCenter-fixedCenter).GetLength();
	Vec2 correction=displacement*overlapDistance;
	mobileCenter+=correction;
}

bool IsPointInForwardSector2D( const Vec2& point, const Vec2& observerPosition, float forwardDegrees, float apertureDegrees, float maxDistance )
{
	Vec2 displacement= point-observerPosition;
	float displacementAngle = displacement.GetAngleDegrees();
	float distance= sqrtf( ( observerPosition.x - point.x ) * ( observerPosition.x - point.x ) + ( observerPosition.y - point.y ) * ( observerPosition.y - point.y ) );

	if( distance > maxDistance )
	{
		return false;
	}

	if( abs(GetShortestAngularDisplacement(forwardDegrees,displacementAngle))<= apertureDegrees*0.5f )
	{
		return true;
	}
	return false;
}

bool IsPointInsideDisc2D( const Vec2& point, const Vec2& discCenter, float discRadius )
{
	Vec2 displacement = point - discCenter;
	float dispLength = displacement.GetLength();
	
	if( dispLength < discRadius )
	{
		return true;
	}
	
	return false;
}

bool IsPointInsideOBB2D( const Vec2& point, const OBB2& box )
{
	Vec2 pLocal = point - box.m_center;
	Vec2 jBasis = box.m_iBasis.GetRotated90Degrees();
	float pi =  DotProduct2D( pLocal, box.m_iBasis );
	float pj =  DotProduct2D( pLocal, box.m_iBasis.GetRotated90Degrees() );

	if( pi > box.m_halfDimensions.x|| pi < -box.m_halfDimensions.x || pj > box.m_halfDimensions.y || pj < -box.m_halfDimensions.y )
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool IsPointInsideCapsule2D( const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	OBB2 box = OBB2((capsuleMidEnd-capsuleMidStart)*0.5f+capsuleMidStart,Vec2((capsuleMidEnd-capsuleMidStart).GetLength(),capsuleRadius*2.f),(capsuleMidEnd-capsuleMidStart).GetNormalized());

	if( IsPointInsideDisc2D( point, capsuleMidStart, capsuleRadius ) )
	{
		return true;
	}
	else if(IsPointInsideDisc2D(point,capsuleMidEnd,capsuleRadius) )
	{
		 return true;
	}
	else if( IsPointInsideOBB2D( point, box ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

const Vec2 GetNearstPointOnOBB2D( const Vec2& refPos, const OBB2& box )
{
	Vec2 pLocal = refPos - box.m_center;
	Vec2 jBasis = box.m_iBasis.GetRotated90Degrees();
	float pi =  DotProduct2D( pLocal, box.m_iBasis );
	float pj =  DotProduct2D( pLocal, box.m_iBasis.GetRotated90Degrees() );

	float ni = Clamp( pi, -box.m_halfDimensions.x, box.m_halfDimensions.x );
	float nj = Clamp( pj, -box.m_halfDimensions.y, box.m_halfDimensions.y );

	Vec2 n = box.m_center + (box.m_iBasis*ni) + (jBasis*nj);

	return n;
}

const Vec2 GetNearestPointOnCapsule2D( const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	Vec2 nearestPoint = Vec2(0.f,0.f);
	Vec2 displacementStartToPoint = refPos - capsuleMidStart;
	Vec2 displacementStartToEnd = capsuleMidEnd - capsuleMidStart;
	Vec2 displacementEndToPoint = refPos - capsuleMidEnd;
	Vec2 direction = ( capsuleMidEnd - capsuleMidStart ).GetNormalized();

	if( DotProduct2D( direction, displacementStartToPoint ) <= 0 )
	{
		return GetNearstPointOnDisc2D(refPos,capsuleMidStart,capsuleRadius);
	}
	else if( DotProduct2D( -direction, displacementEndToPoint ) <= 0 )
	{
		return GetNearstPointOnDisc2D(refPos,capsuleMidEnd,capsuleRadius);
	}
	else
	{
		Vec2 pointProjectedOnCapsuleBone = GetProjectedOnto2D( displacementStartToPoint, displacementStartToEnd );
		nearestPoint = capsuleMidStart + pointProjectedOnCapsuleBone;
		Vec2 displacementNearestPointToPoint = refPos - nearestPoint;
		displacementNearestPointToPoint.ClampLength( capsuleRadius );
		nearestPoint += displacementNearestPointToPoint;
		return nearestPoint;
	}
}

float GetAngleDegreesBetweenVectors2D( const Vec2& vectorA, const Vec2& vectorB )
{
	float cosAngle = DotProduct2D(vectorA,vectorB) / (vectorA.GetLength() * vectorB.GetLength());
	float angle = acos(cosAngle);
	angle = ConvertRadiansToDegrees(angle);

	return angle;
}

const Vec2 GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	float length = DotProduct2D( sourceVector, ontoVector.GetNormalized() );
	return ontoVector.GetNormalized()*length;
}

float SmoothStart2( float t )
{
	return t * t;
}

float SmoothStart3( float t )
{
	return t * t * t;
}

float SmoothStart4( float t )
{
	return t * t * t * t;
}

float SmoothStart5( float t )
{
	return t * t * t * t * t;
}

float SmoothStop2( float t )
{
	return t * ( 2 - t );
}

float SmoothStop3( float t )
{
	float scale = 1 - t;
	return ( 1 - ( scale * scale * scale ) );
}

float SmoothStop4( float t )
{
	float scale = 1 - t;
	return ( 1 - ( scale * scale * scale * scale ) );
}

float SmoothStop5( float t )
{
	float scale = 1 - t;
	return ( 1 - ( scale * scale * scale * scale * scale ) );
}

float SmoothStep3( float t )
{
	return ( ( t * t ) * ( 3 - ( 2 * t ) ) );
}

float SignFloat( float val )
{
	return ( val >= 0.0f ) ? 1.0f : -1.0f;
}

float GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	Vec2 temp = ontoVector.GetNormalized();
	return DotProduct2D(sourceVector,temp);
}

const Vec2 GetNearestPointOnLineSegment2D( const Vec2& refPos, const Vec2& start, const Vec2& end )
{
	Vec2 direction = end - start;
	direction.Normalize();
	if ( DotProduct2D( direction , refPos - start ) <= 0 )
	{
		return start;
	}
	else if ( DotProduct2D( -direction , refPos - end ) <= 0 )
	{
		return end;
	}
	else
	{
		Vec2 tempVec = refPos - start;
		Vec2 result = GetProjectedOnto2D( tempVec , direction );
		result = result + start;
		return result;
	}

}

const Vec2 GetNearestPointOnInfiniteLine2D( const Vec2& refPos, const Vec2& somePosition, const Vec2& anotherPosition )
{
	Vec2 direction = anotherPosition-somePosition;
	direction.Normalize();

	Vec2 displacementFromStartToRef = refPos-somePosition;
	Vec2 result = GetProjectedOnto2D(displacementFromStartToRef,direction);
	result = result + somePosition;
	return result;
}

FloatRange GetRangeOnProjectedAxis( int numPoints , const Vec2* points , const Vec2& relativePosition , const Vec2& axisNormal )
{
	std::vector<Vec2> projectedPoints;

	for ( int index = 0; index < numPoints; index++ )
	{
		Vec2 displacementVector = points[ index ] - relativePosition;
		Vec2 projectedPoint = GetProjectedOnto2D( displacementVector , axisNormal );

		projectedPoints.push_back( projectedPoint );
	}

	float minLength = projectedPoints[ 0 ].GetLength();
	float maxLength = projectedPoints[ projectedPoints.size() - 1 ].GetLength();



	for ( int index = 0; index < numPoints; index++ )
	{
		float length = projectedPoints[ index ].GetLength();

		if ( length < minLength )
		{
			minLength = length;
			
		}

		if ( length > maxLength )
		{
			maxLength = length;
		}
	}



	return FloatRange( minLength , maxLength );

}

float DotProduct2D( const Vec2& a, const Vec2& b )
{
	return((a.x*b.x)+(a.y*b.y));
}

float DotProduct3D( const Vec3& a , const Vec3& b )
{
	return( ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z ) );
}

float DotProduct4D( const Vec4& a , const Vec4& b)
{
	return ( ( a.x * b.x ) + ( a.y * b.y) + ( a.z * b.z ) + ( a.w * b.w  ) );
}

