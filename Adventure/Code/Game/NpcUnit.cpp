#include "Game/NpcUnit.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"

NpcUnit::NpcUnit( Game* game , Vec2 position , ActorDefinition* definition ) :Entity( game , position )
{
	m_physicsRadius = ( float ) atof( definition->physicsRadius.c_str() );
	m_cosmeticRadius = ( float ) atof( definition->cosmeticRadius.c_str() );
	m_canFly = definition->canFly;
	m_canWalk = definition->canWalk;
	m_canSwim = definition->canSwim;
	m_name = definition->name;
	m_spriteAnimations = definition->m_spriteAnims;



	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/KushnariovaCharacters_12x53.png" );
	SpriteSheet* spriteSheet = new SpriteSheet( *texture , IntVec2( 12 , 53 ) );
	const SpriteDefinition& actorSprite = spriteSheet->GetSpriteDefinition( 55 );
	Vec2 uvMins;
	Vec2 uvMaxs;
	actorSprite.GetUVs( uvMins , uvMaxs );

	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , uvMins );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMaxs.x , uvMins.y ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMins.x , uvMaxs.y ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMaxs.x , uvMins.y ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , uvMaxs );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMins.x , uvMaxs.y ) );

	m_vertices_original[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , uvMins );
	m_vertices_original[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMaxs.x , uvMins.y ) );
	m_vertices_original[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMins.x , uvMaxs.y ) );

	m_vertices_original[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMaxs.x , uvMins.y ) );
	m_vertices_original[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , uvMaxs );
	m_vertices_original[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( uvMins.x , uvMaxs.y ) );

	m_orientationDegrees = 0.f;
}

void NpcUnit::Update( float deltaSeconds )
{
	RandomNumberGenerator rng = RandomNumberGenerator();
	
	m_directionChangeTimer -= deltaSeconds;
	if ( m_directionChangeTimer <= 0.f )
	{
		int newDir = rng.GetRandomIntInRange( 0 , 360 );
		m_orientationDegrees = ( float ) newDir;
		m_directionChangeTimer = 2.f;
	}

	MoveNpc( deltaSeconds );

}

void NpcUnit::Render()
{
	SpriteAnimDefinition* anims = m_spriteAnimations->GetDefinition( "Idle" );
	if ( m_orientationDegrees == 0.f )
	{
		anims = m_spriteAnimations->GetDefinition( "MoveEast" );
	}
	else if ( m_orientationDegrees >= 115.f && m_orientationDegrees <= 230.f )
	{
		anims = m_spriteAnimations->GetDefinition( "MoveWest" );
	}
	else if ( m_orientationDegrees > 10.f && m_orientationDegrees < 100.f )
	{
		anims = m_spriteAnimations->GetDefinition( "MoveEast" );
	}
	else if ( m_orientationDegrees >= 310.f && m_orientationDegrees <= 40.f )
	{
		anims = m_spriteAnimations->GetDefinition( "MoveNorth" );
	}
	else
	{
		anims = m_spriteAnimations->GetDefinition( "MoveSouth" );
	}

	const SpriteDefinition& currentDef = anims->GetSpriteDefAtTime( m_timeElapsedInSeconds );
	currentDef.GetUVs( m_uvMins , m_uvMaxs );
	ResetUvs();

	g_theRenderer->TransformVertexArray( 6 , m_vertices , 1.f , 0.f , m_position );
	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/KushnariovaCharacters_12x53.png" );
	g_theRenderer->BindTexture( texture );
	g_theRenderer->DrawVertexArray( 6 , m_vertices );
	g_theRenderer->BindTexture( nullptr );
	ResetVertices();
}

void NpcUnit::Die()
{
	m_isGarbage = true;
}

void NpcUnit::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}

void NpcUnit::ResetUvs()
{
	m_vertices[ 0 ].m_uvTexCoords = m_uvMins;
	m_vertices[ 1 ].m_uvTexCoords = Vec2( m_uvMaxs.x , m_uvMins.y );
	m_vertices[ 2 ].m_uvTexCoords = Vec2( m_uvMins.x , m_uvMaxs.y );
	m_vertices[ 3 ].m_uvTexCoords = Vec2( m_uvMaxs.x , m_uvMins.y );
	m_vertices[ 4 ].m_uvTexCoords = m_uvMaxs;
	m_vertices[ 5 ].m_uvTexCoords = Vec2( m_uvMins.x , m_uvMaxs.y );
}

void NpcUnit::MoveNpc( float deltaSeconds )
{
	m_position.x += CosDegrees( m_orientationDegrees ) * 1.f * deltaSeconds;
	m_position.y += SinDegrees( m_orientationDegrees ) * 1.f * deltaSeconds;
}

Strings NpcUnit::GetActorDetails()
{
	Strings toReturn;
	std::string temp1 = "true";
	std::string temp2 = "false";

	if ( m_canFly )
	{
		toReturn.push_back( "canFly: " + temp1 );
	}
	else
	{
		toReturn.push_back( "canFly: " + temp2 );
	}

	if ( m_canSwim )
	{
		toReturn.push_back( "canSwim: " + temp1 );
	}
	else
	{
		toReturn.push_back( "canSwim: " + temp2 );
	}

	if ( m_canWalk )
	{
		toReturn.push_back( "canWalk: " + temp1 );
	}
	else
	{
		toReturn.push_back( "canWalk: " + temp2 );
	}

	toReturn.push_back( m_name );

	return toReturn;
}
