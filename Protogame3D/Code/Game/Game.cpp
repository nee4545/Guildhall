#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Texture.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/ThirdParty/tinyxml2.h"

Game::Game()
{
	m_rng = RandomNumberGenerator();

	tinyxml2::XMLDocument gameConfig;
	gameConfig.LoadFile( "Data/XML/gameconfig.xml" );

	tinyxml2::XMLElement* ele = gameConfig.RootElement();
	std::string soundPath = ParseXmlAttribute( *ele , "soundPath" , "none" );

	testSound = g_theAudio->CreateOrGetSound( soundPath);


	m_camera = new Camera();
	m_camera->SetProjectionPerspective( 60.f , 16.f / 9.f , -0.09f , -100.f );

	m_UICamera = new Camera();
	m_UICamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );

	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

	m_camera->m_transform.SetConvenstion( CONVENSION_XEAST_YNORTH_ZUP );

	m_camera->m_transform.m_position  = Vec3( -10.f , 0.f , 0.f );

	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );
	m_UICamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );
	m_devConsoleCamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 2.5f );

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 2.5f );

	m_cube = new GPUMesh(g_theRenderer);
	m_basis = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCU> cube;
	std::vector<unsigned int> cubeIndices;
	AppendCuboidV2( cube ,cubeIndices, AABB3(Vec3(0.f,0.f,0.f),Vec3(1.f,1.f,1.f)) , Rgba8( 255 , 255 , 255 , 255 ) );

	m_cube->UpdateVertices( ( unsigned int) cube.size() , &cube[ 0 ] );
	m_cube->UpdateIndices( ( unsigned int ) cubeIndices.size() , &cubeIndices[ 0 ] );
	
	cubeTransform1.m_position = Vec3( 2.f , 0.f , 0.f );
	cubeTransform2.m_position = Vec3( 0.f , 2.f , 0.f );
	cubeTransform3.m_position = Vec3( 2.f , 2.f , 0.f );

	Mat44 cameraModel = m_camera->m_transform.ToMatrix();

	
	AppendArrow( m_basisVerts , Vec3(0.f,0.f,0.f) , Vec3(0.f,0.f,0.f) + cameraModel.GetIBasis3D()  , 0.03f , 0.01f , 0.02f , Rgba8(100,0,0,255) , Rgba8( 100 , 0 , 0 , 255 ) );
	AppendArrow( m_basisVerts , Vec3(0.f,0.f,0.f) , Vec3(0.f,0.f,0.f) + cameraModel.GetJBasis3D()  , 0.03f , 0.01f , 0.02f , Rgba8(0,100,0,255) , Rgba8( 0 , 100 , 0 , 255 ) );
	AppendArrow( m_basisVerts , Vec3(0.f,0.f,0.f) , Vec3(0.f,0.f,0.f) + cameraModel.GetKBasis3D()  , 0.03f , 0.01f , 0.02f , Rgba8(0,0,100,255) , Rgba8( 0 , 0 , 100 , 255 ) );
	
	m_basis->UpdateVertices( ( unsigned int ) m_basisVerts.size() , &m_basisVerts[ 0 ] );
	

	g_theInput->ClipSystemCursor();
	g_theInput->SetCursorMode( MODE_RELATIVE );

}

Game::~Game()
{
	delete m_camera;
	delete m_UICamera;
	delete m_devConsoleCamera;
	delete m_cube;
	delete m_basis;
}

void Game::Update( float deltaseconds )
{
	ToggleDevConsole();
	SoundTest();

	Mat44 cameraTransform = m_camera->m_transform.ToMatrix();
	Vec3 forwardVector = cameraTransform.GetIBasis3D();
	forwardVector.z			= 0.f;
	Vec3 rightVector = -cameraTransform.GetJBasis3D();
	rightVector.z = 0.f;
	Vec3 upMovement = Vec3(0.f,0.f,1.f);

	float speed = 4.f;
	if ( g_theInput->IsKeyPressed( SHIFT ) )
	{
		speed = 10.f;
	}

	if ( g_theInput->IsKeyPressed( 'A' ) )
	{
		m_camera->SetPosition( m_camera->m_transform.m_position - rightVector * speed * deltaseconds );
	}

	if ( g_theInput->IsKeyPressed( 'D' ) )
	{
		m_camera->SetPosition( m_camera->m_transform.m_position + rightVector * speed * deltaseconds );
	}

	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		Vec3 newPos = m_camera->m_transform.m_position + forwardVector * speed * deltaseconds;
		m_camera->SetPosition( newPos );
	}

	if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		Vec3 newPos = m_camera->m_transform.m_position - forwardVector * speed * deltaseconds;
		m_camera->SetPosition( newPos );
	}

	if ( g_theInput->IsKeyPressed( 'Q' ) )
	{
		m_camera->SetPosition( m_camera->m_transform.m_position - upMovement * speed * deltaseconds );
	}

	if ( g_theInput->IsKeyPressed( 'E' ) )
	{
		m_camera->SetPosition( m_camera->m_transform.m_position + upMovement * speed * deltaseconds );
	}

	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		Vec3 position = Vec3( 0.f , 0.f , 0.f );
		m_camera->SetPosition( position );
		m_pitch = 0.f;
		m_yaw = 0.f;
	}

	Vec2 mousePos = g_theInput->m_relativeMovement;

	m_yaw -= mousePos.x * speed * deltaseconds;
	m_pitch += mousePos.y * speed * deltaseconds;
	m_pitch = Clamp( m_pitch , -89.9f , 89.9f );
	
	float finalRoll = 0.f;
	m_camera->m_transform.SetRotationFromYawPitchRollDegrees( m_yaw ,m_pitch, finalRoll );
}

