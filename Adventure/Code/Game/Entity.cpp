#include "Game/Entity.hpp"
#define UNUSED(x) (void)(x);

void Entity::Update( float deltaseconds )
{
	UNUSED(deltaseconds);
}

void Entity::Render()
{

}

void Entity::Die()
{

}

Entity::Entity( Game* theGame, Vec2 position ):m_game( theGame ), m_position( position )
{
	m_velocity= Vec2( 0.f, 0.f );
	m_orientationDegrees=0.f;
	m_angularVelocity=0.f;
	m_isDead=false;
	m_isGarbage=false;
	m_health=0;

	m_canSwim = false;
	m_canFly = false;
	m_canWalk = false;
}

void Entity::SetPosition( Vec2& position )
{
	m_position=position;
}

Vec2 Entity::GetPosition()
{
	return m_position;
}
