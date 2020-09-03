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
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/JobSystem.hpp"

RenderContext* g_theRenderer = nullptr;
//InputSystem* input=nullptr;
AudioSystem* g_theAudio = nullptr;
Game* thegame = nullptr;
JobSystem* g_theJobSystem = nullptr;


void App:: Startup()
{
	g_theRenderer = new RenderContext();
	g_theRenderer->Startup(g_theWindow);
	g_theRenderer->BeginFrame();
	g_theJobSystem = new JobSystem();
	g_theJobSystem->StartUp();
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

	DebugRenderSystem::sDebugRenderer->SystemStartUp();
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
	
	DebugRenderSystem::sDebugRenderer->SystemShutDown();
	
	g_theRenderer->Shutdown();
	g_theJobSystem->ShutDown();

}



void App::Update(float deltaSeconds)
{

	g_theRenderer->UpdateFrameTime( deltaSeconds );
	DebugRenderSystem::sDebugRenderer->Update();

	/*if ( g_theInput->WasKeyJustPressed( 'G' ) )
	{
		HICON icon = LoadIcon( NULL , IDI_WARNING );

		SendMessage( ( HWND ) g_theWindow->m_hwnd , WM_SETICON , ICON_BIG , ( LPARAM ) icon );
	}
	
	if ( g_theInput->WasKeyJustPressed( 'H' ) )
	{
		SetWindowText( ( HWND ) g_theWindow->m_hwnd , L"New Title" );
	}

	if ( g_theInput->WasKeyJustPressed( 'J' ) )
	{
		SetWindowLong( ( HWND ) g_theWindow->m_hwnd , GWL_STYLE ,
			WS_POPUP );
		ShowWindow( ( HWND ) g_theWindow->m_hwnd , SW_SHOWDEFAULT );
	}

	if ( g_theInput->WasKeyJustPressed( 'K' ) )
	{
		SetWindowLong( ( HWND ) g_theWindow->m_hwnd , GWL_STYLE ,
			WS_POPUP );
		ShowWindow( ( HWND ) g_theWindow->m_hwnd , SW_SHOWMAXIMIZED );
	}*/

	g_theInput->UpdateMouse();

	if ( g_theInput->GetCursorMode() == MODE_RELATIVE )
	{
		g_theInput->UpdateRelativeMode();
	}

	thegame->Update( deltaSeconds );
	
	g_theConsole.Update( deltaSeconds );


	if ( g_theWindow->m_quitRequested == true )
	{
		
		HandleQuitRequested();
	}

	if ( g_theInput->WasKeyJustPressed( ESC ) )
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
	//DebugRenderSystem::sDebugRenderer->Render();
}

void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();
	g_theJobSystem->BeginFrame();
	Clock::BeginFrame();
	
}

bool App::HandleQuitRequested()
{
	m_isQuitting=true;
	return m_isQuitting;
}

void App::ResetGame()
{
	
}

int App::GetFPS()
{
	static double timeLastFrameStarted = GetCurrentTimeSeconds();
	double timethisFrameStarted = GetCurrentTimeSeconds();
	double deltaseconds = timethisFrameStarted - timeLastFrameStarted;
	int fps = ( int ) (1.0 / deltaseconds);
	timeLastFrameStarted = timethisFrameStarted;

	return fps;

}

