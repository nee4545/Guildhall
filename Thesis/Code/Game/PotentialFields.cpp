#include "Game/PotentialFields.hpp"
#include "Engine/Core/Plane.hpp"

PotentialField::PotentialField( float objectRadius , float potentialFeildRadius , Vec2 startPosition )
{
	m_startPosition = IntVec2( RoundDownToInt( startPosition.x ) , RoundDownToInt( startPosition.y ) );
	AABB2 box = AABB2( m_startPosition.x - RoundDownToInt( potentialFeildRadius * 0.5f ) , m_startPosition.y - RoundDownToInt( potentialFeildRadius * 0.5f ) , m_startPosition.x + RoundDownToInt( potentialFeildRadius * 0.5f ) , m_startPosition.y + RoundDownToInt( potentialFeildRadius * 0.5f ) );
	m_radius = potentialFeildRadius*0.5f;

	int size = RoundDownToInt( potentialFeildRadius );
	int numNodes = RoundDownToInt( potentialFeildRadius ) * RoundDownToInt( potentialFeildRadius );

	for ( int i = ( int ) box.mins.x; i <= ( int ) box.maxs.x; i++ )
	{
		for ( int j = ( int ) box.mins.y; j <= ( int ) box.maxs.y; j++ )
		{
			PotentialFieldNode node;
			node.xCoord = i;
			node.yCoord = j;

			nodes.push_back( node );
		}
	}
}

PotentialField::~PotentialField()
{

}

void PotentialField::ConvertToLocalSpace()
{
	float potentialFeildRadius = m_radius;

	AABB2 box = AABB2( m_startPosition.x - RoundDownToInt( potentialFeildRadius * 0.5f ) , m_startPosition.y - RoundDownToInt( potentialFeildRadius * 0.5f ) , m_startPosition.x + RoundDownToInt( potentialFeildRadius * 0.5f ) , m_startPosition.y + RoundDownToInt( potentialFeildRadius * 0.5f ) );
	int xOffset = -box.GetCenter().x;
	int yOffset = -box.GetCenter().y;

	m_startPosition.x = 0;
	m_startPosition.y = 0;
	


	for ( int i = 0; i < nodes.size(); i++ )
	{
		nodes[ i ].xCoord += xOffset;
		nodes[ i ].yCoord += yOffset;
	}

	isInLocalSpace = true;
}

void PotentialField::SetPosition( Vec2 worldPosition )
{
	IntVec2 worldPos = IntVec2( RoundDownToInt( worldPosition.x ) , RoundDownToInt( worldPosition.y ) );


	if ( !isInLocalSpace )
	{
		ConvertToLocalSpace();
	}

	isInLocalSpace = false;
	m_startPosition.x = worldPos.x;
	m_startPosition.y = worldPos.y;

	for ( int i = 0; i < nodes.size(); i++ )
	{
		nodes[ i ].xCoord += worldPos.x;
		nodes[ i ].yCoord += worldPos.y;
	}
}


float PotentialField::GetPotentialFieldForPosition( Vec2 pos, bool shouldFlipValues )
{
	IntVec2 position = IntVec2( RoundDownToInt( pos.x ) , RoundDownToInt( pos.y ) );
	for ( int i = 0; i < nodes.size(); i++ )
	{
		if ( nodes[ i ].xCoord == position.x && nodes[ i ].yCoord == position.y )
		{
			if ( nodes[ i ].isConsiderd && nodes[ i ].direction > -1.f )
			{
				if ( !shouldFlipValues )
				{
					return nodes[ i ].direction;
				}
				else
				{
					return nodes[ i ].direction + 180.f;
				}
			}
			else
			{
				return -1.f;
			}
		}
	}

	return -1.f;
}

void PotentialField::ResizeAndRemoveGarbageNodes()
{
	std::vector<PotentialFieldNode> newNodes;

	for ( int i = 0; i < nodes.size(); i++ )
	{
		if ( nodes[ i ].isConsiderd )
		{
			newNodes.push_back( nodes[ i ] );
		}
	}

	nodes = newNodes;



}

