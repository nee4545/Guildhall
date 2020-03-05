#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Platform/Window.hpp"
#define UNUSED(x) (void)(x);



App* g_theapp = nullptr; //App pointer
InputSystem* g_theInput = nullptr;
Window* g_theWindow = nullptr;
HWND g_hWnd = nullptr;								// ...becomes WindowContext::m_windowHandle

const char* APP_NAME = "ProtoGame 2D";	// ...becomes ??? (Change this per project!)


void RunFrame()
{
	static double timeLastFrameStarted = GetCurrentTimeSeconds();
	double timethisFrameStarted = GetCurrentTimeSeconds();
	double deltaseconds = timethisFrameStarted - timeLastFrameStarted;
	timeLastFrameStarted = timethisFrameStarted;

	g_theapp->BeginFrame(); //for all engine systems to do something early
	g_theapp->Update( ( float ) deltaseconds );
	g_theapp->Render();
	g_theapp->EndFrame();
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle , _In_opt_ HINSTANCE , _In_ LPSTR commandLineString , _In_ int )
{

	UNUSED( commandLineString );
	UNUSED( applicationInstanceHandle );


	g_theapp = new App();

	g_theWindow = new Window();
	g_theWindow->Open( APP_NAME , CLIENT_ASPECT , 0.90f ); // feed from blackBoard

	g_theInput = new InputSystem();
	g_theapp->Startup();

	// Program main loop; keep running frames until it's time to quit
	while ( !g_theapp->isQuitting() )
	{
		g_theWindow->BeginFrame();
		//g_theapp->RunFrame();
		RunFrame();
	}

	g_theapp->Shutdown();
	delete g_theapp;
	g_theapp = nullptr;

	return 0;
}