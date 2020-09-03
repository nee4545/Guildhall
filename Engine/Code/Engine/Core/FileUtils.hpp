#pragma once
#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

Strings GetFileNamesInFolder( const std::string& folderPath , const char* filePattern );