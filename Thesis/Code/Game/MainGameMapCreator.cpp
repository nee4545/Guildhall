#include "Game/MainGameMapCreator.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ThirdParty/ImGuiSystem.hpp"
#include "Game/Game.hpp"
extern ImGuiSystem* g_theGUI;

MainGameMapCreator::MainGameMapCreator()
{
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 80.f, 45.f) );
	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );
	PopulateTiles();
	
	m_groundTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/stones/stones_0008_tile.png" );
	m_waterTileType1Main = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/Water_tiles/water_tiles_0005_Layer-24.png" );
	m_lavaTileTypeMain = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Tiles/sand/sand_0012_tile.png" );
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

	if ( g_theInput->IsRightMouseButtonPressed() )
	{
		SetTileType( m_currentSelection , IntVec2( m_mousePosition.x , m_mousePosition.y ) );
	}

	ImGui::NewFrame();
	ImGui::Begin( "MAP PAINTER" );
	ImGui::Text( "Current Selection" );
	ImGui::Text( m_currentSelectedText.c_str() );
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

	ImGui::End();
}

void MainGameMapCreator::Render()
{
	std::vector<Vertex_PCU> m_mainTiles;
	std::vector<Vertex_PCU> m_solidTiles;
	std::vector<Vertex_PCU> m_waterTiles;
	std::vector<Vertex_PCU> m_lavaTiles;

	g_theRenderer->BeginCamera( *m_camera );

	for ( int i = 0; i < m_tiles.size(); i++ )
	{
		if ( m_tiles[ i ].m_type == TILE_TYPE_SAND )
		{
			AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
			AppendAABB2( m_mainTiles , aabb , Rgba8() );
		}

		if ( m_tiles[ i ].m_type == TILE_TYPE_STONE )
		{
			AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
			AppendAABB2( m_solidTiles , aabb , Rgba8() );
		}

		if ( m_tiles[ i ].m_type == TILE_TYPE_LAVA )
		{
			AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
			AppendAABB2( m_lavaTiles , aabb , Rgba8() );
		}

		if ( m_tiles[ i ].m_type == TILE_TYPE_WATER )
		{
			AABB2 aabb = AABB2( m_tiles[ i ].m_tileCoords.x , m_tiles[ i ].m_tileCoords.y , m_tiles[ i ].m_tileCoords.x + 1 , m_tiles[ i ].m_tileCoords.y + 1 );
			AppendAABB2( m_waterTiles , aabb , Rgba8() );
		}

	}

	g_theRenderer->BindTexture( m_game->m_sandMainTex );
	g_theRenderer->DrawVertexArray( m_mainTiles );
	g_theRenderer->BindTexture( m_groundTex );
	g_theRenderer->DrawVertexArray( m_solidTiles );
	g_theRenderer->BindTexture( m_waterTileType1Main );
	g_theRenderer->DrawVertexArray( m_waterTiles );
	g_theRenderer->BindTexture( m_lavaTileTypeMain );
	g_theRenderer->DrawVertexArray( m_lavaTiles );

	g_theGUI->Render();
	g_theRenderer->EndCamera( *m_camera );
}

void MainGameMapCreator::LoadData()
{

}

void MainGameMapCreator::SaveData()
{

}

void MainGameMapCreator::SetTileType( Selections selection , IntVec2 coords )
{
	int index = GetTileIndexForCoords( coords );

	switch ( selection )
	{
	case PAINT_LAVA_TILE:
	{
		m_tiles[ index ].m_type = TILE_TYPE_LAVA;
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
		break;
	}

	default:break;


	}
}
