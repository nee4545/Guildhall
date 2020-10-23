#pragma once
#include "Game/Entity.hpp"
#include <vector>

class Game;
class Client;
class PlayerClient;
class SinglePlayerGame;
class MultiplayerGame;

enum GameState
{
	SINGLEPLAYER_GAME ,
	MULTIPLAYER_GAME ,
};

class Server
{
public:
	Server(  );
	SinglePlayerGame* m_singlePlayerGame = nullptr;
	MultiplayerGame* m_multiplayerGame = nullptr;
	GameState m_gameState = SINGLEPLAYER_GAME;

	std::vector<Client*> m_clients;

	void AddPlayerClientToServer( Client* client );
	bool RemoveClient( Client* client );
	void CreateSinglePlayerGame();
	void CreateMultiPlayerGame();


};