#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>

class Imap
{
public:
	IntVec2 m_anchorPoint;
	IntVec2 m_dimension;
	std::vector<float> m_influenceValues;
	float** m_infValues;

	Imap( IntVec2 anchorPoint , IntVec2 dimensions );
	~Imap();

	void SetCellValue( IntVec2 cellCoords , float value );
	float GetCellValue( IntVec2 cellCoords );
	void AddValue( IntVec2 cellCoords , float value );
	//void PropogateInfluence(IntVec2 centreCoords,int radius,)

};