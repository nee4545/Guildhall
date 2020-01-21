#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/HUD.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Hero.hpp"
#include "Game/Wall.hpp"
#include "Game/Spike.hpp"
#include "Game/Dino.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"

Map::Map( Game* game, const IntVec2& tile_dimensions ):m_game(game),tileDimensions(tile_dimensions)
{
	m_HUD = new HUD();
	PopulateTiles();
	
}

void Map::Update( float deltaSeconds )
{
	UpdatedSelectedUnits();
	SpawnSelectedUnit();
}



void Map::Render()
{
	Texture* mapTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->BindTexture( mapTexture );
	g_theRenderer->DrawVertexArray( m_tileVertices );
	g_theRenderer->BindTexture( nullptr );
	m_HUD->Render();

	RenderBackRounds();

	m_HUD->RenderSelectedUnit( m_selcetedUnit );

	DisplayLanes();

	for ( int index = 0; index < m_heros.size(); index++ )
	{
		m_heros[ index ]->Render();
	}

	for ( int index = 0; index < m_walls.size(); index++ )
	{
		m_walls[ index ]->Render();
	}

	for ( int index = 0; index < m_spikes.size(); index++ )
	{
		m_spikes[ index ]->Render();
	}

	for ( int index = 0; index < m_dinos.size(); index++ )
	{
		m_dinos[ index ]->Render();
	}
}

void Map::PopulateTiles()
{

	int numTiles=tileDimensions.x*tileDimensions.y;
	for( int index=0; index<numTiles; index++ )
	{
		int tileX=index % tileDimensions.x;
		int tileY=index / tileDimensions.x;
		
		m_tiles.push_back(Tile(tileX,tileY));
	}

	for ( int verticalIndex = 0; verticalIndex < tileDimensions.y; verticalIndex++ )
	{
		for ( int horizontalIndex = 0; horizontalIndex < tileDimensions.x; horizontalIndex++ )
		{
			int index = verticalIndex * tileDimensions.x + horizontalIndex;
			AABB2 aabb = AABB2( ( float ) horizontalIndex , ( float ) verticalIndex , ( float ) horizontalIndex + 1 , ( float ) verticalIndex + 1 );
			Vec2 minUVs;
			Vec2 maxUvs;

			TileType temp = m_tiles[ index ].m_type;
			TileDefinition::s_definitions[ temp ].m_sprite.GetUVs( minUVs , maxUvs );

			AppendAABB2( m_tileVertices , aabb , Rgba8( 255 , 255 , 255 , 255 ) , minUVs , maxUvs );
		}
	}

}

