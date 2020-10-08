#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Texture.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/TileMap.hpp"
#include "Game/MapMaterial.hpp"
#include "Game/MapRegionType.hpp"
#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Actor.hpp"
#include "Game/EntityDef.hpp"
#include "Engine/Job.hpp"
#include "Engine/JobSystem.hpp"

World* world=nullptr;

class TestJob :public Job
{
public:
	TestJob():Job()
	{
	}

	~TestJob() {}

public:
	virtual void Execute() override
	{

		for ( long i = 0; i < 1000000; i++ )
		{
			std::string s = std::to_string( m_jobID ) + " ";
			g_theConsole.PrintString( Rgba8( 100 , 100 , 100 , 255 ) , std::to_string( m_jobID ) );
			//DebuggerPrintf( s.c_str()  );
		}

		//ClaimJobCallback();
	}
};

bool LoadMapFromDevConsole( EventArgs& args )
{
	std::string mapName = args.GetValue( "mapName" , "null" );
	world->LoadMap( mapName );
	return true;
}

bool LoadAllMapNames( EventArgs& args )
{
	UNUSED( args );
	Strings s = world->GetAllCurrentMapNames();
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "Currently supported maps" );
	for ( int i = 0; i < s.size(); i++ )
	{
		g_theConsole.PrintString( Rgba8( 0 , 0 , 0 , 255 ) , s[ i ] );
	}
	return false;
}

