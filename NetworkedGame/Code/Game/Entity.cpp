#include "Game/Entity.hpp"

void Entity::Update( float deltaseconds )
{

}

void Entity::Render()
{

}

void Entity::Die()
{
	m_isGarbage = true;
}



Entity::Entity( Game* game , Vec2 position )
{
	m_game = game;
	m_position = position;
}

void Entity::SetPosition( Vec2& position )
{
	m_position = position;
}

Vec2 Entity::GetPosition()
{
	return m_position;
}
