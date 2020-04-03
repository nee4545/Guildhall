#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

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

void Rigidbody2D::IncreamentMass( float increament )
{
	m_mass += increament;
	m_collider->CalculateMoment();
}

void Rigidbody2D::DecreamentMass( float decreament )
{
	m_mass -= decreament;

	if ( m_mass < 0.f )
	{
		m_mass = 0.001f;
	}

	m_collider->CalculateMoment();
}

void Rigidbody2D::IncreamentDrag( float increament )
{
	m_drag += increament;
}

void Rigidbody2D::DecreamentDrag( float decreament )
{
	m_drag -= decreament;
	
	if ( m_drag < 0.f )
	{
		m_drag = 0.f;
	}
}

void Rigidbody2D::IncreamentAngularDrag( float increament )
{
	m_angularDrag += increament;
}

void Rigidbody2D::DecreamnetAngularDrag( float decreament )
{
	m_angularDrag -= decreament;

	if ( m_angularDrag < 0.f )
	{
		m_angularDrag = 0.f;
	}
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

void Rigidbody2D::ApplyDrag( float deltaTime )
{
	Vec2 dragDirection = 1.f / m_mass * GetVerletVelocity().GetNormalized().GetRotated90Degrees().GetRotated90Degrees();
	m_velocity += m_drag * dragDirection * deltaTime;
}

void Rigidbody2D::ApplyAngularDrag( float deltaTime )
{
	if ( abs( m_angularVelocity ) <= 0.001f )
	{
		return;
	}

	if ( m_angularVelocity > 0 )
	{
		m_angularVelocity -= m_angularDrag * deltaTime;
	}
	else
	{
		m_angularVelocity += m_angularDrag * deltaTime;
	}

}

void Rigidbody2D::ApplyImpulse( Vec2 impulse )
{
	m_velocity += impulse / m_mass;
}

void Rigidbody2D::ApplyTorque( Vec2 impulse , Vec2 point )
{
	Vec2 d;
	Vec2 c;

	if ( m_collider->m_colliderType == COLLIDER2D_DISC )
	{
		c = point - m_worldPosition;
		d = c.GetRotated90Degrees();
	}

	if ( m_collider->m_colliderType == COLLIDER2D_POLYGON )
	{
		PolygonCollider2D* temp = ( PolygonCollider2D* ) m_collider;
		
		d = ( point - temp->m_polygonLocal->GetCentre() ).GetRotated90Degrees();
		
	}

	float torque = DotProduct2D( d , impulse );

	m_angularVelocity += torque / m_moment;
}

void Rigidbody2D::ApplyAngularAccleration( float deltaSeconds )
{
	m_angularVelocity += m_angularAccleration * deltaSeconds;
}

Vec2 Rigidbody2D::GetImapctVeclocity( Vec2 point )
{
	Vec2 d = point - m_worldPosition;
	Vec2 impactVel = m_velocity + d.GetRotated90Degrees() * m_angularVelocity;

	return impactVel;
}

void Rigidbody2D::MoveRigidBody( float deltaSeconds )
{
	if ( !enableSimulation )
	{
		return;
	}
	m_worldPosition += m_velocity * deltaSeconds;
	m_collider->UpdateWorldShape();
}

void Rigidbody2D::RotateRigidBody( float deltaSeconds )
{
	Rotate( ConvertRadiansToDegrees(m_angularVelocity) * deltaSeconds );
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

void Rigidbody2D::Rotate( float deltaRoationDegrees )
{
	float radians = ConvertDegreesToRadians( deltaRoationDegrees );
	m_rotationInRadians += radians;

	if ( m_collider->m_colliderType == COLLIDER2D_POLYGON )
	{
		PolygonCollider2D* poly = ( PolygonCollider2D* ) m_collider;
		poly->Rotate( deltaRoationDegrees );
	}
}

Rigidbody2D::~Rigidbody2D()
{

}
