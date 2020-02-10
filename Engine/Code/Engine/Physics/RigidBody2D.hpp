#pragma once

#include "Engine/Math/Vec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D;
class Rigidbody2D;
class Collider2D;
class Physics2D;

enum eSimulationMode
{
	STATIC,
	KINAMETIC,
	DYNAMIC,
};



//--------------------------------------------------------------------------------------------------------------------------------------------

class Rigidbody2D
{
	friend class Physics2D;

public:
	void Destroy();								// helper for destroying myself (uses owner to mark self for destruction)
	Rigidbody2D() {};

	void TakeCollider( Collider2D* collider );	// takes ownership of a collider (destroying my current one if present)
	Collider2D* GetCollider() const { return m_collider; }
	void SetCollider( Collider2D* collider );
	void SetPosition( Vec2 position );
	Vec2 GetWorldPosition() { return m_worldPosition; }
	void SetVelocity( Vec2 velocity );

private: 

	void MarkForDestroy();
	bool		m_isGarbage = false;

public:
	bool enableSimulation = true;
	Physics2D* m_system=nullptr;						// which scene created/owns this object
	Collider2D* m_collider=nullptr;
	Vec2 m_velocity = Vec2(0.f,0.f);
	eSimulationMode m_mode = STATIC;
	float m_mass=1.f;
	Vec2 m_gravityDirection = Vec2( 0.f , -1.f );
	Vec2 m_gravityVector = Vec2( 0.f , 0.f );
	void ApplyGravity( float deltaTime, float gravityMultiplier );
	void MoveRigidBody( float deltaSeconds );
	void ReverseVelocityYAxis();
	void SetSimulationMode( eSimulationMode mode );
	Vec2 m_worldPosition =Vec2(0.f,0.f);				// where in the world is this rigidbody

private:
	~Rigidbody2D();								// destroys the collider
};