#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"


DevConsole dev = DevConsole();
RandomNumberGenerator rn = RandomNumberGenerator();

//Used for Obb randomization
int degrees = 0;

//used for random line 
int lineStartPositionX = 0;
int lineEndPositionX = 0;
int lineStartPositionY = 0;
int lineEndPositionY = 0;

//Used for discs
int discRadius = 0;

//Used for capsule
int capsuleRadius = 0;
int capsuleStartX = 0;
int capsuleStartY = 0;
int capsuleEndX = 0;
int capsuleEndY = 0;




Game::Game()
{
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 200.f , 112.5f ));

	degrees = rn.RollRandomIntInRange( 0 , 360 );

	lineStartPositionX = rn.RollRandomIntInRange( 10 , 110 );
	lineStartPositionY = rn.RollRandomIntInRange( 10 , 110 );
	lineEndPositionX = rn.RollRandomIntInRange( 10 , 110 );
	lineEndPositionY = rn.RollRandomIntInRange( 10 , 110 );

	discRadius = rn.RollRandomIntInRange( 4 , 9 );


	capsuleRadius = rn.RollRandomIntInRange( 3 , 5 );
	capsuleStartX = rn.RollRandomIntInRange( 50 , 80 );
	capsuleEndX = rn.RollRandomIntInRange( 80 , 100 );
	capsuleStartY = rn.RollRandomIntInRange( 10 , 100 );
	capsuleEndY = rn.RollRandomIntInRange( 50 , 100 );

}

void Game::Update( float deltaseconds )
{
	UNUSED( deltaseconds );
}

void Game::Render()
{
	g_theRenderer->BeginCamera( *m_camera );
	DrawTextTriangles2D( *g_theRenderer , "Press F8 to reset shapes" , Vec2( 50.f , 3.f ) , 5.f , Rgba8( 0 , 0 , 100 , 255 ) );
	Vec2 mousePositionScreen = GetMousePositionScreenView();
	g_theRenderer->DrawRing( mousePositionScreen , .3f , Rgba8( 100 , 100 , 100 , 255 ) , .5f );
	DrawAABBDetails();
	DrawOBDBDetails();
	DrawLineDetails();
	DrawDiscDetails();
	DrawCapsuleDetails();
	g_theRenderer->EndCamera( *m_camera );

	
}



void Game::DrawAABBDetails()
{
	Vec2 mousePositionScreen = GetMousePositionScreenView();


	AABB2 aabb1 = AABB2( Vec2( 10.f , 10.f ) , Vec2( 30.f , 35.f ) );
	Vec2 nearestPoint = aabb1.GetNearestPoint( mousePositionScreen );

	bool isPointInsideAABB;
	isPointInsideAABB = aabb1.IsPointInside( mousePositionScreen );


	g_theRenderer->DrawAABB2D( aabb1 , Rgba8( 100 , 0 , 0 , 255 ));
	g_theRenderer->DrawLine( mousePositionScreen , nearestPoint , Rgba8( 100 , 0 , 0 , 255 ) , 0.3f );
	if ( isPointInsideAABB )
	{
		g_theRenderer->DrawAABB2D( aabb1 , Rgba8( 200 , 100 , 0 , 100 ) );
	}
	g_theRenderer->DrawRing( nearestPoint , .3f , Rgba8( 100 , 100 , 100 , 255 ) , .5f );
}

