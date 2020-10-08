#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"

Bullet::Bullet( Texture* sprite , Vec2 forwardVec, float speed , Faction faction/*=FACTION_GOOD*/ )
{
	m_forwardVec = forwardVec;
	m_sprite = sprite;
	m_faction = faction;
	m_speed = speed;

	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.5f , -0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.5f , 0.5f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );
}

Bullet::~Bullet()
{

}

void Bullet::Update( float deltaseconds )
{
	m_position += m_speed * m_forwardVec * deltaseconds;
}

void Bullet::Render()
{
	g_theRenderer->BindTexture( m_sprite );

	g_theRenderer->BindTexture( nullptr );
}

void Bullet::DebugRender()
{

}

void Bullet::Die()
{
	m_isGarbage = true;
}
