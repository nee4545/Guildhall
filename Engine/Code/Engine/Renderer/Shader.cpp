#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include <stdio.h>
#include <d3dcompiler.h>


void* FileReadToNewBuffer( std::string const& filename, size_t *outSize )
{
	FILE* fp = nullptr;

	fopen_s(&fp, filename.c_str() , "r" );
	if ( fp == nullptr )
	{
		return nullptr;
	}

	fseek( fp , 0 , SEEK_END );
	long file_size = ftell( fp );

	unsigned char* buffer = new unsigned char[ file_size+1 ];
	if ( buffer != nullptr )
	{
		fseek( fp , 0 , SEEK_SET );
		size_t bytes_read=fread( buffer , 1 , (size_t) file_size , fp );
		buffer[ bytes_read ] = NULL;
	}

	if ( outSize != nullptr )
	{
		*outSize = ( size_t ) file_size;
	}

	fclose( fp );

	return buffer;
}


bool Shader::CreateFromFile( std::string const& filename )
{
	size_t file_size = 0;

	void* src = FileReadToNewBuffer( filename,&file_size );
	
	if ( src == nullptr )
	{
		return false;
	}

	return true;
}
