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
#include "Engine/Core/AABB3.hpp"
#include "Engine/Renderer/DebugRender.hpp"

#define UNUSED(x) (void)(x);

eDebugRenderMode currentMode = DEBUG_RENDER_USE_DEPTH;


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
	m_devConsoleCamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0, 0 , 255 ) , 0.f , 0 );
	m_camera->SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );


	g_theEventSystem.SubscribeToEvent( "help" , Help );
	g_theEventSystem.SubscribeToEvent( "quit" , Quit );
	g_theEventSystem.SubscribeToEvent( "close" , Close);

	//DebugRenderSystem::sDebugRenderer->TakeWorldCamera( m_camera );

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

	tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/gg.png" );
	
	

	std::string text = "Pink Floyd > all music";
	std::string s1 = "Press B to add a world Point";
	std::string s2 = "Press N to add a world Line";
	std::string s3 = "Press M to add a world Arrow";
	std::string s4 = "Press K to add a wire sphere";
	std::string s5 = "Press L to add a world Quad";
	std::string s6 = "press P to add a world Basis ";
	std::string s7 = "Press O to add text and billboard text";
	std::string s8 = "Default mode is DEUBUG_RENDER_USE_DEPTH";
	std::string s9 = "Press 1: RENDER_ALWAYS, 2: RENDER_DEPTH, 3: RENDER_XRAY";
	std::string s10 = "U can already see some screen renders for 5-7 seconds";
	std::string s11 = "Press J to add a wire box";

	DebugAddScreenText( Vec4( 10.f , 0.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) ,30.f, Rgba8(100,0,0,255),75.f,s11.c_str());
	DebugAddScreenText( Vec4( 10.f , 30.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , 75.f , s2.c_str() );
	DebugAddScreenText( Vec4( 10.f , 60.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , 75.f , s3.c_str() );
	DebugAddScreenText( Vec4( 10.f , 90.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , 75.f , s1.c_str() );
	DebugAddScreenText( Vec4( 10.f , 120.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , 75.f , s4.c_str() );
	DebugAddScreenText( Vec4( 10.f , 150.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , 75.f , s5.c_str() );
	DebugAddScreenText( Vec4( 10.f , 180.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , 75.f , s6.c_str() );
	DebugAddScreenText( Vec4( 10.f , 210.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , 75.f , s7.c_str() );
	DebugAddScreenText( Vec4( 10.f , 870.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , -1.f , s8.c_str() );
	DebugAddScreenText( Vec4( 10.f , 900.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , -1.f , s9.c_str() );
	DebugAddScreenText( Vec4( 10.f , 840.f , 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 30.f , Rgba8( 100 , 0 , 0 , 255 ) , 10.f , s10.c_str() );

	DebugAddScreenPoint( Vec2( 500.f , 500.f ) , 20.f , Rgba8( 0 , 0 , 100 , 255 ) , 3.f );
	DebugAddScreenLine( Vec2( 600.f , 700.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Rgba8( 0 , 0 , 100 , 255 ) , Vec2( 800.f , 800.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Rgba8( 0 , 0 , 100 , 255 ) , 12.f , 5.f );
	DebugAddScreenArrow( Vec2( 200.f , 300.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Rgba8( 0 , 0 , 100 , 255 ) , Vec2( 600.f , 500.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Rgba8( 0 , 0 , 100 , 255 ) , 12.f , 12.f,5.f );
	DebugAddScreenTexturedQuad( AABB2( 300.f , 300.f , 400.f , 400.f ) , tex ,AABB2(0.f,0.f,1.f,1.f) ,Rgba8( 255 , 255 , 255 , 255 ) , 6.f );
	
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
	ToggleRenderModes();

	if ( g_theInput->WasKeyJustPressed( 'B' ) )
	{
		//DebugAddWorldPoint( m_camera->m_transform , 1.f , Rgba8( 255 , 255 , 255 , 255 ) , 50.f, DEBUG_RENDER_XRAY );
		DebugAddWorldPoint( Vec3( 0.f , 0.f , 0.f ) , 2.f , Rgba8( 100 , 0 , 0 , 255 ) , Rgba8( 0 , 0 , 100 , 255 ) , 4.f, currentMode );
	}

	if ( g_theInput->WasKeyJustPressed( 'N' ) )
	{
		//DebugAddWorldLine( Vec3( 0.f , 0.f , 0.f ) , Vec3( 4.f , 3.f , 1.f ) , Rgba8( 0 , 0 , 255 , 255 ) , 2.f , 4.f );
		DebugAddWorldLine( Vec3( 0.f , 0.f , 0.f ) , Vec3( 4.f , 3.f , 0.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Rgba8( 0 , 100 , 0 , 255 ) , Rgba8( 100 , 100 , 0 , 255 ),Rgba8(0,0,100,255) , 2.f , 5.f,currentMode );
	}

	if ( g_theInput->WasKeyJustPressed( 'M' ) )
	{
		//DebugAddWorldArrow( Vec3( 0.f , 0.f , 0.f ) , Vec3( 4.f , 3.f , 1.f ) , Rgba8( 100 , 0 , 0 , 255 ) , 3.f , 2.f );
		DebugAddWorldArrow( Vec3( 0.f , 0.f , 0.f ) , Vec3( 4.f , 3.f , 1.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Rgba8( 0 , 100 , 0 , 255 ) , Rgba8( 100 , 100 , 0 , 255 ) , Rgba8( 0 , 0 , 100 , 255 ) , 3.f , 2.f ,currentMode);
	}

	if ( g_theInput->WasKeyJustPressed( 'K' ) )
	{
		DebugAddWorldWireSphere( Vec3( 0.f , 0.f , 0.f ) , 2.f , Rgba8( 100 , 0 , 0 , 255 ) , Rgba8( 0 , 0 , 100 , 255 ) , 4.f ,currentMode);
	}

	if ( g_theInput->WasKeyJustPressed( 'L' ) )
	{
		DebugAddWorldQuad( Vec3( 0.f , 0.f , 0.f ) , Vec3( 2.f , 0.f , 4.f ) , Vec3( 3.f , 3.f , 0.f ) , Vec3( 0.f , 4.f , 0.f ) , Rgba8( 0 , 0 , 100 , 255 ),Rgba8(100,0,0,255) , 4.f ,currentMode);
		//DebugAddWorldQuad( Vec3( 0.f , 0.f , 0.f ) , Vec3( 2.f , 0.f , 0.f ) , Vec3( 2.f , 2.f , 0.f ) , Vec3( 0.f , 2.f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ),tex, 4.f );
	}


	if ( g_theInput->WasKeyJustPressed( 'J' ) )
	{
		DebugAddWorldWireBounds( AABB3( 1.3f , 2.9f , 3.6f , 5.8f , 9.2f , 13.8f ) , Rgba8( 100 , 0 , 0 , 255 ) , 4.f,currentMode );
		//DebugAddWorldQuad( Vec3( 0.f , 0.f , 0.f ) , Vec3( 2.f , 0.f , 0.f ) , Vec3( 2.f , 2.f , 0.f ) , Vec3( 0.f , 2.f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ),tex, 4.f );
	}

	if ( g_theInput->WasKeyJustPressed( 'P' ) )
	{
		Transform temp = Transform();
		temp.m_position.z = -9.f;
		temp.m_position.x = -5.f;
		Mat44 basis = m_camera->m_transform.ToMatrix();
		basis.TransformBy( temp.ToMatrix() );
		DebugAddWorldBasis( basis , Rgba8( 100 , 100 , 100 , 255 ) , 6.f,currentMode );
	}

	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		Transform temp = Transform();
		temp.m_position.z = -7.f;
		temp.m_position.x = -5.f;
		Mat44 basis = m_camera->m_transform.ToMatrix();
		basis.TransformBy( temp.ToMatrix() );
		std::string t = "Coming back to life";
		DebugAddWorldText( basis , Vec2( 0.f , 0.f ) , 0.5f , Rgba8( 255 , 255 , 255 , 255 ) , Rgba8( 255 , 255 , 255 , 255 ) , 10.f , currentMode , t.c_str() );
		DebugAddWorldBillboardText( basis.GetTranslation3D()+Vec3(0.f,2.f,0.f) , Vec2( 0.f , 0.f ) , 1.f , Rgba8( 100 , 0 , 0 , 255 ) , Rgba8( 0 , 0 , 100 , 255 ),10.f , currentMode ,t.c_str());
	}

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

	m_cameraRotation.y += g_theInput->m_relativeMovement.x * 0.1f;
	m_cameraRotation.x += g_theInput->m_relativeMovement.y * 0.1f;
	m_cameraRotation.y = Clamp( m_cameraRotation.y , -90.f , 90.f );
	m_cameraRotation.x = Clamp( m_cameraRotation.x , -85.f , 85.f );

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

	DebugRenderSystem::sDebugRenderer->DebugRenderWorldToCamera( m_camera );
	DebugRenderSystem::sDebugRenderer->DebugRenderToScreen( m_camera->GetColorTarget() );
}

void Game::UpdateCamera()
{
	
}

void Game::ToggleDebugCamera()
{
	
}

void Game::ToggleRenderModes()
{
	if ( g_theInput->WasKeyJustPressed( '1' ) )
	{
		renderMode = 1;
		currentMode = DEBUG_RENDER_ALWAYS;
	}

	if ( g_theInput->WasKeyJustPressed( '2' ) )
	{
		renderMode = 2;
		currentMode = DEBUG_RENDER_USE_DEPTH;
	}

	if ( g_theInput->WasKeyJustPressed( '3' ) )
	{
		renderMode = 3;
		currentMode = DEBUG_RENDER_XRAY;
	}
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

































