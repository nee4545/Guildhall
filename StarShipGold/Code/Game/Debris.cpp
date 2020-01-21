#include "Debris.hpp"
#include "Engine/Math/MathUtils.hpp"


void Debris::SpawnDebris( Vec2 pos )
{
	m_position=pos;
	
}

void Debris::Die()
{
	m_isGarbage=true;
}

void Debris::Render()
{
	TransformVertexArray( 3, vertices_original, 0.5f, m_orientationDegrees, m_position );
	render->DrawVertexArray( 3, vertices );
}

void Debris::Update( float deltaseconds )
{
	MoveDebris(deltaseconds);
	m_timeToLive-=deltaseconds;

	 if( m_DebrisColor.a>0 )
	 {
		 m_DebrisColor.a-=0.002f*deltaseconds;
	 }
   
	
	if( m_timeToLive<0.f )
	{
		Die();
	}
	
}

void Debris::MoveDebris(float deltaseconds)
{
	m_position.x+=debris_speed*CosDegrees(m_orientationDegrees)*deltaseconds;
	m_position.y+=debris_speed*SinDegrees(m_orientationDegrees)*deltaseconds;

}

void Debris::TransformVertexArray( int num_vertices, Vertex_PCU* Vert, float scale, float orientationDegrees, Vec2 translation )
{
	for( int index=0; index<num_vertices; index++ )
	{
		vertices[index].m_color=m_DebrisColor;
		vertices[index].m_position=TransformPosition3DXY( Vert[index].m_position, scale, orientationDegrees, translation );
	}
}
