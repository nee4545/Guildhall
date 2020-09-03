#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <io.h>

Strings GetFileNamesInFolder( const std::string& folderPath , const char* filePattern )
{
	Strings fileNamesInFolder;

#ifdef _WIN32
	std::string fileNamePattern = filePattern ? filePattern : "*";
	std::string filePath = folderPath + "/" + fileNamePattern;
	_finddata_t fileInfo;
	intptr_t searchHandle = _findfirst( filePath.c_str() , &fileInfo );

	while ( searchHandle != -1 )
	{
		fileNamesInFolder.push_back( fileInfo.name );
		int errorCode = _findnext( searchHandle , &fileInfo );
		if ( errorCode != 0 )
		{
			break;
		}
	}
#else
	ERROR_AND_DIE( "NOT yet implemented for platform" );
#endif
	return fileNamesInFolder;
}
