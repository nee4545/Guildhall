#pragma once
#include "Game/Entity.hpp"
class Texture;
class SpriteAnimDefinition;
class Timer;
class Game;

class Turret : public Entity
{
public:
	Turret(Game* game,Vec2 position,Texture* sprite, Vec2 forwardVec = Vec2(1.f,0.f) );
	~Turret();

	virtual void Update( float deltaseconds );
	virtual void Render();
	virtual void Die();
	virtual void DebugRender();

	Texture* m_sprite = nullptr;
	Vertex_PCU m_vertices[ 6 ];
	Game* m_game = nullptr;

	Vec2 m_forwardVec = Vec2( 1.f , 0.f );
	float m_halfAngle = 35.f;

	bool hasReachedFinal = false;
	bool hasReachedInitial = false;
	float goalDegrees = 0.f;
	
	
};