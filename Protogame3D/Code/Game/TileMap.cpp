#include "Game/TileMap.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Portal.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/Game.hpp"
#include "Game/MapRegionType.hpp"
#include "Game/MapMaterial.hpp"
#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexLit.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"

bool Tile::IsSolid()
{
	return m_isSolid;
}

TileMap::TileMap( char const* mapName , IntVec2 dimensions ):Map(mapName)
{
	m_dimenstions = dimensions;
	m_worldMesh = new GPUMesh( g_theRenderer );

	PopulateTiles();
}

TileMap::TileMap( char const* mapName , tinyxml2::XMLElement* mapXmlElement ) :Map( mapName )
{
	std::map<char , std::string> legendMap;

	IntVec2 dimensions = ParseXmlAttribute( *mapXmlElement , "dimensions" , IntVec2( -1 , -1 ) );

	if ( dimensions == IntVec2( -1 , -1 ) )
	{
		ERROR_AND_DIE( "Tilemap dimensions must be specified" );
	}

	m_dimenstions = dimensions;
	m_worldMesh = new GPUMesh( g_theRenderer );
	m_debugMesh = new GPUMesh( g_theRenderer );

	PopulateTiles();

	tinyxml2::XMLElement* legendElement = mapXmlElement->FirstChildElement( "Legend" );
	tinyxml2::XMLElement* mapRowElement = mapXmlElement->FirstChildElement( "MapRows" );

	if ( legendElement != nullptr )
	{
		ParseLegend( legendElement,legendMap );
	}

	if ( mapRowElement != nullptr )
	{
		ParseMapRows( mapRowElement,legendMap );
	}

	tinyxml2::XMLElement* entityElement = mapXmlElement->FirstChildElement( "Entities" );

	tinyxml2::XMLElement* playerStart = entityElement->FirstChildElement( "PlayerStart" );

	if ( playerStart != nullptr )
	{
		Vec2 startpos = ParseXmlAttribute( *playerStart , "pos" , Vec2( -1.f , -1.f ) );
		m_playerStartPos = Vec3( startpos , 0.f );

		float startYaw = ParseXmlAttribute( *playerStart , "yaw" , -1.f );
		m_playerStartYaw = startYaw;
	}

	for ( tinyxml2::XMLElement* ele = entityElement->FirstChildElement( "Actor" ); ele != nullptr; ele = ele->NextSiblingElement( "Actor" ) )
	{
		std::string name= ParseXmlAttribute( *ele,"type","null" );
		Entity* newEntity = SpawnNewEntityOfType( name );

		Vec2 pos = ParseXmlAttribute( *ele , "pos" , Vec2( 1.f , 1.f ) );
		newEntity->m_position = pos;

		m_entities.push_back( newEntity );
	}

	for ( tinyxml2::XMLElement* ele = entityElement->FirstChildElement( "Portal" ); ele!=nullptr; ele = ele->NextSiblingElement("Portal") )
	{
		std::string name = ParseXmlAttribute( *ele , "type" , "null" );
		
		Entity* newEntity = SpawnNewEntityOfType( name );

		Portal* entityCopy = (Portal*)newEntity;

		std::string destMapName = ParseXmlAttribute( *ele , "destMap" , "null" );
		if ( destMapName != "null" )
		{
			entityCopy->m_destMap = destMapName;
		}

		Vec2 destPos = ParseXmlAttribute( *ele , "destPos" , Vec2( 0.f , 0.f ) );
		entityCopy->m_desPos = destPos;

		Vec2 pos = ParseXmlAttribute( *ele , "pos" , Vec2( 0.f , 0.f ) );
		entityCopy->m_position = pos;

		float yawOffset = ParseXmlAttribute( *ele , "destYawOffset" , 0.f );
		entityCopy->m_desYawOffset = yawOffset;

		m_portals.push_back( entityCopy );
	}

}

TileMap::~TileMap()
{
	delete m_worldMesh;
	delete m_debugMesh;
}

void TileMap::PopulateTiles()
{
	for ( int yIndex = 0; yIndex < m_dimenstions.y; yIndex++ )
	{
		for ( int xIndex = 0; xIndex < m_dimenstions.x; xIndex++ )
		{
			Tile newTile = Tile();
			newTile.m_tileCoords = IntVec2( xIndex , yIndex );
			/*int x = GetIndexForTileCoords( newTile.m_tileCoords );
			if ( x == 12 )
			{
				newTile.m_isSolid = true;
			}*/
			m_tiles.push_back( newTile );
		}
	}
}

