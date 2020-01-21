#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Actor.hpp"
#include "Game/NpcUnit.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"



Map::Map(Game* game, std::string mapName, MapDefinition* mapDefinition )
{
	m_game = game;
	m_name = mapName;
	m_definition = mapDefinition;
	m_width = mapDefinition->width;
	m_height = mapDefinition->height;
	m_tiletexture = g_theRenderer->GetOrCreateTextureFromFile("Data/Images/Terrain_32x32.png");
	PopulateTiles();

	m_player = new Actor( m_game , Vec2( 2.5f , 2.5f ) , ActorDefinition::s_definitions[ "Player" ] );
	m_NpcUnits.push_back( new NpcUnit( m_game , Vec2( 3.5f , 3.5f ) , ActorDefinition::s_definitions[ "Neeraj" ] ) );
	m_NpcUnits.push_back( new NpcUnit( m_game , Vec2( 4.f , 4.f ) , ActorDefinition::s_definitions[ "Pooja" ] ) );
}

void Map::Update( float deltaSeconds )
{
	m_player->Update( deltaSeconds );
	HandleBlockCollissions();

	for ( int npcIndex = 0; npcIndex < m_NpcUnits.size(); npcIndex++ )
	{
		m_NpcUnits[ npcIndex ]->Update( deltaSeconds );
	}
}

void Map::Render()
{
	g_theRenderer->BindTexture( m_tiletexture );
	g_theRenderer->DrawVertexArray( m_tileVertices );
	g_theRenderer->BindTexture( nullptr );

	m_player->Render();

	for ( int npcIndex = 0; npcIndex < m_NpcUnits.size(); npcIndex++ )
	{
		m_NpcUnits[ npcIndex ]->Render();
	}
}

void Map::PopulateTiles()
{
	RandomNumberGenerator rng = RandomNumberGenerator();

	int numTiles = m_width * m_height;
	for ( int index = 0; index < numTiles; index++ )
	{
		int tileX = index % m_width;
		int tileY = index / m_height;

		m_tiles.push_back( Tile( tileX , tileY , TileDefinition::s_definitions[ m_definition->fillTileType ] ) );
	}

	for ( int mapGenStep = 0; mapGenStep < m_definition->generationStep.size(); mapGenStep++ )
	{
		m_definition->generationStep[ mapGenStep ]->RunStep( *this );
	}

	for ( int tileX = 0; tileX < m_width; tileX++ )
	{
		SetTileType( tileX , 0 , TileDefinition::s_definitions[ m_definition->edgeTileType ] );
		SetTileType( tileX , m_height - 1 , TileDefinition::s_definitions[ m_definition->edgeTileType ] );
	}

	for ( int tileY = 0; tileY < m_height; tileY++ )
	{
		SetTileType( 0 , tileY , TileDefinition::s_definitions[ m_definition->edgeTileType ] );
		SetTileType( m_width - 1 , tileY , TileDefinition::s_definitions[ m_definition->edgeTileType ] );
	}



	for ( int verticalIndex = 0; verticalIndex < m_height; verticalIndex++ )
	{
		for ( int horizontalIndex = 0; horizontalIndex < m_width; horizontalIndex++ )
		{
			int index = verticalIndex * m_width + horizontalIndex;
			AABB2 aabb = AABB2( ( float ) horizontalIndex , ( float ) verticalIndex , ( float ) horizontalIndex + 1 , ( float ) verticalIndex + 1 );

			//m_tiles[index].m_definition->m_spriteUVs.mins;
			AppendAABB2( m_tileVertices , aabb , Rgba8( 255 , 255 , 255 , 255 ) , m_tiles[ index ].m_definition->m_spriteUVs.mins , m_tiles[ index ].m_definition->m_spriteUVs.maxs );
		}
	}

}

void Map::SetTileType( int tileX , int tileY , TileDefinition* newDef )
{
	int tileIndex = GetTileIndexForTileCoords( IntVec2( tileX , tileY ) );
	m_tiles[ tileIndex ].m_definition = newDef;
}

Tile* Map::GetTileAtCoords( IntVec2 tileCoords )
{
	int tileIndex = GetTileIndexForTileCoords( tileCoords );
	return &m_tiles[ tileIndex ];
}

IntVec2 Map::GetTileInfoForMousePosition( Vec2 mousePositionInCameraView )
{
	return IntVec2( RoundDownToInt( mousePositionInCameraView.x ) , RoundDownToInt( mousePositionInCameraView.y ) );
}

