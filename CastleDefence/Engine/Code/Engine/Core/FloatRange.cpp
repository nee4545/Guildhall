#include "Engine/Core/FloatRange.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

FloatRange::FloatRange( float minAndMax )
{
	minimum = minAndMax;
	maximum = minAndMax;
}

FloatRange::FloatRange( float min, float max )
{
	minimum = min;
	maximum = max;
}

FloatRange::FloatRange( const char* asText )
{
	std::vector< std::string > splitStrings = SplitStringOnDelimiter( asText, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 2, Stringf( "FloatRange can't construct from improper string \"%s\"", asText ) );
	minimum = (float)atof( splitStrings[0].c_str() );
	maximum = (float)atof( splitStrings[1].c_str() );
}

bool FloatRange::IsInRange( float value ) const
{
	if( value>=minimum && value<=maximum )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FloatRange::DoesOverlap( const FloatRange& otherRange ) const
{
	if ( otherRange.minimum >= minimum && otherRange.minimum <= maximum )
	{
		return true;
	}
	else if ( otherRange.maximum >= minimum && otherRange.maximum <= maximum )
	{
		return true;
	}
	return false;
}

std::string FloatRange::GetAsString() const
{
	std::string toReturn="";
	toReturn.append( std::to_string( minimum ) );
	toReturn.append( "," );
	toReturn.append( std::to_string( maximum ) );

	return toReturn;
}

float FloatRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	float randomNumber = rng.RollRandomFloatInRange( minimum, maximum );
	return randomNumber;
}

void FloatRange::Set( float newMin, float newMax )
{
	minimum = newMin;
	maximum = newMax;
}

bool FloatRange::SetFromText( const char* asText )
{
	std::vector< std::string > splitStrings = SplitStringOnDelimiter( asText, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 2, Stringf( "FloatRange can't set from improper string \"%s\"", asText ) );
	minimum = (float)atof( splitStrings[0].c_str() );
	maximum = (float)atof( splitStrings[1].c_str() );
	return true;
}
