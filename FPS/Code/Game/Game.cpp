#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Core/Time.hpp"

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

	m_camera->SetProjectionPerspective( 60.f ,16.f/9.f, -0.1f , -100.f );
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );
	m_devConsoleCamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 127 , 127, 127 , 255 ) , 0.f , 0 );
	m_camera->SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );


	g_theEventSystem.SubscribeToEvent( "help" , Help );
	g_theEventSystem.SubscribeToEvent( "quit" , Quit );
	g_theEventSystem.SubscribeToEvent( "close" , Close);

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 2.5f );

	Rgba8 WHITE = Rgba8( 255 , 255 , 255 , 255 );
	Rgba8 GREEN = Rgba8( 0 , 255 , 0 , 255 );
	Rgba8 BLUE = Rgba8( 0 , 0 , 255 , 255 );
	Rgba8 CYAN = Rgba8( 0 , 255 , 255 , 255 );
	Rgba8 RED = Rgba8( 255 , 0 , 0 , 255 );
	Rgba8 YELLOW = Rgba8( 255 , 255 , 0 , 255 );

	mesh = new GPUMesh( g_theRenderer );
	sphere = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCU> sphereVerts;
	std::vector<unsigned int> sphereIndices;
	Vec3 centre = Vec3( 0.f , 0.f , 1.f );

	AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , 3.f , 64 , 32 , WHITE );

	sphere->UpdateVertices((unsigned int) sphereVerts.size() , &sphereVerts[0] );
	sphere->UpdateIndices((unsigned int) sphereIndices.size() , &sphereIndices[0] );


	std::vector<Vertex_PCU> verices;

	Vertex_PCU cube[] =
	{
		Vertex_PCU( Vec3( -0.5f,-0.5f,-0.5f ) , WHITE, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,-0.5f,-0.5f ) , WHITE, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,-0.5f ) , WHITE, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,-0.5f ) , WHITE, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( -0.5f,-0.5f,0.5f ) ,GREEN, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,-0.5f,0.5f ) , GREEN, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,0.5f ) , GREEN, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,0.5f ) , GREEN, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( -0.5f,-0.5f,-0.5f ) ,BLUE, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( -0.5f,-0.5f,0.5f ) , BLUE, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,0.5f ) , BLUE, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,-0.5f ) , BLUE, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( 0.5f,-0.5f,-0.5f ) ,RED, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,-0.5f,0.5f ) , RED, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,0.5f ) , RED, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,-0.5f ) , RED, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( -0.5f, 0.5f, 0.5f ) ,YELLOW, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f, 0.5f, 0.5f ) , YELLOW, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.5f,-0.5f ) , YELLOW, Vec2( 1.f, 1.f ) ),
		Vertex_PCU( Vec3( -0.5f,0.5f,-0.5f ) , YELLOW, Vec2( 0.f, 1.f ) ),

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
	g_theInput->SetCursorMode( MODE_RELATIVE );
}

