#pragma once
#include "Game/MapGenStep.hpp"

class MapGenStepWorms : public MapGenStep
{
public:
	MapGenStepWorms( const tinyxml2::XMLElement& element );
	~MapGenStepWorms() {};

	virtual void RunStepOnce( Map& map ) override;

private:

	IntRange m_numWorms = IntRange( 1 );
	IntRange m_wormLength = IntRange( 1 );

};
