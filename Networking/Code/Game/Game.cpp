#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/SupportPlayer.hpp"
#include "Game/MonsterAI.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ThirdParty/ImGuiSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/UDPListner.hpp"
//#include "Engine/ThirdParty/IMGUI/imgui.h"

extern ImGuiSystem* g_theGUI;


bool Help( EventArgs& args )
{
	Rgba8 color1 = Rgba8( 0 , 0 , 100 , 255 );
	Rgba8 color2 = Rgba8( 20 , 89 , 12, 255 );
	Rgba8 color3 = Rgba8( 0 , 200 , 0 , 200 );
	

	g_theConsole.PrintString( color1 , "Currently Supported Commands" );
	g_theConsole.PrintString( color2,"StartServer" );
	g_theConsole.PrintString( color3 , "Usage: <StartServer>:<portNum=number>" );
	g_theConsole.PrintString( color2 , "Connect" );
	g_theConsole.PrintString( color3 , "Usage: <Connect>:<address=address>" );
	g_theConsole.PrintString( color2 , "SendServerMessage" );
	g_theConsole.PrintString( color3 , "Usage: <SendServerMessage>:<msg=message>" );
	g_theConsole.PrintString( color2 , "SendClientMessage" );
	g_theConsole.PrintString( color3 , "Usage:<SendClientMessage>:<msg=message>" );
	g_theConsole.PrintString( color2 , "StopServer" );
	g_theConsole.PrintString( color3 , "Usage: <StopServer>" );
	g_theConsole.PrintString( color2 , "Disconnect" );
	g_theConsole.PrintString( color3 , "Usage: <Disconnect>" );
	g_theConsole.PrintString( color2 , "StartUDPPort" );
	g_theConsole.PrintString( color3 , "Usage: <StartUDPPort>:<bindPort=number>|<sendPort=number>" );
	g_theConsole.PrintString( color2 , "SendUDPMessage" );
	g_theConsole.PrintString( color3 , "Usage:<SendUDPMessage>:<msg=message>" );
	g_theConsole.PrintString( color2 , "CloseUDPPort" );
	g_theConsole.PrintString( color3 , "Usage:<CloseUDPPort>:<portNum=number>" );

	return false;
}


Game::Game()
{

	g_theEventSystem.SubscribeToEvent( "help" , Help );
	
	LoadPlayerTextures();
	LoadSupportPlayerTextures();

	m_gameCamera = new Camera();
	m_gameCamera->SetOrthoView( Vec2(0.f,0.f) , Vec2((float)m_mapSize.x,(float)m_mapSize.y) );
	m_gameCamera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 80.f ) );

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 1.f );

	m_player = new Player( this );
	m_supportPlayer = new SupportPlayer( this );

	PopulateTiles();
	LoadAIAnimations();

	m_sandMainTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/sand/sand_0012_tile.png" );
	m_borderLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Grass_tiles/grass_tiles_0006_Layer-49.png" );
	m_borderRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Grass_tiles/grass_tiles_0002_Layer-53.png" );
	m_borderTopTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Grass_tiles/grass_tiles_0003_Layer-52.png" );
	m_borderBotTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Grass_tiles/grass_tiles_0001_Layer-54.png" );

	m_blockTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Asphalt_tiles/asphalt_tiles_0012_Layer-0.png" );
	
	MonsterAI* m1 = new MonsterAI( this , TYPE_1 , nullptr , m_aiAnimWalk1 , m_aiAnimMeleeAttack1 );
	m1->m_position = Vec2( 10.f , 10.f );
	m_enemies.push_back( m1 );

}

void Game::LoadAIAnimations()
{
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/SpriteSheets/Monster1/Walk.png" );
	SpriteSheet* a2 = new SpriteSheet( *a1 , IntVec2( 9 , 1 ) );
	m_aiAnimWalk1 = new SpriteAnimDefinition( *a2 , 0 , 8 , 1.f );

	Texture* aa1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/SpriteSheets/Monster1/attack.png" );
	SpriteSheet* aa2 = new SpriteSheet( *aa1 , IntVec2( 9 , 1 ) );
	m_aiAnimMeleeAttack1 = new SpriteAnimDefinition( *aa2 , 0 , 8 , 1.f );

	

	
}

Game::~Game()
{

}

