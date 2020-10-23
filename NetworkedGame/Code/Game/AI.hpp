#pragma once
#include "Game/Entity.hpp"
#include "Game/EntityCommon.hpp"

class Timer;
class Texture;

class AI :public Entity
{
public:
	AI(Game* game,Vec2 position,AiType type);
	~AI();

	virtual void Update( float deltaseconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	Vertex_PCU m_vertices[ 6 ];
	AiType m_type = TYPE_1;
	Timer* m_shootTimer = nullptr;
	Timer* m_moveTimer = nullptr;
	Vec2 m_nextMovePosition;

	Texture* m_sprite = nullptr;

};