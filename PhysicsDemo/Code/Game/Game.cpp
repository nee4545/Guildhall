#include "Game/Game.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Core/Polygon2D.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"

#define UNUSED(x) (void)(x);


bool Help( EventArgs& args )
{
	UNUSED( args );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Currently Supported Commands" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "help" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "quit" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "close" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "set_physics_update_freq  Usage Ex: set_physics_update_freq:100" );
	return false;
}

bool Quit( EventArgs& args )
{
	UNUSED( args );
	g_theapp->HandleQuitRequested();
	return false;
}


bool Close( EventArgs& args )
{
	UNUSED( args );
	g_theConsole.SetIsOpen( false );
	return false;
}

bool SetPhysicsUpdateStep( EventArgs& args )
{
	float dt = args.GetValue("set_physics_update_freq",0.f);
	physicsSystem->SetFixedStepTime( ( double ) 1/dt );
	return false;
}



Game::Game()
{
	m_camera = new Camera();
	m_devConsoleCamera = new Camera();
	//m_camera->m_transform.m_position = Vec3( 0.f , 0.f , -1.f );
	m_camera->SetOrthoView(Vec2(-80.f,-45.f), Vec2( 80.f , 45.f ) );
	m_camera->SetOutputsize( Vec2( 160.f , 90.f ) );
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );
	m_devConsoleCamera->SetClearMode( 0|CLEAR_STENCIL_BIT|CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 0 ) );
	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 2.5f );

	g_theEventSystem.SubscribeToEvent( "help" , Help );
	g_theEventSystem.SubscribeToEvent( "quit" , Quit );
	g_theEventSystem.SubscribeToEvent( "close" , Close );
	g_theEventSystem.SubscribeToEvent( "set_physics_update_freq" , SetPhysicsUpdateStep );

	m_cameraHeight = 90.f;
	m_camera->SetClearMode( CLEAR_COLOR_BIT , Rgba8( 0 , 0 , 0 , 255 ) );
	//m_devConsoleCamera->SetClearMode( 0 , Rgba8(0,0,0,127) );
	//m_camera->SetPosition( Vec2( 0.f , 0.f ) );
	m_maxZoomIn = 0.5f * m_cameraHeight;
	m_maxZoomOut = 2.f * m_cameraHeight;
	m_rng = RandomNumberGenerator();
	StartUp();
	PopulateInitialObjects();
	m_BitmapFont = g_theRenderer->CreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );


}

Game::~Game()
{
	delete m_camera;
	delete m_devConsoleCamera;
}

void Game::StartUp()
{

	physicsSystem = new Physics2D();
	physicsSystem->StartUp();

}

void Game::BeginFrame()
{
	physicsSystem->BeginFrame();
}

void Game::EndFrame()
{
	physicsSystem->EndFrame();
}

GameObject* Game::CreateDisc()
{
	GameObject* obj = new GameObject();
	
	Vec2 mousePos = m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );

	obj->m_rigidbody = physicsSystem->CreateRigidbody();
	obj->m_rigidbody->SetPosition( mousePos );

	float discRadius = m_rng.RollRandomFloatInRange( 1.5f , 6.5f );
	DiscCollider2D* collider = physicsSystem->CreateDiscCollider( Vec2(0.f,0.f) , discRadius );
	obj->m_rigidbody->TakeCollider( collider );

	obj->m_rigidbody->m_collider->CalculateMoment();
	m_gameObjects.push_back( obj );

	return obj;

}

GameObject* Game::CreatePolygon(Polygon2D& polygon)
{
	GameObject* obj = new GameObject();

	Vec2 mousePos = m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );

	obj->m_rigidbody = physicsSystem->CreateRigidbody();

	PolygonCollider2D* collider = physicsSystem->CreatePolygonCollider( mousePos , &polygon );

	obj->m_rigidbody->TakeCollider( collider );
	obj->m_rigidbody->SetPosition( mousePos );

	obj->m_rigidbody->m_collider->CalculateMoment();
	m_gameObjects.push_back( obj );
	

	return obj;
}

void Game::PopulateInitialObjects()
{
	GameObject* obj1 = new GameObject();
	obj1->m_rigidbody = physicsSystem->CreateRigidbody();
	obj1->m_rigidbody->SetPosition( Vec2( 20.f , 20.f ) );

	DiscCollider2D* collider1 = physicsSystem->CreateDiscCollider( Vec2( 0.f , 0.f ) , 4.f );
	obj1->m_rigidbody->TakeCollider( collider1 );
	obj1->m_rigidbody->m_collider->CalculateMoment();
	m_gameObjects.push_back( obj1 );
}


