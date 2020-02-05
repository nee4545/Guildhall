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
		vertexStrtNum = ( vertexEndNum + 1 ) % m_points.size();
		vertexStrtNum = ( vertexStrtNum + 1 ) % m_points.size();

	}

	return true;
	
}

bool Polygon2D::Contains( Vec2 point ) const
{
	for ( int index = 0; index < m_points.size(); index++ )
	{
		if ( m_points[ index ] == point )
		{
			return true;
		}
	}

	return false;
}

float Polygon2D::GetDistance( Vec2 point ) const
{
	Vec2 closestPoint = GetClosestPoint( point );

	return ( point - closestPoint ).GetLength();
}

Vec2 Polygon2D::GetClosestPoint( Vec2 point ) const
{
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

	if ( toReturn.IsValid() )
	{
		return toReturn;
	}
	else
	{
		ERROR_AND_DIE( "Invalid points" );
	}

}

//Polygon2D Polygon2D::MakeConvexFromPointCloud( Vec2 const* points , unsigned int pointCount )
//{
//	//Get rightmost point on the point cloud
//	//Get bottom most point on the point cloud
//	//make an edge out of it
//	//From that make all edges on the point cloud and choose the one that has max 
//
//	Vec2 rightMostVertex = points[ 0 ];
//	Vec2 downMostVertex = points[ 1 ];
//
//	for ( int index = 0; index < pointCount; index++ )
//	{
//		if ( points[ index ].x > rightMostVertex.x )
//		{
//			rightMostVertex = points[ index ];
//		}
//	}
//
//	
//	
//
//}

