#pragma once
#include "Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "GameCommon.hpp"

class PlayerShip: public Entity
{
	float m_thrustFraction=0.f;
public:
	Vertex_PCU vertices[15];
	Vertex_PCU vertices_original[15];

	

	PlayerShip(Game* thegame, Vec2 position):Entity(thegame,position)
	{
		Rgba8 shipColor = Rgba8( 132, 156, 169, 255 );

		vertices[0] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices[1] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices[2] = Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) ); // Point where bullet spawns

		vertices[3] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices[4] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices[5] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );

		vertices[6] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices[7] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) ); 
		vertices[8] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );

		vertices[9] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices[10] = Vertex_PCU( Vec3( 2.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices[11] = Vertex_PCU( Vec3( 0.f, 2.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );

		vertices[12] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices[13] = Vertex_PCU( Vec3( 2.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices[14] = Vertex_PCU( Vec3( 0.f, -2.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );

		m_velocity= Vec2( 0.f, 0.f );
		m_orientationDegrees=0.f;
		m_physicsRadius=PLAYER_SHIP_PHYSICS_RADIUS;
		m_cosmeticRadius=PLAYER_SHIP_COSMETIC_RADIUS;


		// We keep this constant for now
		vertices_original[0] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[1] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[2] = Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) ); // Point where bullet should spawn

		vertices_original[3] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[4] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[5] = Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );

		vertices_original[6] = Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[7] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[8] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );

		vertices_original[9] = Vertex_PCU( Vec3( -2.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[10] = Vertex_PCU( Vec3( 2.f, 1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[11] = Vertex_PCU( Vec3( 0.f, 2.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );

		vertices_original[12] = Vertex_PCU( Vec3( -2.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[13] = Vertex_PCU( Vec3( 2.f, -1.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );
		vertices_original[14] = Vertex_PCU( Vec3( 0.f, -2.f, 0.f ), shipColor, Vec2( 0.f, 0.f ) );

	
	}
	

	void AcclerateShip(float deltaseconds);
    
	void RotateShipRight(float deltaseconds);

	void RotateShipLeft(float deltaseconds);

	void MoveShip(float deltaseconds);

	virtual void Update(float deltaseconds)override;

	void CheckWallCollission();

	void Shoot(Vec2 pos,float orientationDergees);

	void ShootJoystick(Vec2 pos,float orientationDergees);

	virtual void Render()override;

	virtual void Die() override;

	void TransformVertexArray(int num_ship_vertices, Vertex_PCU* shipVert, float scale, float orientationDegrees, Vec2 translation);

	void UpdateFromJoystick(float deltaseconds);

	void ThrustAndMove(float deltaseconds);

	
	

	
	
	


	

	
};