void Game::UpdateCameraMovement(float deltaSeconds )
{
	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		m_camera->Translate2D( Vec2( 0.f , 1.f ) * m_cameraMovementSpeed* deltaSeconds );
	}

	if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		m_camera->Translate2D( Vec2( 0.f , -1.f ) * m_cameraMovementSpeed * deltaSeconds );
	}

	if ( g_theInput->IsKeyPressed( 'A' ) )
	{
		m_camera->Translate2D( Vec2( -1.f , 0.f ) * m_cameraMovementSpeed * deltaSeconds );
	}

	if ( g_theInput->IsKeyPressed( 'D' ) )
	{
		m_camera->Translate2D( Vec2( 1.f , 0.f ) * m_cameraMovementSpeed * deltaSeconds );
	}
}

void Game::HandleMouseInsideObjects()
{
	Vec2 mousePos = m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );

	for ( int index = 0; index < m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[ index ] == nullptr )
		{
			continue;
		}
		if ( m_gameObjects[ index ]->m_rigidbody->m_collider->Contains( mousePos ) )
		{
			m_gameObjects[ index ]->isMouseInside = true;
		}
		else
		{
			m_gameObjects[ index ]->isMouseInside = false;
		}
	}
}

void Game::HandleCollissions()
{
	for ( int i = 0; i < m_gameObjects.size(); i++ )
	{
		if ( m_gameObjects[ i ] == nullptr )
		{
			continue;
		}
		
		for (int j = 0; j < m_gameObjects.size(); j++ )
		{
			if ( m_gameObjects[ j ] == nullptr )
			{
				continue;
			}
			
			if ( m_gameObjects[ i ] != m_gameObjects[ j ] )
			{
				if ( m_gameObjects[ i ]->m_rigidbody->m_collider->Intersects( m_gameObjects[ j ]->m_rigidbody->m_collider ) )
				{
					m_gameObjects[ i ]->isColliding = true;
					break;
				}
				else
				{
					m_gameObjects[ i ]->isColliding = false;
				}
			}
		}
	}
}