void Game::Render()
{
	g_theRenderer->BeginCamera( *m_camera );
	m_camera->CreateDepthStencilTarget( g_theRenderer );
	g_theRenderer->SetDepthTest();
	g_theRenderer->BindDepthStencil( m_camera->m_backBuffer );

	g_theRenderer->CreateRasterState( SOLID , CULL_BACK );
	Texture* tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/gg.png" );
	
	g_theRenderer->BindShader( "Data/Shaders/CustomShader.hlsl" );
	g_theRenderer->BindTexture( tex );
	g_theRenderer->SetModalMatrix( cubeTransform1.ToMatrix() );
	g_theRenderer->DrawMesh( m_cube );

	g_theRenderer->SetModalMatrix( cubeTransform2.ToMatrix() );
	g_theRenderer->DrawMesh( m_cube );

	g_theRenderer->SetModalMatrix( cubeTransform3.ToMatrix() );
	g_theRenderer->DrawMesh( m_cube );

	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->SetDepthTest( COMPARE_ALWAYS );
	g_theRenderer->SetModalMatrix( Mat44() );
	g_theRenderer->DrawMesh( m_basis );

	Transform compassTransform;
	compassTransform.SetPosition( m_camera->m_transform.m_position + 0.1f * m_camera->m_transform.ToMatrix().GetIBasis3D() );
	compassTransform.m_scale = Vec3( 0.01f , 0.01f , 0.01f );

	g_theRenderer->SetModalMatrix( compassTransform.ToMatrix() );
	g_theRenderer->DrawMesh( m_basis );
	
	

	g_theRenderer->EndCamera( *m_camera );

	DisplayUI();

	if ( g_theConsole.IsOpen() )
	{
		g_theConsole.Render( *g_theRenderer , *m_devConsoleCamera , 2.5f , 1.5f );
	}
	

	DebugRenderSystem::sDebugRenderer->DebugRenderWorldToCamera( m_camera );
}

void Game::ToggleDevConsole()
{
	if ( g_theInput->WasKeyJustPressed( 0xC0 ) )
	{
		devConsoleOpen = !devConsoleOpen;
		g_theConsole.SetIsOpen( devConsoleOpen );
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

void Game::DisplayUI()
{
	AABB2 box = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );

	std::string s1 = "Camera Yaw";
	std::string s2 = "Camera Pitch";
	std::string s3 = "Camera Roll";
	std::string s4 = "xyz";

	std::string s5 = "iBasis";
	std::string s6 = "jBasis";
	std::string s7 = "kBasis";

	std::vector<Vertex_PCU> v1;
	std::string temp = s1;
	temp += " " + std::to_string( m_yaw ) + " " +s2 + " " + std::to_string(m_pitch) + " " + s3 + " " + std::to_string(m_roll);
	temp += " " + s4 + "(" + std::to_string( m_camera->m_transform.m_position.x ) + "," + std::to_string( m_camera->m_transform.m_position.y ) + "," + std::to_string( m_camera->m_transform.m_position.z ) + ")";

	m_font->AddVertsForTextInBox2D( v1 , box , 1.f , temp , Rgba8( 100 , 100 , 0 , 255 ) , 1.f , Vec2( 0.03f , 0.98f ) );

	Mat44 cameraModel = m_camera->m_transform.ToMatrix();
	Vec3 iBasis = cameraModel.GetIBasis3D();
	Vec3 jBasis = cameraModel.GetJBasis3D();
	Vec3 kBasis = cameraModel.GetKBasis3D();

	std::vector<Vertex_PCU> v2;
	temp = "";
	temp += "= (" + std::to_string( iBasis.x ) + "," + std::to_string( iBasis.y ) + "," + std::to_string( iBasis.z ) +")";
	s5 += temp;

	m_font->AddVertsForTextInBox2D( v2 , box , 1.f , s5 , Rgba8( 255 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.96f ) );

	std::vector<Vertex_PCU> v3;
	temp = "";
	temp += "= (" + std::to_string( jBasis.x ) + "," + std::to_string( jBasis.y ) + "," + std::to_string( jBasis.z ) + ")";
	s6 += temp;

	m_font->AddVertsForTextInBox2D( v3 , box , 1.f , s6 , Rgba8( 0 , 255 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.94f ) );

	std::vector<Vertex_PCU> v4;
	temp = "";
	temp += "= (" + std::to_string( kBasis.x ) + "," + std::to_string( kBasis.y ) + "," + std::to_string( kBasis.z ) + ")";
	s7 += temp;

	m_font->AddVertsForTextInBox2D( v3 , box , 1.f , s7 , Rgba8( 0 , 0 , 255 , 255 ) , 1.f , Vec2( 0.02f , 0.92f ) );

	g_theRenderer->BeginCamera( *m_UICamera );

	g_theRenderer->BindTexture( m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( v1 );
	g_theRenderer->DrawVertexArray( v2 );
	g_theRenderer->DrawVertexArray( v3 );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->EndCamera( *m_UICamera );
}

void Game::SoundTest()
{
	SoundPlaybackID s = 0;
	float volume = 0.f;
	float balance = 0.f;
	float speed = 0;

	volume = m_rng.RollRandomFloatInRange( 0.5f , 1.0f );
	balance = m_rng.RollRandomFloatInRange( -1.f , 1.f );
	speed = m_rng.RollRandomFloatInRange( 0.5f , 2.0f );

	if ( g_theInput->WasKeyJustPressed( F1 ) )
	{
		s = g_theAudio->PlaySound( testSound , false , volume , balance , speed );
	}
}
