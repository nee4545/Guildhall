#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/EntityCommon.hpp"
#include <vector>


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
	Game* m_game;
	int m_ID=0;

	//Physics Flags
	bool m_isPushedByWalls=false;
	bool m_isPushedByEntities=false;
	bool m_doesPushEntities=false;
	bool m_isHitByBullets=false;

    virtual void Update(float deltaseconds);

    virtual void Render();
    virtual void Die();

	
	Entity(Game* game, Vec2 position);

	void SetPosition(Vec2& position);
	Vec2 GetPosition();
	
};

typedef std::vector<Entity*> EntityList;