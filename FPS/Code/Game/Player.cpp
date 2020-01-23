#include "Game/Player.hpp"
#include "Engine/Math/MathUtils.hpp"

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

	m_orientationDegrees=45.f;
	
	m_physicsRadius=0.2f;
	m_cosmeticRadius=0.4f;
}

void Player::Update( float deltaSeconds )
{
	RotateTank(deltaSeconds);
	RotateTurret(deltaSeconds);
	UpdateFromJoystick(deltaSeconds);
}

void Player::Render()
{
	TransformVertexArrayTank(6,m_tank,1.f,m_orientationDegrees,m_position);
	Texture* tankBase=render->GetOrCreateTextureFromFile("Data/Images/PlayerTankBase.png");
	render->BindTexture(tankBase);
	render->DrawVertexArray(6,m_tank);
	render->BindTexture(nullptr);
	
	TransformVertexArrayTurret(6,m_turret,1.f,m_turretAngle,m_position);
	Texture* tankTurret=render->GetOrCreateTextureFromFile("Data/Images/PlayerTankTop.png");
	render->BindTexture(tankTurret);
	render->DrawVertexArray(6,m_turret);
	render->BindTexture(nullptr);

	if( m_game->isDebugging )
	{
		DebugRender();
	}
}

void Player::Die()
{

}

void Player::MoveTank( float deltaSeconds )
{
	m_velocity=Vec2(CosDegrees(m_orientationDegrees),SinDegrees(m_orientationDegrees));
	m_position+=m_velocity*deltaSeconds;
}

void Player::RotateTurret( float deltaSeconds )
{
	if( input->IsKeyPressed( 'J' ) )
	{
		m_turretAngle=GetTurnedToward(m_turretAngle,180.f,100.f*deltaSeconds);
		
	}

	if( input->IsKeyPressed( 'L' ) )
	{
		m_turretAngle=GetTurnedToward(m_turretAngle,0.f,100.f*deltaSeconds);
	}

	if( input->IsKeyPressed( 'I' ) )
	{
		m_turretAngle=GetTurnedToward( m_turretAngle, 90.f, 100.f*deltaSeconds );
	}

	if( input->IsKeyPressed( 'K' ) )
	{
		m_turretAngle=GetTurnedToward( m_turretAngle, 270.f, 100.f*deltaSeconds );
	}
}

void Player::RotateTank( float deltaSeconds )
{
	if( input->IsKeyPressed( 'A' ) )
	{
		m_orientationDegrees=GetTurnedToward(m_orientationDegrees,180.f,180.f*deltaSeconds);
		
	}

	if( input->IsKeyPressed( 'D' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, 0.f, 180.f*deltaSeconds );
		
	}

	if( input->IsKeyPressed( 'W' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, 90.f, 180.f*deltaSeconds );
		
	}

	if( input->IsKeyPressed( 'S' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, 270.f, 180.f*deltaSeconds );
		
	}

	if( input->IsKeyPressed( 'D' ) && input->IsKeyPressed( 'W' ) )
	{
		m_orientationDegrees=GetTurnedToward(m_orientationDegrees,45.f,180.f*deltaSeconds);
	}

	if( input->IsKeyPressed( 'A' ) && input->IsKeyPressed( 'W' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, 135.f, 180.f*deltaSeconds );
	}

	if( input->IsKeyPressed( 'D' ) && input->IsKeyPressed( 'S' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, -45.f, 180.f*deltaSeconds );
	}

	if( input->IsKeyPressed( 'A' ) && input->IsKeyPressed( 'S' ) )
	{
		m_orientationDegrees=GetTurnedToward( m_orientationDegrees, -135.f, 180.f*deltaSeconds );
	}

	if( input->IsKeyPressed( 'A' )||input->IsKeyPressed( 'W' )||input->IsKeyPressed( 'S' )||input->IsKeyPressed( 'D' ) )
	{
		MoveTank(deltaSeconds);
	}

	
}

void Player::UpdateFromJoystick( float deltaSeconds )
{
	const XboxController& controller= input->GetXboxController( 0 );
	
	const AnalogJoyStick& leftStick = controller.GetLeftJoystick();
	const AnalogJoyStick& rightStick = controller.GetRightJoystick();



	if( !controller.IsCOnnected() )
	{
		return;
	}

	
	float leftStickMagnitude = leftStick.GetMagnitude();
	if( leftStickMagnitude > 0.f )
	{
		m_orientationDegrees = GetTurnedToward(m_orientationDegrees,leftStick.GetAngleDegrees(),180.f*deltaSeconds);
		MoveTank(deltaSeconds);
	}

	float rightStickMagnitude=rightStick.GetMagnitude();
	if(rightStickMagnitude>0.f )
	{
		m_turretAngle= GetTurnedToward(m_turretAngle,rightStick.GetAngleDegrees(),180.f*deltaSeconds);
	}
	
}

void Player::TransformVertexArrayTank( int num_vertices, Vertex_PCU* vert, float scale, float rotationDegrees, Vec2 translation )
{
	for( int index=0; index<num_vertices; index++ )
	{
		vert[index].m_position=TransformPosition3DXY( m_tank_original[index].m_position, scale, rotationDegrees, m_position );
	}
	
}

void Player::TransformVertexArrayTurret( int num_vertices, Vertex_PCU* vert, float scale, float rotationDegrees, Vec2 translation )
{
	for( int index=0; index<num_vertices; index++ )
	{
		vert[index].m_position=TransformPosition3DXY( m_turret_original[index].m_position, scale, rotationDegrees, m_position );
	}
}

void Player::DebugRender()
{
	render->DrawRing(m_position,m_cosmeticRadius,Rgba8(255,0,255,255),0.02f);
	render->DrawRing(m_position,m_physicsRadius,Rgba8(0,255,255,255),0.02f);
}