void PotentialField::PotentialFieldQueryForEntityEndPosition( Vec2& entityEndPosition, bool& shoudGiveFlippedValues , bool& shouldCachePosition , Vec2& entityCachePosition )
{
	OBB2 box = GetBoundingBox();

	Plane2 safePlane = Plane2( forwardVec , GetCentre() + ( forwardVec * GetBoundingRadius() * 0.7f ) );
	Plane2 flippedPlane = Plane2( -forwardVec , GetCentre() + ( -forwardVec * GetBoundingRadius() * 0.7f ) );
	Plane2 rightPlane = Plane2( forwardVec.GetRotated90Degrees() ,GetCentre() + (forwardVec.GetRotated90Degrees() * GetBoundingRadius()* 0.7f ) );
	Plane2 leftPlane = Plane2( forwardVec.GetRotatedMinus90Degrees() , GetCentre() + ( forwardVec.GetRotatedMinus90Degrees() * GetBoundingRadius() *0.7f ) );

	if ( safePlane.IsPointInFrontOfPlane( entityEndPosition ) )
	{
		shoudGiveFlippedValues = false;
		shouldCachePosition = false;
		return;
	}


	if ( flippedPlane.IsPointInFrontOfPlane( entityEndPosition ) )
	{
		shoudGiveFlippedValues = true;
		shouldCachePosition = false;
		return;
	}


	Vec2 pos1 = GetCentre() + ( forwardVec * GetBoundingRadius() );
	Vec2 pos2 = GetCentre() + ( -forwardVec * GetBoundingRadius() );

	if ( rightPlane.IsPointInFrontOfPlane( entityEndPosition ) )
	{
		shouldCachePosition = true;
		if ( ( pos1 - entityEndPosition ).GetLengthSquared()  < ( pos2 - entityEndPosition).GetLengthSquared() )
		{
			entityCachePosition = pos1;
			shoudGiveFlippedValues = false;
		}
		else
		{
			entityCachePosition = pos2;
			shoudGiveFlippedValues = true;
		}
		return;
	}

	if ( leftPlane.IsPointInFrontOfPlane( entityEndPosition ) )
	{
		shouldCachePosition = true;
		if ( ( pos1 - entityEndPosition ).GetLengthSquared() < ( pos2 - entityEndPosition ).GetLengthSquared() )
		{
			entityCachePosition = pos1;
			shoudGiveFlippedValues = false;
		}
		else
		{
			entityCachePosition = pos2;
			shoudGiveFlippedValues = true;
		}
	}

}

void PotentialField::Rotate(float degrees)
{

	Vec2 currentPos = Vec2(m_startPosition.x,m_startPosition.y);
	if ( !isInLocalSpace )
	{
		ConvertToLocalSpace();
	}


	for ( int i = 0; i < nodes.size(); i++ )
	{
		Vec2 vec = Vec2( nodes[ i ].xCoord , nodes[ i ].yCoord );
		vec.RotateDegrees( degrees );
		nodes[ i ].xCoord = RoundDownToInt( vec.x );
		nodes[ i ].yCoord = RoundDownToInt( vec.y );

		nodes[ i ].direction += degrees;
	}

	forwardVec.RotateDegrees( degrees );
	SetPosition( currentPos );

}

OBB2 PotentialField::GetBoundingBox()
{
	int minX = 9999;
	int minY = 9999;
	int maxX = -9999;
	int maxY = -9999;


	for ( int i = 0; i < nodes.size(); i++ )
	{
		if ( nodes[ i ].xCoord < minX )
		{
			minX = nodes[ i ].xCoord;
		}

		if ( nodes[ i ].yCoord < minY )
		{
			minY = nodes[ i ].yCoord;
		}
	}


	for ( int i = 0; i < nodes.size(); i++ )
	{
		if ( nodes[ i ].xCoord > maxX )
		{
			maxX = nodes[ i ].xCoord;
		}

		if ( nodes[ i ].yCoord > maxY )
		{
			maxY = nodes[ i ].yCoord;
		}
	}

	AABB2 aabb = AABB2( minX , minY , maxX , maxY );

	OBB2 obb = OBB2( aabb , forwardVec.GetAngleDegrees() );

	return obb;

}

Vec2 PotentialField::GetCentre()
{
	OBB2 box = GetBoundingBox();

	return box.GetCenter();

}

float PotentialField::GetBoundingRadius()
{
	OBB2 box = GetBoundingBox();

	Vec2 dimension =box.GetDimensions();

	if ( dimension.x > dimension.y )
	{
		return dimension.x * 0.7f;
	}
	else
	{
		return dimension.y * 0.7f;
	}
}
