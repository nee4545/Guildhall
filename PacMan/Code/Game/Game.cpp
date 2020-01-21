#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Entity.hpp"
#include "Engine/DevConsole.hpp"
#include "Engine/Renderer/BitmapFont.hpp"


DevConsole dev = DevConsole();

Game::Game()
{
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 53.33f , 30.f ) );
	m_UICamera = new Camera();
	m_UICamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_XMAX , UI_YMaX ) );
	m_BitmapFont = g_theRenderer->CreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	//m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 10.f , 10.f ) );
	TileDefinition::CreateTileDefinitions();
	m_map = new Map( this , IntVec2( 30 , 30 ) );
	m_map->StartUp();
}

void Game::Update( float deltaseconds )
{
	HandleStates();

	if ( m_state == STATE_PLAYING )
	{
		m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 53.33f , 30.f ) );
		m_map->Update( deltaseconds );
		UpdateCamera();
	}


}

void Game::Render()
{
	if ( m_state == STATE_PLAYING )
	{
		g_theRenderer->BeginCamera( *m_camera );
		m_map->Render();
		g_theRenderer->EndCamera( *m_camera );
		RenderUI();
	}

	if ( m_state == STATE_START )
	{
		DisplayStartScreen();
	}

	if ( m_state == STATE_GAMEOVER )
	{
		DisplayGameOverScreen();
	}

	if ( m_state == STATE_WIN )
	{
		DisplayWinScreen();
	}
}

void Game::UpdateCamera()
{
	Vec2 tankPos = m_map->GetPlayerPosition();
	Vec2 camCoords;

	camCoords.x = Clamp( tankPos.x , 8.f*1.75f , WORLD_SIZE_X - (8.f*1.75f) );
	camCoords.y = Clamp( tankPos.y , 4.5f*1.75f , WORLD_SIZE_Y - (4.5f*1.75f) );

	m_camera->SetOrthoView( camCoords - Vec2( 8.f*1.75f , 4.5f*1.75f ) , camCoords + Vec2( 8.f*1.75f , 4.5f*1.75f ) );
}

void Game::ChangeState( GameStates newState )
{
	m_state = newState;
}

