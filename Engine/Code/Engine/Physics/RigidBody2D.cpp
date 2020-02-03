#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"

void Rigidbody2D::Destroy()
{
	m_isGarbage = true;
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
}

Rigidbody2D::~Rigidbody2D()
{

}
