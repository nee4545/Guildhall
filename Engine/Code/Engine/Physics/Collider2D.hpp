#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
class Physics2D;
class Rigidbody2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum COLLIDER2D_TYPE
{
	COLLIDER2D_DISC ,
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
	virtual bool Intersects( Collider2D const* other ) const = 0;

	virtual void DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor ) = 0;

protected:
	virtual ~Collider2D() {}; // private - make sure this is virtual so correct deconstructor gets called
	bool				m_isGarbage = false;

public:

	virtual void MarkForDestroy() =0;

public:
	COLLIDER2D_TYPE		m_colliderType;				// keep track of the type - will help with collision later
	Physics2D* m_system;                   // system who created or destr
	Rigidbody2D* m_rigidbody = nullptr;		// owning rigidbody, used for calculating world shape
};

