#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Core/Polygon2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include <wtypes.h>
#include "Engine/Renderer/DebugRender.hpp"

manifoldGenerations gManifoldGenerations[ NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES ] =
{
	GenerateDiscAndDiscManifold, GenerateDiscAndPolygonManifold,
	GeneratePolygonAndDiscManifold, nullptr
};


void Physics2D::BeginFrame()
{

}


void Physics2D::StartUp()
{
	m_clock = new Clock();
	m_stepTimer = new Timer();
	m_fixedDeltaTime = 1.0/120.0;
	//m_clock->m_scale = 5.f;
	m_stepTimer->SetSeconds( m_clock , m_fixedDeltaTime );
}

void Physics2D::Update()
{
	while ( m_stepTimer->CheckAndDecrement() )
	{
		for ( int i = 0; i < m_rigidBodies2D.size(); i++ )
		{
			if ( m_rigidBodies2D[ i ] == nullptr )
			{
				continue;
			}

			m_rigidBodies2D[ i ]->m_frameStartPosition = m_rigidBodies2D[ i ]->GetWorldPosition();
		}
		AdvanceSimulation( m_fixedDeltaTime );
		m_frameCollisions.clear();
	}

	
	//AdvanceSimulation( m_clock->GetLastDeltaSeconds());
	
}


void Physics2D::AdvanceSimulation( double deltaSeconds )
{
	ApplyAffectors( (float)deltaSeconds );
	MoveRigidBodies( (float)deltaSeconds );
	RotateRigidBodies( ( float ) deltaSeconds );
	DetectCollissions();
	ResolveCollissions();
	CleanUp();
}

void Physics2D::EndFrame()
{

	//Set Verlet velocity
	

	for ( int i = 0; i < m_rigidBodies2D.size(); i++ )
	{
		if ( m_rigidBodies2D[ i ] == nullptr )
		{
			continue;
		}

		m_rigidBodies2D[ i ]->m_verletVelocity = ( m_rigidBodies2D[ i ]->GetWorldPosition() - m_rigidBodies2D[ i ]->m_frameStartPosition ) / (float)m_fixedDeltaTime;
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
				m_rigidBodies2D[ index ]->ApplyDrag( deltaSeconds );
				m_rigidBodies2D[ index ]->ApplyAngularDrag( deltaSeconds );
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

void Physics2D::ApplyAngularAccletation( float detaSeconds )
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

		m_rigidBodies2D[ i ]->ApplyAngularAccleration( detaSeconds );

	}
}

