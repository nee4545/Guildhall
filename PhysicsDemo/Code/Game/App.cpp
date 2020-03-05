#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include<windows.h>
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )
#include "App.hpp"
#include "GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"

RenderContext* g_theRenderer = nullptr;
//InputSystem* input=nullptr;
AudioSystem* g_theAudio = nullptr;



void App::Startup()
{
	g_theRenderer = new RenderContext();
	g_theRenderer->Startup( g_theWindow );
	g_theRenderer->BeginFrame();
	g_theWindow->SetInputSysten( g_theInput );
	if ( g_theAudio == nullptr )
	{
		g_theAudio = new AudioSystem();
	}
	if ( thegame == nullptr )
	{
		thegame = new Game();
	}
	g_theConsole.Startup();
	Clock::SystemStartup();
}

App::~App()
{
	//	delete thegame;
	//	thegame = nullptr;
}

void App::Shutdown() //Not used right now
{

	delete thegame;
	thegame = nullptr;

	g_theRenderer->Shutdown();

}



void App::Update( float deltaSeconds )
{

	g_theRenderer->UpdateFrameTime( deltaSeconds );

	if ( g_theInput->IsKeyPressed( 'Y' ) )
	{
		deltaSeconds *= 4.f;
	}

	if ( g_theInput->IsKeyPressed( 'T' ) )
	{
		deltaSeconds *= 0.1f;
	}

	g_theInput->UpdateMouse();

	thegame->Update( (float)Clock::gMasterClock.GetLastDeltaSeconds() );

	g_theConsole.Update( deltaSeconds );



	if ( g_theWindow->m_quitRequested == true )
	{

		HandleQuitRequested();
	}


}

void App::RunFrame() //Not used right now
{

}

void App::EndFrame() //Not used right now
{
	g_theInput->EndFrame();
	g_theRenderer->EndFrame();
	g_theConsole.EndFrame();
}

void App::Render() const
{

	/*render->ClaerScreen(Rgba8(0,0,0,1));*/
	thegame->Render();
}

void App::BeginFrame()
{
	Clock::BeginFrame();
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();

}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return m_isQuitting;
}

void App::ResetGame()
{

}

