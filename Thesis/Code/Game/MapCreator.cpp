#include "Game/MapCreator.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/ThirdParty/ImGuiSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"

extern ImGuiSystem* g_theGUI;

MapCreator::MapCreator( Game* game )
{
	m_game = game;

	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2(0.f,0.f) , Vec2(40.f,22.5f) );
	//m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );


	for ( int i = 0; i < m_game->m_mainMapTiles.size(); i++ )
	{
		AABB2 aabb = AABB2( m_game->m_mainMapTiles[ i ].m_tileCoords.x , m_game->m_mainMapTiles[ i ].m_tileCoords.y , m_game->m_mainMapTiles[ i ].m_tileCoords.x + 1 , m_game->m_mainMapTiles[ i ].m_tileCoords.y + 1);
		AppendAABB2( tileVerts , aabb , Rgba8( 100 , 100 , 100 , 100 ) );
	}

	for ( int i = 0; i < m_game->m_mainMapTiles.size(); i++ )
	{
		if ( m_game->m_mainMapTiles[ i ].m_isSolid )
		{
			m_solidTileIndices.push_back( i );
		}
	}

}

void MapCreator::Update( float deltaSeconds )
{
	ImGui::NewFrame();
	ImGui::Begin( "MAP EDITOR" );
	ImGui::SliderInt( "Quadrant" , &currentQuadrant , 1 , 16 );
	ImGui::SliderInt( "Sub-Quadrant" , &currentSubQuadrant , 1 , 4 );
	ImGui::Checkbox( "Current Tile Solid" , &currentTileSolid );
	ImGui::Text( "Press Space to save current Data" );

	if ( ImGui::CollapsingHeader( "Map Parts" ) )
	{
		ImGui::Checkbox( "Show map in parts" , &ShowMapInParts );
		ImGui::SliderInt( "Map Part" , &mapPart , 1 , 4 );
		//ImGui::End();
	}

	ImGui::End();

	ImGui::EndFrame();

	UpdateCamera();
	UpdateCameraForParts();

	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	m_mousePos.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	m_mousePos.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );


	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		IntVec2 pos = IntVec2( RoundDownToInt( m_mousePos.x ) , RoundDownToInt( m_mousePos.y ) );
		int index = m_game->GetTileIndexForTileCoordsForMainMap( pos );
		m_game->m_mainMapTiles[ index ].m_isSolid = currentTileSolid;

		if ( currentTileSolid )
		{
			bool found = false;
			for ( int i = 0; i < m_solidTileIndices.size(); i++ )
			{
				if ( index == m_solidTileIndices[ i ] )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				m_solidTileIndices.push_back( index );
			}
		}
		else
		{
			for ( auto it = m_solidTileIndices.begin(); it != m_solidTileIndices.end(); it++ )
			{
				if ( *it == index )
				{
					m_solidTileIndices.erase( it );
					m_solidTileIndices.shrink_to_fit();
					break;
				}
			}

		}
	}

	if ( g_theInput->WasKeyJustPressed( SPACE ) )
	{
		Save();
	}
}



void MapCreator::Render()
{

	g_theRenderer->BeginCamera( *m_camera );
	g_theRenderer->BindTexture( m_game->m_mapTex1 );
	g_theRenderer->DrawAABB2D( m_game->m_mapPart1, Rgba8() );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawVertexArray( tileVerts );

	std::vector<Vertex_PCU> solidVerts;
	for ( int i = 0; i < m_solidTileIndices.size(); i++ )
	{
		AABB2 aabb = AABB2( m_game->m_mainMapTiles[ m_solidTileIndices[ i ] ].m_tileCoords.x , m_game->m_mainMapTiles[ m_solidTileIndices[ i ] ].m_tileCoords.y , m_game->m_mainMapTiles[ m_solidTileIndices[ i ] ].m_tileCoords.x + 1 , m_game->m_mainMapTiles[ m_solidTileIndices[ i ] ].m_tileCoords.y + 1 );
		AppendAABB2( solidVerts , aabb , Rgba8( 100 , 0 , 0 , 100 ) );
	}

	g_theRenderer->DrawVertexArray( solidVerts );

	g_theGUI->Render();
	g_theRenderer->EndCamera( *m_camera );
}

void MapCreator::Save()
{
	tinyxml2::XMLDocument gameDoc;
	gameDoc.LoadFile( "Data/GameConfig/GameData.xml" );

	gameDoc.Clear();
	
	tinyxml2::XMLNode* tiles = gameDoc.NewElement( "SolidTiles" );

	gameDoc.InsertFirstChild(tiles);


	for ( int i = 0; i < m_solidTileIndices.size(); i++ )
	{
		tinyxml2::XMLElement* ele = gameDoc.NewElement( "Tile" );
		ele->SetAttribute( "index" , m_solidTileIndices[i] );
		tiles->InsertEndChild( ele );
	}

	for ( int i = 0; i < m_game->m_mainMapTiles.size(); i++ )
	{
		if ( m_game->m_mainMapTiles[ i ].m_isSolid )
		{
			m_solidTileIndices.push_back( i );
		}
	}

	gameDoc.SaveFile( "Data/GameConfig/GameData.xml" );


	
}

