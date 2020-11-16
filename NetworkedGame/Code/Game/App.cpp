#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include<windows.h>
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )
#include "App.hpp"
#include "GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerClient.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/JobSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/AuthoritativeServer.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Game/RemoteServer.hpp"
#include "Game/RemoteClient.hpp"
#pragma comment(lib,"Ws2_32.lib")

bool StartMultiplayerServerPort( EventArgs& args )
{
	g_theConsole.PrintString( Rgba8() , "This game is going to stop" );
	App::StartMultiplayerServer( 0 );
	return false;
}

bool ConnectToMultiplayeServer( EventArgs& args )
{
	g_theConsole.PrintString( Rgba8() , "This game is going to stop" );
	App::ConnectToMultiplayerServer( "address" );
	return false;
}

RenderContext* g_theRenderer = nullptr;
AudioSystem* g_theAudio = nullptr;
Game* thegame = nullptr;
JobSystem* g_theJobSystem = nullptr;
NetworkSystem* g_theNetwork = nullptr;
AuthoritativeServer* g_theAutoratitiveServer = nullptr;
RemoteServer* g_theRemoteServer = nullptr;

void App::Startup()
{
	g_theRenderer = new RenderContext();
	g_theRenderer->Startup( g_theWindow );
	g_theRenderer->BeginFrame();
	g_theJobSystem = new JobSystem();
	g_theJobSystem->StartUp();
	g_theWindow->SetInputSysten( g_theInput );

	if ( g_theAudio == nullptr )
	{
		g_theAudio = new AudioSystem();
	}

	g_theNetwork = new NetworkSystem();
	g_theNetwork->StartUp();

	g_theConsole.Startup();

	Clock::SystemStartup();

	DebugRenderSystem::sDebugRenderer->SystemStartUp();


	if ( g_theAutoratitiveServer == nullptr )
	{
		g_theAutoratitiveServer = new AuthoritativeServer();
		g_theAutoratitiveServer->CreateSinglePlayerGame();
	}

	

	m_client = new PlayerClient();
	m_client->StartUp();

	g_theAutoratitiveServer->AddPlayerClientToServer( m_client );
	
	g_theEventSystem.SubscribeToEvent( "StartMultiplayerPort" , &StartMultiplayerServerPort );
	g_theEventSystem.SubscribeToEvent( "ConnectToMultiplayerServer" , &ConnectToMultiplayeServer );
}

App::~App()
{
	//	delete thegame;
	//	thegame = nullptr;
}

void App::Shutdown() //Not used right now
{

	/*delete thegame;
	thegame = nullptr;*/

	DebugRenderSystem::sDebugRenderer->SystemShutDown();

	g_theRenderer->Shutdown();
	g_theJobSystem->ShutDown();

	if ( g_theAutoratitiveServer != nullptr )
	{
		g_theAutoratitiveServer->ShutDown();
	}

}



void App::Update( float deltaSeconds )
{

	g_theRenderer->UpdateFrameTime( deltaSeconds );
	DebugRenderSystem::sDebugRenderer->Update();

	g_theInput->UpdateMouse();

	if ( g_theInput->GetCursorMode() == MODE_RELATIVE )
	{
		g_theInput->UpdateRelativeMode();
	}

	if ( g_theRemoteServer != nullptr )
	{
		g_theRemoteServer->Update( deltaSeconds );
	}
	//thegame->Update( deltaSeconds );

	g_theConsole.Update( deltaSeconds );

	if ( g_theAutoratitiveServer != nullptr )
	{
		g_theAutoratitiveServer->Update( deltaSeconds );
	}

	if ( g_theInput->WasKeyJustPressed( 0xC0 ) )
	{
		bool DevConsoleStatus = g_theConsole.IsOpen();
		g_theConsole.SetIsOpen( !DevConsoleStatus );
	}
	
	if ( g_theWindow->m_quitRequested == true )
	{

		HandleQuitRequested();
	}

	if ( g_theInput->WasKeyJustPressed( ESC ) )
	{
		HandleQuitRequested();
	}


	


}

void App::RunFrame()
{
	static double timeLastFrameStarted = GetCurrentTimeSeconds();
	double timethisFrameStarted = GetCurrentTimeSeconds();
	double deltaseconds = timethisFrameStarted - timeLastFrameStarted;
	timeLastFrameStarted = timethisFrameStarted;

	BeginFrame();
	Update( ( float ) deltaseconds );
	Render();
	EndFrame();
}

void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theRenderer->EndFrame();
	g_theConsole.EndFrame();

	if ( m_client != nullptr )
	{
		m_client->EndFrame();
	}

	if ( m_remoteclient != nullptr )
	{
		m_remoteclient->EndFrame();
	}

	if ( g_theRemoteServer != nullptr )
	{
		g_theRemoteServer->EndFrame();
	}
}

void App::Render() const
{

	/*render->ClaerScreen(Rgba8(0,0,0,1));*/
	//thegame->Render();
	if ( m_client != nullptr )
	{
		m_client->Render();
	}

	if ( m_remoteclient != nullptr )
	{
		m_remoteclient->Render();
	}

	if ( g_theConsole.IsOpen() )
	{
		g_theConsole.Render( *g_theRenderer , *g_theConsole.m_devConsoleCamera , 2.f , 1.f );
	}
	//DebugRenderSystem::sDebugRenderer->Render();
}

void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();
	g_theJobSystem->BeginFrame();
	g_theNetwork->BeginFrame();
	Clock::BeginFrame();

	if ( g_theAutoratitiveServer != nullptr )
	{
		g_theAutoratitiveServer->BeginFrame();
	}

	if ( g_theRemoteServer != nullptr )
	{
		g_theRemoteServer->BeginFrame();
	}

	if ( m_client != nullptr )
	{
		m_client->BeginFrame();
	}

	if ( m_remoteclient != nullptr )
	{
		m_remoteclient->BeginFrame();
	}
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
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
	int fps = ( int ) ( 1.0 / deltaseconds );
	timeLastFrameStarted = timethisFrameStarted;
	return fps;
}

void App::StartMultiplayerServer( int port )
{
	g_theAutoratitiveServer->StartMultiplayerGame( 48001 );

	/*delete g_theapp->m_client;
	g_theapp->m_client = nullptr;*/
}

void App::ConnectToMultiplayerServer( std::string address )
{
	g_theRemoteServer = new RemoteServer();
	g_theRemoteServer->StartUp();
	g_theRemoteServer->StartMultiplayerGame("127.1.1.1",48001);

	delete g_theapp->m_client;
	g_theapp->m_client = nullptr;

	g_theapp->m_remoteclient = new RemoteClient();
}

