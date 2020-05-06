#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/AABB3.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/VertexLit.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/ObjFileLoader.hpp"
#include "Engine/Core/D3D11Common.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SwapChain.hpp"

#define UNUSED(x) (void)(x);

struct Light
{
	light_t light;
	eLightTypes type;
	bool shouldFollwCamera = false;
	bool shouldAnimate = false;
};

eDebugRenderMode currentMode = DEBUG_RENDER_USE_DEPTH;
Light tempLight[8];
Rgba8 lightColor = Rgba8( 255 , 255 , 255 , 255 );
int currentLightIndex = 0;
fog_t fog;


bool Help( EventArgs& args )
{
	UNUSED( args );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Currently Supported Commands" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "help");
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "quit" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "close" );

	g_theConsole.PrintString( Rgba8( 255 , 255 , 255 , 255 ) , "Debug Render Commands" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "debug_render_enabled" );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Parameters: render=(bool)" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "debug_add_world_point" );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Parameters: position=(Vec3), duration=(float), size=(float)" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "debug_add_screen_quad" );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Parameters: mins=(Vec2), maxs=(Vec2), duration=(float)" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "debug_add_world_wiresphere" );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Parameters: position=(Vec3), duration=(float), radius=(float)" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "debug_add_screen_point" );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Parameters: position=(Vec2), duration=(float), size=(float)" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "debug_add_world_wirequad" );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Parameters: mins=(Vec3), maxs=(Vec3), duration=(float)" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "debug_add_world_billboardtext" );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Parameters: text=(string), duration=(float), size=(float), pivot=(Vec2)" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "debug_add_screen_text" );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Parameters: text=(string), duration=(float), size=(float), pivot=(Vec2), alignment=(Vec2)" );
	g_theConsole.PrintString( Rgba8( 255 , 0 , 255 , 255 ) , "Example Usage:" );
	g_theConsole.PrintString( Rgba8( 255 , 0 , 255 , 255 ) , "debug_add_world_point:position=10,10,10|duration=10" );
	g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , "light_set_ambient_color" );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 100 , 255 ) , "Parameters: color=(Rgba8)" );
	g_theConsole.PrintString( Rgba8( 255 , 0 , 255 , 255 ) , "Example Usage:" );
	g_theConsole.PrintString( Rgba8( 255 , 0 , 255 , 255 ) , "light_set_ambient_color:color=100,100,100,100" );

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

bool DebugSetAmbientColor( EventArgs& args )
{
	Rgba8 color = args.GetValue( "color" , Rgba8( 100 , 0 , 0 , 255 ) );
	g_theRenderer->SetAmbientColor( color );
	lightColor = color;
	return true;
}

GPUMesh* me;

