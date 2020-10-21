#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/SupportPlayer.hpp"
#include "Game/MonsterAI.hpp"
#include "Game/PotentialFields.hpp"
#include "Game/Bomb.hpp"
#include "Game/Turret.hpp"
#include "Game/PotentialFieldCreator.hpp"
#include "Game/MapCreator.hpp"
#include "Game/StartScreen.hpp"
#include "Game/GreenBeret.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ThirdParty/ImGuiSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/OBB2.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/Timer.hpp"
//#include "Engine/ThirdParty/IMGUI/imgui.h"

extern ImGuiSystem* g_theGUI;


bool Help( EventArgs& args )
{
	UNUSED( args );
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

	return false;
}


Game::Game()
{
	
	LoadPlayerTextures();
	LoadSupportPlayerTextures();

	m_gameCamera = new Camera();
	m_gameCamera->SetOrthoView( Vec2(0.f,0.f) , Vec2(160.f*0.95f,90.f*0.95f) );
	m_gameCamera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 80.f ) );

	m_hudCamera = new Camera();
	m_hudCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );
	//m_hudCamera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 1.f );

	m_player = new Player( this );
	m_player->m_position.x += 10.f;
	m_supportPlayer = new SupportPlayer( this );

	m_rng = new RandomNumberGenerator();

	PopulateTiles();
	LoadAIAnimations();
	LoadBombAnimations();

	m_sandMainTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/ground/ground_0004_tile.png" );
	m_borderLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Grass_tiles/grass_tiles_0006_Layer-49.png" );
	m_borderRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Grass_tiles/grass_tiles_0002_Layer-53.png" );
	m_borderTopTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Grass_tiles/grass_tiles_0003_Layer-52.png" );
	m_borderBotTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Grass_tiles/grass_tiles_0001_Layer-54.png" );

	m_blockTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Asphalt_tiles/asphalt_tiles_0012_Layer-0.png" );
	m_player1HudTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/girl icon_no_bg.png" );
	m_player2HudTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/man icon_no_bg.png" );

	m_HudBoxTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/BARRA01.png" );
	m_PlayerBoxTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/MARC0000.png" );

	m_turretTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Items/items_0009_magazine_machine_gun.png" );

	MonsterAI* m1 = new MonsterAI( this , TYPE_1 , nullptr , m_aiAnimWalk1 , m_aiAnimMeleeAttack1 );
	m1->m_position = Vec2( 10.f , 10.f );
	m1->m_nextMovePosition = Vec2( 10.f , 10.f );
	m_enemies.push_back( m1 );
	m_startScreen = new StartScreen(this);
	//std::vector<int> path;
	//GetPathUsingAStarIgnoreDiagonalMoves( Vec2( 2.2f , 2.2f ) , Vec2( 33.f , 40.f ) , path );

	Bomb* b = new Bomb( this , Vec2( 20.f , 10.f ) , m_bombIdleTex , m_explosion );
	m_bombs.push_back( b );

	Turret* t = new Turret( this , Vec2( 25.f , 10.f ) , m_turretTex, Vec2(-1.f,0.f) );
	m_turrets.push_back( t );

	m_hudBox = AABB2( Vec2(0.f,0.f) , Vec2(160.f,90.f) );
	m_hudBox.CarveBoxOffTop( 0.1f );

	//m_hudBox = m_hudBox.GetPercentageOfAABB( 0.9f );

	m_player1Box = m_hudBox;
	m_player1Box = m_player1Box.GetPercentageOfAABB( 0.9f );
	m_player1Box.CarveBoxOffLeft( 0.055f );

	m_player2Box = m_player1Box;
	m_player2Box.Translate( Vec2(10.f,0.f) );

	m_player1Box.Translate( Vec2( -5.f , 0.f ) );

	m_playerBox = m_player1Box;

	m_player1Box = m_player1Box.GetPercentageOfAABB( 0.6f );
	m_player1Box.mins.x -= 1.3f;
	m_player1Box.maxs.x -= 1.f;
	m_player1Box.mins.y -= 1.f;
	m_player1Box.maxs.y += 1.25f;

	m_playerBox.Translate( Vec2( 5.f , 0.f ) );
	m_player1Box.Translate( Vec2( 5.f , 0.f ) );

	m_greenBeret = new GreenBeret( this );

	m_greenBeret->m_position = Vec2( 10.f , 10.f );

	LoadDataFromXml();

	m_mapTex1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Map/MpFinal.png" );

	m_mapCretor = new MapCreator( this );
	m_potCreator = new PotentialFieldCreator();
	m_potentialField = new PotentialField( 0.f , 60.f , Vec2( 0.f , 0.f ) );

	LoadPotentialFieldFromXML();
	LoadPlayerHudAnimations();

	m_greenBeretHUDTimer = new Timer();
	m_greenBeretHUDTimer->SetSeconds( 0.5f );

}

void Game::LoadAIAnimations()
{
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/SpriteSheets/Monster1/Walk.png" );
	SpriteSheet* a2 = new SpriteSheet( *a1 , IntVec2( 9 , 1 ) );
	m_aiAnimWalk1 = new SpriteAnimDefinition( *a2 , 0 , 8 , 1.f );

	Texture* aa1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/SpriteSheets/Monster1/attack.png" );
	SpriteSheet* aa2 = new SpriteSheet( *aa1 , IntVec2( 9 , 1 ) );
	m_aiAnimMeleeAttack1 = new SpriteAnimDefinition( *aa2 , 0 , 8 , 1.f );

	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/SpriteSheets/Monster2/Walk.png" );
	SpriteSheet* b2 = new SpriteSheet( *b1 , IntVec2( 9 , 1 ) );
	m_aiAnimWalk2 = new SpriteAnimDefinition( *b2 , 0 , 8 , 1.f );

	Texture* bb1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/SpriteSheets/Monster2/attack.png" );
	SpriteSheet* bb2 = new SpriteSheet( *bb1 , IntVec2( 9 , 1 ) );
	m_aiAnimMeleeAttack2 = new SpriteAnimDefinition( *bb2 , 0 , 8 , 1.f );

	

	
}

void Game::LoadBombAnimations()
{
	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Explosions/BombIdle.png" );
	SpriteSheet* t2 = new SpriteSheet( *t1 , IntVec2( 9 , 1 ) );
	m_bombIdleTex = new SpriteAnimDefinition( *t2 , 0 , 8 , 1.f );

	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Explosions/BombExplosion.png" );
	SpriteSheet* a2 = new SpriteSheet( *a1 , IntVec2( 9 , 1 ) );
	m_explosion = new SpriteAnimDefinition( *a2 , 0 , 8 , 1.f );
}

void Game::LoadPlayerHudAnimations()
{
	std::vector<Texture*> greenBeretActiveTex;
	
	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/GreenBeret/COM0006.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/GreenBeret/COM0007.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/GreenBeret/COM0008.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/GreenBeret/COM0009.png" );

	greenBeretActiveTex.push_back( a1 );
	greenBeretActiveTex.push_back( a2 );
	greenBeretActiveTex.push_back( a3 );
	greenBeretActiveTex.push_back( a4 );

	m_greenBeretActiveAnims = new SpriteAnimDefTex( 0 , greenBeretActiveTex.size() - 1 , 0.5f );
	m_greenBeretActiveAnims->m_animations = greenBeretActiveTex;


	std::vector<Texture*> greenBeretDeactiveTex;

	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/GreenBeret/COM0001.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/GreenBeret/COM0002.png" );
	Texture* b3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/GreenBeret/COM0003.png" );
	Texture* b4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/UI/GreenBeret/COM0004.png" );

	greenBeretDeactiveTex.push_back( b1 );
	greenBeretDeactiveTex.push_back( b2 );
	greenBeretDeactiveTex.push_back( b3 );
	greenBeretDeactiveTex.push_back( b4 );

	m_greenBeretDeactiveAnims = new SpriteAnimDefTex( 0 , greenBeretDeactiveTex.size() - 1 , 0.5f );
	m_greenBeretDeactiveAnims->m_animations = greenBeretDeactiveTex;

}

void Game::UpdatePathFinderMode()
{
	if ( g_theInput->WasKeyJustPressed( SPACE ) )
	{
		RefreshPathFinderMode();
	}

	if ( g_theInput->WasKeyJustPressed( 'C' ) )
	{
		IntVec2 pos = IntVec2( RoundDownToInt( m_mousePosition.x ) , RoundDownToInt( m_mousePosition.y ) );
		CreateInfluenceMap( pos , influenceMapPositive , 10 );
	}

	if ( !StartLocationSet )
	{
		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			startLocation = m_mousePosition;
			StartLocationSet = true;
			return;
		}
	}

	if ( !endLocationSet )
	{
		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			endLocation = m_mousePosition;
			endLocationSet = true;
			return;
		}
	}

	if ( StartLocationSet && endLocationSet )
	{
		if ( g_theInput->IsRightMouseButtonPressed() || g_theInput->WasKeyJustPressed('B'))
		{
			if ( m_currentAlgIs8WayAstar )
			{
				GetPathUsingAStarIgnoreDiagonalMovesOneStep( startLocation , endLocation , pathIndices , false , m_considerInfluenceMaps);
			}

			if ( m_currentAlgIs4WayAStar )
			{
				GetPathUsingAStarIgnoreDiagonalMovesOneStep( startLocation , endLocation , pathIndices, true, m_considerInfluenceMaps );
			}
		}

		if ( g_theInput->WasKeyJustPressed( 'T' ) )
		{
			if ( m_currentAlgIs4WayAStar )
			{
				GetPathUsingAStarIgnoreDiagonalMoves( startLocation , endLocation , pathIndices , m_considerInfluenceMaps , true );
			}
			else
			{
				GetPathUsingAStarIgnoreDiagonalMoves( startLocation , endLocation , pathIndices , m_considerInfluenceMaps , false );
			}
		}
	}
}

void Game::RefreshPathFinderMode()
{
	StartLocationSet = false;
	endLocationSet = false;
	pathIndices.clear();
	openList1.clear();
	closedList1.clear();
	pathFound = false;
	initDone = false;
}

