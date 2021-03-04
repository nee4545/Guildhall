#pragma once
#include "Engine/Math/Vec2.hpp"

class SpriteSheet;
class Texture;

class SpriteDefinition
{
public:
	~SpriteDefinition(){}
	explicit SpriteDefinition(const SpriteSheet& spriteSheet, int spriteIndex, const Vec2& uvAtMins, const Vec2& uvAtMaxs);
	void GetUVs(Vec2& out_uvAtmins, Vec2& out_uvAtMaxs) const;
	const SpriteSheet& GetSpriteSheet() const;
	const Texture& GetTexture() const;
	float GetAspect() const;


protected:
	const SpriteSheet& m_spriteSheet;
	int m_spriteIndex = -1;
	Vec2 m_uvAtMins = Vec2(0.f,0.f);
	Vec2 m_uvAtMaxs = Vec2(1.f,1.f);
};