Game::Game()
{
	MeshBuilderOptions m;
	m.calculateNoramls = true;
	m.generateTangents = true;


	me = OBJLoader::LoadObjFileIntoGpuMesh( m , "Data/Objs/test_models/scifi_fighter/mesh.obj" );
	//x.ParseObjFile( "Data/Objs/test_models/scifi_fighter/mesh.obj" );

	dissolveMaterial = new Material();
	dissolveMaterial->CreateFromFile( "Data/XML/dissolve.xml" );

	rng= RandomNumberGenerator();
	m_camera=new Camera();
	m_devConsoleCamera = new Camera();
	m_UICamera = new Camera();

	m_camera->SetProjectionPerspective( 60.f ,16.f/9.f, -0.1f , -100.f );
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );
	m_UICamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 160.f , 90.f ) );
	m_devConsoleCamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 ,0 , 0 , 255 ) , 0.f , 0 );
	m_UICamera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );
	m_camera->SetClearMode( CLEAR_COLOR_BIT /*| CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT*/ , Rgba8( 0 , 0 , 0 , 255 ) , 1.f , 0 );

	m_font = g_theRenderer->GetOrCreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

	tileDiffuse = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/tile_diffuse.png" );
	tileNormal = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/tile_normal.png" );
	noise = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/noise.png" );


	g_theEventSystem.SubscribeToEvent( "help" , Help );
	g_theEventSystem.SubscribeToEvent( "quit" , Quit );
	g_theEventSystem.SubscribeToEvent( "close" , Close);
	g_theEventSystem.SubscribeToEvent( "light_set_ambient_color" , DebugSetAmbientColor );
	

	//DebugRenderSystem::sDebugRenderer->TakeWorldCamera( m_camera );

	g_theConsole.TakeCamera( m_devConsoleCamera );
	g_theConsole.SetTextSize( 2.5f );

	Rgba8 WHITE = Rgba8( 255 , 255 , 255 , 255 );
	Rgba8 GREEN = Rgba8( 0 , 255 , 0 , 255 );
	Rgba8 BLUE = Rgba8( 0 , 0 , 255 , 255 );
	Rgba8 CYAN = Rgba8( 0 , 255 , 255 , 255 );
	Rgba8 RED = Rgba8( 255 , 0 , 0 , 255 );
	Rgba8 YELLOW = Rgba8( 255 , 255 , 0 , 255 );

	mesh = new GPUMesh( g_theRenderer );
	sphere = new GPUMesh( g_theRenderer );

	std::vector<VertexLit> sphereVerts;
	std::vector<unsigned int> sphereIndices;
	Vec3 centre = Vec3( 0.f , 0.f , 0.f );

	//AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , 3.f , 64 , 32 , WHITE );
	AppendIndexedUVSPhere( 64 , 32 , sphereVerts , sphereIndices , 2.f , centre , WHITE );

	sphere->UpdateVertices((unsigned int) sphereVerts.size() , &sphereVerts[0] );
	sphere->UpdateIndices((unsigned int) sphereIndices.size() , &sphereIndices[0] );

	quad = new GPUMesh(g_theRenderer);
	std::vector<VertexLit> quadVertices;
	std::vector<unsigned int> quadIndices;

	AABB2 sqr = AABB2( Vec2( -2.f , -2.f ) , Vec2( 2.f , 2.f ) );

	AppendIndexedQuad( quadVertices , quadIndices , sqr , WHITE );
	
	quad->UpdateVertices( (unsigned int)quadVertices.size() , &quadVertices[ 0 ] );
	quad->UpdateIndices( ( unsigned int ) quadIndices.size() , &quadIndices[ 0 ] );

	g_theInput->ClipSystemCursor();
	g_theInput->SetCursorMode( MODE_RELATIVE );

	tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/gg.png" );

	for ( int i = 0; i < 8; i++ )
	{
		tempLight[i].light.position = Vec3( 0.f , 0.f , 0.f );
		tempLight[i].light.intensity = 0.f;
		tempLight[i].light.color = Vec3( 1.f , 1.f , 1.f );
		tempLight[ i ].type = POINT_LIGHT;
	}

	tempLight[ 0 ].light.intensity = 0.5f;


	std::vector<VertexLit> cubeVertices;
	std::vector<unsigned int> cubeIndices;

	AppendIndexedCuboid( cubeVertices , cubeIndices , Vec3( -1.f , -1.f , -1.f ) , Vec3( 1.f , 1.f , 1.f ) , WHITE );

	mesh->UpdateVertices( ( unsigned int ) cubeVertices.size() , &cubeVertices[ 0 ] );
	mesh->UpdateIndices( ( unsigned int ) cubeIndices.size() , &cubeIndices[0] );

	cubeTransform.m_position = Vec3( 1.f , 0.5f , -20.f );
	quadTransform.m_position = Vec3( 6.f , 0.5f , -20.f );
	sphereTransform.m_position = Vec3( -5.f , 0.5f , -20.f );

	fresnalData.fresnalColor = Vec3( 0.f , 1.f , 0.f );
	fresnalData.fresnalFactor = 1.f;
	fresnalData.fresnalPower = 1.f;

	dissolveData.burnAmount = 0.f;
	dissolveData.burnStartColor = Vec3( 0.f , 0.f , 1.f );
	dissolveData.burnEndColor = Vec3( 1.f , 0.f , 1.f );
	dissolveData.burnEdgeWidth = 1.f;

	//g_theRenderer->CreateRasterState( D3D11_FILL_SOLID , D3D11_CULL_BACK );

}

Game::~Game()
{
	delete m_devConsoleCamera;
	delete m_camera;
	delete m_UICamera;
	delete mesh;
	delete sphere;
	delete quad;
	delete me;
	delete dissolveMaterial;
}

