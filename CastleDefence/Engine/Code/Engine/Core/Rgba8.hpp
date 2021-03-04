#pragma once
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
struct Vec4;

struct Rgba8
{
	unsigned char r=255;
	unsigned char g=255;
	unsigned char b=255;
	unsigned char a=255;

	Rgba8(){}
	~Rgba8(){}

	Rgba8( unsigned	char r, unsigned char g, unsigned char b, unsigned char a=255 )
	{
		this->r=r;
		this->g=g;
		this->b=b;
		this->a=a;
	}


	void SetFromText( const char* asText )
	{
		std::vector< std::string > splitStrings = SplitStringOnDelimiter( asText, ',' );
		if(splitStrings.size() == 4 )
		{ 
			r = (unsigned char)atoi( splitStrings[0].c_str() );
			g = (unsigned char)atoi( splitStrings[1].c_str() );
			b = (unsigned char)atoi( splitStrings[2].c_str() );
			a = (unsigned char)atoi( splitStrings[3].c_str() );
		}
		else if(splitStrings.size() == 3 )
		{
			r = (unsigned char)atoi( splitStrings[0].c_str() );
			g = (unsigned char)atoi( splitStrings[1].c_str() );
			b = (unsigned char)atoi( splitStrings[2].c_str() );
		}
		else
		{
			GUARANTEE_OR_DIE( true, Stringf( "Vec2 can't construct from improper string \"%s\"", asText ) );
		}
	}

	Vec4 GetAsNormalizedVec4();


};