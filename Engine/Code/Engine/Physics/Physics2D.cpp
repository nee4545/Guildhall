#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Core/Polygon2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <wtypes.h>

manifoldGenerations gManifoldGenerations[ NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES ] =
{
	GenerateDiscAndDiscManifold, GenerateDiscAndPolygonManifold,
	GeneratePolygonAndDiscManifold, nullptr
};


void Physics2D::BeginFrame()
{

}


void Physics2D::Update( float deltaSeconds )
{
	ApplyAffectors( deltaSeconds );
	MoveRigidBodies( deltaSeconds );
	DetectCollissions();
	ResolveCollissions();
	CleanUp();
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
	
	m_frameCollisions.clear();
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
	collider->m_colliderType = COLLIDER2D_DISC;
	return collider;
}


PolygonCollider2D* Physics2D::CreatePolygonCollider( Vec2 localPosition , Polygon2D* polygon )
{
	PolygonCollider2D* collider = new PolygonCollider2D( localPosition , polygon );
	m_colliders2D.push_back( collider );
	collider->m_colliderType = COLLIDER2D_POLYGON;
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

void Physics2D::ApplyAffectors( float deltaSeconds )
{
	for ( int index = 0; index < m_rigidBodies2D.size(); index++ )
	{
		if ( m_rigidBodies2D[ index ] == nullptr )
		{
			continue;
		}

		if ( m_rigidBodies2D[ index ]->enableSimulation == false )
		{
			continue;
		}
		
		switch (m_rigidBodies2D[index]->m_mode)
		{
			case STATIC:
			{
				break;
			}
			case DYNAMIC:
			{
				m_rigidBodies2D[ index ]->ApplyGravity( deltaSeconds , m_gravityMultiplier );
				break;
			}
			case KINAMETIC:
			{
				break;
			}
		}
		
	}
}

void Physics2D::MoveRigidBodies( float deltaSeconds )
{
	for ( int index = 0; index < m_rigidBodies2D.size(); index++ )
	{
		if ( m_rigidBodies2D[ index ] == nullptr )
		{
			continue;
		}

		if ( m_rigidBodies2D[ index ]->enableSimulation == false )
		{
			continue;
		}

		switch ( m_rigidBodies2D[ index ]->m_mode )
		{
			case STATIC:
			{
				m_rigidBodies2D[ index ]->m_velocity = Vec2( 0.f , 0.f );
				break;	
			}
			case DYNAMIC:
			{
				m_rigidBodies2D[ index ]->MoveRigidBody( deltaSeconds );
				break;
			}
			case KINAMETIC:
			{
				m_rigidBodies2D[ index ]->MoveRigidBody( deltaSeconds );
				break;
			}
		}

	}
}

void Physics2D::DetectCollissions()
{
	for ( int i = 0; i < m_rigidBodies2D.size(); i++ )
	{
		if ( m_rigidBodies2D[ i ] == nullptr )
		{
		continue;
		}

		if ( m_rigidBodies2D[ i ]->enableSimulation == false )
		{
			continue;
		}

		for ( int j = i+1; j < m_rigidBodies2D.size(); j++ )
		{
			if ( m_rigidBodies2D[ j ] == nullptr )
			{
				continue;
			}

			if ( m_rigidBodies2D[ j ]->enableSimulation == false )
			{
				continue;
			}

			
				if ( m_rigidBodies2D[ i ]->m_collider->Intersects( m_rigidBodies2D[ j ]->m_collider ) )
				{
					COLLIDER2D_TYPE myType = m_rigidBodies2D[ i ]->m_collider->m_colliderType;
					COLLIDER2D_TYPE otherType = m_rigidBodies2D[ j ]->m_collider->m_colliderType;
					int idx = otherType * NUM_COLLIDER_TYPES + myType;
					manifoldGenerations check = gManifoldGenerations[ idx ];

					if ( check != nullptr )
					{
						Collision2D collission;
						collission.me = m_rigidBodies2D[ i ]->m_collider;
						collission.them = m_rigidBodies2D[ j ]->m_collider;
						collission.manifold = check( m_rigidBodies2D[ i ]->m_collider , m_rigidBodies2D[ j ]->m_collider );
						m_frameCollisions.push_back( collission );
					}

				}
		}
	}
}

void Physics2D::ResolveCollissions()
{
	for ( int index = 0; index < m_frameCollisions.size(); index++ )
	{
		ResolveCollission( m_frameCollisions[ index ] );
	}

}

void Physics2D::ResolveCollission( Collision2D collision )
{
	float myMass = collision.me->m_rigidbody->m_mass;
	float theirMass = collision.them->m_rigidbody->m_mass;

	float pushMe = theirMass / ( myMass + theirMass );
	float pushThem = 1.0f - pushMe;

	if ( collision.me->m_rigidbody->m_collider->m_colliderType == COLLIDER2D_POLYGON )
	{
		collision.manifold.normal *= -1;
	}
	
	if ( ( collision.me->m_rigidbody->m_mode == DYNAMIC && collision.them->m_rigidbody->m_mode == DYNAMIC ) || ( collision.me->m_rigidbody->m_mode == KINAMETIC && collision.them->m_rigidbody->m_mode == KINAMETIC ) )
	{
		collision.me->m_rigidbody->Move( pushMe * collision.manifold.normal * collision.manifold.penetration * 0.5f );
		collision.them->m_rigidbody->Move( -pushThem * collision.manifold.normal * collision.manifold.penetration * 0.5f );
	}

	if ( collision.me->m_rigidbody->m_mode == STATIC && ( collision.them->m_rigidbody->m_mode == KINAMETIC || collision.them->m_rigidbody->m_mode == DYNAMIC ) )
	{
		collision.them->m_rigidbody->Move( -pushThem * collision.manifold.normal * collision.manifold.penetration );
	}

	if ( ( collision.me->m_rigidbody->m_mode == KINAMETIC || collision.me->m_rigidbody->m_mode == DYNAMIC ) && collision.them->m_rigidbody->m_mode == STATIC )
	{
		collision.me->m_rigidbody->Move( pushMe * collision.manifold.normal * collision.manifold.penetration );
	}

	if ( collision.me->m_rigidbody->m_mode == KINAMETIC && collision.them->m_rigidbody->m_mode == DYNAMIC )
	{
		collision.them->m_rigidbody->Move( -pushThem * collision.manifold.normal * collision.manifold.penetration );
	}

	if ( collision.me->m_rigidbody->m_mode == DYNAMIC && collision.them->m_rigidbody->m_mode == KINAMETIC )
	{
		collision.me->m_rigidbody->Move( pushMe * collision.manifold.normal * collision.manifold.penetration );
	}

	if ( collision.me->m_rigidbody->m_mode == KINAMETIC && collision.them->m_rigidbody->m_mode == STATIC )
	{
		collision.me->m_rigidbody->Move( pushMe * collision.manifold.normal * collision.manifold.penetration );
	}

	if ( collision.me->m_rigidbody->m_mode == STATIC && collision.them->m_rigidbody->m_mode == KINAMETIC )
	{
		collision.them->m_rigidbody->Move( -pushThem * collision.manifold.normal * collision.manifold.penetration );
	}


	float jn = ( myMass * theirMass ) / ( myMass + theirMass ) * ( 1 + collision.me->GetRestitutionWith(collision.them) ) *
		DotProduct2D( ( collision.them->m_rigidbody->m_velocity - collision.me->m_rigidbody->m_velocity ) , collision.manifold.normal );

	jn = ( jn < 0 ) ? 0 : jn;


	if ( ( collision.me->m_rigidbody->m_mode == DYNAMIC || collision.me->m_rigidbody->m_mode == KINAMETIC ) && ( collision.them->m_rigidbody->m_mode == DYNAMIC || collision.them->m_rigidbody->m_mode == KINAMETIC ) )
	{
		collision.me->m_rigidbody->ApplyImpulse( jn * collision.manifold.normal );
		collision.them->m_rigidbody->ApplyImpulse( -jn * collision.manifold.normal );
	}


	if ( collision.me->m_rigidbody->m_mode == STATIC && ( collision.them->m_rigidbody->m_mode == KINAMETIC || collision.them->m_rigidbody->m_mode == DYNAMIC ) )
	{
		float j = ( 1 + collision.me->GetRestitutionWith( collision.them )) * DotProduct2D( ( collision.them->m_rigidbody->m_velocity - collision.me->m_rigidbody->m_velocity ) , collision.manifold.normal );

		j = ( j < 0 ) ? 0 : j;

		collision.them->m_rigidbody->ApplyImpulse( -j * collision.manifold.normal );
	}


	if ( collision.them->m_rigidbody->m_mode == STATIC && ( collision.me->m_rigidbody->m_mode == KINAMETIC || collision.me->m_rigidbody->m_mode == DYNAMIC ) )
	{
		float j = ( 1 + collision.me->GetRestitutionWith( collision.them )) * DotProduct2D( ( collision.them->m_rigidbody->m_velocity - collision.me->m_rigidbody->m_velocity ) , collision.manifold.normal );

		j = ( j < 0 ) ? 0 : j;

		collision.me->m_rigidbody->ApplyImpulse( j* collision.manifold.normal );
	}

}

void Physics2D::SetSceneGravity( float gravity )
{
	m_gravityMultiplier = gravity;
}

void Physics2D::CleanUp()
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





Manifold2 GenerateDiscAndDiscManifold( Collider2D const* col0 , Collider2D const* col1 )
{
	Manifold2 collision;

	DiscCollider2D const* disc0 = ( DiscCollider2D const* ) col0;
	DiscCollider2D const* disc1 = ( DiscCollider2D const* ) col1;

	float distance = disc0->m_radius + disc1->m_radius - ( disc1->m_worldPosition - disc0->m_worldPosition ).GetLength();
	Vec2 normal = (disc0->m_worldPosition - disc1->m_worldPosition).GetNormalized();
	Vec2 centre = disc0->m_worldPosition + normal * distance * 0.5f;

	collision.centre = centre;
	collision.normal = normal;
	collision.penetration = distance;

	return collision;

}

Manifold2 GenerateDiscAndPolygonManifold( Collider2D const* col0 , Collider2D const* col1 )
{
	return GeneratePolygonAndDiscManifold( col1 , col0 );
}

Manifold2 GeneratePolygonAndDiscManifold( Collider2D const* col0 , Collider2D const* col1 )
{
	Manifold2 collision;
	DiscCollider2D* discColliderMe = ( DiscCollider2D* ) col0;
	PolygonCollider2D* polyColliderThem = ( PolygonCollider2D* ) col1;
	Vec2 closetPoint = polyColliderThem->GetClosestPoint( discColliderMe->m_worldPosition );
	collision.normal = ( discColliderMe->m_worldPosition - closetPoint ).GetNormalized();
	collision.penetration = discColliderMe->m_radius - ( discColliderMe->m_worldPosition - closetPoint ).GetLength();

	if ( discColliderMe->Contains( closetPoint ) )
	{
		closetPoint = polyColliderThem->m_polygonLocal->GetClosestPointOnTheEdges( closetPoint );
		collision.normal = ( closetPoint - discColliderMe->m_worldPosition ).GetNormalized();
		collision.penetration = -( closetPoint - discColliderMe->m_worldPosition ).GetLength() + discColliderMe->m_radius;
		collision.normal = collision.normal.GetRotatedDegrees( 180.f );
	}

	if ( polyColliderThem->Contains( discColliderMe->m_worldPosition ) )
	{
		collision.normal = -collision.normal;
		collision.penetration = ( discColliderMe->m_worldPosition - closetPoint ).GetLength()+discColliderMe->m_radius;
	}

	collision.centre = discColliderMe->m_worldPosition + ( collision.normal * ( discColliderMe->m_radius - ( collision.penetration * 0.5f ) ) );
	return collision;
}
