#include "Game/World.hpp"
#include "Game/Game.hpp"
#include "Map.hpp"

World::World( Game* game ):m_game(game)
{
	maps[0] = new Map( m_game, this, IntVec2( 20, 30 ) );

	while( !maps[0]->IsMapValidityUsingFloodFill() )
	{
		delete maps[0];
		maps[0] =  new Map( m_game, this, IntVec2( 20, 30 ) );
	}

	maps[1] = new Map( m_game, this, IntVec2( 30, 30 ) );
	while( !maps[1]->IsMapValidityUsingFloodFill() )
	{
		delete maps[1];
		maps[1] =  new Map( m_game, this, IntVec2( 30, 30 ) );
	}

	maps[2] = new Map( m_game, this, IntVec2( 40, 30 ) );
	while( !maps[2]->IsMapValidityUsingFloodFill() )
	{
		delete maps[2];
		maps[2] =  new Map( m_game, this, IntVec2( 40, 30 ) );
	}

	current_map=maps[0];

}

void World::Render()
{
	current_map->Render();
}

void World::Update( float deltaSeconds )
{
	current_map->Update(deltaSeconds);
}

void World::ChangeMap()
{
	if(current_mapindex<2 )
	{ 
	current_map = maps[++current_mapindex];
	current_map->m_EntityListByType[ENTITY_TYPE_PLAYER][0]= maps[current_mapindex-1]->m_EntityListByType[ENTITY_TYPE_PLAYER][0];
	Vec2 position = Vec2(2.5f,2.5f);
	current_map->m_EntityListByType[ENTITY_TYPE_PLAYER][0]->SetPosition(position);
	}
	else
	{
		current_map = maps[0];
		m_game->didPlayerWin=true;
	}
}

void World::ResetGame()
{
	maps[0] = new Map( m_game, this, IntVec2( 20, 30 ) );

	while( !maps[0]->IsMapValidityUsingFloodFill() )
	{
		delete maps[0];
		maps[0] =  new Map( m_game, this, IntVec2( 20, 30 ) );
	}

	maps[1] = new Map( m_game, this, IntVec2( 30, 30 ) );
	while( !maps[1]->IsMapValidityUsingFloodFill() )
	{
		delete maps[1];
		maps[1] =  new Map( m_game, this, IntVec2( 30, 30 ) );
	}

	maps[2] = new Map( m_game, this, IntVec2( 40, 30 ) );
	while( !maps[2]->IsMapValidityUsingFloodFill() )
	{
		delete maps[2];
		maps[2] =  new Map( m_game, this, IntVec2( 40, 30 ) );
	}

	current_map=maps[0];
	current_mapindex =0;
	m_game->currentLives =3;
	m_game->didPlayerWin = false;
}

