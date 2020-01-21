#include "Game/MapGenStep.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/MapGenStepMutate.hpp"
#include "Game/MapGenStepWorms.hpp"
#include "Game/MapGenStepFromImage.hpp"
#include "Game/MapGenStepCellularAutomata.hpp"

MapGenStep::MapGenStep( const tinyxml2::XMLElement& stepElement )
{
	m_stepName          = stepElement.Name();
	std::string ifTile  = ParseXmlAttribute ( stepElement , "ifTile"  , "None" );
	std::string setTile = ParseXmlAttribute ( stepElement , "setTile" , "None" );

	m_ifTile   = TileDefinition::s_definitions[ ifTile ];
	m_setTile  = TileDefinition::s_definitions[ setTile ];
	m_chanceToRun		 = ParseXmlAttribute ( stepElement , "chanceToRun" , m_chanceToRun );
	m_chancePerTile		 = ParseXmlAttribute ( stepElement , "chancePerTile" , m_chancePerTile );
	m_numberOfIterations = ParseXmlAttribute ( stepElement , "iterations" , m_numberOfIterations );

}

void MapGenStep::RunStep ( Map& map )
{
	RandomNumberGenerator rng = RandomNumberGenerator ();

	if( !m_chanceToRun.GetRandomInRange ( rng ) )
	{
		return;
	}

	int numiterations = m_numberOfIterations.GetRandomInRange ( rng );
	for( int count = 0; count < numiterations; count++ )
	{
		RunStepOnce ( map );
	}

}

MapGenStep* MapGenStep::CreateMapGenStep ( const tinyxml2::XMLElement& element )
{
	std::string stepName = element.Name ();

	if( stepName == "Mutate" )
	{
		return new MapGenStepMutate( element );
	}
	else if ( stepName == "CellularAutomata" )
	{
		return new MapGenStepCellularAutomata( element );
	}
	else if ( stepName == "Worms" )
	{
		return new MapGenStepWorms( element );
	}
	/*
	else if( stepName == "FromImage" )
	{
		return new MapGenStepFromImage( element );
	}
	else
	{
		ERROR_AND_DIE ( "XML Error: Invaild Generation" , stepName.c_str());
	}*/
	return nullptr;

}
