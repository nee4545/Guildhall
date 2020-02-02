#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"


void Physics2D::BeginFrame()
{

}


void Physics2D::Update()
{

}


void Physics2D::EndFrame()
{

}

Rigidbody2D* Physics2D::CreateRigidbody()
{
	Rigidbody2D* rb = new Rigidbody2D(); 
	m_rigidBodies2D.push_back( rb );
	return rb;
}

void Physics2D::DestroyRigidbody( Rigidbody2D* rigidbody )
{
	for ( int index = 0; index < m_rigidBodies2D.size(); index++ )
	{
		if ( rigidbody == m_rigidBodies2D[ index ] )
		{
			delete m_rigidBodies2D[ index ];
			m_rigidBodies2D[ index ] = nullptr;
			break;
		}
	}
}


DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 localPosition , float radius )
{
	DiscCollider2D* collider = new DiscCollider2D(localPosition,radius);
	m_colliders2D.push_back( collider );
	return collider;
}


void Physics2D::DestroyCollider( Collider2D* collider )
{
	for ( int index = 0; index < m_colliders2D.size(); index++ )
	{
		if ( collider == m_colliders2D[ index ] )
		{
			delete m_colliders2D[ index ];
			m_colliders2D[ index ] = nullptr;
			break;
		}
	}
}

