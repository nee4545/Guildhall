#include "PlayerShip.hpp"
#include "Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/XboxController.hpp"
//#include "Engine/Input/AnalogJoyStick.hpp"

void PlayerShip::AcclerateShip(float deltaseconds)
{
	m_velocity.x=(m_velocity.x) + 30.f*deltaseconds*CosDegrees( m_orientationDegrees );
	m_velocity.y=(m_velocity.y) + 30.f*deltaseconds*SinDegrees( m_orientationDegrees );
}


void PlayerShip::TransformVertexArray( int num_ship_vertices, Vertex_PCU* shipVert, float scale, float orientationDegrees, Vec2 translation )
{
	for( int index=0; index<num_ship_vertices; index++ )
	{
		vertices[index].m_position=TransformPosition3DXY( shipVert[index].m_position, scale, orientationDegrees, translation );
	}
}

void PlayerShip::UpdateFromJoystick( float deltaseconds )
{
	const XboxController& controller= input->GetXboxController( 0 );
	const KeyButtonState& a=controller.GetButtonState( XBOX_BUTTON_ID_A );
	const KeyButtonState& start= controller.GetButtonState(XBOX_BUTTON_ID_START);
	const AnalogJoyStick& leftStick = controller.GetLeftJoystick();



	if( !controller.IsCOnnected() )
	{
		return;
	}

	if( start.WasJustPressed() )
	{
		if( m_game->attractMode )
		{
			m_game->HandleStateEvents();
			
		}
		m_game->ResetShip();
		
	}

	if(m_game->GetPlayerShipStatus()==false )
	{ 
		if( a.WasJustPressed() )
		{
			ShootJoystick(m_position,m_orientationDegrees);
		}


		float leftStickMagnitude = leftStick.GetMagnitude();
		if( leftStickMagnitude > 0.f ) 
		{
			m_orientationDegrees = leftStick.GetAngleDegrees();
			if(leftStickMagnitude>0.5f )
			{ 
			m_thrustFraction = leftStickMagnitude;
			}
		
			ThrustAndMove(deltaseconds);
		}
	}

}


void PlayerShip::ThrustAndMove( float deltaseconds )
{
	m_velocity.x=(m_velocity.x) + m_thrustFraction*30.f*deltaseconds*CosDegrees( m_orientationDegrees );
	m_velocity.y=(m_velocity.y) + m_thrustFraction*30.f*deltaseconds*SinDegrees( m_orientationDegrees );
}

void PlayerShip::RotateShipRight(float deltaseconds)
{
	m_orientationDegrees+=300.f*deltaseconds;
}

void PlayerShip::RotateShipLeft( float deltaseconds )
{
	m_orientationDegrees-=300.f*deltaseconds;
}

void PlayerShip::MoveShip(float deltaseconds)
{
	Vec2 distance= Vec2( 0.f, 0.f );
	distance.x=m_velocity.x*deltaseconds;
	distance.y=m_velocity.y*deltaseconds;
	m_position+=distance;

	
}

void PlayerShip::CheckWallCollission()
{

	if( m_position.x+m_cosmeticRadius>200.f || m_position.x-m_cosmeticRadius<0.f )
	{
		m_velocity.x=-m_velocity.x;
	}

	if( m_position.y+m_cosmeticRadius>100.f || m_position.y-m_cosmeticRadius<0.f )
	{
		m_velocity.y=-m_velocity.y;
	}
}


void PlayerShip::Shoot( Vec2 pos, float orientationDergees )
{
	m_game->SpawnBullet(pos,orientationDergees);
}

void PlayerShip::ShootJoystick( Vec2 pos, float orientationDergees )
{
	m_game->SpawnBulletJoyStick(pos,orientationDergees);
}

void PlayerShip::Update( float deltaseconds )
{
	UpdateFromJoystick(deltaseconds);

	if(m_isDead )
		return;

	

	if( g_theapp->spacePressed )
	{
		Shoot(m_position,m_orientationDegrees);
	}
	if( g_theapp->upPressed )
	{
		AcclerateShip(deltaseconds);
	}
	if( g_theapp->leftPressed )
	{
		RotateShipRight(deltaseconds);
	}
	if( g_theapp->rightPressed )
	{
		RotateShipLeft(deltaseconds);

	}
	CheckWallCollission();
	MoveShip(deltaseconds);
	

	
	
	
}

void PlayerShip::Render()
{
	TransformVertexArray( 15, vertices_original, 1.f, m_orientationDegrees, m_position );
	render->DrawVertexArray(15,vertices);
}

void PlayerShip::Die()
{
	m_isGarbage=true;
}
