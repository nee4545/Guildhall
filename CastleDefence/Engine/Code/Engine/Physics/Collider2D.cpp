#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Polygon2D.hpp"
#define UNUSED(x) (void)(x);

void Collider2D::Destroy()
{
	m_isGarbage = true;
}

collision_check_cb gCollisionChecks[ NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES ] = {
	/*             disc,                         polygon, */
	/*    disc */  DiscVsDiscCollisionCheck,      DiscVPolygonCollisionCheck,
	/* polygon */  PolygonVDiscCollisionCheck,    PolygonVPolygonCollisionCheck
};

bool Collider2D::Intersects( Collider2D const* other ) const
{
	COLLIDER2D_TYPE myType = m_colliderType;
	COLLIDER2D_TYPE otherType = other->m_colliderType;

	if ( myType <= otherType )
	{
		int idx = otherType * NUM_COLLIDER_TYPES + myType;
		collision_check_cb check = gCollisionChecks[ idx ];
		return check( this , other );
	}
	else
	{
		// flip the types when looking into the index.
		int idx = myType * NUM_COLLIDER_TYPES + otherType;
		collision_check_cb check = gCollisionChecks[ idx ];
		return check( other , this );
	}
}

float Collider2D::GetRestitutionWith( Collider2D const* other ) const
{
	return m_material.bounciness * other->m_material.bounciness;
}

float Collider2D::GetFrictionWith( Collider2D const* other ) const
{
	return (m_material.friction * other->m_material.friction);
}

void Collider2D::IncreamentBounciness( float increament )
{
	m_material.bounciness += increament;

	if ( m_material.bounciness > 1.f )
	{
		m_material.bounciness = 1.f;
	}
}

void Collider2D::DecreamentBounciness( float decreament )
{
	m_material.bounciness -= decreament;

	if ( m_material.bounciness < 0.f )
	{
		m_material.bounciness = 0.f;
	}
}

void Collider2D::IncreamentFriction( float increament )
{
	m_material.friction += increament;

	if ( m_material.friction > 1.f )
	{
		m_material.friction = 1.f;
	}
}

void Collider2D::DecreamentFriction( float decreament )
{
	m_material.friction -= decreament;

	if ( m_material.friction < 0.f )
	{
		m_material.friction = 0.f;
	}
}

bool DiscVPolygonCollisionCheck( Collider2D const* col0 , Collider2D const* col1 )
{

	DiscCollider2D const* disc = ( DiscCollider2D const* ) col0;
	PolygonCollider2D const* poly = ( PolygonCollider2D const* ) col1;

	//Mid Phase Check
	if ( !DoDiscsOverlap( poly->m_polygonLocal->GetCentre() , poly->boundingDiscRadius , disc->m_worldPosition , disc->m_radius ) )
	{
		return false;
	}
	else
	{
		return DoDiscAndPolygonOverlap( disc->m_worldPosition , disc->m_radius , *poly->m_polygonLocal );
	}
}

bool PolygonVDiscCollisionCheck( Collider2D const* col0 , Collider2D const* col1 )
{
	return DiscVPolygonCollisionCheck( col0 , col1 );
}

bool DiscVsDiscCollisionCheck( Collider2D const* col0 , Collider2D const* col1 )
{
	DiscCollider2D const* disc0 = ( DiscCollider2D const* ) col0;
	DiscCollider2D const* disc1 = ( DiscCollider2D const* ) col1;

	return DoDiscsOverlap( disc0->m_rigidbody->m_worldPosition , disc0->m_radius , disc1->m_worldPosition , disc1->m_radius );
}

bool PolygonVPolygonCollisionCheck( Collider2D const* col0 , Collider2D const* col1 )
{
	/*PolygonCollider2D const* poly0 = ( PolygonCollider2D* ) col0;
	PolygonCollider2D const* poly1 = ( PolygonCollider2D* ) col1;

	return DetectPolygonvPolygonIntersections( *poly0->m_polygonLocal , *poly1->m_polygonLocal );*/
	UNUSED( col0 );
	UNUSED( col1 );
	return false;
}
