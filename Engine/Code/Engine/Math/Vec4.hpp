#pragma once

struct Vec4
{
	float x=0.f;
	float y=0.f;
	float z=0.f;
	float w=0.f;

	Vec4(){};
	Vec4( float x, float y, float z, float w )
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	bool		operator==( const Vec4& compare ) const;		// vec2 == vec2
	bool		operator!=( const Vec4& compare ) const;		// vec2 != vec2
	const Vec4	operator+( const Vec4& vecToAdd ) const;		// vec2 + vec2
	const Vec4	operator-( const Vec4& vecToSubtract ) const;	// vec2 - vec2
	const Vec4	operator-() const;								// -vec2, i.e. "unary negation"
	const Vec4	operator*( float uniformScale ) const;			// vec2 * float
	const Vec4	operator*( const Vec4& vecToMultiply ) const;	// vec2 * vec2
	const Vec4	operator/( float inverseScale ) const;			// vec2 / float

																// Operators (self-mutating / non-const)
	void		operator+=( const Vec4& vecToAdd );				// vec2 += vec2
	void		operator-=( const Vec4& vecToSubtract );		// vec2 -= vec2
	void		operator*=( const float uniformScale );			// vec2 *= float
	void		operator/=( const float uniformDivisor );		// vec2 /= float
	void		operator=( const Vec4& copyFrom );
};