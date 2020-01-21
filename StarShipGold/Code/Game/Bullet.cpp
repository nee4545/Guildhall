#include "Game/Bullet.hpp"
#include "Engine/Math/MathUtils.hpp"

void Bullet::MoveBullet(float deltaseconds)
{
	m_velocity.x=BULLET_SPEED*CosDegrees(m_orientationDegrees);
	m_velocity.y=BULLET_SPEED*SinDegrees(m_orientationDegrees);
	m_position.x+=m_velocity.x*deltaseconds;
	m_position.y+=m_velocity.y*deltaseconds;
}

void Bullet::Render()
{
	for( int index=0; index<6; index++ )
	{
		vertices[index].m_position=TransformPosition3DXY( vertices_original[index].m_position, 1.f, m_orientationDegrees, m_position );
	}
	render->DrawVertexArray(6,vertices);
}

void Bullet::Die()
{
	m_isGarbage=true;
}

void Bullet::Update(float deltaseconds)
{
	MoveBullet(deltaseconds);
	//Render();
}