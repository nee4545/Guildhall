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

	// accessors
	int GetVertexCount() const;
	int GetEdgeCount() const;
	void GetEdge( int idx , Vec2* outStart , Vec2* outEnd );
	void GetPoints( Vec2* outPoints ) const;

public: // static constructors (feel free to just use a constructor - I just like descriptive names)
		// in this case, these two take the same parameters but behave differently

	// construct from a counter-clockwise line loop
	static Polygon2D MakeFromLineLoop( Vec2 const* points , unsigned int pointCount );

	// create a convex wrapping of a collection of points;  
	static Polygon2D MakeConvexFromPointCloud( Vec2 const* points , unsigned int pointCount );

private:
	std::vector<Vec2> m_points;
};