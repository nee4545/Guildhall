#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#define UNUSED(x) (void)(x);

void Entity::Update( float deltaseconds )
{
	UNUSED(deltaseconds);
	/*if ( thegame->m_player == this )
	{
		return;
	}*/
}

void Entity::Render()
{
	if ( thegame->m_player == this )
	{
		return;
	}
}

void Entity::DebugRender()
{

}

void Entity::Die()
{

}

Entity::Entity( Vec2 position ): m_position( position )
{
	m_isDead=false;
	m_isGarbage=false;
	m_health=0;
}

Entity::Entity( EntityDef const& entityDef , Map* map )
{
	m_owner = map;
	m_canBePushedByWalls = entityDef.m_pushedByWalls;
	m_canBePushedByEntities = entityDef.m_pushedByEntities;
	m_canPushEntities = entityDef.m_pushesEntities;
	m_spriteSheet = entityDef.m_spriteSheet;
	m_entityAnims[ENTITY_WALK_ANIM] = entityDef.m_entityAnims[ENTITY_WALK_ANIM];
	m_entityAnims[ENTITY_ATTACK_ANIM] = entityDef.m_entityAnims[ENTITY_ATTACK_ANIM];
	m_entityAnims[ENTITY_PAIN_ANIM] = entityDef.m_entityAnims[ENTITY_PAIN_ANIM];
	m_entityAnims[ENTITY_DEATH_ANIM] = entityDef.m_entityAnims[ENTITY_DEATH_ANIM];

	m_physicsRadius = entityDef.m_physicsRadius;
	m_height = entityDef.m_height;
	m_eyeHeight = entityDef.m_eyeHeight;
	m_spriteSize = entityDef.m_spriteSize;
	m_mass = entityDef.m_mass;
	m_damage = entityDef.m_damage;
	m_speed = entityDef.m_speed;
	m_billboard = entityDef.m_billboard;
	m_canBePushedByWalls = true;
	m_canBePushedByEntities = true;
	m_canPushEntities = true;
}

void Entity::SetPosition( Vec2& position )
{
	m_position=position;
}

Vec2 Entity::GetPosition()
{
	return m_position;
}
