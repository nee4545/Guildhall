#pragma once
#include <string>
#include <vector>
#include "Engine/ThirdParty/tinyxml2.h"
typedef std::vector< std::string >	Strings; 
struct Rgba8;
class IntVec2;
struct Vec2;
struct IntRange;
struct FloatRange;

int			ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, int defaultValue );
char		ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, char defaultValue );
bool		ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, bool defaultValue );
float		ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, float defaultValue );
Rgba8		ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Rgba8& defaultValue );
Vec2		ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Vec2& defaultValue );
IntVec2		ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntVec2& defaultValue );
std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const std::string& defaultValue );
Strings		ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Strings& defaultValues );
IntRange	ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntRange& defaultRange);
FloatRange  ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const FloatRange& defaultRange);

std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const char* defaultValue );
