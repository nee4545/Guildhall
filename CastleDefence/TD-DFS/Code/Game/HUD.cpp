#include "Game/HUD.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

HUD::HUD( Game* game,HUDType type )
{
	m_game = game;
	m_type = type;

	UpgradeUpClock = new Clock();
	UpgradeUpTimer = new Timer();
	UpgradeUpTimer->SetSeconds( UpgradeUpClock , 2.5f );

	DownClock = new Clock();
	UpgradeDownTimer = new Timer();
	UpgradeDownTimer->SetSeconds( DownClock , 2.5f );
}

void HUD::Update( float deltaSeconds )
{
	m_time += deltaSeconds;

	if ( !isUpdateInfoBoxClosed )
	{
		UpgradeUpTimer->Reset();
	}

	if ( isUpdateInfoBoxClosed )
	{
		UpgradeDownTimer->Reset();
	}

	if ( m_type == TYPE_MELEE || m_type == TYPE_RANGE || m_type == TYPE_TOWER )
	{
		float time = UpgradeUpTimer->GetSecondsRemaining();
		float x = SmoothStart2( time );

		float distanceTotravel = HeroSpriteBox.GetDimensions().y - 4.f;

		float d = RangeMapFloat( 0.f , distanceTotravel , 0.f , 1.f , x );
		if ( !UpgradeUpTimer->HasElapsed() )
		{
			HeroUpgradeInfoBox.maxs.y += d;
		}

		float time2 = UpgradeDownTimer->GetSecondsRemaining();
		float xx = SmoothStart2( time2 );

		float dd = RangeMapFloat( 0.f , distanceTotravel , 0.f , 1.f , xx );
		if ( !UpgradeDownTimer->HasElapsed() )
		{
			HeroUpgradeInfoBox.maxs.y -= dd;
		}
		HeroUpgradeInfoBox.maxs.y = Clamp( HeroUpgradeInfoBox.maxs.y , HeroUpgradeInfoBox.mins.y , HeroUpgradeInfoBox.mins.y + distanceTotravel );
	}
	else
	{
		float time = UpgradeUpTimer->GetSecondsRemaining();
		float x = SmoothStart2( time );

		float distanceTotravel = HeroSpriteBox.GetDimensions().y - 4.f;

		float d = RangeMapFloat( 0.f , distanceTotravel , 0.f , 1.f , x );
		if ( !UpgradeUpTimer->HasElapsed() )
		{
			HeroUpgradeInfoBox.mins.y -= d;
		}

		float time2 = UpgradeDownTimer->GetSecondsRemaining();
		float xx = SmoothStart2( time2 );

		float dd = RangeMapFloat( 0.f , distanceTotravel , 0.f , 1.f , xx );
		if ( !UpgradeDownTimer->HasElapsed() )
		{
			HeroUpgradeInfoBox.mins.y += dd;
		}
		HeroUpgradeInfoBox.mins.y = Clamp( HeroUpgradeInfoBox.mins.y , HeroUpgradeInfoBox.maxs.y - distanceTotravel , HeroUpgradeInfoBox.maxs.y );
	}

	GetTextureForType();
}

void HUD::Render()
{
	//g_theRenderer->BindTexture( g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/table.png" ) );
	//g_theRenderer->DrawAABB2D( a , Rgba8( 255 , 255 , 255 , 255 ) );
	//g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( SpriteBoxTexure );
	g_theRenderer->DrawAABB2D( HeroSpriteBox,NormalColor );

	g_theRenderer->BindTexture( m_spriteTex );
	if ( m_type != TYPE_TOWER )
	{
		g_theRenderer->DrawAABB2D( HeroSpriteBox.GetPercentageOfAABB( 0.75f ) , NormalColor );
	}
	else
	{
		g_theRenderer->DrawAABB2D( HeroSpriteBox.GetPercentageOfAABB( 0.55f ) , NormalColor );
	}



	g_theRenderer->BindTexture( CostBoxTexture );
	g_theRenderer->DrawAABB2D( HeroCostBox , NormalColor );
	g_theRenderer->DrawAABB2D( HeroUpgradeCostBox , NormalColor );
	g_theRenderer->BindTexture( UpgradeBoxTexture );
	g_theRenderer->DrawAABB2D( HeroUpgradeBox,NormalColor );
	g_theRenderer->BindTexture( CostBoxTexture );
	g_theRenderer->DrawAABB2D( HeroUpgradeInfoBox,NormalColor );
	g_theRenderer->BindTexture( nullptr );
	RenderCoins();

}