std::string Map::GetTileInfoForTileCoords( IntVec2 tileCoords )
{
	std::string tileInfo = "Tiletype: ";
	if ( tileCoords.x <0 || tileCoords.y<0 || tileCoords.x>m_width - 1 || tileCoords.y>m_height - 1 )
	{
		return "Not In Map Area";
	}
	else
	{
		int tileIndex = GetTileIndexForTileCoords( tileCoords );
		tileInfo += m_tiles[ tileIndex ].m_definition->tileName;
		return tileInfo;
	}
}

int Map::GetTileIndexForTileCoords( const IntVec2& tileCoords ) const
{
	if( tileCoords.x<0 || tileCoords.x>=20 || tileCoords.y<0 || tileCoords.y>20 )
	{
		return 0;
	}
	return tileCoords.x+(m_width*tileCoords.y);
}


void Map::HandleBlockCollissions()
{
	{
		Vec2 player_position = m_player->GetPosition();

		IntVec2 current_tileCoords = IntVec2( RoundDownToInt( player_position.x ) , RoundDownToInt( player_position.y ) );

		IntVec2 leftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y );
		IntVec2 rightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y );
		IntVec2 upCoords = IntVec2( current_tileCoords.x , current_tileCoords.y + 1 );
		IntVec2 downCoords = IntVec2( current_tileCoords.x , current_tileCoords.y - 1 );

		IntVec2 topLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y + 1 );
		IntVec2 bottomLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y - 1 );
		IntVec2 topRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y + 1 );
		IntVec2 bottomRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y - 1 );

		int leftIndex = GetTileIndexForTileCoords( leftCoords );
		int rightIndex = GetTileIndexForTileCoords( rightCoords );
		int upIndex = GetTileIndexForTileCoords( upCoords );
		int downIndex = GetTileIndexForTileCoords( downCoords );
		int topLeftIndex = GetTileIndexForTileCoords( topLeftCoords );
		int topRightIndex = GetTileIndexForTileCoords( topRightCoords );
		int bottomLeftIndex = GetTileIndexForTileCoords( bottomLeftCoords );
		int bottomRightIndex = GetTileIndexForTileCoords( bottomRightCoords );

		AABB2 leftAABB = AABB2( ( float ) leftCoords.x , ( float ) leftCoords.y , ( float ) leftCoords.x + 1 , ( float ) leftCoords.y + 1 );
		AABB2 rightAABB = AABB2( ( float ) rightCoords.x , ( float ) rightCoords.y , ( float ) rightCoords.x + 1 , ( float ) rightCoords.y + 1 );
		AABB2 upAABB = AABB2( ( float ) upCoords.x , ( float ) upCoords.y , ( float ) upCoords.x + 1 , ( float ) upCoords.y + 1 );
		AABB2 downAABB = AABB2( ( float ) downCoords.x , ( float ) downCoords.y , ( float ) downCoords.x + 1 , ( float ) downCoords.y + 1 );

		AABB2 topLeftAABB = AABB2( ( float ) topLeftCoords.x , ( float ) topLeftCoords.y , ( float ) topLeftCoords.x + 1 , ( float ) topLeftCoords.y + 1 );
		AABB2 bottomRightAABB = AABB2( ( float ) bottomRightCoords.x , ( float ) bottomRightCoords.y , ( float ) bottomRightCoords.x + 1 , ( float ) bottomRightCoords.y + 1 );
		AABB2 topRightAABB = AABB2( ( float ) topRightCoords.x , ( float ) topRightCoords.y , ( float ) topRightCoords.x + 1 , ( float ) topRightCoords.y + 1 );
		AABB2 bottomLeftAABB = AABB2( ( float ) bottomLeftCoords.x , ( float ) bottomLeftCoords.y , ( float ) bottomLeftCoords.x + 1 , ( float ) bottomLeftCoords.y + 1 );


		if ( !CanEntityMoveOnTile(m_player,leftIndex) )
		{
			PushDiscOutOFAABB2D( player_position , .2f , leftAABB );
		}
		if ( !CanEntityMoveOnTile( m_player , rightIndex ) )
		{
			PushDiscOutOFAABB2D( player_position , .2f , rightAABB );
		}
		if ( !CanEntityMoveOnTile( m_player , upIndex ) )
		{
			PushDiscOutOFAABB2D( player_position , .2f , upAABB );
		}
		if ( !CanEntityMoveOnTile( m_player , downIndex ) )
		{
			PushDiscOutOFAABB2D( player_position , .2f , downAABB );
		}
		if ( !CanEntityMoveOnTile( m_player , topRightIndex ) )
		{
			PushDiscOutOFAABB2D( player_position , .2f , topRightAABB );
		}
		if ( !CanEntityMoveOnTile( m_player , topLeftIndex ) )
		{
			PushDiscOutOFAABB2D( player_position , .2f , topLeftAABB );
		}
		if ( !CanEntityMoveOnTile( m_player , bottomRightIndex ) )
		{
			PushDiscOutOFAABB2D( player_position , .2f , bottomRightAABB );
		}
		if ( !CanEntityMoveOnTile( m_player , bottomLeftIndex ) )
		{
			PushDiscOutOFAABB2D( player_position , .2f , bottomLeftAABB );
		}

		m_player->SetPosition( player_position );
	}

		for ( int npcIndex = 0; npcIndex < m_NpcUnits.size(); npcIndex++ )
		{
			if ( m_NpcUnits[npcIndex]->m_canWalk )
			{
				Vec2 npc_position = m_NpcUnits[ npcIndex ]->GetPosition();

				IntVec2 current_tileCoords = IntVec2( RoundDownToInt( npc_position.x ) , RoundDownToInt( npc_position.y ) );

				IntVec2 leftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y );
				IntVec2 rightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y );
				IntVec2 upCoords = IntVec2( current_tileCoords.x , current_tileCoords.y + 1 );
				IntVec2 downCoords = IntVec2( current_tileCoords.x , current_tileCoords.y - 1 );

				IntVec2 topLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y + 1 );
				IntVec2 bottomLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y - 1 );
				IntVec2 topRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y + 1 );
				IntVec2 bottomRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y - 1 );

				int leftIndex = GetTileIndexForTileCoords( leftCoords );
				int rightIndex = GetTileIndexForTileCoords( rightCoords );
				int upIndex = GetTileIndexForTileCoords( upCoords );
				int downIndex = GetTileIndexForTileCoords( downCoords );
				int topLeftIndex = GetTileIndexForTileCoords( topLeftCoords );
				int topRightIndex = GetTileIndexForTileCoords( topRightCoords );
				int bottomLeftIndex = GetTileIndexForTileCoords( bottomLeftCoords );
				int bottomRightIndex = GetTileIndexForTileCoords( bottomRightCoords );

				AABB2 leftAABB = AABB2( ( float ) leftCoords.x , ( float ) leftCoords.y , ( float ) leftCoords.x + 1 , ( float ) leftCoords.y + 1 );
				AABB2 rightAABB = AABB2( ( float ) rightCoords.x , ( float ) rightCoords.y , ( float ) rightCoords.x + 1 , ( float ) rightCoords.y + 1 );
				AABB2 upAABB = AABB2( ( float ) upCoords.x , ( float ) upCoords.y , ( float ) upCoords.x + 1 , ( float ) upCoords.y + 1 );
				AABB2 downAABB = AABB2( ( float ) downCoords.x , ( float ) downCoords.y , ( float ) downCoords.x + 1 , ( float ) downCoords.y + 1 );

				AABB2 topLeftAABB = AABB2( ( float ) topLeftCoords.x , ( float ) topLeftCoords.y , ( float ) topLeftCoords.x + 1 , ( float ) topLeftCoords.y + 1 );
				AABB2 bottomRightAABB = AABB2( ( float ) bottomRightCoords.x , ( float ) bottomRightCoords.y , ( float ) bottomRightCoords.x + 1 , ( float ) bottomRightCoords.y + 1 );
				AABB2 topRightAABB = AABB2( ( float ) topRightCoords.x , ( float ) topRightCoords.y , ( float ) topRightCoords.x + 1 , ( float ) topRightCoords.y + 1 );
				AABB2 bottomLeftAABB = AABB2( ( float ) bottomLeftCoords.x , ( float ) bottomLeftCoords.y , ( float ) bottomLeftCoords.x + 1 , ( float ) bottomLeftCoords.y + 1 );


				if ( !m_tiles[ leftIndex ].m_definition->allowWalking )
				{
					PushDiscOutOFAABB2D( npc_position , .2f , leftAABB );
				}
				if ( !m_tiles[ rightIndex ].m_definition->allowWalking )
				{
					PushDiscOutOFAABB2D( npc_position , .2f , rightAABB );
				}
				if ( !m_tiles[ upIndex ].m_definition->allowWalking )
				{
					PushDiscOutOFAABB2D( npc_position , .2f , upAABB );
				}
				if ( !m_tiles[ downIndex ].m_definition->allowWalking )
				{
					PushDiscOutOFAABB2D( npc_position , .2f , downAABB );
				}
				if ( !m_tiles[ topRightIndex ].m_definition->allowWalking )
				{
					PushDiscOutOFAABB2D( npc_position , .2f , topRightAABB );
				}
				if ( !m_tiles[ topLeftIndex ].m_definition->allowWalking )
				{
					PushDiscOutOFAABB2D( npc_position , .2f , topLeftAABB );
				}
				if ( !m_tiles[ bottomRightIndex ].m_definition->allowWalking )
				{
					PushDiscOutOFAABB2D( npc_position , .2f , bottomRightAABB );
				}
				if ( !m_tiles[ bottomLeftIndex ].m_definition->allowWalking )
				{
					PushDiscOutOFAABB2D( npc_position , .2f , bottomLeftAABB );
				}

				m_NpcUnits[ npcIndex ]->SetPosition( npc_position );
			}

		}
	
}

