#include "Game/NpcTurret.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/World.hpp"
#include "Engine/Audio/AudioSystem.hpp"

NpcTurret::NpcTurret( Game* game, Vec2 position ):Entity(game,position)
{
	m_turret_base[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_turret_base[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_base[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );
	  
	m_turret_base[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_base[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_turret_base[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_turret_base_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_turret_base_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_base_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );
	  
	m_turret_base_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_base_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_turret_base_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_turret_top[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_turret_top[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_top[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );
			 
	m_turret_top[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_top[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_turret_top[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_turret_top_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_turret_top_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_top_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_turret_top_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turret_top_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_turret_top_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_orientationDegrees=0.f;

	m_physicsRadius=0.2f;
	m_cosmeticRadius=0.4f;
	m_isGarbage=false;

	m_isPushedByWalls=true;
	m_isPushedByEntities=false;
	m_doesPushEntities=true;
	m_isHitByBullets=true;

	m_health=2;
}

void NpcTurret::Update( float deltaSeconds )
{
	Vec2 playerPosition = m_game->world->current_map->GetPlayerPosition();
	Vec2 displacement=playerPosition-m_position;
	float angle= displacement.GetAngleDegrees();

	cooldownTimer-=deltaSeconds;

		if( m_game->world->current_map->HasLineOfSight( playerPosition, m_position, 10.f ) &&  m_game->world->current_map->IsPlayerAlive())
		{
				m_orientationDegrees=GetTurnedToward( m_orientationDegrees, angle, 90.f*deltaSeconds );
				m_lastSeenPosition = playerPosition;
				lastSeenSet = true;
				if( m_orientationDegrees==angle )
				{
					m_RayCastResult = m_game->world->current_map->Raycast( m_position, Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) ), displacement.GetLength() );
				}
				else
				{
					m_RayCastResult = m_game->world->current_map->Raycast( m_position, Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) ), 10.f );
				}

				if( IsPointInForwardSector2D( playerPosition, m_position, m_orientationDegrees, 10.f, 10.f ) )
				{
					if( cooldownTimer<=0.f )
					{
						Shoot();
					}
				}
		}

		else if(lastSeenSet )
		{
			Vec2 final_position = (-m_position+m_lastSeenPosition).GetRotatedDegrees(45.f);
			Vec2 initial_position = (-m_position+m_lastSeenPosition).GetRotatedDegrees(-45.f);

			if(!hasReachedInitial && !hasReachedFinal )
			{ 
			  goalDegrees = (initial_position ).GetAngleDegrees();
			}
		

			if( abs(GetShortestAngularDisplacement(m_orientationDegrees,goalDegrees)) <=1.f && !hasReachedFinal )
			{
				goalDegrees = (final_position).GetAngleDegrees();
				hasReachedFinal = true;
				hasReachedInitial = false;
			}

			if( abs( GetShortestAngularDisplacement( m_orientationDegrees, goalDegrees ) ) <=1.f && !hasReachedInitial )
			{
				goalDegrees = (initial_position ).GetAngleDegrees();
				hasReachedInitial = true;
				hasReachedFinal = false;
			}


			m_orientationDegrees = GetTurnedToward(m_orientationDegrees,goalDegrees,90.f*deltaSeconds);
			Vec2 laserPosition = m_position + Vec2(0.4f*CosDegrees(m_orientationDegrees),0.4f*SinDegrees(m_orientationDegrees));
			m_RayCastResult = m_game->world->current_map->Raycast(laserPosition,Vec2(CosDegrees(m_orientationDegrees),SinDegrees(m_orientationDegrees)),10.f);

		}
	
	else
	{
	    RotateTurret(deltaSeconds);
		Vec2 laserPosition = m_position + Vec2(0.4f*CosDegrees(m_orientationDegrees),0.4f*SinDegrees(m_orientationDegrees));
		m_RayCastResult = m_game->world->current_map->Raycast(laserPosition,Vec2(CosDegrees(m_orientationDegrees),SinDegrees(m_orientationDegrees)),10.f);
	}

	if( cooldownTimer<=0.f )
	{
		cooldownTimer=1.7f;
	}
	
}

void NpcTurret::Render()
{
	

	g_theRenderer->TransformVertexArray(6,m_turret_base,1.f,0.f,m_position);
	Texture* tankBase=g_theRenderer->GetOrCreateTextureFromFile("Data/Images/EnemyTurretBase.png");
	g_theRenderer->BindTexture(tankBase);
	g_theRenderer->DrawVertexArray(6,m_turret_base);
	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->TransformVertexArray( 6, m_turret_top, 1.f, m_orientationDegrees, m_position );
	Texture* tankTop=g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTurretTop.png" );
	g_theRenderer->BindTexture( tankTop );
	g_theRenderer->DrawVertexArray( 6, m_turret_top );
	g_theRenderer->BindTexture( nullptr );

	Vec2 laserPosition = m_position + Vec2(0.4f*CosDegrees(m_orientationDegrees),0.4f*SinDegrees(m_orientationDegrees));
	g_theRenderer->DrawLine(laserPosition,m_RayCastResult.m_impactPosition,Rgba8(100,0,0,255),0.03f);

	ResetVertices();

	if( m_game->isDebugging )
	{
		DebugRender();
	}

}

void NpcTurret::Die()
{
	m_isGarbage=true;
}

void NpcTurret::DebugRender()
{
	g_theRenderer->DrawRing( m_position, m_cosmeticRadius, Rgba8( 255, 0, 255, 255 ), 0.02f );
	g_theRenderer->DrawRing( m_position, m_physicsRadius, Rgba8( 0, 255, 255, 255 ), 0.02f );
}

void NpcTurret::ResetVertices()
{
	for( int index=0; index<6; index++ )
	{
		m_turret_base[index]=m_turret_base_original[index];
		m_turret_top[index]=m_turret_top_original[index];
	}
}

void NpcTurret::Shoot()
{
	Vec2 offSet = Vec2( 0.4f * CosDegrees( m_orientationDegrees ), 0.4f * SinDegrees( m_orientationDegrees ) );
	Vec2 bulletPosition = m_position + offSet;
	m_game->world->current_map->SpawnBullet(bulletPosition,ENTITY_TYPE_EVIL_BULLET,m_orientationDegrees);
	g_theAudio->PlaySound(ENEMY_SHOOT);
}

void NpcTurret::RotateTurret(float deltaSeconds)
{
	m_orientationDegrees=GetTurnedToward(m_orientationDegrees,m_orientationDegrees-90.f,30.f*deltaSeconds);
}
