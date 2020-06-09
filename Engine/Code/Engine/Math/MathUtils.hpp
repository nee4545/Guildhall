#pragma once
#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/OBB2.hpp"
#include "Engine/Core/FloatRange.hpp"
#include "Engine/Core/Polygon2D.hpp"

struct Rgba8;
struct Vec4;

//Angle Utilities
float ConvertDegreesToRadians(float degrees);
float ConvertRadiansToDegrees(float radians);
float CosDegrees(float degrees);
float SinDegrees(float degrees);
float Atan2Degrees(float y,float x);

float GetShortestAngularDisplacement(float fromDegrees, float toDegrees); 
float GetTurnedToward(float currentDegrees, float goalDegrees, float maxDeltaDegrees); 

//Basic 2D & 3D utilities
float GetDistance2D(const Vec2& positionA,const Vec2& positionB);
float GetDistanceSquared2D(const Vec2& positionA,const Vec2& positionB);
float GetDistance3D(const Vec3& positionA,const Vec3& positionB);
float GetDistanceSquared3D(const Vec3& positionA,const Vec3& positionB);
float GetDistanceXY3D(const Vec3& positionA,const Vec3& positionB);
float GetDistanceXYSquared3D(const Vec3& positionA,const Vec3& positionB);
int GetTaxicabDistance2D(const IntVec2& positionA, const IntVec2& positionB);

// overlapping functions
bool DoDiscsOverlap(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB);
bool DoSpheresOverlap(const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB);
bool DoOBBAndOBBOverlap2D( const OBB2& box1 , const OBB2& box2 );
bool DoDiscAndPolygonOverlap( const Vec2& discCenter , float discRadius , const Polygon2D& polygon );
//bool IsDiscInsidePolygon( const Vec2& discCenter , float discRadius , const Polygon2D& polygon );


//SRT functions

const Vec2 TransformPosition2D(const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation );
const Vec3 TransformPosition3DXY(const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXY);
const Vec2 TransformPosition2D(const Vec2& position, const Vec2& iBasis, const Vec2& jBasis,const Vec2& translation );
const Vec3 TransformPosition3DXY(const Vec3& position, const Vec2& iBasisXY, const Vec2& jBasisXY, const Vec2& translationXY);

//Lerp and Clamp

float Interpolate( float a, float b, float fraction0fb );
float RangeMapFloat(float inBegin, float inEnd, float outBegin,float outEnd, float inValue);
float ClampZeroToOne( float value );
float Clamp(float value,float min, float max);
float Round(float value);
int RoundDownToInt(float value);
int RoundToNearestInt(float value);

//Geometric Functions

void PushDiscOutOFAABB2D(Vec2& center,float discRadius,const AABB2& aabb);
void PushDiscOutOfPoint2D(Vec2& center,float discRadius,const Vec2& point);
void PushDIscOutOfEachOther2D(Vec2 &centerDisc1, float discRadius1, Vec2 &centerDisc2, float discRadius2);
void PushMobileDiscOutOfFixedDisc(Vec2 &mobileCenter, float mobileRadius,const Vec2& fixedCenter, float fixedRadius);

bool IsPointInForwardSector2D(const Vec2& point, const Vec2& observerPosition, float forwardDegrees, float apertureDegrees, float maxDistance);
bool IsPointInsideDisc2D(const Vec2& point,const Vec2& discCenter, float discRadius);
bool IsPointInsideOBB2D(const Vec2& point,const OBB2& box);
bool IsPointInsideCapsule2D(const Vec2& point,const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd,float capsuleRadius);

const Vec2 GetNearstPointOnDisc2D(const Vec2& referencePos,const Vec2& discCenter, float discRadius);
const Vec2 GetNearstPointOnOBB2D(const Vec2& refPos, const OBB2& box);
const Vec2 GetNearestPointOnCapsule2D(const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius);
const Vec2 GetNearestPointOnLineSegment2D(const Vec2& refPos, const Vec2& start, const Vec2& end);
const Vec2 GetNearestPointOnInfiniteLine2D(const Vec2& refPos, const Vec2& somePosition, const Vec2& anotherPosition);
Vec2 GetClosestEdgeFromReferencePointOnAABB2( const Vec2& refPos , const AABB2& aabb );
void GetClosestEdgeFromRefrerencePointOnAABB2( const Vec2& refPos , const AABB2& aabb , Vec2& outEdge1 , Vec2& outEdge2 );

Vec3 GetSphericalCoordinates( float theta , float phi , float r );

FloatRange GetRangeOnProjectedAxis( int numPoints , const Vec2* points , const Vec2& relativePosition , const Vec2& axisNormal );

//Dot Product

float DotProduct2D(const Vec2& a, const Vec2& b);
float CrossProduct2D(const Vec2& a, const Vec2& b);
Vec3 CrossProduct3D( Vec3 a , Vec3 b );
float DotProduct3D( const Vec3& a , const Vec3& b);
float DotProduct4D( const Vec4& a , const Vec4& b );
float GetAngleDegreesBetweenVectors2D( const Vec2& vectorA, const Vec2& vectorB );
float GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector );
const Vec2 GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector );

//Smoothsteps

float SmoothStart2( float t );
float SmoothStart3( float t );
float SmoothStart4( float t );
float SmoothStart5( float t );
float SmoothStop2( float t );
float SmoothStop3( float t );
float SmoothStop4( float t );
float SmoothStop5( float t );
float SmoothStep3( float t );

float SignFloat( float val );

Vec2 RotateAroundArbitartPoint( Vec2 arbitaryPoint ,Vec2 point, float rotationDegrees );

float GetMomentOfInertiaOfTriangle( Polygon2D polygon, Vec2 point0 , Vec2 point1 , Vec2 point2, float mass );
float GetAreaOfTriangele( Vec2 point0 , Vec2 point1 , Vec2 point2 );
float GetAreaOfPolygon( Polygon2D polygon );


Vec2 GetSupportPoint( const Vec2* vertices , size_t count , Vec2 direction );
bool DetectPolygonvPolygonIntersections(Polygon2D poly1, Polygon2D poly2, Vec2* outSimplex);
void GetNextSimplex( Vec2& outS1 , Vec2& outS2 , Vec2& outS3, Polygon2D poly1, Polygon2D poly2 );
bool DoesSimplexContainOrigin( Vec2 p1 , Vec2 p2 , Vec2 p3 );
bool IsBothSimplexSame(Vec2 simplex1P1, Vec2 simplex1P2, Vec2 simplex1P3, Vec2 simplex2P1, Vec2 simplex2P2, Vec2 simplex2P3);
Polygon2D GetMinkowskiPolygonIfIntersects( Polygon2D poly1 , Polygon2D poly2 );
void GetContactPoints( Polygon2D minkowskiPoly , Polygon2D poly1 , Polygon2D poly2 , Vec2& cp1 , Vec2 &cp2 );
Vec2 GetDirectionForNextPointInMinkowskiSpace( Polygon2D poly );


