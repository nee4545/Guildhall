#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Texture.hpp"






class Game
{
	RandomNumberGenerator rng;
	
public:
	
	Game();

	Texture* tank=nullptr;
	void Update(float deltaseconds);
	void Render();
	void DrawLine(const Vec2 &start,const Vec2 &end, const Rgba8 &color, float thickness);

	void DrawRing( const Vec2 centre, float radius, Rgba8 color, float thickness );

	
	





	
	
};