#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/Texture.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"




Game::Game()
{
	
	rng= RandomNumberGenerator();
	m_camera=new Camera();
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.33f,30.f));
	m_UICamera=new Camera();
	m_UICamera->SetOrthoView(Vec2(0.f,0.f),Vec2(UI_XMAX,UI_YMaX));
	
}

void Game::Update( float deltaseconds )
{
	if(frameNumber < 3 )
	{ 
	frameNumber++;
	return;
	}
	
	if( !assetsLoaded && frameNumber>=2 )
	{
		LoadTextures();
		LoadSounds();
		LoadFonts();
		TileDefinition::CreateTileDefinitions();
		world=new World(this);
		assetsLoaded = true;
		m_state = ATTRACT;
	}

	

	TogglePaused();
	if( paused )
	{
		deltaseconds=0.f;
	}


	ToggleDebugCamera();
	ToggleDebugging();
	HandleRenderScreens();
	
	if(m_state ==PLAYING )
	{ 
		world->Update( deltaseconds );

		if( !toggleCamera )
		{
			UpdateCamera();
		}
	}
	
}

void Game::Render()
{
	switch( m_state )
	{
	case LOADING:
	{
		RenderLoadingScreen();
		break;
	}
	case ATTRACT:
	{
		RenderStartScreen();
		break;
	}
	case PLAYING:
	{
		if(!world->current_map->IsPlayerAlive() )
		{ 
			RenderDeadScreen();
		}
		else
		{
			RenderPlayingScreen();
		}
		break;
	}
	case VICTORY:
	{
		RenderWinScreen();
		break;
	}

	case PAUSED:
	{
		RenderPausedScreen();
		break;
	}
	case  GAME_OVER:
	{
		RenderGameOverScreen();
		break;
	}
	default:
		break;
	}

}



void Game::UpdateCamera()
{
	Vec2 tankPos= world->current_map->m_EntityListByType[ENTITY_TYPE_PLAYER][0]->GetPosition();
	Vec2 camCoords;

	camCoords.x=Clamp(tankPos.x,8.f,WORLD_SIZE_X-8);
	camCoords.y=Clamp(tankPos.y,4.5f,WORLD_SIZE_Y-4.5f);

	m_camera->SetOrthoView(camCoords-Vec2(8.f,4.5f),camCoords+Vec2(8.f,4.5f));
}

void Game::ToggleDebugCamera()
{
	if(g_theInput->WasKeyJustPressed(0x73) )
	{ 
	toggleCamera=!toggleCamera;
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.3f,30.f));
	}
}

void Game::TogglePaused()
{
	if( g_theInput->WasKeyJustPressed( 'P' ) )
	{
		paused=!paused;
	}
}

void Game::ToggleDebugging()
{
	if( g_theInput->WasKeyJustPressed( 0x70 ) )
	{
		isDebugging=!isDebugging;
	}
}

void Game::LoadTextures()
{
	//Create all the textures required for the game

	Texture *texture =nullptr;
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/PlayerTankBase.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/PlayerTankTop.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/EnemyTurretBase.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/EnemyTurretTop.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/NemesisTankBase.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/NemesisTankTop.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/NemesisTankBase.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/Extras_4x4.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/Terrain_8x8.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/FriendlyTank1.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/FriendlyBullet.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/EnemyBullet.png");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/Plane.jpg");
	texture=g_theRenderer->CreateTextureFromFile("Data/Images/AirStrikePlane.png");
	
}

void Game::LoadSounds()
{
	SoundID sound;
	sound = g_theAudio->CreateOrGetSound("Data/Audio/PlayerShootNormal.ogg");
	sound = g_theAudio->CreateOrGetSound("Data/Audio/PlayerHit.wav");
	sound = g_theAudio->CreateOrGetSound("Data/Audio/PlayerDied.wav");
	sound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyHit.wav");
	sound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyShoot.wav");
	sound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyDied.wav");
	sound = g_theAudio->CreateOrGetSound("Data/Audio/airplanes.mp3");
	sound = g_theAudio->CreateOrGetSound("Data/Audio/AttractMusic.mp3");
	sound = g_theAudio->CreateOrGetSound("Data/Audio/GameplayMusic.mp3");
}

