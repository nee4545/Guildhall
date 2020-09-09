#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <string>

enum PlayerStates
{
	IDLE_MELEE ,
	MOVING_MELEE ,
	ATTACKING_MELEE ,
	IDLE_GUN,
	MOVING_GUN,
	ATTACKING_GUN,
};


class Entity
{
public:

	float m_orientationDegrees;
	float m_cosmeticRadius;
	float m_physicsRadius;
	int m_health;
	bool m_isDead;
	bool m_isGarbage;
	Vec2 m_position;
	
    virtual void Update(float deltaseconds);
    virtual void Render();
	virtual void DebugRender();
    virtual void Die();


	Entity() {};

	void SetPosition(Vec2& position);
	Vec2 GetPosition();
	

};