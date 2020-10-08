#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#define UNUSED(x) (void)(x);

void Entity::Update( float deltaseconds )
{
	UNUSED(deltaseconds);

}

void Entity::Render()
{

}

void Entity::DebugRender()
{

}

void Entity::Die()
{

}




void Entity::SetPosition( Vec2& position )
{
	m_position = position;
}

Vec2 Entity::GetPosition()
{
	return m_position;
}
