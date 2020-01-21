#include "Game/Astroid.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "GameCommon.hpp"

Astroid::Astroid( Game* thegame, Vec2 position ):Entity( thegame, position )
{
	Rgba8 AstroidColor=Rgba8( 100, 100, 100, 255 );

	RandomNumberGenerator random= RandomNumberGenerator();

	float angleDegrees=(float)360/16;
	float initialLength= random.GetRandomIntInRange( 16, 20 )/10.f;

	vertices[0]=Vertex_PCU( Vec3( 0.f, 0.f, 0.f ), AstroidColor, Vec2( 0.f, 0.f ) );
	vertices[1]=Vertex_PCU( Vec3( initialLength, 0.f, 0.f ), AstroidColor, Vec2( 0.f, 0.f ) );

	initialLength=random.GetRandomIntInRange( 16, 20 )/10.f;

	float initialX=CosDegrees( angleDegrees )*initialLength;
	float initialY=SinDegrees( angleDegrees )*initialLength;

	vertices[2]=Vertex_PCU( Vec3( initialX, initialY, 0.f ), AstroidColor, Vec2( 0.f, 0.f ) );
	angleDegrees+=360.f/16.f;

	for( int index=3; index<48; index+=3 )
	{

		vertices[index]=vertices[index-3];
		vertices[index+1]=vertices[index-1];

		angleDegrees+=360.f/16.f;
		float length=random.GetRandomIntInRange( 16, 20 )/10.f;


		vertices[index+2]=Vertex_PCU( Vec3( length*CosDegrees( angleDegrees ), length*SinDegrees( angleDegrees ), 0.f ), AstroidColor, Vec2( 0.f, 0.f ) );

	}

	vertices[47]=vertices[1];

	for( int index=0; index<47; index++ )
	{
		vertices_original[index]=vertices[index];
	}


	SetRandomAngularVelocity();
	SetRandomLinearVelocity();
	m_health=3;
	m_physicsRadius=ASTEROID_PHYSICS_RADIUS;
	m_cosmeticRadius=ASTEROID_COSMETIC_RADIUS;

}

void Astroid::SetRandomAngularVelocity()
{
	RandomNumberGenerator random=RandomNumberGenerator();
	int randomNum= random.GetRandomIntInRange(-200,200);

	m_angularVelocity=(float)randomNum;
}

void Astroid::SetRandomLinearVelocity()
{
	RandomNumberGenerator random=RandomNumberGenerator();
	int randomDegrees= random.GetRandomIntInRange(0,360);

	m_velocity.x=(float)cos(randomDegrees);
	m_velocity.y=(float)sin(randomDegrees);
}

void Astroid::MoveAstroid( float deltaseconds )
{
	m_position.x+=m_velocity.x*deltaseconds*ASTEROID_SPEED;
	m_position.y+=m_velocity.y*deltaseconds*ASTEROID_SPEED;
}

void Astroid::BounceOffWall()
{

	if( m_position.x+m_cosmeticRadius>200.f || m_position.x-m_cosmeticRadius<0.f )
	{
		m_velocity.x=-m_velocity.x;
	}

	if( m_position.y+m_cosmeticRadius>100.f || m_position.y-m_cosmeticRadius<0.f )
	{
		m_velocity.y=-m_velocity.y;
	}
}

void Astroid::Spin(float deltaseconds)
{
	m_orientationDegrees+=m_angularVelocity*deltaseconds;
}

void Astroid::TransformVertexArray( int num_ship_vertices, Vertex_PCU* astroidVert, float scale, float orientationDegrees, Vec2 translation )
{
	for( int index=0; index<num_ship_vertices; index++ )
	{
		vertices[index].m_position=TransformPosition3DXY( astroidVert[index].m_position, scale, orientationDegrees, translation );
	}
}

void Astroid::Update( float deltaseconds )
{
	BounceOffWall();
	MoveAstroid(deltaseconds);
	Spin(deltaseconds);
	//Render();
}

void Astroid::Render()
{
	TransformVertexArray(48,vertices_original,1.f,m_orientationDegrees,m_position);
	render->DrawVertexArray(48,vertices);
}

void Astroid::Die()
{
	m_isGarbage= true;
}
