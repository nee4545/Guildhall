#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <string>

class ActorDefinition;
class Game;
class SpriteAnimSet;

class Actor:public Entity
{
private:
	Vertex_PCU m_vertices[6];
	Vertex_PCU m_vertices_original[6];
	SpriteAnimSet* m_spriteAnimations = nullptr;

	float m_timeElapsedInSeconds=0.f;
	Vec2 m_uvMins;
	Vec2 m_uvMaxs;
	std::string m_name;

public:
	explicit Actor(Game* game, Vec2 position, ActorDefinition* definition);

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void ResetVertices();
	void ResetUvs();
	void UpdateFromJoystick(float deltaSeconds);
	void MoveActor(float deltaSeconds);
	std::string GetActorName() { return m_name; }
	Strings GetActorDetails();
};