void Game::HandleDrag()
{
	GetCurrentSelectedObject();
	Vec2 mousePos = m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );

	if ( isDrawing )
	{
		return;
	}

	if ( m_selectedObject != nullptr )
	{
		if ( g_theInput->IsLeftMouseButtonPressed() )
		{
			
			m_dragInProgress = true;
			m_selectedObject->isBeingDragged = true;
			m_selectedObject->m_rigidbody->enableSimulation = false;

			if ( g_theInput->WasKeyJustPressed( '1' ) )
			{
				m_selectedObject->m_rigidbody->SetSimulationMode( STATIC );
			}

			if ( g_theInput->WasKeyJustPressed( '2' ) )
			{
				m_selectedObject->m_rigidbody->SetSimulationMode( KINAMETIC );
			}

			if ( g_theInput->WasKeyJustPressed( '3' ) )
			{
				m_selectedObject->m_rigidbody->SetSimulationMode( DYNAMIC );
			}

			if ( g_theInput->WasKeyJustPressed( 'H' ) )
			{
				m_selectedObject->m_rigidbody->m_collider->IncreamentBounciness( 0.1f );
			}

			if ( g_theInput->WasKeyJustPressed( 'G' ) )
			{
				m_selectedObject->m_rigidbody->m_collider->DecreamentBounciness( 0.1f );
			}

			if ( g_theInput->WasKeyJustPressed( 'J' ) )
			{
				m_selectedObject->m_rigidbody->m_collider->DecreamentFriction( 0.1f );
			}

			if ( g_theInput->WasKeyJustPressed( 'K' ) )
			{
				m_selectedObject->m_rigidbody->m_collider->IncreamentFriction( 0.1f );
			}

			if ( g_theInput->WasKeyJustPressed( 'N' ) )
			{
				m_selectedObject->m_rigidbody->DecreamentMass( 0.1f );
			}

			if ( g_theInput->WasKeyJustPressed( 'M' ) )
			{
				m_selectedObject->m_rigidbody->IncreamentMass( 0.1f );
			}

			if ( g_theInput->WasKeyJustPressed( 'Z' ) )
			{
				m_selectedObject->m_rigidbody->DecreamentDrag( 0.1f );
			}

			if ( g_theInput->WasKeyJustPressed( 'X' ) )
			{
				m_selectedObject->m_rigidbody->IncreamentDrag( 0.1f );
			}

			if ( g_theInput->IsKeyPressed( 'R' ))
			{
				m_selectedObject->m_rigidbody->Rotate(1.f);
			}

			if ( g_theInput->IsKeyPressed( 'F' ) )
			{
				m_selectedObject->m_rigidbody->Rotate( -1.f );
			}

			if ( g_theInput->WasKeyJustPressed( 'T' ) )
			{
				m_selectedObject->m_rigidbody->m_angularVelocity+=1.f;
			}

			if ( g_theInput->WasKeyJustPressed( 'Y' ) )
			{
				m_selectedObject->m_rigidbody->m_angularVelocity -= 1.f;
			}

			if ( g_theInput->WasKeyJustPressed( 'U' ) )
			{
				m_selectedObject->m_rigidbody->m_angularVelocity = 0.f;
			}

			if ( g_theInput->WasKeyJustPressed( 'C' ) )
			{
				m_selectedObject->m_rigidbody->DecreamnetAngularDrag(0.1f);
			}

			if ( g_theInput->WasKeyJustPressed( 'V' ) )
			{
				m_selectedObject->m_rigidbody->IncreamentAngularDrag(0.1f);
			}

			
			
			if ( !m_offsetSet )
			{
				m_offset = mousePos - m_selectedObject->m_rigidbody->GetWorldPosition();
				m_offsetSet = true;
			}

			m_selectedObject->m_rigidbody->SetPosition( mousePos-m_offset );


			

			if ( g_theInput->WasKeyJustPressed( 0x08 ) || g_theInput->WasKeyJustPressed( 0x2E ) )
			{
				
				for ( int i = 0; i < m_gameObjects.size(); i++ )
				{
					if ( m_gameObjects[ i ] == m_selectedObject )
					{
						delete m_gameObjects[ i ];
						m_gameObjects[ i ] = nullptr;
						m_selectedObject = nullptr;
						m_dragInProgress = false;
						break;
					}
				}
			}

			HandleThrow();

		}
		else
		{
			m_dragInProgress = false;
			m_selectedObject->isBeingDragged = false;
			m_selectedObject->m_rigidbody->enableSimulation = true;
			m_offsetSet = false;
		}
	}
}

void Game::HandleObjectCreationRequests()
{
	Vec2 mousePos = m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		CreateDisc();
	}
}

void Game::HandleThrow()
{
	if ( !initialPointSet || !finalPointSet || m_selectedObject==nullptr)
	{
		return;
	}

	Vec2 velocity = -throwFinalPoint + throwInitialPoint;
	m_selectedObject->m_rigidbody->SetVelocity( velocity * 10.f );
	initialPointSet = false;
	finalPointSet = false;
}

void Game::ResetCamera()
{
	m_camera->SetPosition( Vec2( 0.f , 0.f ) );
}

void Game::GetCurrentSelectedObject()
{
	if ( m_dragInProgress )
	{
		return;
	}

	Vec2 mousePos = m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );

	for( int index =(int) m_gameObjects.size() - 1; index >= 0; index-- )
	{
		if ( m_gameObjects[ index ] == nullptr )
		{
			continue;
		}
		if ( m_gameObjects[ index ]->m_rigidbody->m_collider->Contains( mousePos ) )
		{
			m_selectedObject = m_gameObjects[ index ];
			return;
		}
	}

	m_selectedObject = nullptr;
}

void Game::ZoomInCamera( float deltaSeconds )
{
	if ( m_cameraHeight > m_maxZoomIn )
	{
		m_cameraHeight -= deltaSeconds * m_zoomspeed;
	}

	m_camera->SetProjectionOrthographic( m_cameraHeight );

	/*m_camera->m_transform.m_scale.x += deltaSeconds;
	m_camera->m_transform.m_scale.y += deltaSeconds;*/

}

