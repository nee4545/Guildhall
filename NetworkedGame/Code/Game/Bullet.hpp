#pragma once
#include "Game/Entity.hpp"
#include "Game/EntityCommon.hpp"

class Bullet : public Entity
{
public:
	Bullet( Game* game, Vec2 position,Vec2 forwardVector, Faction faction = FACTION_GOOD );
	~Bullet();

	
	float bulletSpeed=0.f;

	Faction m_faction = FACTION_GOOD;
	Vec2 m_forwardVector = Vec2( 0.f , 1.f );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void MoveBullet(float deltaSeconds);
	void ResetVertices();
	void ChangeDirection();
};