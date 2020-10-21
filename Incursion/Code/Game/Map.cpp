#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/NpcTurret.hpp"
#include "Game/NpcTank.hpp"
#include "Game/Bullet.hpp"
#include "Game/Boulder.hpp"
#include "Game/Explosion.hpp"
#include "Game/AirStrikePlane.hpp"
#include "Game/AirStrike.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // must #include Windows.h before #including Xinput.h
#include <Xinput.h> // include the Xinput API header file (interface)
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use version 9_1_0 explicitly for broadest compatibility


int Map::currentMapNumber =0;

Map::Map( Game* game, World* world, const IntVec2& tile_dimensions ):m_game(game),m_world(world),tileDimensions(tile_dimensions)
{
	currentMapNumber+=1;
	PopulateTiles();
	
	SpawnNewEntity(ENTITY_TYPE_PLAYER,FACTION_GOOD,Vec2(2.5f,2.5f));

	SpawnEntites(5,5,1);

	rng = RandomNumberGenerator();


}

void Map::Update( float deltaSeconds )
{
	ToggleNoClip();
	DetectWin();
	
	m_EntityListByType[ENTITY_TYPE_PLAYER][0]->Update(deltaSeconds);


	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]->Update( deltaSeconds );
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TURRET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]->Update( deltaSeconds );
		}
	}


	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_GOOD_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->Update(deltaSeconds);
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_EVIL_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]->Update( deltaSeconds );
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_EXPLOSION].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_EXPLOSION][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_EXPLOSION][index]->Update( deltaSeconds );
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_AIR_STRIKE].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]->Update( deltaSeconds );
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_AIR_PLANE].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_AIR_PLANE][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_AIR_PLANE][index]->Update( deltaSeconds );
		}
	}
	
	ManagePlayerTankSpeed();
	HandleAirStrikeCollissions();
	if(!noClip)
	{ 
	  HandleBlockCollissions();
	  HandleDiscCollissions();
	  HandleBulletEntityCollissions();
	}
	DeflectBullets();
	DeleteGarbageEntities();

	const XboxController& controller= g_theInput->GetXboxController( 0 );
	const KeyButtonState& start= controller.GetButtonState(XBOX_BUTTON_ID_START);

	if( !controller.IsCOnnected() )
	{
		return;
	}

	if( start.WasJustPressed() )
	{

		Player* p=(Player*)m_EntityListByType[ENTITY_TYPE_PLAYER][0];
		p->RespawnTank();
	}

}



void Map::Render()
{
	
	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile("Data/Images/Terrain_8x8.png");
	g_theRenderer->BindTexture(texture);
	g_theRenderer->DrawVertexArray(tileVertices);
	g_theRenderer->BindTexture(nullptr);

	EntityList& playerList= m_EntityListByType[ENTITY_TYPE_PLAYER];
	playerList[0]->Render();
	
	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TURRET].size(); index++ )
	{
		if(m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]!= nullptr )
		{ 
			m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]->Render();
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); index++ )
	{
		if(m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]!= nullptr )
		{ 
			m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]->Render();
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_GOOD_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->Render();
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_EVIL_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]->Render();
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_BOULDER].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_BOULDER][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_BOULDER][index]->Render();
		}
	}

	
	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_EXPLOSION].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_EXPLOSION][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_EXPLOSION][index]->Render();
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_AIR_STRIKE].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]->Render();
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_AIR_PLANE].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_AIR_PLANE][index]!=nullptr )
		{
			m_EntityListByType[ENTITY_TYPE_AIR_PLANE][index]->Render();
		}
	}

}

void Map::PopulateTiles()
{
	RandomNumberGenerator r=RandomNumberGenerator();


	int numTiles=tileDimensions.x*tileDimensions.y;
	for( int index=0; index<numTiles; index++ )
	{
		int tileX=index % tileDimensions.x;
		int tileY=index / tileDimensions.x;
		
		m_tiles.push_back(Tile(tileX,tileY,defaultTile));
	}

	
	MapFillUsingWorms(TILE_TYPE_STONE);

	MapFillUsingWorms(TILE_TYPE_MUD);
	


	for( int yIndex=1; yIndex<6; yIndex++ )
	{
		for( int xIndex=1; xIndex<6; xIndex++ )
		{
			SetTileType(xIndex,yIndex,TILE_TYPE_START);
		}
	}

	for( int yIndex=tileDimensions.y-1; yIndex>tileDimensions.y-7; yIndex-- )
	{
		for( int xIndex=tileDimensions.x-1; xIndex>tileDimensions.x-7; xIndex-- )
		{
			SetTileType(xIndex,yIndex,TILE_TYPE_START);
		}
	}

	SetTileType( 4, 3, TILE_TYPE_STONE );
	SetTileType( 4, 4, TILE_TYPE_STONE );
	SetTileType( 2, 4, TILE_TYPE_STONE );
	SetTileType( 3, 4, TILE_TYPE_STONE );
	SetTileType( 4, 2, TILE_TYPE_STONE );

	SetTileType( tileDimensions.x-5, tileDimensions.y-3, TILE_TYPE_STONE );
	SetTileType( tileDimensions.x-5, tileDimensions.y-4, TILE_TYPE_STONE );
	SetTileType( tileDimensions.x-5, tileDimensions.y-5, TILE_TYPE_STONE );
	SetTileType( tileDimensions.x-5, tileDimensions.y-6, TILE_TYPE_STONE );
	SetTileType( tileDimensions.x-4, tileDimensions.y-6, TILE_TYPE_STONE );
	SetTileType( tileDimensions.x-3, tileDimensions.y-6, TILE_TYPE_STONE );


	for( int tileX=0; tileX< tileDimensions.x; tileX++ )
	{
		SetTileType(tileX,0,edgeTile);
		SetTileType(tileX,tileDimensions.y-1,edgeTile);
	}

	for( int tileY=0; tileY< tileDimensions.y; tileY++ )
	{
		SetTileType(0,tileY,edgeTile);
		SetTileType(tileDimensions.x-1,tileY,edgeTile);
	}

	SetTileType(tileDimensions.x-2,tileDimensions.y-2,endTile);

	if(IsMapValidityUsingFloodFill())
	{ 

		for( int verticalIndex = 1; verticalIndex<tileDimensions.y-1; verticalIndex++ )
		{
			for( int horizontalIndex =1; horizontalIndex <tileDimensions.x-1; horizontalIndex++ )
			{
				int tileIndex = GetTileIndexForTileCoords( IntVec2( horizontalIndex, verticalIndex ) );
				if( m_tiles[tileIndex].hasBeenSeen == false )
				{
					SetTileType( horizontalIndex, verticalIndex, TILE_TYPE_STONE );
				}
			}
		}

	}

	for( int verticalIndex = 0; verticalIndex<tileDimensions.y; verticalIndex++ )
	{
		for( int horizontalIndex =0; horizontalIndex <tileDimensions.x; horizontalIndex++ )
		{
			int index = verticalIndex * tileDimensions.x + horizontalIndex;
			AABB2 aabb = AABB2((float)horizontalIndex,(float)verticalIndex,(float)horizontalIndex+1,(float)verticalIndex+1);
			Vec2 minUVs;
			Vec2 maxUvs;

			TileType temp = m_tiles[index].m_type;
			TileDefinition::s_definitions[temp].m_sprite.GetUVs(minUVs,maxUvs);

			AppendAABB2(tileVertices,aabb,Rgba8(255,255,255,255),minUVs,maxUvs);
		}
	}

	
}


