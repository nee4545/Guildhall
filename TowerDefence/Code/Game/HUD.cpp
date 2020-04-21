#include "Game/HUD.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"

HUD::HUD()
{
	m_HUDBox = AABB2( 0.f , 25.f , 54.f , 30.f );

	m_dinoBox = AABB2( 2.f , 25.5f , 6.f , 29.5f );
	m_dinoCostBox = AABB2( 6.f , 27.5f , 9.f , 29.5f );
	m_dinoUpgradeBox = AABB2( 6.f , 25.5f , 9.5f , 27.5f );
	m_dinoSprite = AABB2( 2.5f , 26.f , 6.f , 29.f );

	m_spikeBox = AABB2(10.f,25.5f,14.f,29.5f);
	m_spikeCostBox = AABB2( 14.f , 27.5f , 17.f , 29.5f );
	m_spikeUpgradeBox = AABB2( 14.f , 25.5f , 17.5f , 27.5f );
	m_spikeSprite = AABB2( 11.f , 26.f , 13.f , 29.f );

	m_heroBox = AABB2(18.f,25.5f,22.f,29.5f);
	m_heroCostBox = AABB2(22.f,27.5f,25.f,29.5f);
	m_heroUpgradeBox = AABB2( 22.f , 25.5f , 25.5f , 27.5f );
	m_heroSprite = AABB2(18.f,25.5f,21.5f,29.5f);

	m_stoneBox = AABB2(23.f,25.5f,27.f,29.5f);
	m_stoneCostBox = AABB2(27.f,27.5f,29.f,29.5f);
	m_stoneSprite = AABB2( 23.5f , 26.f , 26.5f , 29.f );

	m_selectedBox = AABB2( 32.f , 25.5f , 36.f , 29.5f );

	m_playerCoinBox = AABB2( 38.f , 25.5f , 44.f , 27.f );
	m_enemyCoinBox = AABB2( 38.f , 28.f , 44.f , 29.5f );

	m_playerCoins = AABB2( 45.f , 25.5f , 48.f , 27.f );
	m_enemyCoins = AABB2( 45.f , 28.f , 48.f , 29.5f );

	m_font = g_theRenderer->CreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

}

