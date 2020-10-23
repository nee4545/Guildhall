#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Texture.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"


Game::Game()
{
	
	
}

Game::~Game()
{

}

void Game::StartUp()
{

}

void Game::Update( float deltaseconds )
{

	
}

void Game::Render()
{

}

InputSystem* Game::GetInputSystem()
{
	if ( m_InputSystem != nullptr )
	{
		return m_InputSystem;
	}
	else
	{
		return nullptr;
	}
}
