void Map::ToggleNoClip()
{
	if( g_theInput->WasKeyJustPressed( 0x72 ) )
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

void Map::HandleBlockCollissions()
{
	//Block Colission For Player
	for( int index=0; index< m_EntityListByType[ENTITY_TYPE_PLAYER].size(); index++ )
	{
		if( !m_EntityListByType[ENTITY_TYPE_PLAYER][index]->m_isPushedByWalls )
		{
			break;
		}
		Vec2 player_position=m_EntityListByType[ENTITY_TYPE_PLAYER][index]->GetPosition();

		IntVec2 current_tileCoords= IntVec2( RoundDownToInt( player_position.x ), RoundDownToInt( player_position.y ) );

		IntVec2 leftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y );
		IntVec2 rightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y );
		IntVec2 upCoords=IntVec2( current_tileCoords.x, current_tileCoords.y+1 );
		IntVec2 downCoords=IntVec2( current_tileCoords.x, current_tileCoords.y-1 );

		IntVec2 topLeftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y+1 );
		IntVec2 bottomLeftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y-1 );
		IntVec2 topRightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y+1 );
		IntVec2 bottomRightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y-1 );

		AABB2 leftAABB=AABB2( (float)leftCoords.x, (float)leftCoords.y, (float)leftCoords.x+1, (float)leftCoords.y+1 );
		AABB2 rightAABB=AABB2( (float)rightCoords.x, (float)rightCoords.y, (float)rightCoords.x+1, (float)rightCoords.y+1 );
		AABB2 upAABB=AABB2( (float)upCoords.x, (float)upCoords.y, (float)upCoords.x+1, (float)upCoords.y+1 );
		AABB2 downAABB=AABB2( (float)downCoords.x, (float)downCoords.y, (float)downCoords.x+1, (float)downCoords.y+1 );

		AABB2 topLeftAABB=AABB2( (float)topLeftCoords.x, (float)topLeftCoords.y, (float)topLeftCoords.x+1, (float)topLeftCoords.y+1 );
		AABB2 bottomRightAABB=AABB2( (float)bottomRightCoords.x, (float)bottomRightCoords.y, (float)bottomRightCoords.x+1, (float)bottomRightCoords.y+1 );
		AABB2 topRightAABB=AABB2( (float)topRightCoords.x, (float)topRightCoords.y, (float)topRightCoords.x+1, (float)topRightCoords.y+1 );
		AABB2 bottomLeftAABB=AABB2( (float)bottomLeftCoords.x, (float)bottomLeftCoords.y, (float)bottomLeftCoords.x+1, (float)bottomLeftCoords.y+1 );


		if( IsTileSolid(leftCoords) )
		{
			PushDiscOutOFAABB2D( player_position, .2f, leftAABB );
		}
		if( IsTileSolid(rightCoords) )
		{
			PushDiscOutOFAABB2D( player_position, .2f, rightAABB );
		}
		if( IsTileSolid(upCoords) )
		{
			PushDiscOutOFAABB2D( player_position, .2f, upAABB );
		}
		if( IsTileSolid(downCoords) )
		{
			PushDiscOutOFAABB2D( player_position, .2f, downAABB );
		}
		if( IsTileSolid(topRightCoords) )
		{
			PushDiscOutOFAABB2D( player_position, .2f, topRightAABB );
		}
		if( IsTileSolid(topLeftCoords) )
		{
			PushDiscOutOFAABB2D( player_position, .2f, topLeftAABB );
		}
		if( IsTileSolid(bottomRightCoords) )
		{
			PushDiscOutOFAABB2D( player_position, .2f, bottomRightAABB );
		}
		if( IsTileSolid(bottomLeftCoords) )
		{
			PushDiscOutOFAABB2D( player_position, .2f, bottomLeftAABB );
		}

		m_EntityListByType[ENTITY_TYPE_PLAYER][index]->SetPosition( player_position );
	}

   //Block Colission for NPC Tank
	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); index++ )
	{
		if(m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]!=nullptr )
		{ 
			if( !m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]->m_isPushedByWalls )
			{
				break;
			}
			Vec2 position= m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]->GetPosition();
			IntVec2 current_tileCoords= IntVec2( RoundDownToInt( position.x ), RoundDownToInt( position.y ) );

			IntVec2 leftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y );
			IntVec2 rightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y );
			IntVec2 upCoords=IntVec2( current_tileCoords.x, current_tileCoords.y+1 );
			IntVec2 downCoords=IntVec2( current_tileCoords.x, current_tileCoords.y-1 );

			IntVec2 topLeftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y+1 );
			IntVec2 bottomLeftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y-1 );
			IntVec2 topRightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y+1 );
			IntVec2 bottomRightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y-1 );

			AABB2 leftAABB=AABB2( (float)leftCoords.x, (float)leftCoords.y, (float)leftCoords.x+1, (float)leftCoords.y+1 );
			AABB2 rightAABB=AABB2( (float)rightCoords.x, (float)rightCoords.y, (float)rightCoords.x+1, (float)rightCoords.y+1 );
			AABB2 upAABB=AABB2( (float)upCoords.x, (float)upCoords.y, (float)upCoords.x+1, (float)upCoords.y+1 );
			AABB2 downAABB=AABB2( (float)downCoords.x, (float)downCoords.y, (float)downCoords.x+1, (float)downCoords.y+1 );

			AABB2 topLeftAABB=AABB2( (float)topLeftCoords.x, (float)topLeftCoords.y, (float)topLeftCoords.x+1, (float)topLeftCoords.y+1 );
			AABB2 bottomRightAABB=AABB2( (float)bottomRightCoords.x, (float)bottomRightCoords.y, (float)bottomRightCoords.x+1, (float)bottomRightCoords.y+1 );
			AABB2 topRightAABB=AABB2( (float)topRightCoords.x, (float)topRightCoords.y, (float)topRightCoords.x+1, (float)topRightCoords.y+1 );
			AABB2 bottomLeftAABB=AABB2( (float)bottomLeftCoords.x, (float)bottomLeftCoords.y, (float)bottomLeftCoords.x+1, (float)bottomLeftCoords.y+1 );

			int leftIndex=GetTileIndexForTileCoords( leftCoords );
			int rightIndex=GetTileIndexForTileCoords( rightCoords );
			int upIndex=GetTileIndexForTileCoords( upCoords );
			int downIndex=GetTileIndexForTileCoords( downCoords );

			int topLeftIndex=GetTileIndexForTileCoords( topLeftCoords );
			int bottomLeftIndex=GetTileIndexForTileCoords( bottomLeftCoords );
			int topRighttIndex=GetTileIndexForTileCoords( topRightCoords );
			int bottomRightIndex=GetTileIndexForTileCoords( bottomRightCoords );

			if(leftIndex < (tileDimensions.x * tileDimensions.y -1) )
			{ 
				if(/* m_tiles[leftIndex].m_type==TILE_TYPE_STONE */ IsTileSolid(leftCoords))
				{
					PushDiscOutOFAABB2D( position, .2f, leftAABB );
				}
			}
			if(rightIndex < (tileDimensions.x * tileDimensions.y -1) )
			{ 
				if( IsTileSolid(rightCoords) )
				{
					PushDiscOutOFAABB2D( position, .2f, rightAABB );
				}
			}
			if(upIndex < (tileDimensions.x * tileDimensions.y -1) )
			{ 
				if( IsTileSolid(upCoords) )
				{
					PushDiscOutOFAABB2D( position, .2f, upAABB );
				}
			}
			if(downIndex < (tileDimensions.x * tileDimensions.y -1) )
			{ 
				if( IsTileSolid(downCoords) )
				{
					PushDiscOutOFAABB2D( position, .2f, downAABB );
				}
			}
			if(topRighttIndex < (tileDimensions.x * tileDimensions.y -1) )
			{ 
				if( IsTileSolid(topRightCoords) )
				{
					PushDiscOutOFAABB2D( position, .2f, topRightAABB );
				}
			}
			if(topLeftIndex < (tileDimensions.x * tileDimensions.y -1) )
			{ 
				if( IsTileSolid(topLeftCoords) )
				{
					PushDiscOutOFAABB2D( position, .2f, topLeftAABB );
				}
			}

			if(bottomRightIndex < (tileDimensions.x * tileDimensions.y -1) )
			{ 
				if( IsTileSolid(bottomRightCoords) )
				{
					PushDiscOutOFAABB2D( position, .2f, bottomRightAABB );
				}
			}

			if( bottomLeftIndex < (tileDimensions.x * tileDimensions.y -1) )
			{ 
				if( IsTileSolid(bottomLeftCoords) )
				{
					PushDiscOutOFAABB2D( position, .2f, bottomLeftAABB );
				}
			}

			m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]->SetPosition( position );
		}
	}

	//Block Colission for NPC Turret
	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TURRET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]!=nullptr )
		{
			if( !m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]->m_isPushedByWalls )
			{
				break;
			}
			Vec2 position= m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]->GetPosition();
			IntVec2 current_tileCoords= IntVec2( RoundDownToInt( position.x ), RoundDownToInt( position.y ) );

			IntVec2 leftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y );
			IntVec2 rightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y );
			IntVec2 upCoords=IntVec2( current_tileCoords.x, current_tileCoords.y+1 );
			IntVec2 downCoords=IntVec2( current_tileCoords.x, current_tileCoords.y-1 );

			IntVec2 topLeftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y+1 );
			IntVec2 bottomLeftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y-1 );
			IntVec2 topRightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y+1 );
			IntVec2 bottomRightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y-1 );

			AABB2 leftAABB=AABB2( (float)leftCoords.x, (float)leftCoords.y, (float)leftCoords.x+1, (float)leftCoords.y+1 );
			AABB2 rightAABB=AABB2( (float)rightCoords.x, (float)rightCoords.y, (float)rightCoords.x+1, (float)rightCoords.y+1 );
			AABB2 upAABB=AABB2( (float)upCoords.x, (float)upCoords.y, (float)upCoords.x+1, (float)upCoords.y+1 );
			AABB2 downAABB=AABB2( (float)downCoords.x, (float)downCoords.y, (float)downCoords.x+1, (float)downCoords.y+1 );

			AABB2 topLeftAABB=AABB2( (float)topLeftCoords.x, (float)topLeftCoords.y, (float)topLeftCoords.x+1, (float)topLeftCoords.y+1 );
			AABB2 bottomRightAABB=AABB2( (float)bottomRightCoords.x, (float)bottomRightCoords.y, (float)bottomRightCoords.x+1, (float)bottomRightCoords.y+1 );
			AABB2 topRightAABB=AABB2( (float)topRightCoords.x, (float)topRightCoords.y, (float)topRightCoords.x+1, (float)topRightCoords.y+1 );
			AABB2 bottomLeftAABB=AABB2( (float)bottomLeftCoords.x, (float)bottomLeftCoords.y, (float)bottomLeftCoords.x+1, (float)bottomLeftCoords.y+1 );

			if( IsTileSolid(leftCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, leftAABB );
			}
			if( IsTileSolid(rightCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, rightAABB );
			}
			if( IsTileSolid(upCoords))
			{
				PushDiscOutOFAABB2D( position, .2f, upAABB );
			}
			if( IsTileSolid(downCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, downAABB );
			}
			if( IsTileSolid(topRightCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, topRightAABB );
			}
			if( IsTileSolid(topLeftCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, topLeftAABB );
			}
			if( IsTileSolid(bottomRightCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, bottomRightAABB );
			}
			if( IsTileSolid(bottomLeftCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, bottomLeftAABB );
			}

			m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]->SetPosition( position );
		}
	}

	////Delete Bullet if it is currently on a Block tile
	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_GOOD_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index] != nullptr )
		{

		Vec2 bulletPosition = m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->GetPosition();
		IntVec2 current_tileCoords= IntVec2( RoundDownToInt( bulletPosition.x ), RoundDownToInt( bulletPosition.y ) );

		if( IsTileSolid( current_tileCoords ) )
		{
			m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->Die();
			CreateExplosions(0.4f,0.4f,m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->GetPosition());
		}

		}

	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_EVIL_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index] != nullptr )
		{

			Vec2 bulletPosition = m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]->GetPosition();
			IntVec2 current_tileCoords= IntVec2( RoundDownToInt( bulletPosition.x ), RoundDownToInt( bulletPosition.y ) );

			if( IsTileSolid( current_tileCoords ) )
			{
				m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]->Die();
				CreateExplosions(0.4f,0.4f,m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]->GetPosition());
			}

		}

	}

	//Block Colission for NPC Boulder
	for( int index=0; index< m_EntityListByType[ENTITY_TYPE_BOULDER].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_BOULDER][index]!=nullptr )
		{
			if( !m_EntityListByType[ENTITY_TYPE_BOULDER][index]->m_isPushedByWalls )
			{
				break;
			}
			Vec2 position= m_EntityListByType[ENTITY_TYPE_BOULDER][index]->GetPosition();
			IntVec2 current_tileCoords= IntVec2( RoundDownToInt( position.x ), RoundDownToInt( position.y ) );

			IntVec2 leftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y );
			IntVec2 rightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y );
			IntVec2 upCoords=IntVec2( current_tileCoords.x, current_tileCoords.y+1 );
			IntVec2 downCoords=IntVec2( current_tileCoords.x, current_tileCoords.y-1 );

			IntVec2 topLeftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y+1 );
			IntVec2 bottomLeftCoords=IntVec2( current_tileCoords.x-1, current_tileCoords.y-1 );
			IntVec2 topRightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y+1 );
			IntVec2 bottomRightCoords=IntVec2( current_tileCoords.x+1, current_tileCoords.y-1 );

			AABB2 leftAABB=AABB2( (float)leftCoords.x, (float)leftCoords.y, (float)leftCoords.x+1, (float)leftCoords.y+1 );
			AABB2 rightAABB=AABB2( (float)rightCoords.x, (float)rightCoords.y, (float)rightCoords.x+1, (float)rightCoords.y+1 );
			AABB2 upAABB=AABB2( (float)upCoords.x, (float)upCoords.y, (float)upCoords.x+1, (float)upCoords.y+1 );
			AABB2 downAABB=AABB2( (float)downCoords.x, (float)downCoords.y, (float)downCoords.x+1, (float)downCoords.y+1 );

			AABB2 topLeftAABB=AABB2( (float)topLeftCoords.x, (float)topLeftCoords.y, (float)topLeftCoords.x+1, (float)topLeftCoords.y+1 );
			AABB2 bottomRightAABB=AABB2( (float)bottomRightCoords.x, (float)bottomRightCoords.y, (float)bottomRightCoords.x+1, (float)bottomRightCoords.y+1 );
			AABB2 topRightAABB=AABB2( (float)topRightCoords.x, (float)topRightCoords.y, (float)topRightCoords.x+1, (float)topRightCoords.y+1 );
			AABB2 bottomLeftAABB=AABB2( (float)bottomLeftCoords.x, (float)bottomLeftCoords.y, (float)bottomLeftCoords.x+1, (float)bottomLeftCoords.y+1 );

			if( IsTileSolid(leftCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, leftAABB );
			}
			if( IsTileSolid(rightCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, rightAABB );
			}
			if( IsTileSolid(upCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, upAABB );
			}
			if( IsTileSolid(downCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, downAABB );
			}
			if( IsTileSolid(topRightCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, topRightAABB );
			}
			if( IsTileSolid(topLeftCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, topLeftAABB );
			}
			if( IsTileSolid(bottomRightCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, bottomRightAABB );
			}
			if( IsTileSolid(bottomLeftCoords) )
			{
				PushDiscOutOFAABB2D( position, .2f, bottomLeftAABB );
			}

			m_EntityListByType[ENTITY_TYPE_BOULDER][index]->SetPosition( position );
		}
	}


}