void HUD::Render()
{
	g_theRenderer->DrawAABB2D( m_HUDBox , Rgba8( 100 , 100 , 100 , 150 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_dinoBox , Rgba8( 100 , 100 , 100 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_dinoCostBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->DrawAABB2D( m_dinoUpgradeBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* dinoTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/dino.png" );
	g_theRenderer->BindTexture( dinoTex );
	g_theRenderer->DrawAABB2D( m_dinoSprite, Rgba8( 200 , 100 , 0 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_spikeBox , Rgba8( 100 , 100 , 100 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_spikeCostBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->DrawAABB2D( m_spikeUpgradeBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* spikeTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Spike.png" );
	g_theRenderer->BindTexture( spikeTex );
	g_theRenderer->DrawAABB2D( m_spikeSprite , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_heroBox , Rgba8( 100 , 100 , 100 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_heroCostBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->DrawAABB2D( m_heroUpgradeBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* heroTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/hero.png" );
	g_theRenderer->BindTexture( heroTex );
	g_theRenderer->DrawAABB2D( m_heroSprite , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_playerCoinBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->DrawAABB2D( m_enemyCoinBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->DrawAABB2D( m_enemyCoins , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->DrawAABB2D( m_playerCoins , Rgba8( 100 , 100 , 100 , 100 ) );

	std::string s1 = "Player Coins ";
	std::string s2 = "Enemy Coins ";
	std::string c = "Cost ";
	std::string u = "Upgrade ";
	std::string temp;

	std::vector<Vertex_PCU> v1;
	m_font->AddVertsForTextInBox2D( v1 , m_playerCoinBox , 0.45f , s1 , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	std::vector<Vertex_PCU> v2;
	m_font->AddVertsForTextInBox2D( v2 , m_enemyCoinBox , 0.45f , s2 , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	std::vector<Vertex_PCU> v3;
	temp = std::to_string( m_map->m_manager.enemyCoins );
	m_font->AddVertsForTextInBox2D( v3 , m_enemyCoins , 0.45f , temp , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	std::vector<Vertex_PCU> v4;
	temp = std::to_string( m_map->m_manager.playerCoins );
	m_font->AddVertsForTextInBox2D( v4 , m_playerCoins , 0.45f , temp , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	std::vector<Vertex_PCU> v5;
	m_font->AddVertsForTextInBox2D( v5 , m_heroCostBox , 0.4f , c , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.8f ) );

	std::vector<Vertex_PCU> v6;
	temp = std::to_string( m_map->m_manager.playerCurrentHeroCost );
	m_font->AddVertsForTextInBox2D( v6 , m_heroCostBox , 0.4f , temp , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	std::vector<Vertex_PCU> v7;
	m_font->AddVertsForTextInBox2D( v7 , m_dinoCostBox , 0.4f , c , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.8f ) );

	std::vector<Vertex_PCU> v8;
	temp = std::to_string( m_map->m_manager.playerCurrentDinoCost );
	m_font->AddVertsForTextInBox2D( v8 , m_dinoCostBox , 0.4f , temp , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	std::vector<Vertex_PCU> v9;
	m_font->AddVertsForTextInBox2D( v9 , m_spikeCostBox , 0.4f , c , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.8f ) );

	std::vector<Vertex_PCU> v10;
	temp = std::to_string( m_map->m_manager.playerCurrentSpikeCost );
	m_font->AddVertsForTextInBox2D( v10 , m_spikeCostBox , 0.4f , temp , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	std::vector<Vertex_PCU> v11;
	m_font->AddVertsForTextInBox2D( v11 , m_spikeUpgradeBox , 0.4f , u , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.8f ) );

	std::vector<Vertex_PCU> v12;
	temp = std::to_string( m_map->m_manager.playerSpikeUpgradeCost );
	m_font->AddVertsForTextInBox2D( v12 , m_spikeUpgradeBox , 0.4f , temp , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	std::vector<Vertex_PCU> v13;
	m_font->AddVertsForTextInBox2D( v13 , m_dinoUpgradeBox , 0.4f , u , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.8f ) );

	std::vector<Vertex_PCU> v14;
	temp = std::to_string( m_map->m_manager.playerDinoUpgradeCost );
	m_font->AddVertsForTextInBox2D( v14 , m_dinoUpgradeBox , 0.4f , temp , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	std::vector<Vertex_PCU> v15;
	m_font->AddVertsForTextInBox2D( v15 , m_heroUpgradeBox , 0.4f , u , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.8f ) );

	std::vector<Vertex_PCU> v16;
	temp = std::to_string( m_map->m_manager.playerHeroUpgradeCost );
	m_font->AddVertsForTextInBox2D( v16 , m_heroUpgradeBox , 0.4f , temp , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.2f , 0.4f ) );

	g_theRenderer->BindTexture( m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( v1 );
	g_theRenderer->DrawVertexArray( v2 );
	g_theRenderer->DrawVertexArray( v3 );
	g_theRenderer->DrawVertexArray( v4 );
	g_theRenderer->DrawVertexArray( v5 );
	g_theRenderer->DrawVertexArray( v6 );
	g_theRenderer->DrawVertexArray( v7 );
	g_theRenderer->DrawVertexArray( v8 );
	g_theRenderer->DrawVertexArray( v9 );
	g_theRenderer->DrawVertexArray( v10 );
	g_theRenderer->DrawVertexArray( v11 );
	g_theRenderer->DrawVertexArray( v12 );
	g_theRenderer->DrawVertexArray( v13 );
	g_theRenderer->DrawVertexArray( v14 );
	g_theRenderer->DrawVertexArray( v15 );
	g_theRenderer->DrawVertexArray( v16 );
	g_theRenderer->BindTexture( nullptr );

	/*g_theRenderer->DrawAABB2D( m_stoneBox , Rgba8( 100 , 100 , 100 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_stoneCostBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->BindTexture( nullptr );*/

	/*Texture* stoneTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/block.png" );
	g_theRenderer->BindTexture( stoneTex );
	g_theRenderer->DrawAABB2D( m_stoneSprite , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_selectedBox , Rgba8( 100 , 100 , 100 , 255 ) );*/

}


void HUD::RenderSelectedUnit( int unit )
{
	//hero =1 spike=2 box=3 dino=4 none=0

	switch ( unit )
	{
	case 1:
	{
		AABB2 box = AABB2( 32.f , 25.5f , 35.5f , 29.5f );
		Texture* boxTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/hero.png" );
		g_theRenderer->BindTexture( boxTex );
		g_theRenderer->DrawAABB2D( box , Rgba8( 255 , 255 , 255 , 255 ) );
		g_theRenderer->BindTexture( nullptr );
		break;
	}

	case 2:
	{
		AABB2 box = AABB2( 33.f , 26.f , 35.f , 29.f );
		Texture* boxTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Spike.png" );
		g_theRenderer->BindTexture( boxTex );
		g_theRenderer->DrawAABB2D( box , Rgba8( 255 , 255 , 255 , 255 ) );
		g_theRenderer->BindTexture( nullptr );
		break;
	}

	case 3:
	{
		AABB2 box = AABB2( 32.5f , 26.f , 35.5f , 29.f );
		Texture* boxTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/block.png" );
		g_theRenderer->BindTexture( boxTex );
		g_theRenderer->DrawAABB2D( box , Rgba8( 255 , 255 , 255 , 255 ) );
		g_theRenderer->BindTexture( nullptr );
		break;
	}

	case 4:
	{
		AABB2 box = AABB2( 32.5f , 26.f , 36.5f , 29.f );
		Texture* boxTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/dino.png" );
		g_theRenderer->BindTexture( boxTex );
		g_theRenderer->DrawAABB2D( box , Rgba8( 200 , 100 , 0 , 255 ) );
		g_theRenderer->BindTexture( nullptr );
		break;
	}

	default:
	{
		AABB2 box = AABB2( 32.f , 25.5f , 35.5f , 29.5f );
		g_theRenderer->DrawAABB2D( box , Rgba8( 100 , 100 , 100 , 100 ) );
		g_theRenderer->BindTexture( nullptr );
		break;
	}

	}

}
