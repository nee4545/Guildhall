#include "Game/HUD.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"

HUD::HUD( Game* game )
{
	m_game = game;

	UpgradeUpClock = new Clock();
	UpgradeUpTimer = new Timer();
	UpgradeUpTimer->SetSeconds( UpgradeUpClock , 0.f );

	DownClock = new Clock();
	UpgradeDownTimer = new Timer();
	UpgradeDownTimer->SetSeconds( DownClock , 0.f );


}

void HUD::Update( float deltaSeconds )
{
	m_time += deltaSeconds;

	if ( !isUpdateInfoBoxClosed )
	{
		UpgradeUpTimer->SetSeconds( 1.f );
		UpgradeUpTimer->Reset();
	}

	if ( isUpdateInfoBoxClosed )
	{
		UpgradeDownTimer->SetSeconds( 1.f );
		UpgradeDownTimer->Reset();
	}

	float time = UpgradeUpTimer->GetSecondsRemaining();
	float x = SmoothStart2( time );

	float distanceTotravel = HeroSpriteBox.GetDimensions().y-4.f;

	float d = RangeMapFloat( 0.f , distanceTotravel , 0.f , 1.f , x );
	if ( !UpgradeUpTimer->HasElapsed() )
	{
		HeroUpgradeInfoBox.maxs.y+=d;
	}

	float time2 = UpgradeDownTimer->GetSecondsRemaining();
	float xx = SmoothStart2( time2 );

	float dd = RangeMapFloat( 0.f , distanceTotravel , 0.f , 1.f , xx );
	if ( !UpgradeDownTimer->HasElapsed() )
	{
		HeroUpgradeInfoBox.maxs.y -= dd;
	}
	HeroUpgradeInfoBox.maxs.y = Clamp( HeroUpgradeInfoBox.maxs.y , HeroUpgradeInfoBox.mins.y , HeroUpgradeInfoBox.mins.y+ distanceTotravel );
}

void HUD::Render()
{
	//g_theRenderer->BindTexture( g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/table.png" ) );
	//g_theRenderer->DrawAABB2D( a , Rgba8( 255 , 255 , 255 , 255 ) );
	//g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( SpriteBoxTexure );
	g_theRenderer->DrawAABB2D( HeroSpriteBox,NormalColor );

	g_theRenderer->BindTexture( heroUpgrade0Anims->GetSpriteTextureAtTime( m_time ) );
	g_theRenderer->DrawAABB2D( HeroSpriteBox.GetPercentageOfAABB( 0.75f ) , NormalColor );

	g_theRenderer->BindTexture( CostBoxTexture );
	g_theRenderer->DrawAABB2D( HeroCostBox , NormalColor );
	g_theRenderer->DrawAABB2D( HeroUpgradeCostBox , NormalColor );
	g_theRenderer->BindTexture( UpgradeBoxTexture );
	g_theRenderer->DrawAABB2D( HeroUpgradeBox,NormalColor );
	g_theRenderer->BindTexture( CostBoxTexture );
	g_theRenderer->DrawAABB2D( HeroUpgradeInfoBox,NormalColor );
	g_theRenderer->BindTexture( nullptr );

}


void HUD::LoadTextures()
{

}