Game::Game()
{
	m_rng = RandomNumberGenerator();

	TestJob* job1 = new TestJob();
	TestJob* job2 = new TestJob();
	TestJob* job3 = new TestJob();
	TestJob* job4 = new TestJob();
	TestJob* job5 = new TestJob();
	TestJob* job6 = new TestJob();
	TestJob* job7 = new TestJob();
	TestJob* job8 = new TestJob();
	TestJob* job9 = new TestJob();
	TestJob* job10 = new TestJob();
	TestJob* job11 = new TestJob();
	TestJob* job12 = new TestJob();

	g_theJobSystem->PostJob( job1 );
	g_theJobSystem->PostJob( job2 );
	g_theJobSystem->PostJob( job3 );
	g_theJobSystem->PostJob( job4 );
	g_theJobSystem->PostJob( job5 );
	g_theJobSystem->PostJob( job6 );
	g_theJobSystem->PostJob( job7 );
	g_theJobSystem->PostJob( job8 );
	g_theJobSystem->PostJob( job9 );
	g_theJobSystem->PostJob( job10 );
	g_theJobSystem->PostJob( job11 );
	g_theJobSystem->PostJob( job12 );

	tinyxml2::XMLDocument gameConfig;
	gameConfig.LoadFile( "Data/XML/gameconfig.xml" );

	tinyxml2::XMLElement* ele = gameConfig.RootElement();
	std::string soundPath = ParseXmlAttribute( *ele , "soundPath" , "none" );

	testSound = g_theAudio->CreateOrGetSound( soundPath);

	Texture* spriteTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/ViewModelsSpriteSheet_8x8.png" );

	m_weaponSprites = new SpriteSheet( *spriteTex , IntVec2( 8 , 8 ) );

	m_camera = new Camera();
	m_camera->SetProjectionPerspective( 60.f , 16.f / 9.f , -0.09f , -100.f );

	m_UICamera = new Camera();
	m_UICamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 80.f ) );

	m_HUDCamera = new Camera();
	m_HUDCamera->SetOrthoView( Vec2(0.f,0.f) , Vec2(1600.f,800.f) );

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 80.f ) );

	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

	m_camera->m_transform.SetConvenstion( CONVENSION_XEAST_YNORTH_ZUP );

	m_camera->m_transform.m_position  = Vec3( 0.f , 0.f , 0.65f );

	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );
	m_UICamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );
	m_devConsoleCamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );
	m_HUDCamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 2.5f );

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 2.5f );

	m_cube = new GPUMesh(g_theRenderer);
	m_basis = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCU> cube;
	std::vector<unsigned int> cubeIndices;
	AppendCuboidV2( cube ,cubeIndices, AABB3(Vec3(0.f,0.f,0.f),Vec3(1.f,1.f,1.f)) , Rgba8( 255 , 255 , 255 , 255 ) );

	m_cube->UpdateVertices( ( unsigned int) cube.size() , &cube[ 0 ] );
	m_cube->UpdateIndices( ( unsigned int ) cubeIndices.size() , &cubeIndices[ 0 ] );
	
	cubeTransform1.m_position = Vec3( 2.f , 0.f , 0.f );
	cubeTransform2.m_position = Vec3( 0.f , 2.f , 0.f );
	cubeTransform3.m_position = Vec3( 2.f , 2.f , 0.f );

	Mat44 cameraModel = m_camera->m_transform.ToMatrix();

	
	AppendArrow( m_basisVerts , Vec3(0.f,0.f,0.f) , Vec3(0.f,0.f,0.f) + cameraModel.GetIBasis3D()  , 0.03f , 0.01f , 0.02f , Rgba8(100,0,0,255) , Rgba8( 100 , 0 , 0 , 255 ) );
	AppendArrow( m_basisVerts , Vec3(0.f,0.f,0.f) , Vec3(0.f,0.f,0.f) + cameraModel.GetJBasis3D()  , 0.03f , 0.01f , 0.02f , Rgba8(0,100,0,255) , Rgba8( 0 , 100 , 0 , 255 ) );
	AppendArrow( m_basisVerts , Vec3(0.f,0.f,0.f) , Vec3(0.f,0.f,0.f) + cameraModel.GetKBasis3D()  , 0.03f , 0.01f , 0.02f , Rgba8(0,0,100,255) , Rgba8( 0 , 0 , 100 , 255 ) );
	
	m_basis->UpdateVertices( ( unsigned int ) m_basisVerts.size() , &m_basisVerts[ 0 ] );
	

	MapMaterial::LoadDefinitions( "Data/Definitions/MapMaterialTypes.xml" );
	MapRegionType::LoadDefinitions( "Data/Definitions/MapRegionTypes.xml" );
	EntityDef::LoadDefinitions( "Data/Definitions/EntityTypes.xml" );

	EntityDef::s_entityDefs;

	m_world = new World(this);
	m_world->LoadAllMaps( "Data/Maps" );
	m_world->m_sound = testSound;

	tinyxml2::XMLDocument GameConfigDoc;
	GameConfigDoc.LoadFile( "Data/GameConfig/GameConfig.xml" );

	tinyxml2::XMLElement* gameConfigRoot = GameConfigDoc.RootElement();
	std::string startMap = ParseXmlAttribute( *gameConfigRoot , "StartMap" , "NULL" );

	m_world->LoadMap( startMap );

	world = m_world;

	g_theEventSystem.SubscribeToEvent( "maps" , LoadAllMapNames );
	g_theEventSystem.SubscribeToEvent( "map" , LoadMapFromDevConsole );

	g_theInput->ClipSystemCursor();
	g_theInput->SetCursorMode( MODE_RELATIVE );

}

Game::~Game()
{
	delete m_camera;
	delete m_UICamera;
	delete m_HUDCamera;
	delete m_devConsoleCamera;
	delete m_cube;
	delete m_basis;
	delete m_weaponSprites;
}

