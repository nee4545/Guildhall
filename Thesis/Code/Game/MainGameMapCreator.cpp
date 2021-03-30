#include "Game/MainGameMapCreator.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Game/MonsterAI.hpp"
#include "Game/OccAI.hpp"
#include "Game/InfluenceMap.hpp"
#include "Game/Bomb.hpp"
#include "Game/PotentialFieldPathFinder.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ThirdParty/ImGuiSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
extern ImGuiSystem* g_theGUI;

MainGameMapCreator::MainGameMapCreator(Game* game)
{
	m_game = game;
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 80.f, 45.f) );
	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	m_debugCamera = new Camera();
	m_debugCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 80.f , 45.f ) );
	//m_debugCamera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	PopulateTiles();
	
	m_sandTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/sand/sand_0012_tile.png" );

	m_groundTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/block.png" );
	m_waterTileType1Main = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Water_tiles/water_tiles_0005_Layer-24.png" );
	m_lavaTileTypeMain = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/water/water_0012_tile.png" );
	m_lavaTileTypeMain1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/sand/lava_tile2.png" );
	m_monsterBaseTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Objects/2.png" );


	m_groundUpTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/stones_0011_tile.png" );
	m_groundBotTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/stones_0005_tile.png" );
	m_groundLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/stones_0009_tile.png" );
	m_groundRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/stones_0006_tile.png" );
	m_groundTopRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/stones_0010_tile.png" );
	m_groundTopLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/stones_0012_tile.png" );
	m_groundBottomRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/stones_0004_tile.png" );
	m_groundBottomLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/stones_0007_tile.png" );


    m_waterUpTex = g_theRenderer->GetOrCreateTextureFromFile("Data/GameAssets/Tiles/Water_tiles/water_tiles_0001_Layer-28.png");
    m_waterBotTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Water_tiles/water_tiles_0006_Layer-23.png" );
    m_waterLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Water_tiles/water_tiles_0003_Layer-26.png" );
    m_waterRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Water_tiles/water_tiles_0007_Layer-22.png" );
    m_waterTopRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Water_tiles/water_tiles_0002_Layer-27.png" );
    m_waterTopLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Water_tiles/water_tiles_0000_Layer-29.png" );
    m_waterBottomRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Water_tiles/water_tiles_0008_Layer-21.png" );
    m_waterBottomLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Water_tiles/water_tiles_0004_Layer-25.png" );


	m_lavaUpTex = g_theRenderer->GetOrCreateTextureFromFile("Data/GameAssets/Tiles/water/water_0009_tile.png");
	m_lavaBotTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/water/water_0004_tile.png" );
	m_lavaLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/water/water_0011_tile.png" );
	m_lavaRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/water/water_0007_tile.png" );
	m_lavaTopRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/water/water_0008_tile.png" );
	m_lavaTopLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/water/water_0010_tile.png" );
	m_lavaBottomRightTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/water/water_0005_tile.png" );
	m_lavaBottomLeftTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/water/water_0006_tile.png" );

	m_player = m_game->m_player;
	m_player->m_position = Vec2( 5.5f , 40.f );
	m_player->m_isActive = true;

	LoadData();
	UpdateTileState();

	m_fps_box = AABB2(74.f,43.f,78.f,45.f);
	m_fps_box.Translate( Vec2(2.f,0.5f) );

	m_game->m_occMapTiles = m_tiles;
	m_game->m_occAIs = m_occAis;

	m_finder = new PotentialFieldPathFinder( m_game , m_tiles );
	m_finder->GenerateCostFields();
	m_finder->GenerateIntegrationFields();
	m_finder->GenerateFlowFields();
}

MainGameMapCreator::~MainGameMapCreator()
{
	
}

void MainGameMapCreator::PopulateTiles()
{
	int numTiles = m_mapSize.x* m_mapSize.y;

	for ( int i = 0; i < numTiles; i++ )
	{
		int tileX = i % m_mapSize.x;
		int tileY = i / m_mapSize.x;

		Tile newTile = Tile( tileX , tileY , TILE_TYPE_SAND );
		m_tiles.push_back( newTile );
	}
}

int MainGameMapCreator::GetTileIndexForCoords(IntVec2 coords)
{
	return coords.x + ( m_mapSize.x * coords.y );
}

