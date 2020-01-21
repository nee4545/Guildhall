#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"






Game::Game()
{
	//tank=render->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
}

void Game::Update( float deltaseconds )
{
	
}

void Game::Render()
{
	AABB2 aabb= AABB2(0.f,0.f,50.f,50.f);
	//render->BindTexture(tank);
	render->DrawAABB2D(aabb,Rgba8(255,0,255,255));
	
}




void Game::DrawLine( const Vec2 &start,const Vec2 &end, const Rgba8& color, float thickness )
{
	

	Vec2 fwd=end-start;
	fwd.SetLength(thickness*0.5f);
	Vec2 lef=fwd.GetRotated90Degrees();

	Vec2 endRight= end+fwd-lef;
	Vec2 startRight= start-fwd-lef;
	Vec2 endLeft= end+fwd+lef;
	Vec2 startLeft= start-fwd+lef;

	Vec3 er=Vec3(endRight.x,endRight.y,0.f);
	Vec3 el=Vec3( endLeft.x, endLeft.y, 0.f );
	Vec3 sr=Vec3( startRight.x, startRight.y, 0.f );
	Vec3 sl=Vec3( startLeft.x, startLeft.y, 0.f );




	Vertex_PCU vert1[3] = 
	{
		Vertex_PCU(sr,color,Vec2(0.f,0.f)),
		Vertex_PCU(er,color,Vec2(0.f,0.f)),
		Vertex_PCU(el,color,Vec2(0.f,0.f))
		
	};

	Vertex_PCU vert2[3] =
	{
		Vertex_PCU( sr,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( el,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( sl,color,Vec2( 0.f,0.f ) )
	};

	render->DrawVertexArray(3,vert1);
	render->DrawVertexArray(3,vert2);
	

}

void Game::DrawRing( const Vec2 centre, float radius, Rgba8 color, float thickness )
{
	constexpr float theta= 360.f/16.f;

	for( int index=0; index<16; index++ )
	{
		float degree1=theta*float( index );
		float degree2=theta*float( index+1 );

		DrawLine( centre+Vec2( radius*CosDegrees( degree1 ), radius*SinDegrees( degree1 ) ), centre+Vec2( radius*CosDegrees( degree2 ), radius*SinDegrees( degree2 ) ), color, thickness );
	}

}





































