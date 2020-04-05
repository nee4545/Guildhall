#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include <stdexcept>


void NamedStrings::PopulateFromXmlElementAttributes( const tinyxml2::XMLElement& element )
{
	std::string name;
	std:: string value;
	const tinyxml2::XMLAttribute* attrib=element.FirstAttribute();

	for( attrib; attrib; attrib=attrib->Next() )
	{
		name = attrib->Name();
		value = attrib->Value();
		dictionary.emplace(name,value);
	}

}

void NamedStrings::SetValue( const std::string& keyName, const std::string& newValue )
{
	dictionary[keyName]= newValue;
}

bool NamedStrings::GetValue( const std::string& keyName, bool defaultValue ) const
{
	 if( dictionary.find( keyName )!=dictionary.end() )
	 {
		std::string toReturn=dictionary.at(keyName);
		if( toReturn.compare( "true" )==0 )
		{
			return true;
		}
		else if(toReturn.compare("false")==0 )
		{
			return false;
		}
		else
		{
			return defaultValue;
		}
	 }
	 else
		 return defaultValue;
	 
}

int NamedStrings::GetValue( const std::string& keyName, int defaultValue ) const
{
	if( dictionary.find( keyName )!=dictionary.end() )
	{
		std::string toReturn=dictionary.at( keyName );
		return atoi(toReturn.c_str());
	}
	else
		return defaultValue;
}

float NamedStrings::GetValue( const std::string& keyName, float defaultValue ) const
{
	if( dictionary.find( keyName )!=dictionary.end() )
	{
		std::string toReturn=dictionary.at( keyName );
		return (float)atof( toReturn.c_str() );
	}
	else
		return defaultValue;
}

Vec2 NamedStrings::GetValue( const std::string& keyName, const Vec2& defaultValue ) const
{
	if( dictionary.find( keyName )!=dictionary.end() )
	{
		std::string toReturn=dictionary.at( keyName );
		Vec2 temp;
		temp.SetFromText( toReturn.c_str() );
		return temp;
	}
	else
		return defaultValue;
}

IntVec2 NamedStrings::GetValue( const std::string& keyName, const IntVec2& defaultValue ) const
{
	if( dictionary.find( keyName )!=dictionary.end() )
	{
		std::string toReturn=dictionary.at( keyName );
		IntVec2 temp;
		temp.SetFromText( toReturn.c_str() );
		return temp;
	}
	else
		return defaultValue;
}

Rgba8 NamedStrings::GetValue( const std::string& keyName, const Rgba8& defaultValue ) const
{
	if( dictionary.find( keyName )!=dictionary.end() )
	{
		std::string toReturn=dictionary.at( keyName );
		Rgba8 temp;
		temp.SetFromText( toReturn.c_str() );
		return temp;
	}
	else
		return defaultValue;
}

std::string NamedStrings::GetValue( const std::string& keyName, std::string defaultValue ) const
{
	if( dictionary.find( keyName )!=dictionary.end() )
	{
		std::string toReturn=dictionary.at( keyName );
		return toReturn;
	}
	else
		return defaultValue;
}

std::string NamedStrings::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	if( dictionary.find( keyName )!=dictionary.end() )
	{
		std::string toReturn=dictionary.at( keyName );
		return toReturn;
	}
	else
		return defaultValue;
}

Vec3 NamedStrings::GetValue( const std::string keyName , const Vec3& defaultValue ) const
{
	if ( dictionary.find( keyName ) != dictionary.end() )
	{
		std::string toReturn = dictionary.at( keyName );
		Vec3 temp;
		temp.SetFromText( toReturn.c_str() );
		return temp;
	}
	else
		return defaultValue;
}
