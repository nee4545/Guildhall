#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"

#define UNUSED(x) (void)(x);


bool Help( EventArgs& args )
{
	UNUSED( args );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Currently Supported Commands" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "help");
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "quit" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "close" );
	return false;
}

bool Quit( EventArgs& args )
{
	UNUSED( args );
	g_theapp->HandleQuitRequested();
	return false;
}


bool Close( EventArgs& args )
{
	UNUSED( args );
	g_theConsole.SetIsOpen( false );
	return false;
}


Game::Game()
{
	rng= RandomNumberGenerator();
	m_camera=new Camera();
	m_devConsoleCamera = new Camera();
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(160.f,90.f));
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );
   
	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

	/*g_theConsole.SetIsOpen( true );*/

	g_theEventSystem.SubscribeToEvent( "help" , Help );
	g_theEventSystem.SubscribeToEvent( "quit" , Quit );
	g_theEventSystem.SubscribeToEvent( "close" , Close);

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetSize( 2.5f );

}

Game::~Game()
{
	delete m_devConsoleCamera;
	delete m_camera;
}

void Game::Update( float deltaseconds )
{
	ToggleDevConsole();
	if ( g_theConsole.IsOpen() )
	{
		return;
	}

	if ( g_theInput->WasKeyJustPressed( 0x1B ) )
	{
		g_theapp->HandleQuitRequested();
	}

	UNUSED( deltaseconds );
	
	m_rColorValue = 1.f;


	m_camera->SetClearMode( CLEAR_COLOR_BIT , Rgba8( (unsigned char)(100 * m_rColorValue) , 0 , 0 , 0 ) , 0.f , 0 );
	
}

void Game::Render()
{

	g_theRenderer->BeginCamera(*m_camera);
	
	tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/asd.png" );

	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( tex );
	g_theRenderer->DrawAABB2D( AABB2( 10.5f , 10.5f , 20.5f , 20.5f ) , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindShader( "Data/Shaders/Inverse.hlsl" );
	g_theRenderer->BindTexture( tex );
	g_theRenderer->DrawAABB2D( AABB2( 20.5f , 20.5f , 30.5f , 30.5f ) , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );


	g_theRenderer->EndCamera(*m_camera);


	if ( g_theConsole.IsOpen() )
	{
		g_theRenderer->BeginCamera( *m_devConsoleCamera );
		g_theConsole.Render( *g_theRenderer , *m_devConsoleCamera , 2.5f , 1.5f );
		g_theRenderer->EndCamera( *m_devConsoleCamera );
	}


}



void Game::UpdateCamera()
{
	
}

void Game::ToggleDebugCamera()
{
	
}

void Game::ToggleDevConsole()
{
	if ( g_theInput->WasKeyJustPressed( 0xC0 ) )
	{
		devConsoleOpen = !devConsoleOpen;
		g_theConsole.SetIsOpen(devConsoleOpen);
	}
}

void Game::ToggleDebugging()
{
	
}

































