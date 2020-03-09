#pragma once
#include <vector>
#include <Engine/Physics/Collider2D.hpp>
#include <Engine/Math/Vec2.hpp>
#include "Engine/Core/Vertex_PCU.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
class Physics2D;
class Rigidbody2D;


//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D : public Collider2D
{
public:
	DiscCollider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType , Vec2 localPosition , float radius = 1.f );
	DiscCollider2D( Vec2 localPosition , float radius );

	virtual void UpdateWorldShape() override; // UpdateShape world location when the rigidbody moves.

	virtual Vec2 GetClosestPoint( Vec2 pos ) const override;
	virtual bool Contains( Vec2 pos ) const override;
	virtual void CalculateMoment();
	

	virtual void DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor ) override;

private:

	void MarkForDestroy() override;

public:
	Vec2	m_localPosition = Vec2(0.f,0.f); // my local offset from my parent
	Vec2	m_worldPosition = Vec2(0.f,0.f); // calculated from local position and owning rigidbody if present
	std::vector<Vertex_PCU> m_verts;

	
	float	m_radius =0;

protected:
	 ~DiscCollider2D();
	
};

//---------------