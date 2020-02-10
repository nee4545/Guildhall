#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D;
class Rigidbody2D;
class Collider2D;
class PolygonCollider2D;
class Polygon2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Physics2D
{
public:
	void BeginFrame();
	void Update(float deltaSeconds);
	void EndFrame();    // cleanup destroyed objects

	Physics2D() {};

	// factory style create/destroy
	Rigidbody2D* CreateRigidbody();
	void DestroyRigidbody( Rigidbody2D* rigidbody );

	DiscCollider2D* CreateDiscCollider( Vec2 localPosition , float radius );
	PolygonCollider2D* CreatePolygonCollider( Vec2 localPosition , Polygon2D* polygon );
	void DestroyCollider( Collider2D* collider );
	void ApplyAffectors( float deltaSeconds );
	void MoveRigidBodies( float deltaSeconds );
	void SetSceneGravity( float gravity );

public:
	// add members you may need to store these
	// storage for all rigid bodies
	std::vector<Rigidbody2D*>	m_rigidBodies2D;
	// storage for all colliders
	std::vector<Collider2D*>	m_colliders2D;
	float m_gravityMultiplier = 8.2f;


};








//--------------------------------------------------------------------------------------------------------------------------------------------
