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




Game::Game()
{
	m_camera = new Camera();
	m_camera->SetOutputsize( Vec2( 160.f , 90.f ) );
	m_cameraHeight = 90.f;
	m_camera->SetPosition( Vec2( 0.f , 0.f ) );
	m_maxZoomIn = 0.1f * m_cameraHeight;
	m_maxZoomOut = 3.f * m_cameraHeight;
	m_rng = RandomNumberGenerator();
	StartUp();
	PopulateInitialObjects();
	m_BitmapFont = g_theRenderer->CreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );


}

void Game::StartUp()
{

	m_physicsSystem = new Physics2D();

}

void Game::EndFrame()
{
	m_physicsSystem->EndFrame();
}

GameObject* Game::CreateDisc()
{
	GameObject* obj = new GameObject();
	
	Vec2 mousePos = m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );

	obj->m_rigidbody = m_physicsSystem->CreateRigidbody();
	obj->m_rigidbody->SetPosition( mousePos );

	float discRadius = m_rng.RollRandomFloatInRange( 1.5f , 6.5f );
	DiscCollider2D* collider = m_physicsSystem->CreateDiscCollider( Vec2(0.f,0.f) , discRadius );
	obj->m_rigidbody->TakeCollider( collider );

	m_gameObjects.push_back( obj );

	return obj;

}

GameObject* Game::CreatePolygon(Polygon2D& polygon)
{
	GameObject* obj = new GameObject();

	Vec2 mousePos = m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );

	obj->m_rigidbody = m_physicsSystem->CreateRigidbody();

	PolygonCollider2D* collider = m_physicsSystem->CreatePolygonCollider( mousePos , &polygon );

	obj->m_rigidbody->TakeCollider( collider );
	obj->m_rigidbody->SetPosition( mousePos );

	m_gameObjects.push_back( obj );

	return obj;
}

void Game::PopulateInitialObjects()
{
	GameObject* obj1 = new GameObject();
	obj1->m_rigidbody = m_physicsSystem->CreateRigidbody();
	obj1->m_rigidbody->SetPosition( Vec2( 20.f , 20.f ) );

	DiscCollider2D* collider1 = m_physicsSystem->CreateDiscCollider( Vec2( 0.f , 0.f ) , 4.f );
	obj1->m_rigidbody->TakeCollider( collider1 );

	m_gameObjects.push_back( obj1 );

	GameObject* obj2 = new GameObject();
	obj2->m_rigidbody = m_physicsSystem->CreateRigidbody();
	obj2->m_rigidbody->SetPosition( Vec2( 0.f , 0.f ) );

	DiscCollider2D* collider2 = m_physicsSystem->CreateDiscCollider( Vec2( 0.f , 0.f ) , 3.f );
	obj2->m_rigidbody->TakeCollider( collider2 );

	m_gameObjects.push_back( obj2 );

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
	Vec2 mousePos = m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );

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
	Vec2 mousePos = m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );

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

			if ( g_theInput->WasKeyJustPressed( 'G' ) )
			{
				m_selectedObject->m_rigidbody->m_collider->IncreamentBounciness( 0.1f );
			}

			if ( g_theInput->WasKeyJustPressed( 'H' ) )
			{
				m_selectedObject->m_rigidbody->m_collider->DecreamentBounciness( 0.1f );
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
	Vec2 mousePos = m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		CreateDisc();
	}
}

void Game::HandleThrow()
{
	if ( !initialPointSet || !finalPointSet )
	{
		return;
	}

	Vec2 velocity = -throwFinalPoint+throwInitialPoint;
	m_selectedObject->m_rigidbody->SetVelocity( velocity * 25.f );
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

	Vec2 mousePos = m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );

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

	m_camera->SetProjectionOrthographic(m_cameraHeight );
	
}

void Game::ZoomOutCamera( float deltaSeconds )
{
	if ( m_cameraHeight < m_maxZoomOut )
	{
		m_cameraHeight += deltaSeconds * m_zoomspeed;
	}

	m_camera->SetProjectionOrthographic( m_cameraHeight );
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
			drawModePoints.push_back( m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() ) );
			return;
		}

		Vec2 point = m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );
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
		Vec2 point = m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );
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

void Game::DisplayGravityInfo()
{
	AABB2 aabb = AABB2( m_camera->GetOrthoBottomLeft() , m_camera->GetOrthoTopRight() );
	aabb.CarveBoxOffTop( 0.2f );


	std::string data = "Current Gravity: ";
	std::string gravity = "";
	gravity =data + std::to_string(m_physicsSystem->m_gravityMultiplier);

	std::vector<Vertex_PCU> textVerts;

	m_BitmapFont->AddVertsForTextInBox2D( textVerts , aabb , 2.f , gravity , Rgba8( 0 , 0 , 100 , 255 ) , 1.f , Vec2( 0.9f , 0.9f ));
	
	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( textVerts );
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
		m_physicsSystem->m_gravityMultiplier += 0.1f;
	}

	if ( g_theInput->WasKeyJustPressed( 0x6D ) )
	{
		m_physicsSystem->m_gravityMultiplier -= 0.1f;
	}
}

void Game::UpdateFramePositions( )
{

	m_frameCounter++;
	if ( m_frameCounter > 30 )
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

	if ( m_frameCounter == 30 )
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

	if ( !isDrawing )
	{
		if ( g_theInput->WasKeyJustPressed( 0x1B ) )
		{
			g_theapp->HandleQuitRequested();
		}
	}

	UpdateCameraMovement( deltaseconds );

	m_physicsSystem->Update( deltaseconds );
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
	//HandleCollissions();
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

	DisplayGravityInfo();
	DisplayX();
	
	g_theRenderer->EndCamera( *m_camera );

	
}
