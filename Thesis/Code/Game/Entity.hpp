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

enum Moves
{
	MOVE_UP ,
	MOVE_DOWN ,
	MOVE_RIGHT ,
	MOVE_LEFT ,
	MOVE_TOPRIGHT ,
	MOVE_TOPLEFT ,
	MOVE_BOTLEFT ,
	MOVE_BOTRIGHT ,
};

struct MovingHelper
{
	Moves move;
	std::vector<int> path;
	int currentIndex = 0;

	int GetNextMove()
	{
		if ( currentIndex < 0 )
		{
			Reset();
			return -1;
		}
		return path[ currentIndex-- ];
	}

	void Reset()
	{
		currentIndex = -1;
		path.clear();
	}

	bool DoesHavePath()
	{
		return path.size() > 0;
	}

};


class Entity
{
public:

	float m_orientationDegrees;
	float m_cosmeticRadius;
	float m_physicsRadius;
	float m_health;
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