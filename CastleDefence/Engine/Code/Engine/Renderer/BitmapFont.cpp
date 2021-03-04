#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Texture.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"
#define UNUSED(x) (void)(x);




BitmapFont::BitmapFont( const char* fontName, const Texture* fontTexture ):m_fontName(fontName),m_glyphSpriteSheet(*fontTexture,IntVec2(16,16))
{
	
}

const Texture* BitmapFont::GetTexture() const
{
	return &m_glyphSpriteSheet.GetTexture();
	
}

void BitmapFont::AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins, float cellHeight, const std::string& text, const Rgba8& tint, float cellAspect  )
{
	float cellWidth = cellHeight * GetGlyphAspect( 1 ) * cellAspect;
	Vec2 cellMins = textMins;
	Vec2 cellMaxs = cellMins + Vec2( cellWidth , cellHeight );
	Vec2 uvMins;
	Vec2 uvMaxs;


	for ( int index = 0; index < text.size(); index++ )
	{
		AABB2 aabb = AABB2( cellMins , cellMaxs );
		int spriteIndex = ( unsigned char ) text[ index ];
		const SpriteDefinition& character = m_glyphSpriteSheet.GetSpriteDefinition( spriteIndex );
		character.GetUVs( uvMins , uvMaxs );
		AppendAABB2( vertexArray , aabb , tint , uvMins , uvMaxs );

		cellMins += Vec2( cellWidth , 0.f );
		cellMaxs = cellMins + Vec2( cellWidth , cellHeight );
	}

}



void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight, const std::string& text, const Rgba8& tint, float cellAspect=1.f, const Vec2& alignment =ALIGN_CENTERED )
{
	float cellWidth = cellHeight * GetGlyphAspect( 1 ) * cellAspect;
	Vec2 textDimesions = GetDimensionsForText2D( cellHeight , text , cellAspect );
	Vec2 cellMins = box.mins + ( ( box.maxs - box.mins ) * alignment ) - ( textDimesions * alignment );
	Vec2 cellMaxs = ( cellMins + ( Vec2( cellWidth , cellHeight ) /*+ alignment*/ ) );
	Vec2 uvMins;
	Vec2 uvMaxs;
	for ( int charIndex = 0; charIndex < ( int ) text.length(); charIndex++ )
	{
		AABB2 textCell = AABB2( cellMins , cellMaxs );
		int textSpriteIndex = ( unsigned char ) text[ charIndex ];
		const SpriteDefinition& character = m_glyphSpriteSheet.GetSpriteDefinition( textSpriteIndex );
		character.GetUVs( uvMins , uvMaxs );
		AppendAABB2( vertexArray , textCell , tint , uvMins , uvMaxs );
		cellMins += Vec2( cellWidth , 0.f ) /*+ ( box.m_maxs * alignment )*/;
		cellMaxs = cellMins + ( Vec2( cellWidth , cellHeight ) /** alignment*/ );
	}
}

Vec2 BitmapFont::GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect/*=1.f */ )
{
	float cellWidth = cellHeight * GetGlyphAspect( 1 ) * cellAspect;
	float textLength = text.length() * cellWidth;
	return Vec2( textLength, cellHeight );
}

float BitmapFont::GetGlyphAspect( int glyphUnicode ) const
{
	UNUSED( glyphUnicode );
	return 1.f;
}