void Game::Update( float deltaseconds )
{
	if ( g_theConsole.IsOpen() )
	{
		return;
	}

	ToggleDevConsole();
	ToggleShaders();
	ToggleLightPositions();
	ToggleAmbientLightIntensity();
	ToggleSpecularPower();
	ToggleSpecularFactor();
	ToggleSpecularAttenuation();
	ToggleLightIntensity();
	ToggleRenderModes();
	UpdateDissolveDetails();
	ToggleLights();
	ToggleLightTypes();
	ToggleBloom();
	ToggleColorTone();

	if ( g_theConsole.IsOpen() )
	{
		return;
	}

	if ( g_theInput->WasKeyJustPressed( 0x1B ) )
	{
		g_theapp->HandleQuitRequested();
	}

	float speedMultiplier = 1.f;
	if ( g_theInput->IsKeyPressed( 0x10 ) )
	{
		speedMultiplier = 2.f;
	}
	else
	{
		speedMultiplier = 1.f;
	}
	

	cubeTransform.m_rotationPitchRollYawDegrees.x += deltaseconds * 10.f;

	if ( cubeTransform.m_rotationPitchRollYawDegrees.x >= 360.f )
	{
		cubeTransform.m_rotationPitchRollYawDegrees.x = 0.f;
	}

	cubeTransform.m_rotationPitchRollYawDegrees.y += deltaseconds * 20.f;

	if ( cubeTransform.m_rotationPitchRollYawDegrees.y >= 360.f )
	{
		cubeTransform.m_rotationPitchRollYawDegrees.y = 0.f;
	}

	
	sphereTransform.SetRotationFromPitchRollYawDegrees( 20.f* (float) GetCurrentTimeSeconds() , 10.f * ( float ) GetCurrentTimeSeconds() , 0.f );
	//sphereTransform.m_rotationPitchRollYawDegrees.x = 20.f * ( float ) GetCurrentTimeSeconds();

	Mat44 modal = m_camera->m_transform.ToMatrix();
	Vec3 forwardVec = modal.GetKBasis3D();
	Vec3 rightVec = modal.GetIBasis3D();

	
	m_cameraRotation.y -= g_theInput->m_relativeMovement.x * 0.1f;
	m_cameraRotation.x -= g_theInput->m_relativeMovement.y * 0.1f;

	m_camera->m_transform.SetRotationFromPitchRollYawDegrees( m_cameraRotation.x , m_cameraRotation.y , m_cameraRotation.z );
	
	//m_cameraRotation.y = Clamp( m_cameraRotation.y , -90.f , 90.f );
	//m_cameraRotation.x = Clamp( m_cameraRotation.x , -85.f , 85.f );

	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		m_camera->m_transform.m_position -= forwardVec * deltaseconds * 8.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		m_camera->m_transform.m_position += forwardVec * deltaseconds * 8.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 'A' ) )
	{
		m_camera->m_transform.m_position -= rightVec * deltaseconds * 8.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 'D' ) )
	{
		m_camera->m_transform.m_position += rightVec * deltaseconds * 8.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 'C' ) )
	{
		m_camera->m_transform.m_position.y += 1.f * deltaseconds * 8.f * speedMultiplier;
	}

	if ( g_theInput->IsKeyPressed( 0x20 ) )
	{
		m_camera->m_transform.m_position.y -= 1.f * deltaseconds * 8.f * speedMultiplier;
	}

	

}

