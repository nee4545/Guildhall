#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"

void Rigidbody2D::Destroy()
{
	m_isGarbage = true;
	m_collider->Destroy();
}

void Rigidbody2D::TakeCollider( Collider2D* collider )
{
	m_collider = collider;
	m_collider->m_rigidbody = this;
}

void Rigidbody2D::SetCollider( Collider2D* collider )
{
	if ( m_collider != nullptr )
	{
		m_system->DestroyCollider( collider );
	}

	m_collider = collider;
}

void Rigidbody2D::SetPosition( Vec2 position )
{
	m_worldPosition = position;
	if ( m_collider != nullptr )
	{
		m_collider->UpdateWorldShape();
	}
}

void Rigidbody2D::SetVelocity( Vec2 velocity )
{
	m_velocity = velocity;
}

void Rigidbody2D::MarkForDestroy()
{
	m_isGarbage = true;
}

void Rigidbody2D::ApplyGravity( float deltaTime, float gravityMultiplier )
{
	if ( !enableSimulation )
	{
		return;
	}
	m_gravityVector = ( m_mass * gravityMultiplier * m_gravityDirection );
	m_velocity += m_gravityVector*deltaTime;

}

//void Rigidbody2D::ApplyDrag( float deltaTime )
//{
//	Vec2 velocity = GetVerletVelocity();
//	Vec2 dragForce = -velocity * m_drag;
//}

void Rigidbody2D::ApplyImpulse( Vec2 impulse )
{
	m_velocity += impulse / m_mass;
}

void Rigidbody2D::MoveRigidBody( float deltaSeconds )
{
	if ( !enableSimulation )
	{
		return;
	}
	m_worldPosition += m_velocity * deltaSeconds;
}



void Rigidbody2D::Move(Vec2 movement)
{
	m_worldPosition += movement;
	m_collider->UpdateWorldShape();
}

void Rigidbody2D::ReverseVelocityYAxis()
{
	m_velocity.y *= -1;
}

void Rigidbody2D::SetSimulationMode( eSimulationMode mode )
{
	m_mode = mode;
}

Rigidbody2D::~Rigidbody2D()
{

}
