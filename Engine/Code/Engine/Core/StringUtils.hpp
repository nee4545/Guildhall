#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

typedef std::vector< std::string >		Strings;

Strings SplitStringOnDelimiter( const std::string& originalString, char delimiterToSplitOn );
//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );




