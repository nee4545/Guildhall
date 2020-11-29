#include "Game/IMap.hpp"
#include "Game/GameCommon.hpp"

Imap::Imap( IntVec2 anchorPoint , IntVec2 dimensions )
{

	m_anchorPoint = anchorPoint;
	m_dimension = dimensions;

	m_infValues = new float* [ m_dimension.x ];
	for ( int i = 0; i < m_dimension.x; ++i )
	{
		m_infValues[ i ] = new float[ m_dimension.y ];
	}

}

Imap::~Imap()
{

}

void Imap::SetCellValue( IntVec2 cellCoords , float value )
{
	if ( cellCoords.x >= 0 && cellCoords.x < m_dimension.x && cellCoords.y >= 0 && cellCoords.y < m_dimension.y )
	{
		m_infValues[ cellCoords.x ][ cellCoords.y ] = value;

	}
	else
	{
		g_theConsole.PrintError( "Invalid cell coords" );
	}

}

float Imap::GetCellValue( IntVec2 cellCoords )
{
	if ( cellCoords.x >= 0 && cellCoords.x < m_dimension.x && cellCoords.y >= 0 && cellCoords.y < m_dimension.y )
	{
		return	m_infValues[ cellCoords.x ][ cellCoords.y ];

	}
	else
	{
		g_theConsole.PrintError( "Invalid cell coords" );
		return -100.f;
	}
}

void Imap::AddValue( IntVec2 cellCoords , float value )
{
	if ( cellCoords.x >= 0 && cellCoords.x < m_dimension.x && cellCoords.y >= 0 && cellCoords.y < m_dimension.y )
	{
		m_infValues[ cellCoords.x ][ cellCoords.y ] += value;

	}
	else
	{
		g_theConsole.PrintError( "Invalid cell coords" );
	}
}