void Game::DrawLineDetails()
{
	Vec2 mousePositionScreen = GetMousePositionScreenView();

	Vec2 lineStart = Vec2( float(lineStartPositionX) , float(lineStartPositionY) );
	Vec2 lineEnd = Vec2( float( lineEndPositionX ) , float( lineEndPositionY ) );

	g_theRenderer->DrawLine( lineStart , lineEnd, Rgba8(100,100,100,100 ),0.5f);

	Vec2 nearestPointOnLine;
	Vec2 nearestPointOnLineSegment;

	nearestPointOnLine = GetNearestPointOnInfiniteLine2D( mousePositionScreen , lineStart , lineEnd );
	nearestPointOnLineSegment = GetNearestPointOnLineSegment2D( mousePositionScreen , lineStart , lineEnd );

	g_theRenderer->DrawLine( mousePositionScreen , nearestPointOnLine , Rgba8( 100 , 100 , 100 , 255 ) , 0.3f );
	g_theRenderer->DrawLine( mousePositionScreen , nearestPointOnLineSegment , Rgba8( 100 , 100 , 100 , 255 ) , 0.3f );

	g_theRenderer->DrawRing( nearestPointOnLine , 0.3f , Rgba8( 100 , 100 , 100 , 255 ) , 0.5f );
	g_theRenderer->DrawRing( nearestPointOnLineSegment , 0.3f , Rgba8( 100 , 100 , 100 , 255 ) , 0.5f );

}

