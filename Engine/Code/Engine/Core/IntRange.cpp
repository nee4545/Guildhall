#include "Engine/Core/IntRange.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

IntRange::IntRange( int minAndMax )
{
	minimum = minAndMax;
	maximum = minAndMax;
}

IntRange::IntRange( int min, int max )
{
	minimum = min;
	maximum = max;
}

IntRange::IntRange( const char* asText )
{
	std::vector< std::string > splitStrings = SplitStringOnDelimiter( asText, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 2, Stringf( "IntRange can't construct from improper string \"%s\"", asText ) );
	minimum = atoi(splitStrings[0].c_str());
	maximum = atoi(splitStrings[1].c_str());
}

bool IntRange::IsInRange( int value ) const
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

bool IntRange::DoesOverlap( const IntRange& otherRange ) const
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

std::string IntRange::GetAsString() const
{
	std::string toReturn="";
	toReturn.append(std::to_string(minimum));
	toReturn.append(",");
	toReturn.append(std::to_string(maximum));

	return toReturn;
}

int IntRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	int randomNumber = rng.RollRandomIntInRange(minimum,maximum);
	return randomNumber;
}

void IntRange::Set( int newMin, int newMax )
{
	minimum = newMin;
	maximum = newMax;
}

bool IntRange::SetFromText( const char* asText )
{
	std::vector< std::string > splitStrings = SplitStringOnDelimiter( asText, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 2, Stringf( "IntRange can't set from improper string \"%s\"", asText ) );
	minimum = atoi( splitStrings[0].c_str() );
	maximum = atoi( splitStrings[1].c_str() );
	return true;
}