bool Map::CanEntityMoveOnTile( Entity* entity , int tileIndex )
{
	bool canMove = false;
	if ( m_tiles[ tileIndex ].m_definition->allowFlying && entity->m_canFly )
	{
		canMove = true;
	}
	if ( m_tiles[ tileIndex ].m_definition->allowSwimming && entity->m_canSwim )
	{
		canMove = true;
	}
	if ( m_tiles[ tileIndex ].m_definition->allowWalking && entity->m_canWalk )
	{
		canMove = true;
	}

	return canMove;
}

bool Map::IsMousePointingActor( Vec2 mousePositionInCameraView )
{
	Vec2 playerPos=m_player->GetPosition();
	if ( RoundDownToInt( mousePositionInCameraView.x ) == RoundDownToInt( playerPos.x ) && RoundDownToInt( mousePositionInCameraView.y ) == RoundDownToInt( playerPos.y ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Map::IsMousePointingNpcUnits( Vec2 mousePositionInCameraView )
{
	for ( int npcIndex = 0; npcIndex < m_NpcUnits.size(); npcIndex++ )
	{
		Vec2 npcPosition = m_NpcUnits[ npcIndex ]->GetPosition();
		if ( RoundDownToInt( mousePositionInCameraView.x ) == RoundDownToInt( npcPosition.x ) && RoundDownToInt( mousePositionInCameraView.y ) == RoundDownToInt( npcPosition.y ) )
		{
			return true;
		}
	}

	return false;
}

Strings Map::GetPlayerDetails()
{
	return m_player->GetActorDetails();
}

Strings Map::GetNpcDetails( Vec2 mousePositionInCameraView )
{
	for ( int npcIndex = 0; npcIndex < m_NpcUnits.size(); npcIndex++ )
	{
		Vec2 npcPosition = m_NpcUnits[ npcIndex ]->GetPosition();
		if ( RoundDownToInt( mousePositionInCameraView.x ) == RoundDownToInt( npcPosition.x ) && RoundDownToInt( mousePositionInCameraView.y ) == RoundDownToInt( npcPosition.y ) )
		{
			return m_NpcUnits[ npcIndex ]->GetActorDetails();
		}
	}
	
	ERROR_AND_DIE( "Mouse not pointing to any Npc, call this function only when mouse points Npc" );

}

Tile* Map::GetNorthTile( IntVec2 currentTileCoords )
{
	currentTileCoords.y += 1;
	int index = GetTileIndexForTileCoords( currentTileCoords );
	return &(m_tiles[ index ]);
}

Tile* Map::GetSouthTile( IntVec2 currentTileCoords )
{
	currentTileCoords.y -= 1;
	int index= GetTileIndexForTileCoords( currentTileCoords );
	return &( m_tiles[ index ] );
}

Tile* Map::GetEastTile( IntVec2 currentTileCoords )
{
	currentTileCoords.x += 1;
	int index = GetTileIndexForTileCoords( currentTileCoords );
	return &( m_tiles[ index ] );
}

Tile* Map::GetWestTile( IntVec2 currentTileCoords )
{
	currentTileCoords.x -= 1;
	int index = GetTileIndexForTileCoords( currentTileCoords );
	return &( m_tiles[ index ] );
}


Tile* Map::GetNorthEastTile( IntVec2 currentTileCoords )
{
	currentTileCoords.x += 1;
	currentTileCoords.y += 1;
	int index = GetTileIndexForTileCoords( currentTileCoords );
	return &( m_tiles[ index ] );
}

Tile* Map::GetNorthWestTile( IntVec2 currentTileCoords )
{
	currentTileCoords.x += 1;
	currentTileCoords.y -= 1;
	int index = GetTileIndexForTileCoords( currentTileCoords );
	return &( m_tiles[ index ] );
}

Tile* Map::GetSouthEastTile( IntVec2 currentTileCoords )
{
	currentTileCoords.x -= 1;
	currentTileCoords.y += 1;
	int index = GetTileIndexForTileCoords( currentTileCoords );
	return &( m_tiles[ index ] );
}

Tile* Map::GetSouthWestTile( IntVec2 currentTileCoords )
{
	currentTileCoords.x -= 1;
	currentTileCoords.y -= 1;
	int index = GetTileIndexForTileCoords( currentTileCoords );
	return &( m_tiles[ index ] );
}

