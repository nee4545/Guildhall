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
	Vec2 GetVerletVelocity() { return m_verletVelocity; }
	Vec2 GetImapctVeclocity( Vec2 point );
	void SetVelocity( Vec2 velocity );
	void IncreamentMass( float increament );
	void DecreamentMass( float decreament );
	void IncreamentDrag( float increament );
	void DecreamentDrag( float decreament );
	void IncreamentAngularDrag( float increament );
	void DecreamnetAngularDrag( float decreament );
	void ApplyGravity( float deltaTime , float gravityMultiplier );
	void ApplyDrag( float deltaTime );
	void ApplyAngularDrag( float deltaTime );
	void ApplyImpulse( Vec2 impulse );
	void ApplyTorque( Vec2 impulse , Vec2 point );
	void ApplyAngularAccleration( float deltaSeconds );
	void MoveRigidBody( float deltaSeconds );
	void RotateRigidBody( float deltaSeconds );
	void Move( Vec2 movement );
	void ReverseVelocityYAxis();
	void SetSimulationMode( eSimulationMode mode );
	void Rotate( float deltaRoationDegrees );

private: 

	void MarkForDestroy();
	bool		m_isGarbage = false;

public:
	bool enableSimulation = true;
	Physics2D* m_system=nullptr;						// which scene created/owns this object
	Collider2D* m_collider=nullptr;
	Vec2 m_velocity = Vec2(0.f,0.f);
	Vec2 m_verletVelocity = Vec2(0.f,0.f);
	eSimulationMode m_mode = DYNAMIC;
	Vec2 m_gravityDirection = Vec2( 0.f , -1.f );
	Vec2 m_gravityVector = Vec2( 0.f , 0.f );
	Vec2 m_frameStartPosition = Vec2( 0.f , 0.f );
	Vec2 m_worldPosition = Vec2( 0.f , 0.f );
	float m_drag = 0.f;
	float m_angularDrag = 0.f;
	float m_mass=1.f;
	
	float m_rotationInRadians = 0.0f;
	float m_angularAccleration = 0.f;
	float m_angularVelocity = 0.0f;
	float m_frameTorque = 0.f;
	float m_moment = 0.f;

private:
	~Rigidbody2D();								// destroys the collider
};