void TileMap::ParseLegend( tinyxml2::XMLElement* legendElement,  std::map<char , std::string>& legendMap )
{
	
	for ( tinyxml2::XMLElement* ele = legendElement->FirstChildElement( "Tile" ); ele != nullptr; ele = ele->NextSiblingElement( "Tile" ) )
	{
		const char* glyph = ele->Attribute( "glyph" );
		std::string regionName = ParseXmlAttribute( *ele , "regionType" , "NULL" );

		auto found = legendMap.find( *glyph );
		
		if ( found == legendMap.end() )
		{
			legendMap[ *glyph ] = regionName;
		}
	}
}

void TileMap::ParseMapRows( tinyxml2::XMLElement* rowElement,  std::map<char , std::string>& legendMap )
{
	int counter = 0;
	
	for ( tinyxml2::XMLElement* ele = rowElement->LastChildElement( "MapRow" ); ele != nullptr; ele = ele->PreviousSiblingElement( "MapRow" ) )
	{
		std::string rowStr = ParseXmlAttribute( *ele , "tiles" , "NULL" );

		if ( rowStr == "NULL" )
		{
			g_theConsole.PrintError( "Missing tiles attribute" );
		}

		if ( rowStr.size() != m_dimenstions.x )
		{
			g_theConsole.PrintError( "Not enough tiles specified in this row" );
		}


		for ( int i = 0; i < rowStr.size(); i++ )
		{
			char c = rowStr[ i ];
			auto found = legendMap.find( c );
			std::string region="";

			if ( found != legendMap.end() )
			{
				region = found->second;
				const MapRegionType* reg = MapRegionType::GetDefinition( region );

				if ( reg == nullptr )
				{
					g_theConsole.PrintError( "No tile is associated with this glyph , default region will be selected" );
					m_tiles[ counter ].m_region = MapRegionType::s_defaultRegion;
					m_tiles[ counter ].m_isSolid = MapRegionType::s_defaultRegion->IsSolid();
				}
				else
				{
					m_tiles[ counter ].m_region = reg;
					m_tiles[ counter ].m_isSolid = reg->IsSolid();
				}
			}
			else
			{
				g_theConsole.PrintError( "No tile is associated with '" + std::to_string(found->first) + "' , using default map region " );
				m_tiles[ counter ].m_region = MapRegionType::s_defaultRegion;
				m_tiles[ counter ].m_isSolid = MapRegionType::s_defaultRegion->IsSolid();
			}

			counter++;
		}

	}
}

void TileMap::Update( float deltaSeconds )
{
	for ( int i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] == nullptr )
		{
			continue;
		}

		m_entities[ i ]->Update(deltaSeconds);
	}

	for ( int i = 0; i < m_portals.size(); i++ )
	{
		m_portals[ i ]->Update(deltaSeconds);
	}

	ResolveEntityVsEntityCollisions();

	for ( int i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] == nullptr )
		{
			continue;
		}
		ResolveEntityVsTileCollisions( m_entities[ i ] );
	}

	CheckForPortalCollissions();
}

void TileMap::UpdateMeshes()
{
	std::vector<Vertex_PCU> worldVerts;
	
	for ( int yIndex = 0; yIndex < m_dimenstions.y; yIndex++ )
	{
		for ( int xIndex = 0; xIndex < m_dimenstions.x; xIndex++ )
		{
			int tileIndex = ( m_dimenstions.x * yIndex ) + xIndex;
			AddVertsForTile( worldVerts , tileIndex );
		}
	}

	m_worldMesh->UpdateVertices( (int)worldVerts.size() , &worldVerts[ 0 ] );

}

void TileMap::Render()
{
	g_theRenderer->BindTexture( g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" ) );
	g_theRenderer->DrawMesh( m_worldMesh );
	g_theRenderer->BindTexture( nullptr );

	for ( int i = 0; i < m_portals.size(); i++ )
	{
		m_portals[ i ]->Render();
	}

	for ( int i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] == nullptr )
		{
			continue;
		}
		m_entities[ i ]->Render();
	}


}

