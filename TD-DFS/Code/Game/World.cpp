#include "Game/World.hpp"
#include "Game/Game.hpp"
#include "Map.hpp"

World::World( Game* game ):m_game(game)
{
	current_map=new Map(game,this,IntVec2(20,30));
}

void World::Render()
{
	current_map->Render();
}

void World::Update( float deltaSeconds )
{
	current_map->Update(deltaSeconds);
}
