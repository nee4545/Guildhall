#pragma once
#include "Game/Entity.hpp"

class Texture;
class Game;
class Clock;
class Timer;



class Player :public Entity
{
public:
	Player();
	Player( Game* game );
	~Player();

	Clock* m_animClock = nullptr;
	Timer* m_animTimer = nullptr;


	virtual void Update( float deltaseconds ) override;
	virtual void Render() override;
	virtual void Die() override;
	virtual void DebugRender() override;

	void MovePlayer( float deltaseconds );
	void ToggleMeleeState();
	void Shoot();
	void MeleeAttack();
	Texture* GetTexutreBasedOnState();
	void ChangeAspectBasedOnState();
	void TransformArrayOnState(Vertex_PCU verts[6]);

	Vertex_PCU m_vertices[ 6 ];
	PlayerStates m_currentState = IDLE_MELEE;
	Game* m_game;
	float m_time = 0.f;

	Vec2 m_aspectAtIdle;
	Vec2 m_aspectAtAttack;
	Vec2 m_aspectAtWalk;

	Vec2 m_aspectAtIdleGun;
	Vec2 m_aspectAtAttackGun;
	Vec2 m_aspectAtWalkGun;

	bool m_lockAnimState = false;
	bool m_isActive = false;
	bool m_isMelee = true;
};