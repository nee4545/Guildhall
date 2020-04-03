#pragma once

#include "Engine/Math/Vec2.hpp"
#include <vector>

class Polygon2D
{
public:
	bool IsValid() const; // must have at least 3 points to be considered a polygon

	bool IsConvex() const;
	bool Contains( Vec2 point ) const;
	float GetDistance( Vec2 point ) const;
	Vec2 GetClosestPoint( Vec2 point ) const;
	Vec2 GetClosestPointOnTheEdges( Vec2 point ) const;
	Vec2 GetClosestPointOnEdgeAndIndicesOfTheEdge( Vec2 point , int &outIndex1 , int &outIndex2 );

	// accessors
	int GetVertexCount() const;
	int GetEdgeCount() const;
	void GetEdge( int idx , Vec2* outStart , Vec2* outEnd );
	Vec2 GetBottomMostEdge();
	Vec2 GetRightMostEdge();
	Vec2 GetLeftMostEdge();
	void GetPoints( Vec2* outPoints ) const;
	std::vector<Vec2> m_points;
	Vec2 m_localPos;

public: // static constructors (feel free to just use a constructor - I just like descriptive names)
		// in this case, these two take the same parameters but behave differently

	// construct from a counter-clockwise line loop
	static Polygon2D MakeFromLineLoop( Vec2 const* points , unsigned int pointCount );

	// create a convex wrapping of a collection of points;  
	static Polygon2D MakeConvexFromPointCloud( Vec2 const* points , int pointCount );
	static bool IsPointPartOfPolygon(Vec2 point, Polygon2D &polygon);
	void Translate2D( Vec2 translation2D );
	Vec2 GetCentre();
	void SetPosition(Vec2 pos);
	float GetBoundingDiscRadius();

private:
	Vec2 m_centre;
};