void Map::HandleDiscCollissions()
{
	Vec2 player_position= m_EntityListByType[ENTITY_TYPE_PLAYER][0]->GetPosition();

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TURRET].size(); index++ )
	{
		if(m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]!= nullptr )
		{ 
		Vec2 enemy_position=m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]->GetPosition();
		PushMobileDiscOutOfFixedDisc(player_position,0.2f,enemy_position,0.2f);
		m_EntityListByType[ENTITY_TYPE_PLAYER][0]->SetPosition(player_position);
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); index++ )
	{
		if(m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]!= nullptr )
		{ 
		Vec2 enemy_position=m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]->GetPosition();
		PushDIscOutOfEachOther2D(player_position,0.2f,enemy_position,0.2f);
		m_EntityListByType[ENTITY_TYPE_PLAYER][0]->SetPosition( player_position );
		m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]->SetPosition(enemy_position);
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_BOULDER].size(); index++ )
	{
		if(m_EntityListByType[ENTITY_TYPE_BOULDER][index]!=nullptr)
		{ 
		Vec2 boulder_position = m_EntityListByType[ENTITY_TYPE_BOULDER][index]->GetPosition();
		PushDIscOutOfEachOther2D(player_position,0.2f,boulder_position,0.2f);
		m_EntityListByType[ENTITY_TYPE_PLAYER][0]->SetPosition(player_position);
		m_EntityListByType[ENTITY_TYPE_BOULDER][index]->SetPosition(boulder_position);
		}
	}

	for( int tankIndex =0; tankIndex<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); tankIndex++ )
	{
		
		if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex]!=nullptr )
		{
			Vec2 tankPosition = m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex]->GetPosition();

			for( int turretIndex=0; turretIndex<m_EntityListByType[ENTITY_TYPE_NPC_TURRET].size(); turretIndex++ )
			{
				if( m_EntityListByType[ENTITY_TYPE_NPC_TURRET][turretIndex]!=nullptr )
				{
					Vec2 turretPosition = m_EntityListByType[ENTITY_TYPE_NPC_TURRET][turretIndex]->GetPosition();
					PushMobileDiscOutOfFixedDisc(tankPosition,0.2f,turretPosition,0.2f);
					m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex]->SetPosition(tankPosition);

				}
			}
		}
	}

	for( int tankIndex =0; tankIndex<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); tankIndex++ )
	{

		if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex]!=nullptr )
		{
			Vec2 tankPosition = m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex]->GetPosition();

			for( int boulderIndex=0; boulderIndex<m_EntityListByType[ENTITY_TYPE_BOULDER].size(); boulderIndex++ )
			{
				if( m_EntityListByType[ENTITY_TYPE_BOULDER][boulderIndex]!=nullptr )
				{
					Vec2 boulderPosition = m_EntityListByType[ENTITY_TYPE_BOULDER][boulderIndex]->GetPosition();
					PushDIscOutOfEachOther2D(tankPosition,0.2f,boulderPosition,0.2f);
					m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex]->SetPosition( tankPosition );
					m_EntityListByType[ENTITY_TYPE_BOULDER][boulderIndex]->SetPosition(boulderPosition);

				}
			}
		}
	}


	for( int tankIndex =0; tankIndex<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); tankIndex++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex]!=nullptr )
		{
			Vec2 tankPosition = m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex]->GetPosition();
			for( int tankIndex2=tankIndex+1; tankIndex2<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); tankIndex2++ )
			{
				if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex2]!= nullptr )
				{
					Vec2 tankPosition2 =m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex2]->GetPosition();
					PushDIscOutOfEachOther2D(tankPosition,0.2f,tankPosition2,0.2f);
					m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex]->SetPosition( tankPosition );
					m_EntityListByType[ENTITY_TYPE_NPC_TANK][tankIndex2]->SetPosition( tankPosition2 );
				}
			}
		}
	}

	


}