void HUD::RenderCoins()
{
	switch ( m_type )
	{
	case TYPE_MELEE:
	{
		std::string c = std::to_string( m_game->m_currentMeleeHeroCost );
		std::string c1;

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_NONE )
		{
			c1 = std::to_string( MELEE_UPGRADE1_COST );
		}

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_SMALL )
		{
			c1 = std::to_string( MELEE_UPGRADE2_COST );
		}

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_MEDIUM )
		{
			c1 = std::to_string( MELEE_UPGRADE3_COST );
		}

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_LARGE )
		{
			c1 = "MAX";
		}

		std::vector<Vertex_PCU> v1;
		std::vector<Vertex_PCU> v2;

		m_game->m_font->AddVertsForTextInBox2D( v1 , HeroCostBox , 2.f , c , Rgba8( 0 , 0 , 150 , 255 ) , 2.f , Vec2( 0.2f , 0.4f ) );
		m_game->m_font->AddVertsForTextInBox2D( v2 , HeroUpgradeCostBox , 2.f , c1 , Rgba8( 0 , 0 , 150 , 255 ) , 2.f , Vec2( 0.2f , 0.4f ) );
											
		g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( v1 );
		g_theRenderer->DrawVertexArray( v2 );
		g_theRenderer->BindTexture( nullptr );
		
		break;
	}
	case TYPE_RANGE:
	{
		std::string c = std::to_string( m_game->m_currentRangeHeroCost );
		std::string c1;

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_NONE )
		{
			c1 = std::to_string( RANGE_UPGRADE1_COST );
		}

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_SMALL )
		{
			c1 = std::to_string( RANGE_UPGRADE2_COST );
		}

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_MEDIUM )
		{
			c1 = std::to_string( RANGE_UPGRADE3_COST );
		}

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_LARGE )
		{
			c1 = "MAX";
		}

		std::vector<Vertex_PCU> v1;
		std::vector<Vertex_PCU> v2;

		m_game->m_font->AddVertsForTextInBox2D( v1 , HeroCostBox , 2.f , c , Rgba8( 0 , 0 , 150 , 255 ) , 2.f , Vec2( 0.2f , 0.4f ) );
		m_game->m_font->AddVertsForTextInBox2D( v2 , HeroUpgradeCostBox , 2.f , c1 , Rgba8( 0 , 0 , 150 , 255 ) , 2.f , Vec2( 0.2f , 0.4f ) );

		g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( v1 );
		g_theRenderer->DrawVertexArray( v2 );
		g_theRenderer->BindTexture( nullptr );
		break;
	}
	case TYPE_TOWER:
	{
		std::string c = std::to_string( m_game->m_currentTowerCost );
		std::string c1;

		if ( m_game->m_playerUpgradeState.towerState == UPGRADE_NONE )
		{
			c1 = std::to_string( TOWER_UPGRADE1_COST );
		}

		if ( m_game->m_playerUpgradeState.towerState == UPGRADE_SMALL )
		{
			c1 = std::to_string( TOWER_UPGRADE2_COST );
		}

		if ( m_game->m_playerUpgradeState.towerState == UPGRADE_MEDIUM )
		{
			c1 = std::to_string( TOWER_UPGRADE3_COST );
		}

		if ( m_game->m_playerUpgradeState.towerState == UPGRADE_LARGE )
		{
			c1 = "MAX";
		}

		std::vector<Vertex_PCU> v1;
		std::vector<Vertex_PCU> v2;

		m_game->m_font->AddVertsForTextInBox2D( v1 , HeroCostBox , 2.f , c , Rgba8( 0 , 0 , 150 , 255 ) , 2.f , Vec2( 0.2f , 0.4f ) );
		m_game->m_font->AddVertsForTextInBox2D( v2 , HeroUpgradeCostBox , 2.f , c1 , Rgba8( 0 , 0 , 150 , 255 ) , 2.f , Vec2( 0.2f , 0.4f ) );

		g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( v1 );
		g_theRenderer->DrawVertexArray( v2 );
		g_theRenderer->BindTexture( nullptr );
		break;
	}
	case TYPE_TOWER_DESTROYER:
	{
		std::string c = std::to_string( m_game->m_currentTowerDestroyerCost );
		std::string c1;

		if ( m_game->m_playerUpgradeState.towerState == UPGRADE_NONE )
		{
			c1 = std::to_string( TOWER_DESTROYER_UPGRADE_COST );
		}
		if ( m_game->m_playerUpgradeState.towerState == UPGRADE_SMALL )
		{
			c1 = "MAX";
		}

		std::vector<Vertex_PCU> v1;
		std::vector<Vertex_PCU> v2;

		m_game->m_font->AddVertsForTextInBox2D( v1 , HeroCostBox , 2.f , c , Rgba8( 0 , 0 , 150 , 255 ) , 2.f , Vec2( 0.2f , 0.4f ) );
		m_game->m_font->AddVertsForTextInBox2D( v2 , HeroUpgradeCostBox , 2.f , c1 , Rgba8( 0 , 0 , 150 , 255 ) , 2.f , Vec2( 0.2f , 0.4f ) );

		g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( v1 );
		g_theRenderer->DrawVertexArray( v2 );
		g_theRenderer->BindTexture( nullptr );
		break;
	}
	default:
		break;
	}
}

