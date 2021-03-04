#include "MathUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Polygon2D.hpp"
#include <cmath>
#include <vector>
#include "../Core/Rgba8.hpp"
#include "../Renderer/DebugRender.hpp"
#include "Engine/Core/Plane.hpp"
#define UNUSED(x) (void)(x);


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


Vec2 RotateAroundArbitartPoint( Vec2 arbitaryPoint , Vec2 point , float rotationDegrees )
{
	Vec2 vec = point - arbitaryPoint;
	vec.RotateDegrees( rotationDegrees );
	vec += arbitaryPoint;
	return vec;
}

float GetMomentOfInertiaOfTriangle( Polygon2D polygon , Vec2 point0 , Vec2 point1 , Vec2 point2, float mass )
{
	UNUSED( mass );
	Vec2 u = point1 - point0;
	Vec2 v = point2 - point0;
	float area = GetAreaOfTriangele( point0 , point1 , point2 );
	Vec2 center = ( ( point1 + point2 + point0 ) / 3.f ) - polygon.GetCentre();
	float b = u.GetLengthSquared();																		
	float a = DotProduct2D( v , u );
	Vec2 h = ( v - ( ( DotProduct2D( v , u ) / DotProduct2D( u , u ) ) * u ) );
	float inertia = ( b - a + ( ( a * a ) / b ) + ( DotProduct2D( h , h ) ) );
	inertia *= area / 18.f;
	inertia += area * center.GetLengthSquared();
	return inertia;
}

float GetAreaOfTriangele( Vec2 point0 , Vec2 point1 , Vec2 point2 )
{
	Vec2 u = point1 - point0;
	Vec2 v = point2 - point0;

	float projectedDistance = GetProjectedLength2D( v , u );

	Vec2 uNormal = u.GetNormalized();
	Vec2 point = point0 + uNormal * projectedDistance;

	float height = ( point2 - point ).GetLength();
	float b = u.GetLength();

	return 0.5f * height * b;
}

float GetAreaOfPolygon( Polygon2D polygon )
{
	float area = 0;

	Vec2 commonVert = polygon.m_points[ 0 ];

	int totalTriangles = ( int ) polygon.m_points.size() - 2;
	int counter = 0;
	int i = 1;
	int j = 2;

	while ( counter < totalTriangles )
	{
		
		Vec2 nextVert1 = polygon.m_points[ i ];
		Vec2 nextVert2 = polygon.m_points[ j ];

		area += GetAreaOfTriangele( commonVert , nextVert1 , nextVert2 );

		i++;
		j++;
		counter++;
	}

	return area;
}

Vec2 GetSupportPoint( const Vec2* vertices , size_t count , Vec2 direction )
{
	float maxProduct = DotProduct2D( direction , vertices[ 0 ] );
	int index = 0;

	for ( int i = 1; i < count; i++ )
	{
		float product = DotProduct2D( direction , vertices[ i ] );
		if ( product > maxProduct )
		{
			maxProduct = product;
			index = i;
		}
	}
	return vertices[index];
}

