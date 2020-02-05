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

	if ( m_selectedObject != nullptr )
	{
		if ( g_theInput->IsLeftMouseButtonPressed() )
		{
			m_dragInProgress = true;
			m_selectedObject->isBeingDragged = true;
			
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
			m_dragInProgress = false;
			m_selectedObject->isBeingDragged = false;
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

void Game::Update( float deltaseconds )
{
	UpdateCameraMovement( deltaseconds );

	if ( g_theInput->GetMouseWheelData()>0 )
	{
		ZoomInCamera( deltaseconds );
	}

	if ( g_theInput->GetMouseWheelData()<0 )
	{
		ZoomOutCamera( deltaseconds );
	}
	HandleObjectCreationRequests();
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




































