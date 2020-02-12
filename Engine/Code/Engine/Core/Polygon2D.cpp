#include "Engine/Core/Polygon2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"

bool Polygon2D::IsValid() const
{
	if ( m_points.size() < 3 )
	{
		return false;
	}

	if ( !IsConvex() )
	{
		return false;
	}

	return true;
}

bool Polygon2D::IsConvex() const
{
	if ( m_points.size() < 3 )
	{
		return false;
	}

	Vec2 currentNormal = (m_points[ 1 ] - m_points[ 0 ]).GetRotated90Degrees();
	currentNormal.Normalize();

	size_t vertexEndNum = 2;
	size_t vertexStrtNum = 1;
	int counter = 0;

	while ( counter<m_points.size() )
	{
		Vec2 currentEdge = m_points[ vertexEndNum ] - m_points[ vertexStrtNum ];

		if ( DotProduct2D( currentEdge , currentNormal ) < 0 )
		{
			return false;
		}

		currentNormal = currentEdge.GetRotated90Degrees();
		currentNormal.Normalize();

		counter++;
		vertexEndNum = ( vertexEndNum + 1 ) % m_points.size();
		vertexStrtNum = ( vertexStrtNum + 1 ) % m_points.size();

	}

	return true;
	
}

bool Polygon2D::Contains( Vec2 point ) const
{

	int startVert = 0;
	int endVert = 1;
	int counter = 0;

	while ( counter<m_points.size() )
	{

		Vec2 currentEdge = m_points[ endVert ] - m_points[ startVert ];
		Vec2 pointVec = point - m_points[ startVert ];
		Vec2 normal = currentEdge.GetRotated90Degrees();
		normal.Normalize();

		if ( DotProduct2D( pointVec , normal ) <= 0 )
		{
			return false;
		}

		startVert = ( startVert + 1 ) % m_points.size();
		endVert = ( endVert + 1 ) % m_points.size();
		counter++;

	}
	
	return true;
}

float Polygon2D::GetDistance( Vec2 point ) const
{
	Vec2 closestPoint = GetClosestPoint( point );

	return ( point - closestPoint ).GetLength();
}

Vec2 Polygon2D::GetClosestPoint( Vec2 point ) const
{
	if ( Contains( point ) )
	{
		return point;
	}

	std::vector<Vec2> closestPointsOnEachEdge;

	Vec2 currentEdgeStart = m_points[ 0 ];
	Vec2 currentEdgeEnd = m_points[ 1 ];

	int counter = 0;
	size_t start = 0;
	size_t end = 1;

	while ( counter<m_points.size() )
	{
		Vec2 closestPointOnEdge = GetNearestPointOnLineSegment2D( point , currentEdgeStart , currentEdgeEnd );
		closestPointsOnEachEdge.push_back( closestPointOnEdge );

		start = ( start + 1 ) % m_points.size();
		end = ( end + 1 ) % m_points.size();

		currentEdgeStart = m_points[ start ];
		currentEdgeEnd = m_points[ end ];

		counter++;
	}

	 float nearestDistance = (point - closestPointsOnEachEdge[ 0 ]).GetLength();
	 Vec2 nearestPoint = closestPointsOnEachEdge[ 0 ];

	for ( int index = 0; index < closestPointsOnEachEdge.size(); index++ )
	{
		float distance = ( point - closestPointsOnEachEdge[ index ] ).GetLength();

		if ( distance < nearestDistance )
		{
			nearestDistance = distance;
			nearestPoint = closestPointsOnEachEdge[ index ];
		}
	}

	return nearestPoint;
}

int Polygon2D::GetVertexCount() const
{
	return (int)m_points.size();
}

int Polygon2D::GetEdgeCount() const
{
	return (int)m_points.size();
}

void Polygon2D::GetEdge( int idx , Vec2* outStart , Vec2* outEnd )
{
	*outStart = m_points[ (size_t)idx ];
	*outEnd = m_points[ (size_t) idx + 1 ];
}

Vec2 Polygon2D::GetBottomMostEdge()
{
	Vec2 toReturn = m_points[ 0 ];

	for ( int index = 0; index < m_points.size(); index++ )
	{
		if ( m_points[ index ].y < toReturn.y )
		{
			toReturn = m_points[ index ];
		}
	}

	return toReturn;
}

Vec2 Polygon2D::GetRightMostEdge()
{
	Vec2 toReturn = m_points[ 0 ];

	for ( int index = 0; index < m_points.size(); index++ )
	{
		if ( m_points[ index ].x > toReturn.x )
		{
			toReturn = m_points[ index ];
		}
	}

	return toReturn;
}

Vec2 Polygon2D::GetLeftMostEdge()
{
	Vec2 toReturn = m_points[ 0 ];

	for ( int index = 0; index < m_points.size(); index++ )
	{
		if ( m_points[ index ].x < toReturn.x )
		{
			toReturn = m_points[ index ];
		}
	}

	return toReturn;
}

