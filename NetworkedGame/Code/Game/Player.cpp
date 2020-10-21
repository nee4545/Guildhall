#include "Game/Player.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
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


	m_orientationDegrees=45.f;
	m_turretAngle=0.f;
	m_physicsRadius=0.2f;
	m_cosmeticRadius=0.4f;
	m_health =3;

	m_isPushedByWalls=true;
	m_isPushedByEntities=true;
	m_doesPushEntities=true;
	m_isHitByBullets=true;

}

void Player::Update( float deltaSeconds )
{

}

void Player::Render()
{

}

void Player::Die()
{

}

