#pragma once

#include "Engine/Math/Vec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D;
class Rigidbody2D;
class Collider2D;
class Physics2D;

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

public:
	Physics2D* m_system=nullptr;						// which scene created/owns this object
	Collider2D* m_collider=nullptr;

	Vec2		m_worldPosition =Vec2(0.f,0.f);				// where in the world is this rigidbody
	bool		m_isGarbage = false;

private:
	~Rigidbody2D();								// destroys the collider
};