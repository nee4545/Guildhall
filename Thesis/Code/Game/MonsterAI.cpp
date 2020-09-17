#include "Game/MonsterAI.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Input/InputSystem.hpp"

MonsterAI::MonsterAI( Game* game , AI_TYPE type , SpriteAnimDefinition* idleAnims , SpriteAnimDefinition* walkAnims , SpriteAnimDefinition* meleeAttackAnims )
{
	m_game = game;
	m_currentType = type;
	m_animationAtIdle = idleAnims;
	m_animationAtWalk = walkAnims;
	m_animationAtMeleeAttack = meleeAttackAnims;

	m_aspectAtWalk = Vec2( 0.65f , 0.65f );
	m_aspectAtMeleeAttack = Vec2( 1.f , 1.f );
	SetVerticesBasedOnAspect( Vec2( 1.f , 1.f ) );

}

MonsterAI::MonsterAI( Game* game , AI_TYPE type , SpriteAnimDefinition* idleAnims , SpriteAnimDefinition* walkAnims , SpriteAnimDefinition* meleeAttackAnims , SpriteAnimDefinition* rangeAttack1Anims )
{

}

MonsterAI::MonsterAI( Game* game , AI_TYPE type , SpriteAnimDefinition* idleAnims , SpriteAnimDefinition* walkAnims , SpriteAnimDefinition* meleeAttackAnims , SpriteAnimDefinition* rangeAttack1Anims , SpriteAnimDefinition* rangeAttack2Anims )
{

}

MonsterAI::~MonsterAI()
{

}

void MonsterAI::Update( float deltaseconds )
{
	m_time += deltaseconds;
	if ( g_theInput->WasKeyJustPressed( 'C' ) )
	{
		m_currentState = MELEE_ATTACK;
	}
	if ( g_theInput->WasKeyJustPressed( 'V' ) )
	{
		m_currentState = MOVING;
	}
}

void MonsterAI::Render()
{
	switch ( m_currentState )
	{
	case IDLE:
	{
		g_theRenderer->BindTexture( nullptr );
		break;
	}
	case MOVING:
	{
		SpriteDefinition def = m_animationAtWalk->GetSpriteDefAtTime( m_time );
		def.GetUVs( m_minUV , m_maxUV );
		g_theRenderer->BindTexture(&def.GetTexture());
		SetVerticesBasedOnAspect( m_aspectAtWalk );
		break;
	}
	case MELEE_ATTACK:
	{
		SpriteDefinition def = m_animationAtMeleeAttack->GetSpriteDefAtTime( m_time );
		def.GetUVs( m_minUV , m_maxUV );
		g_theRenderer->BindTexture( &def.GetTexture() );
		SetVerticesBasedOnAspect( m_aspectAtMeleeAttack );
		break;
	}
	case RANGE_ATTACK_1:
		break;
	case RANGE_ATTACK_2:
		break;
	case DEAD:
		break;
	default:
		break;
	}

	Vertex_PCU vertCopy[ 6 ];
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );

	TransformVertexArray( 6 , vertCopy , 1.f , m_orientationDegrees , m_position );

	g_theRenderer->DrawVertexArray( 6 , vertCopy );

}

void MonsterAI::DebugRender()
{

}

void MonsterAI::Die()
{
	m_isGarbage = true;
}

void MonsterAI::SetVerticesBasedOnAspect( Vec2 aspect )
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_minUV );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_maxUV );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );
}
