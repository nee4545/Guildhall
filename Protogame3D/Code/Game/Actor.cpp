#include "Game/Actor.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/GameCommon.hpp"
#include "Game/BillBoard.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Texture.hpp"
#include "Game/EntityDef.hpp"

Actor::Actor( Vec2 position ):Entity(position)
{
	m_mesh = new GPUMesh( g_theRenderer );
	m_debugMesh = new GPUMesh( g_theRenderer );
	m_bounds = AABB2( Vec2( -1.f , -0.f ) , Vec2( 1.f , 1.5f ) );
}

Actor::Actor( EntityDef const& entityDef , Map* map ) :
	Entity( entityDef , map )
{
	m_mesh = new GPUMesh( g_theRenderer );
	m_debugMesh = new GPUMesh( g_theRenderer );
	m_bounds = AABB2( Vec2( -1.f , -0.f ) , Vec2( 1.f , 1.5f ) );
	m_def = const_cast<EntityDef*>(&entityDef);
}

Actor::Actor()
{
	m_mesh = new GPUMesh( g_theRenderer );
	m_debugMesh = new GPUMesh( g_theRenderer );
	m_bounds = AABB2( Vec2( -1.f , -0.f ) , Vec2( 1.f , 1.f ) );
}

Actor::~Actor()
{
	delete m_mesh;
	delete m_debugMesh;
}

void Actor::Update( float deltaseconds )
{
	m_time += deltaseconds;

	if ( thegame->m_player == this )
	{
		return;
	}
	//m_position.x += deltaseconds;
	m_transform.m_position = Vec3( m_position , 0.f );
	m_tranformMatrix = m_transform.ToMatrix();
	UpdateCameraFacingXY( *thegame->m_camera , m_tranformMatrix , Vec3( m_position , 0.f ) );
	UpdateSprites();
}

void Actor::Render()
{
	if ( thegame->m_player == this )
	{
		return;
	}


	const Texture* tex = &m_spriteSheet->GetTexture();
	g_theRenderer->BindTexture(tex);
	std::vector<Vertex_PCU> verts;

	g_theRenderer->SetModalMatrix( m_tranformMatrix );
	AppendAABB2( verts , m_bounds , Rgba8( 255 , 255 , 255 , 255 ) , m_minUVs , m_maxUVs );
	g_theRenderer->DrawVertexArray( verts );

	g_theRenderer->SetModalMatrix( Mat44() );

	if ( thegame->m_debugDrawOn )
	{
		DebugRender();
	}
}

void Actor::DebugRender()
{
	g_theRenderer->CreateRasterState( WIRE_FRAME , CULL_NONE );
	std::vector<Vertex_PCU> debugVerts;

	g_theRenderer->BindTexture( nullptr );
	Vec3 start = Vec3( m_position , 0.f );
	Vec3 end = start + Vec3(0.f,0.f,m_height);

	AppendCyinder( debugVerts , start , end , m_physicsRadius , m_physicsRadius , Rgba8( 255 , 255 , 255 , 255 ) , Rgba8( 255 , 255 , 255 , 255 ) );

	m_debugMesh->UpdateVertices( debugVerts.size() , &debugVerts[ 0 ] );
	g_theRenderer->DrawMesh( m_debugMesh );

	//g_theRenderer->CreateRasterState( SOLID , CULL_BACK );

	std::vector<Vertex_PCU> debugVerts2;

	start = Vec3( m_position , 0.5f );
	end = start + Vec3( m_forwardVector , 0.f )*0.3f;

	AppendArrow( debugVerts2 , start , end , 0.02f , 0.01f , 0.02f , Rgba8( 0 , 0 , 100 , 255 ) , Rgba8( 0 , 0 , 100 , 255 ) );
	g_theRenderer->CreateRasterState( SOLID , CULL_NONE );
	g_theRenderer->DrawVertexArray( debugVerts2 );
	g_theRenderer->CreateRasterState( SOLID , CULL_BACK );


}

void Actor::Die()
{

}

void Actor::UpdateSprites()
{

	Vec3 vec = thegame->m_camera->m_transform.m_position;

	Vec2 dispVec = Vec2( vec.x , vec.y ) - m_position;

	Vec2 forwardVec = (m_forwardVector);
	Vec2 backVec = -forwardVec;
	Vec2 rightVec = forwardVec.GetRotated90Degrees();
	Vec2 leftVec = forwardVec.GetRotatedMinus90Degrees();
	Vec2 forwardRightVec = forwardVec.GetRotatedDegrees( 45.f );
	Vec2 forwardLeftVec = forwardVec.GetRotatedDegrees( -45.f );
	Vec2 backRightVec = forwardVec.GetRotatedDegrees( 135.f );
	Vec2 backLeftVec = forwardVec.GetRotatedDegrees( -135.f );

	float dotProduct1 = DotProduct2D( forwardVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct2 = DotProduct2D( backVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct3 = DotProduct2D( rightVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct4 = DotProduct2D( leftVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct5 = DotProduct2D( forwardRightVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct6 = DotProduct2D( forwardLeftVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct7 = DotProduct2D( backRightVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct8 = DotProduct2D( backLeftVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	
	float min = dotProduct1;

	if ( min < dotProduct2 )
	{
		min = dotProduct2;
	}

	if ( min < dotProduct3 )
	{
		min = dotProduct3;
	}

	if ( min < dotProduct4 )
	{
		min = dotProduct4;
	}

	if ( min < dotProduct5 )
	{
		min = dotProduct5;
	}

	if ( min < dotProduct6 )
	{
		min = dotProduct6;
	}

	if ( min < dotProduct7 )
	{
		min = dotProduct7;
	}

	if ( min < dotProduct8 )
	{
		min = dotProduct8;
	}


	if ( min == dotProduct1 )
	{
		SpriteAnimDefinition* def = m_entityAnims[ ENTITY_WALK_ANIM ][ "front" ];
		if ( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_minUVs , m_maxUVs );
		}
	}

	if ( min == dotProduct2 )
	{
		SpriteAnimDefinition* def = m_entityAnims[ ENTITY_WALK_ANIM ][ "back" ];
		if ( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_minUVs , m_maxUVs );
		}
	}

	if ( min == dotProduct3 )
	{
		SpriteAnimDefinition* def = m_entityAnims[ ENTITY_WALK_ANIM ][ "left" ];
		if ( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_minUVs , m_maxUVs );
		}
	}


	if ( min == dotProduct4 )
	{
		SpriteAnimDefinition* def = m_entityAnims[ ENTITY_WALK_ANIM ][ "right" ];
		if ( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_minUVs , m_maxUVs );
		}
	}

	if ( min == dotProduct5 )
	{
		SpriteAnimDefinition* def = m_entityAnims[ ENTITY_WALK_ANIM ][ "frontLeft" ];
		if ( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_minUVs , m_maxUVs );
		}
	}

	if ( min == dotProduct6 )
	{
		SpriteAnimDefinition* def = m_entityAnims[ ENTITY_WALK_ANIM ][ "frontRight" ];
		if ( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_minUVs , m_maxUVs );
		}
	}

	if ( min == dotProduct7 )
	{
		SpriteAnimDefinition* def = m_entityAnims[ ENTITY_WALK_ANIM ][ "backLeft" ];
		if ( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_minUVs , m_maxUVs );
		}
	}


	if ( min == dotProduct8 )
	{
		SpriteAnimDefinition* def = m_entityAnims[ ENTITY_WALK_ANIM ][ "backRight" ];
		if ( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_minUVs , m_maxUVs );
		}
	}

}