void Map::HandleBulletEntityCollissions()
{
	//Good Bullet impact all evil entities

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_GOOD_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]!=nullptr )
		{
			for( int TankIndex=0; TankIndex<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); TankIndex++ )
			{
				if(m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]!=nullptr )
				{ 
					if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]->entityFaction == FACTION_EVIL )
					{
						if( DoDiscsOverlap( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->GetPosition(), 0.02f, m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]->GetPosition(), 0.2f ) )
						{
							m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]->m_health--;
							m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->Die();
							CreateExplosions(0.4f,0.4f,m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->GetPosition());
							g_theAudio->PlaySound(ENEMY_HIT);
							if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]->m_health<=0 )
							{
								g_theAudio->PlaySound(ENEMY_DIED);
								CreateExplosions(1.f,1.f,m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]->GetPosition());
								m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]->Die();
							}
						}
					}
				}
				
			}
		}
	}


	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_GOOD_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]!=nullptr )
		{
			for( int TankIndex=0; TankIndex<m_EntityListByType[ENTITY_TYPE_NPC_TURRET].size(); TankIndex++ )
			{
				if( m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]!=nullptr )
				{
					if( m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]->entityFaction == FACTION_EVIL )
					{
						if( DoDiscsOverlap( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->GetPosition(), 0.02f, m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]->GetPosition(), 0.2f ) )
						{
							m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]->m_health--;
							m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->Die();
							CreateExplosions(0.4f,0.4f,m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->GetPosition());
							g_theAudio->PlaySound(ENEMY_HIT);
							if( m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]->m_health<=0 )
							{
								m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]->Die();
								CreateExplosions(1.f,1.f,m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]->GetPosition());
								g_theAudio->PlaySound(ENEMY_DIED);
							}
						}
					}
				}

			}
		}
	}

  //Evil Bullet impact all good entities

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_EVIL_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]!=nullptr )
		{
			for( int tankIndex=0; tankIndex<m_EntityListByType[ENTITY_TYPE_PLAYER].size(); tankIndex++ )
			{
				if( m_EntityListByType[ENTITY_TYPE_PLAYER][tankIndex]!=nullptr )
				{
					if( m_EntityListByType[ENTITY_TYPE_PLAYER][tankIndex]->entityFaction == FACTION_GOOD )
					{
						if(IsPlayerAlive() )
						{ 
							if( DoDiscsOverlap( m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]->GetPosition(), 0.02f, m_EntityListByType[ENTITY_TYPE_PLAYER][tankIndex]->GetPosition(), 0.2f ) )
							{
								m_EntityListByType[ENTITY_TYPE_PLAYER][tankIndex]->m_health--;
								m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]->Die();
								CreateExplosions( 0.4f, 0.4f, m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]->GetPosition() );
								g_theAudio->PlaySound( PLAYER_HIT );

								if( !m_EntityListByType[ENTITY_TYPE_PLAYER][tankIndex]->m_isGarbage )
								{
									if( m_EntityListByType[ENTITY_TYPE_PLAYER][tankIndex]->m_health <= 0 )
									{
										m_EntityListByType[ENTITY_TYPE_PLAYER][tankIndex]->Die();
										m_game->currentLives--;
										CreateExplosions( 2.f, 2.f, m_EntityListByType[ENTITY_TYPE_PLAYER][tankIndex]->GetPosition() );
										g_theAudio->PlaySound( PLAYER_DIED );
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Map::HandleAirStrikeCollissions()
{
	//Air strike collissions impacts turrets and NPC tanks

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_AIR_STRIKE].size(); index++ )
	{
		if(m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]!= nullptr )
		{ 
			for( int TankIndex=0; TankIndex<m_EntityListByType[ENTITY_TYPE_NPC_TURRET].size(); TankIndex++ )
			{
				if( m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]!=nullptr )
				{
					if( DoDiscsOverlap( m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]->GetPosition(), 1.f, m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]->GetPosition(), 0.2f ) )
					{
						m_EntityListByType[ENTITY_TYPE_NPC_TURRET][TankIndex]->Die();
					}
				}
			}
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_AIR_STRIKE].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]!= nullptr )
		{
			for( int TankIndex=0; TankIndex<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); TankIndex++ )
			{
				if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]!=nullptr )
				{
					if( DoDiscsOverlap( m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]->GetPosition(), 1.f, m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]->GetPosition(), 0.2f ) )
					{
						m_EntityListByType[ENTITY_TYPE_NPC_TANK][TankIndex]->Die();
					}
				}
			}
		}
	}

}

