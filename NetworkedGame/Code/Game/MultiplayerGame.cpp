#include "Game/MultiplayerGame.hpp"
#include "Game/Player.hpp"
#include "Game/EntityCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Bullet.hpp"
#include "Game/AI.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

int MultiplayerGame::entityID =0;

MultiplayerGame::MultiplayerGame()
{
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 80.f , 45.f ) );
	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );
	isSinglePlayer = false;
	aiSpawnTimer = new Timer();
	aiSpawnTimer->SetSeconds( 15.f );
	m_rng = new RandomNumberGenerator();
}

MultiplayerGame::~MultiplayerGame()
{

}

void MultiplayerGame::AssignInputSystemForPlayer1( InputSystem* inputSystem )
{
	m_player1Input = inputSystem;
}

void MultiplayerGame::CreatePlayer()
{
	m_player1 = new Player(this,Vec2(10.f,10.f),FACTION_GOOD);
	m_player1->m_ID = ++entityID;
	m_player1->SetSpriteBasedOnID();
	
}

void MultiplayerGame::CreateSecondPlayer()
{
	m_player2 = new Player( this , Vec2( 30.f , 20.f ) , FACTION_BAD );
	m_player2->m_ID = ++entityID;
	m_player2->SetSpriteBasedOnID();
}

void MultiplayerGame::CreateOrUpdateEntitiesFromStr( std::string messageStr )
{
	Strings s = SplitStringOnDelimiter( messageStr , '|' );

	Strings s1 = SplitStringOnDelimiter( s[ 0 ] , '=' );
	Strings s2 = SplitStringOnDelimiter( s[ 1 ] , '=' );
	Strings s3 = SplitStringOnDelimiter( s[ 2 ] , '=' );
	Strings s4 = SplitStringOnDelimiter( s[ 3 ] , '=' );
	Strings s5 = SplitStringOnDelimiter( s[ 4 ] , '=' );

	int id = atoi( s1[ 1 ].c_str() );
	float posx = atof( s4[ 1 ].c_str() );
	float posy = atof( s5[ 1 ].c_str() );
	
	if ( id == 1 )
	{
		if ( m_player1 == nullptr )
		{
			m_player1 = new Player(this,Vec2(posx,posy),FACTION_GOOD);
			//m_player1->SetSpriteBasedOnID();
		}
		else
		{
			m_player1->m_position = Vec2( posx , posy );
		}
		return;
	}

	if ( id == 2 )
	{
		if ( m_player2 == nullptr )
		{
			m_player2 = new Player( this , Vec2( posx , posy ) , FACTION_BAD );
			m_player2->SetSpriteBasedOnID();
		}
		else
		{
			m_player2->m_position = Vec2( posx , posy );
		}
		return;
	}

	if ( s2[ 1 ] == "Bullet" )
	{
		bool idFound = false;
		for ( int i = 0; i < m_bullets.size(); i++ )
		{
			if ( m_bullets[ i ] != nullptr )
			{
				if ( m_bullets[ i ]->m_ID == id )
				{
					m_bullets[ i ]->m_position = Vec2( posx , posy );
					idFound = true;
				}
			}
		}

		if ( idFound == false )
		{
			Faction faction;
			if ( s3[ 1 ] == "Good" )
			{
				faction = FACTION_GOOD;
			}
			else
			{
				faction = FACTION_BAD;
			}
			Bullet* bullet = new Bullet( this , Vec2( posx , posy ),Vec2(0.f,1.f) , faction );
			bullet->m_ID = id;

			m_bullets.push_back( bullet );
		}
	}

	if ( s2[ 1 ] == "AI" )
	{
		bool idFound = false;
		for ( int i = 0; i < m_ais.size(); i++ )
		{
			if ( m_ais[ i ] != nullptr )
			{
				if ( m_ais[ i ]->m_ID == id )
				{
					m_ais[ i ]->m_position = Vec2( posx , posy );
					idFound = true;
				}
			}
		}

		if ( idFound == false )
		{
			Faction faction;
			if ( s3[ 1 ] == "Good" )
			{
				faction = FACTION_GOOD;
			}
			else
			{
				faction = FACTION_BAD;
			}
			AI* a = new AI( this ,Vec2(posx,posy),TYPE_2);
			a->m_ID = id;
			a->m_faction = faction;
			a->SetSpriteBasedOnFaction();

			m_ais.push_back( a );
		}
	}



}

void MultiplayerGame::SpawnBullet( Vec2 position , Vec2 forwardVector , Faction faction )
{
	Bullet* b = new Bullet( this , position , forwardVector , faction );
	b->m_ID = ++entityId;
	m_bullets.push_back( b );
}

