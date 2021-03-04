#pragma once
#include <string>



enum eBufferFormatType
{
	// add types as you need them - for now, we can get by with just this
	BUFFER_FORMAT_INVALID = -1 ,
	BUFFER_FORMAT_VEC3 = 6 ,
	BUFFER_FORMAT_VEC4 = 2 ,
	BUFFER_FORMAT_VEC2 = 16 ,
	BUFFER_FORMAT_R8G8B8A8_UNORM = 28 ,	
};


struct buffer_attribute_t
{
	std::string name; 			// used to link to a D3D11 shader
	// uint location; 			// used to link to a GL/Vulkan shader 
	eBufferFormatType type;		// what data are we describing
	unsigned int offset; 		// where is it relative to the start of a vertex

	buffer_attribute_t( char const* n , eBufferFormatType t , unsigned int offset );
	buffer_attribute_t();
};