void MainGameMapCreator::Update( float deltaSeconds )
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	m_mousePosition.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	m_mousePosition.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	UpdateCamera();


	if ( m_updateAI )
	{
		for ( int i = 0; i < m_ais.size(); i++ )
		{
			m_ais[ i ]->Update( deltaSeconds );
		}
	}

	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		m_gamePaused = !m_gamePaused;
		if ( m_gamePaused )
		{
			m_camera->SetOrthoView( Vec2() , Vec2(80.f,45.f) );
		}
	}

	if ( !m_gamePaused )
	{
		m_player->Update( deltaSeconds );

		for ( int i = 0; i < m_ais.size(); i++ )
		{
			if ( m_ais[ i ] == nullptr )
			{
				continue;
			}
			m_ais[ i ]->Update( deltaSeconds );
		}

		for ( int i = 0; i < m_occAis.size(); i++ )
		{
			if ( m_occAis[ i ] == nullptr )
			{
				continue;
			}

			m_occAis[ i ]->Update( deltaSeconds );
		}

		for ( int i = 0; i < m_bombs.size(); i++ )
		{
			if ( m_bombs[ i ] == nullptr )
			{
				continue;
			}

			m_bombs[ i ]->Update( deltaSeconds );
		}

		HandleBlockCollissions( m_player );
		for ( int i = 0; i < m_occAis.size(); i++ )
		{
			if ( m_occAis[ i ] == nullptr )
			{
				continue;
			}

			HandleBlockCollissions( m_occAis[ i ] );
		}
	}


	if ( m_gamePaused )
	{

		if ( g_theInput->WasRightMouseButtonJustPressed() )
		{
			if ( m_currentSelection == PAINT_INFLUENCE_MAP )
			{
				InfluenceMap* infMap = new InfluenceMap( m_game , IntVec2( m_mousePosition.x , m_mousePosition.y ) , m_infMapDimensions , m_infMapCurrentStartValue , m_infMapCurrentFalloff );
				infMap->Create();
				m_infMaps.push_back( infMap );
				return;
			}

			if ( m_currentSelection == PAINT_MONSTER_BASE )
			{

				AABB2* aabb = new AABB2( Vec2( 0.f , 0.f ) , Vec2( m_monsterBasedimensions.x , m_monsterBasedimensions.y ) );
				aabb->SetCenter( m_mousePosition );

				m_monsterBases.push_back( aabb );

			}

			if ( m_currentSelection == PAINT_AI_TYPE_1 )
			{
				MonsterAI* ai = new MonsterAI( m_game , TYPE_1 , nullptr , m_game->m_aiAnimWalk1 , m_game->m_aiAnimMeleeAttack1 );
				ai->m_position = m_mousePosition;

				m_ais.push_back( ai );
			}

			if ( m_currentSelection == PAINT_AI_TYPE_2 )
			{
				MonsterAI* ai = new MonsterAI( m_game , TYPE_2 , nullptr , m_game->m_aiAnimWalk2 , m_game->m_aiAnimMeleeAttack2 );
				ai->m_position = m_mousePosition;

				m_ais.push_back( ai );
			}

			if ( m_currentSelection == PAINT_AI_TYPE_3 )
			{
				MonsterAI* ai = new MonsterAI( m_game , TYPE_3 , nullptr , m_game->m_aiAnimWalk3 , m_game->m_aiAnimMeleeAttack3 );
				ai->m_position = m_mousePosition;

				m_ais.push_back( ai );
			}

			if ( m_currentSelection == DEBUG_POTENTIAL_FINDER_COST )
			{
				m_currentFinderTileIndex = GetTileIndexForCoords( IntVec2( m_mousePosition.x , m_mousePosition.y ) );
			}

			if ( m_currentSelection == DEBUG_POTENTIAL_FNDER_INTEGRATION_COST )
			{
				m_currentFinderTileIndex = GetTileIndexForCoords( IntVec2( m_mousePosition.x , m_mousePosition.y ) );
			}

			if ( m_currentSelection == DEBUG_FLOW_FIELDS )
			{
				m_currentFinderTileIndex = GetTileIndexForCoords( IntVec2( m_mousePosition.x , m_mousePosition.y ) );
			}

			if ( m_currentSelection == SET_PLAYER_POSITION )
			{
				m_player->m_position = m_mousePosition;
			}

			if ( m_currentSelection == PAINT_OCC_AI )
			{
				OccAI* ai = new OccAI();
				ai->m_game = m_game;
				ai->m_type = m_currentType;
				ai->m_position = m_mousePosition;

				m_occAis.push_back( ai );

				m_game->m_occAIs = m_occAis;
			}
		}

		if ( g_theInput->IsRightMouseButtonPressed() )
		{
			SetTileType( m_currentSelection , IntVec2( m_mousePosition.x , m_mousePosition.y ) );
			UpdateTileState();
		}

		ImGui::NewFrame();
		ImGui::Begin( "MAP PAINTER" );
		ImGui::Text( "Current Selection" );
		ImGui::Text( m_currentSelectedText.c_str() );

		if ( ImGui::CollapsingHeader( "Paint Options" ) )
		{

			if ( ImGui::Button( "Paint Sand Tile" ) )
			{
				m_currentSelection = PAINT_SAND_TILE;
				m_currentSelectedText = "Paint Sand Tile";
			}
			if ( ImGui::Button( "Paint Solid Tile" ) )
			{
				m_currentSelection = PAINT_SOLID_TILE;
				m_currentSelectedText = "Paint Solid Tile";
			}
			if ( ImGui::Button( "Paint Water Tile" ) )
			{
				m_currentSelection = PAINT_WATER_TILE;
				m_currentSelectedText = "Paint Water Tile";
			}
			if ( ImGui::Button( "Paint Lava Tile" ) )
			{
				m_currentSelection = PAINT_LAVA_TILE;
				m_currentSelectedText = "Paint Lava Tile";
			}

			if ( m_currentSelection == PAINT_WATER_TILE || m_currentSelection == PAINT_LAVA_TILE )
			{
				if ( ImGui::CollapsingHeader( "Choose Tile Sub-Type" ) )
				{

					if ( ImGui::Button( "Default" ) )
					{
						m_currentSubSelection = PAINT_DEFAULT;
					}

					if ( ImGui::Button( "Top" ) )
					{
						m_currentSubSelection = PAINT_UP;
					}

					if ( ImGui::Button( "Bottom" ) )
					{
						m_currentSubSelection = PAINT_DOWN;
					}

					if ( ImGui::Button( "Left" ) )
					{
						m_currentSubSelection = PAINT_LEFT;
					}

					if ( ImGui::Button( "Right" ) )
					{
						m_currentSubSelection = PAINT_RIGHT;
					}

					if ( ImGui::Button( "Top Right" ) )
					{
						m_currentSubSelection = PAINT_TOPRIGHT;
					}

					if ( ImGui::Button( "Top Left" ) )
					{
						m_currentSubSelection = PAINT_TOPLEFT;
					}

					if ( ImGui::Button( "Bottom Right" ) )
					{
						m_currentSubSelection = PAINT_BOTRIGHT;
					}

					if ( ImGui::Button( "Bottom Left" ) )
					{
						m_currentSubSelection = PAINT_BOTLEFT;
					}
				}

			}

			if ( ImGui::Button( "Paint Influence Map" ) )
			{
				m_currentSelection = PAINT_INFLUENCE_MAP;
				m_currentSelectedText = "Paint Influence Map";
			}
			if ( ImGui::Button( "Paint Monster Bases" ) )
			{
				m_currentSelection = PAINT_MONSTER_BASE;
				m_currentSelectedText = "Paint Monster Base";
			}
			if ( m_currentSelection == PAINT_INFLUENCE_MAP )
			{
				ImGui::Text( "Influence Map Controls" );
				ImGui::SliderInt( "StartValue" , &m_infMapCurrentStartValue , -10 , 10 );
				ImGui::InputInt( "XDimensions" , &m_infMapDimensions.x );
				ImGui::InputInt( "YDimensions" , &m_infMapDimensions.y );
				ImGui::SliderInt( "FallOff" , &m_infMapCurrentFalloff , -3 , 3 );
			}
			if ( m_currentSelection == PAINT_MONSTER_BASE )
			{
				ImGui::Text( "Monster Base Controls" );
				ImGui::SliderInt( "XDimensions" , &m_monsterBasedimensions.x , 0 , 10 );
				ImGui::SliderInt( "YDimensions" , &m_monsterBasedimensions.y , 0 , 10 );
			}
		}

		if ( ImGui::CollapsingHeader( "Camera Controls" ) )
		{
			ImGui::SliderInt( "Zoom Factor" , &m_zoomFactor , 1 , 10 );
			ImGui::SliderFloat( "Camera Position X" , &m_camerapos.x , 0.f , 80.f );
			ImGui::SliderFloat( "Camera Position Y" , &m_camerapos.y , 0.f , 45.f );
		}


		if ( ImGui::CollapsingHeader( "Potential Field Path Debug" ) )
		{

			if ( ImGui::Button( "Debug Flow Fields" ) )
			{
				m_currentSelection = DEBUG_FLOW_FIELDS;
				m_currentSelectedText = "Debug flow fields";
				m_debugPFFcost != m_debugPFFcost;
			}

			if ( ImGui::Button( "Debug Potential Field PathFinder Costs" ) )
			{
				m_currentSelection = DEBUG_POTENTIAL_FINDER_COST;
				m_currentSelectedText = "Debug PFF Costs";
				m_debugPFFcost = !m_debugPFFcost;
			}

			if ( ImGui::Button( "Debug Integration Field Costs" ) )
			{
				m_currentSelection = DEBUG_POTENTIAL_FNDER_INTEGRATION_COST;
				m_currentSelectedText = "Debug Integration Field Costs";
				m_debugPFFcost != m_debugPFFcost;
			}
		}

		if ( ImGui::CollapsingHeader( "Player and AI Options" ) )
		{
			if ( ImGui::Button( "Set Player Position" ) )
			{
				m_currentSelection = SET_PLAYER_POSITION;
				m_currentSelectedText = "Set Player Position";
			}

			if ( ImGui::Button( "Paint Occ AI" ) )
			{
				m_currentSelection = PAINT_OCC_AI;
			}

			if ( m_currentSelection == PAINT_OCC_AI )
			{
				if ( ImGui::Button( "Single Occupancy Map" ) )
				{
					m_currentType = OCC_AI_SINGLE_MAP;
				}

				if ( ImGui::Button( "Shared Occupancy Map" ) )
				{
					m_currentType = OCC_AI_SHARED_MAP;
				}
			}

			if ( ImGui::Button( "Paint AI Type 1" ) )
			{
				m_currentSelection = PAINT_AI_TYPE_1;
				m_currentSelectedText = "Paint AI type 1";
			}

			if ( ImGui::Button( "Paint AI Type 2" ) )
			{
				m_currentSelection = PAINT_AI_TYPE_2;
				m_currentSelectedText = "Paint AI type 2";
			}

			if ( ImGui::Button( "Paint AI Type 3" ) )
			{
				m_currentSelection = PAINT_AI_TYPE_3;
				m_currentSelectedText = "Paint AI type 3";
			}

		}


		if ( ImGui::CollapsingHeader( "Misc Options" ) )
		{
			if ( ImGui::Button( "Update AI" ) )
			{
				m_updateAI = !m_updateAI;
			}

			if ( ImGui::Button( "Save Data" ) )
			{
				SaveData();
			}
		}

		if ( ImGui::CollapsingHeader( "Debug Options" ) )
		{
			ImGui::Checkbox( "Debug AI influence Maps" , &debug_AI_InfluenceMaps );
			ImGui::Checkbox( "Debug Map influence Maps" , &debug_Map_InfluenceMaps );
		}

		ImGui::End();

	}
	
}

