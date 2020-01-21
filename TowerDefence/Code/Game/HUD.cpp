#include "Game/HUD.hpp"
#include "Game/GameCommon.hpp"

HUD::HUD()
{
	m_HUDBox = AABB2( 0.f , 25.f , 54.f , 30.f );

	m_dinoBox = AABB2( 2.f , 25.5f , 6.f , 29.5f );
	m_dinoCostBox = AABB2( 6.f , 27.5f , 8.f , 29.5f );
	m_dinoSprite = AABB2( 2.5f , 26.f , 6.f , 29.f );

	m_spikeBox = AABB2(9.f,25.5f,13.f,29.5f);
	m_spikeCostBox = AABB2( 13.f , 27.5f , 15.f , 29.5f );
	m_spikeSprite = AABB2( 10.f , 26.f , 12.f , 29.f );

	m_heroBox = AABB2(16.f,25.5f,20.f,29.5f);
	m_heroCostBox = AABB2(20.f,27.5f,22.f,29.5f);
	m_heroSprite = AABB2(16.f,25.5f,19.5f,29.5f);

	m_stoneBox = AABB2(23.f,25.5f,27.f,29.5f);
	m_stoneCostBox = AABB2(27.f,27.5f,29.f,29.5f);
	m_stoneSprite = AABB2( 23.5f , 26.f , 26.5f , 29.f );

	m_selectedBox = AABB2( 32.f , 25.5f , 36.f , 29.5f );
}

void HUD::Render()
{
	g_theRenderer->DrawAABB2D( m_HUDBox , Rgba8( 100 , 100 , 100 , 150 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_dinoBox , Rgba8( 100 , 100 , 100 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_dinoCostBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* dinoTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/dino.png" );
	g_theRenderer->BindTexture( dinoTex );
	g_theRenderer->DrawAABB2D( m_dinoSprite, Rgba8( 200 , 100 , 0 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_spikeBox , Rgba8( 100 , 100 , 100 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_spikeCostBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* spikeTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Spike.png" );
	g_theRenderer->BindTexture( spikeTex );
	g_theRenderer->DrawAABB2D( m_spikeSprite , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_heroBox , Rgba8( 100 , 100 , 100 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_heroCostBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* heroTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/hero.png" );
	g_theRenderer->BindTexture( heroTex );
	g_theRenderer->DrawAABB2D( m_heroSprite , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_stoneBox , Rgba8( 100 , 100 , 100 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_stoneCostBox , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* stoneTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/block.png" );
	g_theRenderer->BindTexture( stoneTex );
	g_theRenderer->DrawAABB2D( m_stoneSprite , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( m_selectedBox , Rgba8( 100 , 100 , 100 , 255 ) );

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