void TileMap::AddVertsForTile( std::vector<Vertex_PCU>& destination , int tileIndex )
{
	IntVec2 tileCoords = GetTileCoordsForIndex( tileIndex );

	Tile* tile = GetTileAtCoords( tileCoords );

	if ( tile->IsSolid() )
	{
		AddVertsForSolidTile( destination , tileIndex );
	}
	else
	{
		AddVertsForNonSolidTile( destination , tileIndex );
	}
}

void TileMap::AddVertsForSolidTile( std::vector<Vertex_PCU>& destination , int tileIndex )
{
	AABB3 box = GetTilebounds( tileIndex );

	//std::vector<Vertex_PCU> tiles;

	Rgba8 tint = Rgba8( 255 , 255 , 255 , 255 );

	IntVec2 currentTileCoords = GetTileCoordsForIndex( tileIndex );
	IntVec2 northTile( -1 , 0 );
	IntVec2 southTile( 1 , 0 );
	IntVec2 eastTile( 0 , -1 );
	IntVec2 westTile( 0 , 1 );

	Tile* tile = GetTileAtCoords( currentTileCoords );
	const MapRegionType* region = tile->m_region;

	Vec2 minUvs;
	Vec2 maxUvs;

	region->GetSideMaterial()->GetUVCoords( minUvs , maxUvs );

	Vec2 minXUVMaxYUV( minUvs.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUvs.y );


	if ( !isTileSolid( IntVec2(currentTileCoords.x+northTile.x,currentTileCoords.y+northTile.y ) ) )
	{
		Vertex_PCU backFaceVerts[ 6 ] = {
			// BACK FACE VERTS
			Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, minUvs ),
			Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),
			Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, maxUvs ),
			
			Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, maxUvs ),
			Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
			Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, minUvs ),
		};

		for ( int i = 0; i < 6; i++ )
		{
			destination.push_back( backFaceVerts[ i ] );
		}
	}

	if ( !isTileSolid( IntVec2( currentTileCoords.x + southTile.x , currentTileCoords.y + southTile.y ) ) )
	{
		Vertex_PCU frontFaceVerts[ 6 ] = {
			// FRONT FACE VERTS
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, minUvs ),
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, maxXUVMinYUV ),
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),

			Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, minUvs ),
		};

		for ( int i = 0; i < 6; i++ )
		{
			destination.push_back( frontFaceVerts[ i ] );
		}
	}

	if ( !isTileSolid( IntVec2( currentTileCoords.x + eastTile.x , currentTileCoords.y + eastTile.y ) ) )
	{
		Vertex_PCU eastFaceVerts[ 6 ] = {
			// RIGHT FACE VERTS
			Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, minUvs ),
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, maxUvs ),

			Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, maxUvs ),
			Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
			Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, minUvs ),
		};

		for ( int i = 0; i < 6; i++ )
		{
			destination.push_back( eastFaceVerts[ i ] );
		}
	}

	if ( !isTileSolid( IntVec2( currentTileCoords.x + westTile.x , currentTileCoords.y + westTile.y ) ) )
	{
		Vertex_PCU westFaceVerts[ 6 ] = {
			//LEFT FACE VERTS
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, minUvs ),
			Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, maxXUVMinYUV ),
			Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
			
			Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
			Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, minUvs ),
		};

		for ( int i = 0; i < 6; i++ )
		{
			destination.push_back( westFaceVerts[ i ] );
		}
	}

}

void TileMap::AddVertsForNonSolidTile( std::vector<Vertex_PCU>& destination , int tileIndex )
{
	AABB3 box = GetTilebounds( tileIndex );

	//std::vector<Vertex_PCU> tiles;
	Rgba8 tint = Rgba8( 255 , 255 , 255 , 255 );

	IntVec2 currentTileCoords = GetTileCoordsForIndex( tileIndex );
	Tile* tile = GetTileAtCoords( currentTileCoords );
	const MapRegionType* region = tile->m_region;

	Vec2 minUvs;
	Vec2 maxUvs;

	region->GetCelingMaterial()->GetUVCoords( minUvs , maxUvs );

	Vec2 minXUVMaxYUV( minUvs.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUvs.y );

	Vertex_PCU topfaceVerts[ 6 ] = 
	{
		// TOP FACE VERTS
		Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, minUvs ),
		Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, maxXUVMinYUV ),
		Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint , maxUvs ),
		
		Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, minUvs ),
		Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint , maxUvs ),
		Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	region->GetFloorMaterial()->GetUVCoords( minUvs , maxUvs );

	minXUVMaxYUV = Vec2( minUvs.x , maxUvs.y );
	maxXUVMinYUV = Vec2( maxUvs.x , minUvs.y );

	Vertex_PCU bottomfaceVerts[ 6 ] = {
		// BOTTOM FACE VERTS
		Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, maxUvs ),
		Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, minUvs ),
		Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

		Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, minXUVMaxYUV ),
		Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, minUvs ),
		Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, maxUvs ),
	};
	
	for ( int i = 0; i < 6; i++ )
	{
		destination.push_back( topfaceVerts[ i ] );
	}

	for ( int i = 0; i < 6; i++ )
	{
		destination.push_back( bottomfaceVerts[ i ] );
	}

}