bool DetectPolygonvPolygonIntersections( Polygon2D poly1 , Polygon2D poly2, Vec2* outSimplex )
{
	Vec2 dir1 = Vec2( 0.f , 1.f );
	Vec2 dir2 = Vec2( 1.f , 0.f );

	Vec2 sp1 = GetSupportPoint( &poly1.m_points[ 0 ] , poly1.m_points.size() , dir1 ) - GetSupportPoint( &poly2.m_points[ 0 ] , poly2.m_points.size() , -dir1 );
	Vec2 sp2 = GetSupportPoint( &poly1.m_points[ 0 ] , poly1.m_points.size() , dir2 ) - GetSupportPoint( &poly2.m_points[ 0 ] , poly2.m_points.size() , -dir2 );

	Vec2 directionTowardsOrigin = -sp1.GetNormalized();

	Vec2 sp3 = GetSupportPoint( &poly1.m_points[ 0 ] , poly1.m_points.size() , directionTowardsOrigin ) - GetSupportPoint( &poly2.m_points[ 0 ] , poly2.m_points.size() , -directionTowardsOrigin );

	bool simplexFound = false;

	Vec2 currentSp1 = sp1;
	Vec2 currentSp2 = sp2;
	Vec2 currentSp3 = sp3;

	if ( DoesSimplexContainOrigin( sp1 , sp2 , sp3 ) )
	{
		outSimplex[ 0 ] = sp1;
		outSimplex[ 1 ] = sp2;
		outSimplex[ 3 ] = sp3;
		simplexFound = true;
		return true;
	}

	GetNextSimplex( sp1 , sp2 , sp3 , poly1 , poly2 );

	while ( !IsBothSimplexSame(sp1,sp2,sp3,currentSp1,currentSp2,currentSp3) )
	{
		if ( DoesSimplexContainOrigin( sp1 , sp2 , sp3 ) )
		{
			simplexFound = true;
			outSimplex[ 0 ] = sp1;
			outSimplex[ 1 ] = sp2;
			outSimplex[ 3 ] = sp3;
			return true;
		}

		currentSp1 = sp1;
		currentSp2 = sp2;
		currentSp3 = sp3;

		GetNextSimplex( sp1 , sp2 , sp3 , poly1 , poly2 );
	}

	return false;
}


void GetNextSimplex( Vec2& outS1 , Vec2& outS2 , Vec2& outS3, Polygon2D poly1 , Polygon2D poly2 )
{
	Vec2 pointOnEdgeS1S2 = GetNearestPointOnLineSegment2D( Vec2( 0.f , 0.f ) , outS1 , outS2 );
	Vec2 pointOnEdgeS1S3 = GetNearestPointOnLineSegment2D( Vec2( 0.f , 0.f ) , outS1 , outS3 );
	Vec2 pointOnEdgeS2S3 = GetNearestPointOnLineSegment2D( Vec2( 0.f , 0.f ) , outS2 , outS3 );
	Vec2 minPoint;
	Vec2 s1;
	Vec2 s2;
	Vec2 s3;

	float minLength = pointOnEdgeS1S2.GetLength();
	s1 = outS1;
	s2 = outS2;
	minPoint = pointOnEdgeS1S2;

	if ( pointOnEdgeS1S3.GetLength() < minLength )
	{
		minLength = pointOnEdgeS1S3.GetLength();
		minPoint = pointOnEdgeS1S3;
		s1 = outS1;
		s2 = outS3;
	}

	if ( pointOnEdgeS2S3.GetLength() < minLength )
	{
		minLength = pointOnEdgeS2S3.GetLength();
		minPoint = pointOnEdgeS2S3;
		s1 = outS2;
		s2 = outS3;
	}

	Vec2 dir = -minPoint.GetNormalized();
	s3 = GetSupportPoint( &poly1.m_points[ 0 ] , poly1.m_points.size() , dir ) - GetSupportPoint( &poly2.m_points[ 0 ] , poly2.m_points.size() , -dir );

	outS1 = s1;
	outS2 = s2;
	outS3 = s3;

}

bool DoesSimplexContainOrigin( Vec2 p1 , Vec2 p2 , Vec2 p3 )
{
	Polygon2D temp = Polygon2D();

	if ( CrossProduct2D( p2 - p1 , p3 - p1 ) > 0 )
	{
		temp.m_points.push_back( p1 );
		temp.m_points.push_back( p2 );
		temp.m_points.push_back( p3 );
	}
	else if ( CrossProduct2D( p3 - p1 , p2 - p1 ) > 0 )
	{
		temp.m_points.push_back( p1 );
		temp.m_points.push_back( p3 );
		temp.m_points.push_back( p2 );
	}
	else
	{
		temp.m_points.push_back( p2 );
		temp.m_points.push_back( p3 );
		temp.m_points.push_back( p1 );
	}

	return temp.Contains( Vec2( 0.f , 0.f ) );
}