void Game::LoadFonts()
{
	m_BitmapFont = g_theRenderer->CreateBitMapFontFromFile("Data/Fonts/SquirrelFixedFont");
}

void Game::RenderUI()
{
	float offset=50.f;
	Vertex_PCU iconVertices[6];
	Vertex_PCU iconVertices_original[6];

	iconVertices[0] = Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	iconVertices[1] = Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	iconVertices[2] = Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	iconVertices[3] = Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	iconVertices[4] = Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	iconVertices[5] = Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	iconVertices_original[0] = Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	iconVertices_original[1] = Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	iconVertices_original[2] = Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );
				
	iconVertices_original[3] = Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	iconVertices_original[4] = Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	iconVertices_original[5] = Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );
				

	for( int livesIndex=0; livesIndex<currentLives; livesIndex++ )
	{
		for( int index=0; index<6; index++ )
		{
			iconVertices[index].m_position=TransformPosition3DXY( iconVertices_original[index].m_position, 50.f, 0.f, Vec2( 30.f+(livesIndex*offset), UI_YMaX-30.f ) );
		}

		Texture* tex = g_theRenderer->GetOrCreateTextureFromFile("Data/Images/FriendlyTank1.png");
		g_theRenderer->BindTexture(tex);
		g_theRenderer->DrawVertexArray( 6, iconVertices );
		g_theRenderer->BindTexture(nullptr);
	}

	for( int airCharges =0; airCharges<airStrikeCharges; airCharges++ )
	{
		for( int index=0; index<6; index++ )
		{
			iconVertices[index].m_position=TransformPosition3DXY( iconVertices_original[index].m_position, 55.f, 0.f, Vec2( 1450.f+(airCharges*offset), UI_YMaX-30.f ) );
		}

		Texture* tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/AirStrikePlane.png" );
		g_theRenderer->BindTexture( tex );
		g_theRenderer->DrawVertexArray( 6, iconVertices );
		g_theRenderer->BindTexture( nullptr );
	}
}

void Game::RenderStartScreen()
{
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.3f,30.f));
	g_theRenderer->BeginCamera(*m_camera);
	std::vector<Vertex_PCU> textVerts;
	m_BitmapFont->AddVertsForText2D( textVerts, Vec2( 15.f, 20.f ), 2.f, "INCURSION",Rgba8(100,0,0,255) );
	m_BitmapFont->AddVertsForText2D( textVerts, Vec2( 5.f, 18.f ), 1.5f, "Press Start or Space to Play!!",Rgba8(100,0,0,255) );
	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( textVerts );

}

void Game::RenderGameOverScreen()
{
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.3f,30.f));
	std::vector<Vertex_PCU> textVerts;
	m_BitmapFont->AddVertsForText2D( textVerts, Vec2( 15.f, 20.f ), 2.f, "GAME OVER", Rgba8( 100, 0, 0, 255 ) );
	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( textVerts );
}

void Game::RenderWinScreen()
{
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.3f,30.f));
	g_theRenderer->BeginCamera(*m_camera);
	std::vector<Vertex_PCU> textVerts;
	m_BitmapFont->AddVertsForText2D( textVerts, Vec2( 10.f, 20.f ), 2.f, "YOU WIN", Rgba8( 100, 0, 0, 255 ) );
	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( textVerts );
}