void Map::SpawnNewEntity( EntityType entityType, EntityFaction faction, const Vec2& spawnPosition )
{
	Entity* entity= nullptr;

	switch( entityType )
	{
	case ENTITY_TYPE_PLAYER:
	{
		entity = new Player(m_game,spawnPosition);
		entity->entityType=entityType;
		entity->entityFaction=faction;
		AddEntityToMap(entity,entityType);
		break;
	}
	case ENTITY_TYPE_NPC_TURRET:
	{
		entity= new NpcTurret(m_game,spawnPosition);
		entity->entityType=entityType;
		entity->entityFaction=faction;
		AddEntityToMap(entity,entityType);
		break;
	}
	case ENTITY_TYPE_NPC_TANK:
	{
		entity= new NpcTank(m_game,spawnPosition);
		entity->entityType=entityType;
		entity->entityFaction=faction;
		AddEntityToMap(entity,entityType);
		break;
	}

	case ENTITY_TYPE_BOULDER:
	{
		entity = new Boulder(m_game,spawnPosition);
		entity->entityType=entityType;
		entity->entityFaction =faction;
		AddEntityToMap(entity,entityType);
		break;
	}

	default:
		break;
	}

	   //
}

void Map::AddEntityToMap( Entity* entity, EntityType entityType )
{
	for( int index=0; index<m_EntityListByType[entityType].size(); index++ )
	{
		if( m_EntityListByType[entityType][index] == nullptr )
		{
			m_EntityListByType[entityType][index] = entity;
			return;
		}
	}

	m_EntityListByType[entityType].push_back( entity );
}


