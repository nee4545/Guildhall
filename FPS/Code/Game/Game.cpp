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

	m_camera->SetClearMode( CLEAR_COLOR_BIT , Rgba8(  0 , 0 , 0 , 0 ) );
   
	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );


	g_theEventSystem.SubscribeToEvent( "help" , Help );
	g_theEventSystem.SubscribeToEvent( "quit" , Quit );
	g_theEventSystem.SubscribeToEvent( "close" , Close);

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 2.5f );

	mesh = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCU> verices;

	Rgba8 WHITE = Rgba8( 255 , 255 , 255 , 255 );
	Rgba8 GREEN = Rgba8( 0 , 255 , 0 , 255 );
	Rgba8 BLUE = Rgba8( 0 , 0 , 255 , 255 );
	Rgba8 CYAN = Rgba8( 0 , 255 , 255 , 255 );
	Rgba8 RED = Rgba8( 255 , 0 , 0 , 255 );
	Rgba8 YELLOW = Rgba8( 255 , 255 , 0 , 255 );


	Vertex_PCU cube[] =
	{
		Vertex_PCU( Vec3( -0.5f,-0.5f,-0.5f ) , WHITE, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,-0.5f,-0.5f ) , WHITE, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,-0.5f ) , WHITE, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,-0.5f ) , WHITE, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( -0.5f,-0.5f,0.5f ) , WHITE, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,-0.5f,0.5f ) , WHITE, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,0.5f ) , WHITE, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,0.5f ) , WHITE, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( -0.5f,-0.5f,-0.5f ) , WHITE, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( -0.5f,-0.5f,0.5f ) , WHITE, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,0.5f ) , WHITE, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,-0.5f ) , WHITE, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( 0.5f,-0.5f,-0.5f ) , WHITE, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,-0.5f,0.5f ) , WHITE, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,0.5f ) , WHITE, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,-0.5f ) , WHITE, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( -0.5f, 0.5f, 0.5f ) , WHITE, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f, 0.5f, 0.5f ) , WHITE, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,-0.5f ) , WHITE, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,-0.5f ) , WHITE, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( -0.5f, -0.5f, 0.5f ) , WHITE, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f, -0.5f, 0.5f ) , WHITE, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,-0.5f,-0.5f ) , WHITE, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( -0.5f,-0.5f,-0.5f ) , WHITE, Vec2( 0.f, 1.f ) ),

	};

	

	for ( int index = 0; index < 24; index++ )
	{
		verices.push_back( cube[ index ] );
	}


	unsigned int cubeInd[] =
	{
		0,1,2,
		0,2,3,
		4,5,6,
		4,6,7,
		8,9,10,
		8,10,11,
		12,13,14,
		12,14,15,
		16,17,18,
		16,18,19,
		20,21,22,
		20,22,23
	};


	mesh->UpdateVertices( (unsigned int)verices.size() , &verices[ 0 ] );
	mesh->UpdateIndices( 36 , cubeInd );

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

	cubeTransform.m_rotationPitchRollYawDegrees.x += deltaseconds*10.f;

	if ( cubeTransform.m_rotationPitchRollYawDegrees.x >= 360.f )
	{
		cubeTransform.m_rotationPitchRollYawDegrees.x = 0.f;
	}

	//cubeTransform.SetRotationFromPitchRollYawDegrees( 0.f , 30.f , 0.f );

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
	g_theRenderer->BindTexture( tex );
	g_theRenderer->SetModalMatrix( cubeTransform.ToMatrix() );
	g_theRenderer->DrawMesh( mesh );
	

	//g_theRenderer->DrawDisc( Vec2( 0.f , 0.f ) , 3.f , Rgba8( 255 , 0 , 0 , 255 ) );
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

