void MultiplayerGame::Update( float deltaSeconds )
{

	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	m_mousePosition.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	m_mousePosition.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	if ( g_theRemoteServer != nullptr )
	{
		return;
	}

	if ( m_player1 != nullptr )
	{
		m_player1->Update( deltaSeconds );
	}

	if ( m_player2 != nullptr )
	{
		m_player2->Update( deltaSeconds );
	}

	for ( int i = 0; i < m_bullets.size(); i++ )
	{
		if ( m_bullets[ i ] != nullptr )
		{
			m_bullets[ i ]->Update( deltaSeconds );
		}
	}

	for ( int i = 0; i < m_ais.size(); i++ )
	{
		if ( m_ais[ i ] != nullptr )
		{
			m_ais[ i ]->Update( deltaSeconds );
		}
	}

	if ( aiSpawnTimer->HasElapsed() )
	{
		if ( !aiSpawnned )
		{
			SpawnAI();
			aiSpawnned = true;
		}
		aiSpawnTimer->Reset();
	}

	//DoCollissionDetection();
	//DoGarbageCollection();
}

void MultiplayerGame::DoCollissionDetection()
{
	for ( int i = 0; i < m_bullets.size(); i++ )
	{
		if ( m_bullets[ i ] != nullptr )
		{
			for ( int j = 0; j < m_ais.size(); j++ )
			{
				if ( m_ais[ j ] != nullptr )
				{
					if ( DoDiscsOverlap( m_bullets[ i ]->m_position , 0.5f , m_ais[ j ]->m_position , 1.f ) && m_bullets[ i ]->m_faction != m_ais[ j ]->m_faction )
					{
						m_bullets[ i ]->Die();
						m_ais[ j ]->Die();
					}
				}
			}


			if ( m_bullets[ i ]->m_faction == FACTION_GOOD )
			{
				if ( DoDiscsOverlap( m_bullets[ i ]->m_position , 0.5f , m_player2->m_position , 1.f ) )
				{
					m_bullets[ i ]->Die();
					m_player2->m_health--;
				}
			}

			if ( m_bullets[ i ]->m_faction == FACTION_BAD )
			{
				if ( DoDiscsOverlap( m_bullets[ i ]->m_position , 0.5f , m_player1->m_position , 1.f ) )
				{
					m_bullets[ i ]->Die();
					m_player1->m_health--;
				}
			}

			if ( m_bullets[ i ]->m_position.x > 80.f || m_bullets[ i ]->m_position.y > 45.f || m_bullets[ i ]->m_position.x < 0.f || m_bullets[ i ]->m_position.y < 0.f )
			{
				m_bullets[ i ]->Die();
			}
		}
	}

	for ( int i = 0; i < m_ais.size(); i++ )
	{
		if ( m_ais[ i ] != nullptr )
		{
			if ( m_ais[ i ]->m_faction == FACTION_GOOD )
			{
				if ( DoDiscsOverlap( m_ais[ i ]->m_position , 1.f , m_player2->m_position , 1.f ) )
				{
					m_ais[ i ]->Die();
					m_player2->m_health--;
				}
			}
			else
			{
				if ( DoDiscsOverlap( m_ais[ i ]->m_position , 1.f , m_player1->m_position , 1.f ) )
				{
					m_ais[ i ]->Die();
					m_player1->m_health--;
				}
			}
		}

	}
}

void MultiplayerGame::DoGarbageCollection()
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

void MultiplayerGame::Render()
{
	g_theRenderer->BeginCamera( *m_camera );

	if ( m_player1 != nullptr )
	{
		m_player1->Render();
	}

	if ( m_player2 != nullptr )
	{
		m_player2->Render();
	}

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
			m_ais[ i ]->Render();
		}
	}

	g_theRenderer->EndCamera( *m_camera );
}

void MultiplayerGame::SpawnAI()
{
	float x1 = m_rng->RollRandomIntInRange( 0 , 75 );
	float y1 = m_rng->RollRandomIntInRange( 0 , 40 );

	float x2 = m_rng->RollRandomIntInRange( 0 , 75 );
	float y2 = m_rng->RollRandomIntInRange( 0 , 40 );

	AI* a1 = new AI( this , Vec2( x1 , y1 ) , TYPE_2 );
	a1->m_ID = ++entityId;
	a1->m_faction = FACTION_GOOD;
	a1->SetSpriteBasedOnFaction();

	AI* a2 = new AI( this , Vec2( x2 , y2 ) , TYPE_2 );
	a2->m_ID = ++entityId;
	a2->m_faction = FACTION_BAD;
	a2->SetSpriteBasedOnFaction();

	
	m_ais.push_back( a1 );
	m_ais.push_back( a2 );


}