void Game::DisplayStartScreen()
{
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 53.3f*2.f , 30.f*2.f ) );
	g_theRenderer->BeginCamera( *m_camera );

	AABB2 box = AABB2( Vec2( 0.f , 0.f ) , Vec2( 53.3f * 2.f , 30.f * 2.f ) );
	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" );
	SpriteSheet* spriteSheet = new SpriteSheet( *texture , IntVec2( 8 , 8 ) );
	const SpriteDefinition& boxSprite = spriteSheet->GetSpriteDefinition( 13 );
	Vec2 minUVs;
	Vec2 maxUVs;
	boxSprite.GetUVs( minUVs , maxUVs );

	std::vector<Vertex_PCU> boxVertices;

	AppendAABB2( boxVertices , box , Rgba8( 255 , 255 , 255 , 50 ) , minUVs , maxUVs );

	g_theRenderer->BindTexture( texture );
	g_theRenderer->DrawVertexArray( boxVertices );
	g_theRenderer->BindTexture( nullptr );

	Texture* bearTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/bbear.png" );
	AABB2 bearBox = AABB2( 45.f , 30.f , 55.f , 40.f );

	g_theRenderer->BindTexture( bearTex );
	g_theRenderer->DrawAABB2D( bearBox , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* titleTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/title.png" );
	AABB2 titleBox = AABB2( 35.f , 48.f , 65.f , 58.f );

	g_theRenderer->BindTexture( titleTex );
	g_theRenderer->DrawAABB2D( titleBox , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* controlsTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/controls.png" );
	AABB2 controlBox = AABB2( 35.f , 5.f , 65.f , 25.f );

	g_theRenderer->BindTexture( controlsTex );
	g_theRenderer->DrawAABB2D( controlBox , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );
	
	
	
}

void Game::RenderUI()
{
	g_theRenderer->BeginCamera( *m_UICamera );

	float offset = 50.f;
	Vertex_PCU iconVertices[ 6 ];
	Vertex_PCU iconVertices_original[ 6 ];

	iconVertices[ 0 ] = Vertex_PCU( Vec3( -0.6f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	iconVertices[ 1 ] = Vertex_PCU( Vec3( 0.6f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	iconVertices[ 2 ] = Vertex_PCU( Vec3( -0.6f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	iconVertices[ 3 ] = Vertex_PCU( Vec3( 0.6f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	iconVertices[ 4 ] = Vertex_PCU( Vec3( 0.6f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	iconVertices[ 5 ] = Vertex_PCU( Vec3( -0.6f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	iconVertices_original[ 0 ] = Vertex_PCU( Vec3( -0.6f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	iconVertices_original[ 1 ] = Vertex_PCU( Vec3( 0.6f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	iconVertices_original[ 2 ] = Vertex_PCU( Vec3( -0.6f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	iconVertices_original[ 3 ] = Vertex_PCU( Vec3( 0.6f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	iconVertices_original[ 4 ] = Vertex_PCU( Vec3( 0.6f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	iconVertices_original[ 5 ] = Vertex_PCU( Vec3( -0.6f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );


	for ( int livesIndex = 0; livesIndex < currentLives; livesIndex++ )
	{
		for ( int index = 0; index < 6; index++ )
		{
			iconVertices[ index ].m_position = TransformPosition3DXY( iconVertices_original[ index ].m_position , 31.f , 0.f , Vec2( 30.f + ( livesIndex * offset ) , UI_YMaX - 18.f ) );
		}

		Texture* tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/bear.png" );
		g_theRenderer->BindTexture( tex );
		g_theRenderer->DrawVertexArray( 6 , iconVertices );
		g_theRenderer->BindTexture( nullptr );
	}
}

void Game::HandleStates()
{
	if ( m_state == STATE_START )
	{
		if ( g_theInput->WasKeyJustPressed( 0x20 ) )
		{
			m_state = STATE_PLAYING;
			return;
		}
	}

	if ( m_state == STATE_WIN )
	{
		if ( g_theInput->WasKeyJustPressed( 0x20 ) )
		{
			delete m_map;
			m_map = nullptr;
			m_map = new Map( this , IntVec2( 30 , 30 ) );
			m_map->StartUp();
			currentLives = 3;
			m_state = STATE_PLAYING;
			return;
		}
	}

	if ( m_state == STATE_GAMEOVER )
	{
		if ( g_theInput->WasKeyJustPressed( 0x20 ) )
		{
			delete m_map;
			m_map = nullptr;
			m_map = new Map( this , IntVec2( 30 , 30 ) );
			m_map->StartUp();
			currentLives = 3;
			m_state = STATE_PLAYING;
			return;
		}
	}
}

void Game::DisplayGameOverScreen()
{
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 53.3f * 2.f , 30.f * 2.f ) );
	g_theRenderer->BeginCamera( *m_camera );

	Texture* ggTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/gameover.png" );
	AABB2 ggBox = AABB2( 40.f , 30.f , 65.f , 40.f );

	g_theRenderer->BindTexture( ggTex );
	g_theRenderer->DrawAABB2D( ggBox , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );


	Texture* replayTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/playagain.png" );
	AABB2 replayBox = AABB2( 25.f , 20.f , 90.f , 30.f );

	g_theRenderer->BindTexture( replayTex );
	g_theRenderer->DrawAABB2D( replayBox , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );
}

void Game::DisplayWinScreen()
{
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 53.3f * 2.f , 30.f * 2.f ) );
	g_theRenderer->BeginCamera( *m_camera );

	Texture* ggTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/gg.png" );
	AABB2 ggBox = AABB2( 40.f , 30.f , 65.f , 40.f );

	g_theRenderer->BindTexture( ggTex );
	g_theRenderer->DrawAABB2D( ggBox , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );


	Texture* replayTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/playagain.png" );
	AABB2 replayBox = AABB2( 25.f , 20.f , 90.f , 30.f );

	g_theRenderer->BindTexture( replayTex );
	g_theRenderer->DrawAABB2D( replayBox , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );
}

void Game::ToggleDebugCamera()
{

}

void Game::TogglePaused()
{
	
}

void Game::ToggleDebugging()
{
	
}

































