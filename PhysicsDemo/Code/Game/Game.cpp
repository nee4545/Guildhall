#include "Game/Game.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
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
	obj->m_rigidbody->SetPosition( mousePos );

	PolygonCollider2D* collider = m_physicsSystem->CreatePolygonCollider( Vec2(mousePos) , &polygon );

	obj->m_rigidbody->TakeCollider( collider );

	m_gameObjects.push_back( obj );

	return obj;
}

void Game::PopulateInitialObjects()
{
	GameObject* obj1 = new GameObject();
	obj1->m_rigidbody = m_physicsSystem->CreateRigidbody();
	obj1->m_rigidbody->SetPosition( Vec2( 20.f , 20.f ) );

	DiscCollider2D* collider1 = m_physicsSystem->CreateDiscCollider( Vec2( 0.f , 0.f ) , 2.f );
	obj1->m_rigidbody->TakeCollider( collider1 );

	m_gameObjects.push_back( obj1 );

	GameObject* obj2 = new GameObject();
	obj2->m_rigidbody = m_physicsSystem->CreateRigidbody();
	obj2->m_rigidbody->SetPosition( Vec2( 0.f , 0.f ) );

	DiscCollider2D* collider2 = m_physicsSystem->CreateDiscCollider( Vec2( 0.f , 0.f ) , 1.f );
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
			if ( !initialPointSet )
			{
				throwInitialPoint = m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );
				initialPointSet = true;
			}

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

			

		}
		else
		{
			if ( !finalPointSet )
			{
				throwFinalPoint= m_camera->ClientToWorldPosition( g_theInput->GetCurrentMousePosition() );
				finalPointSet = true;
			}
			HandleThrow();
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
	if ( !initialPointSet )
	{
		return;
	}

	Vec2 velocity = -throwFinalPoint+throwInitialPoint;
	m_selectedObject->m_rigidbody->SetVelocity( velocity * 1.f );
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
		drawModePoints.pop_back();
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
		*poly = Polygon2D::MakeFromLineLoop( points , drawModePoints.size() );

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
}

void Game::Update( float deltaseconds )
{
	UpdateCameraMovement( deltaseconds );

	m_physicsSystem->Update( deltaseconds );
	if ( !isDrawing )
	{
		HandleObjectCreationRequests();
	}
	
	HandlePolygonDrawMode();
	

	for ( int index = 0; index < m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[ index ] == nullptr )
		{
			continue;
		}

		if ( m_gameObjects[ index ]->m_rigidbody->m_worldPosition.y < m_camera->GetOrthoBottomLeft().y )
		{
			m_gameObjects[ index ]->m_rigidbody->ReverseVelocityYAxis();
		}
	}

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

	HandleCollissions();
	
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
	
	g_theRenderer->EndCamera( *m_camera );

	
}




