RayCastResult Map::Raycast( Vec2 Start, Vec2 fwdDir, float maxDistance )
{
	RayCastResult result;
	float stepSize= 0.1f;
	float curretDistance=0.f;
	Vec2 temp=Start;

	while(curretDistance <=maxDistance )
	{ 
		Start+=fwdDir*stepSize;
		curretDistance+=stepSize;
		IntVec2 current_tileCoords= IntVec2( RoundDownToInt( Start.x ), RoundDownToInt( Start.y ) );

		if( IsTileSolid( current_tileCoords ) )
		{
			result.m_didImpact=true;
			result.m_impactDistance=curretDistance;
			result.m_impactFraction=curretDistance/maxDistance;
			result.m_impactPosition=Start;
			break;
		}
	
	}

	if( curretDistance>=maxDistance )
	{
		result.m_didImpact=false;
		result.m_impactDistance=maxDistance;
		result.m_impactPosition=Start;
		result.m_impactFraction=1.f;
	}


	return result;
}

bool Map::IsTileSolid( IntVec2& tileCoords )
{
	int tileIndex=GetTileIndexForTileCoords(tileCoords); 

	/*if( m_tiles[tileIndex].m_type== TILE_TYPE_STONE )
	{
		return true;
	}*/

	if( TileDefinition::s_definitions[m_tiles[tileIndex].m_type].isSolid )
	{
		return true;
	}

	return false;
}

bool Map::IsPointInSolid( Vec2 point )
{
	IntVec2 roundedPoint = IntVec2(RoundDownToInt(point.x),RoundDownToInt(point.y));
	return IsTileSolid(roundedPoint);
}

bool Map::HasLineOfSight( Vec2 destination, Vec2 source, float maxDistance )
{
	Vec2 fwdDir = destination-source;
	fwdDir.Normalize();
	float distance = sqrtf((destination.x-source.x)*(destination.x-source.x)+(destination.y-source.y)*(destination.y-source.y));

	if( distance>maxDistance )
	{
		return false;
	}

	RayCastResult r= Raycast(source,fwdDir,distance);

	if( !r.m_didImpact )
	{
		return true;
	}

	return false;
}

Vec2 Map::GetPlayerPosition()
{
	return m_EntityListByType[ENTITY_TYPE_PLAYER][0]->GetPosition();
}

void Map::SpawnBullet( Vec2 position, EntityType entityType, float orientationDegrees )
{
	Entity* entity= new Bullet(m_game,position);
	entity->m_orientationDegrees=orientationDegrees;
	entity->entityType=entityType;

	AddEntityToMap(entity,entityType);
}