void Game::Update( float deltaseconds )
{

	ToggleDevConsole();
	UpdateMousePosition();



	if ( g_theConsole.IsOpen() )
	{
		return;
	}

	ToggleCameraUpdate();
	TogglePlayers();

	m_player->Update( deltaseconds );
	m_supportPlayer->Update( deltaseconds );

	for ( int i = 0; i < m_enemies.size(); i++ )
	{
		if ( m_enemies[ i ] == nullptr )
		{
			continue;
		}

		m_enemies[ i ]->Update(deltaseconds);
	}

	HandleBlockCollissions( m_player );
	HandleBlockCollissions( m_supportPlayer );
	UpdateCamera();
	m_time += deltaseconds;

}

void Game::Render()
{
	g_theRenderer->BeginCamera( *m_gameCamera );
	

	g_theRenderer->BindTexture( m_sandMainTex );
	g_theRenderer->DrawVertexArray( m_sandTiles );
	g_theRenderer->BindTexture( m_borderLeftTex );
	g_theRenderer->DrawVertexArray( m_sandLeftTiles );
	g_theRenderer->BindTexture( m_borderRightTex );
	g_theRenderer->DrawVertexArray( m_sandRightTiles );
	g_theRenderer->BindTexture( m_borderTopTex );
	g_theRenderer->DrawVertexArray( m_sandTopTiles );
	g_theRenderer->BindTexture( m_borderBotTex );
	g_theRenderer->DrawVertexArray( m_sandDownTiles );
	g_theRenderer->BindTexture( m_blockTex );
	g_theRenderer->DrawVertexArray( m_blockTiles );
	g_theRenderer->BindTexture( nullptr );

	
	m_player->Render();
	m_supportPlayer->Render();

	for ( int i = 0; i < m_enemies.size(); i++ )
	{
		if ( m_enemies[ i ] == nullptr )
		{
			continue;
		}
		m_enemies[ i ]->Render();
	}

	g_theRenderer->EndCamera( *m_gameCamera );

	//g_theGUI->Render();

	if ( g_theConsole.IsOpen() )
	{
		g_theConsole.Render( *g_theRenderer , *m_devConsoleCamera , 2.f , 1.f );
	}

}

