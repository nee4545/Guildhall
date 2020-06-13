#include "Game/Game.hpp"
#include "Game/Hero.hpp"
#include "Game/HUD.hpp"
#include "Game/RangeHero.hpp"
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

	tinyxml2::XMLDocument gameConfig;
	gameConfig.LoadFile( "Data/XML/gameconfig.xml" );

	tinyxml2::XMLElement* ele = gameConfig.RootElement();
	std::string soundPath = ParseXmlAttribute( *ele , "soundPath" , "none" );

	testSound = g_theAudio->CreateOrGetSound( soundPath);


	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 320.f , 180.f ));

	m_camera->SetPosition( Vec3( 0.f , 0.f , 0.f ) );

	m_UICamera = new Camera();
	m_UICamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 320.f , 180.f ) );

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );

	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

	//m_camera->m_transform.m_position  = Vec3( -10.f , 0.f , 0.f );

	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );
	m_UICamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );
	m_devConsoleCamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

	m_guiBackround = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/registration/table_1.png" );

	LayeredObject mapBackround;

	mapBackround.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet3/PNG/game_background_1/layers/main_bg.png" );
	mapBackround.object = AABB2( Vec2( 0.f , 0.f ) , Vec2( 320.f , 180.f ) );

	LayeredObject basePlayer_1;
	basePlayer_1.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_1.object = AABB2( Vec2( 10.f , 50.f ) , Vec2( 25.f , 75.f ) );

	LayeredObject basePlayer_2;
	basePlayer_2.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_2.object = AABB2( Vec2( 10.f , 60.f ) , Vec2( 25.f , 85.f ) );

	LayeredObject basePlayer_3;
	basePlayer_3.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_3.object = AABB2( Vec2( 10.f , 70.f ) , Vec2( 25.f , 95.f ) );

	LayeredObject basePlayer_4;
	basePlayer_4.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_4.object = AABB2( Vec2( 10.f , 80.f ) , Vec2( 25.f , 105.f ) );

	LayeredObject basePlayer_5;
	basePlayer_5.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_5.object = AABB2( Vec2( 10.f , 90.f ) , Vec2( 25.f , 115.f ) );

	LayeredObject basePlayer_6;
	basePlayer_6.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/5.png" );
	basePlayer_6.object = AABB2( Vec2( 10.f , 100.f ) , Vec2( 25.f , 125.f ) );

	
	LayeredObject baseEnemy_1;
	baseEnemy_1.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_1.object = AABB2( Vec2( 295.f , 50.f ) , Vec2( 310.f , 75.f ) );

	LayeredObject baseEnemy_2;
	baseEnemy_2.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_2.object = AABB2( Vec2( 295.f , 60.f ) , Vec2( 310.f , 85.f ) );

	LayeredObject baseEnemy_3;
	baseEnemy_3.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_3.object = AABB2( Vec2( 295.f , 70.f ) , Vec2( 310.f , 95.f ) );

	LayeredObject baseEnemy_4;
	baseEnemy_4.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_4.object = AABB2( Vec2( 295.f , 80.f ) , Vec2( 310.f , 105.f ) );

	LayeredObject baseEnemy_5;
	baseEnemy_5.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_5.object = AABB2( Vec2( 295.f , 90.f ) , Vec2( 310.f , 115.f ) );

	LayeredObject baseEnemy_6;
	baseEnemy_6.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Towers/TowerPack1/6.png" );
	baseEnemy_6.object = AABB2( Vec2( 295.f , 100.f ) , Vec2( 310.f , 125.f ) );

	
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


	LayeredObject path1;
	path1.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path1.object = AABB2( Vec2( 25.f , 55.f ) , Vec2( 55.f , 70.f ));

	LayeredObject path2;
	path2.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path2.object = AABB2( Vec2( 55.f , 55.f ) , Vec2( 85.f , 70.f ) );

	LayeredObject path3;
	path3.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path3.object = AABB2( Vec2( 85.f , 55.f ) , Vec2( 115.f , 70.f ) );

	LayeredObject path4;
	path4.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path4.object = AABB2( Vec2( 115.f , 55.f ) , Vec2( 145.f , 70.f ) );

	LayeredObject path5;
	path5.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path5.object = AABB2( Vec2( 145.f , 55.f ) , Vec2( 185.f , 70.f ) );

	LayeredObject path6;
	path6.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path6.object = AABB2( Vec2( 185.f , 55.f ) , Vec2( 215.f , 70.f ) );

	LayeredObject path7;
	path7.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path7.object = AABB2( Vec2( 215.f , 55.f ) , Vec2( 245.f , 70.f ) );

	LayeredObject path8;
	path8.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path8.object = AABB2( Vec2( 245.f , 55.f ) , Vec2( 295.f , 70.f ) );

	m_Layer1Objects.push_back( path1 );
	m_Layer1Objects.push_back( path2 );
	m_Layer1Objects.push_back( path3 );
	m_Layer1Objects.push_back( path4 );
	m_Layer1Objects.push_back( path5 );
	m_Layer1Objects.push_back( path6 );
	m_Layer1Objects.push_back( path7 );
	m_Layer1Objects.push_back( path8 );
	
	LayeredObject path2_1;
	path2_1.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path2_1.object = AABB2( Vec2( 25.f , 105.f ) , Vec2( 55.f , 120.f ) );

	LayeredObject path2_2;
	path2_2.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path2_2.object = AABB2( Vec2( 55.f , 105.f ) , Vec2( 85.f , 120.f ) );

	LayeredObject path2_3;
	path2_3.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path2_3.object = AABB2( Vec2( 85.f , 105.f ) , Vec2( 115.f , 120.f ) );

	LayeredObject path2_4;
	path2_4.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path2_4.object = AABB2( Vec2( 115.f , 105.f ) , Vec2( 145.f , 120.f ) );

	LayeredObject path2_5;
	path2_5.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path2_5.object = AABB2( Vec2( 145.f , 105.f ) , Vec2( 185.f , 120.f ) );

	LayeredObject path2_6;
	path2_6.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path2_6.object = AABB2( Vec2( 185.f , 105.f ) , Vec2( 215.f , 120.f ) );

	LayeredObject path2_7;
	path2_7.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path2_7.object = AABB2( Vec2( 215.f , 105.f ) , Vec2( 245.f , 120.f ) );

	LayeredObject path2_8;
	path2_8.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/road_5.png" );
	path2_8.object = AABB2( Vec2( 245.f , 105.f ) , Vec2( 295.f , 120.f ) );

	m_Layer1Objects.push_back( path2_1 );
	m_Layer1Objects.push_back( path2_2 );
	m_Layer1Objects.push_back( path2_3 );
	m_Layer1Objects.push_back( path2_4 );
	m_Layer1Objects.push_back( path2_5 );
	m_Layer1Objects.push_back( path2_6 );
	m_Layer1Objects.push_back( path2_7 );
	m_Layer1Objects.push_back( path2_8 );

	LayeredObject towerSpot1;
	towerSpot1.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/dot.png" );
	towerSpot1.object = AABB2(Vec2( 25.f,10.f),Vec2(45.f,30.f));
	towerSpot1.object.Translate(Vec2( 50.f , 15.f ));

	LayeredObject towerSpot2;
	towerSpot2.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/dot.png" );
	towerSpot2.object = AABB2( Vec2( 25.f , 10.f ) , Vec2( 45.f , 30.f ) );
	towerSpot2.object.Translate( Vec2( 200.f , 15.f ) );

	LayeredObject towerSpot3;
	towerSpot3.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/dot.png" );
	towerSpot3.object = AABB2( Vec2( 25.f , 10.f ) , Vec2( 45.f , 30.f ) );
	towerSpot3.object.Translate( Vec2( 50.f , 120.f ) );

	LayeredObject towerSpot4;
	towerSpot4.tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/TileSets/TileSet1/PNG/game_background_2/layers/dot.png" );
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


	m_Layer1Objects.push_back( decor1 );


	m_UIAreaBot = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );
	m_UIAreaBot.CarveBoxOffBottom( 0.12f );
	m_UIAreaTop = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );
	m_UIAreaTop.CarveBoxOffTop( 0.12f );

	

	
	LoadPlayerMeleeAnimations();
	LoadEnemyMeleeAnimations();
	LoadPlayerRangeAnimations();
	CreateHUD();
	MeleeHeroTest( FACTION_GOOD , UPGRADE_NONE , LANE_1 );
	RangeHeroTest( FACTION_GOOD , UPGRADE_NONE , LANE_1 );
	
}

