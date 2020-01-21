#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>

enum EntityFaction
{
	FACTION_GOOD=0,
	FACTION_EVIL,
	FACTION_NEUTRAL
};

enum EntityType
{
	ENTITY_TYPE_PLAYER=0,
	ENTITY_TYPE_NPC_TURRET,
	ENTITY_TYPE_NPC_TANK,
	ENTITY_TYPE_GOOD_BULLET,
	ENTITY_TYPE_EVIL_BULLET,
	ENTITY_TYPE_BOULDER,
	ENTITY_TYPE_EXPLOSION,
	ENTITY_TYPE_AIR_PLANE,
	ENTITY_TYPE_AIR_STRIKE,
	NUM_ENTITY_TYPES

};

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
	EntityType entityType;
	EntityFaction entityFaction;

	//Physics Flags
	bool m_isPushedByWalls=false;
	bool m_isPushedByEntities=false;
	bool m_doesPushEntities=false;
	bool m_isHitByBullets=false;

    virtual void Update(float deltaseconds);

    virtual void Render();
    virtual void Die();

	Entity(Game* theGame, Vec2 position);

	void SetPosition(Vec2& position);
	Vec2 GetPosition();
	

};

typedef std::vector<Entity*> EntityList;