void Game::UpdateMousePosition()
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	m_mousePosition.x = RangeMapFloat( 0.f , 1.f , m_gameCamera->GetOrthoBottomLeft().x , m_gameCamera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	m_mousePosition.y = RangeMapFloat( 0.f , 1.f , m_gameCamera->GetOrthoBottomLeft().y , m_gameCamera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

}

void Game::TogglePlayers()
{
	if ( g_theInput->WasKeyJustPressed( 'Q' ) )
	{
		m_mainPlayerActive = !m_mainPlayerActive;
	}

	m_player->m_isActive = m_mainPlayerActive;
	m_supportPlayer->m_isActive = !m_mainPlayerActive;
}

void Game::PopulateTiles()
{
	int numTiles = m_mapSize.x * m_mapSize.y;
	for ( int index = 0; index < numTiles; index++ )
	{
		int tileX = index % m_mapSize.x;
		int tileY = index / m_mapSize.x;

		Tile newTile = Tile( tileX , tileY , TILE_TYPE_SAND );
		newTile.m_textureType = TILE_SAND_MAIN;
		m_tiles.push_back( newTile );
	}

	for ( int i = 0; i < m_mapSize.y; i++ )
	{
		IntVec2 coords = IntVec2( 0 , i );
		int index = GetTileIndexForTileCoords( coords );

		m_tiles[ index ].m_textureType = TILE_SAND_LEFT;
		m_tiles[ index ].m_isSolid = true;

		coords = IntVec2( m_mapSize.x - 1 , i );
		index = GetTileIndexForTileCoords( coords );

		m_tiles[ index ].m_textureType = TILE_SAND_RIGHT;
		m_tiles[ index ].m_isSolid = true;
	}

	for ( int i = 0; i < m_mapSize.x; i++ )
	{
		IntVec2 coords = IntVec2( i , 0 );
		int index = GetTileIndexForTileCoords( coords );

		m_tiles[ index ].m_textureType = TILE_SAND_BOT;
		m_tiles[ index ].m_isSolid = true;

		coords = IntVec2( i , m_mapSize.y - 1 );
		index = GetTileIndexForTileCoords( coords );

		m_tiles[ index ].m_textureType = TILE_SAND_TOP;
		m_tiles[ index ].m_isSolid = true;
	}

	MapFillUsingWorms( TILE_BLOCK );

	//m_tiles[ 2 ].m_textureType = TILE_INVALID;

	for ( int vIndex = 0; vIndex < m_mapSize.y; vIndex++ )
	{
		for ( int hIndex = 0; hIndex < m_mapSize.x; hIndex++ )
		{
			int index = vIndex * m_mapSize.x + hIndex;
			AABB2 aabb = AABB2( ( float ) hIndex , ( float ) vIndex , ( float ) hIndex + 1 , ( float ) vIndex + 1 );

			switch ( m_tiles[index].m_textureType )
			{
			case TILE_SAND_MAIN:
			{
				AppendAABB2( m_sandTiles , aabb , Rgba8( 255 , 255 , 255 , 255 ) );
				break;
			}
			case  TILE_SAND_LEFT:
			{
				AppendAABB2( m_sandLeftTiles , aabb , Rgba8( 255 , 255 , 255 , 255 ) );
				break;
			}
			case  TILE_SAND_RIGHT:
			{
				AppendAABB2( m_sandRightTiles , aabb , Rgba8( 255 , 255 , 255 , 255 ) );
				break;
			}
			case  TILE_SAND_BOT:
			{
				AppendAABB2( m_sandDownTiles , aabb , Rgba8( 255 , 255 , 255 , 255 ) );
				break;
			}
			case  TILE_SAND_TOP:
			{
				AppendAABB2( m_sandTopTiles , aabb , Rgba8( 255 , 255 , 255 , 255 ) );
				break;
			}
			case TILE_BLOCK:
			{
				AppendAABB2( m_blockTiles , aabb , Rgba8(255,255,255,255) );
				break;
			}
			default:
				AppendAABB2( sampleTile , aabb , Rgba8( 255 , 255 , 255 , 255 ) );
				break;
			}
		}
	}

}

void Game::HandleBlockCollissions( Entity* entity )
{
	
	Vec2 player_position = entity->m_position;

	IntVec2 current_tileCoords = IntVec2( RoundDownToInt( player_position.x ) , RoundDownToInt( player_position.y ) );

	IntVec2 leftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y );
	IntVec2 rightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y );
	IntVec2 upCoords = IntVec2( current_tileCoords.x , current_tileCoords.y + 1 );
	IntVec2 downCoords = IntVec2( current_tileCoords.x , current_tileCoords.y - 1 );

	IntVec2 topLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y + 1 );
	IntVec2 bottomLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y - 1 );
	IntVec2 topRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y + 1 );
	IntVec2 bottomRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y - 1 );

	AABB2 leftAABB = AABB2( ( float ) leftCoords.x , ( float ) leftCoords.y , ( float ) leftCoords.x + 1 , ( float ) leftCoords.y + 1 );
	AABB2 rightAABB = AABB2( ( float ) rightCoords.x , ( float ) rightCoords.y , ( float ) rightCoords.x + 1 , ( float ) rightCoords.y + 1 );
	AABB2 upAABB = AABB2( ( float ) upCoords.x , ( float ) upCoords.y , ( float ) upCoords.x + 1 , ( float ) upCoords.y + 1 );
	AABB2 downAABB = AABB2( ( float ) downCoords.x , ( float ) downCoords.y , ( float ) downCoords.x + 1 , ( float ) downCoords.y + 1 );

	AABB2 topLeftAABB = AABB2( ( float ) topLeftCoords.x , ( float ) topLeftCoords.y , ( float ) topLeftCoords.x + 1 , ( float ) topLeftCoords.y + 1 );
	AABB2 bottomRightAABB = AABB2( ( float ) bottomRightCoords.x , ( float ) bottomRightCoords.y , ( float ) bottomRightCoords.x + 1 , ( float ) bottomRightCoords.y + 1 );
	AABB2 topRightAABB = AABB2( ( float ) topRightCoords.x , ( float ) topRightCoords.y , ( float ) topRightCoords.x + 1 , ( float ) topRightCoords.y + 1 );
	AABB2 bottomLeftAABB = AABB2( ( float ) bottomLeftCoords.x , ( float ) bottomLeftCoords.y , ( float ) bottomLeftCoords.x + 1 , ( float ) bottomLeftCoords.y + 1 );


	if ( IsTileSolid( leftCoords ) )
	{
		PushDiscOutOFAABB2D( player_position , .4f , leftAABB );
	}
	if ( IsTileSolid( rightCoords ) )
	{
		PushDiscOutOFAABB2D( player_position , .4f , rightAABB );
	}
	if ( IsTileSolid( upCoords ) )
	{
		PushDiscOutOFAABB2D( player_position , .4f , upAABB );
	}
	if ( IsTileSolid( downCoords ) )
	{
		PushDiscOutOFAABB2D( player_position , .4f , downAABB );
	}
	if ( IsTileSolid( topRightCoords ) )
	{
		PushDiscOutOFAABB2D( player_position , .4f , topRightAABB );
	}
	if ( IsTileSolid( topLeftCoords ) )
	{
		PushDiscOutOFAABB2D( player_position , .4f , topLeftAABB );
	}
	if ( IsTileSolid( bottomRightCoords ) )
	{
		PushDiscOutOFAABB2D( player_position , .4f , bottomRightAABB );
	}
	if ( IsTileSolid( bottomLeftCoords ) )
	{
		PushDiscOutOFAABB2D( player_position , .4f , bottomLeftAABB );
	}

	entity->m_position = player_position;
	
}

