#include "Game/Game.hpp"
#include "Game/Hero.hpp"
#include "Game/HUD.hpp"
#include "Game/SelectedHud.hpp"
#include "Game/Tower.hpp"
#include "Game/TowerDestroyer.hpp"
#include "Game/RangeHero.hpp"
#include "Game/ShootObject.hpp"
#include "Game/StartScreen.hpp"
#include "Game/AIController.hpp"
#include "Game/LoadingScreen.hpp"
#include "Game/UncontrolledHero.hpp"
#include "Game/TowerBullet.hpp"
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
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

Game::Game()
{
	m_rng = RandomNumberGenerator();

	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 320.f , 180.f ));

	m_camera->SetPosition( Vec3( 0.f , 0.f , 0.f ) );

	m_UICamera = new Camera();
	m_UICamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 320.f , 180.f ) );

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );

	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );
	m_UICamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );
	m_devConsoleCamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

	m_loadingScreen = new LoadingScreen( this );

	m_healthBarBase = AABB2( Vec2(0.5f,20.f) , Vec2(3.5f,100.f) );
	m_healthBarBase.Translate( Vec2(3.f,30.f) );

	m_healthBar = m_healthBarBase;

	m_font = g_theRenderer->CreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

	m_enemyHealthBarBase = m_healthBarBase;
	m_enemyHealthBarBase.Translate( Vec2(310.f,0.f) );

	m_enemyHealthBar = m_enemyHealthBarBase;

	m_enemyHealthBar.mins.x += 0.5f;
	m_enemyHealthBar.maxs.x -= 0.5f;
	m_enemyHealthBar.maxs.y -= 1.f;
	m_enemyHealthBar.mins.y += 1.f;

	m_healthBar.mins.x += 0.5f;
	m_healthBar.maxs.x -= 0.5f;
	m_healthBar.maxs.y -= 1.f;
	m_healthBar.mins.y += 1.f;

	m_UIAreaBot = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );
	m_UIAreaBot.CarveBoxOffBottom( 0.12f );
	m_UIAreaTop = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );
	m_UIAreaTop.CarveBoxOffTop( 0.12f );

	m_enemytdlane1 = AABB2( Vec2( 2.f , 5.f ) , Vec2( 70.f , 18.f ) );
	m_enemytdlane1.Translate( Vec2( 0.f , 23.f ) );

	m_enemytdlane2 = m_enemytdlane1;
	m_enemytdlane2.Translate( Vec2( 0.f , 106.f ) );

	m_playertdlane1 = m_enemytdlane1;
	m_playertdlane2 = m_enemytdlane2;

	m_playertdlane1.Translate( Vec2(250.f,0.f) );
	m_playertdlane2.Translate( Vec2(250.f,0.f) );

	success = g_theAudio->CreateOrGetSound( "Data/Sounds/Success.wav" );
	error = g_theAudio->CreateOrGetSound( "Data/Sounds/Error.wav" );
	BackRound = g_theAudio->CreateOrGetSound( "Data/Sounds/bgm.wav" );

	g_theAudio->PlaySound( BackRound , true, 0.7f );

	m_winBackRound = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );
	m_winBackRound = m_winBackRound.GetPercentageOfAABB( 0.5f );

	m_winLoseBox = m_winBackRound.GetPercentageOfAABB( 0.5f );

	m_winBackTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/achievement/table.png" );
	m_winTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/win.png" );
	m_loseTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/lose.png" );

}

Game::~Game()
{
	delete m_camera;
	delete m_UICamera;
	delete m_devConsoleCamera;
}