void Game::ZoomOutCamera( float deltaSeconds )
{
	if ( m_cameraHeight < m_maxZoomOut )
	{
		m_cameraHeight += deltaSeconds * m_zoomspeed;
	}

	m_camera->SetProjectionOrthographic( m_cameraHeight );

	/*m_camera->m_transform.m_scale.x -= deltaSeconds;
	m_camera->m_transform.m_scale.y -= deltaSeconds;*/
}

void Game::PolygonDrawMode()
{
	if ( !isDrawing )
	{
		return;
	}

	isDrawing = true;
	
	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		if ( drawModePoints.size() == 1 )
		{
			drawModePoints.push_back( m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() ) );
			return;
		}

		Vec2 point = m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );
		if ( IsPolygonPotentiallyConvex( point ) )
		{
			drawModePoints.push_back( point );
			isThereInvalidPoint = false;
		}
		else
		{
			invalidPoint = point;
			isThereInvalidPoint = true;
		}
	}

	if ( g_theInput->WasKeyJustPressed( 0x08 ) || g_theInput->WasKeyJustPressed( 0x2E ) )
	{
		if ( isThereInvalidPoint )
		{
			isThereInvalidPoint = false;
		}
		else
		{
			if ( drawModePoints.size() > 0 )
			{
				drawModePoints.pop_back();
			}
		}
	}

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		if ( drawModePoints.size() < 3 || isThereInvalidPoint )
		{
			drawModePoints.clear();
			isThereInvalidPoint = false;
			isDrawing = false;
			return;
		}

		Vec2* points = new Vec2[ drawModePoints.size() ];
		for ( int index = 0; index < drawModePoints.size(); index++ )
		{
			points[ index ] = drawModePoints[ index ];
		}
		Polygon2D* poly = new Polygon2D();
		*poly = Polygon2D::MakeFromLineLoop( points , (unsigned int)drawModePoints.size() );

		CreatePolygon( *poly );

		isDrawing = false;
		drawModePoints.clear();
	}

	if ( g_theInput->WasKeyJustPressed( 0x1B ) )
	{
		isDrawing = false;
		drawModePoints.clear();
	}


}

bool Game::IsPolygonPotentiallyConvex( Vec2 newAddedPoint )
{
	Polygon2D polygon;
	polygon.m_points = drawModePoints;
	polygon.m_points.push_back( newAddedPoint );

	return polygon.IsConvex();

}

void Game::HandlePolygonDrawMode()
{
	if ( m_dragInProgress )
	{
		return;
	}

	if ( g_theInput->WasKeyJustPressed( '2' ) )
	{
		isDrawing = true;
		Vec2 point = m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );
		drawModePoints.clear();
		isThereInvalidPoint = false;
		drawModePoints.push_back( point );
	}

	PolygonDrawMode();

}

void Game::DrawModeRender()
{
	if ( !isDrawing )
	{
		return;
	}

	for ( int index = 0; index < drawModePoints.size(); index++ )
	{
		g_theRenderer->DrawDisc( drawModePoints[ index ] , 0.3f , Rgba8( 100 , 100 , 100 , 255 ));
	}

	if ( drawModePoints.size() > 1 )
	{
		for ( int index = 0; index < drawModePoints.size() - 1; index++ )
		{
			g_theRenderer->DrawLine( drawModePoints[ index ] , drawModePoints[ index + 1 ] , Rgba8( 0 , 0 , 100 , 128 ) , 0.15f );
		}
	}

	if ( isThereInvalidPoint )
	{
		g_theRenderer->DrawLine( drawModePoints[ drawModePoints.size() - 1 ] , invalidPoint , Rgba8( 100 , 0 , 0 , 128 ) , 0.15f );
	}

	Polygon2D polygon;
	polygon.m_points = drawModePoints;

	g_theRenderer->DrawDisc( polygon.GetCentre() , polygon.GetBoundingDiscRadius(), Rgba8(100,0,0,100) );

	
}

