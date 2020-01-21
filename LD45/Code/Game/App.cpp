#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include<windows.h>
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )
#include "App.hpp"
#include "GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"

RenderContext* render = nullptr;
//InputSystem* input=nullptr;




void App:: Startup()
{
	render = new RenderContext();
	render->BeginFrame();
	thegame =new Game();
	//input= new InputSystem();
	
}

void App::Shutdown() //Not used right now
{
	
}



void App::Update(float deltaSeconds)
{
	
	thegame->Update(deltaSeconds);

	if( input->WasKeyJustReleased( VK_ESCAPE ) )
	{
		m_isQuitting=true;
	}
	

}

void App::RunFrame() //Not used right now
{

}

void App::EndFrame() //Not used right now
{
	input->EndFrame();
}

void App::Render() const
{

	render->ClaerScreen(Rgba8(0,0,0,1));
	
	render->BeginCamera(render->game_cam);
	thegame->Render();
	render->EndCamera(render->game_cam);

}





void App::BeginFrame()
{
	input->BeginFrame();
	
}

bool App::HandleQuitRequested()
{

	return m_isQuitting;
}

void App::ResetGame()
{
	delete(thegame);
	thegame=nullptr;
	thegame=new Game();
}