void Game::Render()
{
	Texture* backBuffer = g_theRenderer->m_swapChain->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->GetOrCreatematchingRenderTarget( backBuffer );
	Texture* bloomTarget = g_theRenderer->GetOrCreatematchingRenderTarget( backBuffer );
	m_camera->SetColorTarget( colorTarget );
	m_camera->SetColorTarget( 1 , bloomTarget );
	g_theRenderer->BeginCamera(*m_camera);
	m_camera->CreateDepthStencilTarget( g_theRenderer );
	g_theRenderer->SetDepthTest();
	g_theRenderer->BindDepthStencil( m_camera->m_backBuffer );
	
	tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/gg.png" );
	
	g_theRenderer->BindTexture( tileDiffuse , TEXTURE_SLOT_DIFFUSE );
	g_theRenderer->BindTexture( tileNormal  , TEXTURE_SLOT_NORMAL );

	switch ( currentShaderNumber )
	{
	case 0: g_theRenderer->BindShader( "Data/Shaders/lit.hlsl" );
		g_theRenderer->DisableFog();
		break;
	case 1: g_theRenderer->BindShader( "Data/Shaders/Default.hlsl" );
		g_theRenderer->DisableFog();
		break;
	case 2: g_theRenderer->BindShader( "Data/Shaders/Normals.hlsl" );
		g_theRenderer->DisableFog();
		break;
	case 3: g_theRenderer->BindShader( "Data/Shaders/tangents.hlsl" );
		g_theRenderer->DisableFog();
		break;
	case 4: g_theRenderer->BindShader( "Data/Shaders/bitangents.hlsl" );
		g_theRenderer->DisableFog();
		break;
	case 5: g_theRenderer->BindShader( "Data/Shaders/surfaceNormals.hlsl" );
		g_theRenderer->DisableFog();
		break;
	default:
		break;
	}

	g_theRenderer->SetAmbientLight( lightColor , ambientLightIntensity );
	g_theRenderer->SetSpecularPower( specularPower );
	g_theRenderer->SetSpecularFactor( specularFactor );
	tempLight[currentLightIndex].light.specularAttunation = specularAttenuation;
	
	for ( int i = 0; i < 8; i++ )
	{
		if ( tempLight[i].shouldAnimate )
		{
			float zOffset = -7.f;
			tempLight[ i ].light.position = Vec3( CosDegrees( roataionAngleForAnimation ) * 9.f , 0.5f , ( SinDegrees( roataionAngleForAnimation ) * 5.f ) + zOffset );
			roataionAngleForAnimation += 1.f;

			if ( roataionAngleForAnimation >= 360.f )
			{
				roataionAngleForAnimation = 0.f;
			}
		}
	}
	
	for ( int i = 0; i < 8; i++ )
	{
		if ( tempLight[ i ].shouldFollwCamera )
		{
			tempLight[ i ].light.position = m_camera->m_transform.m_position;
		}
	}

	for ( int i = 0; i < 8; i++ )
	{
		g_theRenderer->EnableLight( i , tempLight[i].light );
	}

	
	g_theRenderer->SetModalMatrix( quadTransform.ToMatrix() );
	g_theRenderer->DrawMesh( quad );

	/*g_theRenderer->SetModalMatrix( cubeTransform.ToMatrix() );
	g_theRenderer->DrawMesh( mesh );*/

	g_theRenderer->SetModalMatrix( sphereTransform.ToMatrix() );
	g_theRenderer->DrawMesh( sphere );

	Texture* t = g_theRenderer->GetOrCreateTextureFromFile( "Data/Objs/test_models/scifi_fighter/diffuse.jpg" );

	g_theRenderer->BindTexture( t,TEXTURE_SLOT_DIFFUSE );
	Transform objTransform;
	objTransform.SetPosition( Vec3( 0.f , 0.f , -10.f  ) );
	g_theRenderer->SetModalMatrix( objTransform.ToMatrix() );
	g_theRenderer->DrawMesh( me );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->SetDepthTest( COMPARE_LEQUAL );
	g_theRenderer->BindShader( "Data/Shaders/fresnal.hlsl" );
	g_theRenderer->BindMaterialData( ( void* ) &fresnalData , sizeof( fresnalData ) );
	g_theRenderer->SetBlendMode( BlendMode::ALPHA );
	g_theRenderer->SetModalMatrix( sphereTransform.ToMatrix() );
	g_theRenderer->DrawMesh( sphere );

	dissolveMaterial->SetData( dissolveData );
	g_theRenderer->BindMaterial( dissolveMaterial );
	g_theRenderer->SetModalMatrix( cubeTransform.ToMatrix() );
	g_theRenderer->DrawMesh( mesh );
	m_camera->SetColorTarget( backBuffer );
	g_theRenderer->EndCamera(*m_camera);

	if ( isBloomOn )
	{
		Shader* blur = g_theRenderer->GetOrCreateShader( "Data/Shaders/blur.hlsl" );;
		Texture* blurredBloom = g_theRenderer->GetOrCreatematchingRenderTarget( bloomTarget );
		g_theRenderer->StartEffect( blurredBloom , bloomTarget , blur );
		g_theRenderer->BindTexture( bloomTarget , TEXTURE_SLOT_USER );
		g_theRenderer->EndEffect();

		Shader* combineImg = g_theRenderer->GetOrCreateShader( "Data/Shaders/combine.hlsl" );;
		Texture* finalImage = g_theRenderer->GetOrCreatematchingRenderTarget( colorTarget );
		g_theRenderer->StartEffect( finalImage , colorTarget , combineImg );
		g_theRenderer->BindTexture( blurredBloom , TEXTURE_SLOT_USER , 0 );
		g_theRenderer->BindTexture( colorTarget , TEXTURE_SLOT_USER , 1 );
		g_theRenderer->EndEffect();
		g_theRenderer->CopyTexture( backBuffer , finalImage );
		g_theRenderer->ReleaseRenderTarget( blurredBloom );
		g_theRenderer->ReleaseRenderTarget( finalImage );
	}
	else
	{
		Shader* combineImg = g_theRenderer->GetOrCreateShader( "Data/Shaders/combine.hlsl" );
		Texture* finalImage = g_theRenderer->GetOrCreatematchingRenderTarget( colorTarget );
		g_theRenderer->StartEffect( finalImage , colorTarget , combineImg );
		g_theRenderer->BindTexture( bloomTarget , TEXTURE_SLOT_USER );
		g_theRenderer->BindTexture( colorTarget , TEXTURE_SLOT_USER , 1 );
		g_theRenderer->EndEffect();
		g_theRenderer->CopyTexture( backBuffer , finalImage );
		g_theRenderer->ReleaseRenderTarget( finalImage );
	}

	if ( isColorToneOn )
	{
		Texture* toneMapTarget = g_theRenderer->GetOrCreatematchingRenderTarget( colorTarget );
		Texture* currentView = g_theRenderer->GetOrCreatematchingRenderTarget( backBuffer );
		g_theRenderer->CopyTexture( currentView , backBuffer );
		Shader* color = g_theRenderer->GetOrCreateShader( "Data/Shaders/colorTone.hlsl" );
		g_theRenderer->StartEffect( toneMapTarget , currentView , color );
		g_theRenderer->BindMaterialData( ( void* ) &m_toneMapTransform , sizeof( m_toneMapTransform ) );
		g_theRenderer->EndEffect();
		g_theRenderer->CopyTexture( backBuffer , toneMapTarget );
		g_theRenderer->ReleaseRenderTarget( currentView );
		g_theRenderer->ReleaseRenderTarget( toneMapTarget );
	}
		g_theRenderer->ReleaseRenderTarget( bloomTarget );
		g_theRenderer->ReleaseRenderTarget( colorTarget );

	DisplayUIText();
	
	for ( int i = 0; i < 8; i++ )
	{
		DebugAddWorldPoint( tempLight[ i ].light.position , 0.1f , Rgba8( 255 , 255 , 255 , 255 ) , 0.f , DEBUG_RENDER_USE_DEPTH );
	}
	//
	DebugRenderSystem::sDebugRenderer->DebugRenderWorldToCamera( m_camera );
	//
	//if ( !g_theConsole.IsOpen() )
	//{
	//	DebugRenderSystem::sDebugRenderer->DebugRenderToScreen( m_camera->GetColorTarget() );
	//}
	//
	if ( g_theConsole.IsOpen() )
	{
		g_theConsole.Render( *g_theRenderer , *m_devConsoleCamera , 2.5f , 1.5f );
	}
}

