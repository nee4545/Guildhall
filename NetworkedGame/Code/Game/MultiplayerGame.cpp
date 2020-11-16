#include "Game/MultiplayerGame.hpp"
#include "Game/Player.hpp"
#include "Game/EntityCommon.hpp"
#include "Engine/Core/StringUtils.hpp"

int MultiplayerGame::entityID =0;

MultiplayerGame::MultiplayerGame()
{
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 80.f , 45.f ) );
	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );
	isSinglePlayer = false;
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
	
}

void MultiplayerGame::CreateSecondPlayer()
{
	m_player2 = new Player( this , Vec2( 30.f , 20.f ) , FACTION_BAD );
	m_player2->m_ID = ++entityID;
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
		}
		else
		{
			m_player1->m_position = Vec2( posx , posy );
		}
	}

	if ( id == 2 )
	{
		if ( m_player2 == nullptr )
		{
			m_player2 = new Player( this , Vec2( posx , posy ) , FACTION_BAD );
		}
		else
		{
			m_player2->m_position = Vec2( posx , posy );
		}
	}

}

void MultiplayerGame::Update( float deltaSeconds )
{
	if ( g_theAutoratitiveServer == nullptr )
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

	g_theRenderer->EndCamera( *m_camera );
}
