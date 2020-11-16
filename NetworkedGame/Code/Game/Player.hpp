#pragma once
#include "Entity.hpp"
#include "Engine/Core/AABB2.hpp"

class Texture;

class Player:public Entity
{
public:

	Player(Game* game,Vec2 position,Faction faction = FACTION_GOOD);

	Vertex_PCU m_vertices[6];
	Faction m_faction = FACTION_GOOD;

	Texture* m_sprite = nullptr;

	virtual void Update(float deltaSeconds) override;

	void MoveOnKeyPress( InputSystem* sys , float deltaSeconds );
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

	virtual void Render() override;
	virtual void Die() override;
	void LoadTextures();
	void CreateSpriteBasedOnFaction();
	void SetSpriteBasedOnID();

	int m_health = 10;

	bool m_hasMovedLastFrame = false;
	bool m_hasMovedThisFrame = false;
};