void MainGameMapCreator::UpdateCamera()
{
	if ( !m_gamePaused )
	{
		Vec2 playerPos = m_player->m_position;
		Vec2 camCoords;
		camCoords.x = Clamp( playerPos.x , 8.f * 1.75f , 80.f - ( 8.f * 1.75f ) );
		camCoords.y = Clamp( playerPos.y , 4.5f * 1.75f , 45.f - ( 4.5f * 1.75f ) );

		m_camera->SetOrthoView( camCoords - Vec2( 8.f * 1.75f , 4.5f * 1.75f ) , camCoords + Vec2( 8.f * 1.75f , 4.5f * 1.75f ) );
	}
	else
	{
		m_camera->SetOrthoView( m_camerapos + ( Vec2( 0.f , 0.f ) / m_zoomFactor ) , m_camerapos + Vec2( 80.f , 45.f ) / m_zoomFactor );
	}
}

void MainGameMapCreator::UpdateTileState()
{

	m_mainTiles.clear();
	m_solidTiles.clear();

	m_waterTiles.clear();
	m_waterBotTiles.clear();
	m_waterTopTiles.clear();
	m_waterLeftTiles.clear();
	m_waterRightTiles.clear();
	m_waterTopRightTiles.clear();
	m_waterBottomRightTiles.clear();
	m_waterTopLeftTiles.clear();
	m_waterBottomLeftTiles.clear();


	m_lavaTiles.clear();
	m_lavaBotTiles.clear();
	m_lavaTopTiles.clear();
	m_lavaLeftTiles.clear();
	m_lavaRightTiles.clear();
	m_lavaTopRightTiles.clear();
	m_lavaBottomRightTiles.clear();
	m_lavaTopLeftTiles.clear();
	m_lavaBottomLeftTiles.clear();
	

	for ( int i = 0; i < m_tiles.size(); i++ )
	{

		AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
		AppendAABB2( m_mainTiles , aabb , Rgba8() );


		if ( m_tiles[ i ].m_type == TILE_TYPE_STONE )
		{
			AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );

			if ( m_tiles[ i ].m_subType == TILE_TYPE_NONE )
			{
				AppendAABB2( m_solidTiles , aabb , Rgba8() );
			}
		}

		if ( m_tiles[ i ].m_type == TILE_TYPE_LAVA )
		{
			AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
			Rgba8 color = Rgba8( 255 , 50 , 0 , 150 );


			if ( m_tiles[ i ].m_subType == TILE_TYPE_NONE )
			{
				AppendAABB2( m_lavaTiles , aabb , color );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_UP )
			{
				AppendAABB2( m_lavaTopTiles , aabb , color );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_DOWN )
			{
				AppendAABB2( m_lavaBotTiles , aabb , color );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_LEFT )
			{
				AppendAABB2( m_lavaLeftTiles , aabb , color );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_RIGHT )
			{
				AppendAABB2( m_lavaRightTiles , aabb , color );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_TOP_LEFT )
			{
				AppendAABB2( m_lavaTopLeftTiles , aabb , color );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_TOP_RIGHT )
			{
				AppendAABB2( m_lavaTopRightTiles , aabb , color );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_BOT_RIGHT )
			{
				AppendAABB2( m_lavaBottomRightTiles , aabb , color );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_BOT_LEFT )
			{
				AppendAABB2( m_lavaBottomLeftTiles , aabb , color );
			}
		}

		if ( m_tiles[ i ].m_type == TILE_TYPE_WATER )
		{
			AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
			if ( m_tiles[ i ].m_subType == TILE_TYPE_NONE )
			{
				AppendAABB2( m_waterTiles , aabb , Rgba8() );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_UP )
			{
				AppendAABB2( m_waterTopTiles , aabb , Rgba8() );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_DOWN )
			{
				AppendAABB2( m_waterBotTiles , aabb , Rgba8() );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_LEFT )
			{
				AppendAABB2( m_waterLeftTiles , aabb , Rgba8() );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_RIGHT )
			{
				AppendAABB2( m_waterRightTiles , aabb , Rgba8() );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_TOP_LEFT )
			{
				AppendAABB2( m_waterTopLeftTiles , aabb , Rgba8() );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_TOP_RIGHT )
			{
				AppendAABB2( m_waterTopRightTiles , aabb , Rgba8() );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_BOT_RIGHT )
			{
				AppendAABB2( m_waterBottomRightTiles , aabb , Rgba8() );
			}

			if ( m_tiles[ i ].m_subType == TILE_TYPE_BOT_LEFT )
			{
				AppendAABB2( m_waterBottomLeftTiles , aabb , Rgba8() );
			}


		}

	}
}

