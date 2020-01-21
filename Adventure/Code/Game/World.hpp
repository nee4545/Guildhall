#pragma once

#include "Game/Map.hpp"
class Game;

class World
{
	Game* m_game;
public:

	Map* current_map=nullptr;
	World(Game* game);
	~World(){};

	void Render();
	void Update(float deltaSeconds);
	
};