bool Game::IsTileSolid( IntVec2 tileCoords )
{
	int index = GetTileIndexForTileCoords( tileCoords );
	return m_tiles[ index ].m_isSolid;
}

void Game::StartListner( int bindPort , int sendPort )
{
	listner = new UDPListner();

	listner->StartSocket( bindPort , sendPort );

	g_theConsole.PrintString( Rgba8() , "Listner Started" );
}

void Game::ToggleDevConsole()
{
	if ( g_theInput->WasKeyJustPressed( 0xC0 ) )
	{
		devConsoleOpen = !devConsoleOpen;
		g_theConsole.SetIsOpen( devConsoleOpen );
	}

}

int Game::GetTileIndexForTileCoords( const IntVec2& tileCoords )
{
	return tileCoords.x + ( m_mapSize.x * tileCoords.y );
}

void Game::MapFillUsingWorms( TileTextureType type, int minWormLenth, int maxWormLength, int maxWorms )
{
	RandomNumberGenerator rng = RandomNumberGenerator();
	int numberOfWorms = maxWorms;
	for ( int currentWormNum = 0; currentWormNum < numberOfWorms; currentWormNum++ )
	{
		IntVec2 tileCoords;
		tileCoords.x = rng.GetRandomIntInRange( 2 , m_mapSize.x - 2 );
		tileCoords.y = rng.GetRandomIntInRange( 2 , m_mapSize.y - 2 );
		int wormLength = rng.GetRandomIntInRange( minWormLenth , maxWormLength );


		for ( int wormCurrentLength = 0; wormCurrentLength < wormLength; wormCurrentLength++ )
		{
			
			int index = GetTileIndexForTileCoords( tileCoords );
			m_tiles[ index ].m_textureType = type;
			m_tiles[ index ].m_isSolid = true;

			int nextTimeMovement = rng.GetRandomIntInRange( 0 , 3 );

			if ( nextTimeMovement == 0 )
			{
				if ( tileCoords.x + 1 < m_mapSize.x-2 )
					tileCoords.x += 1;
			}
			else if ( nextTimeMovement == 1 )
			{
				if ( tileCoords.x - 1 < m_mapSize.x-2 )
					tileCoords.x -= 1;
			}
			else if ( nextTimeMovement == 2 )
			{
				if ( tileCoords.y + 1 < m_mapSize.y-2 )
					tileCoords.y += 1;
			}
			else
			{
				if ( tileCoords.y - 1 < m_mapSize.y-2 )
					tileCoords.y -= 1;
			}
		}
	}
}