void Physics2D::RotateRigidBodies( float deltaSeconds )
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

		m_rigidBodies2D[ i ]->RotateRigidBody( deltaSeconds );

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

			if ( m_rigidBodies2D[ i ]->m_mode == STATIC && m_rigidBodies2D[ j ]->m_mode == STATIC )
			{
				continue;
			}

			
			if ( m_rigidBodies2D[ i ]->m_collider->Intersects( m_rigidBodies2D[ j ]->m_collider ) )
			{
				/*if ( m_rigidBodies2D[ i ]->m_collider->m_colliderType == COLLIDER2D_POLYGON && m_rigidBodies2D[ j ]->m_collider->m_colliderType == COLLIDER2D_POLYGON )
				{
					continue;
				}*/

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
	
	if ( collision.me->m_rigidbody->m_collider->m_colliderType == COLLIDER2D_POLYGON )
	{
		collision.manifold.normal *= -1;
	}

	if ( ( collision.me->m_rigidbody->m_mode == DYNAMIC && collision.them->m_rigidbody->m_mode == DYNAMIC ) || ( collision.me->m_rigidbody->m_mode == KINAMETIC && collision.them->m_rigidbody->m_mode == KINAMETIC ) )
	{
		collision.me->m_rigidbody->Move( collision.manifold.normal * collision.manifold.penetration * 0.5f );
		collision.them->m_rigidbody->Move( -collision.manifold.normal * collision.manifold.penetration * 0.5f );
	}

	if ( collision.me->m_rigidbody->m_mode == STATIC && ( collision.them->m_rigidbody->m_mode == KINAMETIC || collision.them->m_rigidbody->m_mode == DYNAMIC ) )
	{
		collision.them->m_rigidbody->Move( -collision.manifold.normal * collision.manifold.penetration );
	}

	if ( ( collision.me->m_rigidbody->m_mode == KINAMETIC || collision.me->m_rigidbody->m_mode == DYNAMIC ) && collision.them->m_rigidbody->m_mode == STATIC )
	{
		collision.me->m_rigidbody->Move( collision.manifold.normal * collision.manifold.penetration );
	}

	if ( collision.me->m_rigidbody->m_mode == KINAMETIC && collision.them->m_rigidbody->m_mode == DYNAMIC )
	{
		collision.them->m_rigidbody->Move( -collision.manifold.normal * collision.manifold.penetration );
	}

	if ( collision.me->m_rigidbody->m_mode == DYNAMIC && collision.them->m_rigidbody->m_mode == KINAMETIC )
	{
		collision.me->m_rigidbody->Move( collision.manifold.normal * collision.manifold.penetration );
	}

	if ( collision.me->m_rigidbody->m_mode == KINAMETIC && collision.them->m_rigidbody->m_mode == STATIC )
	{
		collision.me->m_rigidbody->Move( collision.manifold.normal * collision.manifold.penetration );
	}

	if ( collision.me->m_rigidbody->m_mode == STATIC && collision.them->m_rigidbody->m_mode == KINAMETIC )
	{
		collision.them->m_rigidbody->Move( -collision.manifold.normal * collision.manifold.penetration );
	}

	Vec2 tangentImp;
	Vec2 imp = GetImpulse( collision, tangentImp );

	if ( ( collision.me->m_rigidbody->m_mode == DYNAMIC || collision.me->m_rigidbody->m_mode == KINAMETIC ) && ( collision.them->m_rigidbody->m_mode == DYNAMIC || collision.them->m_rigidbody->m_mode == KINAMETIC ) )
	{
		collision.me->m_rigidbody->ApplyImpulse( imp );
		collision.them->m_rigidbody->ApplyImpulse( -imp );

		collision.me->m_rigidbody->ApplyImpulse( tangentImp );
		collision.them->m_rigidbody->ApplyImpulse( -tangentImp );

		collision.me->m_rigidbody->ApplyTorque( tangentImp , collision.manifold.centre );
		collision.them->m_rigidbody->ApplyTorque( -tangentImp , collision.manifold.centre );

		collision.me->m_rigidbody->ApplyTorque( imp , collision.manifold.centre );
		collision.them->m_rigidbody->ApplyTorque( -imp , collision.manifold.centre );
	}

	if ( collision.me->m_rigidbody->m_mode == STATIC && ( collision.them->m_rigidbody->m_mode == KINAMETIC || collision.them->m_rigidbody->m_mode == DYNAMIC ) )
	{
		collision.them->m_rigidbody->ApplyImpulse( -imp );
		collision.them->m_rigidbody->ApplyImpulse( -tangentImp );
		collision.them->m_rigidbody->ApplyTorque( -tangentImp , collision.manifold.centre );
		collision.them->m_rigidbody->ApplyTorque( -imp , collision.manifold.centre );
	}

	if ( collision.them->m_rigidbody->m_mode == STATIC && ( collision.me->m_rigidbody->m_mode == KINAMETIC || collision.me->m_rigidbody->m_mode == DYNAMIC ) )
	{
		collision.me->m_rigidbody->ApplyImpulse( imp );
		collision.me->m_rigidbody->ApplyImpulse( tangentImp );
		collision.me->m_rigidbody->ApplyTorque( tangentImp , collision.manifold.centre );
		collision.me->m_rigidbody->ApplyTorque( imp , collision.manifold.centre );
	}

}