void Map::DeleteGarbageEntities()
{

	for( int playerIndex =0; playerIndex<m_EntityListByType[ENTITY_TYPE_PLAYER].size(); playerIndex++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_PLAYER][playerIndex]->m_isGarbage )
		{
			m_EntityListByType[ENTITY_TYPE_PLAYER][playerIndex]->m_isDead = true;
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TURRET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]==nullptr )
		{
			continue;
		}
		if( m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]->m_isGarbage )
		{
			delete m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index];
			m_EntityListByType[ENTITY_TYPE_NPC_TURRET][index]=nullptr;
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_NPC_TANK].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]==nullptr )
		{
			continue;
		}
		if( m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]->m_isGarbage )
		{
			delete m_EntityListByType[ENTITY_TYPE_NPC_TANK][index];
			m_EntityListByType[ENTITY_TYPE_NPC_TANK][index]=nullptr;
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_GOOD_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]==nullptr )
		{
			continue;
		}
		if( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]->m_isGarbage )
		{
			delete m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index];
			m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][index]=nullptr;
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_EVIL_BULLET].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]==nullptr )
		{
			continue;
		}
		if( m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]->m_isGarbage )
		{
			delete m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index];
			m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][index]=nullptr;
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_EXPLOSION].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_EXPLOSION][index]==nullptr )
		{
			continue;
		}
		if( m_EntityListByType[ENTITY_TYPE_EXPLOSION][index]->m_isGarbage )
		{
			delete m_EntityListByType[ENTITY_TYPE_EXPLOSION][index];
			m_EntityListByType[ENTITY_TYPE_EXPLOSION][index]=nullptr;
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_AIR_STRIKE].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]==nullptr )
		{
			continue;
		}
		if( m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]->m_isGarbage )
		{
			delete m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index];
			m_EntityListByType[ENTITY_TYPE_AIR_STRIKE][index]=nullptr;
		}
	}

	for( int index=0; index<m_EntityListByType[ENTITY_TYPE_AIR_PLANE].size(); index++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_AIR_PLANE][index]==nullptr )
		{
			continue;
		}
		if( m_EntityListByType[ENTITY_TYPE_AIR_PLANE][index]->m_isGarbage )
		{
			delete m_EntityListByType[ENTITY_TYPE_AIR_PLANE][index];
			m_EntityListByType[ENTITY_TYPE_AIR_PLANE][index]=nullptr;
		}
	}
	

	//Change player's status only

	if( m_EntityListByType[ENTITY_TYPE_PLAYER][0]->m_isGarbage )
	{
		m_EntityListByType[ENTITY_TYPE_PLAYER][0]->m_isDead=true;
	}

}

void Map::DeflectBullets()
{
	for( int bulletIndex=0; bulletIndex<m_EntityListByType[ENTITY_TYPE_GOOD_BULLET].size(); bulletIndex++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][bulletIndex]!=nullptr )
		{
			for( int boulderIndex=0; boulderIndex<m_EntityListByType[ENTITY_TYPE_BOULDER].size(); boulderIndex++ )
			{
				if( DoDiscsOverlap( m_EntityListByType[ENTITY_TYPE_BOULDER][boulderIndex]->GetPosition(), 0.45f, m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][bulletIndex]->GetPosition(), 0.02f ))
				{
					Bullet *b = (Bullet*)m_EntityListByType[ENTITY_TYPE_GOOD_BULLET][bulletIndex];
					b->ChangeDirection();
				}
			}
		}
	}

	for( int bulletIndex=0; bulletIndex<m_EntityListByType[ENTITY_TYPE_EVIL_BULLET].size(); bulletIndex++ )
	{
		if( m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][bulletIndex]!=nullptr )
		{
			for( int boulderIndex=0; boulderIndex<m_EntityListByType[ENTITY_TYPE_BOULDER].size(); boulderIndex++ )
			{
				if( DoDiscsOverlap( m_EntityListByType[ENTITY_TYPE_BOULDER][boulderIndex]->GetPosition(), 0.45f, m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][bulletIndex]->GetPosition(), 0.02f ) )
				{
					Bullet* b = (Bullet*)m_EntityListByType[ENTITY_TYPE_EVIL_BULLET][bulletIndex];
					b->ChangeDirection();
				}
			}
		}
	}
}