void Game::LoadPlayerTextures()
{
	std::vector<Texture*> player1IdleKnifeTextures;

	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_knife/Idle_knife_000.png" );
	Texture* t2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_knife/Idle_knife_001.png" );
	Texture* t3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_knife/Idle_knife_002.png" );
	Texture* t4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_knife/Idle_knife_003.png" );
	Texture* t5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_knife/Idle_knife_004.png" );
	Texture* t6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_knife/Idle_knife_005.png" );
	Texture* t7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_knife/Idle_knife_006.png" );
	Texture* t8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_knife/Idle_knife_007.png" );

	player1IdleKnifeTextures.push_back( t1 );
	player1IdleKnifeTextures.push_back( t2 );
	player1IdleKnifeTextures.push_back( t3 );
	player1IdleKnifeTextures.push_back( t4 );
	player1IdleKnifeTextures.push_back( t5 );
	player1IdleKnifeTextures.push_back( t6 );
	player1IdleKnifeTextures.push_back( t7 );
	player1IdleKnifeTextures.push_back( t8 );

	m_player1IdleMeleeTex = new SpriteAnimDefTex(0,player1IdleKnifeTextures.size()-1,1.f);
	m_player1IdleMeleeTex->m_animations = player1IdleKnifeTextures;

	std::vector<Texture*> player1WalkKnifeTextures;

	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_knife/Walk_knife_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_knife/Walk_knife_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_knife/Walk_knife_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_knife/Walk_knife_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_knife/Walk_knife_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_knife/Walk_knife_005.png" );

	player1WalkKnifeTextures.push_back( a1 );
	player1WalkKnifeTextures.push_back( a2 );
	player1WalkKnifeTextures.push_back( a3 );
	player1WalkKnifeTextures.push_back( a4 );
	player1WalkKnifeTextures.push_back( a5 );
	player1WalkKnifeTextures.push_back( a6 );

	m_player1WalkMeleeTex = new SpriteAnimDefTex( 0 , player1WalkKnifeTextures.size() - 1 , 1.f );
	m_player1WalkMeleeTex->m_animations = player1WalkKnifeTextures;

	std::vector<Texture*> player1AttackTextures;

	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Knife/Knife_000.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Knife/Knife_001.png" );
	Texture* b3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Knife/Knife_002.png" );
	Texture* b4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Knife/Knife_003.png" );
	Texture* b5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Knife/Knife_004.png" );
	Texture* b6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Knife/Knife_005.png" );
	Texture* b7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Knife/Knife_006.png" );
	Texture* b8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Knife/Knife_007.png" );

	player1AttackTextures.push_back( b1 );
	player1AttackTextures.push_back( b2 );
	player1AttackTextures.push_back( b3 );
	player1AttackTextures.push_back( b4 );
	player1AttackTextures.push_back( b5 );
	player1AttackTextures.push_back( b6 );
	player1AttackTextures.push_back( b7 );
	player1AttackTextures.push_back( b8 );

	m_player1AttackTex = new SpriteAnimDefTex( 0 , player1AttackTextures.size() - 1 , 1.f );
	m_player1AttackTex->m_animations = player1AttackTextures;

	std::vector<Texture*> playerIdleGunTextures;

	Texture* c1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_gun/Idle_gun_000.png" );
	Texture* c2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_gun/Idle_gun_001.png" );
	Texture* c3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_gun/Idle_gun_002.png" );
	Texture* c4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_gun/Idle_gun_003.png" );
	Texture* c5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_gun/Idle_gun_004.png" );
	Texture* c6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_gun/Idle_gun_005.png" );
	Texture* c7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_gun/Idle_gun_006.png" );
	Texture* c8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Idle_gun/Idle_gun_007.png" );

	playerIdleGunTextures.push_back( c1 );
	playerIdleGunTextures.push_back( c2 );
	playerIdleGunTextures.push_back( c3 );
	playerIdleGunTextures.push_back( c4 );
	playerIdleGunTextures.push_back( c5 );
	playerIdleGunTextures.push_back( c6 );
	playerIdleGunTextures.push_back( c7 );
	playerIdleGunTextures.push_back( c8 );

	m_player1IdleGunTex = new SpriteAnimDefTex( 0 , playerIdleGunTextures.size() - 1 , 1.f );
	m_player1IdleGunTex->m_animations = playerIdleGunTextures;

	std::vector<Texture*> playerWalkGunTextures;

	Texture* d1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_gun/Walk_gun_000.png" );
	Texture* d2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_gun/Walk_gun_001.png" );
	Texture* d3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_gun/Walk_gun_002.png" );
	Texture* d4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_gun/Walk_gun_003.png" );
	Texture* d5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_gun/Walk_gun_004.png" );
	Texture* d6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Walk_gun/Walk_gun_005.png" );

	playerWalkGunTextures.push_back( d1 );
	playerWalkGunTextures.push_back( d2 );
	playerWalkGunTextures.push_back( d3 );
	playerWalkGunTextures.push_back( d4 );
	playerWalkGunTextures.push_back( d5 );
	playerWalkGunTextures.push_back( d6 );

	m_player1WalkGunTex = new SpriteAnimDefTex( 0 , playerWalkGunTextures.size() - 1 , 1.f );
	m_player1WalkGunTex->m_animations = playerWalkGunTextures;

	std::vector<Texture*> playerAttackGunTextures;

	Texture* g1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Gun_Shot/Gun_Shot_000.png" );
	Texture* g2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Gun_Shot/Gun_Shot_001.png" );
	Texture* g3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Gun_Shot/Gun_Shot_002.png" );
	Texture* g4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Gun_Shot/Gun_Shot_003.png" );
	Texture* g5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Girl/Gun_Shot/Gun_Shot_004.png" );

	playerAttackGunTextures.push_back( g1 );
	playerAttackGunTextures.push_back( g2 );
	playerAttackGunTextures.push_back( g3 );
	playerAttackGunTextures.push_back( g4 );
	playerAttackGunTextures.push_back( g5 );

	m_player1AttackGunTex = new SpriteAnimDefTex( 0 , playerAttackGunTextures.size() - 1 , 1.f );
	m_player1AttackGunTex->m_animations = playerAttackGunTextures;

}

