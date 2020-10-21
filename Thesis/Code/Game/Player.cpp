#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Game/Game.hpp"

Player::Player()
{


}

Player::Player( Game* game )
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.325f , -0.328f , 0.f )  , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.325f , -0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.325f , 0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.325f , -0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.325f , 0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.325f , 0.328f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_position = Vec2( 10.f , 10.f );
	m_game = game;

	m_aspectAtIdle = Vec2( 0.325f , 0.328f )*0.35f;
	m_aspectAtAttack = Vec2(0.438f,0.342f)*0.35f;
	m_aspectAtWalk = Vec2(0.608f,0.481f)*0.35f;

	m_aspectAtIdleGun = Vec2(0.285f,0.353f)*0.35f;
	m_aspectAtAttackGun = Vec2(0.334f,0.579f)*0.35f;
	m_aspectAtWalkGun = Vec2( 0.271f , 0.351f )*0.35f;

	m_animClock = new Clock();
	m_animTimer = new Timer();
	m_animTimer->SetSeconds( m_animClock , 0.f );

	m_health = 10.f;
}

Player::~Player()
{

}

void Player::Update( float deltaseconds )
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

	ToggleMeleeState();
	MovePlayer( deltaseconds );
	MeleeAttack();
	Shoot();

	if ( m_lockAnimState )
	{
		if ( m_animTimer->HasElapsed() )
		{
			m_lockAnimState = false;
		}
	}



	m_orientationDegrees = ( m_position - m_game->m_mousePosition ).GetNormalized().GetAngleDegrees()-90.f;
}

void Player::Render()
{
	Vertex_PCU vertCopy[ 6 ];
	ChangeAspectBasedOnState();
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );

	TransformArrayOnState( vertCopy );

	g_theRenderer->BindTexture( GetTexutreBasedOnState() );
	g_theRenderer->DrawVertexArray(6, vertCopy );
	g_theRenderer->BindTexture( nullptr );

	//DebugRender();
}

void Player::Die()
{
	m_isGarbage = true;
}

void Player::DebugRender()
{
	g_theRenderer->DrawRing( m_position , 0.5f , Rgba8( 100 , 0 , 0 , 255 ) , 0.1f );
}

void Player::TakeDamage( float damage )
{
	m_health -= damage;
	if ( m_health <= 0.f )
	{
		Die();
	}
}

void Player::MovePlayer(float deltaseconds)
{
	

	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		m_position.y += PLAYER_SPEED * deltaseconds;
		if ( !m_lockAnimState )
		{
			if ( m_isMelee )
			{
				m_currentState = MOVING_MELEE;
			}
			else
			{
				m_currentState = MOVING_GUN;
			}
		}
	}

	else if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		m_position.y -= PLAYER_SPEED * deltaseconds;
		if ( !m_lockAnimState )
		{
			if ( m_isMelee )
			{
				m_currentState = MOVING_MELEE;
			}
			else
			{
				m_currentState = MOVING_GUN;
			}
		}
	}

	else if ( g_theInput->IsKeyPressed( 'A' ) )
	{
		m_position.x -= PLAYER_SPEED * deltaseconds;
		if ( !m_lockAnimState )
		{
			if ( m_isMelee )
			{
				m_currentState = MOVING_MELEE;
			}
			else
			{
				m_currentState = MOVING_GUN;
			}
		}
	}

	else if ( g_theInput->IsKeyPressed( 'D' ) )
	{
		m_position.x += PLAYER_SPEED * deltaseconds;
		if ( !m_lockAnimState )
		{
			if ( m_isMelee )
			{
				m_currentState = MOVING_MELEE;
			}
			else
			{
				m_currentState = MOVING_GUN;
			}
		}
	}

	else
	{
		if ( !m_lockAnimState )
		{
			if ( m_isMelee )
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

void Player::ToggleMeleeState()
{
	if ( g_theInput->WasKeyJustPressed( 'E' ) )
	{
		m_isMelee = !m_isMelee;
	}
}

void Player::Shoot()
{
	if ( m_isMelee )
	{
		return;
	}

	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_currentState = ATTACKING_GUN;
		m_lockAnimState = true;
		m_animTimer->SetSeconds( 1.f );
		m_animTimer->Reset();
	}


}

void Player::MeleeAttack()
{
	if ( m_isMelee )
	{
		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_currentState = ATTACKING_MELEE;
			m_lockAnimState = true;
			m_animTimer->SetSeconds( 1.f );
			m_animTimer->Reset();
		}
	}
}