void Game::Update( float deltaseconds )
{
	
	ToggleDebugging();
	ToggleDevConsole();
	SoundTest();

	if ( g_theConsole.IsOpen() )
	{
		return;
	}

	UpdatePossesingEntityFromKeyBoard();

	Mat44 cameraTransform = m_camera->m_transform.ToMatrix();
	Vec3 forwardVector = cameraTransform.GetIBasis3D();
	forwardVector.z			= 0.f;
	Vec3 rightVector = -cameraTransform.GetJBasis3D();
	rightVector.z = 0.f;
	Vec3 upMovement = Vec3(0.f,0.f,1.f);

	float speed = 4.f;
	if ( g_theInput->IsKeyPressed( SHIFT ) )
	{
		speed = 10.f;
	}

	if ( g_theInput->IsKeyPressed( 'A' ) )
	{
		m_camera->SetPosition( m_camera->m_transform.m_position - rightVector * speed * deltaseconds );
	}

	if ( g_theInput->IsKeyPressed( 'D' ) )
	{
		m_camera->SetPosition( m_camera->m_transform.m_position + rightVector * speed * deltaseconds );
	}

	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		Vec3 newPos = m_camera->m_transform.m_position + forwardVector * speed * deltaseconds;
		m_camera->SetPosition( newPos );
	}

	if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		Vec3 newPos = m_camera->m_transform.m_position - forwardVector * speed * deltaseconds;
		m_camera->SetPosition( newPos );
	}

	if ( g_theInput->IsKeyPressed( 'Q' ) )
	{
		m_camera->SetPosition( m_camera->m_transform.m_position - upMovement * speed * deltaseconds );
	}

	if ( g_theInput->IsKeyPressed( 'E' ) )
	{
		m_camera->SetPosition( m_camera->m_transform.m_position + upMovement * speed * deltaseconds );
	}

	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		Vec3 position = Vec3( 0.f , 0.f , 0.f );
		m_camera->SetPosition( position );
		m_pitch = 0.f;
		m_yaw = 0.f;
	}

	Vec2 mousePos = g_theInput->m_relativeMovement;

	m_yaw -= mousePos.x * speed * deltaseconds;
	m_pitch += mousePos.y * speed * deltaseconds;
	m_pitch = Clamp( m_pitch , -89.9f , 89.9f );
	
	float finalRoll = 0.f;
	m_camera->m_transform.SetRotationFromYawPitchRollDegrees( m_yaw ,m_pitch, finalRoll );

	m_world->Update( deltaseconds );

	if ( m_player != nullptr )
	{
		m_player->m_position = Vec2( m_camera->m_transform.m_position.x , m_camera->m_transform.m_position.y );
		m_player->m_orientationDegrees = m_camera->m_transform.m_yaw;
	}

	m_fps = g_theapp->GetFPS();
}

void Game::Render()
{
	g_theRenderer->BeginCamera( *m_camera );
	m_camera->CreateDepthStencilTarget( g_theRenderer );
	g_theRenderer->SetDepthTest();
	g_theRenderer->BindDepthStencil( m_camera->m_backBuffer );

	g_theRenderer->CreateRasterState( SOLID , CULL_BACK );

	m_world->Render();

	Mat44 cameraTransform = m_camera->m_transform.ToMatrix();
	Vec3 forwardVector = cameraTransform.GetIBasis3D();
	forwardVector.z = 0.f;

	if ( thegame->m_debugDrawOn )
	{
		if ( m_player != nullptr )
		{
			TileMap* m = ( TileMap* ) m_world->m_currentMap;
			m->DebugRenderRaycasts( Vec2( m_camera->m_transform.m_position.x , m_camera->m_transform.m_position.y ) , Vec2( forwardVector.x , forwardVector.y ).GetNormalized() , 5.f );
		}
	}


	g_theRenderer->EndCamera( *m_camera );

	if ( m_debugDrawOn )
	{
		DisplayDebugInfo();
	}

	DisplayHUD();
	DisplayFPS();

	if ( g_theConsole.IsOpen() )
	{
		g_theConsole.Render( *g_theRenderer , *m_devConsoleCamera , 2.5f , 1.5f );
	}
	

	//DebugRenderSystem::sDebugRenderer->DebugRenderWorldToCamera( m_camera );

	
}