void Game::LoadSupportPlayerTextures()
{
	std::vector<Texture*> playerIdleTextures;

	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Idle_bat/Idle_Bat_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Idle_bat/Idle_Bat_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Idle_bat/Idle_Bat_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Idle_bat/Idle_Bat_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Idle_bat/Idle_Bat_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Idle_bat/Idle_Bat_005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Idle_bat/Idle_Bat_006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Idle_bat/Idle_Bat_007.png" );

	playerIdleTextures.push_back( a1 );
	playerIdleTextures.push_back( a2 );
	playerIdleTextures.push_back( a3 );
	playerIdleTextures.push_back( a4 );
	playerIdleTextures.push_back( a5 );
	playerIdleTextures.push_back( a6 );
	playerIdleTextures.push_back( a7 );
	playerIdleTextures.push_back( a8 );

	m_supportPlayerIdleTex = new SpriteAnimDefTex( 0 , playerIdleTextures.size() - 1 , 1.f );
	m_supportPlayerIdleTex->m_animations = playerIdleTextures;

	std::vector<Texture*> playerAttackTextures;

	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_000.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_001.png" );
	Texture* b3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_002.png" );
	Texture* b4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_003.png" );
	Texture* b5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_004.png" );
	Texture* b6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_005.png" );
	Texture* b7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_006.png" );
	Texture* b8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_007.png" );
	Texture* b9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_008.png" );
	Texture* b10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_009.png" );
	Texture* b11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_010.png" );
	Texture* b12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Bat/Bat_011.png" );

	playerAttackTextures.push_back( b1 );
	playerAttackTextures.push_back( b2 );
	playerAttackTextures.push_back( b3 );
	playerAttackTextures.push_back( b4 );
	playerAttackTextures.push_back( b5 );
	playerAttackTextures.push_back( b6 );
	playerAttackTextures.push_back( b7 );
	playerAttackTextures.push_back( b8 );
	playerAttackTextures.push_back( b9 );
	playerAttackTextures.push_back( b10 );
	playerAttackTextures.push_back( b11 );
	playerAttackTextures.push_back( b12 );

	m_supportPlayerAttackTex = new SpriteAnimDefTex( 0 , playerAttackTextures.size() - 1 , 1.f );
	m_supportPlayerAttackTex->m_animations = playerAttackTextures;

	std::vector<Texture*> playerWalkTextures;

	Texture* c1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Walk_bat/Walk_bat_000.png" );
	Texture* c2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Walk_bat/Walk_bat_001.png" );
	Texture* c3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Walk_bat/Walk_bat_002.png" );
	Texture* c4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Walk_bat/Walk_bat_003.png" );
	Texture* c5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Walk_bat/Walk_bat_004.png" );
	Texture* c6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/MainCharacters/Boy/Walk_bat/Walk_bat_005.png" );

	playerWalkTextures.push_back( c1 );
	playerWalkTextures.push_back( c2 );
	playerWalkTextures.push_back( c3 );
	playerWalkTextures.push_back( c4 );
	playerWalkTextures.push_back( c5 );
	playerWalkTextures.push_back( c6 );

	m_supportPlayerWalkTex = new SpriteAnimDefTex( 0 , playerWalkTextures.size() - 1 , 1.f );
	m_supportPlayerWalkTex->m_animations = playerWalkTextures;

	std::vector<Texture*> disguiseIdleTex;

	Texture* d1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Walk/walk_000.png" );
	Texture* d2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Walk/walk_001.png" );
	Texture* d3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Walk/walk_002.png" );
	Texture* d4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Walk/walk_003.png" );
	Texture* d5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Walk/walk_004.png" );
	Texture* d6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Walk/walk_005.png" );
	Texture* d7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Walk/walk_006.png" );
	Texture* d8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Walk/walk_007.png" );
	Texture* d9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Walk/walk_008.png" );

	disguiseIdleTex.push_back( d1 );
	disguiseIdleTex.push_back( d2 );
	disguiseIdleTex.push_back( d3 );
	disguiseIdleTex.push_back( d4 );
	disguiseIdleTex.push_back( d5 );
	disguiseIdleTex.push_back( d6 );
	disguiseIdleTex.push_back( d7 );
	disguiseIdleTex.push_back( d8 );
	disguiseIdleTex.push_back( d9 );

	m_supportPlayerDisguiseWalk = new SpriteAnimDefTex( 0 , disguiseIdleTex.size() - 1 , 1.f );
	m_supportPlayerDisguiseWalk->m_animations = disguiseIdleTex;

	std::vector<Texture*> disguiseAttackTex;

	Texture* g1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Attack/attack_000.png" );
	Texture* g2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Attack/attack_001.png" );
	Texture* g3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Attack/attack_002.png" );
	Texture* g4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Attack/attack_003.png" );
	Texture* g5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Attack/attack_004.png" );
	Texture* g6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Attack/attack_005.png" );
	Texture* g7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Attack/attack_006.png" );
	Texture* g8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Attack/attack_007.png" );
	Texture* g9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/2LVL/Army_zombie/Attack/attack_008.png" );

	disguiseAttackTex.push_back( g1 );
	disguiseAttackTex.push_back( g2 );
	disguiseAttackTex.push_back( g3 );
	disguiseAttackTex.push_back( g4 );
	disguiseAttackTex.push_back( g5 );
	disguiseAttackTex.push_back( g6 );
	disguiseAttackTex.push_back( g7 );
	disguiseAttackTex.push_back( g8 );
	disguiseAttackTex.push_back( g9 );
	
	m_supportPlayerDisguiseAttack = new SpriteAnimDefTex( 0 , disguiseAttackTex.size() - 1 , 1.f );
	m_supportPlayerDisguiseAttack->m_animations = disguiseAttackTex;
}



