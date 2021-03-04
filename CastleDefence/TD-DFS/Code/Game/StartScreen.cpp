#include "Game/StartScreen.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Tutorial.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"

StartScreen::StartScreen(Game* game)
{
	m_game = game;
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 320.f , 180.f ) );

	m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );
	
	LoadTextures();
	LoadAnimations();


	AABB2 temp = AABB2( m_camera->GetOrthoBottomLeft() , m_camera->GetOrthoTopRight() );

	m_table = temp.GetPercentageOfAABB(0.7f);

	temp = m_table;

	m_titleButton = temp.GetPercentageOfAABB( 0.5f );
	m_titleButton.CarveBoxOffBottom( 0.4f );

	m_titleButton.Translate(Vec2( 0.f , 60.f) );


	m_startButton = m_titleButton;
	m_startButton = m_startButton.GetPercentageOfAABB( 0.5 );
	m_startButton.Translate( Vec2( 0.f , -30.f ) );

	m_tutorialButton = m_startButton;
	m_tutorialButton.mins.x -= 5.f;
	m_tutorialButton.maxs.x += 5.f;

	m_startButton.mins.x += 5.f;
	m_startButton.maxs.x -= 5.f;

	m_tutorialButton.Translate( Vec2(0.f,-20.f) );

	m_exitButton = m_startButton;
	m_exitButton.Translate( Vec2(0.f , -40.f) );
	m_exitButton.mins.x += 7.f;
	m_exitButton.maxs.x -= 7.f;

	
	m_heroBox1 = AABB2( Vec2(-10.f,-10.f) , Vec2(10.f,10.f) );

	m_heroBox1.Translate( Vec2( 30.f , 30.f ) );
	m_heroBox1.Translate( Vec2(100.f,72.f) );

	m_heroBox2 = m_heroBox1;
	m_heroBox3 = m_heroBox1;

	m_heroBox2.Translate( Vec2(-10.f,-20.f) );
	m_heroBox3.Translate( Vec2(7.f,-40.f) );

	m_tutorialScreen = new Tutorial( game , this );
	
}

void StartScreen::Update( float deltaSeconds )
{
	m_time += deltaSeconds;

	if ( m_inTutorial )
	{
		m_tutorialScreen->Update( deltaSeconds );
	}

	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	Vec2 mousePositionScreen;
	mousePositionScreen.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	mousePositionScreen.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	if ( m_startButton.IsPointInside( mousePositionScreen ) )
	{
		hero1Idle = false;

		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_game->isInStartScreen = false;
		}
	}
	else
	{
		hero1Idle = true;
	}

	if ( m_tutorialButton.IsPointInside( mousePositionScreen ) )
	{
		hero2Idle = false;

		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_inTutorial = true;
		}

	}
	else
	{
		hero2Idle = true;
	}



	if ( m_exitButton.IsPointInside( mousePositionScreen ) )
	{
		hero3Idle = false;
		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			g_theapp->HandleQuitRequested();
		}
	}
	else
	{
		hero3Idle = true;
	}

}

