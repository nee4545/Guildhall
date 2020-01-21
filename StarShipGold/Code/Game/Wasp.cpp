#include "Game/Wasp.hpp"
#include "Engine/Math/MathUtils.hpp"


void Wasp::FollowPlayerShip()
{
	Vec2 pos=m_game->GetPlayerShipPosition()-m_position;
	m_orientationDegrees=Atan2Degrees( pos.y, pos.x );
}

void Wasp::MoveWasp( float deltaseconds )
{
     m_velocity.x=(m_velocity.x) + wasp_accleration*deltaseconds*CosDegrees(m_orientationDegrees);
	 m_velocity.y=(m_velocity.y) + wasp_accleration*deltaseconds*SinDegrees(m_orientationDegrees);

     //m_velocity+=wasp_accleration*deltaseconds;

	
	 m_velocity.ClampLength(max_velocity);
	

	m_position.x+=m_velocity.x*deltaseconds;
	m_position.y+=m_velocity.y*deltaseconds;

	//m_position.ClampLength((max_velocity));

	
}

void Wasp::Update( float deltaseconds )
{
	if( m_game->GetPlayerShipStatus()==false )
	{ 
	FollowPlayerShip();
	MoveWasp(deltaseconds);
	}

	Render();
}

void Wasp::Render()
{
	TransformVertexArray( 12, vertices_original, 1.5f, m_orientationDegrees, m_position );
	render->DrawVertexArray( 12, vertices );
}

void Wasp::Die()
{
	m_isGarbage=true;
}

void Wasp::TransformVertexArray( int num_vertices, Vertex_PCU* Vert, float scale, float orientationDegrees, Vec2 translation )
{
	for( int index=0; index<num_vertices; index++ )
	{
		vertices[index].m_position=TransformPosition3DXY( Vert[index].m_position, scale, orientationDegrees, translation );
	}
}

