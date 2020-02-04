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
	for ( int index = 0; index < m_colliders2D.size(); index++ )
	{
		if ( m_colliders2D[ index ] == nullptr )
		{
			continue;
		}
		if ( m_colliders2D[ index ]->m_isGarbage )
		{
			delete m_colliders2D[ index ];
			m_colliders2D[ index ] = nullptr;
		}
	}

	for ( int index = 0; index < m_rigidBodies2D.size(); index++ )
	{
		if ( m_rigidBodies2D[ index ] == nullptr )
		{
			continue;
		}
		if ( m_rigidBodies2D[ index ]->m_isGarbage )
		{
			if ( m_rigidBodies2D[ index ]->m_collider != nullptr )
			{
				m_rigidBodies2D[ index ]->m_collider->Destroy();
			}
			delete m_rigidBodies2D[ index ];
			m_rigidBodies2D[ index ] = nullptr;
		}
	}
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
		if ( m_rigidBodies2D[ index ] == nullptr )
		{
			continue;
		}

		if ( rigidbody == m_rigidBodies2D[ index ] )
		{
			if ( m_rigidBodies2D[ index ]->m_collider != nullptr )
			{
				m_rigidBodies2D[ index ]->m_collider->MarkForDestroy();
			}

			m_rigidBodies2D[ index ]->MarkForDestroy();
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

		if ( m_colliders2D[ index ] == nullptr )
		{
			continue;
		}
		if ( collider == m_colliders2D[ index ] )
		{
			m_colliders2D[ index ]->MarkForDestroy();
		}
	}
}