Texture* Player::GetTexutreBasedOnState()
{
	switch ( m_currentState )
	{
	case IDLE_MELEE:
	{
		return m_game->m_player1IdleMeleeTex->GetSpriteTextureAtTime( m_time );
		break;
	}
	case MOVING_MELEE:
		return m_game->m_player1WalkMeleeTex->GetSpriteTextureAtTime( m_time );
		break;
	case ATTACKING_MELEE:
		return m_game->m_player1AttackTex->GetSpriteTextureAtTime( m_time );
		break;
	case  IDLE_GUN:
		return m_game->m_player1IdleGunTex->GetSpriteTextureAtTime( m_time );
		break;
	case  ATTACKING_GUN:
		return m_game->m_player1AttackGunTex->GetSpriteTextureAtTime( m_time );
		break;
	case  MOVING_GUN:
		return m_game->m_player1WalkGunTex->GetSpriteTextureAtTime( m_time );
		break;
	default:
		return nullptr;
		break;
	}

}

void Player::ChangeAspectBasedOnState()
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
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtWalk.x ,-m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtWalk.x , -m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtWalk.x , m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtWalk.x  , -m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtWalk.x  , m_aspectAtWalk.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
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
	case IDLE_GUN:
	{
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtIdleGun.x , -m_aspectAtIdleGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtIdleGun.x , -m_aspectAtIdleGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtIdleGun.x , m_aspectAtIdleGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtIdleGun.x , -m_aspectAtIdleGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtIdleGun.x , m_aspectAtIdleGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
		m_vertices[ 5 ] = Vertex_PCU( Vec3( -m_aspectAtIdleGun.x , m_aspectAtIdleGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
		break;
	}
	case ATTACKING_GUN:
	{
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtAttackGun.x , -m_aspectAtAttackGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtAttackGun.x , -m_aspectAtAttackGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtAttackGun.x , m_aspectAtAttackGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtAttackGun.x , -m_aspectAtAttackGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtAttackGun.x , m_aspectAtAttackGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
		m_vertices[ 5 ] = Vertex_PCU( Vec3( -m_aspectAtAttackGun.x , m_aspectAtAttackGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
		break;
	}
	case MOVING_GUN:
	{
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtWalkGun.x , -m_aspectAtWalkGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtWalkGun.x , -m_aspectAtWalkGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtWalkGun.x , m_aspectAtWalkGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtWalkGun.x , -m_aspectAtWalkGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtWalkGun.x , m_aspectAtWalkGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
		m_vertices[ 5 ] = Vertex_PCU( Vec3( -m_aspectAtWalkGun.x , m_aspectAtWalkGun.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
		break;
	}
	default:
	{
		m_vertices[ 0 ] = Vertex_PCU( Vec3( -m_aspectAtAttack.x , -m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
		m_vertices[ 1 ] = Vertex_PCU( Vec3( m_aspectAtAttack.x , -m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 2 ] = Vertex_PCU( Vec3( -m_aspectAtAttack.x , m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

		m_vertices[ 3 ] = Vertex_PCU( Vec3( m_aspectAtAttack.x , -m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
		m_vertices[ 4 ] = Vertex_PCU( Vec3( m_aspectAtAttack.x , m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
		m_vertices[ 5 ] = Vertex_PCU( Vec3( -m_aspectAtAttack.x , m_aspectAtAttack.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
		break;
	}
		
	}
}

void Player::TransformArrayOnState( Vertex_PCU verts[ 6 ] )
{
	switch ( m_currentState )
	{
	case IDLE_MELEE:
	{
		TransformVertexArray( 6 , verts , 4.1f , m_orientationDegrees , m_position );
		break;
	}
	case MOVING_MELEE:
	{
		TransformVertexArray( 6 , verts , 3.f , m_orientationDegrees , m_position );
		break;
	}
	case ATTACKING_MELEE:
	{
		TransformVertexArray( 6 , verts , 6.f , m_orientationDegrees , m_position );
		break;
	}
	case IDLE_GUN:
	{
		TransformVertexArray( 6 , verts , 4.f , m_orientationDegrees , m_position );
		break;
	}
	case ATTACKING_GUN:
	{
		TransformVertexArray( 6 , verts , 4.f , m_orientationDegrees , m_position );
		break;
	}
	case  MOVING_GUN:
	{
		TransformVertexArray( 6 , verts , 4.f , m_orientationDegrees , m_position );
		break;
	}

	default:
	{
		TransformVertexArray( 6 , verts , 6.f , m_orientationDegrees , m_position );
		break;
	}
	}
}