void Game::Update( float deltaseconds )
{
	if(framecount<=1000 )
	framecount++;

	if ( !assetsLoaded )
	{
		if ( renderReached )
		{
			LoadTextures();
		}
	}

	if ( !assetsLoaded )
	{
		if ( framecount > 100 )
		{
			LoadTextures();
			assetsLoaded = true;
			return;
		}

		return;
	}

	if ( isInStartScreen )
	{
		m_startscreen->Update( deltaseconds );
		return;
	}

	if ( m_UncontollerHeroSpawnTimer->HasElapsed() )
	{
		SpawnUnControlledHeros();
		m_UncontollerHeroSpawnTimer->Reset();
	}


	UpdateHealthBars();
	UpdateCosts();
	CheckForWin();
	
	if ( gameOver )
	{
		return;
	}

	for ( int i = 0; i < m_playerMeleeHerosLane1_1.size(); i++ )
	{
		if ( m_playerMeleeHerosLane1_1[ i ] != nullptr )
		{
			m_playerMeleeHerosLane1_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_playerMeleeHerosLane1_2.size(); i++ )
	{
		if ( m_playerMeleeHerosLane1_2[ i ] != nullptr )
		{
			m_playerMeleeHerosLane1_2[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_playerMeleeHerosLane2_1.size(); i++ )
	{
		if ( m_playerMeleeHerosLane2_1[ i ] != nullptr )
		{
			m_playerMeleeHerosLane2_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_playerMeleeHerosLane2_2.size(); i++ )
	{
		if ( m_playerMeleeHerosLane2_2[ i ] != nullptr )
		{
			m_playerMeleeHerosLane2_2[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane1_1.size(); i++ )
	{
		if ( m_playerRangeHerosLane1_1[ i ] != nullptr )
		{
			m_playerRangeHerosLane1_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane1_2.size(); i++ )
	{
		if ( m_playerRangeHerosLane1_2[ i ] != nullptr )
		{
			m_playerRangeHerosLane1_2[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane2_1.size(); i++ )
	{
		if ( m_playerRangeHerosLane2_1[ i ] != nullptr )
		{
			m_playerRangeHerosLane2_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane2_2.size(); i++ )
	{
		if ( m_playerRangeHerosLane2_2[ i ] != nullptr )
		{
			m_playerRangeHerosLane2_2[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane1_1.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane1_1[ i ] != nullptr )
		{
			m_enemyMeleeHerosLane1_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane1_2.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane1_2[ i ] != nullptr )
		{
			m_enemyMeleeHerosLane1_2[ i ]->Update( deltaseconds );
		}
	}


	for ( int i = 0; i < m_enemyMeleeHerosLane2_1.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane2_1[ i ] != nullptr )
		{
			m_enemyMeleeHerosLane2_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane2_2.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane2_2[ i ] != nullptr )
		{
			m_enemyMeleeHerosLane2_2[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_enemyRangeHerosLane1_1.size(); i++ )
	{
		if ( m_enemyRangeHerosLane1_1[ i ] != nullptr )
		{
			m_enemyRangeHerosLane1_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_enemyRangeHerosLane1_2.size(); i++ )
	{
		if ( m_enemyRangeHerosLane1_2[ i ] != nullptr )
		{
			m_enemyRangeHerosLane1_2[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_enemyRangeHerosLane2_1.size(); i++ )
	{
		if ( m_enemyRangeHerosLane2_1[ i ] != nullptr )
		{
			m_enemyRangeHerosLane2_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_enemyRangeHerosLane2_2.size(); i++ )
	{
		if ( m_enemyRangeHerosLane2_2[ i ] != nullptr )
		{
			m_enemyRangeHerosLane2_2[ i ]->Update( deltaseconds );
		}
	}


	for ( int i = 0; i < m_shootObjectsLane1_1.size(); i++ )
	{
		if ( m_shootObjectsLane1_1[ i ] != nullptr )
		{
			m_shootObjectsLane1_1[ i ]->Update(deltaseconds);
		}
	}

	for ( int i = 0; i < m_UplayerLane1_1.size(); i++ )
	{
		if ( m_UplayerLane1_1[ i ] != nullptr )
		{
			m_UplayerLane1_1[ i ]->Update(deltaseconds);
		}
	}

	for ( int i = 0; i < m_UplayerLane1_2.size(); i++ )
	{
		if ( m_UplayerLane1_2[ i ] != nullptr )
		{
			m_UplayerLane1_2[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_UplayerLane2_1.size(); i++ )
	{
		if ( m_UplayerLane2_1[ i ] != nullptr )
		{
			m_UplayerLane2_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_UenemyLane1_1.size(); i++ )
	{
		if ( m_UenemyLane1_1[ i ] != nullptr )
		{
			m_UenemyLane1_1[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_UenemyLane1_2.size(); i++ )
	{
		if ( m_UenemyLane1_2[ i ] != nullptr )
		{
			m_UenemyLane1_2[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_UenemyLane2_1.size(); i++ )
	{
		if ( m_UenemyLane2_1[ i ] != nullptr )
		{
			m_UenemyLane2_1[ i ]->Update( deltaseconds );
		}
	}

	if ( m_playerTower1 != nullptr )
	{
		m_playerTower1->Update( deltaseconds );
	}

	if ( m_playerTower2 != nullptr )
	{
		m_playerTower2->Update( deltaseconds );
	}

	if ( m_enemyTower1 != nullptr )
	{
		m_enemyTower1->Update( deltaseconds );
	}

	if ( m_enemyTower2 != nullptr )
	{
		m_enemyTower2->Update( deltaseconds );
	}

	for ( int i = 0; i < m_towerDestroyers.size(); i++ )
	{
		if ( m_towerDestroyers[ i ] != nullptr )
		{
			m_towerDestroyers[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_towerBullets.size(); i++ )
	{
		if ( m_towerBullets[ i ] != nullptr )
		{
			m_towerBullets[ i ]->Update( deltaseconds );
		}
	}
	
	m_enemy->Update( deltaseconds );
	DetectPlayerActions();
	ExecutePlayerActions();
	m_selectedHUD->UpdateTexturesBasedOnSelection();

	m_meleeHeroHud->Update( deltaseconds );
	m_rangeHeroHud->Update( deltaseconds );
	m_towerHud->Update( deltaseconds );
	m_towerDestroyerHud->Update( deltaseconds );
	
	DeleteGarbageEntities();
}

void Game::Render()
{

	UpdateMousePosition();

	if ( !assetsLoaded )
	{
		m_loadingScreen->Render();
		if ( framecount > 100 )
		{
			renderReached = true;
		}
		return;
	}

	if ( isInStartScreen )
	{
		m_startscreen->Render();
		return;
	}

	g_theRenderer->BeginCamera( *m_camera );

	for ( int i = 0; i < m_Layer1Objects.size(); i++ )
	{
		g_theRenderer->BindTexture( m_Layer1Objects[ i ].tex );
		if ( !m_Layer1Objects[ i ].isXFlipped )
		{
			g_theRenderer->DrawAABB2D( m_Layer1Objects[ i ].object , Rgba8( 255 , 255 , 255 , 255 ) );
		}
		else
		{
			g_theRenderer->DrawXFlippedAABB2D( m_Layer1Objects[ i ].object , Rgba8( 255 , 255 , 255 , 255 ) );
		}
		g_theRenderer->BindTexture( nullptr );
	}

	for ( int i = 0; i < m_playerMeleeHerosLane1_2.size(); i++ )
	{
		if ( m_playerMeleeHerosLane1_2[ i ] != nullptr )
		{
			m_playerMeleeHerosLane1_2[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane1_2.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane1_2[ i ] == nullptr )
		{
			continue;
		}
		m_enemyMeleeHerosLane1_2[ i ]->Render();
	}

	for ( int i = 0; i < m_playerRangeHerosLane1_1.size(); i++ )
	{
		if ( m_playerRangeHerosLane1_1[ i ] != nullptr )
		{
			m_playerRangeHerosLane1_1[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_enemyRangeHerosLane1_1.size(); i++ )
	{
		if ( m_enemyRangeHerosLane1_1[ i ] == nullptr )
		{
			continue;
		}
		m_enemyRangeHerosLane1_1[ i ]->Render();
	}

	for ( int i = 0; i < m_playerMeleeHerosLane1_1.size(); i++ )
	{
		if ( m_playerMeleeHerosLane1_1[ i ] == nullptr )
		{
			continue;
		}
		m_playerMeleeHerosLane1_1[ i ]->Render();
	}

	for ( int i = 0; i < m_playerMeleeHerosLane2_2.size(); i++ )
	{
		if ( m_playerMeleeHerosLane2_2[ i ] != nullptr )
		{
			m_playerMeleeHerosLane2_2[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane2_2.size(); i++ )
	{
		if ( m_playerRangeHerosLane2_2[ i ] != nullptr )
		{
			m_playerRangeHerosLane2_2[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_playerMeleeHerosLane2_1.size(); i++ )
	{
		if ( m_playerMeleeHerosLane2_1[ i ] != nullptr )
		{
			m_playerMeleeHerosLane2_1[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane1_2.size(); i++ )
	{
		if ( m_playerRangeHerosLane1_2[ i ] != nullptr )
		{
			m_playerRangeHerosLane1_2[ i ]->Render();
		}
	}

	

	for ( int i = 0; i < m_playerRangeHerosLane2_1.size(); i++ )
	{
		if ( m_playerRangeHerosLane2_1[ i ] != nullptr )
		{
			m_playerRangeHerosLane2_1[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane1_1.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane1_1[ i ] == nullptr )
		{
			continue;
		}
		m_enemyMeleeHerosLane1_1[ i ]->Render();
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane2_2.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane2_2[ i ] == nullptr )
		{
			continue;
		}
		m_enemyMeleeHerosLane2_2[ i ]->Render();
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane2_1.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane2_1[ i ] == nullptr )
		{
			continue;
		}
		m_enemyMeleeHerosLane2_1[ i ]->Render();
	}

	for ( int i = 0; i < m_enemyRangeHerosLane1_2.size(); i++ )
	{
		if ( m_enemyRangeHerosLane1_2[ i ] == nullptr )
		{
			continue;
		}
		m_enemyRangeHerosLane1_2[ i ]->Render();
	}

	

	for ( int i = 0; i < m_enemyRangeHerosLane2_2.size(); i++ )
	{
		if ( m_enemyRangeHerosLane2_2[ i ] == nullptr )
		{
			continue;
		}
		m_enemyRangeHerosLane2_2[ i ]->Render();
	}

	for ( int i = 0; i < m_enemyRangeHerosLane2_1.size(); i++ )
	{
		if ( m_enemyRangeHerosLane2_1[ i ] == nullptr )
		{
			continue;
		}
		m_enemyRangeHerosLane2_1[ i ]->Render();
	}

	for ( int i = 0; i < m_shootObjectsLane1_1.size(); i++ )
	{
		if ( m_shootObjectsLane1_1[ i ] != nullptr )
		{
			m_shootObjectsLane1_1[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_UplayerLane1_1.size(); i++ )
	{
		if ( m_UplayerLane1_1[ i ] != nullptr )
		{
			m_UplayerLane1_1[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_UenemyLane1_1.size(); i++ )
	{
		if ( m_UenemyLane1_1[ i ] != nullptr )
		{
			m_UenemyLane1_1[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_UplayerLane1_2.size(); i++ )
	{
		if ( m_UplayerLane1_2[ i ] != nullptr )
		{
			m_UplayerLane1_2[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_UenemyLane1_2.size(); i++ )
	{
		if ( m_UenemyLane1_2[ i ] != nullptr )
		{
			m_UenemyLane1_2[ i ]->Render();
		}
	}


	for ( int i = 0; i < m_UplayerLane2_1.size(); i++ )
	{
		if ( m_UplayerLane2_1[ i ] != nullptr )
		{
			m_UplayerLane2_1[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_UenemyLane2_1.size(); i++ )
	{
		if ( m_UenemyLane2_1[ i ] != nullptr )
		{
			m_UenemyLane2_1[ i ]->Render();
		}
	}

	if ( m_playerTower1 != nullptr )
	{
		m_playerTower1->Render();
	}

	if ( m_playerTower2 != nullptr )
	{
		m_playerTower2->Render();
	}

	if ( m_enemyTower1 != nullptr )
	{
		m_enemyTower1->Render();
	}

	if ( m_enemyTower2 != nullptr )
	{
		m_enemyTower2->Render();
	}

	for ( int i = 0; i < m_towerBullets.size(); i++ )
	{
		if ( m_towerBullets[ i ] != nullptr )
		{
			m_towerBullets[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_towerDestroyers.size(); i++ )
	{
		if ( m_towerDestroyers[ i ] != nullptr )
		{
			m_towerDestroyers[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_Layer2Objects.size(); i++ )
	{
		g_theRenderer->BindTexture( m_Layer2Objects[ i ].tex );
		if ( !m_Layer2Objects[ i ].isXFlipped )
		{
			g_theRenderer->DrawAABB2D( m_Layer2Objects[ i ].object , Rgba8( 255 , 255 , 255 , 255 ) );
		}
		else
		{
			g_theRenderer->DrawXFlippedAABB2D( m_Layer2Objects[ i ].object , Rgba8( 255 , 255 , 255 , 255 ) );
		}
		g_theRenderer->BindTexture( nullptr );
	}

	//DisplayMouseInfo();
	g_theRenderer->EndCamera( *m_camera );

	g_theRenderer->BeginCamera( *m_UICamera );

	g_theRenderer->DrawAABB2D( m_UIAreaBot , Rgba8( 200 , 200 , 200 , 125 ) );
	g_theRenderer->DrawAABB2D( m_UIAreaTop , Rgba8( 200 , 200 , 200 , 125 ) );

	if ( ( m_playerAction == ACTION_SELECT_MELEE || m_playerAction == ACTION_SELECT_RANGE ) )
	{
		if ( m_lane1.IsPointInside( m_mousePosition ) )
		{
			g_theRenderer->DrawAABB2D( m_lane1 , Rgba8( 235 , 235 , 235 , 100 ) );
		}
		if ( m_lane2.IsPointInside( m_mousePosition ) )
		{
			g_theRenderer->DrawAABB2D( m_lane2 , Rgba8( 235 , 235 , 235 , 100 ) );
		}
	}

	if ( m_playerAction == ACTION_SELECT_TOWER )
	{
		if ( IsPointInsideDisc2D( m_mousePosition , m_ptowerBase1Box.GetCenter() , 5.f ) )
		{
			g_theRenderer->DrawDisc( m_ptowerBase1Box.GetCenter() , 8.f , Rgba8( 235 , 235 , 235 , 100 ) );
		}

		if ( IsPointInsideDisc2D( m_mousePosition , m_ptowerBase2Box.GetCenter() , 5.f ) )
		{
			g_theRenderer->DrawDisc( m_ptowerBase2Box.GetCenter() , 8.f , Rgba8( 235 , 235 , 235 , 100 ) );
		}
	}

	

	AABB2 castleHealthBar = m_healthBar.GetBoxAtBottom( m_castleHealth / 200.f );
	AABB2 enemycastleHealthBar = m_enemyHealthBar.GetBoxAtBottom( m_enemyCastleHealth / 200.f );

	g_theRenderer->BindTexture( m_castleHealthBaseLoadBar );
	g_theRenderer->DrawAABB2D( m_healthBarBase , Rgba8() );
	g_theRenderer->BindTexture( m_castleLoadBar );
	g_theRenderer->DrawAABB2D( castleHealthBar , Rgba8() );
	g_theRenderer->BindTexture( m_castleHealthBaseLoadBar );
	g_theRenderer->DrawAABB2D( m_enemyHealthBarBase , Rgba8() );
	g_theRenderer->BindTexture( m_castleLoadBar );
	g_theRenderer->DrawAABB2D( enemycastleHealthBar , Rgba8() );
	
	m_meleeHeroHud->Render();
	m_rangeHeroHud->Render();
	m_towerHud->Render();
	m_towerDestroyerHud->Render();
	m_selectedHUD->Render();

	DisplayCoins();
	//DisplayFPS();

	if ( gameOver )
	{
		
		if ( didPlayerWin )
		{
			g_theRenderer->BindTexture( m_winTex );
			g_theRenderer->DrawAABB2D( m_winLoseBox,Rgba8() );
		}
		else
		{
			g_theRenderer->BindTexture( m_loseTex );
			g_theRenderer->DrawAABB2D( m_winLoseBox , Rgba8() );
		}
	}
	g_theRenderer->EndCamera( *m_camera );
}

void Game::LoadTextures()
{
	m_guiBackround = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_3/layers/land.png" );

	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

	m_castleHealthBaseLoadBar = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_bg - Copy.png" );
	m_castleLoadBar = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_2 - Copy.png" );

	LoadUncontrolledHeroTextures();

	m_startscreen = new StartScreen( this );

	LayeredObject mapBackround;

	mapBackround.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_3/layers/land.png" );
	mapBackround.object = AABB2( Vec2( 0.f , 0.f ) , Vec2( 320.f , 180.f ) );

	LayeredObject basePlayer_1;
	basePlayer_1.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_1.object = AABB2( Vec2( 10.f , 50.f ) , Vec2( 25.f , 80.f ) );

	LayeredObject basePlayer_2;
	basePlayer_2.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_2.object = AABB2( Vec2( 10.f , 60.f ) , Vec2( 25.f , 90.f ) );

	LayeredObject basePlayer_3;
	basePlayer_3.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_3.object = AABB2( Vec2( 10.f , 70.f ) , Vec2( 25.f , 100.f ) );

	LayeredObject basePlayer_4;
	basePlayer_4.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_4.object = AABB2( Vec2( 10.f , 80.f ) , Vec2( 25.f , 110.f ) );

	LayeredObject basePlayer_5;
	basePlayer_5.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_5.object = AABB2( Vec2( 10.f , 90.f ) , Vec2( 25.f , 120.f ) );

	LayeredObject basePlayer_6;
	basePlayer_6.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_6.object = AABB2( Vec2( 10.f , 100.f ) , Vec2( 25.f , 130.f ) );

	LayeredObject baseEnemy_1;
	baseEnemy_1.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_1.object = AABB2( Vec2( 295.f , 50.f ) , Vec2( 310.f , 80.f ) );

	LayeredObject baseEnemy_2;
	baseEnemy_2.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_2.object = AABB2( Vec2( 295.f , 60.f ) , Vec2( 310.f , 90.f ) );

	LayeredObject baseEnemy_3;
	baseEnemy_3.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_3.object = AABB2( Vec2( 295.f , 70.f ) , Vec2( 310.f , 100.f ) );

	LayeredObject baseEnemy_4;
	baseEnemy_4.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_4.object = AABB2( Vec2( 295.f , 80.f ) , Vec2( 310.f , 110.f ) );

	LayeredObject baseEnemy_5;
	baseEnemy_5.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_5.object = AABB2( Vec2( 295.f , 90.f ) , Vec2( 310.f , 120.f ) );

	LayeredObject baseEnemy_6;
	baseEnemy_6.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_6.object = AABB2( Vec2( 295.f , 100.f ) , Vec2( 310.f , 130.f ) );


	m_Layer1Objects.push_back( mapBackround );
	m_Layer1Objects.push_back( basePlayer_6 );
	m_Layer1Objects.push_back( basePlayer_5 );
	m_Layer1Objects.push_back( basePlayer_4 );
	m_Layer1Objects.push_back( basePlayer_3 );
	m_Layer1Objects.push_back( basePlayer_2 );
	m_Layer1Objects.push_back( basePlayer_1 );
	m_Layer1Objects.push_back( baseEnemy_6 );
	m_Layer1Objects.push_back( baseEnemy_5 );
	m_Layer1Objects.push_back( baseEnemy_4 );
	m_Layer1Objects.push_back( baseEnemy_3 );
	m_Layer1Objects.push_back( baseEnemy_2 );
	m_Layer1Objects.push_back( baseEnemy_1 );
	


	m_enemy = new AIController( this );


	m_lane2 = AABB2( Vec2( 25.f , 105.f ) , Vec2( 295.f , 120.f ) );
	m_lane1 = AABB2( Vec2( 25.f , 55.f ) , Vec2( 295.f , 70.f ) );

	

	LayeredObject towerSpot1;
	towerSpot1.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet2/PNG/game_background_4/layers/dot.png" );
	towerSpot1.object = AABB2( Vec2( 25.f , 10.f ) , Vec2( 45.f , 30.f ) );
	towerSpot1.object.Translate( Vec2( 50.f , 15.f ) );

	LayeredObject towerSpot2;
	towerSpot2.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet2/PNG/game_background_4/layers/dot.png" );
	towerSpot2.object = AABB2( Vec2( 25.f , 10.f ) , Vec2( 45.f , 30.f ) );
	towerSpot2.object.Translate( Vec2( 200.f , 15.f ) );

	LayeredObject towerSpot3;
	towerSpot3.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet2/PNG/game_background_4/layers/dot.png" );
	towerSpot3.object = AABB2( Vec2( 25.f , 10.f ) , Vec2( 45.f , 30.f ) );
	towerSpot3.object.Translate( Vec2( 50.f , 120.f ) );

	LayeredObject towerSpot4;
	towerSpot4.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet2/PNG/game_background_4/layers/dot.png" );
	towerSpot4.object = AABB2( Vec2( 25.f , 10.f ) , Vec2( 45.f , 30.f ) );
	towerSpot4.object.Translate( Vec2( 200.f , 120.f ) );

	m_Layer1Objects.push_back( towerSpot1 );
	m_Layer1Objects.push_back( towerSpot2 );
	m_Layer1Objects.push_back( towerSpot3 );
	m_Layer1Objects.push_back( towerSpot4 );


	LayeredObject decor1;
	decor1.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet2/PNG/game_background_4/layers/tree_1.png" );
	decor1.object = AABB2( Vec2( 25.f , 10.f ) , Vec2( 65.f , 60.f ) );
	decor1.object.Translate( Vec2( 115.f , 65.f ) );


	m_Layer2Objects.push_back( decor1 );


	

	LoadPlayerMeleeAnimations();
	LoadEnemyMeleeAnimations();
	LoadPlayerRangeAnimations();
	LoadEnemyRangeAnimations();
	LoadPlayerBulletTextures();
	LoadTowerTextures();
	LoadTowerDestroyerTextures();
	LoadTowerBulletTextures();
	CreateHUD();


	m_ptowerBase1Box = AABB2( Vec2( 84.f , 25.f ) , Vec2( 102.f , 40.f ) );
	m_ptowerBase1Box.Translate( Vec2( -8.f , 3.f ) );

	m_ptowerBase2Box = m_ptowerBase1Box;
	m_ptowerBase2Box.Translate( Vec2( 0.f , 105.f ) );

	m_etowerBase1Box = m_ptowerBase1Box;
	m_etowerBase1Box.Translate( Vec2( 150.f , 0.f ) );

	m_etowerBase2Box = m_etowerBase1Box;
	m_etowerBase2Box.Translate( Vec2( 0.f , 105.f ) );

	m_UncontollerHeroSpawnClock = new Clock();
	m_UncontollerHeroSpawnTimer = new Timer();
	m_UncontollerHeroSpawnTimer->SetSeconds( m_UncontollerHeroSpawnClock , 6.f );


	m_winBackRound = AABB2( Vec2(50.f,50.f) , Vec2(100.f,100.f) );


	m_winBackTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/achievement/table.png" );
	m_gameOverTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/GameOver.png" );
	m_winTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/win.png" );
	m_loseTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/lose.png" );
	


	//UHeroTest();
	//TDtest();
}

void Game::CheckForWin()
{
	if ( m_castleHealth <= 0.f )
	{
		gameOver = true;
		didPlayerWin = false;
		return;
	}

	if ( m_enemyCastleHealth <= 0.f )
	{
		gameOver = true;
		didPlayerWin = true;
		return;
	}
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

void Game::DisplayMouseInfo()
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	Vec2 mousePositionScreen;

	mousePositionScreen.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	mousePositionScreen.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	AABB2 box = AABB2( Vec2(mousePositionScreen.x,mousePositionScreen.y) , Vec2(mousePositionScreen.x+50.f,mousePositionScreen.y+20.f) );

	std::string s = "";
	s += std::to_string( mousePositionScreen.x ) + " " + std::to_string( mousePositionScreen.y );

	std::vector<Vertex_PCU> verts;
	m_font->AddVertsForTextInBox2D( verts , box , 2.f , s , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.f , 0.f ) );

	g_theRenderer->DrawAABB2D( box , Rgba8( 255 , 255 , 255 , 100 ) );
	g_theRenderer->BindTexture( m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( verts );
	g_theRenderer->BindTexture( nullptr );


}

void Game::DisplayFPS()
{
	AABB2 box = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );
	box.CarveBoxOffTop( 0.05f );

	std::vector<Vertex_PCU> v1;

	m_font->AddVertsForTextInBox2D( v1 , box , 3.5f ,   "(FPS =" + std::to_string( g_theapp->GetFPS() ) + ")" , Rgba8( 0 , 0 , 100 , 255 ) , 1.f , Vec2( 0.95f , 0.0f ) );

	g_theRenderer->BeginCamera( *m_UICamera );
	g_theRenderer->BindTexture( m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( v1 );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera( *m_UICamera );

}

void Game::LoadPlayerMeleeAnimations()
{
	std::vector<Texture*> playerUpdgrade0MeleeAttackTextures;

	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_000.png" );
	Texture* t2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_001.png" );
	Texture* t3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_002.png" );
	Texture* t4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_003.png" );
	Texture* t5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_004.png" );
	Texture* t6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_005.png" );
	Texture* t7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_006.png" );
	Texture* t8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_007.png" );
	Texture* t9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_008.png" );
	Texture* t10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_009.png" );
	Texture* t11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_010.png" );
	Texture* t12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Slashing/Slashing_011.png" );
	

	playerUpdgrade0MeleeAttackTextures.push_back( t1 );
	playerUpdgrade0MeleeAttackTextures.push_back( t2 );
	playerUpdgrade0MeleeAttackTextures.push_back( t3 );
	playerUpdgrade0MeleeAttackTextures.push_back( t4 );
	playerUpdgrade0MeleeAttackTextures.push_back( t5 );
	playerUpdgrade0MeleeAttackTextures.push_back( t6 );
	playerUpdgrade0MeleeAttackTextures.push_back( t7 );
	playerUpdgrade0MeleeAttackTextures.push_back( t8 );
	playerUpdgrade0MeleeAttackTextures.push_back( t9 );
	playerUpdgrade0MeleeAttackTextures.push_back( t10 );
	playerUpdgrade0MeleeAttackTextures.push_back( t11 );
	playerUpdgrade0MeleeAttackTextures.push_back( t12 );

	m_PlayerUpgrade0MeleeAttack = new SpriteAnimDefTex(0,playerUpdgrade0MeleeAttackTextures.size()-1,1.f);
	m_PlayerUpgrade0MeleeAttack->m_animations = playerUpdgrade0MeleeAttackTextures;

	std::vector<Texture*> playerUpgrade0MeleeRunTextures;
	Texture* r1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_000.png" );
	Texture* r2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_001.png" );
	Texture* r3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_002.png" );
	Texture* r4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_003.png" );
	Texture* r5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_004.png" );
	Texture* r6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_005.png" );
	Texture* r7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_006.png" );
	Texture* r8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_007.png" );
	Texture* r9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_008.png" );
	Texture* r10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_009.png" );
	Texture* r11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_010.png" );
	Texture* r12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_011.png" );
	Texture* r13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_012.png" );
	Texture* r14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_013.png" );
	Texture* r15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_014.png" );
	Texture* r16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_015.png" );
	Texture* r17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_016.png" );
	Texture* r18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_017.png" );
	Texture* r19 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_018.png" );
	Texture* r20 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_019.png" );
	Texture* r21 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_020.png" );
	Texture* r22 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_021.png" );
	Texture* r23 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_022.png" );
	Texture* r24 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Walking/Walking_023.png" );
	

	playerUpgrade0MeleeRunTextures.push_back( r1 );
	playerUpgrade0MeleeRunTextures.push_back( r2 );
	playerUpgrade0MeleeRunTextures.push_back( r3 );
	playerUpgrade0MeleeRunTextures.push_back( r4 );
	playerUpgrade0MeleeRunTextures.push_back( r5 );
	playerUpgrade0MeleeRunTextures.push_back( r6 );
	playerUpgrade0MeleeRunTextures.push_back( r7 );
	playerUpgrade0MeleeRunTextures.push_back( r8 );
	playerUpgrade0MeleeRunTextures.push_back( r9 );
	playerUpgrade0MeleeRunTextures.push_back( r10 );
	playerUpgrade0MeleeRunTextures.push_back( r11 );
	playerUpgrade0MeleeRunTextures.push_back( r12 );
	playerUpgrade0MeleeRunTextures.push_back( r13 );
	playerUpgrade0MeleeRunTextures.push_back( r14 );
	playerUpgrade0MeleeRunTextures.push_back( r15 );
	playerUpgrade0MeleeRunTextures.push_back( r16 );
	playerUpgrade0MeleeRunTextures.push_back( r17 );
	playerUpgrade0MeleeRunTextures.push_back( r18 );
	playerUpgrade0MeleeRunTextures.push_back( r19 );
	playerUpgrade0MeleeRunTextures.push_back( r20 );
	playerUpgrade0MeleeRunTextures.push_back( r21 );
	playerUpgrade0MeleeRunTextures.push_back( r22 );
	playerUpgrade0MeleeRunTextures.push_back( r23 );
	playerUpgrade0MeleeRunTextures.push_back( r24 );

	m_PlayerUpgrade0MeleeRun = new SpriteAnimDefTex( 0 , playerUpgrade0MeleeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade0MeleeRun->m_animations = playerUpgrade0MeleeRunTextures;

	std::vector<Texture*> playerUpdgrade1MeleeAttackTextures;
	Texture* tt1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_000.png" );
	Texture* tt2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_001.png" );
	Texture* tt3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_002.png" );
	Texture* tt4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_003.png" );
	Texture* tt5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_004.png" );
	Texture* tt6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_005.png" );
	Texture* tt7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_006.png" );
	Texture* tt8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_007.png" );
	Texture* tt9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_008.png" );
	Texture* tt10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_009.png" );
	Texture* tt11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_010.png" );
	Texture* tt12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_011.png" );

	
	playerUpdgrade1MeleeAttackTextures.push_back( tt1 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt2 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt3 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt4 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt5 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt6 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt7 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt8 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt9 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt10 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt11 );
	playerUpdgrade1MeleeAttackTextures.push_back( tt12 );

	m_PlayerUpgrade1MeleeAttack = new SpriteAnimDefTex( 0 , playerUpdgrade1MeleeAttackTextures.size()-1 , 1.f );
	m_PlayerUpgrade1MeleeAttack->m_animations = playerUpdgrade1MeleeAttackTextures;

	std::vector<Texture*> playerUpgrade1MeleeRunTextures;
	Texture* rr1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_000.png" );
	Texture* rr2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_001.png" );
	Texture* rr3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_002.png" );
	Texture* rr4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_003.png" );
	Texture* rr5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_004.png" );
	Texture* rr6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_005.png" );
	Texture* rr7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_006.png" );
	Texture* rr8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_007.png" );
	Texture* rr9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_008.png" );
	Texture* rr10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_009.png" );
	Texture* rr11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_010.png" );
	Texture* rr12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_011.png" );
	Texture* rr13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_012.png" );
	Texture* rr14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_013.png" );
	Texture* rr15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_014.png" );
	Texture* rr16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_015.png" );
	Texture* rr17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_016.png" );
	Texture* rr18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_017.png" );
	Texture* rr19 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_018.png" );
	Texture* rr20 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_019.png" );
	Texture* rr21 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_020.png" );
	Texture* rr22 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_021.png" );
	Texture* rr23 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_022.png" );
	Texture* rr24 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Walking/Walking_023.png" );
	

	playerUpgrade1MeleeRunTextures.push_back( rr1 );
	playerUpgrade1MeleeRunTextures.push_back( rr2 );
	playerUpgrade1MeleeRunTextures.push_back( rr3 );
	playerUpgrade1MeleeRunTextures.push_back( rr4 );
	playerUpgrade1MeleeRunTextures.push_back( rr5 );
	playerUpgrade1MeleeRunTextures.push_back( rr6 );
	playerUpgrade1MeleeRunTextures.push_back( rr7 );
	playerUpgrade1MeleeRunTextures.push_back( rr8 );
	playerUpgrade1MeleeRunTextures.push_back( rr9 );
	playerUpgrade1MeleeRunTextures.push_back( rr10 );
	playerUpgrade1MeleeRunTextures.push_back( rr11 );
	playerUpgrade1MeleeRunTextures.push_back( rr12 );
	playerUpgrade1MeleeRunTextures.push_back( rr13 );
	playerUpgrade1MeleeRunTextures.push_back( rr14 );
	playerUpgrade1MeleeRunTextures.push_back( rr15 );
	playerUpgrade1MeleeRunTextures.push_back( rr16 );
	playerUpgrade1MeleeRunTextures.push_back( rr17 );
	playerUpgrade1MeleeRunTextures.push_back( rr18 );
	playerUpgrade1MeleeRunTextures.push_back( rr19 );
	playerUpgrade1MeleeRunTextures.push_back( rr20 );
	playerUpgrade1MeleeRunTextures.push_back( rr21 );
	playerUpgrade1MeleeRunTextures.push_back( rr22 );
	playerUpgrade1MeleeRunTextures.push_back( rr23 );
	playerUpgrade1MeleeRunTextures.push_back( rr24 );

	m_PlayerUpgrade1MeleeRun = new SpriteAnimDefTex( 0 , playerUpgrade1MeleeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade1MeleeRun->m_animations = playerUpgrade1MeleeRunTextures;

	std::vector<Texture*> playerUpdgrade2MeleeAttackTextures;
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_007.png" );
	Texture* a9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_008.png" );
	Texture* a10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_009.png" );
	Texture* a11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_010.png" );
	Texture* a12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Slashing/Slashing_011.png" );
	
	playerUpdgrade2MeleeAttackTextures.push_back( a1 );
	playerUpdgrade2MeleeAttackTextures.push_back( a2 );
	playerUpdgrade2MeleeAttackTextures.push_back( a3 );
	playerUpdgrade2MeleeAttackTextures.push_back( a4 );
	playerUpdgrade2MeleeAttackTextures.push_back( a5 );
	playerUpdgrade2MeleeAttackTextures.push_back( a6 );
	playerUpdgrade2MeleeAttackTextures.push_back( a7 );
	playerUpdgrade2MeleeAttackTextures.push_back( a8 );
	playerUpdgrade2MeleeAttackTextures.push_back( a9 );
	playerUpdgrade2MeleeAttackTextures.push_back( a10 );
	playerUpdgrade2MeleeAttackTextures.push_back( a11 );
	playerUpdgrade2MeleeAttackTextures.push_back( a12 );

	m_PlayerUpgrade2MeleeAttack = new SpriteAnimDefTex( 0 , playerUpdgrade2MeleeAttackTextures.size() - 1 , 1.f );
	m_PlayerUpgrade2MeleeAttack->m_animations = playerUpdgrade2MeleeAttackTextures;

	std::vector<Texture*> playerUpgrade2MeleeRunTextures;
	
	Texture* aa1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_000.png" );
	Texture* aa2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_001.png" );
	Texture* aa3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_002.png" );
	Texture* aa4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_003.png" );
	Texture* aa5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_004.png" );
	Texture* aa6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_005.png" );
	Texture* aa7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_006.png" );
	Texture* aa8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_007.png" );
	Texture* aa9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_008.png" );
	Texture* aa10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_009.png" );
	Texture* aa11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_010.png" );
	Texture* aa12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_011.png" );
	Texture* aa13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_012.png" );
	Texture* aa14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_013.png" );
	Texture* aa15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_014.png" );
	Texture* aa16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_015.png" );
	Texture* aa17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_016.png" );
	Texture* aa18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_017.png" );
	Texture* aa19 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_018.png" );
	Texture* aa20 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_019.png" );
	Texture* aa21 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_020.png" );
	Texture* aa22 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_021.png" );
	Texture* aa23 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_022.png" );
	Texture* aa24 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Walking/Walking_023.png" );
	

	playerUpgrade2MeleeRunTextures.push_back( aa1 );
	playerUpgrade2MeleeRunTextures.push_back( aa2 );
	playerUpgrade2MeleeRunTextures.push_back( aa3 );
	playerUpgrade2MeleeRunTextures.push_back( aa4 );
	playerUpgrade2MeleeRunTextures.push_back( aa5 );
	playerUpgrade2MeleeRunTextures.push_back( aa6 );
	playerUpgrade2MeleeRunTextures.push_back( aa7 );
	playerUpgrade2MeleeRunTextures.push_back( aa8 );
	playerUpgrade2MeleeRunTextures.push_back( aa9 );
	playerUpgrade2MeleeRunTextures.push_back( aa10 );
	playerUpgrade2MeleeRunTextures.push_back( aa11 );
	playerUpgrade2MeleeRunTextures.push_back( aa12 );
	playerUpgrade2MeleeRunTextures.push_back( aa13 );
	playerUpgrade2MeleeRunTextures.push_back( aa14 );
	playerUpgrade2MeleeRunTextures.push_back( aa15 );
	playerUpgrade2MeleeRunTextures.push_back( aa16 );
	playerUpgrade2MeleeRunTextures.push_back( aa17 );
	playerUpgrade2MeleeRunTextures.push_back( aa18 );
	playerUpgrade2MeleeRunTextures.push_back( aa19 );
	playerUpgrade2MeleeRunTextures.push_back( aa20 );
	playerUpgrade2MeleeRunTextures.push_back( aa21 );
	playerUpgrade2MeleeRunTextures.push_back( aa22 );
	playerUpgrade2MeleeRunTextures.push_back( aa23 );
	playerUpgrade2MeleeRunTextures.push_back( aa24 );

	m_PlayerUpgrade2MeleeRun = new SpriteAnimDefTex( 0 , playerUpgrade2MeleeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade2MeleeRun->m_animations = playerUpgrade2MeleeRunTextures;
	
	std::vector<Texture*> playerUpdgrade3MeleeAttackTextures;

	Texture* x1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_000.png" );
	Texture* x2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_001.png" );
	Texture* x3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_002.png" );
	Texture* x4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_003.png" );
	Texture* x5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_004.png" );
	Texture* x6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_005.png" );
	Texture* x7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_006.png" );
	Texture* x8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_007.png" );
	Texture* x9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_008.png" );
	Texture* x10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_009.png" );
	Texture* x11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_010.png" );
	Texture* x12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Slashing/Slashing_011.png" );
	

	playerUpdgrade3MeleeAttackTextures.push_back( x1 );
	playerUpdgrade3MeleeAttackTextures.push_back( x2 );
	playerUpdgrade3MeleeAttackTextures.push_back( x3 );
	playerUpdgrade3MeleeAttackTextures.push_back( x4 );
	playerUpdgrade3MeleeAttackTextures.push_back( x5 );
	playerUpdgrade3MeleeAttackTextures.push_back( x6 );
	playerUpdgrade3MeleeAttackTextures.push_back( x7 );
	playerUpdgrade3MeleeAttackTextures.push_back( x8 );
	playerUpdgrade3MeleeAttackTextures.push_back( x9 );
	playerUpdgrade3MeleeAttackTextures.push_back( x10 );
	playerUpdgrade3MeleeAttackTextures.push_back( x11 );
	playerUpdgrade3MeleeAttackTextures.push_back( x12 );
	
	m_PlayerUpgrade3MeleeAttack = new SpriteAnimDefTex( 0 , playerUpdgrade3MeleeAttackTextures.size() - 1 , 1.f );
	m_PlayerUpgrade3MeleeAttack->m_animations = playerUpdgrade3MeleeAttackTextures;

	std::vector<Texture*>  playerUpdgrade3MeleeRunTextures;

	Texture* xx1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_000.png" );
	Texture* xx2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_001.png" );
	Texture* xx3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_002.png" );
	Texture* xx4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_003.png" );
	Texture* xx5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_004.png" );
	Texture* xx6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_005.png" );
	Texture* xx7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_006.png" );
	Texture* xx8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_007.png" );
	Texture* xx9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_008.png" );
	Texture* xx10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_009.png" );
	Texture* xx11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_010.png" );
	Texture* xx12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_011.png" );
	Texture* xx13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_012.png" );
	Texture* xx14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_013.png" );
	Texture* xx15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_014.png" );
	Texture* xx16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_015.png" );
	Texture* xx17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_016.png" );
	Texture* xx18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_017.png" );
	Texture* xx19 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_018.png" );
	Texture* xx20 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_019.png" );
	Texture* xx21 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_020.png" );
	Texture* xx22 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_021.png" );
	Texture* xx23 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_022.png" );
	Texture* xx24 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Walking/Walking_023.png" );

	playerUpdgrade3MeleeRunTextures.push_back( xx1 );
	playerUpdgrade3MeleeRunTextures.push_back( xx2 );
	playerUpdgrade3MeleeRunTextures.push_back( xx3 );
	playerUpdgrade3MeleeRunTextures.push_back( xx4 );
	playerUpdgrade3MeleeRunTextures.push_back( xx5 );
	playerUpdgrade3MeleeRunTextures.push_back( xx6 );
	playerUpdgrade3MeleeRunTextures.push_back( xx7 );
	playerUpdgrade3MeleeRunTextures.push_back( xx8 );
	playerUpdgrade3MeleeRunTextures.push_back( xx9 );
	playerUpdgrade3MeleeRunTextures.push_back( xx10 );
	playerUpdgrade3MeleeRunTextures.push_back( xx11 );
	playerUpdgrade3MeleeRunTextures.push_back( xx12 );
	playerUpdgrade3MeleeRunTextures.push_back( xx13 );
	playerUpdgrade3MeleeRunTextures.push_back( xx14 );
	playerUpdgrade3MeleeRunTextures.push_back( xx15 );
	playerUpdgrade3MeleeRunTextures.push_back( xx16 );
	playerUpdgrade3MeleeRunTextures.push_back( xx17 );
	playerUpdgrade3MeleeRunTextures.push_back( xx18 );
	playerUpdgrade3MeleeRunTextures.push_back( xx19 );
	playerUpdgrade3MeleeRunTextures.push_back( xx20 );
	playerUpdgrade3MeleeRunTextures.push_back( xx21 );
	playerUpdgrade3MeleeRunTextures.push_back( xx22 );
	playerUpdgrade3MeleeRunTextures.push_back( xx23 );
	playerUpdgrade3MeleeRunTextures.push_back( xx24 );
	

	m_PlayerUpgrade3MeleeRun = new SpriteAnimDefTex( 0 , playerUpdgrade3MeleeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade3MeleeRun->m_animations = playerUpdgrade3MeleeRunTextures;
	

}

void Game::LoadEnemyMeleeAnimations()
{
	std::vector<Texture*> enemyUpdgrade0MeleeAttackTextures;

	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_000.png" );
	Texture* t2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_001.png" );
	Texture* t3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_002.png" );
	Texture* t4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_003.png" );
	Texture* t5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_004.png" );
	Texture* t6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_005.png" );
	Texture* t7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_006.png" );
	Texture* t8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_007.png" );
	Texture* t9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_008.png" );
	Texture* t10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_009.png" );
	Texture* t11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_010.png" );
	Texture* t12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Slashing/Slashing_011.png" );



	enemyUpdgrade0MeleeAttackTextures.push_back( t1 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t2 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t3 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t4 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t5 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t6 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t7 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t8 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t9 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t10 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t11 );
	enemyUpdgrade0MeleeAttackTextures.push_back( t12 );

	m_EnemyUpgrade0MeleeAttack = new SpriteAnimDefTex( 0 , enemyUpdgrade0MeleeAttackTextures.size() - 1 , 1.f );
	m_EnemyUpgrade0MeleeAttack->m_animations = enemyUpdgrade0MeleeAttackTextures;

	std::vector<Texture*> enemyUpgrade0MeleeRunTextures;
	Texture* r1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_000.png" );
	Texture* r2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_001.png" );
	Texture* r3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_002.png" );
	Texture* r4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_003.png" );
	Texture* r5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_004.png" );
	Texture* r6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_005.png" );
	Texture* r7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_006.png" );
	Texture* r8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_007.png" );
	Texture* r9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_008.png" );
	Texture* r10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_009.png" );
	Texture* r11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_010.png" );
	Texture* r12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_011.png" );
	Texture* r13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_012.png" );
	Texture* r14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_013.png" );
	Texture* r15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_014.png" );
	Texture* r16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_015.png" );
	Texture* r17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_016.png" );
	Texture* r18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_017.png" );
	Texture* r19 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_018.png" );
	Texture* r20 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_019.png" );
	Texture* r21 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_020.png" );
	Texture* r22 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_021.png" );
	Texture* r23 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_022.png" );
	Texture* r24 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Anubis/PNG/PNG Sequences/Walking/Walking_023.png" );


	enemyUpgrade0MeleeRunTextures.push_back( r1 );
	enemyUpgrade0MeleeRunTextures.push_back( r2 );
	enemyUpgrade0MeleeRunTextures.push_back( r3 );
	enemyUpgrade0MeleeRunTextures.push_back( r4 );
	enemyUpgrade0MeleeRunTextures.push_back( r5 );
	enemyUpgrade0MeleeRunTextures.push_back( r6 );
	enemyUpgrade0MeleeRunTextures.push_back( r7 );
	enemyUpgrade0MeleeRunTextures.push_back( r8 );
	enemyUpgrade0MeleeRunTextures.push_back( r9 );
	enemyUpgrade0MeleeRunTextures.push_back( r10 );
	enemyUpgrade0MeleeRunTextures.push_back( r11 );
	enemyUpgrade0MeleeRunTextures.push_back( r12 );
	enemyUpgrade0MeleeRunTextures.push_back( r13 );
	enemyUpgrade0MeleeRunTextures.push_back( r14 );
	enemyUpgrade0MeleeRunTextures.push_back( r15 );
	enemyUpgrade0MeleeRunTextures.push_back( r16 );
	enemyUpgrade0MeleeRunTextures.push_back( r17 );
	enemyUpgrade0MeleeRunTextures.push_back( r18 );
	enemyUpgrade0MeleeRunTextures.push_back( r19 );
	enemyUpgrade0MeleeRunTextures.push_back( r20 );
	enemyUpgrade0MeleeRunTextures.push_back( r21 );
	enemyUpgrade0MeleeRunTextures.push_back( r22 );
	enemyUpgrade0MeleeRunTextures.push_back( r23 );
	enemyUpgrade0MeleeRunTextures.push_back( r24 );

	m_EnemyUpgrade0MeleeRun = new SpriteAnimDefTex( 0 , enemyUpgrade0MeleeRunTextures.size() - 1 , 1.f );
	m_EnemyUpgrade0MeleeRun->m_animations = enemyUpgrade0MeleeRunTextures;

	std::vector<Texture*> enemyUpdgrade1MeleeAttackTextures;
	Texture* tt1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_000.png" );
	Texture* tt2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_001.png" );
	Texture* tt3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_002.png" );
	Texture* tt4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_003.png" );
	Texture* tt5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_004.png" );
	Texture* tt6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_005.png" );
	Texture* tt7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_006.png" );
	Texture* tt8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_007.png" );
	Texture* tt9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_008.png" );
	Texture* tt10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_009.png" );
	Texture* tt11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_010.png" );
	Texture* tt12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Slashing/Slashing_011.png" );

	enemyUpdgrade1MeleeAttackTextures.push_back( tt1 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt2 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt3 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt4 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt5 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt6 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt7 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt8 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt9 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt10 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt11 );
	enemyUpdgrade1MeleeAttackTextures.push_back( tt12 );

	m_EnemyUpgrade1MeleeAttack = new SpriteAnimDefTex( 0 , enemyUpdgrade1MeleeAttackTextures.size() - 1 , 1.f );
	m_EnemyUpgrade1MeleeAttack->m_animations = enemyUpdgrade1MeleeAttackTextures;

	std::vector<Texture*> enemyUpgrade1MeleeRunTextures;
	Texture* rr1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_000.png" );
	Texture* rr2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_001.png" );
	Texture* rr3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_002.png" );
	Texture* rr4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_003.png" );
	Texture* rr5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_004.png" );
	Texture* rr6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_005.png" );
	Texture* rr7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_006.png" );
	Texture* rr8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_007.png" );
	Texture* rr9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_008.png" );
	Texture* rr10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_009.png" );
	Texture* rr11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_010.png" );
	Texture* rr12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_011.png" );
	Texture* rr13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_012.png" );
	Texture* rr14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_013.png" );
	Texture* rr15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_014.png" );
	Texture* rr16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_015.png" );
	Texture* rr17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_016.png" );
	Texture* rr18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_017.png" );
	Texture* rr19 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_018.png" );
	Texture* rr20 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_019.png" );
	Texture* rr21 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_020.png" );
	Texture* rr22 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_021.png" );
	Texture* rr23 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_022.png" );
	Texture* rr24 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demon1/PNG/PNG Sequences/Walking/0_Magician_Walking_023.png" );
	

	enemyUpgrade1MeleeRunTextures.push_back( rr1 );
	enemyUpgrade1MeleeRunTextures.push_back( rr2 );
	enemyUpgrade1MeleeRunTextures.push_back( rr3 );
	enemyUpgrade1MeleeRunTextures.push_back( rr4 );
	enemyUpgrade1MeleeRunTextures.push_back( rr5 );
	enemyUpgrade1MeleeRunTextures.push_back( rr6 );
	enemyUpgrade1MeleeRunTextures.push_back( rr7 );
	enemyUpgrade1MeleeRunTextures.push_back( rr8 );
	enemyUpgrade1MeleeRunTextures.push_back( rr9 );
	enemyUpgrade1MeleeRunTextures.push_back( rr10 );
	enemyUpgrade1MeleeRunTextures.push_back( rr11 );
	enemyUpgrade1MeleeRunTextures.push_back( rr12 );
	enemyUpgrade1MeleeRunTextures.push_back( rr13 );
	enemyUpgrade1MeleeRunTextures.push_back( rr14 );
	enemyUpgrade1MeleeRunTextures.push_back( rr15 );
	enemyUpgrade1MeleeRunTextures.push_back( rr16 );
	enemyUpgrade1MeleeRunTextures.push_back( rr17 );
	enemyUpgrade1MeleeRunTextures.push_back( rr18 );
	enemyUpgrade1MeleeRunTextures.push_back( rr19 );
	enemyUpgrade1MeleeRunTextures.push_back( rr20 );
	enemyUpgrade1MeleeRunTextures.push_back( rr21 );
	enemyUpgrade1MeleeRunTextures.push_back( rr22 );
	enemyUpgrade1MeleeRunTextures.push_back( rr23 );
	enemyUpgrade1MeleeRunTextures.push_back( rr24 );

	m_EnemyUpgrade1MeleeRun = new SpriteAnimDefTex( 0 , enemyUpgrade1MeleeRunTextures.size() - 1 , 1.f );
	m_EnemyUpgrade1MeleeRun->m_animations = enemyUpgrade1MeleeRunTextures;

	std::vector<Texture*> enemyUpdgrade2MeleeAttackTextures;
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_007.png" );
	Texture* a9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_008.png" );
	Texture* a10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_009.png" );
	Texture* a11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_010.png" );
	Texture* a12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Slashing/0_Golem_Slashing_011.png" );

	enemyUpdgrade2MeleeAttackTextures.push_back( a1 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a2 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a3 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a4 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a5 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a6 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a7 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a8 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a9 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a10 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a11 );
	enemyUpdgrade2MeleeAttackTextures.push_back( a12 );

	m_EnemyUpgrade2MeleeAttack = new SpriteAnimDefTex( 0 , enemyUpdgrade2MeleeAttackTextures.size() - 1 , 1.f );
	m_EnemyUpgrade2MeleeAttack->m_animations = enemyUpdgrade2MeleeAttackTextures;

	std::vector<Texture*> enemyUpgrade2MeleeRunTextures;

	Texture* aa1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_000.png" );
	Texture* aa2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_001.png" );
	Texture* aa3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_002.png" );
	Texture* aa4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_003.png" );
	Texture* aa5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_004.png" );
	Texture* aa6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_005.png" );
	Texture* aa7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_006.png" );
	Texture* aa8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_007.png" );
	Texture* aa9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_008.png" );
	Texture* aa10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_009.png" );
	Texture* aa11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_010.png" );
	Texture* aa12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_011.png" );
	Texture* aa13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_012.png" );
	Texture* aa14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_013.png" );
	Texture* aa15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_014.png" );
	Texture* aa16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_015.png" );
	Texture* aa17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_016.png" );
	Texture* aa18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_017.png" );
	Texture* aa19 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_018.png" );
	Texture* aa20 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_019.png" );
	Texture* aa21 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_020.png" );
	Texture* aa22 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_021.png" );
	Texture* aa23 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_022.png" );
	Texture* aa24 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Golem_3/PNG/PNG Sequences/Walking/0_Golem_Walking_023.png" );
	
	enemyUpgrade2MeleeRunTextures.push_back( aa1 );
	enemyUpgrade2MeleeRunTextures.push_back( aa2 );
	enemyUpgrade2MeleeRunTextures.push_back( aa3 );
	enemyUpgrade2MeleeRunTextures.push_back( aa4 );
	enemyUpgrade2MeleeRunTextures.push_back( aa5 );
	enemyUpgrade2MeleeRunTextures.push_back( aa6 );
	enemyUpgrade2MeleeRunTextures.push_back( aa7 );
	enemyUpgrade2MeleeRunTextures.push_back( aa8 );
	enemyUpgrade2MeleeRunTextures.push_back( aa9 );
	enemyUpgrade2MeleeRunTextures.push_back( aa10 );
	enemyUpgrade2MeleeRunTextures.push_back( aa11 );
	enemyUpgrade2MeleeRunTextures.push_back( aa12 );
	enemyUpgrade2MeleeRunTextures.push_back( aa13 );
	enemyUpgrade2MeleeRunTextures.push_back( aa14 );
	enemyUpgrade2MeleeRunTextures.push_back( aa15 );
	enemyUpgrade2MeleeRunTextures.push_back( aa16 );
	enemyUpgrade2MeleeRunTextures.push_back( aa17 );
	enemyUpgrade2MeleeRunTextures.push_back( aa18 );
	enemyUpgrade2MeleeRunTextures.push_back( aa19 );
	enemyUpgrade2MeleeRunTextures.push_back( aa20 );
	enemyUpgrade2MeleeRunTextures.push_back( aa21 );
	enemyUpgrade2MeleeRunTextures.push_back( aa22 );
	enemyUpgrade2MeleeRunTextures.push_back( aa23 );
	enemyUpgrade2MeleeRunTextures.push_back( aa24 );

	m_EnemyUpgrade2MeleeRun = new SpriteAnimDefTex( 0 , enemyUpgrade2MeleeRunTextures.size() - 1 , 1.f );
	m_EnemyUpgrade2MeleeRun->m_animations = enemyUpgrade2MeleeRunTextures;

	std::vector<Texture*> enemyUpdgrade3MeleeAttackTextures;

	Texture* x1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_000.png" );
	Texture* x2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_001.png" );
	Texture* x3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_002.png" );
	Texture* x4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_003.png" );
	Texture* x5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_004.png" );
	Texture* x6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_005.png" );
	Texture* x7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_006.png" );
	Texture* x8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_007.png" );
	Texture* x9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_008.png" );
	Texture* x10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_009.png" );
	Texture* x11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_010.png" );
	Texture* x12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Slashing/0_Elementals_Slashing_011.png" );
	
	enemyUpdgrade3MeleeAttackTextures.push_back( x1 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x2 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x3 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x4 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x5 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x6 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x7 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x8 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x9 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x10 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x11 );
	enemyUpdgrade3MeleeAttackTextures.push_back( x12 );

	m_EnemyUpgrade3MeleeAttack = new SpriteAnimDefTex( 0 , enemyUpdgrade3MeleeAttackTextures.size() - 1 , 1.f );
	m_EnemyUpgrade3MeleeAttack->m_animations = enemyUpdgrade3MeleeAttackTextures;

	std::vector<Texture*>  enemyUpdgrade3MeleeRunTextures;

	Texture* xx1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_000.png" );
	Texture* xx2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_001.png" );
	Texture* xx3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_002.png" );
	Texture* xx4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_003.png" );
	Texture* xx5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_004.png" );
	Texture* xx6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_005.png" );
	Texture* xx7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_006.png" );
	Texture* xx8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_007.png" );
	Texture* xx9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_008.png" );
	Texture* xx10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_009.png" );
	Texture* xx11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_010.png" );
	Texture* xx12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_011.png" );
	Texture* xx13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_012.png" );
	Texture* xx14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_013.png" );
	Texture* xx15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_014.png" );
	Texture* xx16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_015.png" );
	Texture* xx17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_016.png" );
	Texture* xx18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_017.png" );
	Texture* xx19 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_018.png" );
	Texture* xx20 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_019.png" );
	Texture* xx21 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_020.png" );
	Texture* xx22 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_021.png" );
	Texture* xx23 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_022.png" );
	Texture* xx24 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_2/PNG/PNG Sequences/Walking/0_Elementals_Walking_023.png" );


	enemyUpdgrade3MeleeRunTextures.push_back( xx1 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx2 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx3 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx4 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx5 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx6 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx7 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx8 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx9 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx10 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx11 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx12 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx13 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx14 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx15 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx16 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx17 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx18 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx19 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx20 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx21 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx22 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx23 );
	enemyUpdgrade3MeleeRunTextures.push_back( xx24 );


	m_EnemyUpgrade3MeleeRun = new SpriteAnimDefTex( 0 , enemyUpdgrade3MeleeRunTextures.size() - 1 , 1.f );
	m_EnemyUpgrade3MeleeRun->m_animations = enemyUpdgrade3MeleeRunTextures;
}

void Game::LoadPlayerRangeAnimations()
{
	std::vector<Texture*> playerUpdgrade0RangeAttackTextures;

	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_000.png" );
	Texture* t2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_001.png" );
	Texture* t3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_002.png" );
	Texture* t4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_003.png" );
	Texture* t5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_004.png" );
	Texture* t6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_005.png" );
	Texture* t7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_006.png" );
	Texture* t8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_007.png" );
	Texture* t9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_008.png" );
	Texture* t10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_009.png" );
	Texture* t11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_010.png" );
	Texture* t12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Throwing/Throwing_011.png" );


	

	playerUpdgrade0RangeAttackTextures.push_back( t1 );
	playerUpdgrade0RangeAttackTextures.push_back( t2 );
	playerUpdgrade0RangeAttackTextures.push_back( t3 );
	playerUpdgrade0RangeAttackTextures.push_back( t4 );
	playerUpdgrade0RangeAttackTextures.push_back( t5 );
	playerUpdgrade0RangeAttackTextures.push_back( t6 );
	playerUpdgrade0RangeAttackTextures.push_back( t7 );
	playerUpdgrade0RangeAttackTextures.push_back( t8 );
	playerUpdgrade0RangeAttackTextures.push_back( t9 );
	playerUpdgrade0RangeAttackTextures.push_back( t10 );
	playerUpdgrade0RangeAttackTextures.push_back( t11 );
	playerUpdgrade0RangeAttackTextures.push_back( t12 );


	m_PlayerUpgade0RangeAttack = new SpriteAnimDefTex( 0 , playerUpdgrade0RangeAttackTextures.size() - 1 , 1.f );
	m_PlayerUpgade0RangeAttack->m_animations = playerUpdgrade0RangeAttackTextures;

	std::vector<Texture*> playerUpgrade0RangeRunTextures;

	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_007.png" );
	Texture* a9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_008.png" );
	Texture* a10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_009.png" );
	Texture* a11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_010.png" );
	Texture* a12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Running/Running_011.png" );

	playerUpgrade0RangeRunTextures.push_back( a1 );
	playerUpgrade0RangeRunTextures.push_back( a2 );
	playerUpgrade0RangeRunTextures.push_back( a3 );
	playerUpgrade0RangeRunTextures.push_back( a4 );
	playerUpgrade0RangeRunTextures.push_back( a5 );
	playerUpgrade0RangeRunTextures.push_back( a6 );
	playerUpgrade0RangeRunTextures.push_back( a7 );
	playerUpgrade0RangeRunTextures.push_back( a8 );
	playerUpgrade0RangeRunTextures.push_back( a9 );
	playerUpgrade0RangeRunTextures.push_back( a10 );
	playerUpgrade0RangeRunTextures.push_back( a11 );
	playerUpgrade0RangeRunTextures.push_back( a12 );
	playerUpgrade0RangeRunTextures.push_back( a1 );
	
	m_PlayerUpgrade0RangeRun = new SpriteAnimDefTex( 0 , playerUpgrade0RangeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade0RangeRun->m_animations = playerUpgrade0RangeRunTextures;


	std::vector<Texture*> playerUpdgrade1RangeAttackTextures;

	Texture* x1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_000.png" );
	Texture* x2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_001.png" );
	Texture* x3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_002.png" );
	Texture* x4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_003.png" );
	Texture* x5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_004.png" );
	Texture* x6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_005.png" );
	Texture* x7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_006.png" );
	Texture* x8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_007.png" );
	Texture* x9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_008.png" );
	Texture* x10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_009.png" );
	Texture* x11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_010.png" );
	Texture* x12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Throwing/0_Elementals_Throwing_011.png" );
	
	
	playerUpdgrade1RangeAttackTextures.push_back( x1 );
	playerUpdgrade1RangeAttackTextures.push_back( x2 );
	playerUpdgrade1RangeAttackTextures.push_back( x3 );
	playerUpdgrade1RangeAttackTextures.push_back( x4 );
	playerUpdgrade1RangeAttackTextures.push_back( x5 );
	playerUpdgrade1RangeAttackTextures.push_back( x6 );
	playerUpdgrade1RangeAttackTextures.push_back( x7 );
	playerUpdgrade1RangeAttackTextures.push_back( x8 );
	playerUpdgrade1RangeAttackTextures.push_back( x9 );
	playerUpdgrade1RangeAttackTextures.push_back( x10 );
	playerUpdgrade1RangeAttackTextures.push_back( x11 );
	playerUpdgrade1RangeAttackTextures.push_back( x12 );

	m_PlayerUpgade1RangeAttack = new SpriteAnimDefTex( 0 , playerUpdgrade1RangeAttackTextures.size() - 1 , 1.f );
	m_PlayerUpgade1RangeAttack->m_animations = playerUpdgrade1RangeAttackTextures;

	std::vector<Texture*> playerUpgrade1RangeRunTextures;

	Texture* y1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_000.png" );
	Texture* y2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_001.png" );
	Texture* y3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_002.png" );
	Texture* y4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_003.png" );
	Texture* y5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_004.png" );
	Texture* y6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_005.png" );
	Texture* y7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_006.png" );
	Texture* y8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_007.png" );
	Texture* y9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_008.png" );
	Texture* y10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_009.png" );
	Texture* y11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_010.png" );
	Texture* y12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Running/0_Elementals_Running_011.png" );

	playerUpgrade1RangeRunTextures.push_back( y1 );
	playerUpgrade1RangeRunTextures.push_back( y2 );
	playerUpgrade1RangeRunTextures.push_back( y3 );
	playerUpgrade1RangeRunTextures.push_back( y4 );
	playerUpgrade1RangeRunTextures.push_back( y5 );
	playerUpgrade1RangeRunTextures.push_back( y6 );
	playerUpgrade1RangeRunTextures.push_back( y7 );
	playerUpgrade1RangeRunTextures.push_back( y8 );
	playerUpgrade1RangeRunTextures.push_back( y9 );
	playerUpgrade1RangeRunTextures.push_back( y10 );
	playerUpgrade1RangeRunTextures.push_back( y11 );
	playerUpgrade1RangeRunTextures.push_back( y12 );

	m_PlayerUpgrade1RangeRun = new SpriteAnimDefTex( 0 , playerUpgrade1RangeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade1RangeRun->m_animations = playerUpgrade1RangeRunTextures;


	std::vector<Texture*> playerUpdgrade2RangeAttackTextures;
	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_000.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_001.png" );
	Texture* b3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_002.png" );
	Texture* b4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_003.png" );
	Texture* b5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_004.png" );
	Texture* b6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_005.png" );
	Texture* b7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_006.png" );
	Texture* b8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_007.png" );
	Texture* b9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_008.png" );
	Texture* b10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_009.png" );
	Texture* b11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_010.png" );
	Texture* b12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Throwing/0_Magician_Throwing_011.png" );

	playerUpdgrade2RangeAttackTextures.push_back( b1 );
	playerUpdgrade2RangeAttackTextures.push_back( b2 );
	playerUpdgrade2RangeAttackTextures.push_back( b3 );
	playerUpdgrade2RangeAttackTextures.push_back( b4 );
	playerUpdgrade2RangeAttackTextures.push_back( b5 );
	playerUpdgrade2RangeAttackTextures.push_back( b6 );
	playerUpdgrade2RangeAttackTextures.push_back( b7 );
	playerUpdgrade2RangeAttackTextures.push_back( b8 );
	playerUpdgrade2RangeAttackTextures.push_back( b9 );
	playerUpdgrade2RangeAttackTextures.push_back( b10 );
	playerUpdgrade2RangeAttackTextures.push_back( b11 );
	playerUpdgrade2RangeAttackTextures.push_back( b12 );
	
	m_PlayerUpgrade2RangeAttack = new SpriteAnimDefTex( 0 , playerUpdgrade2RangeAttackTextures.size() - 1 , 1.f );
	m_PlayerUpgrade2RangeAttack->m_animations = playerUpdgrade2RangeAttackTextures;


	std::vector<Texture*> playerUpgrade2RangeRunTextures;
	Texture* c1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_000.png" );
	Texture* c2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_001.png" );
	Texture* c3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_002.png" );
	Texture* c4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_003.png" );
	Texture* c5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_004.png" );
	Texture* c6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_005.png" );
	Texture* c7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_006.png" );
	Texture* c8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_007.png" );
	Texture* c9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_008.png" );
	Texture* c10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_009.png" );
	Texture* c11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_010.png" );
	Texture* c12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Running/0_Magician_Running_011.png" );

	
	playerUpgrade2RangeRunTextures.push_back( c1 );
	playerUpgrade2RangeRunTextures.push_back( c2 );
	playerUpgrade2RangeRunTextures.push_back( c3 );
	playerUpgrade2RangeRunTextures.push_back( c4 );
	playerUpgrade2RangeRunTextures.push_back( c5 );
	playerUpgrade2RangeRunTextures.push_back( c6 );
	playerUpgrade2RangeRunTextures.push_back( c7 );
	playerUpgrade2RangeRunTextures.push_back( c8 );
	playerUpgrade2RangeRunTextures.push_back( c9 );
	playerUpgrade2RangeRunTextures.push_back( c10 );
	playerUpgrade2RangeRunTextures.push_back( c11 );
	playerUpgrade2RangeRunTextures.push_back( c12 );
	
	m_PlayerUpgrade2RangeRun = new SpriteAnimDefTex( 0 , playerUpgrade2RangeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade2RangeRun->m_animations = playerUpgrade2RangeRunTextures;


	std::vector<Texture*> playerUpdgrade3RangeAttackTextures;
	Texture* m1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_000.png" );
	Texture* m2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_001.png" );
	Texture* m3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_002.png" );
	Texture* m4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_003.png" );
	Texture* m5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_004.png" );
	Texture* m6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_005.png" );
	Texture* m7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_006.png" );
	Texture* m8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_007.png" );
	Texture* m9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_008.png" );
	Texture* m10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_009.png" );
	Texture* m11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_010.png" );
	Texture* m12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Throwing/Throwing_011.png" );

	playerUpdgrade3RangeAttackTextures.push_back( m1 );
	playerUpdgrade3RangeAttackTextures.push_back( m2 );
	playerUpdgrade3RangeAttackTextures.push_back( m3 );
	playerUpdgrade3RangeAttackTextures.push_back( m4 );
	playerUpdgrade3RangeAttackTextures.push_back( m5 );
	playerUpdgrade3RangeAttackTextures.push_back( m6 );
	playerUpdgrade3RangeAttackTextures.push_back( m7 );
	playerUpdgrade3RangeAttackTextures.push_back( m8 );
	playerUpdgrade3RangeAttackTextures.push_back( m9 );
	playerUpdgrade3RangeAttackTextures.push_back( m10 );
	playerUpdgrade3RangeAttackTextures.push_back( m11 );
	playerUpdgrade3RangeAttackTextures.push_back( m12 );
	
	m_PlayerUpgrade3RangeAttack = new SpriteAnimDefTex( 0 , playerUpdgrade3RangeAttackTextures.size() - 1 , 1.f );
	m_PlayerUpgrade3RangeAttack->m_animations = playerUpdgrade3RangeAttackTextures;

	
	std::vector<Texture*> playerUpgrade3RangeRunTextures;

	Texture* n1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_000.png" );
	Texture* n2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_001.png" );
	Texture* n3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_002.png" );
	Texture* n4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_003.png" );
	Texture* n5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_004.png" );
	Texture* n6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_005.png" );
	Texture* n7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_006.png" );
	Texture* n8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_007.png" );
	Texture* n9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_008.png" );
	Texture* n10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_009.png" );
	Texture* n11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_010.png" );
	Texture* n12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Running/Running_011.png" );

	
	playerUpgrade3RangeRunTextures.push_back( n1 );
	playerUpgrade3RangeRunTextures.push_back( n2 );
	playerUpgrade3RangeRunTextures.push_back( n3 );
	playerUpgrade3RangeRunTextures.push_back( n4 );
	playerUpgrade3RangeRunTextures.push_back( n5 );
	playerUpgrade3RangeRunTextures.push_back( n6 );
	playerUpgrade3RangeRunTextures.push_back( n7 );
	playerUpgrade3RangeRunTextures.push_back( n8 );
	playerUpgrade3RangeRunTextures.push_back( n9 );
	playerUpgrade3RangeRunTextures.push_back( n10 );
	playerUpgrade3RangeRunTextures.push_back( n11 );
	playerUpgrade3RangeRunTextures.push_back( n12 );

	m_PlayerUpgrade3RangeRun = new SpriteAnimDefTex( 0 , playerUpgrade3RangeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade3RangeRun->m_animations = playerUpgrade3RangeRunTextures;

}

void Game::LoadEnemyRangeAnimations()
{
	std::vector<Texture*> enemyUpdgrade0RangeAttackTextures;

	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_000.png" );
	Texture* t2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_001.png" );
	Texture* t3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_002.png" );
	Texture* t4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_003.png" );
	Texture* t5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_004.png" );
	Texture* t6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_005.png" );
	Texture* t7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_006.png" );
	Texture* t8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_007.png" );
	Texture* t9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_008.png" );
	Texture* t10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_009.png" );
	Texture* t11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_010.png" );
	Texture* t12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Throwing/0_Golem_Throwing_011.png" );

	enemyUpdgrade0RangeAttackTextures.push_back( t1 );
	enemyUpdgrade0RangeAttackTextures.push_back( t2 );
	enemyUpdgrade0RangeAttackTextures.push_back( t3 );
	enemyUpdgrade0RangeAttackTextures.push_back( t4 );
	enemyUpdgrade0RangeAttackTextures.push_back( t5 );
	enemyUpdgrade0RangeAttackTextures.push_back( t6 );
	enemyUpdgrade0RangeAttackTextures.push_back( t7 );
	enemyUpdgrade0RangeAttackTextures.push_back( t8 );
	enemyUpdgrade0RangeAttackTextures.push_back( t9 );
	enemyUpdgrade0RangeAttackTextures.push_back( t10 );
	enemyUpdgrade0RangeAttackTextures.push_back( t11 );
	enemyUpdgrade0RangeAttackTextures.push_back( t12 );

	m_EnemyUpgrade0RangeAttack = new SpriteAnimDefTex( 0 , enemyUpdgrade0RangeAttackTextures.size() - 1 , 1.f );
	m_EnemyUpgrade0RangeAttack->m_animations = enemyUpdgrade0RangeAttackTextures;

	std::vector<Texture*> enemyUpdgrade0RangeRunTextures;

	Texture* r1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_000.png" );
	Texture* r2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_001.png" );
	Texture* r3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_002.png" );
	Texture* r4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_003.png" );
	Texture* r5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_004.png" );
	Texture* r6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_005.png" );
	Texture* r7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_006.png" );
	Texture* r8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_007.png" );
	Texture* r9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_008.png" );
	Texture* r10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_009.png" );
	Texture* r11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_010.png" );
	Texture* r12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/PNG Sequences/Running/0_Golem_Running_011.png" );

	enemyUpdgrade0RangeRunTextures.push_back( r1 );
	enemyUpdgrade0RangeRunTextures.push_back( r2 );
	enemyUpdgrade0RangeRunTextures.push_back( r3 );
	enemyUpdgrade0RangeRunTextures.push_back( r4 );
	enemyUpdgrade0RangeRunTextures.push_back( r5 );
	enemyUpdgrade0RangeRunTextures.push_back( r6 );
	enemyUpdgrade0RangeRunTextures.push_back( r7 );
	enemyUpdgrade0RangeRunTextures.push_back( r8 );
	enemyUpdgrade0RangeRunTextures.push_back( r9 );
	enemyUpdgrade0RangeRunTextures.push_back( r10 );
	enemyUpdgrade0RangeRunTextures.push_back( r11 );
	enemyUpdgrade0RangeRunTextures.push_back( r12 );

	m_EnemyUpgrade0RangeRun = new SpriteAnimDefTex( 0 , enemyUpdgrade0RangeRunTextures.size() - 1 , 1.f );
	m_EnemyUpgrade0RangeRun->m_animations = enemyUpdgrade0RangeRunTextures;

	std::vector<Texture*> enemyUpdgrade1RangeAttackTextures;

	Texture* x1		= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_000.png" );
	Texture* x2		= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_001.png" );
	Texture* x3		= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_002.png" );
	Texture* x4		= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_003.png" );
	Texture* x5		= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_004.png" );
	Texture* x6		= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_005.png" );
	Texture* x7		= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_006.png" );
	Texture* x8		= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_007.png" );
	Texture* x9		= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_008.png" );
	Texture* x10	= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_009.png" );
	Texture* x11	= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_010.png" );
	Texture* x12	= g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Throwing/0_Demons_of_Darkness_Throwing_011.png" );
	
	enemyUpdgrade1RangeAttackTextures.push_back( x1 );
	enemyUpdgrade1RangeAttackTextures.push_back( x2 );
	enemyUpdgrade1RangeAttackTextures.push_back( x3 );
	enemyUpdgrade1RangeAttackTextures.push_back( x4 );
	enemyUpdgrade1RangeAttackTextures.push_back( x5 );
	enemyUpdgrade1RangeAttackTextures.push_back( x6 );
	enemyUpdgrade1RangeAttackTextures.push_back( x7 );
	enemyUpdgrade1RangeAttackTextures.push_back( x8 );
	enemyUpdgrade1RangeAttackTextures.push_back( x9 );
	enemyUpdgrade1RangeAttackTextures.push_back( x10 );
	enemyUpdgrade1RangeAttackTextures.push_back( x11 );
	enemyUpdgrade1RangeAttackTextures.push_back( x12 );

	m_EnemyUpgrade1RangeAttack = new SpriteAnimDefTex( 0 , enemyUpdgrade1RangeAttackTextures.size() - 1 , 1.f );
	m_EnemyUpgrade1RangeAttack->m_animations = enemyUpdgrade1RangeAttackTextures;

	std::vector<Texture*> enemyUpdgrade1RangeRunTextures;

	Texture* y1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_000.png" );
	Texture* y2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_001.png" );
	Texture* y3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_002.png" );
	Texture* y4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_003.png" );
	Texture* y5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_004.png" );
	Texture* y6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_005.png" );
	Texture* y7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_006.png" );
	Texture* y8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_007.png" );
	Texture* y9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_008.png" );
	Texture* y10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_009.png" );
	Texture* y11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_010.png" );
	Texture* y12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/PNG Sequences/Running/0_Demons_of_Darkness_Running_011.png" );


	enemyUpdgrade1RangeRunTextures.push_back( y1 );
	enemyUpdgrade1RangeRunTextures.push_back( y2 );
	enemyUpdgrade1RangeRunTextures.push_back( y3 );
	enemyUpdgrade1RangeRunTextures.push_back( y4 );
	enemyUpdgrade1RangeRunTextures.push_back( y5 );
	enemyUpdgrade1RangeRunTextures.push_back( y6 );
	enemyUpdgrade1RangeRunTextures.push_back( y7 );
	enemyUpdgrade1RangeRunTextures.push_back( y8 );
	enemyUpdgrade1RangeRunTextures.push_back( y9 );
	enemyUpdgrade1RangeRunTextures.push_back( y10 );
	enemyUpdgrade1RangeRunTextures.push_back( y11 );
	enemyUpdgrade1RangeRunTextures.push_back( y12 );

	m_EnemyUpgrade1RangeRun = new SpriteAnimDefTex( 0 , enemyUpdgrade1RangeRunTextures.size() - 1 , 1.f );
	m_EnemyUpgrade1RangeRun->m_animations = enemyUpdgrade1RangeRunTextures;


	std::vector<Texture*> enemyUpdgrade2RangeAttackTextures;

	Texture* s1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_000.png" );
	Texture* s2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_001.png" );
	Texture* s3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_002.png" );
	Texture* s4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_003.png" );
	Texture* s5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_004.png" );
	Texture* s6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_005.png" );
	Texture* s7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_006.png" );
	Texture* s8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_007.png" );
	Texture* s9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_008.png" );
	Texture* s10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_009.png" );
	Texture* s11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_010.png" );
	Texture* s12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Throwing/0_Archer_Throwing_011.png" );

	enemyUpdgrade2RangeAttackTextures.push_back( s1 );
	enemyUpdgrade2RangeAttackTextures.push_back( s2 );
	enemyUpdgrade2RangeAttackTextures.push_back( s3 );
	enemyUpdgrade2RangeAttackTextures.push_back( s4 );
	enemyUpdgrade2RangeAttackTextures.push_back( s5 );
	enemyUpdgrade2RangeAttackTextures.push_back( s6 );
	enemyUpdgrade2RangeAttackTextures.push_back( s7 );
	enemyUpdgrade2RangeAttackTextures.push_back( s8 );
	enemyUpdgrade2RangeAttackTextures.push_back( s9 );
	enemyUpdgrade2RangeAttackTextures.push_back( s10 );
	enemyUpdgrade2RangeAttackTextures.push_back( s11 );
	enemyUpdgrade2RangeAttackTextures.push_back( s12 );

	m_EnemyUpgrade2RangeAttack = new SpriteAnimDefTex( 0 , enemyUpdgrade2RangeAttackTextures.size() - 1 , 1.f );
	m_EnemyUpgrade2RangeAttack->m_animations = enemyUpdgrade2RangeAttackTextures;

	std::vector<Texture*> enemyUpdgrade2RangeRunTextures;

	Texture* c1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_000.png" );
	Texture* c2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_001.png" );
	Texture* c3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_002.png" );
	Texture* c4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_003.png" );
	Texture* c5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_004.png" );
	Texture* c6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_005.png" );
	Texture* c7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_006.png" );
	Texture* c8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_007.png" );
	Texture* c9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_008.png" );
	Texture* c10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_009.png" );
	Texture* c11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_010.png" );
	Texture* c12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/PNG Sequences/Running/0_Archer_Running_011.png" );

	enemyUpdgrade2RangeRunTextures.push_back( c1 );
	enemyUpdgrade2RangeRunTextures.push_back( c2 );
	enemyUpdgrade2RangeRunTextures.push_back( c3 );
	enemyUpdgrade2RangeRunTextures.push_back( c4 );
	enemyUpdgrade2RangeRunTextures.push_back( c5 );
	enemyUpdgrade2RangeRunTextures.push_back( c6 );
	enemyUpdgrade2RangeRunTextures.push_back( c7 );
	enemyUpdgrade2RangeRunTextures.push_back( c8 );
	enemyUpdgrade2RangeRunTextures.push_back( c9 );
	enemyUpdgrade2RangeRunTextures.push_back( c10 );
	enemyUpdgrade2RangeRunTextures.push_back( c11 );
	enemyUpdgrade2RangeRunTextures.push_back( c12 );

	m_EnemyUpgrade2RangeRun = new SpriteAnimDefTex( 0 , enemyUpdgrade2RangeRunTextures.size() - 1 , 1.f );
	m_EnemyUpgrade2RangeRun->m_animations = enemyUpdgrade2RangeRunTextures;


	std::vector<Texture*> enemyUpdgrade3RangeAttackTextures;

	Texture* z1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_000.png" );
	Texture* z2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_001.png" );
	Texture* z3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_002.png" );
	Texture* z4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_003.png" );
	Texture* z5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_004.png" );
	Texture* z6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_005.png" );
	Texture* z7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_006.png" );
	Texture* z8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_007.png" );
	Texture* z9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_008.png" );
	Texture* z10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_009.png" );
	Texture* z11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_010.png" );
	Texture* z12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Throwing/0_Archer_Throwing_011.png" );

	enemyUpdgrade3RangeAttackTextures.push_back( z1 );
	enemyUpdgrade3RangeAttackTextures.push_back( z2 );
	enemyUpdgrade3RangeAttackTextures.push_back( z3 );
	enemyUpdgrade3RangeAttackTextures.push_back( z4 );
	enemyUpdgrade3RangeAttackTextures.push_back( z5 );
	enemyUpdgrade3RangeAttackTextures.push_back( z6 );
	enemyUpdgrade3RangeAttackTextures.push_back( z7 );
	enemyUpdgrade3RangeAttackTextures.push_back( z8 );
	enemyUpdgrade3RangeAttackTextures.push_back( z9 );
	enemyUpdgrade3RangeAttackTextures.push_back( z10 );
	enemyUpdgrade3RangeAttackTextures.push_back( z11 );
	enemyUpdgrade3RangeAttackTextures.push_back( z12 );
	
	m_EnemyUpgrade3RangeAttack = new SpriteAnimDefTex( 0 , enemyUpdgrade3RangeAttackTextures.size() - 1 , 1.f );
	m_EnemyUpgrade3RangeAttack->m_animations = enemyUpdgrade3RangeAttackTextures;
	
	std::vector<Texture*> enemyUpdgrade3RangeRunTextures;

	Texture* d1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_000.png" );
	Texture* d2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_001.png" );
	Texture* d3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_002.png" );
	Texture* d4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_003.png" );
	Texture* d5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_004.png" );
	Texture* d6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_005.png" );
	Texture* d7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_006.png" );
	Texture* d8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_007.png" );
	Texture* d9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_008.png" );
	Texture* d10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_009.png" );
	Texture* d11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_010.png" );
	Texture* d12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/PNG Sequences/Running/0_Archer_Running_011.png" );

	enemyUpdgrade3RangeRunTextures.push_back( d1 );
	enemyUpdgrade3RangeRunTextures.push_back( d2 );
	enemyUpdgrade3RangeRunTextures.push_back( d3 );
	enemyUpdgrade3RangeRunTextures.push_back( d4 );
	enemyUpdgrade3RangeRunTextures.push_back( d5 );
	enemyUpdgrade3RangeRunTextures.push_back( d6 );
	enemyUpdgrade3RangeRunTextures.push_back( d7 );
	enemyUpdgrade3RangeRunTextures.push_back( d8 );
	enemyUpdgrade3RangeRunTextures.push_back( d9 );
	enemyUpdgrade3RangeRunTextures.push_back( d10 );
	enemyUpdgrade3RangeRunTextures.push_back( d11 );
	enemyUpdgrade3RangeRunTextures.push_back( d12 );
	
	m_EnemyUpgrade3RangeRun = new SpriteAnimDefTex( 0 , enemyUpdgrade3RangeRunTextures.size() - 1 , 1.f );
	m_EnemyUpgrade3RangeRun->m_animations = enemyUpdgrade3RangeRunTextures;

}

void Game::LoadPlayerBulletTextures()
{
	m_RangerHero1PlayerBulletTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/Vector Parts/Head.png" );
	m_RangerHero0PlayerBulletTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/Vector Parts/Head.png" );
	m_RangerHero2PlayerBulletTex = g_theRenderer->GetOrCreateTextureFromFile("Data/GameAssets/CharactersNew/Magician_3/PNG/Vector Parts/Head.png");
	m_RangerHero3PlayerBulletTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/Vector Parts/Head.png" );

	m_RangerHero0EnemyBulletTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Golem_1/PNG/Vector Parts/Head.png" );
	m_RangerHero1EnemyBulletTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Demons_of_Darkness_1/PNG/Vector Parts/Head.png" );
	m_RangerHero2EnemyBulletTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_1/PNG/Vector Parts/Head.png" );
	m_RangerHero3EnemyBulletTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/EnemyArchers1/Archer_3/PNG/Vector Parts/Head.png" );

}

void Game::LoadUncontrolledHeroTextures()
{
	std::vector<Texture*> player0RunTex;

	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_007.png" );
	Texture* a9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_008.png" );
	Texture* a10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_009.png" );
	Texture* a11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_010.png" );
	Texture* a12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Running/Running_011.png" );
	
	player0RunTex.push_back( a1 );
	player0RunTex.push_back( a2 );
	player0RunTex.push_back( a3 );
	player0RunTex.push_back( a4 );
	player0RunTex.push_back( a5 );
	player0RunTex.push_back( a6 );
	player0RunTex.push_back( a7 );
	player0RunTex.push_back( a8 );
	player0RunTex.push_back( a9 );
	player0RunTex.push_back( a10 );
	player0RunTex.push_back( a11 );
	player0RunTex.push_back( a12 );
	
	m_UPlayer0Run = new SpriteAnimDefTex( 0 , player0RunTex.size() - 1 , 1.f );
	m_UPlayer0Run->m_animations = player0RunTex;

	std::vector<Texture*> player0AttackTex;

	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_000.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_001.png" );
	Texture* b3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_002.png" );
	Texture* b4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_003.png" );
	Texture* b5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_004.png" );
	Texture* b6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_005.png" );
	Texture* b7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_006.png" );
	Texture* b8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_007.png" );
	Texture* b9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_008.png" );
	Texture* b10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_009.png" );
	Texture* b11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_010.png" );
	Texture* b12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Spartan Knight with Spear/PNG/PNG Sequences/Attacking/Attacking_011.png" );
	
	player0AttackTex.push_back( b1 );
	player0AttackTex.push_back( b2 );
	player0AttackTex.push_back( b3 );
	player0AttackTex.push_back( b4 );
	player0AttackTex.push_back( b5 );
	player0AttackTex.push_back( b6 );
	player0AttackTex.push_back( b7 );
	player0AttackTex.push_back( b8 );
	player0AttackTex.push_back( b9 );
	player0AttackTex.push_back( b10 );
	player0AttackTex.push_back( b11 );
	player0AttackTex.push_back( b12 );

	m_UPlayer0Attack = new SpriteAnimDefTex( 0 , player0AttackTex.size() - 1 , 1.f );
	m_UPlayer0Attack->m_animations = player0AttackTex;

	std::vector<Texture*> player1RunTex;

	Texture* c1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_000.png" );
	Texture* c2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_001.png" );
	Texture* c3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_002.png" );
	Texture* c4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_003.png" );
	Texture* c5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_004.png" );
	Texture* c6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_005.png" );
	Texture* c7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_006.png" );
	Texture* c8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_007.png" );
	Texture* c9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_008.png" );
	Texture* c10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_009.png" );
	Texture* c11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_010.png" );
	Texture* c12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Running/Running_011.png" );

	player1RunTex.push_back( c1 );
	player1RunTex.push_back( c2 );
	player1RunTex.push_back( c3 );
	player1RunTex.push_back( c4 );
	player1RunTex.push_back( c5 );
	player1RunTex.push_back( c6 );
	player1RunTex.push_back( c7 );
	player1RunTex.push_back( c8 );
	player1RunTex.push_back( c9 );
	player1RunTex.push_back( c10 );
	player1RunTex.push_back( c11 );
	player1RunTex.push_back( c12 );

	m_UPlayer1Run = new SpriteAnimDefTex( 0 , player1RunTex.size() - 1 , 1.f );
	m_UPlayer1Run->m_animations = player1RunTex;

	std::vector<Texture*> player1AttackTex;

	Texture* d1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_000.png" );
	Texture* d2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_001.png" );
	Texture* d3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_002.png" );
	Texture* d4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_003.png" );
	Texture* d5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_004.png" );
	Texture* d6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_005.png" );
	Texture* d7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_006.png" );
	Texture* d8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_007.png" );
	Texture* d9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_008.png" );
	Texture* d10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_009.png" );
	Texture* d11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_010.png" );
	Texture* d12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Frontier Defender Spartan Knight/PNG/PNG Sequences/Slashing/Slashing_011.png" );
	
	player1AttackTex.push_back( d1 );
	player1AttackTex.push_back( d2 );
	player1AttackTex.push_back( d3 );
	player1AttackTex.push_back( d4 );
	player1AttackTex.push_back( d5 );
	player1AttackTex.push_back( d6 );
	player1AttackTex.push_back( d7 );
	player1AttackTex.push_back( d8 );
	player1AttackTex.push_back( d9 );
	player1AttackTex.push_back( d10 );
	player1AttackTex.push_back( d11 );
	player1AttackTex.push_back( d12 );

	m_UPlayer1Attack = new SpriteAnimDefTex( 0 , player1AttackTex.size() - 1 , 1.f );
	m_UPlayer1Attack->m_animations = player1AttackTex;

	std::vector<Texture*> enemy0RunTex;

	Texture* e1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_000.png" );
	Texture* e2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_001.png" );
	Texture* e3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_002.png" );
	Texture* e4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_003.png" );
	Texture* e5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_004.png" );
	Texture* e6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_005.png" );
	Texture* e7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_006.png" );
	Texture* e8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_007.png" );
	Texture* e9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_008.png" );
	Texture* e10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_009.png" );
	Texture* e11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_010.png" );
	Texture* e12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Running/0_Gnoll_Running_011.png" );
	

	enemy0RunTex.push_back( e1 );
	enemy0RunTex.push_back( e2 );
	enemy0RunTex.push_back( e3 );
	enemy0RunTex.push_back( e4 );
	enemy0RunTex.push_back( e5 );
	enemy0RunTex.push_back( e6 );
	enemy0RunTex.push_back( e7 );
	enemy0RunTex.push_back( e8 );
	enemy0RunTex.push_back( e9 );
	enemy0RunTex.push_back( e10 );
	enemy0RunTex.push_back( e11 );
	enemy0RunTex.push_back( e12 );

	m_UEnemy0Run = new SpriteAnimDefTex( 0 , enemy0RunTex.size() - 1 , 1.f );
	m_UEnemy0Run->m_animations = enemy0RunTex;

	std::vector<Texture*> enemy0AttackTex;

	Texture* f1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_000.png" );
	Texture* f2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_001.png" );
	Texture* f3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_002.png" );
	Texture* f4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_003.png" );
	Texture* f5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_004.png" );
	Texture* f6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_005.png" );
	Texture* f7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_006.png" );
	Texture* f8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_007.png" );
	Texture* f9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_008.png" );
	Texture* f10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_009.png" );
	Texture* f11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_010.png" );
	Texture* f12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_1/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_011.png" );
	
	enemy0AttackTex.push_back( f1 );
	enemy0AttackTex.push_back( f2 );
	enemy0AttackTex.push_back( f3 );
	enemy0AttackTex.push_back( f4 );
	enemy0AttackTex.push_back( f5 );
	enemy0AttackTex.push_back( f6 );
	enemy0AttackTex.push_back( f7 );
	enemy0AttackTex.push_back( f8 );
	enemy0AttackTex.push_back( f9 );
	enemy0AttackTex.push_back( f10 );
	enemy0AttackTex.push_back( f11 );
	enemy0AttackTex.push_back( f12 );

	m_UEnemy0Attack = new SpriteAnimDefTex( 0 , enemy0AttackTex.size() - 1 , 1.f );
	m_UEnemy0Attack->m_animations = enemy0AttackTex;

	std::vector<Texture*> enemy1RunTex;

	Texture* x1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_000.png" );
	Texture* x2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_001.png" );
	Texture* x3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_002.png" );
	Texture* x4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_003.png" );
	Texture* x5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_004.png" );
	Texture* x6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_005.png" );
	Texture* x7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_006.png" );
	Texture* x8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_007.png" );
	Texture* x9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_008.png" );
	Texture* x10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_009.png" );
	Texture* x11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_010.png" );
	Texture* x12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Running/0_Gnoll_Running_011.png" );
	
	enemy1RunTex.push_back( x1 );
	enemy1RunTex.push_back( x2 );
	enemy1RunTex.push_back( x3 );
	enemy1RunTex.push_back( x4 );
	enemy1RunTex.push_back( x5 );
	enemy1RunTex.push_back( x6 );
	enemy1RunTex.push_back( x7 );
	enemy1RunTex.push_back( x8 );
	enemy1RunTex.push_back( x9 );
	enemy1RunTex.push_back( x10 );
	enemy1RunTex.push_back( x11 );
	enemy1RunTex.push_back( x12 );

	m_UEnemy1Run = new SpriteAnimDefTex( 0 , enemy1RunTex.size() - 1 , 1.f );
	m_UEnemy1Run->m_animations = enemy1RunTex;

	std::vector<Texture*> enemy1AttackTex;

	Texture* y1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_000.png" );
	Texture* y2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_001.png" );
	Texture* y3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_002.png" );
	Texture* y4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_003.png" );
	Texture* y5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_004.png" );
	Texture* y6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_005.png" );
	Texture* y7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_006.png" );
	Texture* y8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_007.png" );
	Texture* y9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_008.png" );
	Texture* y10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_009.png" );
	Texture* y11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_010.png" );
	Texture* y12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Gnoll_2/PNG/PNG Sequences/Slashing/0_Gnoll_Slashing_011.png" );

	enemy1AttackTex.push_back( y1 );
	enemy1AttackTex.push_back( y2 );
	enemy1AttackTex.push_back( y3 );
	enemy1AttackTex.push_back( y4 );
	enemy1AttackTex.push_back( y5 );
	enemy1AttackTex.push_back( y6 );
	enemy1AttackTex.push_back( y7 );
	enemy1AttackTex.push_back( y8 );
	enemy1AttackTex.push_back( y9 );
	enemy1AttackTex.push_back( y10 );
	enemy1AttackTex.push_back( y11 );
	enemy1AttackTex.push_back( y12 );

	m_UEnemy1Attack = new SpriteAnimDefTex( 0 , enemy1AttackTex.size() - 1 , 1.f );
	m_UEnemy1Attack->m_animations = enemy1AttackTex;

}

void Game::LoadTowerTextures()
{
	m_towerBase = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerFinal/Platform.png" );
	m_tower = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerFinal/Dot_C.png" );
	m_towerTurret = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerFinal/Gun_05_B.png" );
}

void Game::LoadTowerDestroyerTextures()
{
	std::vector<Texture*> walk1Tex;

	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew/salamander/Salamander_walk1.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew/salamander/Salamander_walk2.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew/salamander/Salamander_walk3.png" );

	walk1Tex.push_back( a1 );
	walk1Tex.push_back( a2 );
	walk1Tex.push_back( a3 );

	m_TowerDestroyer1WalkTex = new SpriteAnimDefTex( 0 , walk1Tex.size() - 1 , 0.5f );
	m_TowerDestroyer1WalkTex->m_animations = walk1Tex;

	std::vector<Texture*> attack1Tex;

	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew/salamander/Salamander_attack1.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew/salamander/Salamander_attack2.png" );

	attack1Tex.push_back( b1 );
	attack1Tex.push_back( b2 );

	m_TowerDestroyer1AttackTex = new SpriteAnimDefTex( 0 , attack1Tex.size() - 1 , 0.5f );
	m_TowerDestroyer1AttackTex->m_animations = attack1Tex;

	std::vector<Texture*> walk2Tex;

	Texture* c1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/walk1.png" );
	Texture* c2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/walk2.png" );
	Texture* c3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/walk3.png" );
	Texture* c4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/walk4.png" );

	walk2Tex.push_back( c1 );
	walk2Tex.push_back( c2 );
	walk2Tex.push_back( c3 );
	walk2Tex.push_back( c4 );

	m_TowerDestroyer2WalkTex = new SpriteAnimDefTex( 0 , walk2Tex.size() - 1 , 0.7f );
	m_TowerDestroyer2WalkTex->m_animations = walk2Tex;

	std::vector<Texture*> attack2Tex;

	Texture* d1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/attack1.png" );
	Texture* d2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/attack2.png" );
	Texture* d3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/attack3.png" );
	Texture* d4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/attack4.png" );
	Texture* d5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/attack5.png" );

	attack2Tex.push_back( d1 );
	attack2Tex.push_back( d2 );
	attack2Tex.push_back( d3 );
	attack2Tex.push_back( d4 );
	attack2Tex.push_back( d5 );

	m_TowerDestroyer2AttackTex = new SpriteAnimDefTex( 0 , walk2Tex.size() - 1 , 1.f );
	m_TowerDestroyer2AttackTex->m_animations = attack2Tex;

}

void Game::LoadTowerBulletTextures()
{
	m_bullet0tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/FinalWeapons/Exhaust_1_2.png" );
	m_bullet1tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/FinalWeapons/Exhaust_3_1.png" );
	m_bullet2tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/FinalWeapons/weapon_19_plasma_fire.png" );
	m_bullet3tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/FinalWeapons/weapon_20_plasma_ice.png" );
}

void Game::SpawnUnControlledHeros()
{
	int randomNumber1 = m_rng.RollRandomIntInRange( 1 , 3 );
	int randomNumber2 = m_rng.RollRandomIntInRange( 1 , 3 );
	int randomNumber3 = m_rng.RollRandomIntInRange( 1 , 3 );
	int randomNumber4 = m_rng.RollRandomIntInRange( 1 , 3 );
	
	switch ( randomNumber1 )
	{
	case 1:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2(25.f,90.f) , FACTION_GOOD , LANE1_1 );
		m_UplayerLane1_1.push_back( h );
		break;
	}

	case 2:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 25.f , 96.f ) , FACTION_GOOD , LANE1_2 );
		m_UplayerLane1_2.push_back( h );
		break;
	}

	case 3:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 25.f , 84.f ) , FACTION_GOOD , LANE2_1 );
		m_UplayerLane2_1.push_back( h );
		break;
	}
	default:
		break;
	}

	switch ( randomNumber2 )
	{
	case 1:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 25.f , 90.f ) , FACTION_GOOD , LANE1_1 );
		m_UplayerLane1_1.push_back( h );
		break;
	}

	case 2:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 25.f , 96.f ) , FACTION_GOOD , LANE1_2 );
		m_UplayerLane1_2.push_back( h );
		break;
	}

	case 3:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 25.f , 84.f ) , FACTION_GOOD , LANE2_1 );
		m_UplayerLane2_1.push_back( h );
		break;
	}
	default:
		break;
	}

	switch ( randomNumber3 )
	{
	case 1:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 290.f , 90.f ) , FACTION_BAD , LANE1_1 );
		m_UenemyLane1_1.push_back( h );
		break;
	}

	case 2:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 290.f , 96.f ) , FACTION_BAD , LANE1_2 );
		m_UenemyLane1_2.push_back( h );
		break;
	}

	case 3:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 290.f , 84.f ) , FACTION_BAD , LANE2_1 );
		m_UenemyLane2_1.push_back( h );
		break;
	}
	default:
		break;
	}

	switch ( randomNumber4 )
	{
	case 1:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 290.f , 90.f ) , FACTION_BAD , LANE1_1 );
		m_UenemyLane1_1.push_back( h );
		break;
	}

	case 2:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 290.f , 96.f ) , FACTION_BAD , LANE1_2 );
		m_UenemyLane1_2.push_back( h );
		break;
	}

	case 3:
	{
		UncontrolledHero* h = new UncontrolledHero( this , Vec2( 290.f , 84.f ) , FACTION_BAD , LANE2_1 );
		m_UenemyLane2_1.push_back( h );
		break;
	}
	default:
		break;
	}
}

void Game::UpdateHealthBars()
{
	//m_healthBar.CarveBoxOffBottom( m_castleHealth / 200.f );
	//m_enemyHealthBar.CarveBoxOffBottom( m_enemyCastleHealth / 200.f );
}

void Game::ReduceHealth( float amt , bool isEnemy /*= false */ )
{
	if ( isEnemy )
	{
		m_enemyCastleHealth -= amt;
	}
	else
	{
		m_castleHealth -= amt;
	}
}

void Game::UpdateCosts()
{
	switch ( m_playerUpgradeState.meleeHeroState )
	{

	case UPGRADE_NONE:
	{
		m_currentMeleeHeroCost = MELEE_UPGRADE0_COST;
		break;
	}
	case  UPGRADE_SMALL:
	{
		m_currentMeleeHeroCost = MELEE_UPGRADE1_COST;
		break;
	}

	case  UPGRADE_MEDIUM:
	{
		m_currentMeleeHeroCost = MELEE_UPGRADE2_COST;
		break;
	}

	case  UPGRADE_LARGE:
	{
		m_currentMeleeHeroCost = MELEE_UPGRADE3_COST;
		break;
	}

	default:
		break;
	}

	switch ( m_playerUpgradeState.rangeHeroState )
	{

	case UPGRADE_NONE:
	{
		m_currentRangeHeroCost = RANGE_UPGRADE0_COST;
		break;
	}
	case  UPGRADE_SMALL:
	{
		m_currentRangeHeroCost = RANGE_UPGRADE1_COST;
		break;
	}

	case  UPGRADE_MEDIUM:
	{
		m_currentRangeHeroCost = RANGE_UPGRADE2_COST;
		break;
	}

	case  UPGRADE_LARGE:
	{
		m_currentRangeHeroCost = RANGE_UPGRADE3_COST;
		break;
	}

	default:
		break;
	}

	switch ( m_playerUpgradeState.towerState )
	{

	case UPGRADE_NONE:
	{
		m_currentTowerCost = TOWER_UPGRADE0_SPAWN_COST;
		break;
	}
	case  UPGRADE_SMALL:
	{
		m_currentTowerCost = TOWER_UPGRADE1_SPAWN_COST;
		break;
	}

	case  UPGRADE_MEDIUM:
	{
		m_currentTowerCost = TOWER_UPGRADE2_SPAWN_COST;
		break;
	}

	case  UPGRADE_LARGE:
	{
		m_currentTowerCost = TOWER_UPGRADE3_SPAWN_COST;
		break;
	}

	default:
		break;
	}

	switch ( m_playerUpgradeState.towerDestroyerState )
	{

	case UPGRADE_NONE:
	{
		m_currentTowerDestroyerCost = TOWER_DESTROYER0_COST;
		break;
	}
	case  UPGRADE_SMALL:
	{
		m_currentTowerDestroyerCost = TOWER_DESTROYER1_COST;
		break;
	}

	default:
		break;
	}

}

void Game::DisplayCoins()
{
	AABB2 coinTextBox = AABB2( Vec2(20.f,50.f) , Vec2(40.f,60.f) );
	coinTextBox.Translate( Vec2(245.f,110.f) );

	AABB2 coinBox = coinTextBox;
	coinBox.Translate( Vec2( 20.f , 0.f ) );
	
	std::string c = std::to_string( m_playerCoins );
	std::vector<Vertex_PCU> verts;

	m_font->AddVertsForTextInBox2D( verts , coinBox , 3.f , c , Rgba8( 0 , 0 , 100 , 255 ) , 2.f , Vec2( 0.2f , 0.3f ) );

	g_theRenderer->BindTexture( g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Coins.png" ) );
	g_theRenderer->DrawAABB2D( coinTextBox ,Rgba8(255,255,255,255));

	g_theRenderer->BindTexture( m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( verts );
}

bool Game::SpawnTower( Vec2 position , Faction faction, Lanes lane, Upgrade upg )
{
	if ( lane == LANE1_1 )
	{
		if ( faction == FACTION_GOOD )
		{
			if ( m_playerTower1 == nullptr )
			{
				m_playerTower1 = new Tower( this , Vec2() , FACTION_GOOD , upg );
				m_playerTower1->m_towerBase = m_ptowerBase1Box;
				m_playerTower1->m_tower = m_ptowerBase1Box;
				m_playerTower1->m_position = m_ptowerBase1Box.GetCenter();
				return true;
			}
		}
		else
		{
			if ( m_enemyTower1 == nullptr )
			{
				m_enemyTower1 = new Tower( this , Vec2() , FACTION_BAD , upg );
				m_enemyTower1->m_towerBase = m_etowerBase1Box;
				m_enemyTower1->m_tower = m_etowerBase1Box;
				m_enemyTower1->m_position = m_etowerBase1Box.GetCenter();
				return true;
			}
		}
	}
	else
	{
		if ( faction == FACTION_GOOD )
		{
			if ( m_playerTower2 == nullptr )
			{
				m_playerTower2 = new Tower( this , Vec2() , FACTION_GOOD , upg );
				m_playerTower2->m_towerBase = m_ptowerBase2Box;
				m_playerTower2->m_tower = m_ptowerBase2Box;
				m_playerTower2->m_position = m_ptowerBase2Box.GetCenter();
				m_playerTower2->rotateClockWise = true;
				return true;
			}
		}
		else
		{
			if ( m_enemyTower2 == nullptr )
			{
				m_enemyTower2 = new Tower( this , Vec2() , FACTION_BAD , upg );
				m_enemyTower2->m_towerBase = m_etowerBase2Box;
				m_enemyTower2->m_tower = m_etowerBase2Box;
				m_enemyTower2->m_position = m_etowerBase2Box.GetCenter();
				m_enemyTower2->rotateClockWise = true;
				return true;
			}
		}
	}

	return false;
}

void Game::SpawnTowerDestroyer( Faction faction , Upgrade upg , Lanes lane )
{
	if ( lane == LANE1_1 && faction == FACTION_GOOD )
	{
		TowerDestroyer* td = new TowerDestroyer( this , Vec2( 305.f , 35.f ) , faction , upg , LANE1_1 );
		m_towerDestroyers.push_back( td );
		return;
	}

	if ( lane == LANE1_1 && faction == FACTION_BAD )
	{
		TowerDestroyer* td = new TowerDestroyer( this , Vec2( 10.f , 35.f ) , faction , upg , LANE1_1 );
		m_towerDestroyers.push_back( td );
		return;
	}

	if ( lane == LANE1_2 && faction == FACTION_GOOD )
	{
		TowerDestroyer* td = new TowerDestroyer( this , Vec2( 305.f , 140.f ) , faction , upg , LANE1_2 );
		m_towerDestroyers.push_back( td );
		return;
	}
	if ( lane == LANE1_2 && faction == FACTION_BAD )
	{
		TowerDestroyer* td = new TowerDestroyer( this , Vec2( 10.f , 140.f ) , faction , upg , LANE1_2 );
		m_towerDestroyers.push_back( td );
		return;
	}

}

void Game::SpawnToweBullet( Faction faction , Upgrade upg , Vec2 position , float orientationDegrees , bool shouldMoveForward )
{
	TowerBullet* b = new TowerBullet( this , position , faction , upg , shouldMoveForward , orientationDegrees );
	m_towerBullets.push_back( b );
}

void Game::MeleeHeroTest( Faction faction , Upgrade upg , Lanes lane )
{
	//Hero* hero = new Hero( this , Vec2(50.f,50.f) , faction );
	//m_playerMeleeHerosLane1_1.push_back( hero );

	//Hero* h3 = new Hero( this , Vec2( 80.f , 50.f ) , faction );
	//h3->m_upgrade = UPGRADE_LARGE;

	//m_playerMeleeHerosLane1_1.push_back( h3 );

	Hero* h4 = new Hero( this , Vec2( 90.f+100.f , 60.f ) , FACTION_BAD ,LANE1_1, UPGRADE_NONE);
	
	m_enemyMeleeHerosLane1_1.push_back( h4 );

	Hero* h5 = new Hero( this , Vec2( 200.f , 60.f ) , FACTION_BAD,LANE1_1, UPGRADE_SMALL );


	m_enemyMeleeHerosLane1_1.push_back( h5 );

	Hero* h6 = new Hero( this , Vec2( 210.f , 60.f ) , FACTION_BAD,LANE1_1, UPGRADE_MEDIUM );


	m_enemyMeleeHerosLane1_1.push_back( h6 );


	Hero* h7 = new Hero( this , Vec2( 220.f , 60.f ) , FACTION_BAD,LANE1_1, UPGRADE_LARGE );
	

	m_enemyMeleeHerosLane1_1.push_back( h7 );
}

void Game::RangeHeroTest( Faction faction , Upgrade upg , Lanes lane )
{
	RangeHero* hero = new RangeHero( this , Vec2( 50.f , 90.f ) , faction );
	hero->m_upgrade = UPGRADE_NONE;
	m_playerRangeHerosLane1_1.push_back( hero );

	RangeHero* h = new RangeHero( this , Vec2( 60.f , 90.f ) , faction );
	h->m_upgrade = UPGRADE_SMALL;
	m_playerRangeHerosLane1_1.push_back( h );

	RangeHero* h1 = new RangeHero( this , Vec2( 70.f , 90.f ) , faction );
	h1->m_upgrade = UPGRADE_MEDIUM;
	m_playerRangeHerosLane1_1.push_back( h1 );

	RangeHero* h2 = new RangeHero( this , Vec2( 80.f , 90.f ) , faction );
	h2->m_upgrade = UPGRADE_LARGE;
	m_playerRangeHerosLane1_1.push_back( h2 );
	
}

void Game::UHeroTest()
{
	UncontrolledHero* h1 = new UncontrolledHero( this, Vec2( 50.f , 90.f ) , FACTION_GOOD , LANE1_1 , true );
	m_UplayerLane1_1.push_back( h1 );

	UncontrolledHero* h11 = new UncontrolledHero( this , Vec2( 50.f , 94.f ) , FACTION_GOOD , LANE1_1 , true );
	m_UplayerLane1_1.push_back( h11 );

	UncontrolledHero* h12 = new UncontrolledHero( this , Vec2( 50.f , 86.f ) , FACTION_GOOD , LANE1_1 , true );
	m_UplayerLane1_1.push_back( h12 );


	UncontrolledHero* h2 = new UncontrolledHero( this , Vec2( 60.f , 90.f ) , FACTION_GOOD , LANE1_1 , false );
	m_UplayerLane1_1.push_back( h2 );

	UncontrolledHero* h3 = new UncontrolledHero( this , Vec2( 70.f , 90.f ) , FACTION_BAD , LANE1_1 , true );
	m_UplayerLane1_1.push_back( h3 );

	UncontrolledHero* h4 = new UncontrolledHero( this , Vec2( 80.f , 90.f ) , FACTION_BAD , LANE1_1 , false );
	m_UplayerLane1_1.push_back( h4 );
}

void Game::TDtest()
{
	TowerDestroyer* td = new TowerDestroyer( this , Vec2(305.f,35.f) );
	m_towerDestroyers.push_back( td );

	TowerDestroyer* td1 = new TowerDestroyer( this , Vec2( 305.f , 140.f ),FACTION_GOOD,UPGRADE_SMALL );
	m_towerDestroyers.push_back( td1 );

	TowerDestroyer* td2 = new TowerDestroyer( this , Vec2( 10.f , 140.f ) , FACTION_BAD , UPGRADE_SMALL );
	m_towerDestroyers.push_back( td2 );

	TowerDestroyer* td3 = new TowerDestroyer( this , Vec2( 10.f , 35.f ) , FACTION_BAD , UPGRADE_SMALL );
	m_towerDestroyers.push_back( td3 );
}

void Game::CreateShootObjects(Faction faction,Vec2 position,Texture* tex,Lanes lane)
{
	ShootObject* obj = new ShootObject( this , position , tex , faction, lane );
	m_shootObjectsLane1_1.push_back( obj );
}

void Game::CreateMeleeHero( Faction faction , Upgrade upg , Lanes lane )
{
	switch ( faction )
	{
	case FACTION_GOOD:
	{
		if ( lane == LANE1_1 )
		{
			Hero* h1 = new Hero( this , Vec2( 25.f , 60.f ) , faction , lane, upg );
			m_playerMeleeHerosLane1_1.push_back( h1 );
		}
		if ( lane == LANE1_2 )
		{
			Hero* h1 = new Hero( this , Vec2( 25.f , 65.f ) , faction , lane, upg );
			m_playerMeleeHerosLane1_2.push_back( h1 );
		}
		if ( lane == LANE2_1 )
		{
			Hero* h1 = new Hero( this , Vec2( 25.f , 110.f ) , faction , lane, upg );
			m_playerMeleeHerosLane2_1.push_back( h1 );
		}
		if ( lane == LANE2_2 )
		{
			Hero* h1 = new Hero( this , Vec2( 25.f , 115.f ) , faction , lane, upg );
			m_playerMeleeHerosLane2_2.push_back( h1 );
		}
		break;
	}
	case FACTION_BAD:
	{
		if ( lane == LANE1_1 )
		{
			Hero* h1 = new Hero( this , Vec2( 295.f , 60.f ) , faction , lane, upg );
			m_enemyMeleeHerosLane1_1.push_back( h1 );
		}
		if ( lane == LANE1_2 )
		{
			Hero* h1 = new Hero( this , Vec2( 295.f , 65.f ) , faction , lane, upg );
			m_enemyMeleeHerosLane1_2.push_back( h1 );
		}
		if ( lane == LANE2_1 )
		{
			Hero* h1 = new Hero( this , Vec2( 295.f , 110.f ) , faction , lane, upg );
			m_enemyMeleeHerosLane2_1.push_back( h1 );
		}
		if ( lane == LANE2_2 )
		{
			Hero* h1 = new Hero( this , Vec2( 295.f , 115.f ) , faction , lane, upg );
			m_enemyMeleeHerosLane2_2.push_back( h1 );
		}
		break;
	}
	default:break;
	}
}

void Game::CreateRangeHero( Faction faction , Upgrade upg , Lanes lane )
{
	switch ( faction )
	{
	case FACTION_GOOD:
	{
		if ( lane == LANE1_1 )
		{
			RangeHero* h1 = new RangeHero( this , Vec2( 25.f , 60.f ) , faction , lane , upg );
			m_playerRangeHerosLane1_1.push_back( h1 );
		}
		if ( lane == LANE1_2 )
		{
			RangeHero* h1 = new RangeHero( this , Vec2( 25.f , 65.f ) , faction , lane , upg );
			m_playerRangeHerosLane1_2.push_back( h1 );
		}
		if ( lane == LANE2_1 )
		{
			RangeHero* h1 = new RangeHero( this , Vec2( 25.f , 110.f ) , faction , lane , upg );
			m_playerRangeHerosLane2_1.push_back( h1 );
		}
		if ( lane == LANE2_2 )
		{
			RangeHero* h1 = new RangeHero( this , Vec2( 25.f , 115.f ) , faction , lane , upg );
			m_playerRangeHerosLane2_2.push_back( h1 );
		}
		break;
	}
	case FACTION_BAD:
	{
		if ( lane == LANE1_1 )
		{
			RangeHero* h1 = new RangeHero( this , Vec2( 295.f , 60.f ) , faction , lane , upg );
			m_enemyRangeHerosLane1_1.push_back( h1 );
		}
		if ( lane == LANE1_2 )
		{
			RangeHero* h1 = new RangeHero( this , Vec2( 295.f , 65.f ) , faction , lane , upg );
			m_enemyRangeHerosLane1_2.push_back( h1 );
		}
		if ( lane == LANE2_1 )
		{
			RangeHero* h1 = new RangeHero( this , Vec2( 295.f , 110.f ) , faction , lane , upg );
			m_enemyRangeHerosLane2_1.push_back( h1 );
		}
		if ( lane == LANE2_2 )
		{
			RangeHero* h1 = new RangeHero( this , Vec2( 295.f , 115.f ) , faction , lane , upg );
			m_enemyRangeHerosLane2_2.push_back( h1 );
		}
		break;
	}
	default:break;
	}
}

void Game::DeleteGarbageEntities()
{
	for ( int i = 0; i < m_playerMeleeHerosLane1_1.size(); i++ )
	{
		if ( m_playerMeleeHerosLane1_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_playerMeleeHerosLane1_1[ i ]->m_isGarbage )
		{
			delete m_playerMeleeHerosLane1_1[ i ];
			m_playerMeleeHerosLane1_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_playerMeleeHerosLane1_2.size(); i++ )
	{
		if ( m_playerMeleeHerosLane1_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_playerMeleeHerosLane1_2[ i ]->m_isGarbage )
		{
			delete m_playerMeleeHerosLane1_2[ i ];
			m_playerMeleeHerosLane1_2[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_playerMeleeHerosLane2_1.size(); i++ )
	{
		if ( m_playerMeleeHerosLane2_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_playerMeleeHerosLane2_1[ i ]->m_isGarbage )
		{
			delete m_playerMeleeHerosLane2_1[ i ];
			m_playerMeleeHerosLane2_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_playerMeleeHerosLane2_2.size(); i++ )
	{
		if ( m_playerMeleeHerosLane2_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_playerMeleeHerosLane2_2[ i ]->m_isGarbage )
		{
			delete m_playerMeleeHerosLane2_2[ i ];
			m_playerMeleeHerosLane2_2[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane1_1.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane1_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_enemyMeleeHerosLane1_1[ i ]->m_isGarbage )
		{
			delete m_enemyMeleeHerosLane1_1[ i ];
			m_enemyMeleeHerosLane1_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane1_2.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane1_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_enemyMeleeHerosLane1_2[ i ]->m_isGarbage )
		{
			delete m_enemyMeleeHerosLane1_2[ i ];
			m_enemyMeleeHerosLane1_2[ i ] = nullptr;
		}
	}


	for ( int i = 0; i < m_enemyMeleeHerosLane2_1.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane2_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_enemyMeleeHerosLane2_1[ i ]->m_isGarbage )
		{
			delete m_enemyMeleeHerosLane2_1[ i ];
			m_enemyMeleeHerosLane2_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_enemyMeleeHerosLane2_2.size(); i++ )
	{
		if ( m_enemyMeleeHerosLane2_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_enemyMeleeHerosLane2_2[ i ]->m_isGarbage )
		{
			delete m_enemyMeleeHerosLane2_2[ i ];
			m_enemyMeleeHerosLane2_2[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_enemyRangeHerosLane1_1.size(); i++ )
	{
		if ( m_enemyRangeHerosLane1_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_enemyRangeHerosLane1_1[ i ]->m_isGarbage )
		{
			delete m_enemyRangeHerosLane1_1[ i ];
			m_enemyRangeHerosLane1_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_enemyRangeHerosLane1_2.size(); i++ )
	{
		if ( m_enemyRangeHerosLane1_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_enemyRangeHerosLane1_2[ i ]->m_isGarbage )
		{
			delete m_enemyRangeHerosLane1_2[ i ];
			m_enemyRangeHerosLane1_2[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_enemyRangeHerosLane2_1.size(); i++ )
	{
		if ( m_enemyRangeHerosLane2_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_enemyRangeHerosLane2_1[ i ]->m_isGarbage )
		{
			delete m_enemyRangeHerosLane2_1[ i ];
			m_enemyRangeHerosLane2_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_enemyRangeHerosLane2_2.size(); i++ )
	{
		if ( m_enemyRangeHerosLane2_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_enemyRangeHerosLane2_2[ i ]->m_isGarbage )
		{
			delete m_enemyRangeHerosLane2_2[ i ];
			m_enemyRangeHerosLane2_2[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane1_1.size(); i++ )
	{
		if ( m_playerRangeHerosLane1_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_playerRangeHerosLane1_1[ i ]->m_isGarbage )
		{
			delete m_playerRangeHerosLane1_1[ i ];
			m_playerRangeHerosLane1_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane1_2.size(); i++ )
	{
		if ( m_playerRangeHerosLane1_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_playerRangeHerosLane1_2[ i ]->m_isGarbage )
		{
			delete m_playerRangeHerosLane1_2[ i ];
			m_playerRangeHerosLane1_2[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane2_1.size(); i++ )
	{
		if ( m_playerRangeHerosLane2_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_playerRangeHerosLane2_1[ i ]->m_isGarbage )
		{
			delete m_playerRangeHerosLane2_1[ i ];
			m_playerRangeHerosLane2_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_playerRangeHerosLane2_2.size(); i++ )
	{
		if ( m_playerRangeHerosLane2_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_playerRangeHerosLane2_2[ i ]->m_isGarbage )
		{
			delete m_playerRangeHerosLane2_2[ i ];
			m_playerRangeHerosLane2_2[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_shootObjectsLane1_1.size(); i++ )
	{
		if ( m_shootObjectsLane1_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_shootObjectsLane1_1[ i ]->m_isGarbage )
		{
			delete m_shootObjectsLane1_1[ i ];
			m_shootObjectsLane1_1[ i ] = nullptr;
		}
	}


	for ( int i = 0; i < m_UplayerLane1_1.size(); i++ )
	{
		if ( m_UplayerLane1_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_UplayerLane1_1[ i ]->m_isGarbage )
		{
			delete m_UplayerLane1_1[ i ];
			m_UplayerLane1_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_UplayerLane1_2.size(); i++ )
	{
		if ( m_UplayerLane1_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_UplayerLane1_2[ i ]->m_isGarbage )
		{
			delete m_UplayerLane1_2[ i ];
			m_UplayerLane1_2[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_UplayerLane2_1.size(); i++ )
	{
		if ( m_UplayerLane2_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_UplayerLane2_1[ i ]->m_isGarbage )
		{
			delete m_UplayerLane2_1[ i ];
			m_UplayerLane2_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_UenemyLane1_1.size(); i++ )
	{
		if ( m_UenemyLane1_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_UenemyLane1_1[ i ]->m_isGarbage )
		{
			delete m_UenemyLane1_1[ i ];
			m_UenemyLane1_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_UenemyLane1_2.size(); i++ )
	{
		if ( m_UenemyLane1_2[ i ] == nullptr )
		{
			continue;
		}

		if ( m_UenemyLane1_2[ i ]->m_isGarbage )
		{
			delete m_UenemyLane1_2[ i ];
			m_UenemyLane1_2[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_UenemyLane2_1.size(); i++ )
	{
		if ( m_UenemyLane2_1[ i ] == nullptr )
		{
			continue;
		}

		if ( m_UenemyLane2_1[ i ]->m_isGarbage )
		{
			delete m_UenemyLane2_1[ i ];
			m_UenemyLane2_1[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_towerDestroyers.size(); i++ )
	{
		if ( m_towerDestroyers[ i ] != nullptr )
		{
			if ( m_towerDestroyers[ i ]->m_isGarbage )
			{
				delete m_towerDestroyers[ i ];
				m_towerDestroyers[ i ] = nullptr;
			}
		}
	}

	for ( int i = 0; i < m_towerBullets.size(); i++ )
	{
		if ( m_towerBullets[ i ] != nullptr )
		{
			if ( m_towerBullets[ i ]->m_isGarbage )
			{
				delete m_towerBullets[ i ];
				m_towerBullets[ i ] = nullptr;
			}
		}
	}

	if ( m_playerTower1 != nullptr )
	{
		if ( m_playerTower1->m_isGarbage )
		{
			delete m_playerTower1;
			m_playerTower1 = nullptr;
		}
	}

	if ( m_playerTower2 != nullptr )
	{
		if ( m_playerTower2->m_isGarbage )
		{
			delete m_playerTower2;
			m_playerTower2 = nullptr;
		}
	}

	if ( m_enemyTower1 != nullptr )
	{
		if ( m_enemyTower1->m_isGarbage )
		{
			delete m_enemyTower1;
			m_enemyTower1 = nullptr;
		}
	}

	if ( m_enemyTower2 != nullptr )
	{
		if ( m_enemyTower2->m_isGarbage )
		{
			delete m_enemyTower2;
			m_enemyTower2 = nullptr;
		}
	}

}

void Game::CreateHUD()
{
	m_meleeHeroHud = new HUD(this,TYPE_MELEE);

	temp = m_UIAreaBot.GetPercentageOfAABB( 0.9 );
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

	m_meleeHeroHud->HeroSpriteBox = MeleeHeroBox;
	m_meleeHeroHud->HeroCostBox = MeleeHeroCostBox;
	m_meleeHeroHud->HeroUpgradeBox = MeleeHeroUpgradeBox;
	m_meleeHeroHud->HeroUpgradeCostBox = MeleeHeroUpgradeCostBox;
	m_meleeHeroHud->HeroUpgradeInfoBox = MeleeHeroInfoBox;

	m_meleeHeroHud->SpriteBoxTexure =  g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/btn/button_empty_1.png" ) ;
	m_meleeHeroHud->CostBoxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/table.png" );
	m_meleeHeroHud->UpgradeBoxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/ico_4.png" );

	m_towerDestroyerHud = new HUD( this , TYPE_TOWER_DESTROYER );
	m_towerDestroyerHud->HeroSpriteBox = MeleeHeroBox;
	m_towerDestroyerHud->HeroCostBox = MeleeHeroCostBox;
	m_towerDestroyerHud->HeroUpgradeBox = MeleeHeroUpgradeBox;
	m_towerDestroyerHud->HeroUpgradeCostBox = MeleeHeroUpgradeCostBox;
	m_towerDestroyerHud->HeroUpgradeInfoBox = MeleeHeroInfoBox;

	m_towerDestroyerHud->SpriteBoxTexure = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/btn/button_empty_1.png" );
	m_towerDestroyerHud->CostBoxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/table.png" );
	m_towerDestroyerHud->UpgradeBoxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/ico_4.png" );

	m_towerDestroyerHud->HeroSpriteBox.Translate( Vec2(0.f,158.f) );
	m_towerDestroyerHud->HeroCostBox.Translate( Vec2(0.f,168.5f) );
	m_towerDestroyerHud->HeroUpgradeBox.Translate( Vec2(0.f,149.f) );
	m_towerDestroyerHud->HeroUpgradeCostBox.Translate( Vec2(0.f,149.f) );
	m_towerDestroyerHud->HeroUpgradeInfoBox.Translate( Vec2(0.f,139.5f) );

	m_rangeHeroHud = new HUD( this,TYPE_RANGE );
	AABB2 RangeHeroBox = MeleeHeroBox;
	RangeHeroBox.Translate( Vec2( 70.f , 0.f ) );
	AABB2 RangeHeroCostBox = AABB2( Vec2( RangeHeroBox.maxs.x , RangeHeroBox.mins.y ) , Vec2( RangeHeroBox.maxs.x + 15.f , RangeHeroBox.mins.y + heroBoxDimensions.y * 0.5f ));
	AABB2 RangeHeroUpgradeBox = AABB2( Vec2( RangeHeroCostBox.mins.x , RangeHeroCostBox.maxs.y ) , Vec2( RangeHeroCostBox.maxs.x + 10.f , RangeHeroCostBox.maxs.y + ( heroBoxDimensions.y * 0.5f ) ) );

	temp1 = RangeHeroUpgradeBox.GetBoxAtLeft( 0.4f );
	temp2 = RangeHeroUpgradeBox.GetBoxAtRight( 0.6f );
	RangeHeroUpgradeBox = temp1;
	AABB2 RangeHeroUpgradeCostBox = temp2;
	AABB2 RangeHeroInfoBox = AABB2( Vec2( RangeHeroBox.mins ) , Vec2( RangeHeroBox.mins.x + dimensions1.x + dimensions2.x , RangeHeroBox.mins.y ) );
	RangeHeroInfoBox.Translate( Vec2( 0.f , dimensions1.y ) );

	m_rangeHeroHud->HeroSpriteBox = RangeHeroBox;
	m_rangeHeroHud->HeroCostBox = RangeHeroCostBox;
	m_rangeHeroHud->HeroUpgradeBox = RangeHeroUpgradeBox;
	m_rangeHeroHud->HeroUpgradeCostBox = RangeHeroUpgradeCostBox;
	m_rangeHeroHud->HeroUpgradeInfoBox = RangeHeroInfoBox;

	m_rangeHeroHud->SpriteBoxTexure = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/btn/button_empty_1.png" );
	m_rangeHeroHud->CostBoxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/table.png" );
	m_rangeHeroHud->UpgradeBoxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/ico_4.png" );

	m_towerHud = new HUD( this,TYPE_TOWER );
	AABB2 towerBox = RangeHeroBox;
	towerBox.Translate( Vec2(70.f,0.f) );
	AABB2 towerCostBox = AABB2( Vec2( towerBox.maxs.x , towerBox.mins.y ) , Vec2( towerBox.maxs.x + 15.f , towerBox.mins.y + heroBoxDimensions.y * 0.5f ) );
	AABB2 towerUpgradeBox = AABB2( Vec2( towerCostBox.mins.x , towerCostBox.maxs.y ) , Vec2( towerCostBox.maxs.x + 10.f , towerCostBox.maxs.y + ( heroBoxDimensions.y * 0.5f ) ) );

	temp1 = towerUpgradeBox.GetBoxAtLeft( 0.4f );
	temp2 = towerUpgradeBox.GetBoxAtRight( 0.6f );
	towerUpgradeBox = temp1;
	AABB2 towerUpgradeCostBox = temp2;
	AABB2 towerInfoBox = AABB2( Vec2( towerBox.mins ) , Vec2( towerBox.mins.x + dimensions1.x + dimensions2.x , towerBox.mins.y ) );
	//towerInfoBox.Translate( Vec2( 0.f , dimensions1.y ) );

	m_towerHud->HeroSpriteBox = towerBox;
	m_towerHud->HeroCostBox = towerCostBox;
	m_towerHud->HeroUpgradeBox = towerUpgradeBox;
	m_towerHud->HeroUpgradeCostBox = towerUpgradeCostBox;
	m_towerHud->HeroUpgradeInfoBox = towerInfoBox;
	m_towerHud->HeroUpgradeInfoBox.Translate( Vec2( 0.f , dimensions1.y ) );

	m_towerHud->SpriteBoxTexure = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/btn/button_empty_1.png" );
	m_towerHud->CostBoxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/table.png" );
	m_towerHud->UpgradeBoxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/upgrade/ico_4.png" );

	m_selectedHUD = new SelectedHUD( this );

	m_selectedHUD->m_textBox = AABB2( Vec2(towerBox.mins) , Vec2(towerInfoBox.maxs+Vec2(15.f,10.f)) );
	m_selectedHUD->m_textBox.Translate( Vec2(60.f,5.f) );

	m_selectedHUD->m_textboxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/selected.png" );

	m_selectedHUD->m_spriteBox = towerBox;
	m_selectedHUD->m_spriteBox.Translate( Vec2(110.f,0.f) );
	m_selectedHUD->m_spriteBoxTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/btn/button_empty_1.png" );
	



	std::vector<Texture*> meleeHeroIdle0Tex;
	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_000.png" );
	Texture* t2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_001.png" );
	Texture* t3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_002.png" );
	Texture* t4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_003.png" );
	Texture* t5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_004.png" );
	Texture* t6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_005.png" );
	Texture* t7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_006.png" );
	Texture* t8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_007.png" );
	Texture* t9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_008.png" );
	Texture* t10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_009.png" );
	Texture* t11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_010.png" );
	Texture* t12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_011.png" );
	Texture* t13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_012.png" );
	Texture* t14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_013.png" );
	Texture* t15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_014.png" );
	Texture* t16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_015.png" );
	Texture* t17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_016.png" );
	Texture* t18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_017.png" );
	meleeHeroIdle0Tex.push_back( t1 );
	meleeHeroIdle0Tex.push_back( t2 );
	meleeHeroIdle0Tex.push_back( t3 );
	meleeHeroIdle0Tex.push_back( t4 );
	meleeHeroIdle0Tex.push_back( t5 );
	meleeHeroIdle0Tex.push_back( t6 );
	meleeHeroIdle0Tex.push_back( t7 );
	meleeHeroIdle0Tex.push_back( t8 );
	meleeHeroIdle0Tex.push_back( t9 );
	meleeHeroIdle0Tex.push_back( t10 );
	meleeHeroIdle0Tex.push_back( t11 );
	meleeHeroIdle0Tex.push_back( t12 );
	meleeHeroIdle0Tex.push_back( t13 );
	meleeHeroIdle0Tex.push_back( t14 );
	meleeHeroIdle0Tex.push_back( t15 );
	meleeHeroIdle0Tex.push_back( t16 );
	meleeHeroIdle0Tex.push_back( t17 );
	meleeHeroIdle0Tex.push_back( t18 );


	SpriteAnimDefTex* meleeHeroIdleAnims = new SpriteAnimDefTex(0,meleeHeroIdle0Tex.size()-1,1.5f);
	meleeHeroIdleAnims->m_animations = meleeHeroIdle0Tex;
	m_meleeHeroHud->heroUpgrade0Anims = meleeHeroIdleAnims;

	std::vector<Texture*> meleeHeroIdle1Tex;
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_007.png" );
	Texture* a9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_008.png" );
	Texture* a10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_009.png" );
	Texture* a11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_010.png" );
	Texture* a12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_011.png" );
	Texture* a13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_012.png" );
	Texture* a14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_013.png" );
	Texture* a15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_014.png" );
	Texture* a16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_015.png" );
	Texture* a17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_016.png" );
	Texture* a18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_017.png" );
	
	meleeHeroIdle1Tex.push_back( a1 );
	meleeHeroIdle1Tex.push_back( a2 );
	meleeHeroIdle1Tex.push_back( a3 );
	meleeHeroIdle1Tex.push_back( a4 );
	meleeHeroIdle1Tex.push_back( a5 );
	meleeHeroIdle1Tex.push_back( a6 );
	meleeHeroIdle1Tex.push_back( a7 );
	meleeHeroIdle1Tex.push_back( a8 );
	meleeHeroIdle1Tex.push_back( a9 );
	meleeHeroIdle1Tex.push_back( a10 );
	meleeHeroIdle1Tex.push_back( a11 );
	meleeHeroIdle1Tex.push_back( a12 );
	meleeHeroIdle1Tex.push_back( a13 );
	meleeHeroIdle1Tex.push_back( a14 );
	meleeHeroIdle1Tex.push_back( a15 );
	meleeHeroIdle1Tex.push_back( a16 );
	meleeHeroIdle1Tex.push_back( a17 );
	meleeHeroIdle1Tex.push_back( a18 );

	m_meleeHeroHud->heroUpgrade1Anims = new SpriteAnimDefTex( 0 , meleeHeroIdle1Tex.size() - 1 , 1.f );
	m_meleeHeroHud->heroUpgrade1Anims->m_animations = meleeHeroIdle1Tex;

	std::vector<Texture*> meleeHeroIdle2Tex;

	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_000.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_001.png" );
	Texture* b3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_002.png" );
	Texture* b4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_003.png" );
	Texture* b5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_004.png" );
	Texture* b6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_005.png" );
	Texture* b7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_006.png" );
	Texture* b8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_007.png" );
	Texture* b9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_008.png" );
	Texture* b10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_009.png" );
	Texture* b11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_010.png" );
	Texture* b12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_011.png" );
	Texture* b13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_012.png" );
	Texture* b14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_013.png" );
	Texture* b15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_014.png" );
	Texture* b16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_015.png" );
	Texture* b17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_016.png" );
	Texture* b18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Templar Knight/PNG/PNG Sequences/Idle/Idle_017.png" );
	
	meleeHeroIdle2Tex.push_back( b1 );
	meleeHeroIdle2Tex.push_back( b2 );
	meleeHeroIdle2Tex.push_back( b3 );
	meleeHeroIdle2Tex.push_back( b4 );
	meleeHeroIdle2Tex.push_back( b5 );
	meleeHeroIdle2Tex.push_back( b6 );
	meleeHeroIdle2Tex.push_back( b7 );
	meleeHeroIdle2Tex.push_back( b8 );
	meleeHeroIdle2Tex.push_back( b9 );
	meleeHeroIdle2Tex.push_back( b10 );
	meleeHeroIdle2Tex.push_back( b11 );
	meleeHeroIdle2Tex.push_back( b12 );
	meleeHeroIdle2Tex.push_back( b13 );
	meleeHeroIdle2Tex.push_back( b14 );
	meleeHeroIdle2Tex.push_back( b15 );
	meleeHeroIdle2Tex.push_back( b16 );
	meleeHeroIdle2Tex.push_back( b17 );
	meleeHeroIdle2Tex.push_back( b18 );

	m_meleeHeroHud->heroUpgrade2Anims = new SpriteAnimDefTex( 0 , meleeHeroIdle2Tex.size() - 1 , 1.f );
	m_meleeHeroHud->heroUpgrade2Anims->m_animations = meleeHeroIdle2Tex;

	std::vector<Texture*> meleeHeroIdle3Tex;

	Texture* c1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_000.png" );
	Texture* c2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_001.png" );
	Texture* c3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_002.png" );
	Texture* c4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_003.png" );
	Texture* c5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_004.png" );
	Texture* c6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_005.png" );
	Texture* c7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_006.png" );
	Texture* c8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_007.png" );
	Texture* c9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_008.png" );
	Texture* c10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_009.png" );
	Texture* c11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_010.png" );
	Texture* c12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_011.png" );
	Texture* c13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_012.png" );
	Texture* c14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_013.png" );
	Texture* c15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_014.png" );
	Texture* c16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_015.png" );
	Texture* c17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_016.png" );
	Texture* c18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Very Heavy Armored Frontier Defender/PNG/PNG Sequences/Idle/Idle_017.png" );

	meleeHeroIdle3Tex.push_back( c1 );
	meleeHeroIdle3Tex.push_back( c2 );
	meleeHeroIdle3Tex.push_back( c3 );
	meleeHeroIdle3Tex.push_back( c4 );
	meleeHeroIdle3Tex.push_back( c5 );
	meleeHeroIdle3Tex.push_back( c6 );
	meleeHeroIdle3Tex.push_back( c7 );
	meleeHeroIdle3Tex.push_back( c8 );
	meleeHeroIdle3Tex.push_back( c9 );
	meleeHeroIdle3Tex.push_back( c10 );
	meleeHeroIdle3Tex.push_back( c11 );
	meleeHeroIdle3Tex.push_back( c12 );
	meleeHeroIdle3Tex.push_back( c13 );
	meleeHeroIdle3Tex.push_back( c14 );
	meleeHeroIdle3Tex.push_back( c15 );
	meleeHeroIdle3Tex.push_back( c16 );
	meleeHeroIdle3Tex.push_back( c17 );
	meleeHeroIdle3Tex.push_back( c18 );

	m_meleeHeroHud->heroUpgrade3Anims = new SpriteAnimDefTex( 0 , meleeHeroIdle3Tex.size() - 1 , 1.f );
	m_meleeHeroHud->heroUpgrade3Anims->m_animations = meleeHeroIdle3Tex;

	std::vector<Texture*> playerUpdgrade0RangeIdleTextures;

	Texture* l1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_000.png" );
	Texture* l2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_001.png" );
	Texture* l3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_002.png" );
	Texture* l4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_003.png" );
	Texture* l5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_004.png" );
	Texture* l6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_005.png" );
	Texture* l7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_006.png" );
	Texture* l8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_007.png" );
	Texture* l9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_008.png" );
	Texture* l10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_009.png" );
	Texture* l11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_010.png" );
	Texture* l12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_011.png" );
	Texture* l13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_012.png" );
	Texture* l14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_013.png" );
	Texture* l15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_014.png" );
	Texture* l16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_015.png" );
	Texture* l17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_016.png" );
	Texture* l18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Medieval Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_017.png" );
	
	playerUpdgrade0RangeIdleTextures.push_back( l1 );
	playerUpdgrade0RangeIdleTextures.push_back( l2 );
	playerUpdgrade0RangeIdleTextures.push_back( l3 );
	playerUpdgrade0RangeIdleTextures.push_back( l4 );
	playerUpdgrade0RangeIdleTextures.push_back( l5 );
	playerUpdgrade0RangeIdleTextures.push_back( l6 );
	playerUpdgrade0RangeIdleTextures.push_back( l7 );
	playerUpdgrade0RangeIdleTextures.push_back( l8 );
	playerUpdgrade0RangeIdleTextures.push_back( l9 );
	playerUpdgrade0RangeIdleTextures.push_back( l10 );
	playerUpdgrade0RangeIdleTextures.push_back( l11 );
	playerUpdgrade0RangeIdleTextures.push_back( l12 );
	playerUpdgrade0RangeIdleTextures.push_back( l13 );
	playerUpdgrade0RangeIdleTextures.push_back( l14 );
	playerUpdgrade0RangeIdleTextures.push_back( l15 );
	playerUpdgrade0RangeIdleTextures.push_back( l16 );
	playerUpdgrade0RangeIdleTextures.push_back( l17 );
	playerUpdgrade0RangeIdleTextures.push_back( l18 );

	SpriteAnimDefTex* rangeIdelAnims = new SpriteAnimDefTex( 0 , playerUpdgrade0RangeIdleTextures.size() - 1 , 1.5f );
	rangeIdelAnims->m_animations = playerUpdgrade0RangeIdleTextures;
	m_rangeHeroHud->heroUpgrade0Anims = rangeIdelAnims;

	std::vector<Texture*> playerUpdgrade1RangeIdleTextures;

	Texture* d1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_000.png" );
	Texture* d2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_001.png" );
	Texture* d3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_002.png" );
	Texture* d4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_003.png" );
	Texture* d5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_004.png" );
	Texture* d6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_005.png" );
	Texture* d7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_006.png" );
	Texture* d8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_007.png" );
	Texture* d9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_008.png" );
	Texture* d10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_009.png" );
	Texture* d11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_010.png" );
	Texture* d12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_011.png" );
	Texture* d13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_012.png" );
	Texture* d14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_013.png" );
	Texture* d15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_014.png" );
	Texture* d16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_015.png" );
	Texture* d17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_016.png" );
	Texture* d18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Elementals_1/PNG/PNG Sequences/Idle Blinking/0_Elementals_Idle Blinking_017.png" );

	playerUpdgrade1RangeIdleTextures.push_back( d1 );
	playerUpdgrade1RangeIdleTextures.push_back( d2 );
	playerUpdgrade1RangeIdleTextures.push_back( d3 );
	playerUpdgrade1RangeIdleTextures.push_back( d4 );
	playerUpdgrade1RangeIdleTextures.push_back( d5 );
	playerUpdgrade1RangeIdleTextures.push_back( d6 );
	playerUpdgrade1RangeIdleTextures.push_back( d7 );
	playerUpdgrade1RangeIdleTextures.push_back( d8 );
	playerUpdgrade1RangeIdleTextures.push_back( d9 );
	playerUpdgrade1RangeIdleTextures.push_back( d10 );
	playerUpdgrade1RangeIdleTextures.push_back( d11 );
	playerUpdgrade1RangeIdleTextures.push_back( d12 );
	playerUpdgrade1RangeIdleTextures.push_back( d13 );
	playerUpdgrade1RangeIdleTextures.push_back( d14 );
	playerUpdgrade1RangeIdleTextures.push_back( d15 );
	playerUpdgrade1RangeIdleTextures.push_back( d16 );
	playerUpdgrade1RangeIdleTextures.push_back( d17 );
	playerUpdgrade1RangeIdleTextures.push_back( d18 );

	m_rangeHeroHud->heroUpgrade1Anims = new SpriteAnimDefTex( 0 , playerUpdgrade1RangeIdleTextures.size() - 1 , 1.f );
	m_rangeHeroHud->heroUpgrade1Anims->m_animations = playerUpdgrade1RangeIdleTextures;

	std::vector<Texture*> playerUpdgrade2RangeIdleTextures;

	Texture* e1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_000.png" );
	Texture* e2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_001.png" );
	Texture* e3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_002.png" );
	Texture* e4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_003.png" );
	Texture* e5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_004.png" );
	Texture* e6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_005.png" );
	Texture* e7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_006.png" );
	Texture* e8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_007.png" );
	Texture* e9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_008.png" );
	Texture* e10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_009.png" );
	Texture* e11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_010.png" );
	Texture* e12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_011.png" );
	Texture* e13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_012.png" );
	Texture* e14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_013.png" );
	Texture* e15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_014.png" );
	Texture* e16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_015.png" );
	Texture* e17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_016.png" );
	Texture* e18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Magician_3/PNG/PNG Sequences/Idle Blinking/0_Magician_Idle Blinking_017.png" );

	playerUpdgrade2RangeIdleTextures.push_back( e1 );
	playerUpdgrade2RangeIdleTextures.push_back( e2 );
	playerUpdgrade2RangeIdleTextures.push_back( e3 );
	playerUpdgrade2RangeIdleTextures.push_back( e4 );
	playerUpdgrade2RangeIdleTextures.push_back( e5 );
	playerUpdgrade2RangeIdleTextures.push_back( e6 );
	playerUpdgrade2RangeIdleTextures.push_back( e7 );
	playerUpdgrade2RangeIdleTextures.push_back( e8 );
	playerUpdgrade2RangeIdleTextures.push_back( e9 );
	playerUpdgrade2RangeIdleTextures.push_back( e10 );
	playerUpdgrade2RangeIdleTextures.push_back( e11 );
	playerUpdgrade2RangeIdleTextures.push_back( e12 );
	playerUpdgrade2RangeIdleTextures.push_back( e13 );
	playerUpdgrade2RangeIdleTextures.push_back( e14 );
	playerUpdgrade2RangeIdleTextures.push_back( e15 );
	playerUpdgrade2RangeIdleTextures.push_back( e16 );
	playerUpdgrade2RangeIdleTextures.push_back( e17 );
	playerUpdgrade2RangeIdleTextures.push_back( e18 );

	m_rangeHeroHud->heroUpgrade2Anims = new SpriteAnimDefTex( 0 , playerUpdgrade2RangeIdleTextures.size() - 1 , 1.f );
	m_rangeHeroHud->heroUpgrade2Anims->m_animations = playerUpdgrade2RangeIdleTextures;

	std::vector<Texture*> playerUpdgrade3RangeIdleTextures;

	Texture* f1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_000.png" );
	Texture* f2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_001.png" );
	Texture* f3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_002.png" );
	Texture* f4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_003.png" );
	Texture* f5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_004.png" );
	Texture* f6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_005.png" );
	Texture* f7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_006.png" );
	Texture* f8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_007.png" );
	Texture* f9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_008.png" );
	Texture* f10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_009.png" );
	Texture* f11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_010.png" );
	Texture* f12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_011.png" );
	Texture* f13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_012.png" );
	Texture* f14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_013.png" );
	Texture* f15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_014.png" );
	Texture* f16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_015.png" );
	Texture* f17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_016.png" );
	Texture* f18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Heavy Armored Defender Knight/PNG/PNG Sequences/Idle Blinking/Idle Blinking_017.png" );

	playerUpdgrade3RangeIdleTextures.push_back( f1 );
	playerUpdgrade3RangeIdleTextures.push_back( f2 );
	playerUpdgrade3RangeIdleTextures.push_back( f3 );
	playerUpdgrade3RangeIdleTextures.push_back( f4 );
	playerUpdgrade3RangeIdleTextures.push_back( f5 );
	playerUpdgrade3RangeIdleTextures.push_back( f6 );
	playerUpdgrade3RangeIdleTextures.push_back( f7 );
	playerUpdgrade3RangeIdleTextures.push_back( f8 );
	playerUpdgrade3RangeIdleTextures.push_back( f9 );
	playerUpdgrade3RangeIdleTextures.push_back( f10 );
	playerUpdgrade3RangeIdleTextures.push_back( f11 );
	playerUpdgrade3RangeIdleTextures.push_back( f12 );
	playerUpdgrade3RangeIdleTextures.push_back( f13 );
	playerUpdgrade3RangeIdleTextures.push_back( f14 );
	playerUpdgrade3RangeIdleTextures.push_back( f15 );
	playerUpdgrade3RangeIdleTextures.push_back( f16 );
	playerUpdgrade3RangeIdleTextures.push_back( f17 );
	playerUpdgrade3RangeIdleTextures.push_back( f18 );

	m_rangeHeroHud->heroUpgrade3Anims = new SpriteAnimDefTex( 0 , playerUpdgrade3RangeIdleTextures.size() - 1 , 1.f );
	m_rangeHeroHud->heroUpgrade3Anims->m_animations = playerUpdgrade3RangeIdleTextures;

	std::vector<Texture*> tower0Anims;

	tower0Anims.push_back( m_towerTurret );

	//m_towerHud->heroUpgrade0Anims = rangeIdelAnims;
	m_towerHud->heroUpgrade0Anims = new SpriteAnimDefTex( 0 , 0 , 1.f );
	m_towerHud->heroUpgrade0Anims->m_animations = tower0Anims;


	std::vector<Texture*> towerDestroyer0IdleAnims;

	Texture* aa1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew/salamander/Salamander_idle1.png" );
	Texture* aa2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew/salamander/Salamander_idle2.png" );
	Texture* aa3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew/salamander/Salamander_idle3.png" );

	towerDestroyer0IdleAnims.push_back( aa1 );
	towerDestroyer0IdleAnims.push_back( aa2 );
	towerDestroyer0IdleAnims.push_back( aa3 );

	m_towerDestroyerHud->heroUpgrade0Anims = new SpriteAnimDefTex( 0 , towerDestroyer0IdleAnims.size() - 1 , 0.4f );
	m_towerDestroyerHud->heroUpgrade0Anims->m_animations = towerDestroyer0IdleAnims;

	std::vector<Texture*> towerDestroyer1IdleAnims;

	Texture* bb1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/0dino.png" );
	Texture* bb2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/idle2.png" );
	Texture* bb3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/idle3.png" );
	Texture* bb4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/idle4.png" );
	Texture* bb5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Misc/MonsterPackNew2/dino/idle5.png" );

	towerDestroyer1IdleAnims.push_back( bb1 );
	towerDestroyer1IdleAnims.push_back( bb2 );
	towerDestroyer1IdleAnims.push_back( bb3 );
	towerDestroyer1IdleAnims.push_back( bb4 );
	towerDestroyer1IdleAnims.push_back( bb5 );

	m_towerDestroyerHud->heroUpgrade1Anims = new SpriteAnimDefTex( 0 , towerDestroyer1IdleAnims.size() - 1 , 0.5f );
	m_towerDestroyerHud->heroUpgrade1Anims->m_animations = towerDestroyer1IdleAnims;


	/*std::vector<Texture*> meleeHeroIdleTex;
	Texture* r1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_000.png" );*/

}

void Game::DetectPlayerActions()
{
	
	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_meleeHeroHud->HeroSpriteBox.IsPointInside( m_mousePosition ) )
	{
		m_playerAction = ACTION_SELECT_MELEE;
	}

	/*if ( g_theInput->WasRightMouseButtonJustPressed() && m_meleeHeroHud->HeroUpgradeBox.IsPointInside( m_mousePosition ) )
	{
		m_meleeHeroHud->isUpdateInfoBoxClosed = !m_meleeHeroHud->isUpdateInfoBoxClosed;
	}*/

	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_meleeHeroHud->HeroUpgradeBox.IsPointInside( m_mousePosition ) )
	{
		switch ( m_playerUpgradeState.meleeHeroState )
		{
		case UPGRADE_NONE:
		{
			if ( m_playerCoins >= MELEE_HERO_UPGRADE1_COST )
			{
				m_playerUpgradeState.meleeHeroState = UPGRADE_SMALL;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= MELEE_HERO_UPGRADE1_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		case UPGRADE_SMALL:
		{
			if ( m_playerCoins >= MELEE_HERO_UPGRADE2_COST )
			{
				m_playerUpgradeState.meleeHeroState = UPGRADE_MEDIUM;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= MELEE_HERO_UPGRADE2_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		case UPGRADE_MEDIUM:
		{
			if ( m_playerCoins >= MELEE_HERO_UPGRADE3_COST )
			{
				m_playerUpgradeState.meleeHeroState = UPGRADE_LARGE;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= MELEE_HERO_UPGRADE3_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		default:g_theAudio->PlaySound( error ); break;
		}
	}

	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_rangeHeroHud->HeroSpriteBox.IsPointInside( m_mousePosition ) )
	{
		m_playerAction = ACTION_SELECT_RANGE;
	}

	/*if ( g_theInput->WasRightMouseButtonJustPressed() && m_rangeHeroHud->HeroUpgradeBox.IsPointInside( m_mousePosition ) )
	{
		m_rangeHeroHud->isUpdateInfoBoxClosed = !m_rangeHeroHud->isUpdateInfoBoxClosed;
	}*/

	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_rangeHeroHud->HeroUpgradeBox.IsPointInside( m_mousePosition ) )
	{
		switch ( m_playerUpgradeState.rangeHeroState )
		{
		case UPGRADE_NONE:
		{
			if ( m_playerCoins >= RANGE_HERO_UPGRADE1_COST )
			{
				m_playerUpgradeState.rangeHeroState = UPGRADE_SMALL;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= RANGE_HERO_UPGRADE1_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		case UPGRADE_SMALL:
		{
			if ( m_playerCoins >= RANGE_HERO_UPGRADE2_COST )
			{
				m_playerUpgradeState.rangeHeroState = UPGRADE_MEDIUM;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= RANGE_HERO_UPGRADE2_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		case UPGRADE_MEDIUM:
		{
			if ( m_playerCoins >= RANGE_HERO_UPGRADE3_COST )
			{
				m_playerUpgradeState.rangeHeroState = UPGRADE_LARGE;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= RANGE_HERO_UPGRADE3_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		default:g_theAudio->PlaySound( error ); break;
		}
	}

	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_towerHud->HeroSpriteBox.IsPointInside( m_mousePosition ) )
	{
		m_playerAction = ACTION_SELECT_TOWER;
	}

	/*if ( g_theInput->WasRightMouseButtonJustPressed() && m_towerHud->HeroUpgradeBox.IsPointInside( m_mousePosition ) )
	{
		m_towerHud->isUpdateInfoBoxClosed = !m_towerHud->isUpdateInfoBoxClosed;
	}*/

	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_towerHud->HeroUpgradeBox.IsPointInside( m_mousePosition ) )
	{
		switch ( m_playerUpgradeState.towerState )
		{
		case UPGRADE_NONE:
		{
			if ( m_playerCoins >= TOWER_UPGRADE0_COST )
			{
				m_playerUpgradeState.towerState = UPGRADE_SMALL;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= TOWER_UPGRADE0_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		case UPGRADE_SMALL:
		{
			if ( m_playerCoins >= TOWER_UPGRADE1_COST )
			{
				m_playerUpgradeState.towerState = UPGRADE_MEDIUM;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= TOWER_UPGRADE1_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		case UPGRADE_MEDIUM:
		{
			if ( m_playerCoins >= TOWER_UPGRADE2_COST )
			{
				m_playerUpgradeState.towerState = UPGRADE_LARGE;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= TOWER_UPGRADE2_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		default:g_theAudio->PlaySound( error ); break;
		}
	}

	//if ( g_theInput->WasRightMouseButtonJustPressed() && m_towerDestroyerHud->HeroUpgradeBox.IsPointInside( m_mousePosition ) )
	//{
	//	m_towerDestroyerHud->isUpdateInfoBoxClosed = !m_towerDestroyerHud->isUpdateInfoBoxClosed;
	//}

	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_towerDestroyerHud->HeroUpgradeBox.IsPointInside( m_mousePosition ) )
	{
		switch ( m_playerUpgradeState.towerDestroyerState )
		{
		case UPGRADE_NONE:
		{
			if ( m_playerCoins >= TOWER_DESTROYER_UPGRADE_COST )
			{
				m_playerUpgradeState.towerDestroyerState = UPGRADE_SMALL;
				m_playerAction = ACTION_NONE;
				m_playerCoins -= TOWER_DESTROYER_UPGRADE_COST;
				g_theAudio->PlaySound( success );
			}
			else
			{
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( error );
			}
			break;
		}
		default:g_theAudio->PlaySound( error ); break;
		}
	}

	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_towerDestroyerHud->HeroSpriteBox.IsPointInside( m_mousePosition ) )
	{
		if ( m_playerCoins >= m_currentTowerDestroyerCost )
		{
			int randomNumber = m_rng.GetRandomIntInRange( 0 , 1 );

			if ( randomNumber == 0 )
			{
				if ( m_enemyTower1 )
				{
					SpawnTowerDestroyer( FACTION_GOOD , m_playerUpgradeState.towerDestroyerState , LANE1_1 );
					m_playerCoins -= m_currentTowerDestroyerCost;
					g_theAudio->PlaySound( success );
				}
				else if ( m_enemyTower2 )
				{
					SpawnTowerDestroyer( FACTION_GOOD , m_playerUpgradeState.towerDestroyerState , LANE1_2 );
					m_playerCoins -= m_currentTowerDestroyerCost;
					g_theAudio->PlaySound( success );
				}
				else
				{
					g_theAudio->PlaySound( error );
				}
			}

			else
			{
				if ( m_enemyTower2 )
				{
					SpawnTowerDestroyer( FACTION_GOOD , m_playerUpgradeState.towerDestroyerState , LANE1_2 );
					m_playerCoins -= m_currentTowerDestroyerCost;
					g_theAudio->PlaySound( success );
				}
				else if ( m_enemyTower1 )
				{
					SpawnTowerDestroyer( FACTION_GOOD , m_playerUpgradeState.towerDestroyerState , LANE1_1 );
					m_playerCoins -= m_currentTowerDestroyerCost;
					g_theAudio->PlaySound( success );
				}
				else
				{
					g_theAudio->PlaySound( error );
				}
			}
		
		}
	}
	
}

void Game::ExecutePlayerActions()
{
	if ( m_playerAction == ACTION_SELECT_MELEE )
	{
		if ( g_theInput->WasLeftMouseButtonJustPressed() && m_lane1.IsPointInside( m_mousePosition ) )
		{
			if ( m_playerCoins >= m_currentMeleeHeroCost )
			{

				int randomNum = m_rng.GetRandomIntInRange( 0 , 1 );

				if ( randomNum == 0 )
				{
					CreateMeleeHero( FACTION_GOOD , m_playerUpgradeState.meleeHeroState , LANE1_1 );
					g_theAudio->PlaySound( success );
				}
				else
				{
					CreateMeleeHero( FACTION_GOOD , m_playerUpgradeState.meleeHeroState , LANE1_2 );
					g_theAudio->PlaySound( success );
				}

				m_playerCoins -= m_currentMeleeHeroCost;

				m_playerAction = ACTION_NONE;
			}
			else
			{
				g_theAudio->PlaySound( error );
				m_playerAction = ACTION_NONE;
			}
		}

		if ( g_theInput->WasLeftMouseButtonJustPressed() && m_lane2.IsPointInside( m_mousePosition ) )
		{
			if ( m_playerCoins >= m_currentMeleeHeroCost )
			{
				int randomNum = m_rng.GetRandomIntInRange( 0 , 1 );

				if ( randomNum == 0 )
				{
					CreateMeleeHero( FACTION_GOOD , m_playerUpgradeState.meleeHeroState , LANE2_1 );
					g_theAudio->PlaySound( success );
				}
				else
				{
					CreateMeleeHero( FACTION_GOOD , m_playerUpgradeState.meleeHeroState , LANE2_2 );
					g_theAudio->PlaySound( success );
				}
				
				m_playerCoins -= m_currentMeleeHeroCost;
				m_playerAction = ACTION_NONE;
			}
			else
			{
				g_theAudio->PlaySound( error );
				m_playerAction = ACTION_NONE;
			}
		}

	}

	if ( m_playerAction == ACTION_SELECT_RANGE )
	{
		if ( g_theInput->WasLeftMouseButtonJustPressed() && m_lane1.IsPointInside( m_mousePosition ) )
		{
			if ( m_playerCoins >= m_currentRangeHeroCost )
			{
				int randomNum = m_rng.GetRandomIntInRange( 0 , 1 );

				if ( randomNum == 0 )
				{
					CreateRangeHero( FACTION_GOOD , m_playerUpgradeState.rangeHeroState , LANE1_1 );
					g_theAudio->PlaySound( success );
				}
				else
				{
					CreateRangeHero( FACTION_GOOD , m_playerUpgradeState.rangeHeroState , LANE1_2 );
					g_theAudio->PlaySound( success );
				}
				m_playerCoins -= m_currentRangeHeroCost;
				m_playerAction = ACTION_NONE;
			}
			else
			{
				g_theAudio->PlaySound( error );
				m_playerAction = ACTION_NONE;
			}
		}

		if ( g_theInput->WasLeftMouseButtonJustPressed() && m_lane2.IsPointInside( m_mousePosition ) )
		{
			if ( m_playerCoins >= m_currentRangeHeroCost )
			{
				int randomNum = m_rng.GetRandomIntInRange( 0 , 1 );

				if ( randomNum == 0 )
				{
					CreateRangeHero( FACTION_GOOD , m_playerUpgradeState.rangeHeroState , LANE2_1 );
					g_theAudio->PlaySound( success );
				}
				else
				{
					CreateRangeHero( FACTION_GOOD , m_playerUpgradeState.rangeHeroState , LANE2_2 );
					g_theAudio->PlaySound( success );
				}
				m_playerCoins -= m_currentRangeHeroCost;
				m_playerAction = ACTION_NONE;
			}
			else
			{
				g_theAudio->PlaySound( error );
				m_playerAction = ACTION_NONE;
			}
		}

	}

	if ( m_playerAction == ACTION_SELECT_TOWER )
	{
		if ( g_theInput->WasLeftMouseButtonJustPressed() && IsPointInsideDisc2D(m_mousePosition,m_ptowerBase1Box.GetCenter(),5.f) )
		{
			if ( m_playerCoins >= m_currentTowerCost )
			{
				SpawnTower( Vec2() , FACTION_GOOD , LANE1_1 , m_playerUpgradeState.towerState );
				m_playerCoins -= m_currentTowerCost;
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( success );
			}
			else
			{
				g_theAudio->PlaySound( error );
				m_playerAction = ACTION_NONE;
			}
		}

		if ( g_theInput->WasLeftMouseButtonJustPressed() && IsPointInsideDisc2D( m_mousePosition , m_ptowerBase2Box.GetCenter() , 5.f ) )
		{
			if ( m_playerCoins >= m_currentTowerCost )
			{
				SpawnTower( Vec2() , FACTION_GOOD , LANE1_2 , m_playerUpgradeState.towerState );
				m_playerCoins -= m_currentTowerCost;
				m_playerAction = ACTION_NONE;
				g_theAudio->PlaySound( success );
			}
			else
			{
				g_theAudio->PlaySound( error );
				m_playerAction = ACTION_NONE;
			}
		}
	}
}

void Game::UpdateMousePosition()
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();
	
	m_mousePosition.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	m_mousePosition.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );
	
}
