#include "Game/SupportPlayer.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

SupportPlayer::SupportPlayer()
{

}

SupportPlayer::SupportPlayer( Game* game )
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.325f , -0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.325f , -0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.325f , 0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.325f , -0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.325f , 0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.325f , 0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_position = Vec2( 10.f , 10.f );
	m_game = game;

	m_aspectAtIdle = Vec2( 0.396f , 0.451f )*0.35f;
	m_aspectAtAttack = Vec2( 0.874f , 0.826f )*0.35f;
	m_aspectAtWalk = Vec2( 0.504f , 0.491f )*0.35f;

	m_aspectAtDisguiseWalk = Vec2(0.487f,0.552f)*0.35f;
	m_aspectAtDisguiseLure = Vec2(0.826f,0.772f)*0.35f;

	m_animClock = new Clock();
	m_animTimer = new Timer();
	m_animTimer->SetSeconds( m_animClock , 0.f );
}

SupportPlayer::~SupportPlayer()
{

}

void SupportPlayer::Update( float deltaseconds )
{
	m_time += deltaseconds;

	if ( m_time > 1000.f )
	{
		m_time = 0.f;
	}

	if ( !m_isActive )
	{
		m_currentState = IDLE_MELEE;
		return;
	}

	ToggleDisguise();
	MovePlayer( deltaseconds );
	Attack();
	Lure();

	if ( m_lockAnimState )
	{
		if ( m_animTimer->HasElapsed() )
		{
			m_lockAnimState = false;
		}
	}

	m_orientationDegrees = ( m_position - m_game->m_mousePosition ).GetNormalized().GetAngleDegrees() - 90.f;
}

void SupportPlayer::Render()
{
	Vertex_PCU vertCopy[ 6 ];
	ChangeAspectBasedOnState();
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );

	TransformArrayOnState( vertCopy );

	g_theRenderer->BindTexture( GetTexutreBasedOnState() );
	g_theRenderer->DrawVertexArray( 6 , vertCopy );
	g_theRenderer->BindTexture( nullptr );

	DebugRender();
}

void SupportPlayer::Die()
{
	m_isGarbage = true;
}

void SupportPlayer::DebugRender()
{

}

void SupportPlayer::MovePlayer( float deltaseconds )
{
	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		m_position.y += PLAYER_SPEED * deltaseconds;
		if ( !m_lockAnimState )
		{
			if ( !m_isDisguised )
			{
				m_currentState = MOVING_MELEE;
			}
			else
			{
				m_currentState = IDLE_GUN;
			}
		}
			
	}

	else if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		m_position.y -= PLAYER_SPEED * deltaseconds;
		if ( !m_lockAnimState )
		{
			if ( !m_isDisguised )
			{
				m_currentState = MOVING_MELEE;
			}
			else
			{
				m_currentState = IDLE_GUN;
			}
		}
	}

	else if ( g_theInput->IsKeyPressed( 'A' ) )
	{
		m_position.x -= PLAYER_SPEED * deltaseconds;
		if ( !m_lockAnimState )
		{
			if ( !m_isDisguised )
			{
				m_currentState = MOVING_MELEE;
			}
			else
			{
				m_currentState = IDLE_GUN;
			}
		}
	}

	else if ( g_theInput->IsKeyPressed( 'D' ) )
	{
		m_position.x += PLAYER_SPEED * deltaseconds;
		if ( !m_lockAnimState )
		{
			if ( !m_isDisguised )
			{
				m_currentState = MOVING_MELEE;
			}
			else
			{
				m_currentState = IDLE_GUN;
			}
		}
	}

	else
	{
		if ( !m_lockAnimState )
		{
			if ( !m_isDisguised )
			{
				m_currentState = IDLE_MELEE;
			}
			else
			{
				m_currentState = IDLE_GUN;
			}
		}
	}
}

void SupportPlayer::Attack()
{
	if ( m_isDisguised )
	{
		return;
	}


	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_currentState = ATTACKING_MELEE;
		m_lockAnimState = true;
		m_animTimer->SetSeconds( 1.f );
		m_animTimer->Reset();
		m_time = 0.f;
	}
}

void SupportPlayer::ToggleDisguise()
{
	if ( g_theInput->WasKeyJustPressed( 'E' ) )
	{
		m_isDisguised = !m_isDisguised;
	}
}