void Game::HandleBounceAndWrapAround()
{
	for ( int index = 0; index < m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[ index ] == nullptr )
		{
			continue;
		}

		if ( m_gameObjects[ index ]->m_rigidbody->m_collider->m_colliderType == COLLIDER2D_DISC )
		{
			DiscCollider2D* temp = ( DiscCollider2D* ) m_gameObjects[ index ]->m_rigidbody->m_collider;

			if ( m_gameObjects[ index ]->m_rigidbody->m_worldPosition.y - temp->m_radius < m_camera->GetOrthoBottomLeft().y )
			{
				m_gameObjects[ index ]->m_rigidbody->ReverseVelocityYAxis();
			}

			if ( m_gameObjects[ index ]->m_rigidbody->m_worldPosition.x - temp->m_radius > m_camera->GetOrthoTopRight().x )
			{
				m_gameObjects[ index ]->m_rigidbody->m_worldPosition.x = m_camera->GetOrthoBottomLeft().x - temp->m_radius;
			}

			if ( m_gameObjects[ index ]->m_rigidbody->m_worldPosition.x + temp->m_radius < m_camera->GetOrthoBottomLeft().x )
			{
				m_gameObjects[ index ]->m_rigidbody->m_worldPosition.x = m_camera->GetOrthoTopRight().x + temp->m_radius;
			}
		}

		if ( m_gameObjects[ index ]->m_rigidbody->m_collider->m_colliderType == COLLIDER2D_POLYGON )
		{
			PolygonCollider2D* temp = ( PolygonCollider2D* ) m_gameObjects[ index ]->m_rigidbody->m_collider;

			if ( temp->m_polygonLocal->GetBottomMostEdge().y < m_camera->GetOrthoBottomLeft().y )
			{
				m_gameObjects[ index ]->m_rigidbody->ReverseVelocityYAxis();
			}

			if ( temp->m_polygonLocal->GetLeftMostEdge().x > m_camera->GetOrthoTopRight().x )
			{
				m_gameObjects[ index ]->m_rigidbody->m_worldPosition.x = m_camera->GetOrthoBottomLeft().x - ( temp->m_polygonLocal->GetRightMostEdge().x - temp->m_polygonLocal->m_localPos.x );
			}

			if ( temp->m_polygonLocal->GetRightMostEdge().x < m_camera->GetOrthoBottomLeft().x )
			{
				m_gameObjects[ index ]->m_rigidbody->m_worldPosition.x = m_camera->GetOrthoTopRight().x - ( temp->m_polygonLocal->GetLeftMostEdge().x - temp->m_polygonLocal->m_localPos.x );
			}
		}

	}
}

void Game::HandleClockChanges()
{
	if ( g_theInput->WasKeyJustPressed( 'P' ) )
	{
		if ( physicsSystem->m_clock->IsPaused() )
		{
			physicsSystem->m_clock->isPaused = false;
		}
		else
		{
			physicsSystem->m_clock->isPaused = true;
		}
	}

	if ( g_theInput->WasKeyJustPressed( '0' ) )
	{
		physicsSystem->m_clock->SetScale( 1.0 );
		physicsSystem->m_clock->isPaused = false;
	}

	if ( g_theInput->WasKeyJustPressed( '8' ) )
	{
		physicsSystem->m_clock->SetScale( 2.0 * physicsSystem->m_clock->m_scale );
	}

	if ( g_theInput->WasKeyJustPressed( '9' ) )
	{
		physicsSystem->m_clock->SetScale( 0.5 * physicsSystem->m_clock->m_scale );
	}
}

void Game::DisplayMiscInfo()
{
	AABB2 aabb = AABB2( m_camera->GetOrthoBottomLeft() , m_camera->GetOrthoTopRight() );
	aabb.CarveBoxOffTop( 0.2f );


	std::string data = "Current Gravity: ";
	std::string gravity = "";
	gravity =data + std::to_string(physicsSystem->m_gravityMultiplier);

	std::vector<Vertex_PCU> textVerts;

	m_BitmapFont->AddVertsForTextInBox2D( textVerts , aabb , 2.f , gravity , Rgba8( 100 , 0 , 100 , 255 ) , 1.f , Vec2( 0.9f , 0.9f ));
	
	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( textVerts );
	g_theRenderer->BindTexture( nullptr );


	std::vector<Vertex_PCU> timeScaleInfo;
	std::vector<Vertex_PCU> clockPausedInfo;

	std::string scaleStr = "Time Scale :";
	scaleStr = scaleStr + std::to_string( physicsSystem->m_clock->m_scale );

	m_BitmapFont->AddVertsForTextInBox2D( timeScaleInfo , aabb , 2.f , scaleStr , Rgba8( 100 , 0 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.9f ));

	
	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( timeScaleInfo );
	g_theRenderer->BindTexture( nullptr );


	std::string clockStr = "Clock Status :";

	if ( physicsSystem->m_clock->IsPaused() )
	{
		clockStr += "Paused";
	}
	else
	{
		clockStr += "Running";
	}

	m_BitmapFont->AddVertsForTextInBox2D( clockPausedInfo , aabb , 2.f , clockStr , Rgba8( 100 , 0 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.7f ) );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( clockPausedInfo );
	g_theRenderer->BindTexture( nullptr );

}