void HUD::GetTextureForType()
{
	switch ( m_type )
	{

	case TYPE_MELEE:
	{
		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_NONE )
		{
			m_spriteTex = heroUpgrade0Anims->GetSpriteTextureAtTime( m_time );
		}

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_SMALL )
		{
			m_spriteTex = heroUpgrade1Anims->GetSpriteTextureAtTime( m_time );
		}

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_MEDIUM )
		{
			m_spriteTex = heroUpgrade2Anims->GetSpriteTextureAtTime( m_time );
		}

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_LARGE )
		{
			m_spriteTex = heroUpgrade3Anims->GetSpriteTextureAtTime( m_time );
		}

		break;
	}

	case TYPE_RANGE:
	{
		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_NONE )
		{
			m_spriteTex = heroUpgrade0Anims->GetSpriteTextureAtTime( m_time );
		}

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_SMALL )
		{
			m_spriteTex = heroUpgrade1Anims->GetSpriteTextureAtTime( m_time );
		}

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_MEDIUM )
		{
			m_spriteTex = heroUpgrade2Anims->GetSpriteTextureAtTime( m_time );
		}

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_LARGE )
		{
			m_spriteTex = heroUpgrade3Anims->GetSpriteTextureAtTime( m_time );
		}

		break;
	}

	case TYPE_TOWER:
	{
		m_spriteTex = m_game->m_towerTurret;
		break;
	}

	case TYPE_TOWER_DESTROYER:
	{
		if ( m_game->m_playerUpgradeState.towerDestroyerState == UPGRADE_NONE )
		{
			m_spriteTex = heroUpgrade0Anims->GetSpriteTextureAtTime( m_time );
		}

		if ( m_game->m_playerUpgradeState.towerDestroyerState == UPGRADE_SMALL )
		{
			m_spriteTex = heroUpgrade1Anims->GetSpriteTextureAtTime( m_time );
		}

		break;
	}

	default:
	{
		m_spriteTex = nullptr;
	}

	}
}

void HUD::LoadTextures()
{

}