bool IsBothSimplexSame( Vec2 simplex1P1 , Vec2 simplex1P2 , Vec2 simplex1P3 , Vec2 simplex2P1 , Vec2 simplex2P2 , Vec2 simplex2P3 )
{
	//6 possible combinations
	float nearZero = 0.000001f;

	if ( ( simplex1P1 - simplex2P1 ).GetLength() <= nearZero && ( simplex1P2 - simplex2P2 ).GetLength() <= nearZero && ( simplex1P3 - simplex2P3 ).GetLength() <= nearZero )
	{
		return true;
	}

	if ( ( simplex1P1 - simplex2P1 ).GetLength() <= nearZero && ( simplex1P2 - simplex2P3 ).GetLength() <= nearZero && ( simplex1P3 - simplex2P2 ).GetLength() <= nearZero )
	{
		return true;
	}

	if ( ( simplex1P1 - simplex2P2 ).GetLength() <= nearZero && ( simplex1P2 - simplex2P1 ).GetLength() <= nearZero && ( simplex1P3 - simplex2P3 ).GetLength() <= nearZero )
	{
		return true;
	}

	if ( ( simplex1P1 - simplex2P2 ).GetLength() <= nearZero && ( simplex1P2 - simplex2P3 ).GetLength() <= nearZero && ( simplex1P3 - simplex2P1 ).GetLength() <= nearZero )
	{
		return true;
	}

	if ( ( simplex1P1 - simplex2P3 ).GetLength() <= nearZero && ( simplex1P2 - simplex2P2 ).GetLength() <= nearZero && ( simplex1P3 - simplex2P1 ).GetLength() <= nearZero )
	{
		return true;
	}

	if ( ( simplex1P1 - simplex2P3 ).GetLength() <= nearZero && ( simplex1P2 - simplex2P1 ).GetLength() <= nearZero && ( simplex1P3 - simplex2P2 ).GetLength() <= nearZero )
	{
		return true;
	}

	return false;
	
}