void MainGameMapCreator::SpawnBomb( Vec2 position )
{
	Bomb* b = new Bomb( m_game , position , m_game->m_bombIdleTex , m_game->m_explosion );
	m_bombs.push_back( b );
}

void MainGameMapCreator::HandleBlockCollissions( Entity* entity )
{
	Vec2 entityPosition = entity->m_position;

	IntVec2 current_tileCoords = IntVec2( RoundDownToInt( entityPosition.x ) , RoundDownToInt( entityPosition.y ) );

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
		PushDiscOutOFAABB2D( entityPosition , .4f , leftAABB );
	}
	if ( IsTileSolid( rightCoords ) )
	{
		PushDiscOutOFAABB2D( entityPosition , .4f , rightAABB );
	}
	if ( IsTileSolid( upCoords ) )
	{
		PushDiscOutOFAABB2D( entityPosition , .4f , upAABB );
	}
	if ( IsTileSolid( downCoords ) )
	{
		PushDiscOutOFAABB2D( entityPosition , .4f , downAABB );
	}
	if ( IsTileSolid( topRightCoords ) )
	{
		PushDiscOutOFAABB2D( entityPosition , .4f , topRightAABB );
	}
	if ( IsTileSolid( topLeftCoords ) )
	{
		PushDiscOutOFAABB2D( entityPosition , .4f , topLeftAABB );
	}
	if ( IsTileSolid( bottomRightCoords ) )
	{
		PushDiscOutOFAABB2D( entityPosition , .4f , bottomRightAABB );
	}
	if ( IsTileSolid( bottomLeftCoords ) )
	{
		PushDiscOutOFAABB2D( entityPosition , .4f , bottomLeftAABB );
	}

	entity->m_position = entityPosition;
}

bool MainGameMapCreator::IsTileSolid( IntVec2 coords )
{
	int index = GetTileIndexForCoords( coords );
	return m_tiles[ index ].m_isSolid;
}

