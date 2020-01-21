#include "Game/SpecialCollectable.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/Texture.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#define UNUSED(x) (void)(x);


SpecialCollectable::SpecialCollectable( Game* game , Vec2 position ) :Entity( game , position )
{
	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Extras_4x4.png" );
	SpriteSheet* spriteSheet = new SpriteSheet( *texture , IntVec2( 4 , 4 ) );
	const SpriteDefinition& boulder = spriteSheet->GetSpriteDefinition( 9 );
	Vec2 uvMins;
	Vec2 uvMaxs;
	boulder.GetUVs( uvMins , uvMaxs );

	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.35f , -0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , uvMins );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.35f , -0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMaxs.x , uvMins.y ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.35f , 0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMins.x , uvMaxs.y ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.35f , -0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMaxs.x , uvMins.y ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.35f , 0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , uvMaxs );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.35f , 0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMins.x , uvMaxs.y ) );

	m_vertices_original[ 0 ] = Vertex_PCU( Vec3( -0.35f , -0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , uvMins );
	m_vertices_original[ 1 ] = Vertex_PCU( Vec3( 0.35f , -0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMaxs.x , uvMins.y ) );
	m_vertices_original[ 2 ] = Vertex_PCU( Vec3( -0.35f , 0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMins.x , uvMaxs.y ) );

	m_vertices_original[ 3 ] = Vertex_PCU( Vec3( 0.35f , -0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMaxs.x , uvMins.y ) );
	m_vertices_original[ 4 ] = Vertex_PCU( Vec3( 0.35f , 0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , uvMaxs );
	m_vertices_original[ 5 ] = Vertex_PCU( Vec3( -0.35f , 0.35f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMins.x , uvMaxs.y ) );


	m_physicsRadius = 0.3f;
	m_cosmeticRadius = 0.4f;
}

void SpecialCollectable::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void SpecialCollectable::Render()
{
	g_theRenderer->TransformVertexArray( 6 , m_vertices , 1.f , m_orientationDegrees , m_position );
	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Extras_4x4.png" );
	g_theRenderer->BindTexture( texture );
	g_theRenderer->DrawVertexArray( 6 , m_vertices );
	g_theRenderer->BindTexture( nullptr );
	ResetVertices();
}

void SpecialCollectable::Die()
{
	m_isGarbage = true;
}

void SpecialCollectable::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}