Polygon2D GetMinkowskiPolygonIfIntersects( Polygon2D poly1 , Polygon2D poly2 )
{
	Polygon2D toReturn;

	Vec2 simplex[ 3 ];

	DetectPolygonvPolygonIntersections( poly1 , poly2 , simplex );

	std::vector<Vec2> finalListOfPoints;
	finalListOfPoints.push_back( simplex[ 0 ] );
	finalListOfPoints.push_back( simplex[ 1 ] );
	finalListOfPoints.push_back( simplex[ 2 ] );

	toReturn = Polygon2D::MakeConvexFromPointCloud( &finalListOfPoints[0] , 3 );

	int index1;
	int index2;

	Vec2 nearestPoint = toReturn.GetClosestPointOnEdgeAndIndicesOfTheEdge( Vec2( 0.f , 0.f ),index1,index2 );
	Vec2 dir = nearestPoint.GetNormalized();

	Plane2 currentPlane = Plane2( dir , nearestPoint );

	const float nearZero = 0.0001f;
					
	Vec2 supportPoint =GetSupportPoint( &poly1.m_points[ 0 ] , poly1.m_points.size() , dir ) - GetSupportPoint( &poly2.m_points[ 0 ] , poly2.m_points.size() , -dir );
	
	if (currentPlane.GetSignedDistanceFromPlane( supportPoint ) <= nearZero )
	{
		return toReturn;
	}

	finalListOfPoints.push_back( supportPoint );
	toReturn = Polygon2D::MakeConvexFromPointCloud( &finalListOfPoints[ 0 ] , (int)finalListOfPoints.size() );

	
	nearestPoint = toReturn.GetClosestPointOnEdgeAndIndicesOfTheEdge( Vec2( 0.f , 0.f ) , index1 , index2 );
	dir = nearestPoint.GetNormalized();

	Plane2 nextPlane= Plane2(dir,nearestPoint);

	supportPoint = GetSupportPoint( &poly1.m_points[ 0 ] , poly1.m_points.size() , dir ) - GetSupportPoint( &poly2.m_points[ 0 ] , poly2.m_points.size() , -dir );

	if ( nextPlane.GetSignedDistanceFromPlane( supportPoint ) <= nearZero )
	{
		return toReturn;
	}

	finalListOfPoints.push_back( supportPoint );
	toReturn = Polygon2D::MakeConvexFromPointCloud( &finalListOfPoints[ 0 ] , ( int ) finalListOfPoints.size() );

	while ( nextPlane != currentPlane )
	{
		currentPlane = nextPlane;

		nearestPoint = toReturn.GetClosestPointOnEdgeAndIndicesOfTheEdge( Vec2( 0.f , 0.f ) , index1 , index2 );
		dir = nearestPoint.GetNormalized();

		nextPlane = Plane2( dir , nearestPoint );

		supportPoint = GetSupportPoint( &poly1.m_points[ 0 ] , poly1.m_points.size() , dir ) - GetSupportPoint( &poly2.m_points[ 0 ] , poly2.m_points.size() , -dir );

		if ( nextPlane.GetSignedDistanceFromPlane( supportPoint ) <= nearZero )
		{
			return toReturn;
		}

		finalListOfPoints.push_back( supportPoint );
		toReturn = Polygon2D::MakeConvexFromPointCloud( &finalListOfPoints[ 0 ] , ( int ) finalListOfPoints.size() );
	}

	return toReturn;
}

void GetContactPoints( Polygon2D minkowskiPoly , Polygon2D poly1 , Polygon2D poly2 , Vec2& cp1 , Vec2& cp2 )
{
	Vec2 nearestPoint = minkowskiPoly.GetClosestPointOnTheEdges( Vec2(0.f,0.f) );

	Vec2 normal = -nearestPoint.GetNormalized();

	Vec2 supportPoint = GetSupportPoint( &poly2.m_points[ 0 ] , poly2.m_points.size() , normal );

	Plane2 referencePlane = Plane2( normal , supportPoint );

	const float eps = 0.001f;

	struct Temp
	{
		Vec2 point;
		float distace;
	};
	
	std::vector<Temp> pointsAndDistance;

	for ( int i = 0; i < poly2.m_points.size(); i++ )
	{
		float distaceFromPlane = referencePlane.GetSignedDistanceFromPlane( poly2.m_points[ i ] );

		if ( abs( distaceFromPlane ) <= eps )
		{
			Temp temp;
			temp.point = poly2.m_points[ i ];
			temp.distace = distaceFromPlane;

			pointsAndDistance.push_back( temp );
		}
	}

	if ( pointsAndDistance.size() == 1 )
	{
		cp1 = pointsAndDistance[ 0 ].point;
		cp2 = pointsAndDistance[ 0 ].point;
		return;
	}

	if ( pointsAndDistance.size() == 2 )
	{
		cp1 = pointsAndDistance[ 0 ].point;
		cp2 = pointsAndDistance[ 1 ].point;
		return;
	}

	Vec2 tangent = normal.GetRotatedMinus90Degrees();

	for ( int i = 0; i < pointsAndDistance.size(); i++ )
	{
		pointsAndDistance[ i ].distace = DotProduct2D( tangent , pointsAndDistance[ i ].point );
	}

	//Get min and max in this range
	Vec2 minPoint = pointsAndDistance[ 0 ].point;
	Vec2 maxPoint = pointsAndDistance[ 1 ].point;
	float mindistance = INFINITY;
	float maxDistance = pointsAndDistance[ 0 ].distace;

	for ( int i = 0; i < pointsAndDistance.size(); i++ )
	{
		if ( pointsAndDistance[ i ].distace < mindistance )
		{
			mindistance = pointsAndDistance[ i ].distace;
			minPoint = pointsAndDistance[ i ].point;
		}

		if ( pointsAndDistance[ i ].distace > maxDistance )
		{
			maxDistance = pointsAndDistance[ i ].distace;
			maxPoint = pointsAndDistance[ i ].point;
		}
	}
	

	cp1 = minPoint;
	cp2 = maxPoint;
	
}