void Game::UpdateCamera()
{
	
}

void Game::ToggleDebugCamera()
{
	
}

void Game::ToggleRenderModes()
{
	if ( g_theInput->WasKeyJustPressed( '1' ) )
	{
		renderMode = 1;
		currentMode = DEBUG_RENDER_ALWAYS;
	}

	if ( g_theInput->WasKeyJustPressed( '2' ) )
	{
		renderMode = 2;
		currentMode = DEBUG_RENDER_USE_DEPTH;
	}

	if ( g_theInput->WasKeyJustPressed( '3' ) )
	{
		renderMode = 3;
		currentMode = DEBUG_RENDER_XRAY;
	}
}

void Game::ToggleDevConsole()
{
	if ( g_theInput->WasKeyJustPressed( 0xC0 ) )
	{
		devConsoleOpen = !devConsoleOpen;
		g_theConsole.SetIsOpen(devConsoleOpen);
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
		g_theInput->HideSystemCursor();
	}
}

void Game::ToggleDebugging()
{
	
}

void Game::ToggleShaders()
{
	if ( g_theInput->m_characters.empty() )
	{
		return;
	}

	if ( g_theInput->m_characters.front() == '.' )
	{
		currentShaderNumber++;

		if ( currentShaderNumber >= MAX_SHADERS )
		{
			currentShaderNumber = 0;
		}
	}

	if ( g_theInput->m_characters.front() == ',' )
	{
		currentShaderNumber--;

		if ( currentShaderNumber < 0 )
		{
			currentShaderNumber = MAX_SHADERS - 1;
		}
	}

}

void Game::ToggleLightPositions()
{
	if ( g_theInput->WasKeyJustPressed( F5 ) )
	{
		tempLight[currentLightIndex].light.position = Vec3( 0.f , 0.f , 0.f );
		tempLight[ currentLightIndex ].shouldFollwCamera = false;
		tempLight[ currentLightIndex ].shouldAnimate = false;

	}

	if ( g_theInput->WasKeyJustPressed( F6 ) )
	{
		tempLight[currentLightIndex].light.position = m_camera->m_transform.m_position;
		tempLight[ currentLightIndex ].shouldFollwCamera = false;
		tempLight[ currentLightIndex ].shouldAnimate = false;

		Mat44 modal = m_camera->m_transform.ToMatrix();
		Vec3 forwardVec = modal.GetKBasis3D();
		forwardVec = -forwardVec.GetNormalized();

		tempLight[ currentLightIndex ].light.direction = forwardVec;
	}

	if ( g_theInput->WasKeyJustPressed( F7 ) )
	{
		tempLight[ currentLightIndex ].shouldFollwCamera = true;
		tempLight[ currentLightIndex ].shouldAnimate = false;
	}

	if ( g_theInput->WasKeyJustPressed( F8 ) )
	{
		tempLight[ currentLightIndex ].shouldFollwCamera = false;
		tempLight[ currentLightIndex ].shouldAnimate = true;
	}
	
}

