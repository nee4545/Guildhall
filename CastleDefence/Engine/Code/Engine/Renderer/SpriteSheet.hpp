#pragma once
#include <vector>
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"

class Texture;
class SpriteDefinition;
struct Vec2;

class SpriteSheet
{
public:

	~SpriteSheet();

	explicit SpriteSheet(const Texture& texture, const IntVec2 SimpleGrigLayout);
	const Texture& GetTexture() const { return m_texture;}
	int GetNumSprites() const {return (int)m_SpriteDefs.size(); }
	const SpriteDefinition& GetSpriteDefinition(int spriteIndex) const;
	void GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex );
	IntVec2 GetDimensions();
protected:
	const Texture& m_texture;
	std::vector<SpriteDefinition> m_SpriteDefs;
	IntVec2 m_dimensions;

};