void Game::DrawCapsuleDetails()
{
	Vec2 mousePositionScreen = GetMousePositionScreenView();

	Vec2 capsuleStart = Vec2( ( float ) capsuleStartX , ( float ) capsuleStartY );
	Vec2 capsuleEnd = Vec2( ( float ) capsuleEndX , ( float ) capsuleEndY );

	OBB2 obb1 = OBB2( ( capsuleEnd - capsuleStart ) * 0.5f + capsuleStart , Vec2( ( capsuleEnd - capsuleStart ).GetLength() , capsuleRadius * 2.f ) , ( capsuleEnd - capsuleStart ).GetNormalized() );
	Vec2 cornerPoints[ 4 ];
	obb1.GetCornerPositions( cornerPoints );

	Vertex_PCU obbVertices[ 6 ];
	obbVertices[ 0 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Vec2( 0.f , 0.f ) );
	obbVertices[ 1 ] = Vertex_PCU( Vec3( cornerPoints[ 1 ].x , cornerPoints[ 1 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Vec2( 0.f , 0.f ) );
	obbVertices[ 2 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Vec2( 0.f , 0.f ) );

	obbVertices[ 3 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Vec2( 0.f , 0.f ) );
	obbVertices[ 4 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Vec2( 0.f , 0.f ) );
	obbVertices[ 5 ] = Vertex_PCU( Vec3( cornerPoints[ 3 ].x , cornerPoints[ 3 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 255 ) , Vec2( 0.f , 0.f ) );

	g_theRenderer->DrawVertexArray( 6 , obbVertices );
	g_theRenderer->DrawDisc( capsuleStart , (float)capsuleRadius , Rgba8( 100 , 0 , 0 , 255 ) );
	g_theRenderer->DrawDisc( capsuleEnd , (float)capsuleRadius , Rgba8( 100 , 0 , 0 , 255 ) );

	Vec2 nearestPoint = GetNearestPointOnCapsule2D( mousePositionScreen , capsuleStart , capsuleEnd , capsuleRadius );
	g_theRenderer->DrawLine( mousePositionScreen , nearestPoint , Rgba8( 100 , 0 , 0 , 255 ) , 0.3f );
	g_theRenderer->DrawRing( nearestPoint , 0.4f , Rgba8( 100 , 100 , 100 , 255 ) , 0.4f );

	Vertex_PCU obbVertices_highlighted[ 6 ];
	obbVertices_highlighted[ 0 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 255 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 1 ] = Vertex_PCU( Vec3( cornerPoints[ 1 ].x , cornerPoints[ 1 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 255 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 2 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 255 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 3 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 255 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 4 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 255 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 5 ] = Vertex_PCU( Vec3( cornerPoints[ 3 ].x , cornerPoints[ 3 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 255 ) , Vec2( 0.f , 0.f ) );

	if ( IsPointInsideCapsule2D( mousePositionScreen , capsuleStart , capsuleEnd , capsuleRadius ) )
	{
		g_theRenderer->DrawVertexArray( 6 , obbVertices_highlighted );
		g_theRenderer->DrawDisc( capsuleStart , ( float ) capsuleRadius , Rgba8( 200 , 100 , 0 , 255 ) );
		g_theRenderer->DrawDisc( capsuleEnd , ( float ) capsuleRadius , Rgba8( 200 , 100 , 0 , 255 ) );
		
	}

}

void Game::DrawOBDBDetails()
{
	Vec2 mousePositionScreen = GetMousePositionScreenView();
	

	OBB2 obb1 = OBB2( Vec2( 45.f , 45.f ) , Vec2( 10.f , 15.f ) , float(degrees) );

	Vec2 cornerPoints[4];

	obb1.GetCornerPositions( cornerPoints );

	Vertex_PCU obbVertices_highlighted[ 6 ];
	obbVertices_highlighted[ 0 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 100 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 1 ] = Vertex_PCU( Vec3( cornerPoints[ 1 ].x , cornerPoints[ 1 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 100 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 2 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 100 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 3 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 100 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 4 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 100 ) , Vec2( 0.f , 0.f ) );
	obbVertices_highlighted[ 5 ] = Vertex_PCU( Vec3( cornerPoints[ 3 ].x , cornerPoints[ 3 ].y , 0.f ) , Rgba8( 200 , 100 , 0 , 100 ) , Vec2( 0.f , 0.f ) );

	Vertex_PCU obbVertices[ 6 ];
	obbVertices[ 0 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
	obbVertices[ 1 ] = Vertex_PCU( Vec3( cornerPoints[ 1 ].x , cornerPoints[ 1 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
	obbVertices[ 2 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );

	obbVertices[ 3 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
	obbVertices[ 4 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
	obbVertices[ 5 ] = Vertex_PCU( Vec3( cornerPoints[ 3 ].x , cornerPoints[ 3 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );

	
	g_theRenderer->DrawVertexArray( 6 , obbVertices );

	Vec2 nearestPoint = obb1.GetNearestPoint( mousePositionScreen );

	g_theRenderer->DrawLine( mousePositionScreen , nearestPoint , Rgba8( 100 , 0 , 0 , 255 ) , 0.3f );

	if ( IsPointInsideOBB2D( mousePositionScreen , obb1 ) )
	{
		g_theRenderer->DrawVertexArray( 6 , obbVertices_highlighted );
	}

	g_theRenderer->DrawRing( nearestPoint, 0.3f , Rgba8( 100 , 0 , 0 , 255 ) , 0.5f );
}

void Game::DrawDiscDetails()
{
	Vec2 mousePositionScreen = GetMousePositionScreenView();

	Vec2 nearestPoint = GetNearstPointOnDisc2D( mousePositionScreen , Vec2( 70.f , 50.f ) , (float)discRadius );


	g_theRenderer->DrawDisc( Vec2( 70.f , 50.f ),(float)discRadius , Rgba8( 0 , 0 , 100 , 100 ) );

	g_theRenderer->DrawLine( mousePositionScreen , nearestPoint , Rgba8( 0 , 0 , 100 , 255 ) , 0.3f );

	if ( IsPointInsideDisc2D( mousePositionScreen , Vec2( 70.f , 50.f ) , ( float ) discRadius ) )
	{
		g_theRenderer->DrawDisc( Vec2( 70.f , 50.f ) , ( float ) discRadius , Rgba8( 0 , 100 , 100 , 100 ) );
	}

	g_theRenderer->DrawRing( nearestPoint , 0.3f , Rgba8( 100 , 100 , 100 , 255 ) , 0.4f );


}

Vec2 Game::GetMousePositionScreenView()
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	Vec2 mousePositionScreen;

	mousePositionScreen.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	mousePositionScreen.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	return mousePositionScreen;
}

void Game::UpdateCamera()
{
	
}

void Game::ToggleDebugCamera()
{

}

void Game::TogglePaused()
{
	
}

void Game::ToggleDebugging()
{
	
}

