void Map::RenderBackRounds()
{
	AABB2 d1 = AABB2( 8.f , 18.f , 9.2f , 19.f );
	AABB2 d2 = AABB2( 20.f , 10.f , 21.2f , 11.f );
	AABB2 d3 = AABB2( 30.f , 21.f , 31.2f , 22.f );

	AABB2 t1 = AABB2( 2.f , 1.f , 3.5f , 4.5f );
	AABB2 t2 = AABB2( 2.f , 2.5f , 3.5f , 6.5f );
	AABB2 t3 = AABB2( 2.f , 4.f , 3.5f , 8.f );
	AABB2 t4 = AABB2( 2.f , 5.5f , 3.5f , 9.5f );
	AABB2 t5 = AABB2( 2.f , 7.f , 3.5f , 11.f );
	AABB2 t6 = AABB2( 2.f , 8.5f , 3.5f , 12.5f );
	AABB2 t7 = AABB2( 2.f , 10.f , 3.5f , 14.f );
	AABB2 t8 = AABB2( 2.f , 11.5f , 3.5f , 15.5f );
	AABB2 t9 = AABB2( 2.f , 13.f , 3.5f , 17.f );
	AABB2 t10 = AABB2( 2.f , 14.5f , 3.5f , 18.5f );
	AABB2 t11 = AABB2( 2.f , 16.f , 3.5f , 20.f );
	AABB2 t12 = AABB2( 2.f , 17.5f , 3.5f , 21.5f );
	AABB2 t13 = AABB2( 2.f , 19.f , 3.5f , 23.f );
	AABB2 t14 = AABB2( 2.f , 20.5f , 3.5f , 24.5f );

	AABB2 e1 = AABB2( 50.5f, 1.f , 52.f , 4.5f );
	AABB2 e2 = AABB2( 50.5f, 2.5f , 52.f , 6.5f );
	AABB2 e3 = AABB2( 50.5f, 4.f , 52.f , 8.f );
	AABB2 e4 = AABB2( 50.5f, 5.5f , 52.f , 9.5f );
	AABB2 e5 = AABB2( 50.5f, 7.f , 52.f , 11.f );
	AABB2 e6 = AABB2( 50.5f, 8.5f , 52.f , 12.5f );
	AABB2 e7 = AABB2( 50.5f, 10.f , 52.f , 14.f );
	AABB2 e8 = AABB2( 50.5f, 11.5f , 52.f , 15.5f );
	AABB2 e9 = AABB2( 50.5f, 13.f , 52.f , 17.f );
	AABB2 e10 = AABB2(50.5f , 14.5f , 52.f , 18.5f );
	AABB2 e11 = AABB2(50.5f , 16.f , 52.f , 20.f );
	AABB2 e12 = AABB2(50.5f , 17.5f , 52.f , 21.5f );
	AABB2 e13 = AABB2(50.5f , 19.f , 52.f , 23.f );
	AABB2 e14 = AABB2(50.5f , 20.5f , 52.f , 24.5f );



	Texture* d1Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Backrounds/d1.png" );
	g_theRenderer->BindTexture( d1Tex );
	g_theRenderer->DrawAABB2D( d1 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( d2 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( d3 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* t1Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/b1.png" );
	g_theRenderer->BindTexture( t1Tex );
	g_theRenderer->DrawAABB2D( t14 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t13 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t12 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t11 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t10 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t9 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t8 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t7 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t6 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t5 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t4 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t3 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t2 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t1 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* t2Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/b1.png" );
	g_theRenderer->BindTexture( t2Tex );
	g_theRenderer->DrawXFlippedAABB2D( e14 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e13 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e12 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e11 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e10 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e9 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e8 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e7 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e6 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e5 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e4 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e3 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e2 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e1 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->BindTexture( nullptr );

	
}



void Map::CreatePlayer()
{

}

void Map::ToggleNoClip()
{

}

void Map::SetTileType( int tileX, int tileY, TileType type )
{
	int tileIndex=GetTileIndexForTileCoords(IntVec2(tileX,tileY));
	m_tiles[tileIndex].m_type=type;
}

int Map::GetTileIndexForTileCoords( const IntVec2& tileCoords ) const
{
	return tileCoords.x+(tileDimensions.x*tileCoords.y);
}

IntVec2 Map::GetTileCoordsForIndex( int index )
{
	return m_tiles[index].m_tileCoords;
}

void Map::PushEntityOutOfSolid( Entity* entity, const IntVec2& tileCoords )
{
	AABB2 box=AABB2((float)tileCoords.x,(float)tileCoords.y,(float)tileCoords.x+1,(float)tileCoords.y+1);
	PushDiscOutOFAABB2D(entity->m_position,1.f, box);
}

void Map::HandleCollissions()
{
	/*Vec2 player_position=m_enitites[0]->GetPosition();

	IntVec2 current_tileCoords= IntVec2(RoundDownToInt(player_position.x),RoundDownToInt(player_position.y));

	IntVec2 leftCoords=IntVec2(current_tileCoords.x-1,current_tileCoords.y);
	IntVec2 rightCoords=IntVec2(current_tileCoords.x+1,current_tileCoords.y);
	IntVec2 upCoords=IntVec2(current_tileCoords.x,current_tileCoords.y+1);
	IntVec2 downCoords=IntVec2(current_tileCoords.x,current_tileCoords.y-1);

	IntVec2 topLeftCoords=IntVec2(current_tileCoords.x-1,current_tileCoords.y+1);
	IntVec2 bottomLeftCoords=IntVec2(current_tileCoords.x-1,current_tileCoords.y-1);
	IntVec2 topRightCoords=IntVec2(current_tileCoords.x+1,current_tileCoords.y+1);
	IntVec2 bottomRightCoords=IntVec2(current_tileCoords.x+1,current_tileCoords.y-1);

	AABB2 leftAABB=AABB2((float)leftCoords.x,(float)leftCoords.y,(float)leftCoords.x+1,(float)leftCoords.y+1);
	AABB2 rightAABB=AABB2((float)rightCoords.x,(float)rightCoords.y,(float)rightCoords.x+1,(float)rightCoords.y+1);
	AABB2 upAABB=AABB2((float)upCoords.x,(float)upCoords.y,(float)upCoords.x+1,(float)upCoords.y+1);
	AABB2 downAABB=AABB2((float)downCoords.x,(float)downCoords.y,(float)downCoords.x+1,(float)downCoords.y+1);

	AABB2 topLeftAABB=AABB2((float)topLeftCoords.x,(float)topLeftCoords.y,(float)topLeftCoords.x+1,(float)topLeftCoords.y+1);
	AABB2 bottomRightAABB=AABB2((float)bottomRightCoords.x,(float)bottomRightCoords.y,(float)bottomRightCoords.x+1,(float)bottomRightCoords.y+1);
	AABB2 topRightAABB=AABB2((float)topRightCoords.x,(float)topRightCoords.y,(float)topRightCoords.x+1,(float)topRightCoords.y+1);
	AABB2 bottomLeftAABB=AABB2((float)bottomLeftCoords.x,(float)bottomLeftCoords.y,(float)bottomLeftCoords.x+1,(float)bottomLeftCoords.y+1);

	int leftIndex=GetTileIndexForTileCoords(leftCoords);
	int rightIndex=GetTileIndexForTileCoords(rightCoords);
	int upIndex=GetTileIndexForTileCoords(upCoords);
	int downIndex=GetTileIndexForTileCoords(downCoords);

	int topLeftIndex=GetTileIndexForTileCoords(topLeftCoords);
	int bottomLeftIndex=GetTileIndexForTileCoords(bottomLeftCoords);
	int topRighttIndex=GetTileIndexForTileCoords(topRightCoords);
	int bottomRightIndex=GetTileIndexForTileCoords(bottomRightCoords);

	if( m_tiles[leftIndex].m_type==TILE_TYPE_STONE )
	{
		PushDiscOutOFAABB2D(player_position,.2f,leftAABB);
	}

	if( m_tiles[rightIndex].m_type==TILE_TYPE_STONE )
	{
		PushDiscOutOFAABB2D(player_position,.2f,rightAABB);
	}

	if( m_tiles[upIndex].m_type==TILE_TYPE_STONE )
	{
		PushDiscOutOFAABB2D(player_position,.2f,upAABB);
	}

	if( m_tiles[downIndex].m_type==TILE_TYPE_STONE )
	{
		PushDiscOutOFAABB2D(player_position,.2f,downAABB);
	}

	if( m_tiles[topRighttIndex].m_type==TILE_TYPE_STONE )
	{
		PushDiscOutOFAABB2D( player_position, .2f, topRightAABB );
	}

	if( m_tiles[topLeftIndex].m_type==TILE_TYPE_STONE )
	{
		PushDiscOutOFAABB2D( player_position, .2f, topLeftAABB );
	}

	if( m_tiles[bottomRightIndex].m_type==TILE_TYPE_STONE )
	{
		PushDiscOutOFAABB2D( player_position, .2f, bottomRightAABB );
	}

	if( m_tiles[bottomLeftIndex].m_type==TILE_TYPE_STONE )
	{
		PushDiscOutOFAABB2D( player_position, .2f, bottomLeftAABB );
	}


	m_enitites[0]->SetPosition(player_position);*/
}

void Map::DisplayLanes()
{
	Vec2 mousePos = m_game->GetMousePositionScreenView();

	if ( m_selcetedUnit == UNIT_NONE )
	{
		return;
	}

	if ( m_selcetedUnit == UNIT_DINO && ( m_lane0.IsPointInside( mousePos ) || m_lane1.IsPointInside( mousePos ) || m_lane2.IsPointInside( mousePos ) || m_lane3.IsPointInside( mousePos ) ) )
	{
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->DrawAABB2D( m_lane0 , Rgba8( 100 , 100 , 100 , 100 ) );
		g_theRenderer->DrawAABB2D( m_lane1 , Rgba8( 100 , 100 , 100 , 100 ) );
		g_theRenderer->DrawAABB2D( m_lane2 , Rgba8( 100 , 100 , 100 , 100 ) );
		g_theRenderer->DrawAABB2D( m_lane3 , Rgba8( 100 , 100 , 100 , 100 ) );
		return;
	}


	g_theRenderer->BindTexture( nullptr );

	if ( m_lane0.IsPointInside( mousePos ) )
	{
		g_theRenderer->DrawAABB2D( m_lane0 , Rgba8( 100 , 100 , 100 , 100 ) );
	}

	if ( m_lane1.IsPointInside( mousePos ) )
	{
		g_theRenderer->DrawAABB2D( m_lane1 , Rgba8( 100 , 100 , 100 , 100 ) );
	}

	if ( m_lane2.IsPointInside( mousePos ) )
	{
		g_theRenderer->DrawAABB2D( m_lane2 , Rgba8( 100 , 100 , 100 , 100 ) );
	}

	if ( m_lane3.IsPointInside( mousePos ) )
	{
		g_theRenderer->DrawAABB2D( m_lane3 , Rgba8( 100 , 100 , 100 , 100 ) );
	}

}

void Map::SpawnSelectedUnit()
{
	Vec2 mousePos = m_game->GetMousePositionScreenView();

	if ( m_selcetedUnit == UNIT_NONE )
	{
		return;
	}

	if ( m_selcetedUnit == UNIT_HERO )
	{
		if ( m_lane1.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Hero* temp = new Hero( m_game , Vec2( 4.f , 9.5f ) );
			m_heros.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane0.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Hero* temp = new Hero( m_game , Vec2( 4.f , 4.5f ) );
			m_heros.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane2.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Hero* temp = new Hero( m_game , Vec2( 4.f , 14.5f ) );
			m_heros.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane3.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Hero* temp = new Hero( m_game , Vec2( 4.f , 19.5f ) );
			m_heros.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}
	}

	if ( m_selcetedUnit == UNIT_BOX )
	{
		if ( m_lane1.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Wall* temp = new Wall( m_game , Vec2( mousePos.x , 9.5f ) );
			m_walls.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane0.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Wall* temp = new Wall( m_game , Vec2( mousePos.x , 4.5f ) );
			m_walls.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane2.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Wall* temp = new Wall( m_game , Vec2( mousePos.x , 14.5f ) );
			m_walls.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane3.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Wall* temp = new Wall( m_game , Vec2( mousePos.x , 19.5f ) );
			m_walls.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}
	}


	if ( m_selcetedUnit == UNIT_SPIKE )
	{
		if ( m_lane1.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Spike* temp = new Spike( m_game , Vec2( mousePos.x , 9.5f ) );
			m_spikes.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane0.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Spike* temp = new Spike( m_game , Vec2( mousePos.x , 4.5f ) );
			m_spikes.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane2.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Spike* temp = new Spike( m_game , Vec2( mousePos.x , 14.5f ) );
			m_spikes.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane3.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Spike* temp = new Spike( m_game , Vec2( mousePos.x , 19.5f ) );
			m_spikes.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}
	}

	if ( m_selcetedUnit == UNIT_DINO )
	{
		if ( ( m_lane0.IsPointInside( mousePos ) || m_lane1.IsPointInside( mousePos ) || m_lane2.IsPointInside( mousePos ) || m_lane3.IsPointInside( mousePos ) ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Dino* temp = new Dino( m_game , Vec2( 5.5f , 9.5f ) );
			m_dinos.push_back( temp );

			Dino* temp1 = new Dino( m_game , Vec2( 5.5f , 4.5f ) );
			m_dinos.push_back( temp1 );

			Dino* temp2 = new Dino( m_game , Vec2( 5.5f , 19.5f ) );
			m_dinos.push_back( temp2 );

			Dino* temp3 = new Dino( m_game , Vec2( 5.5f , 14.5f ) );
			m_dinos.push_back( temp3 );

			m_selcetedUnit = UNIT_NONE;
		}
	}
}

void Map::UpdatedSelectedUnits()
{
	Vec2 mousePos = m_game->GetMousePositionScreenView();

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_NONE;
	}

	if ( m_HUD->m_dinoBox.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_DINO;
	}

	if ( m_HUD->m_heroBox.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_HERO;
	}

	if ( m_HUD->m_spikeBox.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_SPIKE;
	}

	if ( m_HUD->m_stoneBox.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_BOX;
	}
}