void Game::DisplayX()
{
	std::string x = "X";
	std::vector<Vertex_PCU> textVerts;
	Vec2 offset = Vec2( .9f , 0.f );
	offset.RotateDegrees(45.f);
	


	for ( int index = 0; index < m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[ index ] == nullptr )
		{
			continue;
		}

		if ( m_gameObjects[ index ]->m_rigidbody->enableSimulation )
		{
			if ( m_gameObjects[ index ]->m_rigidbody->m_collider->m_colliderType == COLLIDER2D_DISC )
			{
				DiscCollider2D* temp = ( DiscCollider2D* ) m_gameObjects[ index ]->m_rigidbody->m_collider;
				m_BitmapFont->AddVertsForText2D( textVerts , temp->m_worldPosition-offset , 1.f , x , Rgba8( 0 , 0 , 255 , 255 ) );

				g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
				g_theRenderer->DrawVertexArray( textVerts );
				g_theRenderer->BindTexture( nullptr );
			}

			if ( m_gameObjects[ index ]->m_rigidbody->m_collider->m_colliderType == COLLIDER2D_POLYGON )
			{
				PolygonCollider2D* temp = ( PolygonCollider2D* ) m_gameObjects[ index ]->m_rigidbody->m_collider;
				m_BitmapFont->AddVertsForText2D( textVerts , temp->m_polygonLocal->GetCentre() , 1.f , x , Rgba8( 0 , 0 , 255 , 255 ) );
				g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
				g_theRenderer->DrawVertexArray( textVerts );
				g_theRenderer->BindTexture( nullptr );
			}
			
		}
		else
		{
			if ( m_gameObjects[ index ]->m_rigidbody->m_collider->m_colliderType == COLLIDER2D_DISC )
			{
				DiscCollider2D* temp = ( DiscCollider2D* ) m_gameObjects[ index ]->m_rigidbody->m_collider;
				m_BitmapFont->AddVertsForText2D( textVerts , temp->m_worldPosition-offset , 1.f , x , Rgba8( 100 , 0 , 0 , 255 ) );

				g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
				g_theRenderer->DrawVertexArray( textVerts );
				g_theRenderer->BindTexture( nullptr );
			}

			if ( m_gameObjects[ index ]->m_rigidbody->m_collider->m_colliderType == COLLIDER2D_POLYGON )
			{
				PolygonCollider2D* temp = ( PolygonCollider2D* ) m_gameObjects[ index ]->m_rigidbody->m_collider;
				m_BitmapFont->AddVertsForText2D( textVerts , temp->m_polygonLocal->GetCentre() , 1.f , x , Rgba8( 100 , 0 , 0 , 255 ) );

				g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
				g_theRenderer->DrawVertexArray( textVerts );
				g_theRenderer->BindTexture( nullptr );
			}
		}
	}
}

void Game::HandleGravityModification()
{
	if ( g_theInput->WasKeyJustPressed( 0x6B ) )
	{
		physicsSystem->m_gravityMultiplier += 0.1f;
	}

	if ( g_theInput->WasKeyJustPressed( 0x6D ) )
	{
		physicsSystem->m_gravityMultiplier -= 0.1f;
	}
}