AABB3 TileMap::GetTilebounds( int tileIndex )
{
	AABB3 box = AABB3( Vec3(-0.5f,-0.5f,-0.5f) , Vec3(0.5f,0.5f,0.5f) );
	IntVec2 tileCoords = GetTileCoordsForIndex( tileIndex );
	Vec2 offset=Vec2(0.5f,0.5f);
	Vec3 translation = Vec3( Vec2(tileCoords) + offset , 0.5f );
	box.Translate( translation );

	return box;
}

IntVec2 TileMap::GetTileCoordsForIndex( int index )
{
	return m_tiles[ index ].m_tileCoords;
}

int TileMap::GetIndexForTileCoords( IntVec2 tileCoords )
{
	int tileIndex = (m_dimenstions.x * tileCoords.y) + tileCoords.x;
	return tileIndex;
}

Tile* TileMap::GetTileAtCoords( IntVec2 tileCoords )
{
	int index = GetIndexForTileCoords( tileCoords );
	Tile* tile = nullptr;

	if ( index >= 0 && index < m_tiles.size() )
	{
		tile = &m_tiles[ index ];
	}

	return tile;
}

bool TileMap::isTileSolid( IntVec2 tileCoords )
{
	Tile* tile = GetTileAtCoords( tileCoords );
	if ( tile == nullptr )
	{
		return true;
	}

	return tile->IsSolid();
}

void TileMap::ResolveEntityVsTileCollisions( Entity* entity  )
{
	IntVec2 current_tileCoords = IntVec2( RoundDownToInt( entity->m_position.x ) , RoundDownToInt( entity->m_position.y ) );

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

	if ( entity->m_canBePushedByWalls )
	{
		if ( isTileSolid( leftCoords ) )
		{
			PushDiscOutOFAABB2D( entity->m_position , entity->m_physicsRadius , leftAABB );
		}
		if ( isTileSolid( rightCoords ) )
		{
			PushDiscOutOFAABB2D( entity->m_position , entity->m_physicsRadius , rightAABB );
		}
		if ( isTileSolid( upCoords ) )
		{
			PushDiscOutOFAABB2D( entity->m_position , entity->m_physicsRadius , upAABB );
		}
		if ( isTileSolid( downCoords ) )
		{
			PushDiscOutOFAABB2D( entity->m_position , entity->m_physicsRadius , downAABB );
		}
		if ( isTileSolid( topRightCoords ) )
		{
			PushDiscOutOFAABB2D( entity->m_position , entity->m_physicsRadius , topRightAABB );
		}
		if ( isTileSolid( topLeftCoords ) )
		{
			PushDiscOutOFAABB2D( entity->m_position , entity->m_physicsRadius , topLeftAABB );
		}
		if ( isTileSolid( bottomRightCoords ) )
		{
			PushDiscOutOFAABB2D( entity->m_position , entity->m_physicsRadius , bottomRightAABB );
		}
		if ( isTileSolid( bottomLeftCoords ) )
		{
			PushDiscOutOFAABB2D( entity->m_position , entity->m_physicsRadius , bottomLeftAABB );
		}
	}


}

