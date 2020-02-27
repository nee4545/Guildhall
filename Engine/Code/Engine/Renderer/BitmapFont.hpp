#pragma once
#include <vector>
#include <string>
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/AABB2.hpp"


class Texture;
struct Vec2;
struct Vertex_PCU;
struct Rgba8;
//------------------------------------------------------------------------------------------------
class BitmapFont
{
	friend class RenderContext; // Only the RenderContext can create new BitmapFont objects!

private:
	 BitmapFont( const char* fontName, const Texture* fontTexture );

public:
	

	const Texture* GetTexture() const;

	void AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins,
		float cellHeight, const std::string& text, const Rgba8& tint, float cellAspect=1.f );

	void AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight,
		const std::string& text, const Rgba8& tint, float cellAspect,
		const Vec2& alignment);

	Vec2 GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect=1.f );


protected:
	float GetGlyphAspect( int glyphUnicode ) const;

protected:
	std::string	m_fontName;
	SpriteSheet	m_glyphSpriteSheet;
};