void MainGameMapCreator::Render()
{
	
	std::vector<Vertex_PCU> m_infMapTiles;
	std::vector<Vertex_PCU> m_infMapValues;
	std::vector<Vertex_PCU> m_combinedMapValues;
	std::vector<Vertex_PCU> monsterBases;

	InfluenceMap combinedMap = InfluenceMap( m_game , IntVec2( 40 , 22 ) , IntVec2( 80 , 45 ) , 0.f , 0.f );
	combinedMap.Create();
	combinedMap.ClearInfluence();

	//for ( int i = 0; i < m_infMaps.size(); i++ )
	//{
	//	//combinedMap->AddFromMap( m_infMaps[ i ] );
	//}

	if ( debug_AI_InfluenceMaps )
	{
		for ( int i = 0; i < m_ais.size(); i++ )
		{
			InfluenceMap m = InfluenceMap( m_game , IntVec2( m_ais[ i ]->m_position.x , m_ais[ i ]->m_position.y ) , IntVec2( 6 , 6 ) , -6.f , -1.f );
			m.Create();

			combinedMap.AddFromMap( &m );

		}

		for ( int i = 0; i < m_monsterBases.size(); i++ )
		{
			InfluenceMap m = InfluenceMap( m_game , IntVec2( m_monsterBases[ i ]->GetCenter().x , m_monsterBases[ i ]->GetCenter().y ) , IntVec2( 4 , 4 ) , 4.f );
			m.Create();

			for ( int j = 0; j < m.m_nodes.size(); j++ )
			{
				if ( m.m_nodes[ j ].influenceValue > 2.f )
				{
					m.m_nodes[ j ].influenceValue = 0.f;
				}
			}

			combinedMap.AddFromMap( &m );
		}
	}

	g_theRenderer->BeginCamera( *m_camera );

	

	for ( int i = 0; i < m_infMaps.size(); i++ )
	{
		for ( int j = 0; j < m_infMaps[ i ]->m_nodes.size(); j++ )
		{
			AABB2 aabb = AABB2( m_infMaps[ i ]->m_nodes[ j ].coords.x , m_infMaps[ i ]->m_nodes[ j ].coords.y , m_infMaps[ i ]->m_nodes[ j ].coords.x + 1 , m_infMaps[ i ]->m_nodes[ j ].coords.y + 1 );
			AppendAABB2( m_infMapTiles,aabb , Rgba8( 100 , 100 , 100 , 50 ) );
		}
	}

	/*for ( int i = 0; i < m_infMaps.size(); i++ )
	{
		for ( int j = 0; j < m_infMaps[ i ]->m_nodes.size(); j++ )
		{
			AABB2 aabb = AABB2( m_infMaps[ i ]->m_nodes[ j ].coords.x , m_infMaps[ i ]->m_nodes[ j ].coords.y , m_infMaps[ i ]->m_nodes[ j ].coords.x + 1 , m_infMaps[ i ]->m_nodes[ j ].coords.y + 1 );
			m_game->m_font->AddVertsForTextInBox2D( m_infMapValues , aabb , 0.6f , std::to_string((int) m_infMaps[ i ]->m_nodes[ j ].influenceValue ) , Rgba8(0,0,0,255) , 0.6f , Vec2( 0.2f , 0.2f ) );
		}
	}*/

	for ( int i = 0; i < m_infMaps.size(); i++ )
	{
		for ( int j = 0; j < m_infMaps[ i ]->m_nodes.size(); j++ )
		{
			AABB2 aabb = AABB2( m_infMaps[ i ]->m_nodes[ j ].coords.x , m_infMaps[ i ]->m_nodes[ j ].coords.y , m_infMaps[ i ]->m_nodes[ j ].coords.x + 1 , m_infMaps[ i ]->m_nodes[ j ].coords.y + 1 );
			Rgba8 blue = Rgba8( 0 , 0 , 100 , 255 );
			Rgba8 red = Rgba8( 100 , 0 , 0 , 255 );
			
			if ( m_infMaps[ i ]->m_nodes[ j ].influenceValue > 0 )
			{
				m_game->m_font->AddVertsForTextInBox2D( m_infMapValues , aabb , 0.6f , std::to_string( ( int ) m_infMaps[ i ]->m_nodes[ j ].influenceValue ) , blue , 0.6f , Vec2(0.2f,0.2f) );
			}
			else
			{
				m_game->m_font->AddVertsForTextInBox2D( m_infMapValues , aabb , 0.6f , std::to_string( ( int ) m_infMaps[ i ]->m_nodes[ j ].influenceValue ) , red , 0.6f , Vec2( 0.2f , 0.2f ) );
			}
		}
	}

	for ( int i = 0; i < combinedMap.m_nodes.size(); i++ )
	{
		if ( combinedMap.m_nodes[ i ].influenceValue != 0 )
		{
			AABB2 aabb = AABB2( combinedMap.m_nodes[i].coords.x , combinedMap.m_nodes[ i ].coords.y , combinedMap.m_nodes[ i ].coords.x +1 , combinedMap.m_nodes[ i ].coords.y+1 );
			m_game->m_font->AddVertsForTextInBox2D( m_combinedMapValues , aabb , 0.6f , std::to_string( ( int ) combinedMap.m_nodes[ i ].influenceValue) , Rgba8( 100 , 0 , 0 , 255 ) , 0.6f , Vec2(0.2f,0.2f) );
		}
	}


	g_theRenderer->BindTexture( m_sandTex );
	g_theRenderer->DrawVertexArray( m_mainTiles );

	g_theRenderer->BindTexture( m_groundTex );
	g_theRenderer->DrawVertexArray( m_solidTiles );


	g_theRenderer->BindTexture( m_waterTileType1Main );
	g_theRenderer->DrawVertexArray( m_waterTiles );
	g_theRenderer->BindTexture( m_waterBotTex );
	g_theRenderer->DrawVertexArray( m_waterBotTiles );
	g_theRenderer->BindTexture( m_waterUpTex );
	g_theRenderer->DrawVertexArray( m_waterTopTiles );
	g_theRenderer->BindTexture( m_waterLeftTex );
	g_theRenderer->DrawVertexArray( m_waterLeftTiles );
	g_theRenderer->BindTexture( m_waterRightTex );
	g_theRenderer->DrawVertexArray( m_waterRightTiles );
	g_theRenderer->BindTexture( m_waterTopLeftTex );
	g_theRenderer->DrawVertexArray( m_waterTopLeftTiles );
	g_theRenderer->BindTexture( m_waterTopRightTex );
	g_theRenderer->DrawVertexArray( m_waterTopRightTiles );
	g_theRenderer->BindTexture( m_waterBottomRightTex );
	g_theRenderer->DrawVertexArray( m_waterBottomRightTiles );
	g_theRenderer->BindTexture( m_waterBottomLeftTex );
	g_theRenderer->DrawVertexArray( m_waterBottomLeftTiles );

	
	g_theRenderer->BindTexture( m_lavaTileTypeMain );
	g_theRenderer->DrawVertexArray( m_lavaTiles );
	g_theRenderer->BindTexture( m_lavaBotTex );
	g_theRenderer->DrawVertexArray( m_lavaBotTiles );
	g_theRenderer->BindTexture( m_lavaUpTex );
	g_theRenderer->DrawVertexArray( m_lavaTopTiles );
	g_theRenderer->BindTexture( m_lavaLeftTex );
	g_theRenderer->DrawVertexArray( m_lavaLeftTiles );
	g_theRenderer->BindTexture( m_lavaRightTex );
	g_theRenderer->DrawVertexArray( m_lavaRightTiles );
	g_theRenderer->BindTexture( m_lavaTopLeftTex );
	g_theRenderer->DrawVertexArray( m_lavaTopLeftTiles );
	g_theRenderer->BindTexture( m_lavaTopRightTex );
	g_theRenderer->DrawVertexArray( m_lavaTopRightTiles );
	g_theRenderer->BindTexture( m_lavaBottomRightTex );
	g_theRenderer->DrawVertexArray( m_lavaBottomRightTiles );
	g_theRenderer->BindTexture( m_lavaBottomLeftTex );
	g_theRenderer->DrawVertexArray( m_lavaBottomLeftTiles );


	if ( m_currentSelection == DEBUG_POTENTIAL_FINDER_COST )
	{
		if ( !m_tiles[ m_currentFinderTileIndex ].m_isSolid )
		{
			m_finder->DebugRenderCostFields( m_currentFinderTileIndex );
		}

		g_theGUI->Render();
		return;
	}

	if ( m_currentSelection == DEBUG_POTENTIAL_FNDER_INTEGRATION_COST )
	{
		if ( !m_tiles[ m_currentFinderTileIndex ].m_isSolid )
		{
			m_finder->DebugRenderIntegrationFields( m_currentFinderTileIndex );
		}

		g_theGUI->Render();
		return;
	}

	if ( m_currentSelection == DEBUG_FLOW_FIELDS )
	{
		if ( !m_tiles[ m_currentFinderTileIndex ].m_isSolid )
		{
			//m_camera->SetOrthoView( Vec2() , Vec2( 80.f * 0.1f , 45.f * 0.1f ) );
			m_finder->DebugRenderFlowFields( m_currentFinderTileIndex );
		}

		g_theGUI->Render();
		return;

	}

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawVertexArray( m_infMapTiles );

	g_theRenderer->BindTexture( m_monsterBaseTex );
	for ( int i = 0; i < m_monsterBases.size(); i++ )
	{
		AABB2 aabb = *m_monsterBases[ i ];
		AppendAABB2(monsterBases,aabb,Rgba8(100,100,100,255));

		//g_theRenderer->DrawAABB2D( aabb , Rgba8(100,100,100,255) );
	}

	g_theRenderer->DrawVertexArray( monsterBases );

	for ( int i = 0; i < m_bombs.size(); i++ )
	{
		if ( m_bombs[ i ] == nullptr )
		{
			continue;
		}

		m_bombs[ i ]->Render();
	}

	m_player->Render();

	for ( int i = 0; i < m_ais.size(); i++ )
	{
		if ( m_ais[ i ] == nullptr )
		{
			continue;
		}
		m_ais[ i ]->Render();
	}

	for ( int i = 0; i < m_occAis.size(); i++ )
	{
		if ( m_occAis[ i ] == nullptr )
		{
			continue;
		}
		m_occAis[i]->Render();
	}

	if ( debug_Map_InfluenceMaps )
	{
		g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( m_infMapValues );
	}

	if ( debug_AI_InfluenceMaps )
	{
		g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( m_combinedMapValues );
	}

	if ( m_gamePaused )
	{
		g_theGUI->Render();
	}
	g_theRenderer->EndCamera( *m_camera );

	g_theRenderer->BeginCamera( *m_debugCamera );

	std::vector<Vertex_PCU> fpsVerts;
	g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
	m_game->m_font->AddVertsForTextInBox2D( fpsVerts , m_fps_box , 1.f , std::to_string( g_theapp->GetFPS() ) , Rgba8() , 1.f , Vec2( 0.1f , 0.1f ) );
	g_theRenderer->DrawVertexArray( fpsVerts );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->EndCamera( *m_debugCamera );
}

