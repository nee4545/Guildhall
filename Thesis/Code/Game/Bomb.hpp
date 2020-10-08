#pragma once
#include "Game/Entity.hpp"

class Texture;
class SpriteAnimDefinition;
class Timer;
class Game;

class Bomb : public Entity
{
public:
	Bomb(Game* game,Vec2 position,SpriteAnimDefinition* idle, SpriteAnimDefinition* explode);
	~Bomb();

	virtual void Update( float deltaseconds );
	virtual void Render();
	virtual void Die();
	virtual void DebugRender();
	void Explode();
	void ResetVertices();

	SpriteAnimDefinition* m_Idlesprite = nullptr;
	SpriteAnimDefinition* m_explosion = nullptr;
	Vertex_PCU m_vertices[ 6 ];
	float m_time = 0.f;
	Timer* m_explosionTimer = nullptr;
	Game* m_game;

	bool m_isExploding = false;
	float m_explosionRadius = 4.f;
	float m_triggerRadius = 2.f;

	Vec2 m_minUV;
	Vec2 m_maxUV;
	
};