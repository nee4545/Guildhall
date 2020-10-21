#include "Game/NpcTank.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Engine/Audio/AudioSystem.hpp"

NpcTank::NpcTank( Game* game, Vec2 position ):Entity(game,position)
{
	m_tank[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_tank[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_tank[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_tank[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_tank[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_tank[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_tank_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_tank_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_tank_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_tank_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_tank_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_tank_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_turret[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_turret[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_turret[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_turret[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );


	m_turret_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_turret_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_turret_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_turret_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_orientationDegrees=0.f;

	m_physicsRadius=0.2f;
	m_cosmeticRadius=0.4f;

	m_isHitByBullets=true;
	m_isPushedByEntities=true;
	m_isPushedByWalls=true;
	m_doesPushEntities=true;

	m_health=2;
	m_lastSeenPosition = Vec2(0.f,0.f);
	lastSeenSet = false;
}

void NpcTank::Update( float deltaSeconds )
{
    
	Vec2 playerPosition = m_game->world->current_map->GetPlayerPosition();
	Vec2 displacement=playerPosition-m_position;
	
	gunCoolDownTimer -= deltaSeconds;
	positionTimer -= deltaSeconds;
	
	RandomNumberGenerator rng = RandomNumberGenerator();

	ray1 = m_game->world->current_map->Raycast( m_position, Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) ), 0.5f );
	ray2 = m_game->world->current_map->Raycast( m_position-Vec2( 0.3f*CosDegrees(m_orientationDegrees), 0.3f * SinDegrees(m_orientationDegrees) ).GetRotated90Degrees(), Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) ), 0.5f );
	ray3 = m_game->world->current_map->Raycast( m_position-Vec2( 0.3f*CosDegrees(m_orientationDegrees), 0.3f *SinDegrees(m_orientationDegrees) ).GetRotatedMinus90Degrees(), Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) ), 0.5f );

	if( positionTimer<=0.f )
	{
		positionBeforeASecond = m_position;
		positionTimer = 5.f;
	}
	
	
	if( m_game->world->current_map->HasLineOfSight( playerPosition, m_position, 10.f ) && m_game->world->current_map->IsPlayerAlive())
	{
		float goalDegrees = displacement.GetAngleDegrees();
		RotateTank(goalDegrees,deltaSeconds);
		m_lastSeenPosition = playerPosition;
		lastSeenSet = true;

		

		if( IsPointInForwardSector2D( playerPosition, m_position, m_orientationDegrees, 45.f, 10.f ) )
		{
			MoveTank(deltaSeconds);
		}

		if( IsPointInForwardSector2D( playerPosition, m_position, m_orientationDegrees, 10.f, 10.f ) )
		{
			if( gunCoolDownTimer<=0.f )
			{
				Shoot();
				gunCoolDownTimer=1.7f;
			}
		}
	}

	 else if( lastSeenSet )
	{
		float goalDegrees = (m_lastSeenPosition - m_position).GetAngleDegrees();
		
		RotateTank(goalDegrees,deltaSeconds);
		
		MoveTank(deltaSeconds);

		if( (int)m_position.GetLength() == (int)m_lastSeenPosition.GetLength() )
		{
			lastSeenSet = false;
		}
	}

	 else if(ray1.m_impactFraction<1.f || ray2.m_impactFraction<1.f || ray3.m_impactFraction<1.f )
	{
		
		direction = (float)rng.GetRandomIntInRange( 0, 360 );
		m_orientationDegrees = direction;	
		MoveTank( deltaSeconds );
	}
	 else
	{
		MoveTank(deltaSeconds);
	}
	  
	

	if( (positionBeforeASecond - m_position).GetLength() <= 0.001f )
	{
		directionChangeTimer -= deltaSeconds;
		if( directionChangeTimer <= 0.f )
		{
			direction = (float)rng.GetRandomIntInRange( 0, 360 );
			directionChangeTimer = 2.f;
		}
		RotateTank(direction,deltaSeconds);
		MoveTank( deltaSeconds );
		lastSeenSet = false;
	}

  

}

void NpcTank::Render()
{
	g_theRenderer->TransformVertexArray( 6, m_tank, 1.f, m_orientationDegrees, m_position );
	Texture* tankBase=g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/NemesisTankBase.png" );
	g_theRenderer->BindTexture( tankBase );
	g_theRenderer->DrawVertexArray( 6, m_tank );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->TransformVertexArray( 6, m_turret, 1.f, m_turretAngle+m_orientationDegrees, m_position );
	Texture* tankTurret=g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/NemesisTankTop.png" );
	g_theRenderer->BindTexture( tankTurret );
	g_theRenderer->DrawVertexArray( 6, m_turret );
	g_theRenderer->BindTexture( nullptr );

	
	
	ResetVertices();

	
	if( m_game->isDebugging )
	{
		DebugRender();
	}
	

}

void NpcTank::Die()
{
	m_isGarbage=true;
}

void NpcTank::ResetVertices()
{
	for( int index=0; index<6; index++ )
	{
		m_tank[index]=m_tank_original[index];
		m_turret[index]=m_turret_original[index];
	}
}

void NpcTank::DebugRender()
{
	g_theRenderer->DrawRing( m_position, m_cosmeticRadius, Rgba8( 255, 0, 255, 255 ), 0.02f );
	g_theRenderer->DrawRing( m_position, m_physicsRadius, Rgba8( 0, 255, 255, 255 ), 0.02f );

	if( lastSeenSet )
	{
		g_theRenderer->DrawLine( m_position, m_lastSeenPosition, Rgba8( 111, 0, 0, 255 ), 0.05f );
	}

	g_theRenderer->DrawLine( m_position, ray1.m_impactPosition, Rgba8( 0, 0, 100 ), 0.05f );
	g_theRenderer->DrawLine( m_position, ray2.m_impactPosition, Rgba8( 0, 0, 100 ), 0.05f );
	g_theRenderer->DrawLine( m_position, ray3.m_impactPosition, Rgba8( 0, 0, 100 ), 0.05f );
}

void NpcTank::RotateTank(float angleDegees,float deltaSeconds)
{
	m_orientationDegrees=GetTurnedToward(m_orientationDegrees,angleDegees,90.f*deltaSeconds);
}

void NpcTank::MoveTank( float deltaSeconds )
{
	m_position.x+=CosDegrees(m_orientationDegrees)*deltaSeconds*0.4f;
	m_position.y+=SinDegrees(m_orientationDegrees)*deltaSeconds*0.4f;
}

void NpcTank::Shoot()
{
	Vec2 offSet = Vec2( 0.4f * CosDegrees( m_orientationDegrees ), 0.4f * SinDegrees( m_orientationDegrees ) );
	Vec2 bulletPosition = m_position + offSet;

	m_game->world->current_map->SpawnBullet(bulletPosition,ENTITY_TYPE_EVIL_BULLET,m_orientationDegrees);
	g_theAudio->PlaySound(ENEMY_SHOOT);
}