void Game::DisplayToolTip()
{
	Vec2 mousePos = m_camera->ClientToWordPosition2D( g_theInput->GetCurrentMousePosition() );

	AABB2 tooltipBox = AABB2( mousePos , Vec2( mousePos.x + 50.f , mousePos.y + 25.f ) );

	g_theRenderer->DrawAABB2D( tooltipBox , Rgba8( 100 , 100 , 100 , 200 ) );


	std::vector<Vertex_PCU> massInfo;
	std::vector<Vertex_PCU> simulationModeInfo;
	std::vector<Vertex_PCU> velocityInfo;
	std::vector<Vertex_PCU> verletVelocityInfo;
	std::vector<Vertex_PCU> bounceInfo;
	std::vector<Vertex_PCU> frictionInfo;
	std::vector<Vertex_PCU> dragInfo;
	std::vector<Vertex_PCU> roationInfo;
	std::vector<Vertex_PCU> angualrVelocityInfo;
	std::vector<Vertex_PCU> momentInfo;
	std::vector<Vertex_PCU> angularDragInfo;

	std::string temp = "";
	
	std::string massStr = "Mass:";
	std::string frictionStr = "Friction:";
	std::string bounceStr = "Bounciness:";
	std::string velocityStr = "Velocity:";
	std::string verletStr = "Verlet Velocity:";
	std::string simStr = "Simulation Mode: ";
	std::string dragStr = "Drag: ";
	std::string rotationStr = "Rotation: ";
	std::string angularvStr = "Angular Velocity:";
	std::string momentStr = "Moment :";
	std::string angularDragStr = "Angular Drag: ";

	temp = std::to_string(m_selectedObject->m_rigidbody->m_mass);
	massStr += temp;
	massStr += "(N,M->dec/incr)";

	temp = "";
	temp = std::to_string( m_selectedObject->m_rigidbody->m_collider->m_material.friction );
	frictionStr += temp;
	frictionStr+="(J,K->dec/incr)";

	temp = "";
	temp = std::to_string( m_selectedObject->m_rigidbody->m_collider->m_material.bounciness );
	bounceStr += temp;
	bounceStr += "(G,H->dec/incr)";

	temp = "";
	temp = std::to_string( m_selectedObject->m_rigidbody->m_velocity.x ) + ","+std::to_string( m_selectedObject->m_rigidbody->m_velocity.y );
	velocityStr += temp;

	temp = "";
	temp = std::to_string( m_selectedObject->m_rigidbody->m_verletVelocity.x ) + "," + std::to_string( m_selectedObject->m_rigidbody->m_verletVelocity.y );
	verletStr += temp;

	temp = "";
	temp = std::to_string( ConvertRadiansToDegrees( m_selectedObject->m_rigidbody->m_rotationInRadians ) );
	rotationStr += temp;
	rotationStr += "(R,F->dec/incr)";

	temp = "";
	temp = std::to_string( ConvertRadiansToDegrees( m_selectedObject->m_rigidbody->m_angularVelocity ) );
	angularvStr += temp;
	angularvStr += "(T,Y,U->dec/incr/Reset)";

	temp = "";
	temp = std::to_string( m_selectedObject->m_rigidbody->m_moment );
	momentStr += temp;

	temp = "";
	temp = std::to_string( m_selectedObject->m_rigidbody->m_moment );
	momentStr += temp;

	temp = "";
	temp = std::to_string( m_selectedObject->m_rigidbody->m_angularDrag );
	angularDragStr += temp;
	angularDragStr += "(C,V->dec/incr)";

	switch ( m_selectedObject->m_rigidbody->m_mode )
	{
	case STATIC:
		temp = "STATIC";
		break;
	case KINAMETIC:
		temp = "KINAMETIC";
		break;
	case DYNAMIC:
		temp = "DYNAMIC";
		break;
	}
	simStr += temp;


	temp = "";
	temp = std::to_string( m_selectedObject->m_rigidbody->m_drag );
	dragStr += temp;
	dragStr += "(Z,X->dec/incr)";

	m_BitmapFont->AddVertsForTextInBox2D( massInfo , tooltipBox , 1.f , massStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.0f ));
	m_BitmapFont->AddVertsForTextInBox2D( frictionInfo , tooltipBox , 1.f , frictionStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.1f ) );
	m_BitmapFont->AddVertsForTextInBox2D( bounceInfo , tooltipBox , 1.f , bounceStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.2f ) );
	m_BitmapFont->AddVertsForTextInBox2D( velocityInfo , tooltipBox , 1.f , velocityStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.3f ) );
	m_BitmapFont->AddVertsForTextInBox2D( verletVelocityInfo , tooltipBox , 1.f , verletStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.4f ) );
	m_BitmapFont->AddVertsForTextInBox2D( dragInfo , tooltipBox , 1.f , dragStr , Rgba8( 0 , 0 , 0 , 255 ), 1.f , Vec2( 0.02f , 0.5f ));
	m_BitmapFont->AddVertsForTextInBox2D( simulationModeInfo , tooltipBox , 1.f , simStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.6f ) );
	m_BitmapFont->AddVertsForTextInBox2D( roationInfo , tooltipBox , 1.f , rotationStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.7f ) );
	m_BitmapFont->AddVertsForTextInBox2D( angualrVelocityInfo , tooltipBox , 1.f , angularvStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.8f ) );
	m_BitmapFont->AddVertsForTextInBox2D( momentInfo , tooltipBox , 1.f , momentStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.9f ) );
	m_BitmapFont->AddVertsForTextInBox2D( angularDragInfo , tooltipBox , 1.f , angularDragStr , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.99f ) );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( massInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( frictionInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( bounceInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( velocityInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( verletVelocityInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( dragInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( simulationModeInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( roationInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( angualrVelocityInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( momentInfo );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( angularDragInfo );
	g_theRenderer->BindTexture( nullptr );

}

void Game::UpdateFramePositions( )
{

	m_frameCounter++;
	if ( m_frameCounter > 11 )
	{
		m_frameCounter = 0;
	}

	if ( m_selectedObject == nullptr )
	{
		throwInitialPoint = Vec2( 0.f , 0.f );
		throwFinalPoint = Vec2( 0.f , 0.f );
		initialPointSet = false;
		finalPointSet = false;
		return;
	}
	
	if ( m_frameCounter == 0 )
	{
		if ( m_selectedObject != nullptr  )
		{
			
				throwInitialPoint = m_selectedObject->m_rigidbody->m_worldPosition;
				initialPointSet = true;
		}
	}

	if ( m_frameCounter == 10 )
	{
		if ( m_selectedObject != nullptr )
		{
				throwFinalPoint = m_selectedObject->m_rigidbody->m_worldPosition;
				finalPointSet = true;
		}
	}


}

void Game::Update( float deltaseconds )
{

	ToggleDevConsole();
	if ( g_theConsole.IsOpen() )
	{
		return;
	}

	HandleClockChanges();

	if ( !isDrawing )
	{
		if ( g_theInput->WasKeyJustPressed( 0x1B ) )
		{
			g_theapp->HandleQuitRequested();
		}
	}

	UpdateCameraMovement( deltaseconds );

	physicsSystem->Update();
	if ( !isDrawing )
	{
		HandleObjectCreationRequests();
	}
	
	HandlePolygonDrawMode();
	

	if ( g_theInput->GetMouseWheelData()>0 )
	{
		ZoomInCamera( deltaseconds );
	}

	if ( g_theInput->GetMouseWheelData()<0 )
	{
		ZoomOutCamera( deltaseconds );
	}


	HandleMouseInsideObjects();
	HandleDrag();
	UpdateFramePositions();

	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		ResetCamera();
	}

	for ( int index = 0; index < m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[ index ] == nullptr )
		{
			continue;
		}
		m_gameObjects[ index ]->m_rigidbody->m_collider->UpdateWorldShape();
		m_gameObjects[ index ]->UpdateColorsBasedOnStatus();
	}

	HandleGravityModification();
	HandleCollissions();
	HandleBounceAndWrapAround();

}

void Game::Render()
{

	
	g_theRenderer->BeginCamera( *m_camera );
	
	
	DrawModeRender();
	for ( int index = 0; index < m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[ index ] == nullptr )
		{
			continue;
		}
		m_gameObjects[ index ]->m_rigidbody->m_collider->DebugRender( g_theRenderer , m_gameObjects[index]->m_borderColor , m_gameObjects[index]->m_fillColor  );
	}

	DisplayMiscInfo();
	DisplayX();

	if ( m_selectedObject )
	{
		DisplayToolTip();
	}
	

	g_theRenderer->EndCamera( *m_camera );

	if ( g_theConsole.IsOpen() )
	{
		g_theConsole.Render( *g_theRenderer , *m_devConsoleCamera , 2.5f , 1.5f );
	}

	DebugRenderSystem::sDebugRenderer->DebugRenderWorldToCamera( m_camera );
	
}

void Game::ToggleDevConsole()
{
	if ( g_theInput->WasKeyJustPressed( 0xC0 ) )
	{
		devConsoleOpen = !devConsoleOpen;
		g_theConsole.SetIsOpen( devConsoleOpen );
	}

	/*if ( g_theConsole.IsOpen() )
	{
		physicsSystem->m_clock->isPaused = true;
	}
	else
	{
		physicsSystem->m_clock->isPaused = false;
	}*/
}
