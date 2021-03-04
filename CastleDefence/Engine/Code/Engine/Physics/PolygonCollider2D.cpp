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

void PolygonCollider2D::CalculateMoment()
{
	float areaOfPolygon = GetAreaOfPolygon( *m_polygonLocal );
	float moment =0.f;

	Vec2 commonVert = m_polygonLocal->m_points[ 0 ];

	int totalTriangles = ( int ) m_polygonLocal->m_points.size() - 2;
	int counter = 0;
	int i = 1;
	int j = 2;

	while ( counter < totalTriangles )
	{

		Vec2 nextVert1 = m_polygonLocal->m_points[ i ];
		Vec2 nextVert2 = m_polygonLocal->m_points[ j ];

		float areaOfTriangle = GetAreaOfTriangele( commonVert , nextVert1 , nextVert2 );
		moment += GetMomentOfInertiaOfTriangle( *m_polygonLocal, commonVert , nextVert1 , nextVert2 , ( areaOfTriangle / areaOfPolygon ) );

		i++;
		j++;
		counter++;
	}

	moment *= m_rigidbody->m_mass;
	moment /= areaOfPolygon;


	m_rigidbody->m_moment =moment;
}

void PolygonCollider2D::Rotate( float deltaRotation )
{
	Vec2 c = m_polygonLocal->GetCentre();
	for ( int index = 0; index < m_polygonLocal->m_points.size(); index++ )
	{
		m_polygonLocal->m_points[ index ] = RotateAroundArbitartPoint( c , m_polygonLocal->m_points[ index ] , deltaRotation );
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