Vec2 Physics2D::GetImpulse( Collision2D& collision, Vec2 &outTangentImpusle )
{

	float myMass = collision.me->m_rigidbody->m_mass;
	float theirMass = collision.them->m_rigidbody->m_mass;
	
	Vec2 n = collision.manifold.normal;
	Vec2 t = collision.manifold.normal.GetRotatedMinus90Degrees();

	Vec2 myVel = collision.me->m_rigidbody->GetImapctVeclocity( collision.manifold.centre );
	Vec2 themVelDiff = collision.them->m_rigidbody->GetImapctVeclocity( collision.manifold.centre );
	Vec2 imapctVelDiff = myVel - themVelDiff;
	
	float numerator = -1 * ( 1 + collision.me->GetRestitutionWith( collision.them ) ) * DotProduct2D( ( imapctVelDiff ) , n );

	Vec2 rap = ( collision.manifold.centre - collision.me->m_rigidbody->m_worldPosition ).GetRotated90Degrees();
	Vec2 rbp = ( collision.manifold.centre - collision.them->m_rigidbody->m_worldPosition ).GetRotated90Degrees();

	if ( collision.me->m_rigidbody->m_mode == STATIC && ( collision.them->m_rigidbody->m_mode == KINAMETIC || collision.them->m_rigidbody->m_mode == DYNAMIC ) )
	{
		float massFactor = ( 1 / theirMass );
		massFactor *= DotProduct2D( n , n );
		float themInertia = DotProduct2D( rbp , n );
		themInertia *= themInertia;
		themInertia /= collision.them->m_rigidbody->m_moment;

		float denom = massFactor + themInertia;

		float jn = numerator / denom;
		//jn = ( jn < 0 ) ? 0 : jn;

		float numeratorT = -1 * (1+ collision.me->GetRestitutionWith( collision.them ) ) * DotProduct2D( ( imapctVelDiff ) , t );

		float massFactorT = ( 1 / theirMass );
		float themInertiaT = DotProduct2D( rbp , t);
		themInertiaT *= themInertiaT;
		themInertiaT /= collision.them->m_rigidbody->m_moment;

		float denomT = massFactorT +  themInertiaT;
		float jt = numeratorT / denomT;

		float friction = collision.me->GetFrictionWith( collision.them );
		jt = Clamp( jt , -friction * jn , friction * jn );
		

		Vec2 J = ( jn * n );
		outTangentImpusle = ( jt * t );
		return J;
	}


	if ( collision.them->m_rigidbody->m_mode == STATIC && ( collision.me->m_rigidbody->m_mode == KINAMETIC || collision.me->m_rigidbody->m_mode == DYNAMIC ) )
	{
		
		float massFactor = ( 1 / myMass );
		massFactor *= DotProduct2D( n , n );
		float meInertia = DotProduct2D( rap , n );
		meInertia *= meInertia;
		meInertia /= collision.me->m_rigidbody->m_moment;

		float denom = massFactor + meInertia;

		float jn = numerator / denom;
		//jn = ( jn < 0 ) ? 0 : jn;

		float numeratorT = -1 * (1+ collision.them->GetRestitutionWith( collision.me ) ) * DotProduct2D( ( imapctVelDiff ) , t );

		float massFactorT = ( 1 / myMass );
		float themInertiaT = DotProduct2D( rap , t );
		themInertiaT *= themInertiaT;
		themInertiaT /= collision.me->m_rigidbody->m_moment;

		float denomT = massFactorT + themInertiaT;
		float jt = numeratorT / denomT;

		float friction = collision.me->GetFrictionWith( collision.them );
		jt = Clamp( jt , -friction * jn , friction * jn );

		//jt = 0;

		Vec2 J = ( jn * n );
		outTangentImpusle = ( jt * t );
		return J;
	}

	float massFactor = ( 1 / myMass ) + ( 1 / theirMass );
	massFactor *= DotProduct2D( n , n );
	float meInertia = DotProduct2D( rap , n );
	meInertia *= meInertia;
	meInertia /= collision.me->m_rigidbody->m_moment;

	float themInertia = DotProduct2D( rbp , n );
	themInertia *= themInertia;
	themInertia /= collision.them->m_rigidbody->m_moment;

	float denom = massFactor + meInertia + themInertia;

	float jn = numerator / denom;

	//jn = ( jn < 0 ) ? 0 : jn;

	float numeratorT = -1 * (1+ collision.me->GetRestitutionWith( collision.them ) ) * DotProduct2D( ( imapctVelDiff ) , t );

	float massFactorT = ( 1 / myMass ) + ( 1 / theirMass );
	massFactorT *= DotProduct2D( t , t );
	float meInertiaT = DotProduct2D( rap , t );
	meInertiaT *= meInertiaT;
	meInertiaT /= collision.me->m_rigidbody->m_moment;

	float themInertiaT = DotProduct2D( rbp , t );
	themInertiaT *= themInertiaT;
	themInertiaT /= collision.them->m_rigidbody->m_moment;

	float denomT = massFactorT + meInertiaT + themInertiaT;

	float jt = numeratorT / denomT;

 	Vec2 J = ( jn * n );
	outTangentImpusle = ( jt * t );

	return J;
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


void Physics2D::SetClock( Clock* clock )
{
	if ( clock == nullptr )
	{
		m_clock = &Clock::gMasterClock;
	}
	else
	{
		m_clock = clock;
	}
}

void Physics2D::SetFixedStepTime( double newDt )
{
	m_fixedDeltaTime = newDt;
}

Manifold2 GenerateDiscAndDiscManifold( Collider2D const* col0 , Collider2D const* col1 )
{
	Manifold2 collision;

	DiscCollider2D const* disc0 = ( DiscCollider2D const* ) col0;
	DiscCollider2D const* disc1 = ( DiscCollider2D const* ) col1;

	float distance = disc0->m_radius + disc1->m_radius - ( disc1->m_worldPosition - disc0->m_worldPosition ).GetLength();
	Vec2 normal = ( disc0->m_worldPosition - disc1->m_worldPosition ).GetNormalized();
	Vec2 centre = ( disc0->m_worldPosition ) - ( normal * distance * 0.5f ) - ( normal * disc0->m_radius );

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
		collision.penetration = ( discColliderMe->m_worldPosition - closetPoint ).GetLength() + discColliderMe->m_radius;
	}

	collision.centre = discColliderMe->m_worldPosition - ( collision.normal * ( discColliderMe->m_radius ) );

	/*DebugAddWorldPoint( Vec3( collision.centre , 1.f ) , 1.f , Rgba8( 100 , 0 , 0 , 255 ) , 0.f );
	DebugAddWorldArrow( Vec3( discColliderMe->m_worldPosition , 0.f ) , Vec3( discColliderMe->m_worldPosition , 0.f ) + Vec3( collision.normal , 0.f ) * 3.f , Rgba8( 100 , 0 , 0 , 255 ) , 0.f , 1.f , DEBUG_RENDER_ALWAYS );*/

	
	return collision;
}
