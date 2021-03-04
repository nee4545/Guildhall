#pragma once

struct Vec2;

struct Vec3
{
	float x=0;
	float y=0;
	float z=0;

	Vec3(){};
	~Vec3(){};
	Vec3(const Vec2& vec , float zValue );
	Vec3( const Vec3& copyFrom );							// copy constructor (from another vec2)
	explicit Vec3( float initialX, float initialY,float initialZ );		// explicit constructor (from x, y)

															// Operators (const)
	bool		operator==( const Vec3& compare ) const;		// vec2 == vec2
	bool		operator!=( const Vec3& compare ) const;		// vec2 != vec2
	const Vec3	operator+( const Vec3& vecToAdd ) const;		// vec2 + vec2
	const Vec3	operator-( const Vec3& vecToSubtract ) const;	// vec2 - vec2
	const Vec3	operator-() const;								// -vec2, i.e. "unary negation"
	const Vec3	operator*( float uniformScale ) const;			// vec2 * float
	const Vec3	operator*( const Vec3& vecToMultiply ) const;	// vec2 * vec2
	const Vec3	operator/( float inverseScale ) const;			// vec2 / float

																// Operators (self-mutating / non-const)
	void		operator+=( const Vec3& vecToAdd );				// vec2 += vec2
	void		operator-=( const Vec3& vecToSubtract );		// vec2 -= vec2
	void		operator*=( const float uniformScale );			// vec2 *= float
	void		operator/=( const float uniformDivisor );		// vec2 /= float
	void		operator=( const Vec3& copyFrom );				// vec2 = vec2

																// Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
	friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * vec2

	//Accessors

	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	float GetAngleAboutZRadians() const;
	float GetAngleAboutZDegrees() const;
	const Vec3 GetRotatedAboutZRadians(float deltaRadians) const;
	const Vec3 GetRotatedAboutZDegrees(float deltaDegrees) const;

	const Vec3 GetClamped(float maxLength) const;

	const Vec3 GetNormalized() const;

	void SetFromText(const char* asText);

};