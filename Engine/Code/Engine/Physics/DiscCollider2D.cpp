#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

DiscCollider2D::DiscCollider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType , Vec2 localPosition , float radius /*= 1.f */ )
{
	m_system = system;
	m_rigidbody = rigidbody;
	m_colliderType = colliderType;
	m_localPosition = localPosition;
	m_radius = radius;

}

DiscCollider2D::DiscCollider2D( Vec2 localPosition , float radius )
{
	m_localPosition = localPosition;
	m_radius = radius;
	m_colliderType = COLLIDER2D_DISC;
	m_material.bounciness = 0.5f;
}

DiscCollider2D::~DiscCollider2D()
{

}

void DiscCollider2D::UpdateWorldShape()
{
	//m_worldPosition = m_rigidbody->m_worldPosition+m_localPosition;
	Vec2 worldPos = m_rigidbody->GetWorldPosition();
	m_worldPosition = worldPos /*+ m_localPosition*/;
}

Vec2 DiscCollider2D::GetClosestPoint( Vec2 pos ) const
{
	Vec2 closestPoint;

	closestPoint = GetNearstPointOnDisc2D( closestPoint , m_worldPosition , m_radius );

	return closestPoint;

}

bool DiscCollider2D::Contains( Vec2 pos ) const
{
	return IsPointInsideDisc2D( pos , m_worldPosition , m_radius );

}

//bool DiscCollider2D::Intersects( Collider2D const* other ) const
//{
//	
//
//	switch ( other->m_colliderType )
//	{
//	case COLLIDER2D_DISC:
//	{
//		DiscCollider2D* temp = ( DiscCollider2D* ) other;
//		return DoDiscsOverlap( m_worldPosition , m_radius , temp->m_worldPosition , temp->m_radius );
//	}
//
//	case  COLLIDER2D_POLYGON:
//	{
//		PolygonCollider2D* temp = ( PolygonCollider2D* ) other;
//		return DoDiscAndPolygonOverlap( m_worldPosition , m_radius , *temp->m_polygonLocal );
//	}
//	default:
//		return false;
//		break;
//	}
//}

void DiscCollider2D::DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor )
{
	ctx->DrawRing( m_worldPosition , m_radius , borderColor , 0.3f );
	ctx->DrawDisc( m_worldPosition , m_radius , fillColor );
}

void DiscCollider2D::MarkForDestroy()
{
	m_isGarbage = true;
}
