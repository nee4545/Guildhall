#include "Game/LoadingScreen.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"

LoadingScreen::LoadingScreen( Game* game )
{
	m_game = game;
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 320.f , 180.f ) );

	m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

	LoadTextures();

	AABB2 temp = AABB2( m_camera->GetOrthoBottomLeft() , m_camera->GetOrthoTopRight() );

	m_table = temp.GetPercentageOfAABB( 0.7f );

	temp = m_table;

	m_Loading = temp.GetPercentageOfAABB( 0.5f );
	m_Loading.CarveBoxOffBottom( 0.4f );

	m_Loading.Translate( Vec2( 0.f , 50.f ) );

	m_sprite1 = AABB2( Vec2( -25.f , -25.f ) , Vec2( 25.f , 25.f ) );
	m_sprite1.Translate( Vec2(130.f,75.f) );

	m_sprite2 = m_sprite1;
	m_sprite2.Translate( Vec2(40.f,0.f) );

}

LoadingScreen::~LoadingScreen()
{
	delete m_camera;
}

void LoadingScreen::Update()
{

}

void LoadingScreen::Render()
{
	g_theRenderer->BeginCamera( *m_camera );

	g_theRenderer->BindTexture( m_backroundTex );
	g_theRenderer->DrawAABB2D( AABB2( m_camera->GetOrthoBottomLeft() , m_camera->GetOrthoTopRight() ) , Rgba8( 255 , 255 , 255 , 255 ) );

	g_theRenderer->BindTexture( m_tableTex );
	g_theRenderer->DrawAABB2D( m_table , Rgba8( 255 , 255 , 255 , 255 ) );

	g_theRenderer->BindTexture( m_LoadingTex );
	g_theRenderer->DrawAABB2D( m_Loading , Rgba8(255,255,255,255) );

	g_theRenderer->BindTexture( m_sprite1Tex );
	g_theRenderer->DrawAABB2D( m_sprite1,Rgba8(255,255,255,255) );

	g_theRenderer->BindTexture( m_sprite2Tex );
	g_theRenderer->DrawAABB2D( m_sprite2 , Rgba8( 255 , 255 , 255 , 255 ) );

	g_theRenderer->EndCamera( *m_camera );
}

void LoadingScreen::LoadTextures()
{
	m_backroundTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/Backround/background.png" );
	m_tableTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/achievement/table.png" );
	m_LoadingTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Loading.png" );
	m_sprite1Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_000.png" );
	m_sprite2Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Idle/Idle_000.png" );
}

