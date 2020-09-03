#pragma once

#include <vector>
#include <map>
#include <string>
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Audio/AudioSystem.hpp"
class Map;
class Game;
typedef std::vector<std::string> Strings;

class World
{
public:
	World(Game* game);
	~World() {};

	void Update( float deltaSeconds );
	void Render();
	void LoadMap( std::string mapName );
	Map* GetMap( std::string mapName );

	void LoadAllMaps( std::string folderPath );
	void CreateMap( std::string mapType, std::string mapName ,tinyxml2::XMLElement* mapElement );
	Strings GetAllCurrentMapNames();

	std::map<std::string , Map*> m_maps;

	Map* m_currentMap = nullptr;

	SoundID m_sound=0;
	Game* m_game = nullptr;

};