void MainGameMapCreator::LoadData()
{
	tinyxml2::XMLDocument gameDoc;
	gameDoc.LoadFile( "Data/GameConfig/InfMapGameData.xml" );

	tinyxml2::XMLElement* tileRoot = gameDoc.FirstChildElement("Tiles");

	tinyxml2::XMLElement* infRoot = gameDoc.FirstChildElement( "InfluenceMaps" );

	tinyxml2::XMLElement* monsterRoot = gameDoc.FirstChildElement( "MonsterBases" );

	tinyxml2::XMLElement* playerRoot = gameDoc.FirstChildElement( "PlayerInfo" );

	tinyxml2::XMLElement* occAIRoot = gameDoc.FirstChildElement( "OccupancyAI" );

	if ( occAIRoot != nullptr )
	{
		for ( tinyxml2::XMLElement* ele = occAIRoot->FirstChildElement( "AI" ); ele != nullptr; ele = ele->NextSiblingElement( "AI" ) )
		{
			float posX = ParseXmlAttribute( *ele , "PositionX" , -1.f );
			float posY = ParseXmlAttribute( *ele , "PositionY" , -1.f );

			std::string type = ParseXmlAttribute( *ele , "Type" , "" );
			OCCAI_Type aiType = OCC_AI_SINGLE_MAP;
			if ( type == "Shared" )
			{
				aiType = OCC_AI_SHARED_MAP;
			}

			OccAI* ai = new OccAI();
			ai->m_game = m_game;
			ai->m_position = Vec2( posX , posY );
			ai->m_type = aiType;

			m_occAis.push_back( ai );
			
		}
	}

	if ( playerRoot != nullptr )
	{
		tinyxml2::XMLElement* ele = playerRoot->FirstChildElement( "Player" );
		float posX = ParseXmlAttribute( *ele , "positionX" , -1.f );
		float posY = ParseXmlAttribute( *ele , "positionY" , -1.f );

		m_player->m_position = Vec2( posX , posY );
	}


	if ( monsterRoot != nullptr )
	{
		for ( tinyxml2::XMLElement* ele = monsterRoot->FirstChildElement( "Base" ); ele != nullptr; ele = ele->NextSiblingElement( "Base" ) )
		{
			float minX = ParseXmlAttribute( *ele , "minX" , -1.f );
			float minY = ParseXmlAttribute( *ele , "minY" , -1.f );
			float maxX = ParseXmlAttribute( *ele , "maxX" , -1.f );
			float maxY = ParseXmlAttribute( *ele , "maxY" , -1.f );

			AABB2* aabb = new AABB2( minX , minY , maxX , maxY );
			m_monsterBases.push_back( aabb );
		}
	}

	if ( infRoot != nullptr )
	{
		for ( tinyxml2::XMLElement* ele = infRoot->FirstChildElement( "Maps" ); ele != nullptr; ele = ele->NextSiblingElement( "Maps" ) )
		{
			int dimX = ParseXmlAttribute( *ele , "DimensionX" , -1 );
			int dimY = ParseXmlAttribute( *ele , "DimensionY" , -1 );
			int strtX = ParseXmlAttribute( *ele , "StartPointX" , -1 );
			int strtY = ParseXmlAttribute( *ele , "StartPointY" , -1 );
			float startValue = ParseXmlAttribute( *ele , "StartValue" , -1.f );
			float fallOff = ParseXmlAttribute( *ele , "FallOff" , -1.f );

			InfluenceMap* infMap = new InfluenceMap( m_game , IntVec2( strtX , strtY ) , IntVec2( dimX , dimY ) , startValue , fallOff );
			infMap->Create();
			m_infMaps.push_back( infMap );
		}
	}

	if ( tileRoot != nullptr )
	{
		for ( tinyxml2::XMLElement* ele = tileRoot->FirstChildElement( "Tile" ); ele != nullptr; ele = ele->NextSiblingElement( "Tile" ) )
		{
			int xCoord = ParseXmlAttribute( *ele , "XCoord" , -1 );
			int yCoord = ParseXmlAttribute( *ele , "YCoord" , -1 );

			if ( xCoord >= 0 && yCoord >= 0 )
			{
				int index = GetTileIndexForCoords( IntVec2( xCoord , yCoord ) );

				bool isSolid = ParseXmlAttribute( *ele , "IsSolid" , false );
				m_tiles[ index ].m_isSolid = isSolid;

				std::string textureType = ParseXmlAttribute( *ele , "textureType" , "" );
				TileType currentType = TILE_TYPE_SAND;

				if ( textureType == "water" )
				{
					currentType = TILE_TYPE_WATER;
				}

				if ( textureType == "lava" )
				{
					currentType = TILE_TYPE_LAVA;
				}

				if ( textureType == "stone" )
				{
					currentType = TILE_TYPE_STONE;
				}

				m_tiles[ index ].m_type = currentType;

				std::string textureSubType = ParseXmlAttribute( *ele , "textureSubType" , "" );

				if ( textureSubType == "up" )
				{
					m_tiles[ index ].m_subType = TILE_TYPE_UP;
				}
				if ( textureSubType == "down" )
				{
					m_tiles[ index ].m_subType = TILE_TYPE_DOWN;
				}
				if ( textureSubType == "left" )
				{
					m_tiles[ index ].m_subType = TILE_TYPE_LEFT;
				}
				if ( textureSubType == "right" )
				{
					m_tiles[ index ].m_subType = TILE_TYPE_RIGHT;
				}
				if ( textureSubType == "topLeft" )
				{
					m_tiles[ index ].m_subType = TILE_TYPE_TOP_LEFT;
				}
				if ( textureSubType == "botLeft" )
				{
					m_tiles[ index ].m_subType = TILE_TYPE_BOT_LEFT;
				}
				if ( textureSubType == "botRight" )
				{
					m_tiles[ index ].m_subType = TILE_TYPE_BOT_RIGHT;
				}
				if ( textureSubType == "topRight" )
				{
					m_tiles[ index ].m_subType = TILE_TYPE_TOP_RIGHT;
				}

			}
		}
	}
	

}