void Game::ToggleDevConsole()
{
	if ( g_theInput->WasKeyJustPressed( 0xC0 ) )
	{
		devConsoleOpen = !devConsoleOpen;
		g_theConsole.SetIsOpen( devConsoleOpen );
	}

	if ( g_theConsole.IsOpen() )
	{
		g_theInput->UnClipSystemCursor();
		g_theInput->SetCursorMode( MODE_ABSOLUTE );
		g_theInput->ShowSystemCursor();
	}
	else
	{
		g_theInput->ClipSystemCursor();
		g_theInput->SetCursorMode( MODE_RELATIVE );
		//g_theInput->HideSystemCursor();
	}
}

void Game::ToggleDebugging()
{
	if ( g_theInput->WasKeyJustPressed( F1 ) )
	{
		m_debugDrawOn = !m_debugDrawOn;
	}
}

void Game::DisplayUI()
{
	AABB2 box = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );

	std::string s1 = "Camera Yaw";
	std::string s2 = "Camera Pitch";
	std::string s3 = "Camera Roll";
	std::string s4 = "xyz";

	std::string s5 = "iBasis";
	std::string s6 = "jBasis";
	std::string s7 = "kBasis";

	std::vector<Vertex_PCU> v1;
	std::string temp = s1;
	temp += " " + std::to_string( m_yaw ) + " " +s2 + " " + std::to_string(m_pitch) + " " + s3 + " " + std::to_string(m_roll);
	temp += " " + s4 + "(" + std::to_string( m_camera->m_transform.m_position.x ) + "," + std::to_string( m_camera->m_transform.m_position.y ) + "," + std::to_string( m_camera->m_transform.m_position.z ) + ")";

	m_font->AddVertsForTextInBox2D( v1 , box , 1.f , temp , Rgba8( 100 , 100 , 0 , 255 ) , 1.f , Vec2( 0.03f , 0.98f ) );

	Mat44 cameraModel = m_camera->m_transform.ToMatrix();
	Vec3 iBasis = cameraModel.GetIBasis3D();
	Vec3 jBasis = cameraModel.GetJBasis3D();
	Vec3 kBasis = cameraModel.GetKBasis3D();

	std::vector<Vertex_PCU> v2;
	temp = "";
	temp += "= (" + std::to_string( iBasis.x ) + "," + std::to_string( iBasis.y ) + "," + std::to_string( iBasis.z ) +")";
	s5 += temp;

	m_font->AddVertsForTextInBox2D( v2 , box , 1.f , s5 , Rgba8( 255 , 0 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.96f ) );

	std::vector<Vertex_PCU> v3;
	temp = "";
	temp += "= (" + std::to_string( jBasis.x ) + "," + std::to_string( jBasis.y ) + "," + std::to_string( jBasis.z ) + ")";
	s6 += temp;

	m_font->AddVertsForTextInBox2D( v3 , box , 1.f , s6 , Rgba8( 0 , 255 , 0 , 255 ) , 1.f , Vec2( 0.02f , 0.94f ) );

	std::vector<Vertex_PCU> v4;
	temp = "";
	temp += "= (" + std::to_string( kBasis.x ) + "," + std::to_string( kBasis.y ) + "," + std::to_string( kBasis.z ) + ")";
	s7 += temp;

	m_font->AddVertsForTextInBox2D( v3 , box , 1.f , s7 , Rgba8( 0 , 0 , 255 , 255 ) , 1.f , Vec2( 0.02f , 0.92f ) );

	g_theRenderer->BeginCamera( *m_UICamera );

	g_theRenderer->BindTexture( m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( v1 );
	g_theRenderer->DrawVertexArray( v2 );
	g_theRenderer->DrawVertexArray( v3 );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->EndCamera( *m_UICamera );
}

