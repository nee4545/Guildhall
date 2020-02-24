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




bool DiscVPolygonCollisionCheck( Collider2D const* col0 , Collider2D const* col1 )
{
	DiscCollider2D const* disc = ( DiscCollider2D const* ) col0;
	PolygonCollider2D const* poly = ( PolygonCollider2D const* ) col1;
	return DoDiscAndPolygonOverlap( disc->m_worldPosition , disc->m_radius , *poly->m_polygonLocal );

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
	UNUSED( col0 );
	UNUSED( col1 );
	return false;
}
