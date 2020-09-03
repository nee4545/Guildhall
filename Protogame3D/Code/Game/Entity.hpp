#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <string>
#include "Game/EntityDef.hpp"
#include "Game/BillBoard.hpp"

class SpriteSheet;
class GPUMesh;
class SpriteAnimDefinition;
class Map;


class Entity
{
public:
	Vec2 m_position;
	float m_orientationDegrees;
	float m_physicsRadius=0.5f;
	float m_eyeHeight = 0.f;
	IntVec2 m_damage;
	float m_cosmeticRadius;
	int m_health;
	bool m_isDead;
	bool m_isGarbage;
	SpriteSheet* m_spriteSheet= nullptr;
	bool m_canBePushedByWalls = true;
	bool m_canBePushedByEntities = true;
	bool m_canPushEntities = true;
	float m_mass = 1.f;
	float m_height = 0.5f;
	GPUMesh* m_mesh = nullptr;
	GPUMesh* m_debugMesh = nullptr;
	Map* m_owner = nullptr;
	std::map<std::string , SpriteAnimDefinition*> m_entityAnims[ NUM_TOTAL_ENTITY_ANIMS ];
	Vec2 m_spriteSize;
	float m_speed;
	eBillBoardType m_billboard = CAMERA_FACING_XY;
	Vec2 m_forwardVector = Vec2( 0.f , 1.f );

    virtual void Update(float deltaseconds);
    virtual void Render();
	virtual void DebugRender();
    virtual void Die();

	Entity(Vec2 position);
	Entity( EntityDef const& entityDef , Map* map );
	Entity() {};

	void SetPosition(Vec2& position);
	Vec2 GetPosition();
	

};