void Game::DisplayFPS()
{
	AABB2 box = AABB2( m_UICamera->GetOrthoBottomLeft() , m_UICamera->GetOrthoTopRight() );
	box.CarveBoxOffTop( 0.05f );

	std::vector<Vertex_PCU> v1;

	m_font->AddVertsForTextInBox2D( v1 , box , 1.5f ,"ms/frame ="+std::to_string((int)(m_fps/60))+ "(FPS =" + std::to_string( m_fps )+")" , Rgba8( 0 , 0 , 100 , 255 ) , 1.f , Vec2( 0.95f , 0.0f ) );
	
	g_theRenderer->BeginCamera( *m_UICamera );
	g_theRenderer->BindTexture( m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( v1 );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera( *m_UICamera );

}

void Game::DisplayHUD()
{
	g_theRenderer->BeginCamera( *m_HUDCamera );
	AABB2 hud = AABB2( Vec2(0.f,0.f) , Vec2( 1600.f , 117.f ) );
	AABB2 gun = AABB2( Vec2( 330.f , 117.f ) , Vec2( 1270.f , 1057.f ) );
	g_theRenderer->BindTexture( g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Hud_Base.png" ) );
	g_theRenderer->DrawAABB2D( hud,Rgba8(255,255,255,255) );
	Vec2 minUV , maxUV;
	m_weaponSprites->GetSpriteUVs( minUV , maxUV , 0 );
	std::vector<Vertex_PCU> gunVerts;
	AppendAABB2(gunVerts ,gun , Rgba8( 255 , 255 , 255 , 255 ) , minUV , maxUV );
	g_theRenderer->BindTexture( &(m_weaponSprites->GetTexture()) );
	g_theRenderer->DrawVertexArray( gunVerts );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->EndCamera( *m_HUDCamera );
}

void Game::DisplayDebugInfo()
{
	g_theRenderer->SetDepthTest( COMPARE_ALWAYS );
	g_theRenderer->SetModalMatrix( Mat44() );
	g_theRenderer->DrawMesh( m_basis );

	Transform compassTransform;
	compassTransform.SetPosition( m_camera->m_transform.m_position + 0.1f * m_camera->m_transform.ToMatrix().GetIBasis3D() );
	compassTransform.m_scale = Vec3( 0.01f , 0.01f , 0.01f );

	g_theRenderer->SetModalMatrix( compassTransform.ToMatrix() );
	g_theRenderer->DrawMesh( m_basis );

	DisplayUI();
}

void Game::SoundTest()
{
	SoundPlaybackID s = 0;
	float volume = 0.f;
	float balance = 0.f;
	float speed = 0;

	volume = m_rng.RollRandomFloatInRange( 0.5f , 1.0f );
	balance = m_rng.RollRandomFloatInRange( -1.f , 1.f );
	speed = m_rng.RollRandomFloatInRange( 0.5f , 2.0f );

	if ( g_theInput->WasKeyJustPressed( F5 ) )
	{
		s = g_theAudio->PlaySound( testSound , false , volume , balance , speed );
	}
}


void Game::UpdatePossesingEntityFromKeyBoard()
{
	if ( m_player == nullptr && g_theInput->WasKeyJustPressed( F3 ) )
	{
		Mat44 cameraTransform = m_camera->m_transform.ToMatrix();
		Vec3 forwardVector = cameraTransform.GetIBasis3D();
		Vec3 cameraPos = m_camera->m_transform.m_position;
		if ( m_world->m_currentMap != nullptr )
		{
			std::vector<Entity*>& allEntities = m_world->m_currentMap->m_entities;
			float		minDistanceSq = 4.f;
			int			resultIndex = -1;
			if ( allEntities.size() > 0 )
			{
				for ( size_t index = 0; index < allEntities.size(); index++ )
				{
					float distanceSq = DotProduct3D( cameraPos - Vec3(allEntities[ index ]->m_position,0.f) , forwardVector );
					if ( distanceSq < minDistanceSq )
					{
						resultIndex = ( int ) index;
						minDistanceSq = distanceSq;
					}
				}
				if ( resultIndex == -1 )
				{
					m_player = nullptr;
				}
				else
				{
					m_player = allEntities[ resultIndex ];
					m_camera->m_transform.m_position = Vec3( m_player->m_position , m_camera->m_transform.m_position.z );
				}
			}
		}
	}
	else if ( m_player != nullptr && g_theInput->WasKeyJustPressed( F3 ) )
	{
		m_player = nullptr;
	}
}