void Game::ToggleAmbientLightIntensity()
{
	if ( g_theInput->m_characters.empty() )
	{
		return;
	}

	if ( g_theInput->m_characters.front() == '=' || g_theInput->m_characters.front() == '+' )
	{
		ambientLightIntensity += 0.01f;
	}

	if ( g_theInput->m_characters.front() == '-' || g_theInput->m_characters.front() == '_' )
	{
		ambientLightIntensity -= 0.01f;
	}

	ambientLightIntensity = Clamp( ambientLightIntensity , 0.f , 1.f );
}

void Game::ToggleSpecularPower()
{
	if ( g_theInput->m_characters.empty() )
	{
		return;
	}

	if ( g_theInput->m_characters.front() == '}' || g_theInput->m_characters.front() == ']' )
	{
		specularPower += 10.f;
	}

	if ( g_theInput->m_characters.front() == '{' || g_theInput->m_characters.front() == '[' )
	{
		specularPower -= 10.f;
	}

	if ( specularPower < 1.f )
	{
		specularPower = 1.f;
	}
}

void Game::ToggleSpecularFactor()
{
	if ( g_theInput->IsKeyPressed( 'K' ) )
	{
		specularFactor += 0.01f;
	}

	if ( g_theInput->IsKeyPressed( 'J' ) )
	{
		specularFactor -= 0.01f;
	}

	specularFactor = Clamp( specularFactor , 0.f , 1.f );
}

void Game::ToggleSpecularAttenuation()
{
	if ( g_theInput->WasKeyJustPressed( 'T' ) )
	{
		currentAttenuationNumber+=1;
		currentAttenuationNumber = currentAttenuationNumber % 3;
	}

	if ( currentAttenuationNumber == 0 )
	{
		specularAttenuation = Vec3( 0.f , 0.f , 1.f );
	}

	if ( currentAttenuationNumber == 1 )
	{
		specularAttenuation = Vec3( 0.f , 1.f , 0.f );
	}

	if ( currentAttenuationNumber == 2 )
	{
		specularAttenuation = Vec3( 1.f , 0.f , 0.f );
	}
}