void Game::RenderPathFinderMode()
{
	std::vector<Vertex_PCU> verts;



	int startIndex = GetTileIndexForTileCoords( IntVec2( RoundDownToInt( startLocation.x ) , RoundDownToInt( startLocation.y ) ) );
	int endIndex = GetTileIndexForTileCoords( IntVec2( RoundDownToInt( endLocation.x ) , RoundDownToInt( endLocation.y ) ) );

	for ( int i = 0; i < m_tiles.size(); i++ )
	{
		if ( m_tiles[ i ].m_influenceValue != 0 )
		{
			if ( m_tiles[ i ].m_influenceValue > 0 )
			{
				AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
				float baseAlpha = 100;
				baseAlpha += m_tiles[ i ].m_influenceValue * 40.f;
				baseAlpha =Clamp( baseAlpha , 0 , 255 );
				AppendAABB2( verts , aabb , Rgba8( 200 , 0 , 0 , (int)baseAlpha ) );
			}
			else
			{
				AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
				AppendAABB2( verts , aabb , Rgba8( 0 , 0 , 200 , 150 ) );
			}
		}
	}

	for ( int i = 0; i < pathIndices.size(); i++ )
	{
		AABB2 aabb = AABB2( ( float ) m_tiles[ pathIndices[ i ] ].m_tileCoords.x , ( float ) m_tiles[ pathIndices[ i ] ].m_tileCoords.y , ( float ) m_tiles[ pathIndices[ i ] ].m_tileCoords.x + 1 , ( float ) m_tiles[ pathIndices[ i ] ].m_tileCoords.y + 1 );
		AppendAABB2( verts , aabb , Rgba8( 0 , 0 , 100 , 100 ) );
	}

	for ( int i = 0; i < openList1.size(); i++ )
	{
		AABB2 aabb = AABB2( ( float ) m_tiles[ GetTileIndexForTileCoords( openList1[ i ].coords ) ].m_tileCoords.x , ( float ) m_tiles[ GetTileIndexForTileCoords( openList1[ i ].coords ) ].m_tileCoords.y , ( float ) m_tiles[ GetTileIndexForTileCoords( openList1[ i ].coords ) ].m_tileCoords.x + 1 , ( float ) m_tiles[ GetTileIndexForTileCoords( openList1[ i ].coords ) ].m_tileCoords.y + 1 );
		AppendAABB2( verts , aabb , Rgba8( 0 , 100 , 0 , 150 ) );
	}

	for ( int i = 0; i < closedList1.size(); i++ )
	{
		AABB2 aabb = AABB2( ( float ) m_tiles[ GetTileIndexForTileCoords( closedList1[ i ].coords ) ].m_tileCoords.x , ( float ) m_tiles[ GetTileIndexForTileCoords( closedList1[ i ].coords ) ].m_tileCoords.y , ( float ) m_tiles[ GetTileIndexForTileCoords( closedList1[ i ].coords ) ].m_tileCoords.x + 1 , ( float ) m_tiles[ GetTileIndexForTileCoords( closedList1[ i ].coords ) ].m_tileCoords.y + 1 );
		AppendAABB2( verts , aabb , Rgba8( 0 , 100 , 100 , 150 ) );
	}
	
	if ( StartLocationSet )
	{
		AABB2 s = AABB2( m_tiles[ startIndex ].m_tileCoords.x , m_tiles[ startIndex ].m_tileCoords.y , m_tiles[ startIndex ].m_tileCoords.x + 1 , m_tiles[ startIndex ].m_tileCoords.y + 1 );
		AppendAABB2( verts , s , Rgba8( 100 , 0 , 0 , 100 ) );
	}

	if ( endLocationSet )
	{
		AABB2 e = AABB2( m_tiles[ endIndex ].m_tileCoords.x , m_tiles[ endIndex ].m_tileCoords.y , m_tiles[ endIndex ].m_tileCoords.x + 1 , m_tiles[ endIndex ].m_tileCoords.y + 1 );
		AppendAABB2( verts , e , Rgba8( 100 , 0 , 0 , 100 ) );
	}

	
	g_theRenderer->BindTexture( nullptr );
	if ( verts.size() > 0 )
	{
		g_theRenderer->DrawVertexArray( verts );
	}

}

Game::~Game()
{

}

void Game::Update( float deltaseconds )
{
	
	if ( inStartScreen )
	{
		m_startScreen->Update( deltaseconds );
		return;
	}

	ToggleDevConsole();
	ToggleGameModes();
	UpdateMousePosition();
	if ( m_currentMode == PATHFINDER )
	{
		UpdatePathFinderMode();
	}

	if ( m_currentMode == MAPCREATOR )
	{
		m_mapCretor->Update( deltaseconds );
	}

	if ( m_currentMode == POTENTIALFIELD_CREATOR )
	{
		m_potCreator->Update();
	}

	/*if ( g_theConsole.IsOpen() )
	{
		return;
	}*/

	if ( m_currentMode == GAME )
	{
		ToggleCameraUpdate();
		TogglePlayers();

		//m_player->Update( deltaseconds );
		//m_supportPlayer->Update( deltaseconds );
		m_greenBeret->Update( deltaseconds );

		for ( int i = 0; i < m_enemies.size(); i++ )
		{
			if ( m_enemies[ i ] == nullptr )
			{
				continue;
			}

			m_enemies[ i ]->Update( deltaseconds );
		}

		for ( int i = 0; i < m_bombs.size(); i++ )
		{
			if ( m_bombs[ i ] == nullptr )
			{
				continue;
			}
			m_bombs[ i ]->Update(deltaseconds);
		}

		for ( int i = 0; i < m_turrets.size(); i++ )
		{
			if ( m_turrets[ i ] == nullptr )
			{
				continue;
			}

			m_turrets[ i ]->Update( deltaseconds );
		}

		//HandleBlockCollissions( m_player );
		//HandleBlockCollissions( m_supportPlayer );

		if ( g_theInput->WasRightMouseButtonJustPressed() )
		{
			m_potentialField->SetPosition( m_mousePosition );
		}
	}
	m_time += deltaseconds;

	if ( g_theInput->WasKeyJustPressed( 'K' ) )
	{
		m_potentialField->Rotate( 45.f );
	}
	UpdateCamera();


	if ( m_currentMode == PATHFINDER )
	{
		ImGui::NewFrame();
		ImGui::Begin( "PATH FINDER" );
		ImGui::Text( "Path Finding Controls" );
		ImGui::Checkbox( "4-Way A*" , &m_currentAlgIs4WayAStar );
		ImGui::Checkbox( "8-Way A*" , &m_currentAlgIs8WayAstar );
		ImGui::Checkbox( "Consider Influence Map", &m_considerInfluenceMaps );
		ImGui::Text( "Influence Map Controls" );
		ImGui::Checkbox( "Influence Map Positive" , &influenceMapPositive );
		ImGui::End();

		//ImGui::Begin( "INFLUENCE MAP" );
		//ImGui::End();
	}

}