void Game::HandleRenderScreens()
{
	const XboxController& controller= g_theInput->GetXboxController( 0 );
	const KeyButtonState& start= controller.GetButtonState( XBOX_BUTTON_ID_START );
	const KeyButtonState& back = controller.GetButtonState(XBOX_BUTTON_ID_BACK);
	

	if( m_state == ATTRACT )
	{
		if( g_theInput->WasKeyJustPressed( 0x20 ) || start.WasJustPressed() )
		{
			m_state = PLAYING;
			return;
		}

		if( g_theInput->WasKeyJustPressed( 0x1B ) )
		{
			g_theapp->HandleQuitRequested();
			return;
		}

		if( back.WasJustPressed() )
		{
			g_theapp->HandleQuitRequested();
			return;
		}
	}

	if( m_state == PLAYING )
	{
		if( (g_theInput->WasKeyJustPressed( 'P' ) || start.WasJustPressed()) && world->current_map->IsPlayerAlive()  )
		{
			m_state = PAUSED;
			return;
		}

		if( !world->current_map->IsPlayerAlive() && g_theInput->WasKeyJustPressed( 0x20 ) )
		{
			Player* p=(Player*)world->current_map->m_EntityListByType[ENTITY_TYPE_PLAYER][0];
			p->RespawnTank();
		}

		if( !world->current_map->IsPlayerAlive() && currentLives<=0)
		{
			m_state= GAME_OVER;
			return;
		}

		if( back.WasJustPressed() || g_theInput->WasKeyJustPressed(0x1B) )
		{
			m_state=ATTRACT;
			return;
		}
	}

	if( m_state == PAUSED )
	{
		if( g_theInput->WasKeyJustPressed( 'P' ) || start.WasJustPressed() )
		{
			m_state = PLAYING;
			return;
		}

		if( g_theInput->WasKeyJustPressed( 0x1B ) )
		{
			m_state = ATTRACT;
			return;
		}
	}

	if( didPlayerWin )
	{
		m_state = VICTORY;
	}

	if( m_state == VICTORY )
	{
		if( g_theInput->WasKeyJustPressed( 0x20 ) || start.WasJustPressed() )
		{
			world->ResetGame();
			m_state = ATTRACT;
			return;
		}
	}

	if( m_state == GAME_OVER )
	{
		if( g_theInput->WasKeyJustPressed( 0x20 ) || start.WasJustPressed() )
		{
			world->ResetGame();
			m_state = ATTRACT;
			return;
		}
	}


}

void Game::RenderLoadingScreen()
{
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.3f,30.f));
	g_theRenderer->BeginCamera(*m_camera);
	DrawTextTriangles2D(*g_theRenderer,"LOADING...",Vec2(20.f,10.f),2.f,Rgba8(100,0,0,255));
}

void Game::RenderPausedScreen()
{
	RenderPlayingScreen();
	
	g_theRenderer->BeginCamera(*m_camera);
	AABB2 aabb= AABB2(0.f,0.f,53.3f,30.f);
	Rgba8 tint = Rgba8(0,0,0,100);
	g_theRenderer->DrawAABB2D(aabb,tint);
	std::vector<Vertex_PCU> textVerts;
	m_BitmapFont->AddVertsForText2D( textVerts, m_camera->GetOrthoBottomLeft()+Vec2( 5.f, 5.f ), 1.f, "Paused", Rgba8( 10, 0, 25, 255 ) );
	m_BitmapFont->AddVertsForText2D( textVerts, m_camera->GetOrthoBottomLeft()+Vec2( 1.f, 4.f ), 0.5f, "Press Start or P to continue..", Rgba8( 10, 0, 25, 255 ) );
	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( textVerts );
}

void Game::RenderPlayingScreen()
{
	g_theRenderer->BeginCamera( *m_camera );
	world->Render();
	g_theRenderer->EndCamera( *m_camera );


	g_theRenderer->BeginCamera( *m_UICamera );
	RenderUI();
	g_theRenderer->EndCamera( *m_UICamera );
}

void Game::RenderDeadScreen()
{
	RenderPlayingScreen();

	g_theRenderer->BeginCamera( *m_camera );
	AABB2 aabb= AABB2( 0.f, 0.f, 53.3f, 30.f );
	Rgba8 tint = Rgba8( 0, 0, 0, 100 );
	g_theRenderer->DrawAABB2D( aabb, tint );
	std::vector<Vertex_PCU> textVerts;
	m_BitmapFont->AddVertsForText2D( textVerts,m_camera->GetOrthoBottomLeft()+ Vec2( 5.f, 5.f ), 1.f, "You Died", Rgba8( 10, 0, 25, 255 ) );
	m_BitmapFont->AddVertsForText2D( textVerts, m_camera->GetOrthoBottomLeft()+Vec2( 0.f, 4.f ), 0.5f, "Press Start or Space to Respawn", Rgba8( 10, 0, 25, 255 ) );
	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( textVerts );
}
