void StartScreen::Render()
{
	g_theRenderer->BeginCamera( *m_camera );

	if ( m_inTutorial )
	{
		m_tutorialScreen->Render();
		return;
	}

	g_theRenderer->BindTexture( m_backRound );
	g_theRenderer->DrawAABB2D( AABB2( m_camera->GetOrthoBottomLeft() , m_camera->GetOrthoTopRight() ) , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( m_tableTex );
	g_theRenderer->DrawAABB2D(m_table,Rgba8(255,255,255,255));

	g_theRenderer->BindTexture( m_titleTex );
	g_theRenderer->DrawAABB2D( m_titleButton , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( m_startTex );
	if ( hero1Idle )
	{
		g_theRenderer->DrawAABB2D( m_startButton , normalColor );
	}
	else
	{
		g_theRenderer->DrawAABB2D( m_startButton , highColor );
	}
	g_theRenderer->BindTexture( m_tutorialTex );
	if ( hero2Idle )
	{
		g_theRenderer->DrawAABB2D( m_tutorialButton , Rgba8( 255 , 255 , 255 , 255 ) );
	}
	else
	{
		g_theRenderer->DrawAABB2D( m_tutorialButton , highColor );
	}
	g_theRenderer->BindTexture( m_exitTex );
	if ( hero3Idle )
	{
		g_theRenderer->DrawAABB2D( m_exitButton , Rgba8( 255 , 255 , 255 , 255 ) );
	}
	else
	{
		g_theRenderer->DrawAABB2D( m_exitButton , highColor );
	}

	if ( hero1Idle )
	{
		g_theRenderer->BindTexture(m_heroIdleAnims->GetSpriteTextureAtTime(m_time));
		g_theRenderer->DrawAABB2D( m_heroBox1 , Rgba8( 255 , 255 , 255 , 255 ) );
	}
	else
	{
		g_theRenderer->BindTexture( m_heroAttackAnims->GetSpriteTextureAtTime( m_time ) );
		g_theRenderer->DrawAABB2D( m_heroBox1 , Rgba8( 255 , 255 , 255 , 255 ) );
	}

	if ( hero2Idle )
	{
		g_theRenderer->BindTexture( m_heroIdleAnims->GetSpriteTextureAtTime( m_time ) );
		g_theRenderer->DrawAABB2D( m_heroBox2 , Rgba8( 255 , 255 , 255 , 255 ) );
	}
	else
	{
		g_theRenderer->BindTexture( m_heroAttackAnims->GetSpriteTextureAtTime( m_time ) );
		g_theRenderer->DrawAABB2D( m_heroBox2 , Rgba8( 255 , 255 , 255 , 255 ) );
	}

	if ( hero3Idle )
	{
		g_theRenderer->BindTexture( m_heroIdleAnims->GetSpriteTextureAtTime( m_time ) );
		g_theRenderer->DrawAABB2D( m_heroBox3 , Rgba8( 255 , 255 , 255 , 255 ) );
	}
	else
	{
		g_theRenderer->BindTexture( m_heroAttackAnims->GetSpriteTextureAtTime( m_time ) );
		g_theRenderer->DrawAABB2D( m_heroBox3 , Rgba8( 255 , 255 , 255 , 255 ) );
	}


	g_theRenderer->EndCamera( *m_camera );
}

void StartScreen::LoadTextures()
{
	m_backRound = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/Backround/background.png" );
	m_tableTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/NewGui/PNG/achievement/table.png" );
	m_titleTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/GameName.png" );
	m_startTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Start.png" );
	m_tutorialTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Tutorial.png" );
	m_exitTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Texts/Quit.png" );
}

void StartScreen::LoadAnimations()
{
	std::vector<Texture*> heroIdleTex;

	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_007.png" );
	Texture* a9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_008.png" );
	Texture* a10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_009.png" );
	Texture* a11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_010.png" );
	Texture* a12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_011.png" );
	Texture* a13 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_012.png" );
	Texture* a14 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_013.png" );
	Texture* a15 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_014.png" );
	Texture* a16 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_015.png" );
	Texture* a17 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_016.png" );
	Texture* a18 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Idle/Idle_017.png" );

	heroIdleTex.push_back( a1 );
	heroIdleTex.push_back( a2 );
	heroIdleTex.push_back( a3 );
	heroIdleTex.push_back( a4 );
	heroIdleTex.push_back( a5 );
	heroIdleTex.push_back( a6 );
	heroIdleTex.push_back( a7 );
	heroIdleTex.push_back( a8 );
	heroIdleTex.push_back( a9 );
	heroIdleTex.push_back( a10 );
	heroIdleTex.push_back( a11 );
	heroIdleTex.push_back( a12 );
	heroIdleTex.push_back( a13 );
	heroIdleTex.push_back( a14 );
	heroIdleTex.push_back( a15 );
	heroIdleTex.push_back( a16 );
	heroIdleTex.push_back( a17 );
	heroIdleTex.push_back( a18 );


	m_heroIdleAnims = new SpriteAnimDefTex( 0 , heroIdleTex.size() - 1 , 1.f );
	m_heroIdleAnims->m_animations = heroIdleTex;

	std::vector<Texture*> heroAttackTex;

	Texture* b1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_000.png" );
	Texture* b2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_001.png" );
	Texture* b3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_002.png" );
	Texture* b4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_003.png" );
	Texture* b5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_004.png" );
	Texture* b6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_005.png" );
	Texture* b7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_006.png" );
	Texture* b8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_007.png" );
	Texture* b9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_008.png" );
	Texture* b10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_009.png" );
	Texture* b11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_010.png" );
	Texture* b12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/CharactersNew/White Armored Knight/PNG/PNG Sequences/Slashing/Slashing_011.png" );

	heroAttackTex.push_back( b1 );
	heroAttackTex.push_back( b2 );
	heroAttackTex.push_back( b3 );
	heroAttackTex.push_back( b4 );
	heroAttackTex.push_back( b5 );
	heroAttackTex.push_back( b6 );
	heroAttackTex.push_back( b7 );
	heroAttackTex.push_back( b8 );
	heroAttackTex.push_back( b9 );
	heroAttackTex.push_back( b10 );
	heroAttackTex.push_back( b11 );
	heroAttackTex.push_back( b12 );

	m_heroAttackAnims = new SpriteAnimDefTex( 0 , heroAttackTex.size() - 1 , 1.f );
	m_heroAttackAnims->m_animations = heroAttackTex;

}
