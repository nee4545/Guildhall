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
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/ThirdParty/ImGuiSystem.hpp"
#pragma comment(lib,"Ws2_32.lib")

RenderContext* g_theRenderer = nullptr;
AudioSystem* g_theAudio = nullptr;
Game* thegame = nullptr;
JobSystem* g_theJobSystem = nullptr;
NetworkSystem* g_theNetwork = nullptr;
extern ImGuiSystem* g_theGUI;

void App:: Startup()
{
	g_theRenderer = new RenderContext();
	g_theRenderer->Startup(g_theWindow);
	//g_theRenderer->BeginFrame();

	g_theJobSystem = new JobSystem();
	g_theJobSystem->StartUp();

	g_theWindow->SetInputSysten( g_theInput );

	g_theNetwork = new NetworkSystem();
	g_theNetwork->StartUp();

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

	g_theGUI = new ImGuiSystem( g_theWindow , g_theRenderer );
	g_theGUI->StartUp();
}

App::~App()
{
//	delete thegame;
//	thegame = nullptr;
	delete g_theGUI;
	g_theGUI = nullptr;
}

void App::Shutdown() //Not used right now
{

	delete thegame;
	thegame = nullptr; 
	
	g_theGUI->ShutDown();

	DebugRenderSystem::sDebugRenderer->SystemShutDown();
	
	g_theRenderer->Shutdown();
	g_theJobSystem->ShutDown();

}



void App::Update(float deltaSeconds)
{

	g_theRenderer->UpdateFrameTime( deltaSeconds );
	DebugRenderSystem::sDebugRenderer->Update();

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
	//g_theGUI->EndFrame();
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
	g_theNetwork->BeginFrame();
	Clock::BeginFrame();
	
	g_theGUI->BeginFrame();

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

