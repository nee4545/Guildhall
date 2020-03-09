#pragma once
#include <Engine/Physics/Collider2D.hpp>
#include <Engine/Math/Vec2.hpp>
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Polygon2D.hpp"
//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
class Physics2D;
class Rigidbody2D;
class Polygon2D;


class PolygonCollider2D : public Collider2D
{
public:
	PolygonCollider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType , Vec2 localPosition );
	PolygonCollider2D( Vec2 localPosition , Polygon2D* polygon );

	virtual void UpdateWorldShape() override; // UpdateShape world location when the rigidbody moves.

	virtual Vec2 GetClosestPoint( Vec2 pos ) const override;
	virtual bool Contains( Vec2 pos ) const override;
	virtual void CalculateMoment();
	void Rotate( float deltaRotation );
	/*virtual bool Intersects( Collider2D const* other ) const override;*/

	virtual void DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor ) override;

private:

	void MarkForDestroy() override;

public:
	Vec2	m_localPosition = Vec2( 0.f , 0.f ); // my local offset from my parent
	Vec2	m_worldPosition = Vec2( 0.f , 0.f ); // calculated from local position and owning rigidbody if present
	Polygon2D* m_polygonLocal;
	float   boundingDiscRadius = 0.f;
	

protected:
	~PolygonCollider2D() ;
};