void MainGameMapCreator::SaveData()
{
	tinyxml2::XMLDocument gameDoc;
	gameDoc.LoadFile( "Data/GameConfig/InfMapGameData.xml" );

	gameDoc.Clear();

	tinyxml2::XMLNode* tiles = gameDoc.NewElement( "Tiles" );

	tinyxml2::XMLNode* infmaps = gameDoc.NewElement( "InfluenceMaps" );

	tinyxml2::XMLNode* monsterBases = gameDoc.NewElement( "MonsterBases" );

	tinyxml2::XMLNode* playerInfo = gameDoc.NewElement( "PlayerInfo" );

	tinyxml2::XMLNode* occAIInfo = gameDoc.NewElement( "OccupancyAI" );

	for ( int i = 0; i < m_occAis.size(); i++ )
	{
		tinyxml2::XMLElement* ele = gameDoc.NewElement( "AI" );
		ele->SetAttribute( "PositionX" , m_occAis[ i ]->m_position.x );
		ele->SetAttribute( "PositionY" , m_occAis[ i ]->m_position.y );
		if ( m_occAis[ i ]->m_type == OCC_AI_SINGLE_MAP )
		{
			ele->SetAttribute( "Type" , "Single" );
		}
		else
		{
			ele->SetAttribute( "Type" , "Shared" );
		}
		occAIInfo->InsertEndChild( ele );
	}

	gameDoc.InsertEndChild( occAIInfo );
	

	tinyxml2::XMLElement* playerEle = gameDoc.NewElement( "Player" );
	playerEle->SetAttribute( "positionX" , m_player->m_position.x );
	playerEle->SetAttribute( "positionY" , m_player->m_position.y );
	playerInfo->InsertEndChild( playerEle );

	gameDoc.InsertEndChild( playerInfo );

	for ( int i = 0; i < m_monsterBases.size(); i++ )
	{
		tinyxml2::XMLElement* ele = gameDoc.NewElement( "Base" );
		ele->SetAttribute( "minX" , m_monsterBases[ i ]->mins.x );
		ele->SetAttribute( "maxX" , m_monsterBases[ i ]->maxs.x );
		ele->SetAttribute( "minY" , m_monsterBases[ i ]->mins.y );
		ele->SetAttribute( "maxY" , m_monsterBases[ i ]->maxs.y );

		monsterBases->InsertEndChild( ele );
	}
	
	gameDoc.InsertEndChild( monsterBases );


	for ( int i = 0; i < m_infMaps.size(); i++ )
	{
		tinyxml2::XMLElement* ele = gameDoc.NewElement( "Maps" );
		ele->SetAttribute( "DimensionX" , m_infMaps[ i ]->m_dimesions.x );
		ele->SetAttribute( "DimensionY" , m_infMaps[ i ]->m_dimesions.y );
		ele->SetAttribute( "StartPointX" , m_infMaps[ i ]->m_startPosition.x );
		ele->SetAttribute( "StartPointY" , m_infMaps[ i ]->m_startPosition.y );
		ele->SetAttribute( "StartValue" , m_infMaps[ i ]->m_startValue );
		ele->SetAttribute( "FallOff" , m_infMaps[ i ]->m_fallOffPerTile );

		infmaps->InsertEndChild( ele );
		
	}

	gameDoc.InsertFirstChild( infmaps );

	gameDoc.InsertFirstChild( tiles );

	for ( int i = 0; i < m_tiles.size(); i++ )
	{
		tinyxml2::XMLElement* ele = gameDoc.NewElement( "Tile" );
		ele->SetAttribute( "XCoord" , m_tiles[ i ].m_tileCoords.x );
		ele->SetAttribute( "YCoord" , m_tiles[ i ].m_tileCoords.y );
		ele->SetAttribute( "IsSolid" , m_tiles[ i ].m_isSolid );

		std::string textureType = "";
		if ( m_tiles[ i ].m_type == TILE_TYPE_SAND )
		{
			textureType = "sand";
		}
		if ( m_tiles[ i ].m_type == TILE_TYPE_STONE )
		{
			textureType = "stone";
		}
		if ( m_tiles[ i ].m_type == TILE_TYPE_WATER )
		{
			textureType = "water";
		}
		if ( m_tiles[ i ].m_type == TILE_TYPE_LAVA )
		{
			textureType = "lava";
		}

		std::string textureSubType = "default";
		if ( m_tiles[ i ].m_subType == TILE_TYPE_UP )
		{
			textureSubType = "up";
		}
		if ( m_tiles[ i ].m_subType == TILE_TYPE_DOWN )
		{
			textureSubType = "down";
		}
		if ( m_tiles[ i ].m_subType == TILE_TYPE_LEFT )
		{
			textureSubType = "left";
		}
		if ( m_tiles[ i ].m_subType == TILE_TYPE_RIGHT )
		{
			textureSubType = "right";
		}
		if ( m_tiles[ i ].m_subType == TILE_TYPE_TOP_LEFT )
		{
			textureSubType = "topLeft";
		}
		if ( m_tiles[ i ].m_subType == TILE_TYPE_BOT_LEFT )
		{
			textureSubType = "botLeft";
		}
		if ( m_tiles[ i ].m_subType == TILE_TYPE_BOT_RIGHT )
		{
			textureSubType = "botRight";
		}
		if ( m_tiles[ i ].m_subType == TILE_TYPE_TOP_RIGHT )
		{
			textureSubType = "topRight";
		}



		ele->SetAttribute( "textureType" , textureType.c_str() );
		ele->SetAttribute( "textureSubType" , textureSubType.c_str() );

		tiles->InsertEndChild( ele );

	}

	gameDoc.SaveFile( "Data/GameConfig/InfMapGameData.xml" );
}