void TileMap::CheckForPortalCollissions()
{

	for ( int i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] == nullptr )
		{
			continue;
		}
	
		for ( int j = 0; j < m_portals.size(); j++ )
		{
			if ( DoDiscsOverlap( m_entities[ i ]->m_position , m_entities[ i ]->m_physicsRadius , m_portals[ j ]->m_position , m_portals[ j ]->m_physicsRadius ) )
			{
				if ( m_entities[ i ] == nullptr )
				{
					continue;
				}
				Map* maptToTeleport = thegame->m_world->GetMap( m_portals[ j ]->m_destMap );
				maptToTeleport->m_entities.push_back( m_entities[ i ] );

				m_entities[ i ]->m_position = m_portals[ j ]->m_desPos;
				m_entities[ i ] = nullptr;
				return;
			}
		}
	}

	for ( int i = 0; i < m_portals.size(); i++ )
	{
		if ( DoDiscsOverlap( Vec2( thegame->m_camera->m_transform.m_position.x , thegame->m_camera->m_transform.m_position.y ) , 0.3f , m_portals[ i ]->m_position , m_portals[ i ]->m_physicsRadius ) )
		{
			//Map* maptToTeleport = thegame->m_world->GetMap( m_portals[ i ]->m_destMap );
			thegame->m_yaw += m_portals[ i ]->m_desYawOffset;
			thegame->m_world->LoadMap( m_portals[ i ]->m_destMap );
			break;
		}
	}
}

RayCastResult TileMap::RayCastToEntities( Vec2 start , Vec2 direction , float dist )
{
	std::vector<RayCastResult> results;
	RayCastResult toReturn;

	for ( int i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] != nullptr )
		{

			if ( thegame->m_player == m_entities[ i ] )
			{
				continue;
			}

			Vec2 neartstPoint = GetNearestPointOnLineSegment2D( m_entities[ i ]->m_position , start , start + ( direction * dist ) );

			float distance = (neartstPoint - m_entities[ i ]->m_position).GetLength();

			if ( distance > m_entities[ i ]->m_physicsRadius )
			{
				continue;
			}

			float distanceAlongRay = sqrtf( ( m_entities[ i ]->m_physicsRadius * m_entities[ i ]->m_physicsRadius ) - ( distance * distance ) );

			Vec2 startPoint = m_entities[ i ]->m_position + ( neartstPoint - m_entities[ i ]->m_position ).GetNormalized()*distance;

			RayCastResult r;
			r.didImpact = true;
			r.startHitPoint = startPoint - ( direction * distanceAlongRay );
			r.endHitPoint = startPoint + ( direction * distanceAlongRay );
			r.impactNormal = ( r.startHitPoint - m_entities[ i ]->m_position ).GetRotated90Degrees().GetNormalized();
			results.push_back( r );
		}
	}

	if ( results.size() > 0 )
	{
		if ( results.size() > 1 )
		{
			float minDistanceSq = INFINITY;
			size_t resultIndex = 0;

			for ( size_t index = 0; index < results.size(); index++ )
			{
				float distanceSq = ( start - results[ index ].startHitPoint ).GetLengthSquared();
				if ( distanceSq < minDistanceSq )
				{
					resultIndex = index;
					minDistanceSq = distanceSq;
				}
			}
			return results[ resultIndex ];
		}
		else
		{
			return results[ 0 ];
		}
	}

	return toReturn;
}

