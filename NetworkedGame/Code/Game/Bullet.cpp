#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

Bullet::Bullet( Game* game , Vec2 position , Vec2 forwardVector , Faction faction  ):Entity(game,position)
{

	bulletSpeed=BULLET_SPEED;
	m_faction = faction;
	m_physicsRadius = 0.5f;
	m_forwardVector = forwardVector;
	
}			 

Bullet::~Bullet()
{

}

void Bullet::Update( float deltaSeconds )
{
	MoveBullet(deltaSeconds);
}

void Bullet::Render()
{
	if ( m_faction == FACTION_GOOD )
	{
		g_theRenderer->DrawDisc( m_position , 0.5f , Rgba8(0,0,100,255) );
	}
	else
	{
		g_theRenderer->DrawDisc( m_position , 0.5f , Rgba8( 100 , 100 , 100 , 255 ) );
	}

}

void Bullet::Die()
{
	m_isGarbage=true;
}

void Bullet::MoveBullet(float deltaSeconds)
{
	m_position += m_forwardVector * 10.f * deltaSeconds;
}

void Bullet::ResetVertices()
{

}


