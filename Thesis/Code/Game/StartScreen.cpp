#include "Game/StartScreen.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"

StartScreen::StartScreen(Game* game)
{
	m_game = game;
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2(0.f,0.f) , Vec2(160.f,90.f) );
	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	LoadTextures();
	m_gameNameBox = AABB2( Vec2( 100.f , 75.f ) , Vec2( 150.f , 85.f ) );
	m_gameNameBox.Translate( Vec2(-5.f,-5.f) );
	m_startBox = AABB2( Vec2( 100.f , 75.f ) , Vec2( 112.f , 82.f ) );
	m_startBox.Translate( Vec2( 10.f , -20.f ) );
	m_createBox = m_startBox;
	m_createBox.maxs.x += 10.f;
	m_createBox.Translate( Vec2( 0.f , -10.f ) );
	m_contolsBox = m_createBox;
	m_contolsBox.maxs.x -= 5.f;
	m_contolsBox.Translate( Vec2( 0.f , -10.f ) );
	m_quitBox = m_startBox;
	m_quitBox.Translate( Vec2( 0.f , -30.f ) );
}

void StartScreen::Update( float deltaSeconds )
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	m_mousePosition.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	m_mousePosition.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	if ( m_startBox.IsPointInside( m_mousePosition ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_game->inStartScreen = false;
		m_game->m_currentMode = GAME;
	}

	if ( m_quitBox.IsPointInside( m_mousePosition ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		g_theapp->HandleQuitRequested();
	}
}

void StartScreen::Render()
{
	g_theRenderer->BeginCamera( *m_camera );
	g_theRenderer->BindTexture( m_gameBg );
	g_theRenderer->DrawAABB2D( m_backRound1 , m_normalColor );
	g_theRenderer->BindTexture( m_gameName );
	g_theRenderer->DrawAABB2D( m_gameNameBox, m_normalColor );
	g_theRenderer->BindTexture( m_startGame );
	if ( m_startBox.IsPointInside( m_mousePosition ) )
	{
		g_theRenderer->DrawAABB2D( m_startBox , m_highColor );
	}
	else
	{
		g_theRenderer->DrawAABB2D( m_startBox , m_normalColor );
	}
	g_theRenderer->BindTexture( m_createTex );
	if ( m_createBox.IsPointInside( m_mousePosition ) )
	{
		g_theRenderer->DrawAABB2D( m_createBox , m_highColor );
	}
	else
	{
		g_theRenderer->DrawAABB2D( m_createBox , m_normalColor );
	}
	g_theRenderer->BindTexture( m_controlsTex );
	if ( m_contolsBox.IsPointInside( m_mousePosition ) )
	{
		g_theRenderer->DrawAABB2D( m_contolsBox , m_highColor );
	}
	else
	{
		g_theRenderer->DrawAABB2D( m_contolsBox , m_normalColor );
	}
	g_theRenderer->BindTexture( m_quitTex );
	if ( m_quitBox.IsPointInside( m_mousePosition ) )
	{
		g_theRenderer->DrawAABB2D( m_quitBox , m_highColor );
	}
	else
	{
		g_theRenderer->DrawAABB2D( m_quitBox , m_normalColor );
	}

	g_theRenderer->EndCamera( *m_camera );
}

void StartScreen::LoadTextures()
{
	m_gameBg = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Backrounds/logo.png" );
	m_gameName = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Backrounds/Texts/GameName.png" );
	m_startGame = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Backrounds/Texts/play.png" );
	m_createTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Backrounds/Texts/create_map.png");
	m_controlsTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Backrounds/Texts/controls.png" );
	m_quitTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Backrounds/Texts/quit.png" );
}