void SupportPlayer::Lure()
{
	if ( !m_isDisguised )
	{
		return;
	}

	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_currentState = ATTACKING_GUN;
		m_lockAnimState = true;
		m_animTimer->SetSeconds( 1.f );
		m_animTimer->Reset();
		m_time = 0.f;
	}
}

Texture* SupportPlayer::GetTexutreBasedOnState()
{
	switch ( m_currentState )
	{
	case IDLE_MELEE:
	{
		return m_game->m_supportPlayerIdleTex->GetSpriteTextureAtTime( m_time );
		break;
	}
	case MOVING_MELEE:
		return m_game->m_supportPlayerWalkTex->GetSpriteTextureAtTime( m_time );
		break;
	case ATTACKING_MELEE:
		return m_game->m_supportPlayerAttackTex->GetSpriteTextureAtTime( m_time );
		break;
	case IDLE_GUN:
		return m_game->m_supportPlayerDisguiseWalk->GetSpriteTextureAtTime( m_time );
		break;
	case ATTACKING_GUN:
		return m_game->m_supportPlayerDisguiseAttack->GetSpriteTextureAtTime( m_time );
		break;
	default:
		return nullptr;
		break;
	}
}

void SupportPlayer::ChangeAspectBasedOnState()
{
	switch ( m_currentState )
	{
	case IDLE_MELEE:
	{
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtIdle.x , -m_aspectAtIdle.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtIdle.x , -m_aspectAtIdle.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtIdle.x , m_aspectAtIdle.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtIdle.x , -m_aspectAtIdle.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtIdle.x , m_aspectAtIdle.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
		m_vertices[ 5 ] = Vertex_PCU( Vec3( -m_aspectAtIdle.x , m_aspectAtIdle.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
		break;
	}
	case MOVING_MELEE:
	{
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtWalk.x , -m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtWalk.x , -m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtWalk.x , m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtWalk.x , -m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtWalk.x , m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
		m_vertices[ 5 ] = Vertex_PCU( Vec3( -m_aspectAtWalk.x , m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
		break;
	}
	case ATTACKING_MELEE:
	{
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtAttack.x , -m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtAttack.x , -m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtAttack.x , m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtAttack.x , -m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtAttack.x , m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
		m_vertices[ 5 ] = Vertex_PCU( Vec3( -m_aspectAtAttack.x , m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
		break;
	}
	case ATTACKING_GUN:
	{
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtDisguiseLure.x , -m_aspectAtDisguiseLure.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtDisguiseLure.x , -m_aspectAtDisguiseLure.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtDisguiseLure.x , m_aspectAtDisguiseLure.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtDisguiseLure.x , -m_aspectAtDisguiseLure.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtDisguiseLure.x , m_aspectAtDisguiseLure.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
		m_vertices[ 5 ] = Vertex_PCU( Vec3( -m_aspectAtDisguiseLure.x , m_aspectAtDisguiseLure.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
		break;
	}
	case IDLE_GUN:
	{
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtDisguiseWalk.x , -m_aspectAtDisguiseWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtDisguiseWalk.x , -m_aspectAtDisguiseWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtDisguiseWalk.x , m_aspectAtDisguiseWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtDisguiseWalk.x , -m_aspectAtDisguiseWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtDisguiseWalk.x , m_aspectAtDisguiseWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
		m_vertices[ 5 ] = Vertex_PCU( Vec3( -m_aspectAtDisguiseWalk.x , m_aspectAtDisguiseWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
		break;
	}
	default:
		break;
	}
}

void SupportPlayer::TransformArrayOnState( Vertex_PCU verts[ 6 ] )
{
	switch ( m_currentState )
	{
	case IDLE_MELEE:
	{
		TransformVertexArray( 6 , verts , 3.6f , m_orientationDegrees , m_position );
		break;
	}
	case MOVING_MELEE:
	{
		TransformVertexArray( 6 , verts , 3.5f , m_orientationDegrees , m_position );
		break;
	}
	case ATTACKING_MELEE:
	{
		TransformVertexArray( 6 , verts , 3.7f , m_orientationDegrees , m_position );
		break;
	}
	case ATTACKING_GUN:
	{
		TransformVertexArray( 6 , verts , 3.5f , m_orientationDegrees , m_position );
		break;
	}
	case IDLE_GUN:
	{
		TransformVertexArray( 6 , verts , 3.5f , m_orientationDegrees , m_position );
		break;
	}
	default:
		break;
	}
}
