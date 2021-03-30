#include "Game/Bomb.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/SymmetricPotentialField.hpp"
#include "Game/InfluenceMap.hpp"

Bomb::Bomb( Game* game , Vec2 position , SpriteAnimDefinition* idle , SpriteAnimDefinition* explode )
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_explosionTimer = new Timer();
	m_explosionTimer->SetSeconds( 1.f );

	m_game = game;
	m_Idlesprite = idle;
	m_explosion = explode;
	m_position = position;

	m_symmetricPotentialField = new SymmetricPotentialField( game , IntVec2( RoundDownToInt( m_position.x ) , RoundDownToInt( m_position.y ) ),IntVec2(4,4) , 4.f , 1.f , false );
	m_symmetricPotentialField->Create();

	m_infmap = new InfluenceMap( game , IntVec2( RoundDownToInt( m_position.x ) , RoundDownToInt( m_position.y ) ) , IntVec2( 4 , 4 ) , 4.f );
	m_infmap->Create();
	
}

Bomb::~Bomb()
{

}

void Bomb::Update( float deltaseconds )
{
	m_time += deltaseconds;
	if ( m_isExploding )
	{
		if ( m_explosionTimer->HasElapsed() )
		{
			Die();
		}
	}
}

void Bomb::Render()
{
	if ( !m_isExploding )
	{
		SpriteDefinition def = m_Idlesprite->GetSpriteDefAtTime( m_time );
		def.GetUVs( m_minUV , m_maxUV );
		g_theRenderer->BindTexture( &def.GetTexture() );
		ResetVertices();
	}
	else
	{
		SpriteDefinition def = m_explosion->GetSpriteDefAtTime( m_time );
		def.GetUVs( m_minUV , m_maxUV );
		g_theRenderer->BindTexture( &def.GetTexture() );
		ResetVertices();
	}

	Vertex_PCU vertCopy[ 6 ];
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );

	TransformVertexArray( 6 , vertCopy , 2.f , 0.f , m_position );

	g_theRenderer->DrawVertexArray( 6 , vertCopy );

	g_theRenderer->BindTexture( nullptr );

	DebugRender();
}

void Bomb::Die()
{
	m_isGarbage = true;
}

void Bomb::DebugRender()
{
	std::vector<Vertex_PCU> potentialFieldVerts;

	/*for ( int i = 0; i < m_symmetricPotentialField->m_nodes.size(); i++ )
	{
		if ( m_symmetricPotentialField->m_nodes[ i ].value > 0.f )
		{
			AABB2 aabb = AABB2( m_symmetricPotentialField->m_nodes[ i ].coords.x , m_symmetricPotentialField->m_nodes[ i ].coords.y , m_symmetricPotentialField->m_nodes[ i ].coords.x + 1 , m_symmetricPotentialField->m_nodes[ i ].coords.y + 1 );
			AppendAABB2( potentialFieldVerts , aabb , Rgba8( 100 , 100 , 100 , 100 ) );
		}
	}*/

	m_symmetricPotentialField->DebugRender();

	g_theRenderer->DrawVertexArray( potentialFieldVerts );
}

void Bomb::Explode()
{
	m_time = 0.f;
	m_explosionTimer->Reset();
	m_isExploding = true;
}

void Bomb::ResetVertices()
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_minUV );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_maxUV );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );
}

