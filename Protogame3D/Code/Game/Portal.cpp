
#include "Game/Portal.hpp"
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

Portal::Portal( EntityDef const& entityDef , Map* map ) :
	Entity( entityDef , map )
{
	m_animations = entityDef.m_animations;
	
	m_bounds = AABB2( Vec2( -0.5f , -0.5f ) , Vec2( 0.5f , 0.5f ) );
	m_bounds.Translate( Vec2( 0.f , 0.5f ) );

	m_debugMesh = new GPUMesh( g_theRenderer );
	//m_position = Vec2( 3.f , 3.f );
}


Portal::~Portal()
{
	delete m_debugMesh;
}

void Portal::Update( float deltaseconds )
{
	m_time += deltaseconds;

	m_transform.m_position = Vec3( m_position , 0.f );
	m_tranformMatrix = m_transform.ToMatrix();

	UpdateCameraFacingXY( *thegame->m_camera , m_tranformMatrix , Vec3( m_position , 0.f ) );
	UpdateSprites();
}

void Portal::Render()
{
	const Texture* tex = &m_spriteSheet->GetTexture();
	g_theRenderer->BindTexture( tex );
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

void Portal::DebugRender()
{
	g_theRenderer->CreateRasterState( WIRE_FRAME , CULL_NONE );
	std::vector<Vertex_PCU> debugVerts;

	g_theRenderer->BindTexture( nullptr );
	Vec3 start = Vec3( m_position , 0.f );
	Vec3 end = start + Vec3( 0.f , 0.f , m_height );

	AppendCyinder( debugVerts , start , end , m_physicsRadius , m_physicsRadius , Rgba8( 255 , 255 , 255 , 255 ) , Rgba8( 255 , 255 , 255 , 255 ) );

	m_debugMesh->UpdateVertices( debugVerts.size() , &debugVerts[ 0 ] );
	g_theRenderer->DrawMesh( m_debugMesh );
}

void Portal::UpdateSprites()
{	
	SpriteDefinition d = m_animations[ "animations" ]->GetSpriteDefAtTime(m_time);
	d.GetUVs( m_minUVs , m_maxUVs );
}
