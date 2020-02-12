#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Core/Polygon2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

PolygonCollider2D::PolygonCollider2D( Vec2 localPosition , Polygon2D* polygon )
{
	ASSERT_OR_DIE( polygon != nullptr , "A polygon must be specified" );

	m_colliderType = COLLIDER2D_POLYGON;
	m_polygonLocal = polygon;
	m_polygonLocal->m_localPos = localPosition;
	m_localPosition = localPosition;
	
}

void PolygonCollider2D::UpdateWorldShape()
{
	Vec2 worldPos = m_rigidbody->GetWorldPosition();
	m_worldPosition = worldPos/*+m_localPosition*/;
	m_polygonLocal->SetPosition( m_worldPosition );
}

Vec2 PolygonCollider2D::GetClosestPoint( Vec2 pos ) const
{
	return m_polygonLocal->GetClosestPoint( pos );
}

bool PolygonCollider2D::Contains( Vec2 pos ) const
{
	return m_polygonLocal->Contains( pos );
}

bool PolygonCollider2D::Intersects( Collider2D const* other ) const
{
	switch ( other->m_colliderType )
	{
	case COLLIDER2D_DISC:
	{
		DiscCollider2D* temp = ( DiscCollider2D* ) other;
		return DoDiscAndPolygonOverlap( other->m_rigidbody->m_worldPosition , temp->m_radius , *m_polygonLocal );
	}

	case COLLIDER2D_POLYGON:
	{
		return false;
		break;
	}
	default:
		return false;
		break;
	}
}

void PolygonCollider2D::DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor )
{
	ctx->DrawPolygonFilled( *m_polygonLocal , fillColor );
	ctx->DrawPolygonUnfilled( *m_polygonLocal , borderColor , 0.2f );
}

void PolygonCollider2D::MarkForDestroy()
{
	m_isGarbage = true;
}

PolygonCollider2D::~PolygonCollider2D()
{

}

