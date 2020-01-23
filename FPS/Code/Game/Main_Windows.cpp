#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "GameCommon.hpp"

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Remove all OpenGL references out of Main_Win32.cpp once you have a RenderContext
// Both of the following lines should be relocated to the top of Engine/Renderer/RenderContext.cpp
//
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#include "Engine/Core/Time.hpp"
#include "Engine/Platform/Window.hpp"
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this useful macro to a more central place, e.g. Engine/Core/EngineCommon.hpp
//
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move these constants to Game/GameCommon.hpp or elsewhere
// 
//constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area



 App *g_theapp=nullptr; //App pointer
 InputSystem* input=nullptr;

 Window* g_theWindow = nullptr;

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move each of these items to its proper place, once that place is established
// 
//bool g_isQuitting = false;							// ...becomes App::m_isQuitting
HWND g_hWnd = nullptr;								// ...becomes WindowContext::m_windowHandle
HDC g_displayDeviceContext = nullptr;				// ...becomes WindowContext::m_displayContext
HGLRC g_openGLRenderingContext = nullptr;			// ...becomes RenderContext::m_apiRenderingContext
const char* APP_NAME = "ProtoGame 2D";	// ...becomes ??? (Change this per project!)



void CreateRenderContext()
{
	// Creates an OpenGL rendering context (RC) and binds it to the current window's device context (DC)
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );

	// #SD1ToDo: move all OpenGL functions (including those below) to RenderContext.cpp (only!)
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}



//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void RunMessagePump()
{
	
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Startup()
//
void TheApp_Startup( void* applicationInstanceHandle, const char* commandLineString )
{
	UNUSED( applicationInstanceHandle );
	UNUSED( commandLineString );
										// #SD1ToDo: ...with the two lines below
}


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
// Some simple OpenGL example drawing code.
// This is the graphical equivalent of printing "Hello, world."
// #SD1ToDo:


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::EndFrame()
//
void TheApp_EndFrame()
{
	// "Present" the backbuffer by swapping the front (visible) and back (working) screen buffers
	SwapBuffers( g_displayDeviceContext ); // Note: call this once at the end of each frame
}


//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::RunFrame()
//
void RunFrame()
{
	static double timeLastFrameStarted=GetCurrentTimeSeconds();
	double timethisFrameStarted = GetCurrentTimeSeconds();
	double deltaseconds =timethisFrameStarted-timeLastFrameStarted;
	timeLastFrameStarted=timethisFrameStarted;

	g_theapp->BeginFrame(); //for all engine systems to do something early
	g_theapp->Update((float)deltaseconds);	
	g_theapp->Render(); 
	g_theapp->EndFrame();
	TheApp_EndFrame();		// can't change to g_app->EndFrame() right now as we use swap buffers which requires variables used in main
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString,_In_ int )
{

	UNUSED( commandLineString );
	UNUSED(applicationInstanceHandle);

	
	g_theapp=new App();

	g_theWindow = new Window();
	g_theWindow->Open( APP_NAME , CLIENT_ASPECT , 0.90f ); // feed from blackBoard
		
	
	CreateRenderContext();

	//TheApp_Startup( applicationInstanceHandle, commandLineString );		// #SD1ToDo: ...becomes:  g_theApp = new App();  AND  g_theApp->Startup();
	input= new InputSystem();
	g_theapp->Startup();

	// Program main loop; keep running frames until it's time to quit
	while( !g_theapp->isQuitting() )			// #SD1ToDo: ...becomes:  !g_theApp->IsQuitting()
	{
		
		//Sleep(16);
		g_theWindow->BeginFrame();
		g_theapp->RunFrame();
		RunFrame();					
	}

	g_theapp->Shutdown();				

	return 0;
}




