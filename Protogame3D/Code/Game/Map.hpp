#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>
#include <vector>

class Entity;
class EntityDef;


struct RayCastResult
{
	bool didImpact = false;
	Vec2 startHitPoint;
	Vec2 endHitPoint;
	Vec2 impactNormal;
};


class Map
{
public:

	Map( char const* mapName );
	virtual ~Map();

	virtual Entity* SpawnNewEntityOfType( std::string const& entityDefName );
	virtual Entity* SpawnNewEntityOfType( EntityDef const& entityDef );


	virtual void UpdateMeshes( ) = 0;
	virtual void Render( ) = 0;
	virtual void Update( float deltaSeconds ) = 0;

	void ResolveEntityVsEntityCollisions();

public:
	std::string m_name;
	Vec3 m_playerStartPos;
	float m_playerStartYaw = 0.f;
	std::vector<Entity*> m_entities;
};