#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include<windows.h>
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )
#include "App.hpp"
#include "GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"

RenderContext* render = nullptr;
//InputSystem* input=nullptr;
AudioSystem* audio = nullptr;


void App:: Startup()
{
	if( input == nullptr )
	{
		input=new InputSystem();
	}
	input->StartUp();
	render = new RenderContext();
	render->Startup();
	if( audio == nullptr )
	{
		audio = new AudioSystem();
	}
	if(thegame==nullptr )
	{ 
	thegame =new Game();
	}

	
}

void App::Shutdown() //Not used right now
{
	
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

	
	
	thegame->Update(deltaSeconds);

	/*if( input->WasKeyJustPressed( VK_ESCAPE ) )
	{
		m_isQuitting=true;
	}*/

	
	
	if( input->WasKeyJustPressed( 0x77 ) )
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
	Update((float)deltaseconds);
	Render();
	EndFrame();
}

void App::EndFrame() 
{
	input->EndFrame();
	render->EndFrame();
	audio->EndFrame();
}

void App::Render() const
{

	render->ClaerScreen(Rgba8(0,0,0,1));
	render->BeginCamera(*(thegame->m_camera));
	thegame->Render();
	render->EndCamera(*(thegame->m_camera));

}







void App::BeginFrame()
{
	input->BeginFrame();
	render->BeginFrame();
	audio->BeginFrame();
	
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