Game::~Game()
{
	delete m_camera;
	delete m_UICamera;
	delete m_devConsoleCamera;
}

void Game::Update( float deltaseconds )
{
	
	for ( int i = 0; i < m_playerMeleeHeros.size(); i++ )
	{
		if ( m_playerMeleeHeros[ i ] != nullptr )
		{
			m_playerMeleeHeros[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_playerRangeHeros.size(); i++ )
	{
		if ( m_playerRangeHeros[ i ] != nullptr )
		{
			m_playerRangeHeros[ i ]->Update( deltaseconds );
		}
	}

	for ( int i = 0; i < m_enemyHeros.size(); i++ )
	{
		if ( m_enemyHeros[ i ] != nullptr )
		{
			m_enemyHeros[ i ]->Update( deltaseconds );
		}
	}

	DetectPlayerActions();

	m_meleeHeroHud->Update( deltaseconds );
	m_rangeHeroHud->Update( deltaseconds );

	DeleteGarbageEntities();
}

void Game::Render()
{
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

	for ( int i = 0; i < m_playerMeleeHeros.size(); i++ )
	{
		if ( m_playerMeleeHeros[ i ] == nullptr )
		{
			continue;
		}
		m_playerMeleeHeros[ i ]->Render();
	}

	for ( int i = 0; i < m_playerRangeHeros.size(); i++ )
	{
		if ( m_playerRangeHeros[ i ] != nullptr )
		{
			m_playerRangeHeros[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_enemyHeros.size(); i++ )
	{
		if ( m_enemyHeros[ i ] == nullptr )
		{
			continue;
		}
		m_enemyHeros[ i ]->Render();
	}

	//DisplayMouseInfo();
	g_theRenderer->EndCamera( *m_camera );

	g_theRenderer->BeginCamera( *m_UICamera );

	g_theRenderer->DrawAABB2D( m_UIAreaBot , Rgba8( 125 , 125 , 125 , 125 ));
	g_theRenderer->DrawAABB2D( m_UIAreaTop , Rgba8( 125 , 125 , 125 , 125 ) );
	
	m_meleeHeroHud->Render();
	m_rangeHeroHud->Render();
	g_theRenderer->EndCamera( *m_camera );
}

void Game::LoadTextures()
{

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

void Game::MeleeHeroTest( Faction faction , Upgrade upg , Lanes lane )
{
	Hero* hero = new Hero( this , Vec2(50.f,50.f) , faction );
	m_playerMeleeHeros.push_back( hero );

	Hero* h3 = new Hero( this , Vec2( 80.f , 50.f ) , faction );
	h3->m_upgrade = UPGRADE_LARGE;

	m_playerMeleeHeros.push_back( h3 );

	Hero* h4 = new Hero( this , Vec2( 90.f+100.f , 50.f ) , FACTION_BAD );
	h4->m_upgrade = UPGRADE_NONE;
	h4->m_faction = FACTION_BAD;

	m_enemyHeros.push_back( h4 );

	Hero* h5 = new Hero( this , Vec2( 200.f , 50.f ) , FACTION_BAD );
	h5->m_upgrade = UPGRADE_SMALL;
	h5->m_faction = FACTION_BAD;

	m_enemyHeros.push_back( h5 );

	Hero* h6 = new Hero( this , Vec2( 210.f , 50.f ) , FACTION_BAD );
	h6->m_upgrade = UPGRADE_MEDIUM;
	h6->m_faction = FACTION_BAD;

	m_enemyHeros.push_back( h6 );


	Hero* h7 = new Hero( this , Vec2( 220.f , 50.f ) , FACTION_BAD );
	h7->m_upgrade = UPGRADE_LARGE;
	h7->m_faction = FACTION_BAD;

	m_enemyHeros.push_back( h7 );
}

void Game::RangeHeroTest( Faction faction , Upgrade upg , Lanes lane )
{
	RangeHero* hero = new RangeHero( this , Vec2( 50.f , 90.f ) , faction );
	hero->m_upgrade = UPGRADE_NONE;
	m_playerRangeHeros.push_back( hero );

	RangeHero* h = new RangeHero( this , Vec2( 60.f , 90.f ) , faction );
	h->m_upgrade = UPGRADE_SMALL;
	m_playerRangeHeros.push_back( h );

	RangeHero* h1 = new RangeHero( this , Vec2( 70.f , 90.f ) , faction );
	h1->m_upgrade = UPGRADE_MEDIUM;
	m_playerRangeHeros.push_back( h1 );

	RangeHero* h2 = new RangeHero( this , Vec2( 80.f , 90.f ) , faction );
	h2->m_upgrade = UPGRADE_LARGE;
	m_playerRangeHeros.push_back( h2 );
	
}

void Game::DeleteGarbageEntities()
{
	for ( int i = 0; i < m_playerMeleeHeros.size(); i++ )
	{
		if ( m_playerMeleeHeros[ i ] == nullptr )
		{
			continue;
		}

		if ( m_playerMeleeHeros[ i ]->m_isGarbage )
		{
			delete m_playerMeleeHeros[ i ];
			m_playerMeleeHeros[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_enemyHeros.size(); i++ )
	{
		if ( m_enemyHeros[ i ] == nullptr )
		{
			continue;
		}

		if ( m_enemyHeros[ i ]->m_isGarbage )
		{
			delete m_enemyHeros[ i ];
			m_enemyHeros[ i ] = nullptr;
		}
	}
}

void Game::CreateHUD()
{
	m_meleeHeroHud = new HUD(this);

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

	m_rangeHeroHud = new HUD( this );
	AABB2 RangeHeroBox = MeleeHeroBox;
	RangeHeroBox.Translate( Vec2( 80.f , 0.f ) );
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

	std::vector<Texture*> meleeHeroIdleTex;
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
	meleeHeroIdleTex.push_back( t1 );
	meleeHeroIdleTex.push_back( t2 );
	meleeHeroIdleTex.push_back( t3 );
	meleeHeroIdleTex.push_back( t4 );
	meleeHeroIdleTex.push_back( t5 );
	meleeHeroIdleTex.push_back( t6 );
	meleeHeroIdleTex.push_back( t7 );
	meleeHeroIdleTex.push_back( t8 );
	meleeHeroIdleTex.push_back( t9 );
	meleeHeroIdleTex.push_back( t10 );
	meleeHeroIdleTex.push_back( t11 );
	meleeHeroIdleTex.push_back( t12 );
	meleeHeroIdleTex.push_back( t13 );
	meleeHeroIdleTex.push_back( t14 );
	meleeHeroIdleTex.push_back( t15 );
	meleeHeroIdleTex.push_back( t16 );
	meleeHeroIdleTex.push_back( t17 );
	meleeHeroIdleTex.push_back( t18 );


	SpriteAnimDefTex* meleeHeroIdleAnims = new SpriteAnimDefTex(0,meleeHeroIdleTex.size()-1,1.5f);
	meleeHeroIdleAnims->m_animations = meleeHeroIdleTex;
	m_meleeHeroHud->heroUpgrade0Anims = meleeHeroIdleAnims;


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


	/*std::vector<Texture*> meleeHeroIdleTex;
	Texture* r1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/Greek Warrior/PNG/PNG Sequences/Idle Blinking/Idle Blinking_000.png" );*/



}

void Game::DetectPlayerActions()
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	Vec2 mousePositionScreen;

	mousePositionScreen.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	mousePositionScreen.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_meleeHeroHud->HeroSpriteBox.IsPointInside( mousePositionScreen ) )
	{
		MeleeHeroTest( FACTION_GOOD , UPGRADE_NONE , LANE_1 );
	}

	if ( g_theInput->WasLeftMouseButtonJustPressed() && m_meleeHeroHud->HeroUpgradeBox.IsPointInside( mousePositionScreen ) )
	{
		m_meleeHeroHud->isUpdateInfoBoxClosed = !m_meleeHeroHud->isUpdateInfoBoxClosed;
	}
}
