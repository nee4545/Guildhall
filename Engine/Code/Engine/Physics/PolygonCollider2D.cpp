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
	boundingDiscRadius = m_polygonLocal->GetBoundingDiscRadius();

	AppendPolygon2( m_verts , m_polygonLocal );
	//UpdateWorldShape();
	
}

void PolygonCollider2D::UpdateWorldShape()
{
	Vec2 worldPos = m_rigidbody->GetWorldPosition();
	m_worldPosition = worldPos;
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

void PolygonCollider2D::DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor )
{
	//ctx->DrawPolygonFilled( *m_polygonLocal , fillColor );
	std::vector<Vertex_PCU> verts = m_verts;

	for ( int i = 0; i < verts.size(); i++ )
	{
		verts[ i ].m_color = fillColor;
	}

	TransformVertexArray( verts , 1.f , 0.f , m_worldPosition-m_localPosition );
	ctx->DrawVertexArray( verts );
	ctx->DrawPolygonUnfilled( *m_polygonLocal , borderColor , 0.2f );
	
}

void PolygonCollider2D::MarkForDestroy()
{
	m_isGarbage = true;
}

PolygonCollider2D::~PolygonCollider2D()
{

}

