#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Texture.hpp"
#include <vector>

SpriteSheet::~SpriteSheet()
{
	// delete &m_texture;
}

SpriteSheet::SpriteSheet( const Texture& texture, const IntVec2 SimpleGridLayout ): m_texture(texture)
{
	m_dimensions = SimpleGridLayout;
	Vec2 dimensionOfSprite;
	dimensionOfSprite.x = 1.f/SimpleGridLayout.x;
	dimensionOfSprite.y = 1.f/SimpleGridLayout.y;


	for( int verticalIndex = SimpleGridLayout.y-1; verticalIndex >= 0; verticalIndex-- )
	{
		for( int horizontalIndex = 0; horizontalIndex < SimpleGridLayout.x; horizontalIndex++ )
		{
			int index = (verticalIndex * SimpleGridLayout.x) + horizontalIndex;
			Vec2 uvAtMins = Vec2( (float)horizontalIndex/SimpleGridLayout.x, (float)verticalIndex/SimpleGridLayout.y );
			Vec2 uvAtMaxs = Vec2( (float)(horizontalIndex+1)/SimpleGridLayout.x, (float)(verticalIndex+1)/SimpleGridLayout.y );

			SpriteDefinition temp = SpriteDefinition( *this, index, uvAtMins, uvAtMaxs );
			m_SpriteDefs.push_back( temp );
		}
	}

}


const SpriteDefinition& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
	return m_SpriteDefs[spriteIndex];
}

void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex )
{
	m_SpriteDefs[spriteIndex].GetUVs(out_uvAtMins,out_uvAtMaxs);
}

IntVec2 SpriteSheet::GetDimensions()
{
	return m_dimensions;
}
