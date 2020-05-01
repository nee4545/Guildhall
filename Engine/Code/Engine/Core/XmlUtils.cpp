#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/IntRange.hpp"
#include "Engine/Core/FloatRange.hpp"

std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const std::string& defaultValue )
{
	const char* attributeValueText = element.Attribute(attributeName);
	
	if( attributeValueText )
	{
		return attributeValueText;
	}
	else
	{
		return defaultValue;
	}

}

Vec2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Vec2& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	Vec2 value = defaultValue;
	if( attributeValueText )
	{
		value.SetFromText( attributeValueText );
	}
	return value;
}

int ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, int defaultValue )
{
	std::string valueText = ParseXmlAttribute(element,attributeName,std::to_string(defaultValue));
	if( valueText == std::to_string( defaultValue ) )
	{
		return defaultValue;
	}
	else
	{
		return atoi(valueText.c_str());
	}
}

char ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, char defaultValue )
{
	std::string valueText = ParseXmlAttribute(element,attributeName,std::to_string(defaultValue));
	if( valueText.length()!=1 )
	{
		return defaultValue;
	}
	else
		return valueText[0];
}

bool ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, bool defaultValue )
{
	std::string valueText = ParseXmlAttribute( element , attributeName , "null" );
	if ( valueText == "null" )
	{
		return defaultValue;
	}

	if( valueText.compare("true")==0 )
	{
		return true;
	}
	else if(valueText.compare("false")==0 )
	{
		 return false;
	}
	else
		return defaultValue;
}

float ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, float defaultValue )
{
	std::string valueText = ParseXmlAttribute( element, attributeName, std::to_string( defaultValue ) );
	if( valueText == std::to_string( defaultValue ) )
	{
		return defaultValue;
	}
	else
	{
		return (float)atof( valueText.c_str() );
	}
}

Rgba8 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Rgba8& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );

	if( attributeValueText )
	{
		Rgba8 temp;
		temp.SetFromText(attributeValueText);
		return temp;
	}
	else
	{
		return defaultValue;
	}
}

IntVec2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntVec2& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	IntVec2 value = defaultValue;
	if( attributeValueText )
	{
		value.SetFromText( attributeValueText );
	}
	return value;
}

Strings ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Strings& defaultValues )
{
	std::string attributeValueText = element.Attribute(attributeName);
	Strings toReturn = SplitStringOnDelimiter(attributeValueText,'\n');

	if( toReturn.size()>0 )
	{
		return toReturn;
	}
	else
	{
		return defaultValues;
	}
}

std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const char* defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );

	if( attributeValueText )
	{
		return attributeValueText;
	}
	else
	{
		return defaultValue;
	}
}

IntRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntRange& defaultRange )
{
	const char* attributeValueText = element.Attribute( attributeName );
	IntRange value = defaultRange;
	if( attributeValueText )
	{
		value.SetFromText( attributeValueText );
	}
	return value;
}

FloatRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const FloatRange& defaultRange )
{
	const char* attributeValueText = element.Attribute( attributeName );
	FloatRange value = defaultRange;
	if( attributeValueText )
	{
		value.SetFromText( attributeValueText );
	}
	return value;
}
