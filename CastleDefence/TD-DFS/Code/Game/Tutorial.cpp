#include "Game/Tutorial.hpp"
#include "Game/StartScreen.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Game/HUD.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"

Tutorial::Tutorial( Game* game , StartScreen* startScreen )
{
	m_game = game;
	m_startScreen = startScreen;

	m_table = AABB2( m_startScreen->m_camera->GetOrthoBottomLeft() , m_startScreen->m_camera->GetOrthoTopRight() );
	m_rules = m_startScreen->m_titleButton.GetPercentageOfAABB(0.3f);
	m_rules.Translate( Vec2(-115.f,30.f) );

	m_rulesText = m_rules;
	m_rulesText.maxs.x += 90.f;
	m_rulesText.maxs.y += 40.f;
	m_rulesText.Translate( Vec2( -5.f , -40.f ) );

	m_controlsText = m_rulesText;
	m_controlsText.maxs.y += 35.f;
	m_controlsText.maxs.x += 15.f;
	m_controlsText.Translate( Vec2( 0.f , -95.f ) );
	
	m_controls = m_rules;
	m_controls.maxs.x -= 10.f;
	m_controls.Translate( Vec2(0.f,-50.f) );

	m_rules.maxs.x += 15.f;
	m_rulesText.maxs.y -= 10.f;
	
	m_heroIdleAnims = m_startScreen->m_heroIdleAnims;
	m_heroAttackAnims = m_startScreen->m_heroAttackAnims;

	AABB2 temp = m_game->m_UIAreaBot.GetPercentageOfAABB( 0.9 );
	AABB2 MeleeHeroBox = AABB2( temp.mins , Vec2( temp.mins.x + 20.f , temp.maxs.y ) );
	Vec2 heroBoxDimensions = MeleeHeroBox.GetDimensions();
	AABB2 MeleeHeroCostBox = AABB2( Vec2( MeleeHeroBox.maxs.x , MeleeHeroBox.mins.y ) , Vec2( MeleeHeroBox.maxs.x + 15.f , MeleeHeroBox.mins.y + heroBoxDimensions.y * 0.5f ) );
	AABB2 MeleeHeroUpgradeBox = AABB2( Vec2( MeleeHeroCostBox.mins.x , MeleeHeroCostBox.maxs.y ) , Vec2( MeleeHeroCostBox.maxs.x + 10.f , MeleeHeroCostBox.maxs.y + ( heroBoxDimensions.y * 0.5f ) ) );

	AABB2 temp1 = MeleeHeroUpgradeBox.GetBoxAtLeft( 0.4f );
	AABB2 temp2 = MeleeHeroUpgradeBox.GetBoxAtRight( 0.6f );

	MeleeHeroUpgradeBox = temp1;
	AABB2 MeleeHeroUpgradeCostBox = temp2;

	Vec2 dimensions1 = MeleeHeroBox.GetDimensions();
	Vec2 dimensions2 = MeleeHeroUpgradeBox.GetDimensions();
	AABB2 MeleeHeroInfoBox = AABB2( Vec2( MeleeHeroBox.mins ) , Vec2( MeleeHeroBox.mins.x + dimensions1.x + dimensions2.x , MeleeHeroBox.mins.y ) );
	MeleeHeroInfoBox.Translate( Vec2( 0.f , dimensions1.y ) );
	

	m_heroSpriteBox = MeleeHeroBox;
	m_heroSprite = MeleeHeroBox.GetPercentageOfAABB( 0.9f );
	m_heroCostBox = MeleeHeroCostBox;
	m_upgradeBox = MeleeHeroUpgradeBox;
	m_upgradeCostBox = MeleeHeroUpgradeCostBox;

	Vec2 tans = Vec2( 160.f , 80.f );

	m_heroSpriteBox.Translate( tans );
	m_heroSprite.Translate( tans );
	m_heroCostBox.Translate( tans );
	m_upgradeBox.Translate( tans );
	m_upgradeCostBox.Translate( tans );

	m_costArrow = m_heroCostBox;
	m_upgrdeCostArrow = m_upgradeCostBox;

	m_heroCostText = m_costArrow;
	m_upgradeCostText = m_upgradeCostBox;

	m_heroCostText.Translate( Vec2(2.f* m_costArrow.GetDimensions().x , 0.f ) );
	m_upgradeCostText.Translate( Vec2(2.f * m_costArrow.GetDimensions().x , 0.f ) );

	m_costArrow.Translate( Vec2( m_costArrow.GetDimensions().x , 0.f ) );
	m_upgrdeCostArrow.Translate( Vec2( m_costArrow.GetDimensions().x , 0.f ) );

	m_costArrow = m_costArrow.GetPercentageOfAABB( 0.7f );
	m_upgrdeCostArrow = m_upgrdeCostArrow.GetPercentageOfAABB( 0.7f );

	m_heroCostText.maxs.x += 10.f;
	m_upgradeCostText.maxs.x += 15.f;

	m_heroArrow = m_upgradeBox;
	m_upgradeArrow = m_upgradeBox;
	m_upgradeArrow.Translate( Vec2( 0.f , 10.f ) );
	m_heroArrow.Translate( Vec2( -15.f , 10.f ) );
	m_heroArrow.maxs.y += 9.f;

	m_heroArrow = m_heroArrow.GetPercentageOfAABB( 0.8f );
	m_upgradeArrow = m_upgradeArrow.GetPercentageOfAABB( 0.8f );

	m_heroCostText =m_heroCostText.GetPercentageOfAABB( 0.9f );

	m_heroButton = m_upgradeCostText;
	m_heroButton.maxs.x += 5.f;
	m_heroButton = m_heroButton.GetPercentageOfAABB( 0.9f );
	m_heroButton.Translate( Vec2(-60.f,27.f) );

	m_upgradeButton = m_heroButton;
	m_upgradeButton.maxs.x += 8.f;

	m_upgradeButton.Translate( Vec2(13.f,-9.f) );

	m_hudInfo = m_rules;
	m_hudInfo.maxs.x += 5.f;
	m_hudInfo.maxs.y += 5.f;
	m_hudInfo.Translate( Vec2(170.f,-20.f) );

	m_heroBox1 = m_startScreen->m_heroBox1;
	m_startButton = m_startScreen->m_startButton;

	m_heroBox1 = m_heroBox1.GetPercentageOfAABB( 0.9f );
	m_startButton = m_startButton.GetPercentageOfAABB( 0.9f );

	m_heroBox1.Translate( Vec2(60.f,-50.f) );
	m_startButton.Translate( Vec2(60.f,-50.f) );

	m_heroBox2 = m_heroBox1;
	m_mainMenuButton = m_startButton;
	m_mainMenuButton.maxs.x += 15.f;

	m_heroBox2.Translate( Vec2(0.f,-15.f) );
	m_mainMenuButton.Translate( Vec2(0.f,-15.f) );

	LoadTextures();
}

