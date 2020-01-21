#include "Game/Bettle.hpp"
#include "Engine/Math/MathUtils.hpp"

void Bettle::FollowPlayerShip()
{
	Vec2 pos=m_game->GetPlayerShipPosition()-m_position;
	m_orientationDegrees=Atan2Degrees(pos.y,pos.x);

}

void Bettle::MoveBettle( float deltaseconds )
{
	m_position.x+=bettle_speed*deltaseconds*CosDegrees(m_orientationDegrees);
	m_position.y+=bettle_speed*deltaseconds*SinDegrees(m_orientationDegrees);
}

void Bettle::Update( float deltaseconds )
{
	if(m_game->GetPlayerShipStatus()==false)
	{ 
	FollowPlayerShip();
	MoveBettle(deltaseconds);
	}
	
	//Render();
}

void Bettle::Render()
{
	TransformVertexArray(15,vertices_original,1.5f,m_orientationDegrees,m_position);
	render->DrawVertexArray(15,vertices);
}

void Bettle::Die()
{
	m_isGarbage=true;
}

void Bettle::TransformVertexArray( int num_vertices, Vertex_PCU* Vert, float scale, float orientationDegrees, Vec2 translation )
{
	for( int index=0; index<num_vertices; index++ )
	{
		vertices[index].m_position=TransformPosition3DXY( Vert[index].m_position, scale, orientationDegrees, translation );
	}
}