Game::~Game()
{
	delete m_devConsoleCamera;
	delete m_camera;
	delete mesh;
	delete sphere;
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

	float speedMultiplier = 1.f;
	if ( g_theInput->IsKeyPressed( 0x10 ) )
	{
		speedMultiplier = 2.f;
	}
	else
	{
		speedMultiplier = 1.f;
	}
	

	m_cameraRotation.y += g_theInput->m_relativeMovement.x*0.1f;
	m_cameraRotation.x += g_theInput->m_relativeMovement.y*0.1f;
	m_cameraRotation.y = Clamp( m_cameraRotation.y , -90.f , 90.f );
	m_cameraRotation.x = Clamp( m_cameraRotation.x , -85.f , 85.f );

	cubeTransform.m_rotationPitchRollYawDegrees.x += deltaseconds*10.f;

	if ( cubeTransform.m_rotationPitchRollYawDegrees.x >= 360.f )
	{
		cubeTransform.m_rotationPitchRollYawDegrees.x = 0.f;
	}

	cubeTransform.m_position = Vec3( 1.f , 0.5f , -12.f );
	
	sphereTransform.SetRotationFromPitchRollYawDegrees( 0.f , 10.f * ( float ) GetCurrentTimeSeconds() , 0.f );

	m_camera->m_transform.SetRotationFromPitchRollYawDegrees( m_cameraRotation.x , m_cameraRotation.y , m_cameraRotation.z );

	Mat44 modal = m_camera->m_transform.ToMatrix();
	Vec3 forwardVec = modal.GetKBasis3D();
	Vec3 rightVec = modal.GetIBasis3D();
	
	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		//m_camera->m_transform.m_position.z -= 1.f * deltaseconds * 4.f *speedMultiplier;
		m_camera->m_transform.m_position -= forwardVec * deltaseconds * 8.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		m_camera->m_transform.m_position += forwardVec * deltaseconds * 8.f * speedMultiplier;
		//m_camera->m_transform.m_position.z += 1.f * deltaseconds * 4.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 'A' ) )
	{
		//m_camera->m_transform.m_position.x -= 1.f * deltaseconds * 4.f * speedMultiplier;
		m_camera->m_transform.m_position -= rightVec * deltaseconds * 8.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 'D' ) )
	{
		m_camera->m_transform.m_position += rightVec * deltaseconds * 8.f * speedMultiplier;
		//m_camera->m_transform.m_position.x += 1.f * deltaseconds * 4.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 'C' ) )
	{
		m_camera->m_transform.m_position.y += 1.f * deltaseconds * 8.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 0x20 ) )
	{
		m_camera->m_transform.m_position.y -= 1.f * deltaseconds * 8.f * speedMultiplier;
	}

}

void Game::Render()
{
	

	g_theRenderer->BeginCamera(*m_camera);
	m_camera->CreateDepthStencilTarget( g_theRenderer );
	g_theRenderer->SetDepthTest();
	g_theRenderer->BindDepthStencil( m_camera->m_backBuffer );
	
	tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/gg.png" );

	Transform quadTransform;
	quadTransform.m_position.z = -10.f;
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->SetModalMatrix( quadTransform.ToMatrix() );
	g_theRenderer->DrawAABB2D( AABB2( Vec2( 0.f , 0.f ) , Vec2( 1.f , 1.f ) ),Rgba8(100,100,100,255) );

	g_theRenderer->BindShader( nullptr );
	g_theRenderer->SetModalMatrix( cubeTransform.ToMatrix() );
	g_theRenderer->DrawMesh( mesh );

	g_theRenderer->BindShader( nullptr );
	//g_theRenderer->SetModalMatrix( sphereTransform.ToMatrix() );

	float deltaPhi = 360.f / 10.f;
	Transform ring;

	for ( float index = 0; index <= 360.f ; index += deltaPhi )
	{
		Vec3 position = GetSphericalCoordinates( 0.f , 20.f * (float)GetCurrentTimeSeconds() +  index , 15.f );
		position.z -= 30.f;
		ring.SetPosition( position );
		ring.SetRotationFromPitchRollYawDegrees( 0.f , 20.f * (float)GetCurrentTimeSeconds() + index , 0.f );
		g_theRenderer->SetModalMatrix( ring.ToMatrix() );
		g_theRenderer->BindTexture( tex );
		g_theRenderer->DrawMesh( sphere );
	}

	
	g_theRenderer->EndCamera(*m_camera);

	if ( g_theConsole.IsOpen() )
	{
		g_theConsole.Render( *g_theRenderer , *m_devConsoleCamera , 2.5f , 1.5f );
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

	if ( g_theConsole.IsOpen() )
	{
		g_theInput->UnClipSystemCursor();
		g_theInput->SetCursorMode( MODE_ABSOLUTE );
		g_theInput->ShowSystemCursor();
	}
	else
	{
		g_theInput->ClipSystemCursor();
		g_theInput->SetCursorMode( MODE_RELATIVE );
		g_theInput->HideSystemCursor();
	}
}

void Game::ToggleDebugging()
{
	
}

































