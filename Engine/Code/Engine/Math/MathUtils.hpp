#pragma once
#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/OBB2.hpp"
#include "Engine/Core/FloatRange.hpp"
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

FloatRange GetRangeOnProjectedAxis( int numPoints , const Vec2* points , const Vec2& relativePosition , const Vec2& axisNormal );

//Dot Product

float DotProduct2D(const Vec2& a, const Vec2& b);
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