void Tutorial::Update( float deltaSeconds )
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	Vec2 mousePositionScreen;
	mousePositionScreen.x = RangeMapFloat( 0.f , 1.f , m_startScreen->m_camera->GetOrthoBottomLeft().x , m_startScreen->m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	mousePositionScreen.y = RangeMapFloat( 0.f , 1.f , m_startScreen->m_camera->GetOrthoBottomLeft().y , m_startScreen->m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	m_time += deltaSeconds;

	if ( m_startButton.IsPointInside( mousePositionScreen ) )
	{
		heroIdle = false;

		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_game->isInStartScreen = false;
		}
	}
	else
	{
		heroIdle = true;
	}

	if ( m_mainMenuButton.IsPointInside( mousePositionScreen ) )
	{
		hero1Idle = false;

		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_startScreen->m_inTutorial = false;
		}
	}
	else
	{
		hero1Idle = true;
	}

	
}

void Tutorial::Render()
{
	g_theRenderer->BindTexture( m_tableTex );
	g_theRenderer->DrawAABB2D( m_table , Rgba8( 255 , 255 , 255 , 255 ) );

	g_theRenderer->BindTexture( m_rulesTex );
	g_theRenderer->DrawAABB2D( m_rules , Rgba8( 255 , 255 , 255 , 255 ) );

	g_theRenderer->BindTexture( m_rulesTextTex );
	g_theRenderer->DrawAABB2D( m_rulesText , Rgba8( 255 , 255 , 255 , 255 ) );

	g_theRenderer->BindTexture( m_controlsTex );
	g_theRenderer->DrawAABB2D( m_controls , Rgba8() );

	g_theRenderer->BindTexture( m_controlsTextTex );
	g_theRenderer->DrawAABB2D( m_controlsText , Rgba8() );

	g_theRenderer->BindTexture( m_heroSpriteBoxTex );
	g_theRenderer->DrawAABB2D( m_heroSpriteBox, Rgba8() );

	g_theRenderer->BindTexture( m_heroSpriteTex );
	g_theRenderer->DrawAABB2D( m_heroSprite , Rgba8() );

	g_theRenderer->BindTexture( m_upgradeBoxTex );
	g_theRenderer->DrawAABB2D( m_upgradeBox,Rgba8() );

	g_theRenderer->BindTexture( m_costBoxTex );
	g_theRenderer->DrawAABB2D( m_upgradeCostBox , Rgba8() );
	g_theRenderer->DrawAABB2D( m_heroCostBox , Rgba8() );

	g_theRenderer->BindTexture( m_rightArrowTex );
	g_theRenderer->DrawAABB2D( m_costArrow,Rgba8() );
	g_theRenderer->DrawAABB2D( m_upgrdeCostArrow,Rgba8() );

	g_theRenderer->BindTexture( m_heroCostTextTex );
	g_theRenderer->DrawAABB2D( m_heroCostText , Rgba8() );

	g_theRenderer->BindTexture( m_upgradeCostTextTex );
	g_theRenderer->DrawAABB2D( m_upgradeCostText, Rgba8() );

	g_theRenderer->BindTexture( m_upArrowTex1 );
	g_theRenderer->DrawAABB2D( m_upgradeArrow,Rgba8() );

	g_theRenderer->BindTexture( m_upArrowTex2 );
	g_theRenderer->DrawAABB2D( m_heroArrow , Rgba8() );

	g_theRenderer->BindTexture( m_heroButtonTex );
	g_theRenderer->DrawAABB2D( m_heroButton,Rgba8() );

	g_theRenderer->BindTexture( m_upgradeButtonTex );
	g_theRenderer->DrawAABB2D( m_upgradeButton , Rgba8() );
	
	g_theRenderer->BindTexture( m_HudInfoTex );
	g_theRenderer->DrawAABB2D( m_hudInfo , Rgba8() );

	g_theRenderer->BindTexture( m_startTex );
	if ( heroIdle )
	{
		g_theRenderer->DrawAABB2D( m_startButton , m_startScreen->normalColor );
	}
	else
	{
		g_theRenderer->DrawAABB2D( m_startButton , m_startScreen->highColor );
	}

	g_theRenderer->BindTexture( m_MainMenuTex );
	if ( hero1Idle )
	{
		g_theRenderer->DrawAABB2D( m_mainMenuButton , m_startScreen->normalColor );
	}
	else
	{
		g_theRenderer->DrawAABB2D( m_mainMenuButton , m_startScreen->highColor );
	}

	if ( heroIdle )
	{
		g_theRenderer->BindTexture( m_heroIdleAnims->GetSpriteTextureAtTime( m_time ) );
		g_theRenderer->DrawAABB2D( m_heroBox1 , Rgba8( 255 , 255 , 255 , 255 ) );
	}
	else
	{
		g_theRenderer->BindTexture( m_heroAttackAnims->GetSpriteTextureAtTime( m_time ) );
		g_theRenderer->DrawAABB2D( m_heroBox1 , Rgba8( 255 , 255 , 255 , 255 ) );
	}

	if ( hero1Idle )
	{
		g_theRenderer->BindTexture( m_heroIdleAnims->GetSpriteTextureAtTime( m_time ) );
		g_theRenderer->DrawAABB2D( m_heroBox2 , Rgba8( 255 , 255 , 255 , 255 ) );
	}
	else
	{
		g_theRenderer->BindTexture( m_heroAttackAnims->GetSpriteTextureAtTime( m_time ) );
		g_theRenderer->DrawAABB2D( m_heroBox2 , Rgba8( 255 , 255 , 255 , 255 ) );
	}

	
}

void Tutorial::LoadTextures()
{
	m_tableTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/achievement/table.png" );
	m_rulesTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Rules.png" );
	m_rulesTextTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/RulesText.png" );
	m_controlsTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Contrrols.png" );
	m_controlsTextTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/ControlsText.png" );

	m_heroSpriteBoxTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/btn/button_empty_1.png" );
	m_heroSpriteTex = g_theRenderer->GetOrCreateTextureFromFile("Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_000.png" );
	m_costBoxTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/table.png" );
	m_upgradeBoxTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/ico_4.png" );

	m_rightArrowTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/RightArrow.png" );

	m_heroCostTextTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/HeroCost.png" );
	m_upgradeCostTextTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/UpgradeCost.png" );

	m_upArrowTex1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/RightArrow - Copy.png" );
	m_upArrowTex2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Upp.png" );

	m_heroButtonTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/HButton.png" );
	m_upgradeButtonTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/UpButton.png" );

	m_HudInfoTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Hinfo.png" );

	m_startTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/X.png" );
	m_MainMenuTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Y.png" );
}
