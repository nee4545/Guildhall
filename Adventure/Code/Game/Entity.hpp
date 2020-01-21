#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class Entity
{
public:
	Vec2 m_position;
	Vec2 m_velocity;

	float m_orientationDegrees;
	float m_angularVelocity;
	float m_physicsRadius;
	float m_cosmeticRadius;

	int m_health;

	bool m_isDead;
	bool m_isGarbage;

	bool m_canWalk;
	bool m_canFly;
	bool m_canSwim;
	
	Game* m_game;


    virtual void Update(float deltaseconds);

    virtual void Render();
    virtual void Die();

	Entity(Game* theGame, Vec2 position);

	void SetPosition(Vec2& position);
	Vec2 GetPosition();
	

};