void MainGameMapCreator::SetTileType( TileSelections selection , IntVec2 coords )
{
	int index = GetTileIndexForCoords( coords );

	switch ( selection )
	{

	case PAINT_SAND_TILE:
	{
		m_tiles[ index ].m_type = TILE_TYPE_SAND;
		m_tiles[ index ].m_isSolid = false;
		break;
	}
	
	case PAINT_LAVA_TILE:
	{
		m_tiles[ index ].m_type = TILE_TYPE_LAVA;
		m_tiles[ index ].m_isSolid = false;

		if ( m_currentSubSelection == PAINT_DEFAULT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_NONE;
		}

		if ( m_currentSubSelection == PAINT_DOWN )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_DOWN;
		}

		if ( m_currentSubSelection == PAINT_UP )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_UP;
		}

		if ( m_currentSubSelection == PAINT_LEFT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_LEFT;
		}

		if ( m_currentSubSelection == PAINT_RIGHT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_RIGHT;
		}

		if ( m_currentSubSelection == PAINT_TOPRIGHT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_TOP_RIGHT;
		}

		if ( m_currentSubSelection == PAINT_TOPLEFT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_TOP_LEFT;
		}

		if ( m_currentSubSelection == PAINT_BOTRIGHT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_BOT_RIGHT;
		}

		if ( m_currentSubSelection == PAINT_BOTLEFT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_BOT_LEFT;
		}

		break;
	}
	case PAINT_SOLID_TILE:
	{
		m_tiles[ index ].m_type = TILE_TYPE_STONE;
		m_tiles[ index ].m_isSolid = true;
		break;
	}

	case PAINT_WATER_TILE:
	{
		m_tiles[ index ].m_type = TILE_TYPE_WATER;
		m_tiles[ index ].m_isSolid = false;


		if ( m_currentSubSelection == PAINT_DEFAULT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_NONE;
		}

		if ( m_currentSubSelection == PAINT_DOWN )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_DOWN;
		}

		if ( m_currentSubSelection == PAINT_UP )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_UP;
		}

		if ( m_currentSubSelection == PAINT_LEFT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_LEFT;
		}

		if ( m_currentSubSelection == PAINT_RIGHT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_RIGHT;
		}

		if ( m_currentSubSelection == PAINT_TOPRIGHT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_TOP_RIGHT;
		}

		if ( m_currentSubSelection == PAINT_TOPLEFT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_TOP_LEFT;
		}

		if ( m_currentSubSelection == PAINT_BOTRIGHT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_BOT_RIGHT;
		}

		if ( m_currentSubSelection == PAINT_BOTLEFT )
		{
			m_tiles[ index ].m_subType = TILE_TYPE_BOT_LEFT;
		}

		break;
	}

	default:break;


	}
}
