#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Engine/Math/MathUtils.hpp"

Map::Map( Game* game, World* world, const IntVec2& tile_dimensions ):m_game(game),m_world(world),tileDimensions(tile_dimensions)
{
	PopulateTiles();
	CreatePlayer();
}

void Map::Update( float deltaSeconds )
{
	ToggleNoClip();
	m_enitites[0]->Update(deltaSeconds);

	if(!noClip)
	{ 
	HandleCollissions();
	}

}



void Map::Render()
{
	for( int index=0; index<m_tiles.size(); index++ )
	{
		m_tiles[index].Render();
	}

	m_enitites[0]->Render();
	
}

void Map::PopulateTiles()
{
	RandomNumberGenerator r=RandomNumberGenerator();


	int numTiles=tileDimensions.x*tileDimensions.y;
	for( int index=0; index<numTiles; index++ )
	{
		int tileX=index % tileDimensions.x;
		int tileY=index / tileDimensions.x;
		
		m_tiles.push_back(Tile(tileX,tileY));
	}

	int ObstaclePercentage = (int)((20 * 30) * PERCENTAGE_OF_OBSTACLE);
	for( int index = 0; index < ObstaclePercentage; index++ )
	{
		int randomTileIndexX = r.GetRandomIntInRange( 1, (int)20-2 );
		int randomTileIndexY = r.GetRandomIntInRange( 1, (int)30-2 );
		SetTileType(randomTileIndexX,randomTileIndexY,TILE_TYPE_STONE);
	}

	for( int yIndex=1; yIndex<6; yIndex++ )
	{
		for( int xIndex=1; xIndex<6; xIndex++ )
		{
			SetTileType(xIndex,yIndex,TILE_TYPE_GRASS);
		}
	}

	for( int yIndex=29; yIndex>23; yIndex-- )
	{
		for( int xIndex=19; xIndex>13; xIndex-- )
		{
			SetTileType(xIndex,yIndex,TILE_TYPE_GRASS);
		}
	}

	SetTileType(4,3,TILE_TYPE_STONE);
	SetTileType(4,4,TILE_TYPE_STONE);
	SetTileType(2,4,TILE_TYPE_STONE);
	SetTileType(3,4,TILE_TYPE_STONE);
	SetTileType(4,2,TILE_TYPE_STONE);

	SetTileType(15,27,TILE_TYPE_STONE);
	SetTileType(15,26,TILE_TYPE_STONE);
	SetTileType(15,25,TILE_TYPE_STONE);
	SetTileType(15,24,TILE_TYPE_STONE);
	SetTileType(16,24,TILE_TYPE_STONE);
	SetTileType(17,24,TILE_TYPE_STONE);


	for( int tileX=0; tileX< tileDimensions.x; tileX++ )
	{
		SetTileType(tileX,0,TILE_TYPE_STONE);
		SetTileType(tileX,tileDimensions.y-1,TILE_TYPE_STONE);
	}

	for( int tileY=0; tileY< tileDimensions.y; tileY++ )
	{
		SetTileType(0,tileY,TILE_TYPE_STONE);
		SetTileType(tileDimensions.x-1,tileY,TILE_TYPE_STONE);
	}

	
}

void Map::CreatePlayer()
{
	Entity* playerTank=new Player(m_game,Vec2(2.5f,2.f));
	m_enitites.push_back(playerTank);
}

void Map::ToggleNoClip()
{
	if( input->WasKeyJustPressed( 0x72 ) )
	{
		noClip=!noClip;
	}
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
	Vec2 player_position=m_enitites[0]->GetPosition();

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


	m_enitites[0]->SetPosition(player_position);
}