Vec2 GetDirectionForNextPointInMinkowskiSpace( Polygon2D poly )
{
	Vec2 nearestPoint =poly.GetClosestPointOnTheEdges( Vec2( 0.f , 0.f ) );

	return -nearestPoint.GetNormalized();
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

void GetClosestEdgeFromRefrerencePointOnAABB2( const Vec2& refPos , const AABB2& aabb , Vec2& outEdge1 , Vec2& outEdge2 )
{
	if ( refPos.x <= aabb.mins.x )
	{
		if ( refPos.y >= aabb.mins.y && refPos.y <= aabb.maxs.y )
		{
			outEdge1 = aabb.mins;
			outEdge2 = Vec2( aabb.mins.x , aabb.maxs.y );
		}
		else if ( refPos.y < aabb.mins.y )
		{
			outEdge1 = aabb.mins;
			outEdge2 = Vec2( aabb.maxs.x , aabb.mins.y );
		}
		else
		{
			outEdge1 = Vec2( aabb.mins.x , aabb.maxs.y );
			outEdge2 = aabb.maxs;
		}

	}

	if ( refPos.x >= aabb.maxs.x )
	{
		if ( refPos.y >= aabb.mins.y && refPos.y <= aabb.maxs.y )
		{
			outEdge1 = Vec2(aabb.maxs.x,aabb.mins.y);
			outEdge2 = aabb.maxs;
		}
		else if ( refPos.y < aabb.mins.y )
		{
			outEdge1 = aabb.mins;
			outEdge2 = Vec2( aabb.maxs.x , aabb.mins.y );
		}
		else
		{
			outEdge1 = Vec2( aabb.mins.x , aabb.maxs.y );
			outEdge2 = aabb.maxs;
		}
	}

	if ( refPos.x >= aabb.mins.x && refPos.x <= aabb.maxs.x )
	{
		if ( refPos.y <= aabb.mins.y )
		{
			outEdge1 = aabb.mins;
			outEdge2 = Vec2( aabb.maxs.x , aabb.mins.y );
		}
		else
		{
			outEdge1 = Vec2( aabb.mins.x , aabb.maxs.y );
			outEdge2 = aabb.maxs;
		}
	}

}

Vec3 GetSphericalCoordinates( float theta , float phi , float r )
{
	Vec3 toReturn;
	toReturn.x = CosDegrees( theta ) * CosDegrees( phi ) * r;
	toReturn.y = SinDegrees( phi )*r;
	toReturn.z = SinDegrees( theta ) * CosDegrees( phi ) * r;

	return toReturn;
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

float CrossProduct2D( const Vec2& a , const Vec2& b )
{
	return ( ( a.x * b.y ) - ( a.y * b.x ) );
}

Vec3 CrossProduct3D( Vec3 a , Vec3 b )
{
	Vec3 c;
	c.x = ( a.y * b.z ) - ( a.z * b.y );
	c.y = ( a.z * b.x ) - ( a.x * b.z );
	c.z = ( a.x * b.y ) - ( a.y * b.x );

	return c;
}

float DotProduct3D( const Vec3& a , const Vec3& b )
{
	return( ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z ) );
}

float DotProduct4D( const Vec4& a , const Vec4& b)
{
	return ( ( a.x * b.x ) + ( a.y * b.y) + ( a.z * b.z ) + ( a.w * b.w  ) );
}