void Polygon2D::GetPoints( Vec2* outPoints ) const
{
	for ( int index = 0; index < m_points.size(); index++ )
	{
		outPoints[ index ] = m_points[ index ];
	}
}



Polygon2D Polygon2D::MakeFromLineLoop( Vec2 const* points , unsigned int pointCount )
{
	Polygon2D toReturn;

	for ( unsigned int index = 0; index < pointCount; index++ )
	{
		toReturn.m_points.push_back( points[ index ]);
	}

	Vec2 centre = Vec2( 0.f , 0.f );

	for ( int index = 0; index < toReturn.m_points.size(); index++ )
	{
		centre += points[ index ];
	}

	toReturn.m_centre = centre / (float)toReturn.m_points.size();


	if ( toReturn.IsValid() )
	{
		return toReturn;
	}
	else
	{
		ERROR_AND_DIE( "Invalid points" );
	}

}

Polygon2D Polygon2D::MakeConvexFromPointCloud( Vec2 const* points , int pointCount )
{
	if ( pointCount < 3 )
	{
		ERROR_AND_DIE( "Not enough points to construct" );
	}

	Polygon2D toReturn;

	Vec2 rightMostVertex = points[ 0 ];
	Vec2 nextVertex = points[ 1 ];

	for ( int index = 0; index < pointCount; index++ )
	{
		if ( points[ index ].x > rightMostVertex.x )
		{
			rightMostVertex = points[ index ];
		}
	}

	for ( int index = 0; index < pointCount; index++ )
	{
		if ( points[ index ].y < nextVertex.y && points[ index ] != rightMostVertex )
		{
			nextVertex = points[ index ];
		}
	}

	toReturn.m_points.push_back( rightMostVertex );
	toReturn.m_points.push_back( nextVertex );

	bool constructionDone = false;
	Vec2 nextPoint;
	Vec2 checkingVec = rightMostVertex - nextVertex;
	Vec2 endVertex = nextVertex;

	std::vector<Vec2> edgesToSort;

	while ( !constructionDone )
	{

		for ( int index = 0; index < pointCount; index++ )
		{
			if ( !Polygon2D::IsPointPartOfPolygon( points[ index ] , toReturn ) )
			{
				edgesToSort.push_back( points[ index ] );
			}
		}

		Vec2* nextVec = Polygon2D::GetNextPointToAddFromPointClound( checkingVec , endVertex , edgesToSort );

		if ( nextVec == nullptr )
		{
			constructionDone = true;
			break;
		}

		nextPoint = *nextVec;
		toReturn.m_points.push_back( nextPoint );
		checkingVec = endVertex - nextPoint;
		endVertex = nextPoint;
		edgesToSort.clear();

	}

	return toReturn;
	
	
}

bool Polygon2D::IsPointPartOfPolygon( Vec2 point, Polygon2D &polygon )
{

	for ( int index = 0; index < polygon.m_points.size(); index++ )
	{
		if ( polygon.m_points[ index ] == point )
		{
			return true;
		}
	}
	return false;
}

Vec2* Polygon2D::GetNextPointToAddFromPointClound( Vec2 checkingEdge, Vec2 checkingVertex , std::vector<Vec2> points )
{
	Vec2 normal = checkingEdge.GetRotated90Degrees();

	Vec2* toReturn = nullptr;

	std::vector<Vec2> pointsToConsider;

	for ( int index = 0; index < points.size(); index++ )
	{
		if ( DotProduct2D( checkingEdge , points[ index ] )>0 )
		{
			pointsToConsider.push_back( points[ index ] );
		}
	}

	if ( pointsToConsider.size() == 0 )
	{
		return nullptr;
	}

	toReturn = &(pointsToConsider[ 0 ]);

	for ( int index = 0; index < pointsToConsider.size(); index++ )
	{
		if ( *toReturn == pointsToConsider[ index ] )
		{
			continue;
		}

		if ( GetAngleDegreesBetweenVectors2D( normal , ( *toReturn - checkingVertex )) > GetAngleDegreesBetweenVectors2D( normal , ( pointsToConsider[ index ] - checkingVertex ) ))
		{
			toReturn = &pointsToConsider[ index ];
		}
	}

	return toReturn;
}

void Polygon2D::Translate2D( Vec2 translation2D )
{

	for ( int index = 0; index < m_points.size(); index++ )
	{
		m_points[ index ] += translation2D-m_localPos;
	}

	//m_centre += translation2D;
}




Vec2 Polygon2D::GetCentre()
{
	Vec2 centre = Vec2(0.f,0.f);

	for ( int index = 0; index < m_points.size(); index++ )
	{
		centre += m_points[ index ];
	}

	centre = centre / ( float ) m_points.size();

	return centre;
}

void Polygon2D::SetPosition(Vec2 pos)
{

	for ( int index = 0; index < m_points.size(); index++ )
	{
		m_points[ index ] = m_points[index]+ pos - m_localPos;
	}

	m_localPos = pos;
}

