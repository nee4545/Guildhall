#include "Game/Player.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

Player::Player( Game* game, Vec2 position ):Entity(game,position)
{
	m_tank[0]=Vertex_PCU(Vec3(-0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,0.f));
	m_tank[1]=Vertex_PCU(Vec3(0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,0.f));
	m_tank[2]=Vertex_PCU(Vec3(-0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,1.f));

	m_tank[3]=Vertex_PCU(Vec3(0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,0.f));
	m_tank[4]=Vertex_PCU(Vec3(0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,1.f));
	m_tank[5]=Vertex_PCU(Vec3(-0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,1.f));

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

	m_crosshair[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_crosshair[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_crosshair[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );
	  
	m_crosshair[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_crosshair[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_crosshair[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_crosshair_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_crosshair_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_crosshair_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );
			 
	m_crosshair_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_crosshair_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_crosshair_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	
	

	m_orientationDegrees=45.f;
	m_turretAngle=0.f;
	m_physicsRadius=0.2f;
	m_cosmeticRadius=0.4f;
	m_health =3;

	m_isPushedByWalls=true;
	m_isPushedByEntities=true;
	m_doesPushEntities=true;
	m_isHitByBullets=true;

	m_crosshairPosition = m_position;

}

void Player::Update( float deltaSeconds )
{
	if( m_isDead )
	{
		return;
	}

	RotateTank(deltaSeconds);
	RotateTurret(deltaSeconds);
	UpdateFromJoystick(deltaSeconds);
}

void Player::Render()
{
	if( m_isDead )
	{
		return;
	}

	g_theRenderer->TransformVertexArray(6,m_tank,1.f,m_orientationDegrees,m_position);
	Texture* tankBase=g_theRenderer->GetOrCreateTextureFromFile("Data/Images/PlayerTankBase.png");
	g_theRenderer->BindTexture(tankBase);
	g_theRenderer->DrawVertexArray(6,m_tank);
	g_theRenderer->BindTexture(nullptr);
	ResetTank();

	
	g_theRenderer->TransformVertexArray(6,m_turret,1.f,m_turretAngle+m_orientationDegrees,m_position);
	Texture* tankTurret=g_theRenderer->GetOrCreateTextureFromFile("Data/Images/PlayerTankTop.png");
	g_theRenderer->BindTexture(tankTurret);
	g_theRenderer->DrawVertexArray(6,m_turret);
	g_theRenderer->BindTexture(nullptr);
	ResetTurret();

	g_theRenderer->TransformVertexArray( 6, m_crosshair, 0.5f, m_orientationDegrees, m_crosshairPosition );
	Texture* crosshair=g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTurretBase.png" );
	g_theRenderer->BindTexture( crosshair );
	g_theRenderer->SetBlendMode(BlendMode::ADDITIVE);
	g_theRenderer->DrawVertexArray( 6, m_crosshair );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	ResestCrosshair();

	if( m_game->isDebugging )
	{
		DebugRender();
	}
}

void Player::Die()
{
	m_isGarbage = true;
}

void Player::MoveTank( float deltaSeconds )
{
	/*m_velocity=Vec2(CosDegrees(m_orientationDegrees),SinDegrees(m_orientationDegrees));
	m_position+=m_velocity*deltaSeconds;*/

	m_position.x+=CosDegrees(m_orientationDegrees)*m_speed*deltaSeconds;
	m_position.y+=SinDegrees(m_orientationDegrees)*m_speed*deltaSeconds;
}

void Player::RotateTurret( float deltaSeconds )
{
	if( g_theInput->IsKeyPressed( 'J' ) )
	{
		m_turretAngle=GetTurnedToward(m_turretAngle,180.f,100.f*deltaSeconds);
		
	}

	if( g_theInput->IsKeyPressed( 'L' ) )
	{
		m_turretAngle=GetTurnedToward(m_turretAngle,0.f,100.f*deltaSeconds);
	}

	if( g_theInput->IsKeyPressed( 'I' ) )
	{
		m_turretAngle=GetTurnedToward( m_turretAngle, 90.f, 100.f*deltaSeconds );
	}

	if( g_theInput->IsKeyPressed( 'K' ) )
	{
		m_turretAngle=GetTurnedToward( m_turretAngle, 270.f, 100.f*deltaSeconds );
	}
}

void Player::RotateTank( float deltaSeconds )
{
	if( g_theInput->IsKeyPressed( 'A' ) )
	{
		m_orientationDegrees=GetTurnedToward(m_orientationDegrees,180.f,180.f*deltaSeconds);
		
	}

	if( g_theInput->IsKeyPressed( 'D' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, 0.f, 180.f*deltaSeconds );
		
	}

	if( g_theInput->IsKeyPressed( 'W' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, 90.f, 180.f*deltaSeconds );
		
	}

	if( g_theInput->IsKeyPressed( 'S' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, 270.f, 180.f*deltaSeconds );
		
	}

	if( g_theInput->IsKeyPressed( 'D' ) && g_theInput->IsKeyPressed( 'W' ) )
	{
		m_orientationDegrees=GetTurnedToward(m_orientationDegrees,45.f,180.f*deltaSeconds);
	}

	if( g_theInput->IsKeyPressed( 'A' ) && g_theInput->IsKeyPressed( 'W' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, 135.f, 180.f*deltaSeconds );
	}

	if( g_theInput->IsKeyPressed( 'D' ) && g_theInput->IsKeyPressed( 'S' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, -45.f, 180.f*deltaSeconds );
	}

	if( g_theInput->IsKeyPressed( 'A' ) && g_theInput->IsKeyPressed( 'S' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, -135.f, 180.f*deltaSeconds );
	}

	if( g_theInput->IsKeyPressed( 'A' )||g_theInput->IsKeyPressed( 'W' )||g_theInput->IsKeyPressed( 'S' )||g_theInput->IsKeyPressed( 'D' ) )
	{
		MoveTank(deltaSeconds);
	}
	
}

void Player::UpdateFromJoystick( float deltaSeconds )
{
	const XboxController& controller= g_theInput->GetXboxController( 0 );
	
	const AnalogJoyStick& leftStick = controller.GetLeftJoystick();
	const AnalogJoyStick& rightStick = controller.GetRightJoystick();

	const KeyButtonState& a=controller.GetButtonState( XBOX_BUTTON_ID_A );
	const KeyButtonState& b=controller.GetButtonState( XBOX_BUTTON_ID_B );

	const KeyButtonState& u=controller.GetButtonState(XBOX_BUTTON_ID_DPAD_UP);
	const KeyButtonState& d=controller.GetButtonState(XBOX_BUTTON_ID_DPAD_DOWN);
	const KeyButtonState& l=controller.GetButtonState(XBOX_BUTTON_ID_DPAD_LEFT);
	const KeyButtonState& r=controller.GetButtonState(XBOX_BUTTON_ID_DPAD_RIGHT);
	
	if( !controller.IsCOnnected() )
	{
		return;
	}

	if( a.WasJustPressed() )
	{
		ShootBullet();
	}

	if( b.WasJustPressed() )
	{
		if(m_game->airStrikeCharges>0 )
		{ 
		m_game->world->current_map->CreateAirPlane(Vec2(m_crosshairPosition.x+10.f,m_crosshairPosition.y+1.f),m_crosshairPosition);
		m_game->airStrikeCharges--;
		}
	}

	
	float leftStickMagnitude = leftStick.GetMagnitude();
	if( leftStickMagnitude > 0.f )
	{
		m_orientationDegrees = GetTurnedToward(m_orientationDegrees,leftStick.GetAngleDegrees(),180.f*deltaSeconds);
		MoveTank(deltaSeconds);
		m_crosshairPosition = m_position;
	}

	float rightStickMagnitude=rightStick.GetMagnitude();
	if(rightStickMagnitude>0.f )
	{
		m_turretAngle= GetTurnedToward(m_turretAngle,rightStick.GetAngleDegrees()-m_orientationDegrees,360.f*deltaSeconds);
	}


	if( u.IsPressed() )
	{
		m_crosshairPosition.y+=3*deltaSeconds;
	}

	if( d.IsPressed() )
	{
		m_crosshairPosition.y-=3*deltaSeconds;
	}

	if( l.IsPressed() )
	{
		m_crosshairPosition.x-=3*deltaSeconds;
	}

	if( r.IsPressed() )
	{
		m_crosshairPosition.x+=3*deltaSeconds;
	}
	
	
	
}

void Player::ShootBullet()
{
	Vec2 offSet = Vec2(0.4f * CosDegrees(m_turretAngle+m_orientationDegrees),0.4f * SinDegrees(m_turretAngle+m_orientationDegrees));
	Vec2 bulletPosition = m_position + offSet; 

	m_game->world->current_map->SpawnBullet(bulletPosition,ENTITY_TYPE_GOOD_BULLET,m_turretAngle+m_orientationDegrees);
	m_game->world->current_map->CreateExplosions(0.5f,0.25f,bulletPosition);
	g_theAudio->PlaySound(PLAYER_SHOOT);
	
}

void Player::DebugRender()
{
	g_theRenderer->DrawRing(m_position,m_cosmeticRadius,Rgba8(255,0,255,255),0.02f);
	g_theRenderer->DrawRing(m_position,m_physicsRadius,Rgba8(0,255,255,255),0.02f);
}

void Player::ResetTank()
{
	for( int index=0; index<6; index++ )
	{
		m_tank[index]=m_tank_original[index];
	}
}

void Player::ResetTurret()
{
	for( int index=0; index<6; index++ )
	{
		m_turret[index]=m_turret_original[index];
	}
}

void Player::RespawnTank()
{
	if( m_game->currentLives>0 && m_health<=0 )
	{
		m_health = 3;
		m_isDead = false;
		m_isGarbage = false;
		//m_position = Vec2(2.5f,2.5f);
	}
}

void Player::ResestCrosshair()
{
	for( int index=0; index<6; index++ )
	{
		m_crosshair[index]=m_crosshair_original[index];
	}
}