void Game::DisplayUIText()
{

	AABB2 box = AABB2(m_UICamera->GetOrthoBottomLeft(),m_UICamera->GetOrthoTopRight() );
	box.CarveBoxOffLeft( 0.5f );
	//box.CarveBoxOffTop( 0.5f );

	std::string s1 = "F5    - Position At Origin";
	std::string s2 = "F6    - Position At Camera";
	std::string s3 = "F7    - Follow Camera";
	std::string s4 = "F8    - Animated Light";
	std::string s5 = " T    - Toggle Attenuation: ";
	std::string s6 = "[K,J] - Specular Factors: ";
	std::string s7 = "[-,+] - Ambient Intensity: ";
	std::string s8 = "[{,}] - Specular Power: ";
	std::string s9 = "[<,>] - Shaders: ";
	std::string s10 = "[U,I] - Light Intensity :";
	std::string s11 = "[N,M] - Current Light: ";
	std::string s12 = "[F] - Current Light Type:";
	std::string temp = "";

	std::string x1 = "Fresnal is applied on Sphere";
	std::string x2 = "Fog is applied on Sphere and Quad";
	std::string x3 = "Dissolve is applied on Cube";
	std::string x4 = "[Z,X] - CurrentDissolve Factor";

	std::vector<Vertex_PCU> v1;
	m_font->AddVertsForTextInBox2D( v1 , box , 2.f , s1 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.98f ) );

	std::vector<Vertex_PCU> v2;
	m_font->AddVertsForTextInBox2D( v2 , box , 2.f , s2 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.95f ) );

	std::vector<Vertex_PCU> v3;
	m_font->AddVertsForTextInBox2D( v3 , box , 2.f , s3 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.92f ) );

	std::vector<Vertex_PCU> v4;
	m_font->AddVertsForTextInBox2D( v4 , box , 2.f , s4 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.89f ) );

	std::vector<Vertex_PCU> v5;
	temp = "(" + std::to_string( specularAttenuation.x ) + "," + std::to_string( specularAttenuation.y ) + "," + std::to_string( specularAttenuation.z ) + ")";
	s5 += temp;

	m_font->AddVertsForTextInBox2D( v5 , box , 2.f , s5 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.86f ) );

	std::vector<Vertex_PCU> v6;
	temp = std::to_string( specularFactor );
	s6 += temp;

	m_font->AddVertsForTextInBox2D( v6 , box , 2.f , s6 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.83f ) );

	std::vector<Vertex_PCU> v7;
	temp = std::to_string( ambientLightIntensity );
	s7 += temp;

	m_font->AddVertsForTextInBox2D( v7 , box , 2.f , s7 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.80f ) );

	std::vector<Vertex_PCU> v8;
	temp = std::to_string( specularPower );
	s8 += temp;

	m_font->AddVertsForTextInBox2D( v8 , box , 2.f , s8 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.77f ) );

	std::vector<Vertex_PCU> v10;
	temp = std::to_string( tempLight[currentLightIndex].light.intensity );
	s10 += temp;

	m_font->AddVertsForTextInBox2D( v10 , box , 2.f , s10 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.71f ) );

	std::vector<Vertex_PCU> v11;
	temp = std::to_string( currentLightIndex );
	s11 += temp;

	m_font->AddVertsForTextInBox2D( v11 , box , 2.f , s11 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.68f ) );

	std::vector<Vertex_PCU> v12;
	
	switch ( tempLight[ currentLightIndex ].type )
	{
	case DIRECTIONAL_LIGHT:
		temp = "Directional Light";
		break;
	case  SPOT_LIGHT:
		temp = "Spot Light";
		break;
	case POINT_LIGHT:
		temp = "Point Light";
		break;
	}

	s12 += temp;
	m_font->AddVertsForTextInBox2D( v12 , box , 2.f , s12 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.65f ) );

	std::vector<Vertex_PCU> v9;
	if ( currentShaderNumber == 0 )
	{
		temp = "Lit";
	}
	if ( currentShaderNumber == 1 )
	{
		temp = "Default";
	}
	if ( currentShaderNumber == 2 )
	{
		temp = "Normals";
	}
	if ( currentShaderNumber == 3 )
	{
		temp = "Tangents";
	}
	if ( currentShaderNumber == 4 )
	{
		temp = "Bitangents";
	}
	if ( currentShaderNumber == 5 )
	{
		temp = "SurfaceNormals";
	}
	s9 += temp;

	m_font->AddVertsForTextInBox2D( v9 , box , 2.f , s9 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.74f ) );

	std::vector<Vertex_PCU> k1;
	m_font->AddVertsForTextInBox2D( k1 , box , 2.f , x1 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.12f ) );

	std::vector<Vertex_PCU> k2;
	m_font->AddVertsForTextInBox2D( k2 , box , 2.f , x2 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.09f ) );

	std::vector<Vertex_PCU> k3;
	m_font->AddVertsForTextInBox2D( k3 , box , 2.f , x3 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.06f ) );

	std::vector<Vertex_PCU> k4;
	temp = std::to_string( currentBurnAmt );
	x4 += temp;
	m_font->AddVertsForTextInBox2D( k4 , box , 2.f , x4 , Rgba8( 100 , 100 , 100 , 255 ) , 1.f , Vec2( 0.02f , 0.03f ) );



	g_theRenderer->BeginCamera( *m_UICamera );
	g_theRenderer->BindShader( "Data/Shaders/DefaultVertexPCU.hlsl" );
	g_theRenderer->BindTexture( m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( v1 );
	g_theRenderer->DrawVertexArray( v2 );
	g_theRenderer->DrawVertexArray( v3 );
	g_theRenderer->DrawVertexArray( v4 );
	g_theRenderer->DrawVertexArray( v5 );
	g_theRenderer->DrawVertexArray( v6 );
	g_theRenderer->DrawVertexArray( v7 );
	g_theRenderer->DrawVertexArray( v8 );
	g_theRenderer->DrawVertexArray( v9 );
	g_theRenderer->DrawVertexArray( v10 );
	g_theRenderer->DrawVertexArray( v11 );
	g_theRenderer->DrawVertexArray( v12 );
	g_theRenderer->DrawVertexArray( k1 );
	g_theRenderer->DrawVertexArray( k2 );
	g_theRenderer->DrawVertexArray( k3 );
	g_theRenderer->DrawVertexArray( k4 );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera( *m_UICamera );

}

void Game::ToggleLightIntensity()
{
	if ( g_theInput->WasKeyJustPressed( 'I' ) )
	{
		tempLight[currentLightIndex].light.intensity += 0.1f;
	}

	if ( g_theInput->WasKeyJustPressed( 'U' ) )
	{
		tempLight[currentLightIndex].light.intensity -= 0.1f;
	}

	tempLight[currentLightIndex].light.intensity = Clamp( tempLight[currentLightIndex].light.intensity , 0.f , 1.f );

}

