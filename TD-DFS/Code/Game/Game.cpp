#include "Game/Game.hpp"
#include "Game/Hero.hpp"
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
	m_UICamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );

	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

	//m_camera->m_transform.m_position  = Vec3( -10.f , 0.f , 0.f );

	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );
	m_UICamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );
	m_devConsoleCamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

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
	m_UIAreaBot.CarveBoxOffBottom( 0.1f );
	m_UIAreaTop = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );
	m_UIAreaTop.CarveBoxOffTop( 0.1f );

	LoadAnimations();
	CreateMeleeHeros( FACTION_GOOD , UPGRADE_NONE , LANE_1 );
	
	
}

Game::~Game()
{
	delete m_camera;
	delete m_UICamera;
	delete m_devConsoleCamera;
}

void Game::Update( float deltaseconds )
{
	for ( int i = 0; i < m_Heros.size(); i++ )
	{
		if ( m_Heros[ i ] != nullptr )
		{
			m_Heros[ i ]->Update( deltaseconds );
		}
	}
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

	for ( int i = 0; i < m_Heros.size(); i++ )
	{
		m_Heros[ i ]->Render();
	}

	DisplayMouseInfo();
	g_theRenderer->EndCamera( *m_camera );

	g_theRenderer->BeginCamera( *m_UICamera );
	g_theRenderer->DrawAABB2D( m_UIAreaBot , Rgba8( 100 , 100 , 100 , 255 ));
	g_theRenderer->DrawAABB2D( m_UIAreaTop , Rgba8( 100 , 100 , 100 , 255 ) );
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

void Game::LoadAnimations()
{
	std::vector<Texture*> playerUpdgrade0MeleeAttackTextures;
	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_000.png" );
	Texture* t2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_001.png" );
	Texture* t3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_002.png" );
	Texture* t4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_003.png" );
	Texture* t5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_004.png" );
	Texture* t6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_005.png" );
	Texture* t7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_006.png" );
	Texture* t8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_007.png" );
	Texture* t9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_008.png" );
	Texture* t10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_009.png" );
	Texture* t11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_010.png" );
	Texture* t12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Attacking/Satyr_01_Attacking_011.png" );

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
	Texture* r1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_000.png" );
	Texture* r2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_001.png" );
	Texture* r3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_002.png" );
	Texture* r4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_003.png" );
	Texture* r5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_004.png" );
	Texture* r6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_005.png" );
	Texture* r7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_006.png" );
	Texture* r8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_007.png" );
	Texture* r9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_008.png" );
	Texture* r10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_009.png" );
	Texture* r11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_010.png" );
	Texture* r12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_011.png" );
	Texture* r13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_012.png" );
	Texture* r14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_013.png" );
	Texture* r15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_014.png" );
	Texture* r16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_015.png" );
	Texture* r17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_016.png" );
	Texture* r18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_01/PNG/Walking/Satyr_01_Walking_017.png" );

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

	m_PlayerUpgrade0MeleeRun = new SpriteAnimDefTex( 0 , playerUpgrade0MeleeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade0MeleeRun->m_animations = playerUpgrade0MeleeRunTextures;

	std::vector<Texture*> playerUpdgrade1MeleeAttackTextures;
	Texture* tt1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_000.png" );
	Texture* tt2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_001.png" );
	Texture* tt3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_002.png" );
	Texture* tt4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_003.png" );
	Texture* tt5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_004.png" );
	Texture* tt6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_005.png" );
	Texture* tt7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_006.png" );
	Texture* tt8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_007.png" );
	Texture* tt9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_008.png" );
	Texture* tt10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_009.png" );
	Texture* tt11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_010.png" );
	Texture* tt12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Attacking/Satyr_02_Attacking_011.png" );
	
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
	Texture* rr1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_000.png" );
	Texture* rr2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_001.png" );
	Texture* rr3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_002.png" );
	Texture* rr4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_003.png" );
	Texture* rr5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_004.png" );
	Texture* rr6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_005.png" );
	Texture* rr7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_006.png" );
	Texture* rr8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_007.png" );
	Texture* rr9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_008.png" );
	Texture* rr10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_009.png" );
	Texture* rr11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_010.png" );
	Texture* rr12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_011.png" );
	Texture* rr13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_012.png" );
	Texture* rr14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_013.png" );
	Texture* rr15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_014.png" );
	Texture* rr16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_015.png" );
	Texture* rr17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_016.png" );
	Texture* rr18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_02/PNG/Walking/Satyr_02_Walking_017.png" );

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

	m_PlayerUpgrade1MeleeRun = new SpriteAnimDefTex( 0 , playerUpgrade1MeleeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade1MeleeRun->m_animations = playerUpgrade1MeleeRunTextures;

	std::vector<Texture*> playerUpdgrade2MeleeAttackTextures;
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_007.png" );
	Texture* a9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_008.png" );
	Texture* a10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_009.png" );
	Texture* a11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_010.png" );
	Texture* a12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Attacking/Satyr_03_Attacking_011.png" );

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
	
	Texture* aa1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_000.png" );
	Texture* aa2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_001.png" );
	Texture* aa3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_002.png" );
	Texture* aa4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_003.png" );
	Texture* aa5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_004.png" );
	Texture* aa6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_005.png" );
	Texture* aa7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_006.png" );
	Texture* aa8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_007.png" );
	Texture* aa9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_008.png" );
	Texture* aa10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_009.png" );
	Texture* aa11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_010.png" );
	Texture* aa12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_011.png" );
	Texture* aa13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_012.png" );
	Texture* aa14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_013.png" );
	Texture* aa15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_014.png" );
	Texture* aa16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_015.png" );
	Texture* aa17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_016.png" );
	Texture* aa18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Satyr_03/PNG/Walking/Satyr_03_Walking_017.png" );

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

	m_PlayerUpgrade2MeleeRun = new SpriteAnimDefTex( 0 , playerUpgrade2MeleeRunTextures.size() - 1 , 1.f );
	m_PlayerUpgrade2MeleeRun->m_animations = playerUpgrade2MeleeRunTextures;
	
	std::vector<Texture*> playerUpdgrade3MeleeAttackTextures;

	Texture* x1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_000.png" );
	Texture* x2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_001.png" );
	Texture* x3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_002.png" );
	Texture* x4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_003.png" );
	Texture* x5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_004.png" );
	Texture* x6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_005.png" );
	Texture* x7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_006.png" );
	Texture* x8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_007.png" );
	Texture* x9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_008.png" );
	Texture* x10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_009.png" );
	Texture* x11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_010.png" );
	Texture* x12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Characters/Fallen_Angels_3/PNG/PNGSequences/Slashing/0_Fallen_Angels_Slashing_011.png" );

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

	

}

void Game::CreateMeleeHeros( Faction faction , Upgrade upg , Lanes lane )
{
	Hero* hero = new Hero( this , Vec2(50.f,50.f) , faction );
	m_Heros.push_back( hero );

	Hero* h1 = new Hero( this , Vec2( 60.f , 50.f ) , faction );
	h1->m_upgrade = UPGRADE_SMALL;

	m_Heros.push_back( h1 );

	Hero* h2 = new Hero( this , Vec2( 70.f , 50.f ) , faction );
	h2->m_upgrade = UPGRADE_MEDIUM;

	m_Heros.push_back( h2 );
}
