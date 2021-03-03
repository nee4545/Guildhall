#include "Game/OccupancyMapCollectable.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Math/MathUtils.hpp"

OccMapCollectable::OccMapCollectable(Game* game, Vec2 position )
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_game = game;
	m_position = position;
	LoadAnimations();
}

OccMapCollectable::~OccMapCollectable()
{

}

void OccMapCollectable::LoadAnimations()
{
	std::vector<Texture*> anims;
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Collectable/1.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Collectable/2.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Collectable/3.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Collectable/4.png" );

	anims.push_back( a1 );
	anims.push_back( a2 );
	anims.push_back( a3 );
	anims.push_back( a4 );

	m_anims = new SpriteAnimDefTex( 0 , anims.size() - 1 , 1.f );
	m_anims->m_animations = anims;
}

void OccMapCollectable::Update(float deltaSeconds)
{
	if ( m_isGarbage )
	{
		return;
	}

	m_time += deltaSeconds;

	if ( DoDiscsOverlap( m_position , 0.5f , m_game->m_player->m_position , 0.5f ) )
	{
		m_isGarbage = true;
		m_game->m_numObjectsToCollect -= 1;
	}
}

void OccMapCollectable::Render()
{
	Vertex_PCU vertCopy[ 6 ];
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );
	TransformVertexArray( 6 , vertCopy , 1.f , 0.f , m_position );
	g_theRenderer->BindTexture( m_anims->GetSpriteTextureAtTime( m_time ) );
	g_theRenderer->DrawVertexArray( 6 , vertCopy );
	g_theRenderer->BindTexture( nullptr );
}