void Game::UpdateDissolveDetails()
{
	if ( g_theInput->WasKeyJustPressed( 'Z' ) )
	{
		currentBurnAmt += 0.1f;
		currentBurnAmt = Clamp( currentBurnAmt , 0.f , 1.f );

		dissolveData.burnAmount = currentBurnAmt;
		dissolveData.burnEdgeWidth = 1 + ( 2 * currentBurnAmt );
	}

	if ( g_theInput->WasKeyJustPressed( 'X' ) )
	{
		currentBurnAmt -= 0.1f;
		currentBurnAmt = Clamp( currentBurnAmt , 0.f , 1.f );

		dissolveData.burnAmount = currentBurnAmt;
		dissolveData.burnEdgeWidth = 1 + ( 2 * currentBurnAmt );
	}

}

void Game::ToggleLights()
{
	if ( g_theInput->WasKeyJustPressed( 'M' ) )
	{
		currentLightIndex += 1;

		if ( currentLightIndex > 7 )
		{
			currentLightIndex = 7;
		}
	}

	if ( g_theInput->WasKeyJustPressed( 'N' ) )
	{
		currentLightIndex -= 1;

		if ( currentLightIndex < 0 )
		{
			currentLightIndex = 0;
		}
	}
}

void Game::ToggleLightTypes()
{
	if ( g_theInput->WasKeyJustPressed( 'F' ) )
	{
		if ( tempLight[ currentLightIndex ].type == POINT_LIGHT )
		{
			tempLight[ currentLightIndex ].type = SPOT_LIGHT;
		}
		else if ( tempLight[ currentLightIndex ].type == SPOT_LIGHT )
		{
			tempLight[ currentLightIndex ].type = DIRECTIONAL_LIGHT;
		}
		else
		{
			tempLight[ currentLightIndex ].type = POINT_LIGHT;
		}
	}


	if ( tempLight[ currentLightIndex ].type == POINT_LIGHT )
	{
		tempLight[ currentLightIndex ].light.directionFactor = 0.f;
		tempLight[ currentLightIndex ].light.dotInnerAngle = -1.f;
		tempLight[ currentLightIndex ].light.dotOuterAngle = -1.f;
	}

	if ( tempLight[ currentLightIndex ].type == SPOT_LIGHT )
	{
		tempLight[ currentLightIndex ].light.directionFactor = 0.f;
		tempLight[ currentLightIndex ].light.dotInnerAngle = CosDegrees( 15.f );
		tempLight[ currentLightIndex ].light.dotOuterAngle = CosDegrees( 30.f );
	}

	if ( tempLight[ currentLightIndex ].type == DIRECTIONAL_LIGHT )
	{
		tempLight[ currentLightIndex ].light.directionFactor = 1.f;
		tempLight[ currentLightIndex ].light.dotInnerAngle = -1.f;
		tempLight[ currentLightIndex ].light.dotOuterAngle = -1.f;
	}

}

void Game::ToggleBloom()
{
	if ( g_theInput->WasKeyJustPressed( 0x71 ))
	{
		isBloomOn = !isBloomOn;
	}
}

void Game::ToggleColorTone()
{
	if ( g_theInput->WasKeyJustPressed( 0x72 ) )
	{
		isColorToneOn = !isColorToneOn;
	}
	if ( g_theInput->WasKeyJustPressed( 0x26 ) )
	{
		m_currentToneMap = ColorTone( ( m_currentToneMap + 1 ) % ColorTone::TOTAL_TONEMAPS );
	}
	if ( g_theInput->WasKeyJustPressed( 0x28 ) )
	{
		m_currentToneMap = ColorTone( ( m_currentToneMap - 1 ) );

		if ( m_currentToneMap < 0 )
		{
			m_currentToneMap = ColorTone( ColorTone::TOTAL_TONEMAPS - 1 );
			return;
		}

		m_currentToneMap = ColorTone( m_currentToneMap % ColorTone::TOTAL_TONEMAPS );
	}
	switch ( m_currentToneMap )
	{
	case ColorTone::NOTINT:
	{
		Mat44 temp;
		m_toneMapTransform = temp;
	}break;
	case ColorTone::GRAY:
	{
		Vec3 grayScale = Vec3( 0.2126f , 0.7152f , 0.0722f );
		m_toneMapTransform.SetBasisVectors3D( grayScale , grayScale , grayScale );
	}break;
	case ColorTone::REDBRON:
	{
		Vec3 newRed = Vec3( 0.393f , 0.769f , 0.189f );
		Vec3 newGreen = Vec3( 0.394f , 0.686f , 0.168f );
		Vec3 newBlue = Vec3( 0.272f , 0.534f , 0.131f );
		m_toneMapTransform.SetBasisVectors3D( newRed , newGreen , newBlue );
	}break;
	}
	
}
