#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/OBB2.hpp"
#include <vector>

struct PotentialFieldNode
{
	int xCoord;
	int yCoord;
	float direction = -1.f;
	bool isCore = false;
	bool isConsiderd = false;
};

class PotentialField
{
public:
	PotentialField(float objectRadius,float potentialFeildRadius, Vec2 startPosition);
	~PotentialField();

	IntVec2 m_startPosition;
	float m_radius;
	std::vector<PotentialFieldNode> nodes;
	bool isInLocalSpace = false;
	Vec2 forwardVec = Vec2( 0.f , 1.f );
	Vec2 referenceVec = Vec2( 0.f , 1.f );

	void ConvertToLocalSpace();
	void SetPosition( Vec2 worldPosition );
	float GetPotentialFieldForPosition( Vec2 pos, bool shouldFlipValues );
	void ResizeAndRemoveGarbageNodes();
	void FlipAllNonCoreRotations();

	void PotentialFieldQueryForEntityEndPosition( Vec2& entityEndPosition, bool& shoudGiveFlippedValues, bool& shouldCachePosition, Vec2& entityCachePosition);
	void Rotate(float degrees);

	OBB2 GetBoundingBox();
	Vec2 GetCentre();
	float GetBoundingRadius();


};


