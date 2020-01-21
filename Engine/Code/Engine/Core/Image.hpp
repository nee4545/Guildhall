#pragma once
#include <string>
#include <vector>
#include <map>
#include "Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"

class Image
{
public:
	Image( const char* imageFilePath );
	Image*		       CreateImageFromFile( const char* imageFilePath );
	Image*             GetOrCreateImagefromFile( const char* imageFilePath );
	const std::string& GetImageFilePath() const;
	IntVec2		       GetDimensions() const;
	Rgba8			   GetTexelColor( int texelX, int texelY ) const;
	Rgba8			   GetTexelColor( const IntVec2& texelCoords ) const;
	void               SetTexelColor( int texelX, int texelY,Rgba8 newColor);
	void               SetTexelColor(const IntVec2& texelCoords,Rgba8 newColor);

private:
	std::string			m_imageFilePath;
	IntVec2			m_dimensions = IntVec2( 0, 0 );
	std::vector< Rgba8 >		m_texels;
	static std::map<std::string , Image*> m_LoadedImages;
};
