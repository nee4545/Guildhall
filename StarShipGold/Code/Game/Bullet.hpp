#pragma once
#include "Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "GameCommon.hpp"

class Bullet: public Entity
{
public:
	Vertex_PCU vertices[6];
	Vertex_PCU vertices_original[6];

	Bullet( Game* thegame, Vec2 position ):Entity( thegame, position )
	{
		Rgba8 bulletTipColor=  Rgba8(255, 255, 0, 255 );
		Rgba8 bulletSideColor = Rgba8(255, 0, 0, 255);
		Rgba8 bulletTailColor= Rgba8(255,0,0,0);

		vertices[0]=Vertex_PCU(Vec3(.5f,0.f,0.f),bulletTipColor,Vec2(0.f,0.f));
		vertices[1]=Vertex_PCU(Vec3(0.f,-.5f,0.f),bulletSideColor,Vec2(0.f,0.f));
		vertices[2]=Vertex_PCU(Vec3(0.f,.5f,0.f),bulletSideColor,Vec2(0.f,0.f));

		vertices[3]=Vertex_PCU(Vec3(0.f,-.5f,0.f),bulletSideColor,Vec2(0.f,0.f));
		vertices[4]=Vertex_PCU(Vec3(-2.f,0.f,0.f),bulletTailColor,Vec2(0.f,0.f));
		vertices[5]=Vertex_PCU(Vec3(0.f,.5f,0.f),bulletSideColor,Vec2(0.f,0.f));

		vertices_original[0]=Vertex_PCU( Vec3( .5f, 0.f, 0.f ), bulletTipColor, Vec2( 0.f, 0.f ) );
		vertices_original[1]=Vertex_PCU( Vec3( 0.f, -.5f, 0.f ), bulletSideColor, Vec2( 0.f, 0.f ) );
		vertices_original[2]=Vertex_PCU( Vec3( 0.f, .5f, 0.f ), bulletSideColor, Vec2( 0.f, 0.f ) );

		vertices_original[3]=Vertex_PCU( Vec3( 0.f, -.5f, 0.f ), bulletSideColor, Vec2( 0.f, 0.f ) );
		vertices_original[4]=Vertex_PCU( Vec3( -2.f, 0.f, 0.f ), bulletTailColor, Vec2( 0.f, 0.f ) );
		vertices_original[5]=Vertex_PCU( Vec3( 0.f, .5f, 0.f ), bulletSideColor, Vec2( 0.f, 0.f ) );

		m_physicsRadius=BULLET_PHYSICS_RADIUS;
		m_cosmeticRadius=BULLET_COSMETIC_RADIUS;


	}

	void MoveBullet(float deltaseconds);
	virtual void Update(float deltaseconds)override;
	virtual void Render()override;
	virtual void Die() override;

};