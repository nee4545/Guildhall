#pragma once
#include "Entity.hpp"
#include "Engine/Core/AABB2.hpp"

class Texture;

class Player:public Entity
{
public:

	Player(Game* game,Vec2 position);

	Vertex_PCU m_vertices[6];

	Texture* m_sprite = nullptr;

	virtual void Update(float deltaSeconds) override;
	virtual void Render() override;
	virtual void Die() override;
	void LoadTextures();

	int m_health = 10;
};
