#pragma once
#include "Game/MapGenStep.hpp"

class MapGenStepMutate : public MapGenStep
{
public:
	MapGenStepMutate ( const tinyxml2::XMLElement& element );
	~MapGenStepMutate () {};

	virtual void RunStepOnce( Map& map ) override;

private:

	FloatRange m_chanceToMutate = FloatRange( 1.f );

};

