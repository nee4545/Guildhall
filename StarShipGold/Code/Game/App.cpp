#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include<windows.h>
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )
#include "App.hpp"
#include "GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vec2.hpp"

RenderContext* render = nullptr;
InputSystem* input=nullptr;




void App:: Startup()
{
	render = new RenderContext();
	render->BeginFrame();
	thegame =new Game();
	input= new InputSystem();
	game_cam = new Camera();
	ui_cam = new Camera();
	game_cam->SetOrthoView(Vec2(0.f,0.f),Vec2(200.f,100.f));
	ui_cam->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2(1600.f,800.f) );
	
}

void App::Shutdown() //Not used right now
{
	
}



void App::Update(float deltaSeconds)
{
	if( m_isSlowMo )
	{
		deltaSeconds=deltaSeconds*0.1f;
	}
	if( m_isPaused )
	{
		deltaSeconds=0.f;
	}
	thegame->Update(deltaSeconds);
	

}

void App::RunFrame() //Not used right now
{

}

void App::EndFrame() //Not used right now
{
	//SwapBuffers( g_displayDeviceContext );
}

void App::Render() const
{
	//render->ClaerScreen(Rgba8(0,0,0,1));
	
	//render->EndCamera( render->ui_cam );

	render->ClaerScreen(Rgba8(0,0,0,1));
	//render->BeginCamera( render->ui_cam );
	render->BeginCamera(*game_cam);
	render->BindTexture( nullptr );
	thegame->Render();
	thegame->RenderUI();
	render->EndCamera(*game_cam);

}





void App::BeginFrame()
{
	input->BeginFrame();
	
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

bool App::HandelKeyPressed( unsigned char keyCode )
{
	if( keyCode==0x20 ) 
	{
		spacePressed=true;
		thegame->HandleStateEvents();
	}

	if( keyCode=='T' )
	{
		m_isSlowMo=true;
	}
	if( keyCode == 0x26 )
	{
		upPressed=true;
	}

	if( keyCode == 0x25 )
	{
		leftPressed=true;
	}

	if( keyCode == 0x27 )
	{
		rightPressed=true;
	}


	if( keyCode== 'O' )
	{
		oPressed=true;
	}

	if( keyCode== 0x70 )
	{
		f1Presssed=true;
		thegame->ToggleDebugging();
	}

	if( keyCode==0x77 )
	{
		ResetGame();
	}

	if( keyCode=='N' )
	{
		thegame->HandleStateEvents();
	}


	return true;
}


bool App::HandelKeyReleased( unsigned char keyCode )
{
	if( keyCode=='T' )
	{
		m_isSlowMo=false;
	}

	if( keyCode=='P' )
	{
		m_isPaused=!m_isPaused;
	}

	if( keyCode == 0x26 )
	{
		upPressed=false;
	}

	if( keyCode == 0x25 )
	{
		leftPressed=false;
	}

	if( keyCode == 0x27 )
	{
		rightPressed=false;
	}

	if( keyCode==0x20 )  
	{
		spacePressed=false;
		thegame->canShoot=true;
	}

	if( keyCode== 'O' )
	{
		oPressed=false;
		thegame->canSpawnAstroid=true;
	}

	if( keyCode== 0x70 )
	{
		f1Presssed=false;
		
	}

	return true;
}