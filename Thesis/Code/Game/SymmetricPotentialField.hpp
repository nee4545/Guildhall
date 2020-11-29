#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/AABB2.hpp"
#include <vector>

class Game;

struct SymmetricPotentialFieldNode
{
	IntVec2 coords;
	Vec2 direction;
	float value;
};


class SymmetricPotentialField
{
public:

	SymmetricPotentialField(Game* game,IntVec2 startPos,IntVec2 dimensions,float startValue,float fallOff,bool isPositive);
	~SymmetricPotentialField();
	void Create();
	void Clear();
	AABB2 GetBoudingBox();
	void DebugRender();
	bool GetDirectionAndValueForCoords( const Vec2& position , Vec2& outDir , float& outValue );
	void SetToMapTiles();


public:
	Game* m_game = nullptr;
	IntVec2 m_dimesions;
	IntVec2 m_startPosition;
	float m_startValue = 1.f;
	float m_fallOffPerTile = 1.f;
	bool m_isPositive = false;

	std::vector<SymmetricPotentialFieldNode> m_nodes;
};