bool Map::IsPlayerAlive()
{
	if( m_EntityListByType[ENTITY_TYPE_PLAYER][0]->m_isDead )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Map::SpawnEntites( int num_NPCTurrets, int num_NPCTanks, int num_Boulders )
{
	

	for( int index=0; index<num_NPCTurrets; index++ )
	{
		while(true)
		{ 
			float x=(float)rng.GetRandomIntInRange( 3, tileDimensions.x-2 );
			float y=(float)rng.GetRandomIntInRange( 3, tileDimensions.y-2 );

			if( !IsPointInSolid( Vec2(x, y) ) )
			{
				SpawnNewEntity(ENTITY_TYPE_NPC_TURRET,FACTION_EVIL,Vec2(x+0.4f,y+0.4f));
				break;
			}
		}

	}

	for( int index=0; index<num_NPCTanks; index++ )
	{
		while( true )
		{
			float x=(float)rng.GetRandomIntInRange( 3, tileDimensions.x-2 );
			float y=(float)rng.GetRandomIntInRange( 3, tileDimensions.y-2 );

			if( !IsPointInSolid( Vec2( x, y ) ) )
			{
				SpawnNewEntity( ENTITY_TYPE_NPC_TANK, FACTION_EVIL, Vec2( x+0.4f, y+0.4f ) );
				break;
			}
		}

	}

	for( int index=0; index<num_Boulders; index++ )
	{
		while( true )
		{
			float x=(float)rng.GetRandomIntInRange( 3, tileDimensions.x-2 );
			float y=(float)rng.GetRandomIntInRange( 3, tileDimensions.y-2 );

			if( !IsPointInSolid( Vec2( x, y ) ) )
			{
				SpawnNewEntity( ENTITY_TYPE_BOULDER, FACTION_NEUTRAL, Vec2( x+0.4f, y+0.4f ) );
				break;
			}
		}

	}
}

void Map::ManagePlayerTankSpeed()
{
	const XboxController& controller= g_theInput->GetXboxController( 0 );
	Player* p = (Player*)m_EntityListByType[ENTITY_TYPE_PLAYER][0];
	XINPUT_VIBRATION xboxviration;
	memset( &xboxviration, 0, sizeof( xboxviration ) );

	if( GetTileTypeForPoint( m_EntityListByType[ENTITY_TYPE_PLAYER][0]->GetPosition() ) == TILE_TYPE_MUD )
	{
		p->m_speed=PLAYER_TANK_SPEED*0.5f;
		xboxviration.wLeftMotorSpeed=(WORD)(1000.f);
		xboxviration.wRightMotorSpeed=(WORD)(1000.f);
	}
	else
	{
		p->m_speed=PLAYER_TANK_SPEED;
		xboxviration.wLeftMotorSpeed=(WORD)(0.f);
		xboxviration.wRightMotorSpeed=(WORD)(0.f);
	}

	XInputSetState(controller.GetControllerID(),&xboxviration);
}

TileType Map::GetTileTypeForPoint( Vec2 point )
{
	IntVec2 roundedPoint = IntVec2(RoundDownToInt(point.x),RoundDownToInt(point.y));
	int tileIndex=GetTileIndexForTileCoords(roundedPoint); 
	return m_tiles[tileIndex].m_type;
}



void Map::DetectWin()
{
	if( DoDiscsOverlap( m_EntityListByType[ENTITY_TYPE_PLAYER][0]->GetPosition(), 0.2f, m_exitPosition, 0.01f ) )
	{
		m_game->world->ChangeMap();
	}
}


void Map::MapFillUsingWorms(TileType type)
{
	int numberOfWorms = MAX_WORMS + (currentMapNumber*3);
	for( int currentWormNum = 0; currentWormNum < numberOfWorms ; currentWormNum++ )
	{
		IntVec2 tileCoords;
		tileCoords.x = rng.GetRandomIntInRange(1,tileDimensions.x-2);
		tileCoords.y = rng.GetRandomIntInRange(1,tileDimensions.y-2);
		int wormLength = rng.GetRandomIntInRange( MIN_WORM_LENGTH, MAX_WORM_LENGTH );

		
		for( int wormCurrentLength = 0; wormCurrentLength < wormLength; wormCurrentLength++ )
		{
			SetTileType(tileCoords.x,tileCoords.y,type);
			int nextTimeMovement = rng.GetRandomIntInRange(0,3);

			if( nextTimeMovement ==0 )
			{
				if(tileCoords.x+1 < tileDimensions.x )
				tileCoords.x += 1;
			}
			else if(nextTimeMovement == 1 )
			{
				if(tileCoords.x-1 < tileDimensions.x )
				 tileCoords.x -=1;
			}
			else if(nextTimeMovement ==2 )
			{
				if(tileCoords.y+1 < tileDimensions.y )
				tileCoords.y+= 1;
			}
			else
			{
				if(tileCoords.y-1 < tileDimensions.y )
				tileCoords.y-= 1;
			}
		}
	}
}

bool Map::IsMapValidityUsingFloodFill()
{
		bool hasAnythingChanged = true;
		IntVec2 start_tile = IntVec2(1,1);
		IntVec2 end_tile = IntVec2(tileDimensions.x-2,tileDimensions.y-2);
		
		int startTileIndex = GetTileIndexForTileCoords(start_tile);
		m_tiles[startTileIndex].hasBeenSeen = true;
		int tilesTraversed = 0;

		while( hasAnythingChanged )
		{
			hasAnythingChanged = false;
			for( int currentTileIndex = startTileIndex + 1; currentTileIndex < m_tiles.size(); currentTileIndex++ )
			{
				IntVec2 currentTile = GetTileCoordsForIndex( currentTileIndex );
				IntVec2 northTile = IntVec2( currentTile.x, currentTile.y + 1 );
				IntVec2 southTile = IntVec2( currentTile.x, currentTile.y - 1 );
				IntVec2 eastTile = IntVec2( currentTile.x + 1, currentTile.y );
				IntVec2 westTile = IntVec2( currentTile.x - 1, currentTile.y );
				int northTileIndex = GetTileIndexForTileCoords( northTile );
				int southTileIndex = GetTileIndexForTileCoords( southTile );
				int eastTileIndex = GetTileIndexForTileCoords( eastTile );
				int westTileIndex = GetTileIndexForTileCoords( westTile );
				if( !IsTileSolid( currentTile )  &&
					(m_tiles[northTileIndex].hasBeenSeen || m_tiles[southTileIndex].hasBeenSeen ||
						m_tiles[eastTileIndex].hasBeenSeen || m_tiles[westTileIndex].hasBeenSeen)
					)
				{
					if( !m_tiles[currentTileIndex].hasBeenSeen )
					{
						m_tiles[currentTileIndex].hasBeenSeen = true;
						tilesTraversed++;
						hasAnythingChanged = true;
					}
				}
			}
		}

		if( m_tiles[GetTileIndexForTileCoords( end_tile )].hasBeenSeen )
		{
			return true;
		}
		else
		{
			return false;
		}
}


void Map::CreateExplosions( float explosionTime, float explosionSize, Vec2 position )
{
	Explosion* e = nullptr;
	e = new Explosion(m_game,position);
	e->timeToLive = explosionTime;
	e->explosionSize = explosionSize;

	float orientationDegrees = (float)rng.GetRandomIntInRange(0,360);
	e->m_orientationDegrees=orientationDegrees;

	AddEntityToMap(e,ENTITY_TYPE_EXPLOSION);
}

void Map::CreateAirPlane(Vec2 planePosition,Vec2 airStrikePosition)
{
	AirStrikePlane* a = new AirStrikePlane(m_game,planePosition);
	a->m_airStrikePosition = airStrikePosition;
	AddEntityToMap(a,ENTITY_TYPE_AIR_PLANE);
}

void Map::CreateAirStrike( Vec2 position )
{
	AirStrike* a = new AirStrike(m_game,position);
	AddEntityToMap(a,ENTITY_TYPE_AIR_STRIKE);
	g_theAudio->PlaySound( PLAYER_DIED );
}

