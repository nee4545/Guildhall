#pragma once
#include <map>
#include <string>
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
struct Vec2;
struct Rgba8;
struct Vec3;
class IntVec2;
class NamedStrings
{
private:
	std::map<std::string,std::string> dictionary;

public:
	void			PopulateFromXmlElementAttributes( const tinyxml2::XMLElement& element );
	void			SetValue( const std::string& keyName, const std::string& newValue );

	bool			GetValue( const std::string& keyName, bool defaultValue ) const;
	int			GetValue( const std::string& keyName, int defaultValue ) const;
	float			GetValue( const std::string& keyName, float defaultValue ) const;
	std::string		GetValue( const std::string& keyName, std::string defaultValue ) const;
	std::string		GetValue( const std::string& keyName, const char* defaultValue ) const;
	Rgba8			GetValue( const std::string& keyName, const Rgba8& defaultValue ) const;
	Vec2			GetValue( const std::string& keyName, const Vec2& defaultValue ) const;
	Vec3            GetValue( const std::string keyName , const Vec3& defaultValue ) const;
	IntVec2		GetValue( const std::string& keyName, const IntVec2& defaultValue ) const;


};