void Game::UpdateCamera()
{
	Vec2 camCoords;
	float xFactor = 8.f*0.55f;
	float yFactor = 4.5f*0.55f;

	if ( m_cameraUpdates )
	{
		if ( m_player->m_isActive )
		{
			camCoords.x = Clamp( m_player->m_position.x , xFactor * 1.75f , m_mapSize.x - ( xFactor * 1.75f ) );
			camCoords.y = Clamp( m_player->m_position.y , yFactor * 1.75f , m_mapSize.y - ( yFactor * 1.75f ) );

			m_gameCamera->SetOrthoView( camCoords - Vec2( xFactor * 1.75f ,yFactor * 1.75f ) , camCoords + Vec2( xFactor * 1.75f , yFactor * 1.75f ) );
		}
		else
		{
			camCoords.x = Clamp( m_supportPlayer->m_position.x , xFactor * 1.75f , m_mapSize.x - ( xFactor * 1.75f ) );
			camCoords.y = Clamp( m_supportPlayer->m_position.y , yFactor * 1.75f , m_mapSize.y - ( yFactor * 1.75f ) );

			m_gameCamera->SetOrthoView( camCoords - Vec2( xFactor * 1.75f , yFactor * 1.75f ) , camCoords + Vec2( xFactor * 1.75f , yFactor * 1.75f ) );
		}
	}

	else
	{
		m_gameCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( ( float ) m_mapSize.x , ( float ) m_mapSize.y ) );
	}
}

void Game::ToggleCameraUpdate()
{
	if ( g_theInput->WasKeyJustPressed( F1 ) )
	{
		m_cameraUpdates = !m_cameraUpdates;
	}
}
