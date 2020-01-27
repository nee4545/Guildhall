#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include<windows.h>
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )
#include "App.hpp"
#include "GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"

RenderContext* render = nullptr;
//InputSystem* input=nullptr;




void App:: Startup()
{
	render = new RenderContext();
	render->Startup(g_theWindow);
	render->BeginFrame();
	g_theWindow->SetInputSysten( input );
	/*if(thegame==nullptr )
	{ 
	thegame =new Game();
	}*/
	
}

void App::Shutdown() //Not used right now
{
	render->Shutdown();
}



void App::Update(float deltaSeconds)
{

	if( input->IsKeyPressed( 'Y' ) )
	{
		deltaSeconds*=4.f;
	}

	if( input->IsKeyPressed( 'T' ) )
	{
		deltaSeconds*=0.1f;
	}

	
	
	//thegame->Update(deltaSeconds);

	if( input->WasKeyJustPressed( VK_ESCAPE ) )
	{
		m_isQuitting=true;
	}

	//HandleQuitRequested();
	
	//if( input->WasKeyJustPressed( 0x77 ) )
	//{
	//	ResetGame();
	//}

}

void App::RunFrame() //Not used right now
{

}

void App::EndFrame() //Not used right now
{
	input->EndFrame();
	render->EndFrame();
}

void App::Render() const
{

	/*render->ClaerScreen(Rgba8(0,0,0,1));
	thegame->Render();*/
}







void App::BeginFrame()
{
	input->BeginFrame();
	render->BeginFrame();
	
}

bool App::HandleQuitRequested()
{
	m_isQuitting=true;
	return m_isQuitting;
}

void App::ResetGame()
{
	/*delete(thegame);
	thegame=nullptr;
	thegame=new Game();*/
}

