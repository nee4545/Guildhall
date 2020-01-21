#pragma once


class Map;
class Game;

class World
{
	Game* m_game;
	int current_mapindex=0;
public:

	Map* current_map=nullptr;
	Map* maps[3];
	World(Game* game);
	~World(){};

	void Render();
	void Update(float deltaSeconds);
	void ChangeMap();
	void ResetGame();
	
};