void MapCreator::UpdateCamera()
{

	if ( ShowMapInParts )
	{
		return;
	}

	switch ( currentQuadrant )
	{
	case 1:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2(0.f,0.f),Vec2(80.f,45.f) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 80.f , 0.f ) , Vec2( 160.f , 45.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 45.f ) , Vec2( 80.f , 90.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2(80.f,45.f) , Vec2(160.f,90.f) );
		}
		
		break;
	}

	case 2:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 0.f ) , Vec2( 240.f , 45.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 240.f , 0.f ) , Vec2( 320.f , 45.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 45.f ) , Vec2( 240.f , 90.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 240.f , 45.f ) , Vec2( 320.f , 90.f ) );
		}

		break;
	}

	case 3:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 90.f ) , Vec2( 80.f , 135.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 80.f , 90.f ) , Vec2( 160.f , 135.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 135.f ) , Vec2( 80.f , 180.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 80.f , 135.f ) , Vec2( 160.f , 180.f ) );
		}

		break;
	}

	case 4:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 90.f ) , Vec2( 240.f , 135.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 240.f , 90.f ) , Vec2( 320.f , 135.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 135.f ) , Vec2( 240.f , 180.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 240.f , 135.f ) , Vec2( 320.f , 180.f ) );
		}
		break;
	}

	case 5:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 180.f ) , Vec2( 80.f , 225.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 80.f , 180.f ) , Vec2( 160.f , 225.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 225.f ) , Vec2( 80.f , 270.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 80.f , 225.f ) , Vec2( 160.f , 270.f ) );
		}
		break;
	}

	case 6:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 180.f ) , Vec2( 240.f , 225.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 240.f , 180.f ) , Vec2( 320.f , 225.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 225.f ) , Vec2( 240.f , 270.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 240.f , 225.f ) , Vec2( 320.f , 270.f ) );
		}
		break;
	}

	case 7:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 270.f ) , Vec2( 80.f , 315.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 80.f , 270.f ) , Vec2( 160.f , 315.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 315.f ) , Vec2( 80.f , 360.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 80.f , 315.f ) , Vec2( 160.f , 360.f ) );
		}
		break;
	}

	case 8:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 270.f ) , Vec2( 240.f , 315.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 240.f , 270.f ) , Vec2( 320.f , 315.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 315.f ) , Vec2( 240.f , 360.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 240.f , 315.f ) , Vec2( 320.f , 360.f ) );
		}
		break;
	}

	case 9:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 360.f ) , Vec2( 80.f , 405.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 80.f , 360.f ) , Vec2( 160.f , 405.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 405.f ) , Vec2( 80.f , 450.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 80.f , 405.f ) , Vec2( 160.f , 450.f ) );
		}
		break;
	}

	case 10:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 360.f ) , Vec2( 240.f , 405.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 240.f , 360.f ) , Vec2( 320.f , 405.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 405.f ) , Vec2( 240.f , 450.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 240.f , 405.f ) , Vec2( 320.f , 450.f ) );
		}
		break;
	}

	case 11:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 450.f ) , Vec2( 80.f , 495.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 80.f , 450.f ) , Vec2( 160.f , 495.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 495.f ) , Vec2( 80.f , 540.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 80.f , 495.f ) , Vec2( 160.f , 540.f ) );
		}
		break;
	}

	case 12:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 450.f ) , Vec2( 240.f , 495.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 240.f , 450.f ) , Vec2( 320.f , 495.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 495.f ) , Vec2( 240.f , 540.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 240.f , 495.f ) , Vec2( 320.f , 540.f ) );
		}
		break;
	}

	case 13:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 540.f ) , Vec2( 80.f , 585.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 80.f , 540.f ) , Vec2( 160.f , 585.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 585.f ) , Vec2( 80.f , 630.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 80.f , 585.f ) , Vec2( 160.f , 630.f ) );
		}
		break;
	}

	case 14:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 540.f ) , Vec2( 240.f , 585.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 240.f , 540.f ) , Vec2( 320.f , 585.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 585.f ) , Vec2( 240.f , 630.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 240.f , 585.f ) , Vec2( 320.f , 630.f ) );
		}
		break;
	}

	case 15:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 630.f ) , Vec2( 80.f , 675.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 80.f , 630.f ) , Vec2( 160.f , 675.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 0.f , 675.f ) , Vec2( 80.f , 720.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 80.f , 675.f ) , Vec2( 160.f , 720.f ) );
		}
		break;
	}

	case 16:
	{
		if ( currentSubQuadrant == 1 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 630.f ) , Vec2( 240.f , 675.f ) );
		}

		else if ( currentSubQuadrant == 2 )
		{
			m_camera->SetOrthoView( Vec2( 240.f , 630.f ) , Vec2( 320.f , 675.f ) );
		}

		else if ( currentSubQuadrant == 3 )
		{
			m_camera->SetOrthoView( Vec2( 160.f , 675.f ) , Vec2( 240.f , 720.f ) );
		}

		else
		{
			m_camera->SetOrthoView( Vec2( 240.f , 675.f ) , Vec2( 320.f , 720.f ) );
		}
		break;
	}

	default:
		break;
	}
}

void MapCreator::UpdateCameraForParts()
{
	if ( !ShowMapInParts )
	{
		return;
	}

	if ( mapPart == 1 )
	{
		m_camera->SetOrthoView( Vec2(0.f,0.f) , Vec2(320.f,180.f) );
	}

	if ( mapPart == 2 )
	{
		m_camera->SetOrthoView( Vec2(0.f,180.f) , Vec2(320.f,360.f) );
	}

	if ( mapPart == 3 )
	{
		m_camera->SetOrthoView( Vec2(0.f,360.f) , Vec2(320.f,540.f) );
	}

	if ( mapPart == 4 )
	{
		m_camera->SetOrthoView( Vec2(0.f,540.f) , Vec2(320.f,720.f) );
	}
}
