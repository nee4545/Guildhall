#include "Game/SinglePlayerGame.hpp"
#include "Game/Player.hpp"
#include "Game/Bullet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Timer.hpp"
#include "Game/AI.hpp"

SinglePlayerGame::SinglePlayerGame()
{

}

SinglePlayerGame::~SinglePlayerGame()
{

}

void SinglePlayerGame::AssignInputSystem( InputSystem* inputSystem )
{
	m_InputSystem = inputSystem;
}

void SinglePlayerGame::CreatePlayer()
{
	m_player = new Player(this,Vec2(30.f,10.f));
	m_rng = new RandomNumberGenerator();
	m_aiSpawnTimer = new Timer();
	m_aiSpawnTimer->SetSeconds( 4.f );
}

void SinglePlayerGame::StartUp()
{
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2(0.f,0.f) , Vec2(80.f,45.f) );
	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 1.f );

	m_gameOverTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/gameover.png" );
}

void SinglePlayerGame::Update( float deltaSeconds )
{
	Vec2 mouseNormalizedPosition = m_InputSystem->GetCurrentMousePosition();

	m_mousePosition.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	m_mousePosition.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	//ToggleDevConsole();
	if ( devConsoleOpen )
	{
		return;
	}

	if ( m_player->m_health <= 0 )
	{
		gameOver = true;
	}

	if ( gameOver )
	{
		return;
	}

	if ( m_player != nullptr )
	{
		m_player->Update( deltaSeconds );
	}


	for ( int i = 0; i < m_bullets.size(); i++ )
	{
		if ( m_bullets[ i ] != nullptr )
		{
			m_bullets[ i ]->Update(deltaSeconds);
		}
	}

	for ( int i = 0; i < m_ais.size(); i++ )
	{
		if ( m_ais[ i ] != nullptr )
		{
			m_ais[ i ]->Update( deltaSeconds );
		}
	}

	if ( m_aiSpawnTimer->HasElapsed() )
	{
		SpawnAI();
		m_aiSpawnTimer->Reset();
	}

	DoCollissionDetection();
	DoGarbageCollection();

	
	
}

void SinglePlayerGame::Render( )
{
	g_theRenderer->BeginCamera( *m_camera );

	for ( int i = 0; i < m_bullets.size(); i++ )
	{
		if ( m_bullets[ i ] != nullptr )
		{
			m_bullets[ i ]->Render();
		}
	}

	for ( int i = 0; i < m_ais.size(); i++ )
	{
		if ( m_ais[ i ] != nullptr )
		{
			m_ais[ i ]->Render( );
		}
	}


	if ( m_player != nullptr )
	{
		m_player->Render();
	}

	if ( gameOver )
	{
		g_theRenderer->BindTexture( m_gameOverTex );
		AABB2 aabb = AABB2( m_camera->GetOrthoBottomLeft() , m_camera->GetOrthoTopRight() );
		g_theRenderer->DrawAABB2D( aabb.GetPercentageOfAABB( 0.3f ) , Rgba8() );
	}

	g_theRenderer->EndCamera( *m_camera );

	
}

void SinglePlayerGame::SpawnBullet( Vec2 position , Vec2 forwardVector , Faction faction )
{
	Bullet* b = new Bullet( this , position , forwardVector , faction );
	m_bullets.push_back( b );
}

void SinglePlayerGame::SpawnAI()
{
	float x = m_rng->RollRandomIntInRange( 0 , 75 );
	float y = m_rng->RollRandomIntInRange( 0 , 40 );

	int num = m_rng->RollRandomIntInRange( 0 , 3 );

	if ( num < 1 )
	{
		AI* a = new AI( this , Vec2( x , y ) , TYPE_1 );
		m_ais.push_back( a );
	}
	else
	{
		AI* a = new AI( this , Vec2( x , y ) , TYPE_2 );
		m_ais.push_back( a );
	}
}

void SinglePlayerGame::DoGarbageCollection()
{
	for ( int i = 0; i < m_ais.size(); i++ )
	{
		if ( m_ais[ i ] == nullptr )
		{
			continue;
		}

		if ( m_ais[ i ]->m_isGarbage )
		{
			delete m_ais[ i ];
			m_ais[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_bullets.size(); i++ )
	{
		if ( m_bullets[ i ] == nullptr )
		{
			continue;
		}

		if ( m_bullets[ i ]->m_isGarbage )
		{
			delete m_bullets[ i ];
			m_bullets[ i ] = nullptr;
		}
	}
}

void SinglePlayerGame::DoCollissionDetection()
{
	for ( int i = 0; i < m_bullets.size(); i++ )
	{
		if ( m_bullets[ i ] != nullptr )
		{
			for ( int j = 0; j < m_ais.size(); j++ )
			{
				if ( m_ais[ j ] == nullptr )

				{
					continue;
				}
				if ( DoDiscsOverlap( m_bullets[ i ]->m_position , 0.5f , m_ais[ j ]->m_position , 1.f ) && m_bullets[i]->m_faction == FACTION_GOOD )
				{
					m_bullets[ i ]->Die();
					m_ais[ j ]->Die();
				}
			}

			if ( DoDiscsOverlap( m_bullets[ i ]->m_position , 0.5f , m_player->m_position , 1.f ) && m_bullets[ i ]->m_faction == FACTION_BAD )
			{
				m_bullets[ i ]->Die();
				m_player->m_health--;
			}

			if ( m_bullets[ i ]->m_position.x > 80.f || m_bullets[ i ]->m_position.y > 45.f || m_bullets[ i ]->m_position.x < 0.f || m_bullets[ i ]->m_position.y < 0.f )
			{
				m_bullets[ i ]->Die();
			}
		}
	}

	for ( int i = 0; i < m_ais.size(); i++ )
	{
		if ( m_ais[ i ] == nullptr )
		{
			continue;
		}

		if ( DoDiscsOverlap( m_ais[ i ]->m_position , 1.f , m_player->m_position , 1.f ) )
		{
			m_ais[ i ]->Die();
			m_player->m_health--;
		}
	}
}

void SinglePlayerGame::ToggleDevConsole()
{
	if ( g_theInput->WasKeyJustPressed( 0xC0 ) )
	{
		devConsoleOpen = !devConsoleOpen;
		g_theConsole.SetIsOpen( devConsoleOpen );
	}
}

