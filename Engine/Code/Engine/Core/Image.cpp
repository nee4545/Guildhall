#pragma warning( push )
#pragma warning ( disable: 4100 )
#define STB_IMAGE_IMPLEMENTATION
#include "Engine/ThirdParty/stb_image.h"
#pragma warning( pop )
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

std::map<std::string , Image*> Image::m_LoadedImages;

Image::Image( const char* imageFilePath )
{
	m_imageFilePath = imageFilePath;
}

Image* Image::CreateImageFromFile( const char* imageFilePath )
{
	//unsigned int imageID = 0;
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );
	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );
	GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );
	if(  numComponents == 4 )
	{
		int totalPixels = imageTexelSizeY * imageTexelSizeX;
		for( int pixelIndex = 0; pixelIndex < totalPixels; pixelIndex++ )
		{
			Rgba8 temp;
			temp.r = *imageData;
			imageData++;
			temp.g = *imageData;
			imageData++;
			temp.b = *imageData;
			imageData++;
			m_texels.push_back( temp );
		}
	}
	else if(  numComponents ==3 )
	{
		int totalPixels = imageTexelSizeY * imageTexelSizeX;
		for( int pixelIndex = 0; pixelIndex < totalPixels; pixelIndex++ )
		{
			Rgba8 temp;
			temp.r = *imageData;
			imageData++;
			temp.g = *imageData;
			imageData++;
			temp.b = *imageData;
			imageData++;
			temp.a = *imageData;
			imageData++;
			m_texels.push_back( temp );
		}
	}
	m_dimensions = IntVec2( imageTexelSizeX, imageTexelSizeY );
	m_LoadedImages[imageFilePath]= this;
	return this;
}


Image* Image::GetOrCreateImagefromFile( const char* imageFilePath )
{
	Image* temp = m_LoadedImages[imageFilePath];
	if( temp == nullptr )
	{
		temp = CreateImageFromFile( imageFilePath );
	}
	return temp;
}

const std::string& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

Rgba8 Image::GetTexelColor( int texelX, int texelY ) const
{
	int		texelColorIndex = (texelY * m_dimensions.x) + texelX;
	return	m_texels[texelColorIndex];
}

Rgba8 Image::GetTexelColor( const IntVec2& texelCoords ) const
{
	int		texelColorIndex = (texelCoords.y * m_dimensions.x) + texelCoords.x;
	return  m_texels[texelColorIndex];
}

void Image::SetTexelColor( int texelX, int texelY, Rgba8 newColor )
{
	int		texelColorStartIndex = (texelY * m_dimensions.x) + texelX;
	m_texels[texelColorStartIndex] = newColor;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void Image::SetTexelColor( const IntVec2& texelCoords, Rgba8 newColor )
{
	int		texelColorStartIndex = (texelCoords.y * m_dimensions.x) + texelCoords.x;
	m_texels[texelColorStartIndex] = newColor;
}


