#pragma once
#include <Engine\Math\Vec2.hpp>

class IntVec2;

struct AABB2
{
public:
	Vec2 mins;
	Vec2 maxs;

public:
	AABB2()=default;
	~AABB2(){};
	AABB2(const AABB2& copyFrom);
	explicit AABB2(const Vec2& mins, const Vec2& maxs);
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2( int minX , int minY , int maxX , int maxY );


	bool IsPointInside(const Vec2& point)const;
	const Vec2 GetCenter()const;
	const Vec2 GetDimensions()const;
	const Vec2 GetNearestPoint(const Vec2& referencePosition) const;
	const Vec2 GetPointAtUV(const Vec2& uvCoorsZeroToOne) const;
	const Vec2 GetUVForPoint(const Vec2& point) const;
	float GetOuterRadius() const;
	float GetInnerRadius() const;
	void GetCornerPositions( Vec2* outFourPoints ) const;
	AABB2 GetBoxAtLeft( float fractionOfWidth , float additionalWidth = 0.f ) const;
	AABB2 GetBoxAtRight( float fractionOfWidth , float additionalWidth = 0.f ) const;
	AABB2 GetBoxAtBottom( float fractionOfHeight , float additionalHeight = 0.f ) const;
	AABB2 GetBoxAtTop( float fractionOfHeight , float additionalHeight = 0.f ) const;


	void Translate(const Vec2& translation);
	void SetCenter(const Vec2& newCenter);
	void SetDimension(const Vec2& newDimensions);
	void StretchToIncludePoint(const Vec2& point);
	void CarveBoxOffLeft( float fractionOfWidth , float additionalWidth = 0.f );
	void CarveBoxOffRight( float fractionOfWidth , float additionalWidth = 0.f );
	void CarveBoxOffTop( float fractionOfHeight , float additionalHeight = 0.f );
	void CarveBoxOffBottom( float fractionOfHeight , float additionalHeight = 0.f );
	AABB2 GetPercentageOfAABB( float percentage );




	bool operator==(const AABB2& compareWith)const;
	void operator=(const AABB2& assignFrom);
};