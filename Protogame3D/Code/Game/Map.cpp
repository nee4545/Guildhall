#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Portal.hpp"
#include "Game/EntityDef.hpp"
#include "Engine/Math/MathUtils.hpp"

Map::Map( char const* mapName ):m_name(mapName)
{

}

Map::~Map()
{

}

Entity* Map::SpawnNewEntityOfType( std::string const& entityDefName )
{
	const EntityDef* entityDef = EntityDef::GetDefinition( entityDefName );
	return SpawnNewEntityOfType(*entityDef);
}

Entity* Map::SpawnNewEntityOfType( EntityDef const& entityDef )
{
	
	if ( entityDef.m_className == "Entity" )
	{
		Entity* newEntity = new Entity(entityDef,this);
		return newEntity;
	}

	if ( entityDef.m_className == "Actor" )
	{
		Actor* newActor = new Actor(entityDef,this);
		return newActor;
	}

	if ( entityDef.m_className == "Portal" )
	{
		Portal* portal = new Portal( entityDef , this );
		return portal;
	}

	return nullptr;
}

void Map::ResolveEntityVsEntityCollisions()
{
	for ( size_t i = 0; i < m_entities.size(); i++ )
	{
		for ( size_t j = i; j < m_entities.size(); j++ )
		{
			if ( nullptr != m_entities[ i ] && nullptr != m_entities[ j ] )
			{
				Entity* firstEntity = m_entities[ i ];
				Entity* secondEntity = m_entities[ j ];
				Vec2	firstPos = firstEntity->m_position;
				float	firstRadius = firstEntity->m_physicsRadius;
				Vec2	secondPos = secondEntity->m_position;
				float	secondRadius = secondEntity->m_physicsRadius;

				if ( DoDiscsOverlap( firstPos , firstRadius , secondPos , secondRadius ) )
				{
					if ( firstEntity->m_canPushEntities && secondEntity->m_canBePushedByEntities &&
						firstEntity->m_canBePushedByEntities && secondEntity->m_canPushEntities )
					{
						float firstEntityMass = firstEntity->m_mass;
						float secondEntityMass = secondEntity->m_mass;
						float	massRatioFirst = secondEntityMass / ( firstEntityMass + secondEntityMass );
						float	massRatioSecond = firstEntityMass / ( firstEntityMass + secondEntityMass );
						Vec2	displacementBetweenCenters = Vec2( firstPos - secondPos );
						float	distanceBetweenCenters = displacementBetweenCenters.GetLength();
						float	overlap = firstRadius + secondRadius - distanceBetweenCenters;
						Vec2	normal = ( firstPos - secondPos ).GetNormalized();
						Vec2	firstDisp = overlap * massRatioFirst * normal;
						Vec2	secondDisp = overlap * massRatioSecond * -normal;

						firstEntity->m_position +=  firstDisp;
						secondEntity->m_position += secondDisp;
					}
					if ( firstEntity->m_canPushEntities && !secondEntity->m_canBePushedByEntities &&
						firstEntity->m_canBePushedByEntities && !secondEntity->m_canPushEntities )
					{
						Vec2	displacementBetweenCenters = Vec2( firstPos - secondPos );
						float	distanceBetweenCenters = displacementBetweenCenters.GetLength();
						float	overlap = firstRadius + secondRadius - distanceBetweenCenters;
						Vec2	normal = ( firstPos - secondPos ).GetNormalized();
						Vec2	firstDisp = overlap * normal;

						firstEntity->m_position +=  firstDisp;
					}
					if ( !firstEntity->m_canPushEntities && secondEntity->m_canBePushedByEntities &&
						!firstEntity->m_canBePushedByEntities && secondEntity->m_canPushEntities )
					{
						float firstEntityMass = firstEntity->m_mass;
						float secondEntityMass = secondEntity->m_mass;
						float	massRatioFirst = 1.f / ( firstEntityMass + secondEntityMass );

						Vec2	displacementBetweenCenters = Vec2( firstPos - secondPos );
						float	distanceBetweenCenters = displacementBetweenCenters.GetLength();
						float	overlap = firstRadius + secondRadius - distanceBetweenCenters;
						Vec2	normal = ( firstPos - secondPos ).GetNormalized();
						Vec2	firstDisp = overlap * massRatioFirst * normal;
						Vec2	secondDisp = overlap * -normal;
						secondEntity->m_position += secondDisp ;
					}
				}

			}
		}
	}
}
