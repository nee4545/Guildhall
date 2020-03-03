#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/GPUMesh.hpp"

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
	//m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(160.f,90.f));
	m_camera->SetProjectionPerspective( 60.f ,16.f/9.f, -0.1f , -100.f );
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );

	m_camera->SetClearMode( CLEAR_COLOR_BIT , Rgba8( ( 0 , 0 , 0 , 0 ) , 0.f , 0 ));
   
	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

	/*g_theConsole.SetIsOpen( true );*/

	g_theEventSystem.SubscribeToEvent( "help" , Help );
	g_theEventSystem.SubscribeToEvent( "quit" , Quit );
	g_theEventSystem.SubscribeToEvent( "close" , Close);

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 2.5f );

	mesh = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCU> verices;
	AABB2 aabb = AABB2( 0.5f , 0.5f , 10.5f , 20.5f );

	Vertex_PCU vert1 = Vertex_PCU( Vec3( 0.5f , 0.5f,0.f), Rgba8( 200 , 0 , 100 , 255 ) , Vec2( 0.f , 0.f ) );
	Vertex_PCU vert2 = Vertex_PCU( Vec3( 10.5f , 0.5f , 0.f ) , Rgba8( 200 , 0 , 100 , 255 ) , Vec2( 1.f , 0.f ) );
	Vertex_PCU vert3 = Vertex_PCU( Vec3( 10.5f , 10.5f , 0.f ) , Rgba8( 200 , 0 , 100 , 255 ) , Vec2( 1.f , 1.f ) );
	Vertex_PCU vert4 = Vertex_PCU( Vec3( 0.5f , 10.5f , 0.f ) , Rgba8( 200 , 0 , 100 , 255 ) , Vec2( 0.f , 1.f ) );

	verices.push_back( vert1 );
	verices.push_back( vert2 );
	verices.push_back( vert3 );
	verices.push_back( vert4 );

	/*Vertex_PCU CubeVerts[ 24 ] = {
						Vertex_PCU( Vec3( 1.f,-1.f,1.f ) , WHITE, Vec2( 1.f, 0.f ) ),
						Vertex_PCU( Vec3( -1.f,-1.f,1.f ) , WHITE, Vec2( 0.f, 0.f ) ),

						Vertex_PCU( Vec3( -1.f,1.f,1.f ) , WHITE, Vec2( 1.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f,1.f,1.f ) , WHITE, Vec2( 1.f, 1.f ) ),

						Vertex_PCU( Vec3( 1.f,-1.f,-1.f ) , GREEN, Vec2( 1.f, 0.f ) ),
						Vertex_PCU( Vec3( -1.f,-1.f,-1.f ) , GREEN, Vec2( 0.f, 0.f ) ),

						Vertex_PCU( Vec3( -1.f,1.f,-1.f ) , GREEN, Vec2( 1.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f,1.f,-1.f )  , GREEN, Vec2( 1.f, 1.f ) ),

						Vertex_PCU( Vec3( 1.f,-1.f,1.f ) ,BLUE, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f,-1.f,-1.f ) ,BLUE, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( 1.f,1.f,-1.f ) , BLUE, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( 1.f,1.f,1.f ) ,BLUE, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( -1.f,-1.f,-1.f ) ,CYAN, Vec2( 1.f, 0.f ) ),
						Vertex_PCU( Vec3( -1.f,-1.f,1.f ) ,CYAN, Vec2( 0.f, 0.f ) ),

						Vertex_PCU( Vec3( -1.f,1.f,-1.f ) , CYAN, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( -1.f,1.f,1.f ) ,CYAN, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( -1.f, 1.f, 1.f ) ,RED, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f, 1.f, 1.f ) ,RED, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( -1.f,1.f, -1.f ) ,RED, Vec2( 1.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f,1.f,-1.f ) , RED, Vec2( 1.f, 1.f ) ),

						Vertex_PCU( Vec3( 1.f, -1.f, 1.f ) ,YELLOW, Vec2( 1.f, 0.f ) ),
						Vertex_PCU( Vec3( -1.f, -1.f, 1.f ) ,YELLOW, Vec2( 0.f, 0.f ) ),

						Vertex_PCU( Vec3( -1.f,-1.f, -1.f ) ,YELLOW, Vec2( 1.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f,-1.f,-1.f ) , YELLOW, Vec2( 1.f, 1.f ) ),
	};

	unsigned int CubeIndices[ 36 ] = {
							0,1,2,
							2,3,0,
							4,5,6,
							6,7,4,
							8,9,10,
							10,11,8,
							12,13,14,
							14,15,12,
							16,17,18,
							18,19,16,
							20,21,22,
							22,23,20,
	};*/


	//AppendAABB2( verices , aabb , Rgba8( 200 , 0 , 100 , 255 ) );

	mesh->UpdateVertices( (unsigned int)verices.size() , &verices[ 0 ] );

	int indices[ 6 ] = { 0,1,2,2,3,0 };
	mesh->UpdateIndices( 6 , indices );

	g_theInput->ClipSystemCursor();
	g_theInput->UnClipSystemCursor();
	g_theInput->SetCursorMode( MODE_RELATIVE );
}

Game::~Game()
{
	delete m_devConsoleCamera;
	delete m_camera;
	delete mesh;
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
	

	m_cameraRotation.y += g_theInput->m_relativeMovement.x;
	m_cameraRotation.x += g_theInput->m_relativeMovement.y;
	m_cameraRotation.y = Clamp( m_cameraRotation.y , -180.f , 180.f );
	m_cameraRotation.x = Clamp( m_cameraRotation.x , -180.f , 180.f );

	m_camera->m_transform.SetRotationFromPitchRollYawDegrees( m_cameraRotation.x , m_cameraRotation.y , m_cameraRotation.z );
	
	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		m_camera->m_transform.m_position.z += 1.f;
	}

	if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		m_camera->m_transform.m_position.z -= 1.f;
	}

	
	
	
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

	g_theRenderer->BindShader( nullptr );
	g_theRenderer->DrawMesh( mesh );
	

	g_theRenderer->DrawDisc( Vec2( 0.f , 0.f ) , 3.f , Rgba8( 255 , 0 , 0 , 255 ) );
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

































