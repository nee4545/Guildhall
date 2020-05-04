#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Physics/PhysicsCommon.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D;
class Rigidbody2D;
class Collider2D;
class PolygonCollider2D;
class Polygon2D;
class Clock;
class Timer;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct CollissionInfo
{
	int colliderId1;
	int colliderId2;
	bool wasCollidingLastFrame = false;
	bool isCollidingThisFrame = false;
};

class Physics2D
{
public:
	void BeginFrame();
	void StartUp();
	void Update();
	void AdvanceSimulation( double deltaSeconds );
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
	void ApplyAngularAccletation( float detaSeconds );
	void RotateRigidBodies( float deltaSeconds );
	void DetectCollissions();
	void ResolveCollissions();
	void ResolveCollission( Collision2D collission );

	void HandleCollissionInfo(int colliderId1, int colliderId2);
	Collider2D* GetCColliderForID( int id );

	void FireStartEvents( int colliderId1 , int colliderId2 );
	void FireExitEvents( int colliderId1 , int colliderId2 );
	void FireStayEvents( int colliderId1 , int colliderId2 );

	Vec2 GetImpulse( Collision2D& collision, Vec2& outTangentImpusle );

	void SetSceneGravity( float gravity );
	void CleanUp();
	void SetClock(Clock* clock);
	void SetFixedStepTime( double newDt );

public:
	// add members you may need to store these
	// storage for all rigid bodies
	static int colliderIds;
	std::vector<Rigidbody2D*>	m_rigidBodies2D;
	// storage for all colliders
	std::vector<Collider2D*>	m_colliders2D;
	float m_gravityMultiplier = 9.8f;
	std::vector<Collision2D> m_frameCollisions;

	std::vector<CollissionInfo*> m_collissionInfo;

	Clock* m_clock = nullptr;
	Timer* m_stepTimer = nullptr;
	double m_fixedDeltaTime = 0.1;

	static bool LayerInteractions[ TOTAL_LAYERS ][ TOTAL_LAYERS ];

	bool DoLayersInteract( Layers layer1 , Layers layer2 );
	void SetLayerInteraction( Layers layer1 , Layers layer2 , bool enableInteraction );


};

typedef Manifold2(*manifoldGenerations)( Collider2D const* , Collider2D const* );

Manifold2 GenerateDiscAndDiscManifold( Collider2D const* col0 , Collider2D const* col1 );
Manifold2 GenerateDiscAndPolygonManifold( Collider2D const* col0 , Collider2D const* col1 );
Manifold2 GeneratePolygonAndDiscManifold( Collider2D const* col0 , Collider2D const* col1 );
Manifold2 GeneratePolygonPolygonManifold( Collider2D const* col0 , Collider2D const* col1 );






//--------------------------------------------------------------------------------------------------------------------------------------------
