#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Physics/PhysicsMaterial.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
class Physics2D;
class Rigidbody2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum COLLIDER2D_TYPE
{
	COLLIDER2D_DISC ,
	COLLIDER2D_POLYGON ,
	NUM_COLLIDER_TYPES
};



//--------------------------------------------------------------------------------------------------------------------------------------------

class Collider2D
{
	friend class Physics2D;

public:

	virtual void UpdateWorldShape() = 0;
	void Destroy();

	virtual Vec2 GetClosestPoint( Vec2 pos ) const = 0;
	virtual bool Contains( Vec2 pos ) const = 0;
	virtual void CalculateMoment() = 0;
	bool Intersects( Collider2D const* other ) const;

	float GetRestitutionWith( Collider2D const* other ) const;
	float GetFrictionWith( Collider2D const* other ) const;

	virtual void DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor ) = 0;

	void IncreamentBounciness( float increament );
	void DecreamentBounciness( float decreament );

	void IncreamentFriction( float increament );
	void DecreamentFriction( float decreament );

protected:
	virtual ~Collider2D() {}; // private - make sure this is virtual so correct deconstructor gets called
	bool				m_isGarbage = false;

public:

	virtual void MarkForDestroy() =0;

public:
	COLLIDER2D_TYPE		m_colliderType;				// keep track of the type - will help with collision later
	Physics2D* m_system;                   // system who created or destr
	Rigidbody2D* m_rigidbody = nullptr;		// owning rigidbody, used for calculating world shape
	PhysicsMaterial m_material;
};


typedef bool ( *collision_check_cb )( Collider2D const* , Collider2D const* );

bool DiscVPolygonCollisionCheck( Collider2D const* col0 , Collider2D const* col1 );
bool PolygonVDiscCollisionCheck( Collider2D const* col0 , Collider2D const* col1 );
bool DiscVsDiscCollisionCheck( Collider2D const* col0 , Collider2D const* col1 );
bool PolygonVPolygonCollisionCheck( Collider2D const* col0 , Collider2D const* col1 );




 