RayCastResult TileMap::RayCastToTiles( Vec2 start , Vec2 direction , float distace )
{
	RayCastResult toReturn;

	IntVec2 currentTileCoords = IntVec2( ( int ) ( floor( start.x ) ) , ( int ) ( floor( start.y ) ) );

	if ( isTileSolid( currentTileCoords ) )
	{
		toReturn.didImpact = true;
		toReturn.startHitPoint = start;
		toReturn.endHitPoint = start;
		toReturn.impactNormal = -direction;

		return toReturn;
	}

	Vec2 displacement = direction * distace;

	float xDeltaT = 1 / abs( displacement.x );
	int tileStepX = direction.x > 0.f ? 1 : -1;
	int offsetToLeadingEdgeX = ( tileStepX + 1 ) / 2;
	float firstVerticalIntersectionX = ( float ) ( currentTileCoords.x + offsetToLeadingEdgeX );
	float tOfNextXCrossing = abs( firstVerticalIntersectionX - start.x ) * xDeltaT;

	float yDeltaT = 1 / abs( displacement.y );
	int tileStepY = direction.y > 0.f ? 1 : -1;
	int offsetToLeadingEdgeY = ( tileStepY + 1 ) / 2;
	float firstVerticalIntersectionY = ( float ) ( currentTileCoords.y + offsetToLeadingEdgeY );
	float tOfNextYCrossing = abs( firstVerticalIntersectionY - start.y ) * yDeltaT;

	int tileX = currentTileCoords.x;
	int tileY = currentTileCoords.y;

	while(true)
	{
		if ( tOfNextXCrossing < tOfNextYCrossing )
		{
			if ( tOfNextXCrossing > 1.f )
			{
				toReturn.didImpact = false;
				toReturn.startHitPoint = start;
				toReturn.endHitPoint = start + direction * distace;
				return toReturn;
			}

			tileX += tileStepX;

			if ( isTileSolid( IntVec2( tileX , tileY ) ) )
			{
				toReturn.didImpact = true;
				toReturn.startHitPoint = start + ( displacement * tOfNextXCrossing );
				toReturn.endHitPoint = toReturn.startHitPoint;
				toReturn.impactNormal = Vec2( (float)-tileStepX , 0.f );
				return toReturn;
			}
			
			tOfNextXCrossing += xDeltaT;
		}
		else
		{
			if ( tOfNextYCrossing > 1.f )
			{
				toReturn.didImpact = false;
				toReturn.startHitPoint = start;
				toReturn.endHitPoint = start + direction * distace;
				return toReturn;
			}
			
			tileY += tileStepY;

			if ( isTileSolid( IntVec2( tileX , tileY ) ) )
			{
				toReturn.didImpact = true;
				toReturn.startHitPoint = start + ( displacement * tOfNextYCrossing );
				toReturn.endHitPoint = toReturn.startHitPoint;
				toReturn.impactNormal = Vec2(0.f, ( float ) -tileStepY );
				return toReturn;
			}

			tOfNextYCrossing += yDeltaT;
		}	
	} 

	return toReturn;
}

void TileMap::DebugRenderRaycasts( Vec2 start , Vec2 direction , float distance )
{
	RayCastResult r = RayCastToEntities( start , direction , distance );

	Vec2 end = start + direction * distance;

	std::vector<Vertex_PCU> lineVerts;
	AppendCyinder( lineVerts , Vec3( start , 0.5f ) , Vec3( end , 0.5f ) , 0.01f , 0.01f , Rgba8( 0 , 0 , 100 , 127 ) , Rgba8( 0 , 0 , 100 , 127 ) );

	g_theRenderer->DrawVertexArray( lineVerts );

	if ( r.didImpact )
	{
		std::vector<Vertex_PCU> spherVerts;
		AppendCyinder( spherVerts , Vec3( r.startHitPoint , 0.5f ) , Vec3( r.endHitPoint , 0.5f ) , 0.01f , 0.01f , Rgba8( 0 , 255 , 0 , 255 ) , Rgba8( 0 , 255 , 0 , 255 ) );
		g_theRenderer->DrawVertexArray( spherVerts );
	}

	RayCastResult x = RayCastToTiles( start , direction , distance );

	std::vector<Vertex_PCU> lineVerts2;
	AppendCyinder( lineVerts2 , Vec3( start , 0.7f ) , Vec3( end , 0.7f ) , 0.01f , 0.01f , Rgba8( 0 , 0 , 100 , 127 ) , Rgba8( 0 , 0 , 100 , 127 ) );

	g_theRenderer->DrawVertexArray( lineVerts2 );

	if ( x.didImpact )
	{
		std::vector<Vertex_PCU> spherVerts;
		std::vector<unsigned int> sphereIndices;
		//AppendCyinder( spherVerts , Vec3( x.startHitPoint , 0.7f ) , Vec3( x.endHitPoint+(direction*1.5f) , 0.7f ) , 0.01f , 0.01f , Rgba8( 0 , 255 , 0 , 255 ) , Rgba8( 0 , 255 , 0 , 255 ) );
		AppendUVSPhere( 16 , 32 , spherVerts ,sphereIndices, 0.04f , Vec3( x.startHitPoint , 0.7f ) , Rgba8( 255 , 0 , 0 , 255 ) );
		
		m_debugMesh->UpdateVertices((unsigned int) spherVerts.size() , &spherVerts[ 0 ] );
		m_debugMesh->UpdateIndices( (unsigned int) sphereIndices.size() , &sphereIndices[ 0 ] );

		g_theRenderer->CreateRasterState( SOLID , CULL_NONE );
		g_theRenderer->DrawMesh( m_debugMesh );
		g_theRenderer->CreateRasterState( SOLID , CULL_BACK );

	}

}
