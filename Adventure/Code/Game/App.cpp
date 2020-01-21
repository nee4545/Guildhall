#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include<windows.h>
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )
#include "App.hpp"
#include "GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"

RenderContext* g_theRenderer = nullptr;
//InputSystem* input=nullptr;




void App:: Startup()
{
	g_theRenderer = new RenderContext();
	g_theRenderer->Startup();
	g_theRenderer->BeginFrame();
	if(thegame==nullptr )
	{ 
	thegame =new Game();
	thegame->StartUp();
	}
	
}

void App::Shutdown() //Not used right now
{
	
}



void App::Update(float deltaSeconds)
{

	if( g_theInput->IsKeyPressed( 'Y' ) )
	{
		deltaSeconds*=4.f;
	}

	if( g_theInput->IsKeyPressed( 'T' ) )
	{
		deltaSeconds*=0.1f;
	}
	g_theInput->UpdateMouse();

	
	
	thegame->Update(deltaSeconds);

	if( g_theInput->WasKeyJustPressed( VK_ESCAPE ) )
	{
		m_isQuitting=true;
	}

	
	
	if( g_theInput->WasKeyJustPressed( 0x77 ) )
	{
		ResetGame();
	}

}

void App::RunFrame()
{
	static double timeLastFrameStarted=GetCurrentTimeSeconds();
	double timethisFrameStarted = GetCurrentTimeSeconds();
	double deltaseconds =timethisFrameStarted-timeLastFrameStarted;
	timeLastFrameStarted=timethisFrameStarted;


	BeginFrame();
	Update( (float)deltaseconds );
	Render();
	EndFrame();
}

void App::EndFrame() //Not used right now
{
	g_theInput->EndFrame();
	g_theRenderer->EndFrame();
}

void App::Render() const
{

	g_theRenderer->ClaerScreen(Rgba8(0,0,0,1));
	thegame->Render();
}



void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theInput->UpdateMouse();
	
}

bool App::HandleQuitRequested()
{
	m_isQuitting=true;
	return m_isQuitting;
}

void App::ResetGame()
{
	delete(thegame);
	thegame=nullptr;
	thegame=new Game();
}