void Game::Render()
{
	if ( inStartScreen )
	{
		m_startScreen->Render();
		return;
	}

	g_theRenderer->BeginCamera( *m_gameCamera );
	

	


	if ( m_currentMode == PATHFINDER )
	{
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
		RenderPathFinderMode();
	}

	if ( m_currentMode == GAME )
	{
	

		for ( int i = 0; i < m_bombs.size(); i++ )
		{
			if ( m_bombs[ i ] == nullptr )
			{
				continue;
			}
			m_bombs[ i ]->Render();
		}

		for ( int i = 0; i < m_turrets.size(); i++ )
		{
			if ( m_turrets[ i ] == nullptr )
			{
				continue;
			}
			m_turrets[ i ]->Render();
		}

		for ( int i = 0; i < m_enemies.size(); i++ )
		{
			if ( m_enemies[ i ] == nullptr )
			{
				continue;
			}
			m_enemies[ i ]->Render();
		}

		g_theRenderer->BindTexture( m_mapTex1 );
		g_theRenderer->DrawAABB2D( m_mapPart1 , Rgba8() );
	
		/*std::vector<Vertex_PCU> verts;
		for ( int i = 0; i < m_tiles.size(); i++ )
		{
			AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
			AppendAABB2( verts , aabb , Rgba8() );
		}*/

		g_theRenderer->BindTexture( nullptr );
		std::vector<Vertex_PCU> potVerts;
		if ( m_potentialField != nullptr )
		{
			for ( int i = 0; i < m_potentialField->nodes.size(); i++ )
			{
				AABB2 aabb = AABB2( m_potentialField->nodes[ i ].xCoord , m_potentialField->nodes[ i ].yCoord , m_potentialField->nodes[ i ].xCoord + 1 , m_potentialField->nodes[ i ].yCoord + 1 );
				AppendAABB2( potVerts , aabb , Rgba8( 100 , 100 , 100 , 100 ) );

			}
		}

		g_theRenderer->DrawVertexArray( potVerts );

		
		if ( m_potentialField != nullptr )
		{
			for ( int i = 0; i < m_potentialField->nodes.size(); i++ )
			{
				if ( m_potentialField->nodes[ i ].isConsiderd )
				{
					if ( m_potentialField->nodes[ i ].direction >= 0.f )
					{
						Vec2 lineStartPos = Vec2( m_potentialField->nodes[ i ].xCoord + 0.45f , m_potentialField->nodes[ i ].yCoord + 0.45f );
						Vec2 dirVec = lineStartPos.GetNormalized();
						Vec2 endPos = Vec2( lineStartPos.x + 0.45f , lineStartPos.y );
						Vec2 vec = endPos - lineStartPos;
						vec = vec.GetNormalized();
						vec.RotateDegrees( m_potentialField->nodes[ i ].direction );
						endPos = lineStartPos + ( vec.GetNormalized() * 0.45f );

						g_theRenderer->DrawLine( lineStartPos , endPos , Rgba8( 100 , 0 , 0 , 150 ) , 0.1f );
						g_theRenderer->DrawDisc( endPos , 0.1f , Rgba8( 100 , 0 , 0 , 150 ) );
					}
				}
			}
		}

		g_theRenderer->DrawRing( m_potentialField->GetCentre() , m_potentialField->GetBoundingRadius() , Rgba8( 100 , 0 , 0 , 150 ) , 1.f );

		Vec2 outPts[ 4 ];
		OBB2 box = m_potentialField->GetBoundingBox();
		box.GetCornerPositions( outPts );

		g_theRenderer->DrawLine( outPts[ 0 ] , outPts[ 1 ] , Rgba8( 100 , 0 , 0 , 100 ) , 1.f );
		g_theRenderer->DrawLine( outPts[ 1 ] , outPts[ 2 ] , Rgba8( 100 , 0 , 0 , 100 ) , 1.f );
		g_theRenderer->DrawLine( outPts[ 2 ] , outPts[ 3 ] , Rgba8( 100 , 0 , 0 , 100 ) , 1.f );
		g_theRenderer->DrawLine( outPts[ 0 ] , outPts[ 3 ] , Rgba8( 100 , 0 , 0 , 100 ) , 1.f );

		g_theRenderer->BindTexture( m_player1HudTex );
		//g_theRenderer->DrawVertexArray( verts );

		m_player->Render();
		m_supportPlayer->Render();
		m_greenBeret->Render();
	}


	if ( m_currentMode == PATHFINDER )
	{
		g_theGUI->Render();
	}

	if ( m_currentMode == MAPCREATOR )
	{
		m_mapCretor->Render();
	}

	if ( m_currentMode == POTENTIALFIELD_CREATOR )
	{
		m_potCreator->Render();
	}
	

	if ( g_theConsole.IsOpen() )
	{
		g_theConsole.Render( *g_theRenderer , *m_devConsoleCamera , 2.f , 1.f );
	}
	g_theRenderer->EndCamera( *m_gameCamera );

	if ( m_currentMode == GAME )
	{
		g_theRenderer->BeginCamera( *m_hudCamera );

		g_theRenderer->BindTexture( m_HudBoxTex );
		g_theRenderer->DrawAABB2D( m_hudBox , Rgba8() );

		double secondsRemainig = m_greenBeretHUDTimer->GetSecondsRemaining();
		secondsRemainig = Clamp( secondsRemainig , 0.f , 1.f );


		g_theRenderer->BindTexture( m_PlayerBoxTex );
		g_theRenderer->DrawAABB2D( m_playerBox , Rgba8() );

		g_theRenderer->BindTexture( m_greenBeretActiveAnims->GetSpriteTextureAtTime((float)secondsRemainig) );
		g_theRenderer->DrawAABB2D( m_player1Box, Rgba8() );
		
	
		//g_theRenderer->BindTexture( m_player2HudTex );
	
	
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->EndCamera( *m_hudCamera );
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

void Game::ToggleGameModes()
{
	if ( g_theInput->WasKeyJustPressed( '1' ) )
	{
		m_currentMode = GAME;
	}

	if ( g_theInput->WasKeyJustPressed( '2' ) )
	{
		m_currentMode = PATHFINDER;
	}

	if ( g_theInput->WasKeyJustPressed( '3' ) )
	{
		m_currentMode = MAPCREATOR;
	}

	if ( g_theInput->WasKeyJustPressed( '4' ) )
	{
		m_currentMode = POTENTIALFIELD_CREATOR;
	}
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

	int mainMapNumTiles = m_mainMapSize.x * m_mainMapSize.y;

	for ( int i = 0; i < mainMapNumTiles; i++ )
	{
		int tileX = i % m_mainMapSize.x;
		int tileY = i / m_mainMapSize.x;

		Tile newTile = Tile( tileX , tileY );
		m_mainMapTiles.push_back( newTile );
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

void Game::GetPathUsingAStarIgnoreDiagonalMovesOneStep( Vec2 startPos , Vec2 endPos , std::vector<int>& path, bool ignoreDiagonalMoves, bool considerInfluenceMaps )
{
	if ( pathFound )
	{
		return;
	}

	IntVec2 sPos = IntVec2( RoundDownToInt( startPos.x ) , RoundDownToInt( startPos.y ) );
	IntVec2 ePos = IntVec2( RoundDownToInt( endPos.x ) , RoundDownToInt( endPos.y ) );

	if ( sPos == ePos )
	{
		return;
	}

	IntVec2 currentTile = sPos;

	PathFindingHelper* base = new PathFindingHelper();
	base->coords = sPos;

	PathFindingHelper p1;
	PathFindingHelper p2;
	PathFindingHelper p3;
	PathFindingHelper p4;
	PathFindingHelper p5;
	PathFindingHelper p6;
	PathFindingHelper p7;
	PathFindingHelper p8;

	if ( currentTile.x - 1 > 0 )
	{
		p1.coords = IntVec2( currentTile.x - 1 , currentTile.y );
		p1.gCost = 1;
		p1.hCost = GetManhattanDistance( p1.coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p1.coords ) ].m_isSolid )
		{
			p1.isConsidered = true;
		}
		p1.parent = base;
	}

	if ( currentTile.x + 1 < m_mapSize.x )
	{
		p2.coords = IntVec2( currentTile.x + 1 , currentTile.y );
		p2.gCost = 1;
		p2.hCost = GetManhattanDistance( p2.coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p2.coords ) ].m_isSolid )
		{
			p2.isConsidered = true;
		}

		p2.parent = base;
	}

	if ( currentTile.y - 1 > 0 )
	{
		p3.coords = IntVec2( currentTile.x , currentTile.y - 1 );
		p3.gCost = 1;
		p3.hCost = GetManhattanDistance( p3.coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p3.coords ) ].m_isSolid )
		{
			p3.isConsidered = true;
		}

		p3.parent = base;
		
	}

	if ( currentTile.y + 1 < m_mapSize.y )
	{
		p4.coords = IntVec2( currentTile.x , currentTile.y + 1 );
		p4.gCost = 1;
		p4.hCost = GetManhattanDistance( p4.coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p4.coords ) ].m_isSolid )
		{
			p4.isConsidered = true;
		}

		p4.parent = base;
	}

	if ( !ignoreDiagonalMoves )
	{
		if ( currentTile.x - 1 > 0 && currentTile.y - 1 > 0 )
		{
			p5.coords = IntVec2( currentTile.x -1 , currentTile.y -1 );
			p5.gCost = 1.4f;
			p5.hCost = GetManhattanDistance( p5.coords , ePos );
			if ( !m_tiles[ GetTileIndexForTileCoords( p5.coords ) ].m_isSolid )
			{
				p5.isConsidered = true;
			}

			p5.parent = base;
		}

		if ( currentTile.x - 1 > 0 && currentTile.y + 1 < m_mapSize.y )
		{
			p6.coords = IntVec2( currentTile.x - 1 , currentTile.y + 1 );
			p6.gCost = 1.4f;
			p6.hCost = GetManhattanDistance( p6.coords , ePos );
			if ( !m_tiles[ GetTileIndexForTileCoords( p6.coords ) ].m_isSolid )
			{
				p6.isConsidered = true;
			}

			p6.parent = base;
		}

		if ( currentTile.x + 1 < m_mapSize.x && currentTile.y - 1 > 0 )
		{
			p7.coords = IntVec2( currentTile.x + 1 , currentTile.y - 1 );
			p7.gCost = 1.4f;
			p7.hCost = GetManhattanDistance( p7.coords , ePos );
			if ( !m_tiles[ GetTileIndexForTileCoords( p7.coords ) ].m_isSolid )
			{
				p7.isConsidered = true;
			}

			p7.parent = base;
		}

		if ( currentTile.x + 1 < m_mapSize.x && currentTile.y + 1 < m_mapSize.y )
		{
			p8.coords = IntVec2( currentTile.x + 1 , currentTile.y + 1 );
			p8.gCost = 1.4f;
			p8.hCost = GetManhattanDistance( p8.coords , ePos );
			if ( !m_tiles[ GetTileIndexForTileCoords( p8.coords ) ].m_isSolid )
			{
				p8.isConsidered = true;
			}

			p8.parent = base;
		}

	}

	
	if ( initDone == false )
	{
		if ( p1.isConsidered && !IsPathFindingHelpInList( p1 , openList1 ) )
		{
			openList1.push_back( p1 );
		}

		if ( p2.isConsidered && !IsPathFindingHelpInList( p2 , openList1 ) )
		{
			openList1.push_back( p2 );
		}

		if ( p3.isConsidered && !IsPathFindingHelpInList( p3 , openList1 ) )
		{
			openList1.push_back( p3 );
		}

		if ( p4.isConsidered && !IsPathFindingHelpInList( p4 , openList1 ) )
		{
			openList1.push_back( p4 );
		}


		if ( !ignoreDiagonalMoves )
		{
			if ( p5.isConsidered && !IsPathFindingHelpInList( p5 , openList1 ) )
			{
				openList1.push_back( p5 );
			}

			if ( p6.isConsidered && !IsPathFindingHelpInList( p6 , openList1 ) )
			{
				openList1.push_back( p6 );
			}

			if ( p7.isConsidered && !IsPathFindingHelpInList( p7 , openList1 ) )
			{
				openList1.push_back( p7 );
			}

			if ( p8.isConsidered && !IsPathFindingHelpInList( p8 , openList1 ) )
			{
				openList1.push_back( p8 );
			}
		}

		initDone = true;
	}

	/*while ( !pathFound )
	{*/
		PathFindingHelper* tileToExpand = new PathFindingHelper();
		tileToExpand->coords = openList1[ 0 ].coords;
		tileToExpand->hCost = openList1[ 0 ].hCost;
		tileToExpand->gCost = openList1[ 0 ].gCost;
		tileToExpand->isConsidered = openList1[ 0 ].isConsidered;
		tileToExpand->parent = openList1[ 0 ].parent;

		int index = 0;

		float min = 9999.f;

		for ( int i = 1; i < openList1.size(); i++ )
		{
			float h1 = openList1[ i ].hCost;
			float h2 = tileToExpand->hCost;
			float i1 = 0.f;
			float i2 = 0.f;
			float f1 = openList1[ i ].gCost + h1 ;
			float f2 = tileToExpand->gCost + h2 ;

			if ( considerInfluenceMaps )
			{
				i1 = m_tiles[ GetTileIndexForTileCoords( openList1[ i ].coords ) ].m_influenceValue;
				i2 = m_tiles[ GetTileIndexForTileCoords( tileToExpand->coords ) ].m_influenceValue;

				f1 += i1;
				f2 += i2;

				if ( ignoreDiagonalMoves )
				{
					i1 *= 0.1f;
					i2 *= 0.1f;
				}
				else
				{
					i1 *= 2.f;
					i2 *= 2.f;
				}
			}

			if ( f1 < f2 && f1<min )
			{
				min = f1;
			}
			
			if ( f1 < f2 /*|| h1 < h2*/ || i1<i2 && f1<min )
			{
				tileToExpand->coords = openList1[ i ].coords;
				tileToExpand->hCost = openList1[ i ].hCost;
				tileToExpand->gCost = openList1[ i ].gCost;
				tileToExpand->isConsidered = openList1[ i ].isConsidered;
				tileToExpand->parent = openList1[ i ].parent;
				index = i;
			}
			
			

		}

		if ( tileToExpand->coords == ePos )
		{
			closedList1.push_back( *tileToExpand );
			pathFound = true;
			//break;
		}

	
		closedList1.push_back( *tileToExpand );
		

		
		auto it = openList1.begin();
		openList1.erase( it + index );
	
		openList1.shrink_to_fit();


		PathFindingHelper x1;
		PathFindingHelper x2;
		PathFindingHelper x3;
		PathFindingHelper x4;
		PathFindingHelper x5;
		PathFindingHelper x6;
		PathFindingHelper x7;
		PathFindingHelper x8;

		if ( tileToExpand->coords.x - 1 > 0 )
		{
			x1.coords = IntVec2( tileToExpand->coords.x - 1 , tileToExpand->coords.y );
			x1.gCost = tileToExpand->gCost + 1;
			x1.hCost = GetManhattanDistance( x1.coords , ePos );


			if ( !m_tiles[ GetTileIndexForTileCoords(x1.coords) ].m_isSolid )
			{
				x1.isConsidered = true;
			}

			x1.parent = tileToExpand;
		}

		if ( x1.isConsidered )
		{
			bool found = false;
			for ( int i = 0; i < openList1.size(); i++ )
			{
				if ( openList1[ i ].coords == x1.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList1.size(); i++ )
			{
				if ( closedList1[ i ].coords == x1.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList1.push_back( x1 );
			}
		}

		if ( tileToExpand->coords.x + 1 < m_mapSize.x )
		{
			x2.coords = IntVec2( tileToExpand->coords.x + 1 , tileToExpand->coords.y );
			x2.gCost = tileToExpand->gCost + 1;
			x2.hCost = GetManhattanDistance( x2.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x2.coords ) ].m_isSolid )
			{
				x2.isConsidered = true;
			}
			x2.parent = tileToExpand;

		}

		if ( x2.isConsidered )
		{
			bool found = false;
			for ( int i = 0; i < openList1.size(); i++ )
			{
				if ( openList1[ i ].coords == x2.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList1.size(); i++ )
			{
				if ( closedList1[ i ].coords == x2.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList1.push_back( x2 );
			}
		}

		if ( tileToExpand->coords.y - 1 > 0 )
		{
			x3.coords = IntVec2( tileToExpand->coords.x , tileToExpand->coords.y - 1 );
			x3.gCost = tileToExpand->gCost + 1;
			x3.hCost = GetManhattanDistance( x3.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x3.coords ) ].m_isSolid )
			{
				x3.isConsidered = true;
			}
			x3.parent = tileToExpand;
		}

		if ( x3.isConsidered )
		{
			bool found = false;
			
			for ( int i = 0; i < openList1.size(); i++ )
			{
				if ( openList1[ i ].coords == x3.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList1.size(); i++ )
			{
				if ( closedList1[ i ].coords == x3.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList1.push_back( x3 );
			}
		}

		if ( tileToExpand->coords.y + 1 < m_mapSize.y )
		{
			x4.coords = IntVec2( tileToExpand->coords.x , tileToExpand->coords.y + 1 );
			x4.gCost = tileToExpand->gCost + 1;
			x4.hCost = GetManhattanDistance( x4.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x4.coords ) ].m_isSolid )
			{
				x4.isConsidered = true;
			}

			x4.parent = tileToExpand;
		}

		if ( x4.isConsidered )
		{
			bool found = false;

			for ( int i = 0; i < openList1.size(); i++ )
			{
				if ( openList1[ i ].coords == x4.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList1.size(); i++ )
			{
				if ( closedList1[ i ].coords == x4.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList1.push_back( x4 );
			}
		}

		if ( !ignoreDiagonalMoves )
		{

			if ( tileToExpand->coords.x - 1 > 0 && tileToExpand->coords.y - 1 > 0 )
			{
				x5.coords = IntVec2( tileToExpand->coords.x - 1 , tileToExpand->coords.y - 1 );
				x5.gCost = tileToExpand->gCost + 1.4f;
				x5.hCost = GetManhattanDistance( x5.coords , ePos );

				if ( !m_tiles[ GetTileIndexForTileCoords( x5.coords ) ].m_isSolid )
				{
					x5.isConsidered = true;
				}

				x5.parent = tileToExpand;
			}

			if ( x5.isConsidered )
			{
				bool found = false;

				for ( int i = 0; i < openList1.size(); i++ )
				{
					if ( openList1[ i ].coords == x5.coords )
					{
						found = true;
						break;
					}
				}

				for ( int i = 0; i < closedList1.size(); i++ )
				{
					if ( closedList1[ i ].coords == x5.coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					openList1.push_back( x5 );
				}
			}


			if ( tileToExpand->coords.x - 1 > 0 && tileToExpand->coords.y + 1 < m_mapSize.y )
			{
				x6.coords = IntVec2( tileToExpand->coords.x - 1 , tileToExpand->coords.y + 1 );
				x6.gCost = tileToExpand->gCost + 1.4f;
				x6.hCost = GetManhattanDistance( x6.coords , ePos );

				if ( !m_tiles[ GetTileIndexForTileCoords( x6.coords ) ].m_isSolid )
				{
					x6.isConsidered = true;
				}

				x6.parent = tileToExpand;
			}

			if ( x6.isConsidered )
			{
				bool found = false;

				for ( int i = 0; i < openList1.size(); i++ )
				{
					if ( openList1[ i ].coords == x6.coords )
					{
						found = true;
						break;
					}
				}

				for ( int i = 0; i < closedList1.size(); i++ )
				{
					if ( closedList1[ i ].coords == x6.coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					openList1.push_back( x6 );
				}
			}

			if ( tileToExpand->coords.x + 1 < m_mapSize.x && tileToExpand->coords.y - 1 > 0 )
			{
				x7.coords = IntVec2( tileToExpand->coords.x + 1 , tileToExpand->coords.y - 1 );
				x7.gCost = tileToExpand->gCost + 1.4f;
				x7.hCost = GetManhattanDistance( x7.coords , ePos );

				if ( !m_tiles[ GetTileIndexForTileCoords( x7.coords ) ].m_isSolid )
				{
					x7.isConsidered = true;
				}

				x7.parent = tileToExpand;
			}

			if ( x7.isConsidered )
			{
				bool found = false;

				for ( int i = 0; i < openList1.size(); i++ )
				{
					if ( openList1[ i ].coords == x7.coords )
					{
						found = true;
						break;
					}
				}

				for ( int i = 0; i < closedList1.size(); i++ )
				{
					if ( closedList1[ i ].coords == x7.coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					openList1.push_back( x7 );
				}
			}

			if ( tileToExpand->coords.x + 1 < m_mapSize.x && tileToExpand->coords.y + 1 < m_mapSize.y )
			{
				x8.coords = IntVec2( tileToExpand->coords.x + 1 , tileToExpand->coords.y + 1 );
				x8.gCost = tileToExpand->gCost + 1.4f;
				x8.hCost = GetManhattanDistance( x8.coords , ePos );

				if ( !m_tiles[ GetTileIndexForTileCoords( x8.coords ) ].m_isSolid )
				{
					x8.isConsidered = true;
				}

				x8.parent = tileToExpand;
			}

			if ( x8.isConsidered )
			{
				bool found = false;

				for ( int i = 0; i < openList1.size(); i++ )
				{
					if ( openList1[ i ].coords == x8.coords )
					{
						found = true;
						break;
					}
				}

				for ( int i = 0; i < closedList1.size(); i++ )
				{
					if ( closedList1[ i ].coords == x8.coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					openList1.push_back( x8 );
				}
			}
		}




		if ( pathFound )
		{
			PathFindingHelper foundPath = closedList1.back();
			path.push_back( GetTileIndexForTileCoords( foundPath.coords ) );

			
			PathFindingHelper currentParent = *foundPath.parent;
			while ( currentParent.parent != nullptr )
			{

				std::vector<PathFindingHelper> currentNodesWithSameParent;
				for ( int i = 0; i < closedList1.size(); i++ )
				{
					if ( closedList1[ i ].parent == &currentParent )
					{
						currentNodesWithSameParent.push_back( closedList1[ i ] );
					}
				}

				PathFindingHelper chosenTile;
				if ( currentNodesWithSameParent.size() > 0 )
				{
					chosenTile = currentNodesWithSameParent[ 0 ];
				}
				if ( currentNodesWithSameParent.size() > 1 )
				{
					for ( int i = 1; i < currentNodesWithSameParent.size(); i++ )
					{

						if ( m_tiles[ GetTileIndexForTileCoords( currentNodesWithSameParent[ i ].coords ) ].m_influenceValue < m_tiles[ GetTileIndexForTileCoords( chosenTile.coords ) ].m_influenceValue )
						{
							chosenTile = currentNodesWithSameParent[ i ];
						}
					}
				}
				if ( chosenTile.isConsidered )
				{
					path.push_back( GetTileIndexForTileCoords( chosenTile.coords ) );
					currentParent = *chosenTile.parent;
				}
				else
				{
					path.push_back( GetTileIndexForTileCoords( currentParent.coords ) );
					currentParent = *currentParent.parent;
				}
				currentNodesWithSameParent.clear();
			}
		}
}

void Game::GetPathUsingAstarWithDiagonalMoves( Vec2 startPos , Vec2 endPos , std::vector<int>& path )
{
	IntVec2 sPos = IntVec2( RoundDownToInt( startPos.x ) , RoundDownToInt( startPos.y ) );
	IntVec2 ePos = IntVec2( RoundDownToInt( endPos.x ) , RoundDownToInt( endPos.y ) );

	if ( sPos == ePos )
	{
		return;
	}

	std::vector<PathFindingHelper> openList;
	std::vector<PathFindingHelper> closedList;

	bool pathFound2 = false;

	IntVec2 currentTile = sPos;

	PathFindingHelper* base = new PathFindingHelper();
	base->coords = sPos;

	PathFindingHelper* p1 = new PathFindingHelper();
	PathFindingHelper* p2 = new PathFindingHelper();
	PathFindingHelper* p3 = new PathFindingHelper();
	PathFindingHelper* p4 = new PathFindingHelper();
	PathFindingHelper* p5 = new PathFindingHelper();
	PathFindingHelper* p6 = new PathFindingHelper();
	PathFindingHelper* p7 = new PathFindingHelper();
	PathFindingHelper* p8 = new PathFindingHelper();

	
	if ( currentTile.x - 1 > 0 )
	{
		p1->coords = IntVec2( currentTile.x - 1 , currentTile.y );
		p1->gCost = 10;
		p1->hCost = /*( p1->coords - ePos ).GetLengthSquared()*/GetManhattanDistance(p1->coords,ePos);
		if ( !m_tiles[ GetTileIndexForTileCoords( p1->coords ) ].m_isSolid )
		{
			p1->isConsidered = true;
		}
		p1->parent = base;
	}

	if ( currentTile.x + 1 < m_mapSize.x )
	{
		p2->coords = IntVec2( currentTile.x + 1 , currentTile.y );
		p2->gCost = 10;
		p2->hCost = /*( p2->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p2->coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p2->coords ) ].m_isSolid )
		{
			p2->isConsidered = true;
		}

		p2->parent = base;
	}

	if ( currentTile.y - 1 > 0 )
	{
		p3->coords = IntVec2( currentTile.x , currentTile.y - 1 );
		p3->gCost = 10;
		p3->hCost = /*( p3->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p3->coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p3->coords ) ].m_isSolid )
		{
			p3->isConsidered = true;
		}

		p3->parent = base;

	}

	if ( currentTile.y + 1 < m_mapSize.y )
	{
		p4->coords = IntVec2( currentTile.x , currentTile.y + 1 );
		p4->gCost = 10;
		p4->hCost = /*( p4->coords , ePos ).GetLengthSquared()*/GetManhattanDistance( p4->coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p4->coords ) ].m_isSolid )
		{
			p4->isConsidered = true;
		}

		p4->parent = base;
	}

	if ( currentTile.x - 1 > 0 && currentTile.y - 1 > 0 )
	{
		p5->coords = IntVec2( currentTile.x - 1 , currentTile.y - 1 );
		p5->gCost = 14;
		p5->hCost = /*( p5->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p5->coords , ePos );

		if ( !m_tiles[ GetTileIndexForTileCoords( p5->coords ) ].m_isSolid )
		{
			p5->isConsidered = true;
		}

		p5->parent = base;
	}

	if ( currentTile.x - 1 > 0 && currentTile.y + 1 < m_mapSize.y )
	{
		p6->coords = IntVec2( currentTile.x - 1 , currentTile.y + 1 );
		p6->gCost = 14;
		p6->hCost = /*( p6->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p6->coords , ePos );

		if ( !m_tiles[ GetTileIndexForTileCoords( p6->coords ) ].m_isSolid )
		{
			p6->isConsidered = true;
		}

		p6->parent = base;
	}

	if ( currentTile.x + 1 < m_mapSize.x && currentTile.y - 1 > 0 )
	{
		p7->coords = IntVec2( currentTile.x + 1 , currentTile.y - 1 );
		p7->gCost = 14;
		p7->hCost = /*( p7->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p7->coords , ePos );

		if ( !m_tiles[ GetTileIndexForTileCoords( p7->coords ) ].m_isSolid )
		{
			p7->isConsidered = true;
		}
		p7->parent = base;
	}

	if ( currentTile.x + 1 < m_mapSize.x && currentTile.y + 1 < m_mapSize.y )
	{
		p8->coords = IntVec2( currentTile.x + 1 , currentTile.y + 1 );
		p8->gCost = 14;
		p8->hCost = /*( p8->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p8->coords , ePos );

		if ( !m_tiles[ GetTileIndexForTileCoords( p8->coords ) ].m_isSolid )
		{
			p8->isConsidered = true;
		}

		p8->parent = base;

	}

	if ( p1->isConsidered )
	{
		openList.push_back( *p1 );
	}

	if ( p2->isConsidered )
	{
		openList.push_back( *p2 );
	}

	if ( p3->isConsidered )
	{
		openList.push_back( *p3 );
	}

	if ( p4->isConsidered )
	{
		openList.push_back( *p4 );
	}

	if ( p5->isConsidered )
	{
		openList.push_back( *p5 );
	}

	if ( p6->isConsidered )
	{
		openList.push_back( *p6 );
	}

	if ( p7->isConsidered )
	{
		openList.push_back( *p7 );
	}

	if ( p8->isConsidered )
	{
		openList.push_back( *p8 );
	}

	while ( !pathFound2 )
	{
		PathFindingHelper* tileToExpand = new PathFindingHelper();
		tileToExpand->coords = openList[ 0 ].coords;
		tileToExpand->hCost = openList[ 0 ].hCost;
		tileToExpand->gCost = openList[ 0 ].gCost;
		tileToExpand->isConsidered = openList[ 0 ].isConsidered;
		tileToExpand->parent = openList[ 0 ].parent;

		for ( int i = 1; i < openList.size(); i++ )
		{
				if ( ( openList[ i ].gCost + openList[ i ].hCost ) < ( tileToExpand->gCost + tileToExpand->hCost ) )
				{
					tileToExpand->coords = openList[ i ].coords;
					tileToExpand->hCost = openList[ i ].hCost;
					tileToExpand->gCost = openList[ i ].gCost;
					tileToExpand->isConsidered = openList[ i ].isConsidered;
					tileToExpand->parent = openList[ i ].parent;
				}
		}

		if ( tileToExpand->coords == ePos )
		{
			pathFound = true;
			continue;
		}

		closedList.push_back( *tileToExpand );


		for ( auto it = openList.begin(); it != openList.end(); it++ )
		{
			if ( tileToExpand->coords == it->coords )
			{
				openList.erase( it );
				break;
			}
		}

		openList.shrink_to_fit();

		PathFindingHelper x1;
		PathFindingHelper x2;
		PathFindingHelper x3;
		PathFindingHelper x4;
		PathFindingHelper x5;
		PathFindingHelper x6;
		PathFindingHelper x7;
		PathFindingHelper x8;

		if ( tileToExpand->coords.x - 1 > 0 )
		{
			x1.coords = IntVec2( tileToExpand->coords.x - 1 , tileToExpand->coords.y );
			x1.gCost = tileToExpand->gCost+10;
			x1.hCost = /*( x1.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x1.coords , ePos );


			if ( !m_tiles[ GetTileIndexForTileCoords( x1.coords ) ].m_isSolid )
			{
				x1.isConsidered = true;
			}

			x1.parent = tileToExpand;
		}

		if ( x1.isConsidered )
		{
			bool found = false;
			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ].coords == x1.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ].coords == x1.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList.push_back( x1 );
			}
		}

		if ( tileToExpand->coords.x + 1 < m_mapSize.x )
		{
			x2.coords = IntVec2( tileToExpand->coords.x + 1 , tileToExpand->coords.y );
			x2.gCost = tileToExpand->gCost+10;
			x2.hCost = /*( x2.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x2.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x2.coords ) ].m_isSolid )
			{
				x2.isConsidered = true;
			}
			x2.parent = tileToExpand;

		}

		if ( x2.isConsidered )
		{
			bool found = false;
			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ].coords == x2.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ].coords == x2.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList.push_back( x2 );
			}
		}

		if ( tileToExpand->coords.y - 1 > 0 )
		{
			x3.coords = IntVec2( tileToExpand->coords.x , tileToExpand->coords.y - 1 );
			x3.gCost = tileToExpand->gCost+10;
			x3.hCost = /*( x3.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x3.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x3.coords ) ].m_isSolid )
			{
				x3.isConsidered = true;
			}
			x3.parent = tileToExpand;
		}

		if ( x3.isConsidered )
		{
			bool found = false;

			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ].coords == x3.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ].coords == x3.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList.push_back( x3 );
			}
		}

		if ( tileToExpand->coords.y + 1 < m_mapSize.y )
		{
			x4.coords = IntVec2( tileToExpand->coords.x , tileToExpand->coords.y + 1 );
			x4.gCost = tileToExpand->gCost+10;
			x4.hCost = /*( x4.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x4.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x4.coords ) ].m_isSolid )
			{
				x4.isConsidered = true;
			}

			x4.parent = tileToExpand;
		}

		if ( x4.isConsidered )
		{
			bool found = false;

			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ].coords == x4.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ].coords == x4.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList.push_back( x4 );
			}
		}


		if ( tileToExpand->coords.y + 1 < m_mapSize.y && tileToExpand->coords.x -1>0 )
		{
			x5.coords = IntVec2( tileToExpand->coords.x-1 , tileToExpand->coords.y + 1 );
			x5.gCost = tileToExpand->gCost + 14;
			x5.hCost = /*( x5.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x5.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x5.coords ) ].m_isSolid )
			{
				x5.isConsidered = true;
			}

			x5.parent = tileToExpand;
		}

		if ( x5.isConsidered )
		{
			bool found = false;

			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ].coords == x5.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ].coords == x5.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList.push_back( x5 );
			}
		}

		if ( tileToExpand->coords.y + 1 < m_mapSize.y && tileToExpand->coords.x + 1 < m_mapSize.x )
		{
			x6.coords = IntVec2( tileToExpand->coords.x + 1 , tileToExpand->coords.y + 1 );
			x6.gCost = tileToExpand->gCost + 14;
			x6.hCost = /*( x6.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x6.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x6.coords ) ].m_isSolid )
			{
				x6.isConsidered = true;
			}

			x6.parent = tileToExpand;
		}

		if ( x6.isConsidered )
		{
			bool found = false;

			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ].coords == x6.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ].coords == x6.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList.push_back( x6 );
			}
		}

		if ( tileToExpand->coords.y - 1 > 0 && tileToExpand->coords.x - 1 > 0 )
		{
			x7.coords = IntVec2( tileToExpand->coords.x - 1 , tileToExpand->coords.y - 1 );
			x7.gCost = tileToExpand->gCost + 14;
			x7.hCost = /*( x7.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x7.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x7.coords ) ].m_isSolid )
			{
				x7.isConsidered = true;
			}

			x7.parent = tileToExpand;
		}

		if ( x7.isConsidered )
		{
			bool found = false;

			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ].coords == x7.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ].coords == x7.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList.push_back( x7 );
			}
		}


		if ( tileToExpand->coords.y - 1 > 0 && tileToExpand->coords.x + 1 < m_mapSize.x )
		{
			x8.coords = IntVec2( tileToExpand->coords.x + 1 , tileToExpand->coords.y - 1 );
			x8.gCost = tileToExpand->gCost + 14;
			x8.hCost = /*( x8.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x8.coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x8.coords ) ].m_isSolid )
			{
				x8.isConsidered = true;
			}

			x8.parent = tileToExpand;
		}

		if ( x8.isConsidered )
		{
			bool found = false;

			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ].coords == x8.coords )
				{
					found = true;
					break;
				}
			}

			for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ].coords == x8.coords )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				openList.push_back( x8 );
			}
		}


	}


	PathFindingHelper foundPath = closedList.back();

	while ( foundPath.parent != nullptr )
	{
		path.push_back( GetTileIndexForTileCoords( foundPath.coords ) );
		foundPath = *( foundPath.parent );
	}
}

void Game::GetPathUsingAStarIgnoreDiagonalMoves( Vec2 startPos , Vec2 endPos , std::vector<int>& path , bool considerInfluenceMap, bool ignoreDiagonalMoves )
{
	IntVec2 sPos = IntVec2( RoundDownToInt( startPos.x ) , RoundDownToInt( startPos.y ) );
	IntVec2 ePos = IntVec2( RoundDownToInt( endPos.x ) , RoundDownToInt( endPos.y ) );

	if ( sPos == ePos )
	{
		return;
	}

	std::vector<PathFindingHelper*> openList;
	std::vector<PathFindingHelper*> closedList;

	openList.reserve( 20000 );
	closedList.reserve( 20000 );

	bool pathFound2 = false;

	IntVec2 currentTile = sPos;

	PathFindingHelper* base = new PathFindingHelper();
	base->coords = sPos;

	PathFindingHelper* p1 = new PathFindingHelper();
	PathFindingHelper* p2 = new PathFindingHelper();
	PathFindingHelper* p3 = new PathFindingHelper();
	PathFindingHelper* p4 = new PathFindingHelper();
	PathFindingHelper* p5 = new PathFindingHelper();
	PathFindingHelper* p6 = new PathFindingHelper();
	PathFindingHelper* p7 = new PathFindingHelper();
	PathFindingHelper* p8 = new PathFindingHelper();



	if ( currentTile.x - 1 > 0 )
	{
		p1->coords = IntVec2( currentTile.x - 1 , currentTile.y );
		p1->gCost = 1;
		p1->hCost = /*( p1->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p1->coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p1->coords ) ].m_isSolid )
		{
			p1->isConsidered = true;
		}
		p1->parent = base;
	}

	if ( currentTile.x + 1 < m_mapSize.x )
	{
		p2->coords = IntVec2( currentTile.x + 1 , currentTile.y );
		p2->gCost = 1;
		p2->hCost = /*( p2->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p2->coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p2->coords ) ].m_isSolid )
		{
			p2->isConsidered = true;
		}

		p2->parent = base;
	}

	if ( currentTile.y - 1 > 0 )
	{
		p3->coords = IntVec2( currentTile.x , currentTile.y - 1 );
		p3->gCost = 1;
		p3->hCost = /*( p3->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p3->coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p3->coords ) ].m_isSolid )
		{
			p3->isConsidered = true;
		}

		p3->parent = base;

	}

	if ( currentTile.y + 1 < m_mapSize.y )
	{
		p4->coords = IntVec2( currentTile.x , currentTile.y + 1 );
		p4->gCost = 1;
		p4->hCost = /*( p4->coords , ePos ).GetLengthSquared()*/GetManhattanDistance( p4->coords , ePos );
		if ( !m_tiles[ GetTileIndexForTileCoords( p4->coords ) ].m_isSolid )
		{
			p4->isConsidered = true;
		}

		p4->parent = base;
	}

	if ( !ignoreDiagonalMoves )
	{
		if ( currentTile.x - 1 > 0 && currentTile.y-1 > 0 )
		{
			p5->coords = IntVec2( currentTile.x - 1 , currentTile.y - 1 );
			p5->gCost = 1.4f;
			p5->hCost = /*( p1->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p5->coords , ePos );
			if ( !m_tiles[ GetTileIndexForTileCoords( p5->coords ) ].m_isSolid )
			{
				p5->isConsidered = true;
			}
			p5->parent = base;
		}

		if ( currentTile.x - 1 > 0 && currentTile.y + 1 < m_mapSize.y )
		{
			p6->coords = IntVec2( currentTile.x - 1 , currentTile.y + 1 );
			p6->gCost = 1.4f;
			p6->hCost = /*( p1->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p6->coords , ePos );
			if ( !m_tiles[ GetTileIndexForTileCoords( p6->coords ) ].m_isSolid )
			{
				p6->isConsidered = true;
			}
			p6->parent = base;
		}

		if ( currentTile.x + 1 < m_mapSize.x && currentTile.y - 1 > 0 )
		{
			p7->coords = IntVec2( currentTile.x + 1 , currentTile.y - 1 );
			p7->gCost = 1.4f;
			p7->hCost = /*( p1->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p7->coords , ePos );
			if ( !m_tiles[ GetTileIndexForTileCoords( p7->coords ) ].m_isSolid )
			{
				p7->isConsidered = true;
			}
			p7->parent = base;
		}

		if ( currentTile.x + 1 < m_mapSize.x && currentTile.y + 1 < m_mapSize.y )
		{
			p8->coords = IntVec2( currentTile.x + 1 , currentTile.y + 1 );
			p8->gCost = 1.4f;
			p8->hCost = /*( p1->coords - ePos ).GetLengthSquared()*/GetManhattanDistance( p8->coords , ePos );
			if ( !m_tiles[ GetTileIndexForTileCoords( p8->coords ) ].m_isSolid )
			{
				p8->isConsidered = true;
			}
			p8->parent = base;
		}
	}

	if ( p1->isConsidered )
	{
		openList.push_back( p1 );
	}

	if ( p2->isConsidered )
	{
		openList.push_back( p2 );
	}

	if ( p3->isConsidered )
	{
		openList.push_back( p3 );
	}

	if ( p4->isConsidered )
	{
		openList.push_back( p4 );
	}

	if ( !ignoreDiagonalMoves )
	{
		if ( p5->isConsidered )
		{
			openList.push_back( p5 );
		}

		if ( p6->isConsidered )
		{
			openList.push_back( p6 );
		}

		if ( p7->isConsidered )
		{
			openList.push_back( p7 );
		}

		if ( p8->isConsidered )
		{
			openList.push_back( p8 );
		}
	}

	
	PathFindingHelper* tileToExpand = new PathFindingHelper();

	

	while ( !pathFound2 )
	{

		PathFindingHelper* x1 = new PathFindingHelper();
		PathFindingHelper* x2 = new PathFindingHelper();
		PathFindingHelper* x3 = new PathFindingHelper();
		PathFindingHelper* x4 = new PathFindingHelper();
		PathFindingHelper* x5 = new PathFindingHelper();
		PathFindingHelper* x6 = new PathFindingHelper();
		PathFindingHelper* x7 = new PathFindingHelper();
		PathFindingHelper* x8 = new PathFindingHelper();

		int index = 0;
		
		for ( int i = 0; i < openList.size(); i++ )
		{
			if ( !openList[ i ]->isClosed )
			{
				tileToExpand = openList[ i ];
				index = i;
				break;
			}
		}

		float min = 99999.f;

		for ( int i = 0; i < openList.size(); i++ )
		{
			if ( openList[ i ]->isClosed || i == index  )
			{
				continue;
			}

			float f1 = tileToExpand->gCost;
			float f2 = openList[ i ]->gCost;
			float h1 = tileToExpand->hCost;
			float h2 = openList[ i ]->hCost;

			f1 = f1 + h1;
			f2 = f2 + h2;
			float i1 = 0.f;
			float i2 = 0.f;

			if ( considerInfluenceMap )
			{
				i1 = m_tiles[ GetTileIndexForTileCoords( tileToExpand->coords ) ].m_influenceValue ;
				i2 = m_tiles[ GetTileIndexForTileCoords( openList[ i ]->coords ) ].m_influenceValue ;

				if ( !ignoreDiagonalMoves )
				{
					i1 *= 2.f;
					i2 *= 2.f;
				}
				else
				{
					i1 *= 0.1f;
					i2 *= 0.1f;
				}

				f1 += i1;
				f2 += i2;
			}

			if (f2 < f1 && f2 < min )
			{
				min = f2;
			}

			if ( f2 <f1 || i2<i1  && f2<min )
			{
				tileToExpand = openList[ i ];
			}
			
		}


		if ( tileToExpand->coords == ePos )
		{
			pathFound2 = true;
			closedList.push_back( tileToExpand );
			break;
		}

		tileToExpand->isClosed = true;

		closedList.push_back( tileToExpand );

		if ( tileToExpand->coords.x - 1 > 0 )
		{
			x1->coords = IntVec2( tileToExpand->coords.x - 1 , tileToExpand->coords.y );
			x1->gCost = tileToExpand->gCost + 1.f;
			x1->hCost = /*( x1.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x1->coords , ePos );


			if ( !m_tiles[ GetTileIndexForTileCoords( x1->coords ) ].m_isSolid )
			{
				x1->isConsidered = true;
			}

			x1->parent = tileToExpand;
		}

		if ( x1->isConsidered )
		{
			bool found = false;
			for ( int i = 0; i < openList.size(); i++ )
			{

				if ( openList[ i ]->coords == x1->coords )
				{
					found = true;
					break;
				}
			}

			/*	for ( int i = 0; i < closedList.size(); i++ )
				{
					if ( closedList[ i ]->coords == x1->coords )
					{
						found = true;
						break;
					}
				}*/

			if ( !found )
			{
				openList.push_back( x1 );
			}
		}

		if ( tileToExpand->coords.x + 1 < m_mapSize.x )
		{
			x2->coords = IntVec2( tileToExpand->coords.x + 1 , tileToExpand->coords.y );
			x2->gCost = tileToExpand->gCost + 1.f;
			x2->hCost = /*( x2.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x2->coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x2->coords ) ].m_isSolid )
			{
				x2->isConsidered = true;
			}
			x2->parent = tileToExpand;

		}

		if ( x2->isConsidered )
		{
			bool found = false;
			for ( int i = 0; i < openList.size(); i++ )
			{

				if ( openList[ i ]->coords == x2->coords )
				{
					found = true;
					break;
				}
			}

			/*for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ]->coords == x2->coords )
				{
					found = true;
					break;
				}
			}*/

			if ( !found )
			{
				openList.push_back( x2 );
			}
		}

		if ( tileToExpand->coords.y - 1 > 0 )
		{
			x3->coords = IntVec2( tileToExpand->coords.x , tileToExpand->coords.y - 1 );
			x3->gCost = tileToExpand->gCost + 1.f;
			x3->hCost = /*( x3.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x3->coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x3->coords ) ].m_isSolid )
			{
				x3->isConsidered = true;
			}
			x3->parent = tileToExpand;
		}

		if ( x3->isConsidered )
		{
			bool found = false;

			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ]->coords == x3->coords )
				{
					found = true;
					break;
				}
			}

			/*for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ]->coords == x3->coords )
				{
					found = true;
					break;
				}
			}*/

			if ( !found )
			{
				openList.push_back( x3 );
			}
		}

		if ( tileToExpand->coords.y + 1 < m_mapSize.y )
		{
			x4->coords = IntVec2( tileToExpand->coords.x , tileToExpand->coords.y + 1 );
			x4->gCost = tileToExpand->gCost + 1.f;
			x4->hCost = /*( x4.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x4->coords , ePos );

			if ( !m_tiles[ GetTileIndexForTileCoords( x4->coords ) ].m_isSolid )
			{
				x4->isConsidered = true;
			}

			x4->parent = tileToExpand;
		}

		if ( x4->isConsidered )
		{
			bool found = false;

			for ( int i = 0; i < openList.size(); i++ )
			{
				if ( openList[ i ]->coords == x4->coords )
				{
					found = true;
					break;
				}
			}

			/*for ( int i = 0; i < closedList.size(); i++ )
			{
				if ( closedList[ i ]->coords == x4->coords )
				{
					found = true;
					break;
				}
			}*/

			if ( !found )
			{
				openList.push_back( x4 );
			}
		}

		if ( !ignoreDiagonalMoves )
		{
			if ( tileToExpand->coords.x - 1 > 0 && tileToExpand->coords.y -1 > 0 )
			{
				x5->coords = IntVec2( tileToExpand->coords.x - 1 , tileToExpand->coords.y - 1 );
				x5->gCost = tileToExpand->gCost + 1.4f;
				x5->hCost = /*( x4.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x5->coords , ePos );

				if ( !m_tiles[ GetTileIndexForTileCoords( x5->coords ) ].m_isSolid )
				{
					x5->isConsidered = true;
				}

				x5->parent = tileToExpand;
			}


			if ( x5->isConsidered )
			{
				bool found = false;

				for ( int i = 0; i < openList.size(); i++ )
				{
					if ( openList[ i ]->coords == x5->coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					openList.push_back( x5 );
				}
			}

			if ( tileToExpand->coords.x - 1 > 0 && tileToExpand->coords.y + 1 < m_mapSize.y )
			{
				x6->coords = IntVec2( tileToExpand->coords.x - 1 , tileToExpand->coords.y + 1 );
				x6->gCost = tileToExpand->gCost + 1.4f;
				x6->hCost = /*( x4.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x6->coords , ePos );

				if ( !m_tiles[ GetTileIndexForTileCoords( x6->coords ) ].m_isSolid )
				{
					x6->isConsidered = true;
				}

				x6->parent = tileToExpand;
			}

			if ( x6->isConsidered )
			{
				bool found = false;

				for ( int i = 0; i < openList.size(); i++ )
				{
					if ( openList[ i ]->coords == x6->coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					openList.push_back( x6 );
				}
			}

			if ( tileToExpand->coords.x + 1 < m_mapSize.x && tileToExpand->coords.y - 1 > 0 )
			{
				x7->coords = IntVec2( tileToExpand->coords.x + 1 , tileToExpand->coords.y - 1 );
				x7->gCost = tileToExpand->gCost + 1.4f;
				x7->hCost = /*( x4.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x7->coords , ePos );

				if ( !m_tiles[ GetTileIndexForTileCoords( x7->coords ) ].m_isSolid )
				{
					x7->isConsidered = true;
				}

				x7->parent = tileToExpand;
			}

			if ( x7->isConsidered )
			{
				bool found = false;

				for ( int i = 0; i < openList.size(); i++ )
				{
					if ( openList[ i ]->coords == x7->coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					openList.push_back( x7 );
				}
			}

			if ( tileToExpand->coords.x + 1 < m_mapSize.x && tileToExpand->coords.y + 1 < m_mapSize.y )
			{
				x8->coords = IntVec2( tileToExpand->coords.x + 1 , tileToExpand->coords.y + 1 );
				x8->gCost = tileToExpand->gCost + 1.4f;
				x8->hCost = /*( x4.coords - ePos ).GetLengthSquared()*/GetManhattanDistance( x8->coords , ePos );

				if ( !m_tiles[ GetTileIndexForTileCoords( x8->coords ) ].m_isSolid )
				{
					x8->isConsidered = true;
				}

				x8->parent = tileToExpand;
			}

			if ( x8->isConsidered )
			{
				bool found = false;

				for ( int i = 0; i < openList.size(); i++ )
				{
					if ( openList[ i ]->coords == x8->coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					openList.push_back( x8 );
				}
			}
		}
	}
		
	/*PathFindingHelper* foundPath = closedList.back();

	while ( foundPath->parent != nullptr )
	{
		path.push_back( GetTileIndexForTileCoords( foundPath->coords ) );
		foundPath = ( foundPath->parent );
	}*/


	PathFindingHelper foundPath = *closedList.back();
	path.push_back( GetTileIndexForTileCoords( foundPath.coords ) );


	PathFindingHelper currentParent = *foundPath.parent;
	while ( currentParent.parent != nullptr )
	{

		std::vector<PathFindingHelper> currentNodesWithSameParent;
		for ( int i = 0; i < closedList.size(); i++ )
		{
			if ( closedList[ i ]->parent == &currentParent )
			{
				currentNodesWithSameParent.push_back( *closedList[ i ] );
			}
		}

		PathFindingHelper chosenTile;
		if ( currentNodesWithSameParent.size() > 0 )
		{
			chosenTile = currentNodesWithSameParent[ 0 ];
		}
		if ( currentNodesWithSameParent.size() > 1 )
		{
			for ( int i = 1; i < currentNodesWithSameParent.size(); i++ )
			{

				if ( m_tiles[ GetTileIndexForTileCoords( currentNodesWithSameParent[ i ].coords ) ].m_influenceValue < m_tiles[ GetTileIndexForTileCoords( chosenTile.coords ) ].m_influenceValue )
				{
					chosenTile = currentNodesWithSameParent[ i ];
				}
				else if ( GetManhattanDistance( currentNodesWithSameParent[ i ].coords , ePos ) < GetManhattanDistance( chosenTile.coords , ePos ) )
				{
					chosenTile = currentNodesWithSameParent[ i ];
				}
			}
		}
		if ( chosenTile.isConsidered )
		{
			path.push_back( GetTileIndexForTileCoords( chosenTile.coords ) );
			currentParent = *chosenTile.parent;
		}
		else
		{
			path.push_back( GetTileIndexForTileCoords( currentParent.coords ) );
			currentParent = *currentParent.parent;
		}
		currentNodesWithSameParent.clear();
	}
}

float Game::GetManhattanDistance( IntVec2 currentPos , IntVec2 finalPos )
{
	return( ( abs((float)finalPos.x - (float)currentPos.x) ) + ( abs((float)finalPos.y - (float)currentPos.y) )  )/**10.f*/;
}

void Game::CreateInfluenceMap( IntVec2 startCoords , bool isPositive , int initialValue )
{
	double timeNow = GetCurrentTimeSeconds();

	struct Node
	{
		IntVec2 coords;
		bool influenceSet = false;
		bool isConsidered = false;
		int influenceValue = 0;
	};

	std::vector<Node> nodes;
	std::vector<Node> tempList;

	nodes.reserve( 10000 );
	tempList.reserve( 10000 );

	int multipliyer = -1;
	if ( !isPositive )
	{
		multipliyer = 1;
	}
	int currentValue = initialValue*multipliyer;

	Node StartNode;
	StartNode.coords = startCoords;
	StartNode.influenceSet = true;
	StartNode.influenceValue = currentValue;

	nodes.push_back( StartNode );

	m_tiles[ GetTileIndexForTileCoords( StartNode.coords ) ].m_influenceValue = currentValue;

	Node n1;
	Node n2;
	Node n3;
	Node n4;

	currentValue -= 1*multipliyer;
	
	if ( !m_tiles[ GetTileIndexForTileCoords( IntVec2( StartNode.coords.x - 1 , StartNode.coords.y ) ) ].m_isSolid )
	{
		n1.coords = IntVec2( StartNode.coords.x - 1 , StartNode.coords.y );
		n1.isConsidered = true;
		n1.influenceValue = currentValue;
	}

	if ( !m_tiles[ GetTileIndexForTileCoords( IntVec2( StartNode.coords.x + 1 , StartNode.coords.y ) ) ].m_isSolid )
	{
		n2.coords = IntVec2( StartNode.coords.x + 1 , StartNode.coords.y );
		n2.isConsidered = true;
		n2.influenceValue = currentValue;
	}

	if ( !m_tiles[ GetTileIndexForTileCoords( IntVec2( StartNode.coords.x  , StartNode.coords.y-1 ) ) ].m_isSolid )
	{
		n3.coords = IntVec2( StartNode.coords.x  , StartNode.coords.y -1 );
		n3.isConsidered = true;
		n3.influenceValue = currentValue;
	}

	if ( !m_tiles[ GetTileIndexForTileCoords( IntVec2( StartNode.coords.x  , StartNode.coords.y + 1 ) ) ].m_isSolid )
	{
		n4.coords = IntVec2( StartNode.coords.x , StartNode.coords.y + 1 );
		n4.isConsidered = true;
		n4.influenceValue = currentValue;
	}


	if ( n1.isConsidered )
	{
		m_tiles[ GetTileIndexForTileCoords( n1.coords ) ].m_influenceValue = currentValue;
		nodes.push_back( n1 );
	}

	if ( n2.isConsidered )
	{
		m_tiles[ GetTileIndexForTileCoords( n2.coords ) ].m_influenceValue = currentValue;
		nodes.push_back( n2 );
	}

	if ( n3.isConsidered )
	{
		m_tiles[ GetTileIndexForTileCoords( n3.coords ) ].m_influenceValue = currentValue;
		nodes.push_back( n3 );
	}

	if ( n4.isConsidered )
	{
		m_tiles[ GetTileIndexForTileCoords( n4.coords ) ].m_influenceValue = currentValue;
		nodes.push_back( n4 );
	}

	while ( currentValue != 0 )
	{
		Node x1;
		Node x2;
		Node x3;
		Node x4;

		for ( int index = 0; index < nodes.size(); index++ )
		{
			if ( !nodes[ index ].influenceSet )
			{
				tempList.push_back( nodes[ index ] );
				nodes[ index ].influenceSet = true;
			}
		}
		

		for ( int i = 0; i < tempList.size(); i++ )
		{
		
			if ( tempList[ i ].coords.x - 1 > 0 )
			{
				x1.coords = IntVec2( tempList[ i ].coords.x - 1 , tempList[ i ].coords.y );
				if ( !m_tiles[ GetTileIndexForTileCoords( x1.coords ) ].m_isSolid )
				{
					x1.isConsidered = true;
				}
			}

			if ( tempList[ i ].coords.x + 1 < m_mapSize.x )
			{
				x2.coords = IntVec2( tempList[ i ].coords.x + 1 , tempList[ i ].coords.y );
				if ( !m_tiles[ GetTileIndexForTileCoords( x2.coords ) ].m_isSolid )
				{
					x2.isConsidered = true;
				}
			}

			if ( tempList[ i ].coords.y - 1 > 0 )
			{
				x3.coords = IntVec2( tempList[ i ].coords.x , tempList[ i ].coords.y - 1 );
				if ( !m_tiles[ GetTileIndexForTileCoords( x3.coords ) ].m_isSolid )
				{
					x3.isConsidered = true;
				}
			}

			if ( tempList[ i ].coords.y + 1 < m_mapSize.y )
			{
				x4.coords = IntVec2( tempList[ i ].coords.x , tempList[ i ].coords.y + 1 );
				if ( !m_tiles[ GetTileIndexForTileCoords( x4.coords ) ].m_isSolid )
				{
					x4.isConsidered = true;
				}
			}

			if ( x1.isConsidered )
			{
				bool found = false;
				for ( int j = 0; j < nodes.size(); j++ )
				{
					if ( nodes[ j ].coords == x1.coords )
					{
						found = true;
						break;
					}
				
				}

				for ( int j = 0; j < tempList.size(); j++ )
				{
					if ( tempList[ j ].coords == x1.coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					x1.influenceValue = currentValue - (1*multipliyer);
					nodes.push_back( x1 );
					m_tiles[ GetTileIndexForTileCoords( x1.coords ) ].m_influenceValue = currentValue - ( 1 * multipliyer );
				}
			}

			if ( x2.isConsidered )
			{
				bool found = false;
				for ( int j = 0; j < nodes.size(); j++ )
				{
					if ( nodes[ j ].coords == x2.coords )
					{
						found = true;
						break;
					}
				}

				for ( int j = 0; j < tempList.size(); j++ )
				{
					if ( tempList[ j ].coords == x2.coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					x2.influenceValue = currentValue - ( 1 * multipliyer );
					nodes.push_back( x2 );
					m_tiles[ GetTileIndexForTileCoords( x2.coords ) ].m_influenceValue = currentValue - ( 1 * multipliyer );
				}
			}

			if ( x3.isConsidered )
			{
				bool found = false;
				for ( int j = 0; j < nodes.size(); j++ )
				{
					if ( nodes[ j ].coords == x3.coords )
					{
						found = true;
						break;
					}
				}

				for ( int j = 0; j < tempList.size(); j++ )
				{
					if ( tempList[ j ].coords == x3.coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					x3.influenceValue = currentValue - ( 1 * multipliyer );
					nodes.push_back( x3 );
					m_tiles[ GetTileIndexForTileCoords( x3.coords ) ].m_influenceValue = currentValue - ( 1 * multipliyer );
				}
			}

			if ( x4.isConsidered )
			{
				bool found = false;
				for ( int j = 0; j < nodes.size(); j++ )
				{
					if ( nodes[ j ].coords == x4.coords )
					{
						found = true;
						break;
					}
				}

				for ( int j = 0; j < tempList.size(); j++ )
				{
					if ( tempList[ j ].coords == x4.coords )
					{
						found = true;
						break;
					}
				}

				if ( !found )
				{
					x4.influenceValue = currentValue - ( 1 * multipliyer );
					nodes.push_back( x4 );
					m_tiles[ GetTileIndexForTileCoords( x4.coords ) ].m_influenceValue = currentValue - ( 1 * multipliyer );
				}
			}

			
		}
		tempList.clear();
		currentValue -= 1*multipliyer;
	}

	double timeFinisehd = GetCurrentTimeSeconds();

	influnceMapTime = timeFinisehd - timeNow;
}

void Game::GetAllTilesWithSameFCost( std::vector<PathFindingHelper>& list , std::vector<PathFindingHelper>& outList )
{
	PathFindingHelper* tileToExpand = new PathFindingHelper();
	tileToExpand->coords = list[ 0 ].coords;
	tileToExpand->hCost = list[ 0 ].hCost;
	tileToExpand->gCost = list[ 0 ].gCost;
	tileToExpand->isConsidered = list[ 0 ].isConsidered;
	tileToExpand->parent = list[ 0 ].parent;

	for ( int i = 0; i < list.size(); i++ )
	{
		//tileToExpand = openList[ i ];
		for ( int j = 0; j < list.size(); j++ )
		{
			if ( list[ j ].coords != tileToExpand->coords )
			{
				if ( ( list[ j ].gCost + list[ j ].hCost ) < ( tileToExpand->gCost + tileToExpand->hCost ) )
				{
					tileToExpand->coords = list[ j ].coords;
					tileToExpand->hCost = list[ j ].hCost;
					tileToExpand->gCost = list[ j ].gCost;
					tileToExpand->isConsidered = list[ j ].isConsidered;
					tileToExpand->parent = list[ j ].parent;
				}
			}
		}
	}

	outList.push_back( *tileToExpand );

	for ( int i = 0; i < list.size(); i++ )
	{
		if ( list[ i ].coords != tileToExpand->coords  && (list[i].gCost+list[i].hCost) == (tileToExpand->hCost+tileToExpand->gCost)  )
		{
			PathFindingHelper* newHelper = new PathFindingHelper();
			newHelper->coords = list[ i ].coords;
			newHelper->hCost = list[ i ].hCost;
			newHelper->gCost = list[ i ].gCost;
			tileToExpand->isConsidered = list[ i ].isConsidered;
			tileToExpand->parent = list[ i ].parent;

			outList.push_back( *newHelper );
		}
	}
}

bool Game::IsPathFindingHelpInList( PathFindingHelper toFind , std::vector<PathFindingHelper>& list )
{
	bool found = false;
	for ( int i = 0; i < list.size(); i++ )
	{
		if ( toFind.coords == list[ i ].coords )
		{
			found = true;
			break;
		}
	}

	return found;
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

int Game::GetTileIndexForTileCoordsForMainMap( const IntVec2& tileCoords )
{
	return tileCoords.x + ( m_mainMapSize.x * tileCoords.y );
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

void Game::LoadDataFromXml()
{
	tinyxml2::XMLDocument gameDoc;
	gameDoc.LoadFile( "Data/GameConfig/GameData.xml" );


	tinyxml2::XMLElement* ele = gameDoc.RootElement();

	//tinyxml2::XMLNode* node = gameDoc.FirstChild();

	for ( tinyxml2::XMLElement* e = ele->FirstChildElement( "Tile" ); e != nullptr; e = e->NextSiblingElement( "Tile" ) )
	{
		std::string index = ParseXmlAttribute( *e , "index" , "NULL" );

		if ( index != "NULL" )
		{
			int i = atoi( index.c_str() );

			m_mainMapTiles[ i ].m_isSolid = true;
		}
	}
	
}

void Game::LoadPotentialFieldFromXML()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/GameConfig/PotentialFieldData.xml" );

	m_potentialField->nodes.clear();

	tinyxml2::XMLElement* root = doc.FirstChildElement( "PotentialFieldNodes" );

	for ( tinyxml2::XMLElement* ele = root->FirstChildElement( "Node" ); ele != nullptr; ele = ele->NextSiblingElement( "Node" ) )
	{
		PotentialFieldNode n;
		n.xCoord = ParseXmlAttribute( *ele , "xCoord" , -1 );
		n.yCoord = ParseXmlAttribute( *ele , "yCoord" , -1 );
		n.isConsiderd = ParseXmlAttribute( *ele , "isConsidered" , false );
		n.isCore = ParseXmlAttribute( *ele , "isCore" , false );
		n.direction = ParseXmlAttribute( *ele , "direction" , -1.f );

		m_potentialField->nodes.push_back( n );
	}

	tinyxml2::XMLElement* e1 = root->FirstChildElement( "Position" );
	m_potentialField->m_startPosition.x = ParseXmlAttribute( *e1 , "posX" , 0 );
	m_potentialField->m_startPosition.y = ParseXmlAttribute( *e1 , "posY" , 0 );

	m_potentialField->ConvertToLocalSpace();
	m_potentialField->ResizeAndRemoveGarbageNodes();
	//m_potentialField->Rotate( 60.f );
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

	m_player1IdleMeleeTex = new SpriteAnimDefTex(0,(int)player1IdleKnifeTextures.size()-1,1.f);
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

	m_player1WalkMeleeTex = new SpriteAnimDefTex( 0 , (int)player1WalkKnifeTextures.size() - 1 , 1.f );
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

	m_player1AttackTex = new SpriteAnimDefTex( 0 , (int)player1AttackTextures.size() - 1 , 1.f );
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

	m_player1IdleGunTex = new SpriteAnimDefTex( 0 , (int)playerIdleGunTextures.size() - 1 , 1.f );
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

	m_player1WalkGunTex = new SpriteAnimDefTex( 0 , (int)playerWalkGunTextures.size() - 1 , 1.f );
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

	m_player1AttackGunTex = new SpriteAnimDefTex( 0 , (int)playerAttackGunTextures.size() - 1 , 1.f );
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

	m_supportPlayerIdleTex = new SpriteAnimDefTex( 0 , (int)playerIdleTextures.size() - 1 , 1.f );
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

	m_supportPlayerAttackTex = new SpriteAnimDefTex( 0 , (int)playerAttackTextures.size() - 1 , 1.f );
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

	m_supportPlayerWalkTex = new SpriteAnimDefTex( 0 , (int)playerWalkTextures.size() - 1 , 1.f );
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

	m_supportPlayerDisguiseWalk = new SpriteAnimDefTex( 0 , (int)disguiseIdleTex.size() - 1 , 1.f );
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
	
	m_supportPlayerDisguiseAttack = new SpriteAnimDefTex( 0 , (int)disguiseAttackTex.size() - 1 , 1.f );
	m_supportPlayerDisguiseAttack->m_animations = disguiseAttackTex;
}



void Game::UpdateCamera()
{
	Vec2 camCoords;
	float xFactor = 8.f*4.f;
	float yFactor = 4.5f*4.f;

	if ( m_currentMode == GAME || m_currentMode == MAPCREATOR )
	{
	
		AABB2 aabb = AABB2( m_gameCamera->GetOrthoBottomLeft() , m_gameCamera->GetOrthoTopRight() );
		AABB2 aabb1 = aabb.GetBoxAtLeft( 0.1f );
		AABB2 aabb2 = aabb.GetBoxAtRight( 0.1f );
		AABB2 aabb3 = aabb.GetBoxAtBottom( 0.1f );
		AABB2 aabb4 = aabb.GetBoxAtTop( 0.1f );
		
		if ( aabb1.IsPointInside( m_mousePosition ) )
		{
			m_gameCamera->Translate( Vec3( -0.5f , 0.f, 0.f ) );
		}

		if ( aabb2.IsPointInside( m_mousePosition ) )
		{
			m_gameCamera->Translate( Vec3(0.5f,0.f,0.f) );
		}

		if ( aabb3.IsPointInside( m_mousePosition ) )
		{
			m_gameCamera->Translate( Vec3( 0.f , -0.5f , 0.f ) );
		}

		if ( aabb4.IsPointInside( m_mousePosition ) )
		{
			m_gameCamera->Translate( Vec3( 0.f , 0.5f , 0.f ) );
		}


		m_gameCamera->m_transform.m_position.x = Clamp( m_gameCamera->m_transform.m_position.x , 0.f , 320.f * 0.51f );
		m_gameCamera->m_transform.m_position.y = Clamp( m_gameCamera->m_transform.m_position.y , 0.f , 720.f * 0.881f);


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
