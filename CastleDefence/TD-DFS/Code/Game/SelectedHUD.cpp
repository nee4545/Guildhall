#include "Game/SelectedHud.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"

SelectedHUD::SelectedHUD( Game* game )
{
	m_game = game;
	LoadTextures();
}

void SelectedHUD::UpdateTexturesBasedOnSelection()
{
	switch ( m_game->m_playerAction )
	{
	case ACTION_SELECT_MELEE:
	{
		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_NONE )
		{
			m_spriteTex = m_MeleeUpgrade0Tex;
		}

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_SMALL )
		{
			m_spriteTex = m_MeleeUpgrade1Tex;
		}

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_MEDIUM )
		{
			m_spriteTex = m_MeleeUpgrade2Tex;
		}

		if ( m_game->m_playerUpgradeState.meleeHeroState == UPGRADE_LARGE )
		{
			m_spriteTex = m_MeleeUpgrade3Tex;
		}

		break;
	}

	case ACTION_SELECT_RANGE:
	{
		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_NONE )
		{
			m_spriteTex = m_RangeUpgrade0Tex;
		}

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_SMALL )
		{
			m_spriteTex = m_RangeUpgrade1Tex;
		}

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_MEDIUM )
		{
			m_spriteTex = m_RangeUpgrade1Tex;
		}

		if ( m_game->m_playerUpgradeState.rangeHeroState == UPGRADE_LARGE )
		{
			m_spriteTex = m_RangeUpgrade1Tex;
		}

		break;
	}

	case ACTION_SELECT_TOWER:
	{
		m_spriteTex = m_game->m_towerTurret;
		break;
	}
		
	default:
	{
		m_spriteTex = nullptr;
		break;
	}

	}

}

void SelectedHUD::LoadTextures()
{
	m_MeleeUpgrade0Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_000.png" );
	m_MeleeUpgrade1Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_000.png" );
	m_MeleeUpgrade2Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_000.png" );
	m_MeleeUpgrade3Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_000.png" );

	m_RangeUpgrade0Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle/Idle_000.png" );
	m_RangeUpgrade1Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle/0_Elementals_Idle_000.png" );
	m_RangeUpgrade2Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle/0_Magician_Idle_000.png" );
	m_RangeUpgrade3Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle/Idle_000.png" );

}

void SelectedHUD::Render()
{
	g_theRenderer->BindTexture( m_textboxTexture );
	g_theRenderer->DrawAABB2D( m_textBox , Rgba8( 255 , 255 , 255 , 255 ) );

	g_theRenderer->BindTexture( m_spriteBoxTexture );
	g_theRenderer->DrawAABB2D(m_spriteBox , Rgba8( 255 , 255 , 255 , 255 ) );

	if ( m_spriteTex )
	{
		if ( m_game->m_playerAction == ACTION_SELECT_TOWER )
		{
			g_theRenderer->BindTexture( m_spriteTex );
			g_theRenderer->DrawAABB2D( m_spriteBox.GetPercentageOfAABB( 0.55f ) , Rgba8( 255 , 255 , 255 , 255 ) );
		}
		else
		{
			g_theRenderer->BindTexture( m_spriteTex );
			g_theRenderer->DrawAABB2D( m_spriteBox.GetPercentageOfAABB( 0.75f ) , Rgba8( 255 , 255 , 255 , 255 ) );
		}
	}

}

