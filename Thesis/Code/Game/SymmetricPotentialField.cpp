#include "Game/SymmetricPotentialField.hpp"
#include "Game/Game.hpp"
#include "Game/MainGameMapCreator.hpp"
#include "Engine/Math/MathUtils.hpp"

SymmetricPotentialField::SymmetricPotentialField( Game* game , IntVec2 startPos , IntVec2 dimensions , float startValue , float fallOff , bool isPositive )
{
	m_game = game;
	m_startPosition = startPos;
	m_dimesions = dimensions;
	m_startValue = startValue;
	m_fallOffPerTile = fallOff;
	m_isPositive = isPositive;
}

SymmetricPotentialField::~SymmetricPotentialField()
{

}

void SymmetricPotentialField::Create()
{
	IntVec2 minCoords = IntVec2( m_startPosition.x - m_dimesions.x / 2 , m_startPosition.y - m_dimesions.y / 2 );
	IntVec2 maxCoords = IntVec2( m_startPosition.x + m_dimesions.x / 2 , m_startPosition.y + m_dimesions.y / 2 );

	for ( int i = minCoords.x; i <= maxCoords.x; i++ )
	{
		for ( int j = minCoords.y; j <= maxCoords.y; j++ )
		{
			SymmetricPotentialFieldNode node;
			node.coords = IntVec2( i , j );

			float manHattanDistance = m_game->GetManhattanDistance( node.coords , m_startPosition );
			node.value = m_startValue - (manHattanDistance * m_fallOffPerTile);
			
			if ( !m_isPositive )
			{
				node.direction = Vec2( node.coords - m_startPosition ).GetNormalized();
				if ( ( node.direction - Vec2::MakeFromPolarDegrees( 90.f ) ).GetLength() <= 0.1f )
				{
					node.direction = Vec2::MakeFromPolarDegrees( 70.f );
				}
				else if ( ( node.direction - Vec2::MakeFromPolarDegrees( 180.f ) ).GetLength() <= 0.1f )
				{
					node.direction = Vec2::MakeFromPolarDegrees( 200.f );
				}
				else if ( ( node.direction - Vec2::MakeFromPolarDegrees( 0.f ) ).GetLength() <= 0.1f )
				{
					node.direction = Vec2::MakeFromPolarDegrees( 20.f );
				}
				else if ( ( node.direction - Vec2::MakeFromPolarDegrees( 270.f ) ).GetLength() <= 0.1f )
				{
					node.direction = Vec2::MakeFromPolarDegrees( 250.f );
				}
				
			}
			else
			{
				node.direction = Vec2( m_startPosition - node.coords ).GetNormalized();
				if ( ( node.direction - Vec2::MakeFromPolarDegrees( 90.f )).GetLength() <= 0.1f )
				{
					node.direction = Vec2::MakeFromPolarDegrees( 85.f );
				}
			}

			if ( node.value > 0.f )
			{
				m_nodes.push_back( node );
			}
		}
	}

	//SetToMapTiles();
}

void SymmetricPotentialField::Clear()
{

}

AABB2 SymmetricPotentialField::GetBoudingBox()
{
	Vec2 minCoords = Vec2((float) (m_startPosition.x - m_dimesions.x / 2) ,(float)( m_startPosition.y - m_dimesions.y / 2) );
	Vec2 maxCoords = Vec2( (float) (m_startPosition.x + m_dimesions.x / 2) , (float)(m_startPosition.y + m_dimesions.y / 2) );

	return AABB2( minCoords , maxCoords );
}

void SymmetricPotentialField::DebugRender()
{
	std::vector<Vertex_PCU> verts;
	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		AABB2 aabb = AABB2( Vec2( m_nodes[ i ].coords ) , Vec2( m_nodes[ i ].coords.x + 1 , m_nodes[ i ].coords.y + 1 ) );
		AppendAABB2( verts , aabb , Rgba8( 100 , 0 , 0 , 100 ) );
		Vec2 startPoint = aabb.GetCenter() + Vec2( 0.f , -0.4f );
		Vec2 endPoint = startPoint + m_nodes[ i ].direction * 0.8f;
		g_theRenderer->DrawLine( startPoint , endPoint , Rgba8( 0 , 0 , 0 , 255 ) , 0.05f );
		g_theRenderer->DrawDisc( endPoint , 0.1f , Rgba8( 0 , 0 , 0 , 255 ) );
	}

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawVertexArray( verts );

}

bool SymmetricPotentialField::GetDirectionAndValueForCoords( const Vec2& position , Vec2& outDir , float& outValue )
{
	IntVec2 pos = IntVec2( RoundDownToInt( position.x ) , RoundDownToInt( position.y ) );

	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].coords.x == pos.x && m_nodes[ i ].coords.y == pos.y )
		{
			outDir = m_nodes[ i ].direction;
			outValue = m_nodes[ i ].value;
			return true;
		}
	}

	return false;

}

void SymmetricPotentialField::SetToMapTiles()
{
	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].coords.x < m_game->m_occMapGameSize.x && m_nodes[ i ].coords.x >0 && m_nodes[ i ].coords.y < m_game->m_occMapGameSize.y && m_nodes[ i ].coords.y>0 )
		{
			m_game->m_mainMapCreator->m_tiles[ m_game->GetTileIndexForOccGame( m_nodes[ i ].coords) ].m_doesHaveDirection = true;
			m_game->m_mainMapCreator->m_tiles[ m_game->GetTileIndexForOccGame( m_nodes[ i ].coords) ].m_isDirectionPositive = m_isPositive;
			m_game->m_mainMapCreator->m_tiles[ m_game->GetTileIndexForOccGame( m_nodes[ i ].coords) ].m_direction = m_nodes[i].direction.GetAngleDegrees();
			m_game->m_mainMapCreator->m_tiles[ m_game->GetTileIndexForOccGame( m_nodes[ i ].coords) ].m_directionIntensity = m_nodes[i].value;
		}
	}
}
