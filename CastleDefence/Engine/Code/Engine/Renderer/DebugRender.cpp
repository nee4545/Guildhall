#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/D3D11Common.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/D3D11Common.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

constexpr float CLIENT_ASPECT = 16.f / 9.f;

extern RenderContext* g_theRenderer;
extern BitmapFont* g_theBitMapFont;

bool DebugRenderEnabled( EventArgs& args )
{
	bool value = args.GetValue( "render" , true );
	if ( value )
	{
		DebugRenderSystem::sDebugRenderer->EnableDebugRendering();
	}
	else
	{
		DebugRenderSystem::sDebugRenderer->DisableDebugRendering();
	}

	return true;
}

bool DebugAddWorldPoint( EventArgs& args )
{
	Vec3	position = args.GetValue( "position" , Vec3( 0.f , 0.f , 0.f ) );
	float	duration = args.GetValue( "duration" , 5.f );
	float   size = args.GetValue( "size" , 0.2f );
	DebugAddWorldPoint( position , size , Rgba8( 255 , 255 , 255 , 255 ) , duration , DEBUG_RENDER_ALWAYS );
	return true;
}

bool DebugAddWorldWireSphere( EventArgs& args )
{
	Vec3	position = args.GetValue( "position" , Vec3( 0.f , 0.f , 0.f ) );
	float	duration = args.GetValue( "duration" , 5.f );
	float	radius = args.GetValue( "radius" , 0.5f );

	DebugAddWorldWireSphere( position , radius , Rgba8( 255 , 255 , 255 , 255 ) , duration , DEBUG_RENDER_ALWAYS );
	return true;
}

bool DebugAddWorldWireBounds( EventArgs& args )
{
	Vec3	mins = args.GetValue( "mins" , Vec3( 0.f , 0.f , 0.f ) );
	Vec3	maxs = args.GetValue( "maxs" , Vec3( 1.f , 1.f , 1.f ) );
	float	duration = args.GetValue( "duration" , 5.f );

	DebugAddWorldWireBounds( AABB3( mins , maxs ) , Rgba8( 255 , 0 , 0 , 255 ) , duration , DEBUG_RENDER_ALWAYS );
	return true;
}


bool DebugAddWorldBillboardText( EventArgs& args )
{
	std::string displayText = args.GetValue( "text" , "TEXT NOT READ CORRECTLY" );
	Vec3		position = args.GetValue( "position" , Vec3( 0.f , 0.f , 0.f ) );
	Vec2		pivot = args.GetValue( "pivot" , Vec2( 0.f , 0.f ) );
	float       size = args.GetValue( "size" , 1.f );
	float		duration = args.GetValue( "duration" , 10.f );

	DebugAddWorldBillboardText( position , pivot , size , Rgba8( 255 , 255 , 255 , 255 ) , Rgba8( 255 , 255 , 255 , 255 ) , duration , DEBUG_RENDER_ALWAYS , displayText.c_str() );
	return true;
}


bool DebugAddScreenPoint( EventArgs& args )
{
	Vec2	position = args.GetValue( "position" , Vec2( 0.f , 0.f ) );
	float	duration = args.GetValue( "duration" , 5.f );
	float	size = args.GetValue( "size" , 10.f );

	DebugAddScreenPoint( position , size , Rgba8( 255 , 255 , 255 , 255 ) , duration );
	return true;
}

bool DebugAddScreenQuad( EventArgs& args )
{
	Vec2	mins = args.GetValue( "mins" , Vec2( 0.f , 0.f ) );
	Vec2	maxs = args.GetValue( "maxs" , Vec2( 100.f , 100.f ) );
	float	duration = args.GetValue( "duration" , 5.f );

	DebugAddScreenQuad( AABB2( mins , maxs ) , Rgba8( 0 , 0 , 100 , 255 ) , duration );
	return true;
}

bool DebugAddScreenText( EventArgs& args )
{
	std::string displayText = args.GetValue( "text" , "TEXT NOT READ CORRECTLY" );
	Vec2		position = args.GetValue( "position" , Vec2( 0.f , 0.f ) );
	Vec2		alignment = args.GetValue( "align" , Vec2( 0.f , 0.f ) );
	Vec2		pivot = args.GetValue( "pivot" , Vec2( 0.f , 0.f ) );
	float		duration = args.GetValue( "duration" , 5.f );
	float		size = args.GetValue( "size" , 20.f );

	DebugAddScreenText( Vec4( position.x , position.y , alignment.x , alignment.y ) , pivot , size ,
		Rgba8( 255 , 255 , 255 , 255 ) , Rgba8( 255 , 255 , 255 , 255 ) , duration , displayText.c_str() );
	return true;
}

DebugRenderSystem* DebugRenderSystem::sDebugRenderer = new DebugRenderSystem();

DebugRenderObject::DebugRenderObject( eObjectTypes type, eDebugRenderMode mode , eDebugRenderSpace space , float duration , bool isBillboard /*= false */ )
{
	m_renderMode = mode;
	m_space = space;
	m_duration = duration;
	m_isBillboard = isBillboard;
	m_objectType = type;

	m_timer = new Timer();
	m_timer->SetSeconds( &Clock::gMasterClock,duration );
}

DebugPoint::DebugPoint( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 pos , float size , Rgba8 color , float duration , bool isBillboard /*= false*/ ):
	DebugRenderObject(OBJECT_POINT,mode,space,duration,isBillboard)
{
	m_position = pos;
	m_size = size;
	m_color = color;

	m_transform.m_position = pos;
	m_transform.m_scale *= m_size;

}

DebugPoint::DebugPoint( eDebugRenderMode mode , eDebugRenderSpace space , Transform transform , float size , Rgba8 color , float duration , bool isBillboard /*= false */ ) :
	DebugRenderObject( OBJECT_POINT , mode , space , duration , isBillboard )
{
	m_transform = transform;
	m_size = size;
	m_color = color;

	m_transform.m_scale *= m_size;
}

DebugPoint::DebugPoint( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_POINT , mode , space , duration , isBillboard )
{
	m_startColor = startColor;
	m_endColor = endColor;

	m_needsColorLerp = true;
	m_position = pos;
	m_size = size;
	
	m_transform.m_position = pos;
	m_transform.m_scale *= size;
}

DebugWireSphere::DebugWireSphere( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 pos , float size , Rgba8 color , float duration , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_WIRE_SPHERE , mode , space , duration , isBillboard )
{
	m_position = pos;
	m_size = size;
	m_color = color;

	m_transform.m_position = pos;
	m_transform.m_scale *= size;
}

DebugWireSphere::DebugWireSphere( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_WIRE_SPHERE , mode , space , duration , isBillboard )
{
	m_startColor = startColor;
	m_endColor = endColor;

	m_needsColorLerp = true;
	m_position = pos;
	m_size = size;

	m_transform.m_position = pos;
	m_transform.m_scale *= size;
}

DebugLine::DebugLine( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end ,Rgba8 color, float lineThickness , float duration , bool isBillboard /*= false */ ) :
	DebugRenderObject( OBJECT_LINE , mode , space , duration , isBillboard )
{
	m_startPos = start;
	m_endPos = end;
	m_color = color;
	m_startColor = color;
	m_endColor = color;
	m_lineThickness = lineThickness;
}

DebugLine::DebugLine( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end , Rgba8 p0StartColor , Rgba8 P0endColor , Rgba8 p1StartColor , Rgba8 p1EndColor , float lineThickness , float duration , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_LINE , mode , space , duration , isBillboard )
{
	m_startPos = start;
	m_endPos = end;
	m_lineThickness = lineThickness;

	m_p0StartColor = p0StartColor;
	m_p0EndColor = P0endColor;
	m_p1StartColor = p1StartColor;
	m_p1EndColor = p1EndColor;

	m_needsColorLerp = true;
}

DebugArrow::DebugArrow( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end ,Rgba8 color, float lineThickness , float tipHeight , float duration/*=-1.f */ , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_ARROW , mode , space , duration , isBillboard )
{
	m_startPos = start;
	m_endPos = end;
	m_color = color;
	m_lineThickness = lineThickness;
	m_tipHeight = tipHeight;

	m_startColor = color;
	m_endColor = color;
}

DebugArrow::DebugArrow( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end , Rgba8 p0StartColor , Rgba8 p0EndColor , Rgba8 p1StartColor , Rgba8 p1EndColor , float lineThickness , float tipHeight , float duration /*= -1.f */ , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_ARROW , mode , space , duration , isBillboard )
{
	m_startPos = start;
	m_endPos = end;

	m_lineThickness = lineThickness;
	m_tipHeight = tipHeight;

	m_p0StartColor = p0StartColor;
	m_p0EndColor = p0EndColor;
	m_p1StartColor = p1StartColor;
	m_p1EndColor = p1EndColor;

	m_needsColorLerp = true;
}

DebugQuad::DebugQuad( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color , float duration /*= -1.f */ , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_QUAD , mode , space , duration , isBillboard )
{
	m_p0 = p0;
	m_p1 = p1;
	m_p2 = p2;
	m_p3 = p3;

	m_color = color;
}

DebugQuad::DebugQuad( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color , Texture* tex, AABB2 uvs , float duration /*= -1.f */ , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_QUAD , mode , space , duration , isBillboard )
{
	m_p0 = p0;
	m_p1 = p1;
	m_p2 = p2;
	m_p3 = p3;

	m_color = color;
	m_texture = tex;
	m_uvs = uvs;

}

DebugQuad::DebugQuad( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 startColor , Rgba8 endColor , float duration /*= -1.f */ , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_QUAD , mode , space , duration , isBillboard )
{
	m_p0 = p0;
	m_p1 = p1;
	m_p2 = p2;
	m_p3 = p3;

	m_startColor = startColor;
	m_endColor = endColor;
	m_needsColorLerp = true;
}

DebugScreenText::DebugScreenText( eDebugRenderMode mode , eDebugRenderSpace space ,float size, Rgba8 color ,float duration, Vec2 pivot , Vec4 offset, std::string text ):
	DebugRenderObject( OBJECT_TEXT_SCREEN , mode , space , duration , false )
{
	m_color = color;
	m_pivot = pivot;
	m_offset = offset;
	m_text = text;
	m_size = size;
}

DebugScreenText::DebugScreenText( eDebugRenderMode mode , eDebugRenderSpace space ,float size, Rgba8 startColor , Rgba8 endColor , float duration , Vec2 pivot , Vec4 offset , std::string text ):
	DebugRenderObject( OBJECT_TEXT_SCREEN , mode , space , duration , false )
{
	m_startColor = startColor;
	m_endColor = endColor;
	m_color = startColor;
	m_offset = offset;
	m_text = text;
	m_pivot = pivot;
	m_size = size;
	m_needsColorLerp = true;
}

DebugWorldText::DebugWorldText( eDebugRenderMode mode , eDebugRenderSpace space , float size , Rgba8 color , Mat44 basis , float duration , Vec2 pivot , std::string text , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_TEXT_WORLD , mode , space , duration , isBillboard )
{
	m_size = size;
	m_pivot = pivot;
	m_basis = basis;
	m_color = color;
	m_text = text;
}


DebugWorldText::DebugWorldText( eDebugRenderMode mode , eDebugRenderSpace space , float size , Rgba8 startColor , Rgba8 endColor , Mat44 basis , float duration , Vec2 pivot , std::string text , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_TEXT_WORLD , mode , space , duration , isBillboard )
{
	m_size = size;
	m_text = text;
	m_pivot = pivot;
	m_basis = basis;
	m_color = startColor;
	m_startColor = startColor;
	m_endColor = endColor;
	m_needsColorLerp = true;
}

DebugBasis::DebugBasis( eDebugRenderMode mode , eDebugRenderSpace space , Mat44 basis , Rgba8 color , float duration ):
	DebugRenderObject( OBJECT_BASIS , mode , space , duration , false )
{
	m_basis = basis;
	m_color = color;
}


DebugBasis::DebugBasis( eDebugRenderMode mode , eDebugRenderSpace space , Mat44 basis , Rgba8 startColor , Rgba8 endColor , float duration ):
	DebugRenderObject( OBJECT_BASIS , mode , space , duration , false )
{
	m_basis = basis;
	m_startColor = startColor;
	m_endColor = endColor;
	m_color = startColor;
	m_needsColorLerp = true;
}


DebugWorldWireBox::DebugWorldWireBox( eDebugRenderMode mode , eDebugRenderSpace space , AABB3 box , Rgba8 startColor , Rgba8 endColor , float duration , bool isBillboard /*= false */ ) :
	DebugRenderObject( OBJECT_WIRE_BOX , mode , space , duration , isBillboard )
{
	m_box = box;
	m_startColor = startColor;
	m_endColor = endColor;
	m_color = startColor;
	m_needsColorLerp = true;
}

DebugWorldWireBox::DebugWorldWireBox( eDebugRenderMode mode , eDebugRenderSpace space , AABB3 box , Rgba8 color , float duration , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_WIRE_BOX , mode , space , duration , isBillboard )
{
	m_box = box;
	m_color = color;
}

void DebugRenderObject::Update()
{

	if ( m_timer->m_durationSeconds < 0.f )
	{
		return;
	}


	float elapsedTime = ( float ) m_timer->GetElapsedSeconds();
	unsigned char r = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) m_startColor.r , ( float ) m_endColor.r ,
		( elapsedTime ) );
	unsigned char g = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) m_startColor.g , ( float ) m_endColor.g ,
		( elapsedTime ) );
	unsigned char b = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) m_startColor.b , ( float ) m_endColor.b ,
		( elapsedTime ) );
	unsigned char a = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) m_startColor.a , ( float ) m_endColor.a ,
		( elapsedTime ) );

	

	if ( m_needsColorLerp )
	{
		if ( m_objectType == OBJECT_LINE )
		{
			DebugLine* temp = ( DebugLine* ) this;

			unsigned char lineStartR = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p0StartColor.r , ( float ) temp->m_p0EndColor.r ,
				( elapsedTime ) );
			unsigned char lineStartG = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p0StartColor.g , ( float ) temp->m_p0EndColor.g ,
				( elapsedTime ) );
			unsigned char lineStartB = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p0StartColor.b , ( float ) temp->m_p0EndColor.b ,
				( elapsedTime ) );
			unsigned char lineStartA = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p0StartColor.a , ( float ) temp->m_p0EndColor.a ,
				( elapsedTime ) );

			unsigned char lineEndR = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p1StartColor.r , ( float ) temp->m_p1EndColor.r ,
				( elapsedTime ) );
			unsigned char lineEndG = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p1StartColor.g , ( float ) temp->m_p1EndColor.g ,
				( elapsedTime ) );
			unsigned char lineEndB = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p1StartColor.b , ( float ) temp->m_p1EndColor.b ,
				( elapsedTime ) );
			unsigned char lineEndA = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p1StartColor.a , ( float ) temp->m_p1EndColor.a ,
				( elapsedTime ) );

			m_startColor = Rgba8( lineStartR , lineStartG , lineStartB , lineStartA );
			m_endColor = Rgba8( lineEndR , lineEndG , lineEndB , lineEndA );

		}

		if ( m_objectType == OBJECT_ARROW )
		{
			DebugArrow* temp = ( DebugArrow* ) this;

			unsigned char lineStartR = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p0StartColor.r , ( float ) temp->m_p0EndColor.r ,
				( elapsedTime ) );
			unsigned char lineStartG = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p0StartColor.g , ( float ) temp->m_p0EndColor.g ,
				( elapsedTime ) );
			unsigned char lineStartB = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p0StartColor.b , ( float ) temp->m_p0EndColor.b ,
				( elapsedTime ) );
			unsigned char lineStartA = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p0StartColor.a , ( float ) temp->m_p0EndColor.a ,
				( elapsedTime ) );

			unsigned char lineEndR = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p1StartColor.r , ( float ) temp->m_p1EndColor.r ,
				( elapsedTime ) );
			unsigned char lineEndG = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p1StartColor.g , ( float ) temp->m_p1EndColor.g ,
				( elapsedTime ) );
			unsigned char lineEndB = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p1StartColor.b , ( float ) temp->m_p1EndColor.b ,
				( elapsedTime ) );
			unsigned char lineEndA = ( unsigned char ) RangeMapFloat( 0.f , ( float ) m_timer->m_durationSeconds , ( float ) temp->m_p1StartColor.a , ( float ) temp->m_p1EndColor.a ,
				( elapsedTime ) );

			m_startColor = Rgba8( lineStartR , lineStartG , lineStartB , lineStartA );
			m_endColor = Rgba8( lineEndR , lineEndG , lineEndB , lineEndA );
		}

		m_color = Rgba8( r , g , b , a );
	}

	if ( m_timer->HasElapsed() )
	{
		MarkForDestroy();
	}
}

void DebugRenderObject::MarkForDestroy()
{
	m_isGarbage = true;
}

void DebugRenderSystem::SystemStartUp()
{
	m_context = g_theRenderer;
	m_camera = new Camera();

	point = new GPUMesh( m_context );

	g_theEventSystem.SubscribeToEvent( "debug_render_enabled" , DebugRenderEnabled );
	g_theEventSystem.SubscribeToEvent( "debug_add_world_point" , DebugAddWorldPoint );
	g_theEventSystem.SubscribeToEvent( "debug_add_screen_point" , DebugAddScreenPoint );
	g_theEventSystem.SubscribeToEvent( "debug_add_screen_quad" , DebugAddScreenQuad );
	g_theEventSystem.SubscribeToEvent( "debug_add_world_wiresphere" , DebugAddWorldWireSphere );
	g_theEventSystem.SubscribeToEvent( "debug_add_world_wirequad" , DebugAddWorldWireBounds );
	g_theEventSystem.SubscribeToEvent( "debug_add_world_billboardtext" , DebugAddWorldBillboardText );
	g_theEventSystem.SubscribeToEvent( "debug_add_screen_text" , DebugAddScreenText );

}

void DebugRenderSystem::SystemShutDown()
{
	delete m_camera;
	m_camera = nullptr;

	delete point;
	point = nullptr;

}

void DebugRenderSystem::TakeWorldCamera( Camera* cam )
{
	m_camera = cam;
}

void DebugRenderSystem::TakeScreenTexture( Texture* tex )
{
	m_output = tex;
}

void DebugRenderSystem::BeginFrame()
{

}

void DebugRenderSystem::Update()
{
	for ( int i = 0; i < m_worldObjects.size(); i++ )
	{
		if ( m_worldObjects[ i ] == nullptr )
		{
			continue;
		}

		m_worldObjects[ i ]->Update();
	}

	for ( int i = 0; i < m_screenObjects.size(); i++ )
	{
		if ( m_screenObjects[ i ] == nullptr )
		{
			continue;
		}

		m_screenObjects[ i ]->Update();
	}

	CleanUp();
}

void DebugRenderSystem::CleanUp()
{
	for ( int i = 0; i < m_worldObjects.size(); i++ )
	{
		if ( m_worldObjects[ i ] == nullptr )
		{
			continue;
		}

		if ( m_worldObjects[ i ]->m_isGarbage )
		{
			delete m_worldObjects[ i ];
			m_worldObjects[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_screenObjects.size(); i++ )
	{
		if ( m_screenObjects[ i ] == nullptr )
		{
			continue;
		}

		if ( m_screenObjects[ i ]->m_isGarbage )
		{
			delete m_screenObjects[ i ];
			m_screenObjects[ i ] = nullptr;
		}
	}
}

void DebugRenderSystem::EnableDebugRendering()
{
	enableRendring = true;
}

void DebugRenderSystem::DisableDebugRendering()
{
	enableRendring = false;
}

void DebugRenderSystem::ClearDebugRendering()
{
	m_screenObjects.clear();
	m_worldObjects.clear();
}

void DebugRenderSystem::DebugRenderWorldToCamera( Camera* cam )
{

	if ( !enableRendring )
	{
		return;
	}

	m_camera->SetProjection( cam->GetProjection() );
	m_camera->m_transform = cam->m_transform;
	m_camera->m_backBuffer = cam->m_backBuffer;

	for ( int index = 0; index < m_worldObjects.size(); index++ )
	{
		if ( m_worldObjects[ index ] == nullptr )
		{
			continue;
		}

		switch ( m_worldObjects[index]->m_objectType )
		{
			case OBJECT_POINT:
			{
				DebugPoint* pt = ( DebugPoint* ) m_worldObjects[ index ];

				switch ( m_worldObjects[index]->m_renderMode )
				{
				case DEBUG_RENDER_ALWAYS:
				{
					m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

					m_context->BeginCamera( *m_camera );
					m_context->BindShader( nullptr );
					m_context->SetBlendMode( BlendMode::OPAQE );
					
					std::vector<Vertex_PCU> sphereVerts;
					std::vector<unsigned int> sphereIndices;
					Vec3 centre = Vec3( 0.f , 0.f , 0.f );
					AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , pt->m_size , 64 , 32 , pt->m_color );
					point->UpdateVertices( ( unsigned int ) sphereVerts.size() , &sphereVerts[ 0 ] );
					point->UpdateIndices( ( unsigned int ) sphereIndices.size() , &sphereIndices[ 0 ] );
					
					m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
					m_context->DrawMesh( point );
					m_context->EndCamera( *m_camera );
					
					
					break;
				}

				case DEBUG_RENDER_USE_DEPTH:
				{
					m_camera->SetClearMode( 0, Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

					m_context->BeginCamera( *m_camera );
					m_camera->CreateDepthStencilTarget( g_theRenderer );
					m_context->SetDepthTest();
					m_context->BindDepthStencil( m_camera->m_backBuffer );

					std::vector<Vertex_PCU> sphereVerts;
					std::vector<unsigned int> sphereIndices;
					Vec3 centre = Vec3( 0.f , 0.f , 0.f );
					AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , 1.f , 64 , 32 , pt->m_color );
					point->UpdateVertices( ( unsigned int ) sphereVerts.size() , &sphereVerts[ 0 ] );
					point->UpdateIndices( ( unsigned int ) sphereIndices.size() , &sphereIndices[ 0 ] );

					m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
					m_context->BindShader( nullptr );
					m_context->DrawMesh( point );

					m_context->EndCamera( *m_camera );
					break;
				}

				case DEBUG_RENDER_XRAY:
				{
					m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

					m_context->BeginCamera( *m_camera );
					m_context->SetBlendMode( BlendMode::OPAQE );
					pt->m_color.a = 100;
					m_camera->CreateDepthStencilTarget( g_theRenderer );
					m_context->SetDepthTest();
					m_context->BindDepthStencil( m_camera->m_backBuffer );

					std::vector<Vertex_PCU> sphereVerts;
					std::vector<unsigned int> sphereIndices;
					Vec3 centre = Vec3( 0.f , 0.f , 0.f );
					AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , 1.f , 64, 32 , pt->m_color );
					point->UpdateVertices( ( unsigned int ) sphereVerts.size() , &sphereVerts[ 0 ] );
					point->UpdateIndices( ( unsigned int ) sphereIndices.size() , &sphereIndices[ 0 ] );
					m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
					m_context->DrawMesh( point );

					m_context->EndCamera( *m_camera );

					m_context->BeginCamera( *m_camera );
					m_context->SetBlendMode( BlendMode::ALPHA );
					pt->m_color.a = 200;
					m_context->SetDepthTest( COMPARE_GREATER );
					m_context->BindDepthStencil( m_camera->m_backBuffer );

					m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
					m_context->DrawMesh( point );
					m_context->SetDepthTest( COMPARE_LEQUAL );
					m_context->EndCamera( *m_camera );

					break;
				}

				default:
					break;
				}

				break;
			}

			case OBJECT_LINE:
			{
				DebugLine* pt = ( DebugLine* ) m_worldObjects[ index ];
				switch ( m_worldObjects[ index ]->m_renderMode )
				{
					case DEBUG_RENDER_ALWAYS:
					{
						m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_context->SetBlendMode( BlendMode::OPAQE );
						m_context->BindShader( nullptr );
						std::vector<Vertex_PCU> lineVerts;
						AppendCyinder( lineVerts , pt->m_startPos , pt->m_endPos , pt->m_lineThickness , pt->m_lineThickness , pt->m_startColor , pt->m_endColor );

						m_context->DrawVertexArray( lineVerts );
						m_context->SetBlendMode( BlendMode::ALPHA );
						m_context->EndCamera( *m_camera );

						break;
					}

					case DEBUG_RENDER_USE_DEPTH:
					{
						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->BindShader( nullptr );
						m_context->SetBlendMode( BlendMode::OPAQE );
						std::vector<Vertex_PCU> lineVerts;
						AppendCyinder( lineVerts , pt->m_startPos , pt->m_endPos , pt->m_lineThickness , pt->m_lineThickness , pt->m_startColor , pt->m_endColor );

						m_context->DrawVertexArray( lineVerts );
						m_context->SetBlendMode( BlendMode::ALPHA );
						m_context->EndCamera( *m_camera );

						break;
					}

					case DEBUG_RENDER_XRAY:
					{
						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->BindShader( nullptr );

						std::vector<Vertex_PCU> lineVerts;
						AppendCyinder( lineVerts , pt->m_startPos , pt->m_endPos , pt->m_lineThickness , pt->m_lineThickness , pt->m_startColor , pt->m_endColor );

						m_context->DrawVertexArray( lineVerts );
						m_context->EndCamera( *m_camera );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest( COMPARE_GREATER );
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->BindShader( nullptr );
						m_context->DrawVertexArray( lineVerts );
						m_context->EndCamera( *m_camera );

						break;
					}
				}

				break;
			}

			case OBJECT_ARROW:
			{
				DebugArrow* pt = ( DebugArrow* ) m_worldObjects[ index ];
				switch ( m_worldObjects[ index ]->m_renderMode )
				{
					case DEBUG_RENDER_ALWAYS:
					{
						m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_context->BindShader( nullptr );

						std::vector<Vertex_PCU> arrowVerts;
						AppendArrow( arrowVerts , pt->m_startPos , pt->m_endPos , pt->m_tipHeight , pt->m_lineThickness , pt->m_lineThickness , pt->m_startColor , pt->m_endColor );

						m_context->DrawVertexArray( arrowVerts );
						m_context->EndCamera( *m_camera );

						break;
					}

					case DEBUG_RENDER_USE_DEPTH:
					{
						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->BindShader( nullptr );

						std::vector<Vertex_PCU> arrowVerts;
						AppendArrow( arrowVerts , pt->m_startPos , pt->m_endPos , pt->m_tipHeight , pt->m_lineThickness , pt->m_lineThickness , pt->m_startColor , pt->m_endColor );

						m_context->DrawVertexArray( arrowVerts );
						m_context->EndCamera( *m_camera );

						break;
					}

					case DEBUG_RENDER_XRAY:
					{
						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->BindShader( nullptr );

						std::vector<Vertex_PCU> arrowVerts;
						AppendArrow( arrowVerts , pt->m_startPos , pt->m_endPos , pt->m_tipHeight , pt->m_lineThickness , pt->m_lineThickness , pt->m_startColor , pt->m_endColor );

						m_context->DrawVertexArray( arrowVerts );
						m_context->EndCamera( *m_camera );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest( COMPARE_GREATER );
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->BindShader( nullptr );
						m_context->DrawVertexArray( arrowVerts );
						m_context->EndCamera( *m_camera );

						break;
					}

					default:
						break;
				}

				break;
			}

			case OBJECT_QUAD:
			{
				DebugQuad* pt = (DebugQuad*) m_worldObjects[ index ];

				switch ( m_worldObjects[ index ]->m_renderMode )
				{
					case DEBUG_RENDER_ALWAYS:
					{
						m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_context->BindShader( nullptr );

						std::vector<Vertex_PCU> quadVerts;
						quadVerts.push_back( Vertex_PCU( pt->m_p0 , pt->m_color , pt->m_uvs.mins ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p1 , pt->m_color , Vec2(pt->m_uvs.maxs.x,pt->m_uvs.mins.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p1 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.maxs.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );


						m_context->BindTexture( pt->m_texture );
						m_context->DrawVertexArray( quadVerts );
						m_context->BindTexture( nullptr );
						m_context->EndCamera( *m_camera );

						break;
					}

					case DEBUG_RENDER_USE_DEPTH:
					{
						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->BindShader( nullptr );

						std::vector<Vertex_PCU> quadVerts;
						quadVerts.push_back( Vertex_PCU( pt->m_p0 , pt->m_color , pt->m_uvs.mins ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p1 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p1 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.maxs.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );

						m_context->BindTexture( pt->m_texture );
						m_context->DrawVertexArray( quadVerts );
						m_context->BindTexture( nullptr );
						m_context->EndCamera( *m_camera );

						break;
					}

					case DEBUG_RENDER_XRAY:
					{
						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->BindShader( nullptr );

						std::vector<Vertex_PCU> quadVerts;
						quadVerts.push_back( Vertex_PCU( pt->m_p0 , pt->m_color , pt->m_uvs.mins ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p1 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p1 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.maxs.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );

						m_context->BindTexture( pt->m_texture );
						m_context->DrawVertexArray( quadVerts );
						m_context->BindTexture( nullptr );
						m_context->EndCamera( *m_camera );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest( COMPARE_GREATER );
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->BindShader( nullptr );
						m_context->BindTexture( pt->m_texture );
						m_context->DrawVertexArray( quadVerts );
						m_context->BindTexture( nullptr );
						m_context->EndCamera( *m_camera );

						break;
					}

				}

				break;
			}

			case OBJECT_WIRE_SPHERE:
			{
				DebugWireSphere* pt = ( DebugWireSphere* ) m_worldObjects[ index ];

				switch ( m_worldObjects[ index ]->m_renderMode )
				{
					case DEBUG_RENDER_ALWAYS:
					{
						D3D11_RASTERIZER_DESC currentDesc;
						m_context->m_rasterState->GetDesc( &currentDesc );

						m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_context->CreateRasterState( D3D11_FILL_WIREFRAME , D3D11_CULL_NONE );
						m_context->BindShader( nullptr );
						m_context->SetBlendMode( BlendMode::OPAQE );

						std::vector<Vertex_PCU> sphereVerts;
						std::vector<unsigned int> sphereIndices;
						Vec3 centre = Vec3( 0.f , 0.f , 0.f );
						AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , 1.f , 64 , 32 , pt->m_color );
						point->UpdateVertices( ( unsigned int ) sphereVerts.size() , &sphereVerts[ 0 ] );
						point->UpdateIndices( ( unsigned int ) sphereIndices.size() , &sphereIndices[ 0 ] );

						m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
						m_context->DrawMesh( point );
						m_context->EndCamera( *m_camera );

						m_context->CreateRasterState( currentDesc );
						break;
					}

					case DEBUG_RENDER_USE_DEPTH:
					{
						D3D11_RASTERIZER_DESC currentDesc;
						m_context->m_rasterState->GetDesc( &currentDesc );

						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_context->CreateRasterState( D3D11_FILL_WIREFRAME , D3D11_CULL_NONE );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						std::vector<Vertex_PCU> sphereVerts;
						std::vector<unsigned int> sphereIndices;
						Vec3 centre = Vec3( 0.f , 0.f , 0.f );
						AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , 1.f , 64 , 32 , pt->m_color );
						point->UpdateVertices( ( unsigned int ) sphereVerts.size() , &sphereVerts[ 0 ] );
						point->UpdateIndices( ( unsigned int ) sphereIndices.size() , &sphereIndices[ 0 ] );

						m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
						m_context->DrawMesh( point );

						m_context->EndCamera( *m_camera );
						m_context->CreateRasterState( currentDesc );

						break;
					}

					case DEBUG_RENDER_XRAY:
					{
 						D3D11_RASTERIZER_DESC currentDesc;
 						m_context->m_rasterState->GetDesc( &currentDesc );

						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_context->CreateRasterState( D3D11_FILL_WIREFRAME , D3D11_CULL_NONE );
						m_context->SetBlendMode( BlendMode::OPAQE );
						pt->m_color.a = 100;
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						std::vector<Vertex_PCU> sphereVerts;
						std::vector<unsigned int> sphereIndices;
						Vec3 centre = Vec3( 0.f , 0.f , 0.f );
						AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , 1.f , 64 , 32 , pt->m_color );
						point->UpdateVertices( ( unsigned int ) sphereVerts.size() , &sphereVerts[ 0 ] );
						point->UpdateIndices( ( unsigned int ) sphereIndices.size() , &sphereIndices[ 0 ] );
						m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
						m_context->DrawMesh( point );

						m_context->EndCamera( *m_camera );

						m_context->BeginCamera( *m_camera );
						m_context->SetBlendMode( BlendMode::ALPHA );
						pt->m_color.a = 200;
						m_context->SetDepthTest( COMPARE_GREATER );
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
						m_context->DrawMesh( point );
						m_context->SetDepthTest( COMPARE_LEQUAL );
						m_context->EndCamera( *m_camera );
						m_context->CreateRasterState( currentDesc );
						break;
					}
				}

				break;
			}
			case OBJECT_WIRE_BOX:
			{
				DebugWorldWireBox* pt = ( DebugWorldWireBox* ) m_worldObjects[ index ];

				switch ( m_worldObjects[ index ]->m_renderMode )
				{
				case DEBUG_RENDER_ALWAYS:
				{
					D3D11_RASTERIZER_DESC currentDesc;
					m_context->m_rasterState->GetDesc( &currentDesc );

					m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

					m_context->BeginCamera( *m_camera );
					m_context->CreateRasterState( D3D11_FILL_WIREFRAME , D3D11_CULL_NONE );
					m_context->BindShader( nullptr );
					m_context->SetBlendMode( BlendMode::OPAQE );

					std::vector<Vertex_PCU> boxVerts;
					std::vector<unsigned int> boxIndices;
					Vec3 centre = Vec3( 0.f , 0.f , 0.f );
					AppendCuboid( boxVerts , boxIndices , pt->m_box , pt->m_color );
					point->UpdateVertices( ( unsigned int ) boxVerts.size() , &boxVerts[ 0 ] );
					point->UpdateIndices( ( unsigned int ) boxIndices.size() , &boxIndices[ 0 ] );

					m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
					m_context->DrawMesh( point );
					m_context->EndCamera( *m_camera );

					m_context->CreateRasterState( currentDesc );
					
					break;
				}

				case DEBUG_RENDER_USE_DEPTH:
				{
					D3D11_RASTERIZER_DESC currentDesc;
					m_context->m_rasterState->GetDesc( &currentDesc );

					m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

					m_context->BeginCamera( *m_camera );
					m_context->CreateRasterState( D3D11_FILL_WIREFRAME , D3D11_CULL_NONE );
					m_camera->CreateDepthStencilTarget( g_theRenderer );
					m_context->SetDepthTest();
					m_context->BindDepthStencil( m_camera->m_backBuffer );

					std::vector<Vertex_PCU> boxVerts;
					std::vector<unsigned int> boxIndices;
					Vec3 centre = Vec3( 0.f , 0.f , 0.f );
					AppendCuboid( boxVerts , boxIndices , pt->m_box , pt->m_color );
					point->UpdateVertices( ( unsigned int ) boxVerts.size() , &boxVerts[ 0 ] );
					point->UpdateIndices( ( unsigned int ) boxIndices.size() , &boxIndices[ 0 ] );

					m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
					m_context->DrawMesh( point );

					m_context->EndCamera( *m_camera );
					m_context->CreateRasterState( currentDesc );
					break;
				}

				case DEBUG_RENDER_XRAY:
				{
					D3D11_RASTERIZER_DESC currentDesc;
					m_context->m_rasterState->GetDesc( &currentDesc );

					m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

					m_context->BeginCamera( *m_camera );
					m_context->CreateRasterState( D3D11_FILL_WIREFRAME , D3D11_CULL_NONE );
					m_context->SetBlendMode( BlendMode::OPAQE );
					pt->m_color.a = 100;
					m_camera->CreateDepthStencilTarget( g_theRenderer );
					m_context->SetDepthTest();
					m_context->BindDepthStencil( m_camera->m_backBuffer );

					std::vector<Vertex_PCU> boxVerts;
					std::vector<unsigned int> boxIndices;
					Vec3 centre = Vec3( 0.f , 0.f , 0.f );
					AppendCuboid( boxVerts , boxIndices , pt->m_box , pt->m_color );
					point->UpdateVertices( ( unsigned int ) boxVerts.size() , &boxVerts[ 0 ] );
					point->UpdateIndices( ( unsigned int ) boxIndices.size() , &boxIndices[ 0 ] );
					m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
					m_context->DrawMesh( point );

					m_context->EndCamera( *m_camera );

					m_context->BeginCamera( *m_camera );
					m_context->SetBlendMode( BlendMode::ALPHA );
					pt->m_color.a = 200;
					m_context->SetDepthTest( COMPARE_GREATER );
					m_context->BindDepthStencil( m_camera->m_backBuffer );

					m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
					m_context->DrawMesh( point );
					m_context->SetDepthTest( COMPARE_LEQUAL );
					m_context->EndCamera( *m_camera );
					m_context->CreateRasterState( currentDesc );

					break;
				 }
				}

				break;
			}
			case OBJECT_BASIS:
			{
				DebugBasis* pt = ( DebugBasis* ) m_worldObjects[ index ];

				switch ( m_worldObjects[ index ]->m_renderMode )
				{
					case DEBUG_RENDER_ALWAYS:
					{
						m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_context->BindShader( nullptr );
						m_context->SetBlendMode( BlendMode::OPAQE );

						Vec3 position = pt->m_basis.GetTranslation3D();

						std::vector<Vertex_PCU> arrowVerts1;
						std::vector<Vertex_PCU> arrowVerts2;
						std::vector<Vertex_PCU> arrowVerts3;
						AppendArrow( arrowVerts1 , position , position + pt->m_basis.GetIBasis3D() * 3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8( 100 , 0 , 0 , 255 ) );
						AppendArrow( arrowVerts2 , position , position + pt->m_basis.GetJBasis3D() * 3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8( 0 , 100 , 0 , 255 ) );
						AppendArrow( arrowVerts3 , position , position + pt->m_basis.GetKBasis3D() * 3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8( 0 , 0 , 100 , 255 ) );

						m_context->DrawVertexArray( arrowVerts1 );
						m_context->DrawVertexArray( arrowVerts2 );
						m_context->DrawVertexArray( arrowVerts3 );

						m_context->EndCamera( *m_camera );

						break;
					}

					case DEBUG_RENDER_XRAY:
					{
						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );

						m_context->BindShader( nullptr );

						Vec3 position = pt->m_basis.GetTranslation3D();
						pt->m_color.a = 100;
						std::vector<Vertex_PCU> arrowVerts1;
						std::vector<Vertex_PCU> arrowVerts2;
						std::vector<Vertex_PCU> arrowVerts3;
						AppendArrow( arrowVerts1 , position , position + pt->m_basis.GetIBasis3D() * 3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8( 100 , 0 , 0 , 255 ) );
						AppendArrow( arrowVerts2 , position , position + pt->m_basis.GetJBasis3D() * 3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8( 0 , 100 , 0 , 255 ) );
						AppendArrow( arrowVerts3 , position , position + pt->m_basis.GetKBasis3D() * 3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8( 0 , 0 , 100 , 255 ) );

						m_context->DrawVertexArray( arrowVerts1 );
						m_context->DrawVertexArray( arrowVerts2 );
						m_context->DrawVertexArray( arrowVerts3 );

						m_context->EndCamera( *m_camera );

						m_context->BeginCamera( *m_camera );
						m_context->SetBlendMode( BlendMode::ALPHA );
						pt->m_color.a = 200;
						m_context->SetDepthTest( COMPARE_GREATER );
						m_context->BindDepthStencil( m_camera->m_backBuffer );

						AppendArrow( arrowVerts1 , position , position + pt->m_basis.GetIBasis3D() * 3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8( 100 , 0 , 0 , 255 ) );
						AppendArrow( arrowVerts2 , position , position + pt->m_basis.GetJBasis3D() * 3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8( 0 , 100 , 0 , 255 ) );
						AppendArrow( arrowVerts3 , position , position + pt->m_basis.GetKBasis3D() * 3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8( 0 , 0 , 100 , 255 ) );

						m_context->SetDepthTest( COMPARE_LEQUAL );
						m_context->EndCamera( *m_camera );

						break;
					}

					case DEBUG_RENDER_USE_DEPTH:
					{
						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );
						m_context->BindShader( nullptr );
						

						Vec3 position = pt->m_basis.GetTranslation3D();

						std::vector<Vertex_PCU> arrowVerts1;
						std::vector<Vertex_PCU> arrowVerts2;
						std::vector<Vertex_PCU> arrowVerts3;
						AppendArrow( arrowVerts1 , position , position + pt->m_basis.GetIBasis3D()*3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8(100,0,0,255) );
						AppendArrow( arrowVerts2 , position , position + pt->m_basis.GetJBasis3D()*3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8(0,100,0,255) );
						AppendArrow( arrowVerts3 , position , position + pt->m_basis.GetKBasis3D()*3.f , 0.5f , 0.5f , 0.8f , pt->m_color , Rgba8(0,0,100,255) );

						m_context->DrawVertexArray( arrowVerts1 );
						m_context->DrawVertexArray( arrowVerts2 );
						m_context->DrawVertexArray( arrowVerts3 );

						m_context->EndCamera( *m_camera );

						break;
					}
				}

				break;
			}

			case OBJECT_TEXT_WORLD:
			{
				DebugWorldText* pt = ( DebugWorldText* ) m_worldObjects[ index ];

				switch ( m_worldObjects[ index ]->m_renderMode )
				{
					case DEBUG_RENDER_ALWAYS:
					{
						m_camera->SetClearMode( 0 | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_context->BindShader( nullptr );
						//m_context->SetBlendMode( BlendMode::OPAQE );
						std::vector<Vertex_PCU> textVerts;
						AABB2 textArea=AABB2( Vec2(0.f,0.f) , Vec2( pt->m_text.size()* pt->m_size , pt->m_size ) );
						g_theBitMapFont->AddVertsForTextInBox2D( textVerts , textArea , pt->m_size , pt->m_text , pt->m_color , 1.f , pt->m_pivot );
						Mat44 modal = pt->m_basis;
						Mat44 cameraModal = m_camera->m_transform.ToMatrix();
						Mat44 mo = Mat44::LookAt( modal.GetTranslation3D() , cameraModal.GetTranslation3D() , -cameraModal.GetJBasis3D() );
						if ( pt->m_isBillboard )
						{
							modal.SetBasisVectors3D( mo.GetIBasis3D() , -mo.GetJBasis3D() , mo.GetKBasis3D() );
						}

						m_context->SetModalMatrix( modal );
						m_context->BindTexture( g_theBitMapFont->GetTexture() );
						m_context->DrawVertexArray( textVerts );
						m_context->BindTexture( nullptr );
						m_context->EndCamera( *m_camera );
						break;
					}

					case DEBUG_RENDER_USE_DEPTH:
					{
						m_camera->SetClearMode( 0  , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

						m_context->BeginCamera( *m_camera );
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );
						m_context->BindShader( nullptr );
						
						std::vector<Vertex_PCU> textVerts;
						AABB2 textArea = AABB2( Vec2( 0.f , 0.f ) , Vec2( pt->m_text.size() * pt->m_size , pt->m_size ) );
						g_theBitMapFont->AddVertsForTextInBox2D( textVerts , textArea , pt->m_size , pt->m_text , pt->m_color , 1.f , pt->m_pivot );

						Mat44 modal = pt->m_basis;
						Mat44 cameraModal = m_camera->m_transform.ToMatrix();
						Mat44 mo = Mat44::LookAt( modal.GetTranslation3D() , cameraModal.GetTranslation3D(), -cameraModal.GetJBasis3D() );
						if ( pt->m_isBillboard )
						{
							modal.SetBasisVectors3D( mo.GetIBasis3D() , -mo.GetJBasis3D() , mo.GetKBasis3D() );
						}

						m_context->SetModalMatrix( modal );
						m_context->BindTexture( g_theBitMapFont->GetTexture() );
						m_context->DrawVertexArray( textVerts );
						m_context->BindTexture( nullptr );
						m_context->EndCamera( *m_camera );
						break;
					}

					case DEBUG_RENDER_XRAY:
					{
						m_camera->SetClearMode( 0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );
						m_context->SetBlendMode( BlendMode::ALPHA );
						m_context->BeginCamera( *m_camera );
						pt->m_color.a = 100;
						m_camera->CreateDepthStencilTarget( g_theRenderer );
						m_context->SetDepthTest();
						m_context->BindDepthStencil( m_camera->m_backBuffer );
						m_context->BindShader( nullptr );

						std::vector<Vertex_PCU> textVerts;
						AABB2 textArea = AABB2( Vec2( 0.f , 0.f ) , Vec2( pt->m_text.size() * pt->m_size , pt->m_size ) );
						g_theBitMapFont->AddVertsForTextInBox2D( textVerts , textArea , pt->m_size , pt->m_text , pt->m_color , 1.f , pt->m_pivot );

						Mat44 modal = pt->m_basis;
						Mat44 cameraModal = m_camera->m_transform.ToMatrix();
						Mat44 mo = Mat44::LookAt( modal.GetTranslation3D() , cameraModal.GetTranslation3D() , -cameraModal.GetJBasis3D() );
						if ( pt->m_isBillboard )
						{
							modal.SetBasisVectors3D( mo.GetIBasis3D() , -mo.GetJBasis3D() , mo.GetKBasis3D() );
						}

						m_context->SetModalMatrix( modal );
						m_context->BindTexture( g_theBitMapFont->GetTexture() );
						m_context->DrawVertexArray( textVerts );
						m_context->BindTexture( nullptr );
						m_context->EndCamera( *m_camera );


						m_context->BeginCamera( *m_camera );
						pt->m_color.a = 255;
						m_context->SetDepthTest( COMPARE_GREATER );
						m_context->BindDepthStencil( m_camera->m_backBuffer );
						m_context->BindShader( nullptr );

						m_context->SetModalMatrix( modal );
						m_context->BindTexture( g_theBitMapFont->GetTexture() );
						m_context->DrawVertexArray( textVerts );
						m_context->BindTexture( nullptr );
						m_context->SetDepthTest( COMPARE_LESS );
						m_context->EndCamera( *m_camera );

						break;
					}


					break;
				}
				break;
			}

			default:
				break;
		}
	}

	m_camera->m_backBuffer = nullptr;
}


void DebugRenderSystem::DebugRenderToScreen( Texture* output )
{

	if ( !enableRendring )
	{
		return;
	}

	Camera camera;	
	if ( output == nullptr )
	{

		Vec2 min = Vec2( 0.f , 0.f );
		Vec2 max = Vec2( ( 16.f / 9.f ) * m_defaultScreenHeight , m_defaultScreenHeight );
		camera.SetOrthoView( min , max );
	}
	else
	{
		camera.m_texture = output;
		Vec2 min = Vec2( 0.f , 0.f );
		Vec2 max = Vec2( output->GetTexelSizeCoords() );
		camera.SetOrthoView( min , max );
	}

	camera.SetClearMode(0 , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

	

	for ( int index = 0; index < m_screenObjects.size(); index++ )
	{
		if ( m_screenObjects[ index ] == nullptr )
		{
			continue;
		}

		switch ( m_screenObjects[index]->m_objectType )
		{
		case OBJECT_POINT:
		{
			DebugPoint* pt = ( DebugPoint* ) m_screenObjects[ index ];

			m_context->BeginCamera( camera );
			m_context->BindShader( nullptr );
			m_context->SetModalMatrix( pt->m_transform.ToMatrix() );

			/*std::vector<Vertex_PCU> sphereVerts;
			std::vector<unsigned int> sphereIndices;
			Vec3 centre = Vec3( 0.f , 0.f , 0.f );
			AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , 1.f , 64 , 32 , pt->m_color );
			point->UpdateVertices( ( unsigned int ) sphereVerts.size() , &sphereVerts[ 0 ] );
			point->UpdateIndices( ( unsigned int ) sphereIndices.size() , &sphereIndices[ 0 ] );

			m_context->DrawMesh( point );*/
			m_context->DrawDisc( Vec2( 0.f , 0.f ) , 1.f , pt->m_color );
			break;
		}

		case  OBJECT_LINE:
		{
			DebugLine* pt = ( DebugLine* ) m_screenObjects[ index ];

			m_context->BeginCamera( camera );
			m_context->BindShader( nullptr );
			std::vector<Vertex_PCU> lineVerts;
			AppendCyinder( lineVerts , pt->m_startPos , pt->m_endPos , pt->m_lineThickness , pt->m_lineThickness , pt->m_startColor , pt->m_endColor );
			m_context->DrawVertexArray( lineVerts );
			break;

		}

		case OBJECT_ARROW:
		{
			DebugArrow* pt = ( DebugArrow* ) m_screenObjects[ index ];

			m_context->BeginCamera( camera );
			m_context->BindShader( nullptr );
			std::vector<Vertex_PCU> arrowVerts;
			AppendArrow( arrowVerts , pt->m_startPos , pt->m_endPos , pt->m_tipHeight , pt->m_lineThickness , pt->m_lineThickness , pt->m_startColor , pt->m_endColor );
			m_context->DrawVertexArray( arrowVerts );
			break;
		}

		case  OBJECT_QUAD:
		{
			DebugQuad* pt = ( DebugQuad* ) m_screenObjects[ index ];

			m_context->BeginCamera( camera );
			m_context->BindShader( nullptr );

			std::vector<Vertex_PCU> quadVerts;
			quadVerts.push_back( Vertex_PCU( pt->m_p0 , pt->m_color , pt->m_uvs.mins ) );
			quadVerts.push_back( Vertex_PCU( pt->m_p1 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
			quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );
			quadVerts.push_back( Vertex_PCU( pt->m_p1 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
			quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.maxs.y ) ) );
			quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );

			m_context->BindTexture( pt->m_texture );
			m_context->DrawVertexArray( quadVerts );
			m_context->BindTexture( nullptr );
			break;
		}

		case OBJECT_TEXT_SCREEN:
		{
			DebugScreenText* pt = ( DebugScreenText* ) m_screenObjects[ index ];

			m_context->BeginCamera( camera );
			m_context->BindShader( nullptr );
			
			AABB2 screenArea = AABB2( camera.GetOrthoBottomLeft() , camera.GetOrthoTopRight() );
			AABB2 textArea = AABB2( screenArea.mins , Vec2( pt->m_text.size() * pt->m_size * 1.f , pt->m_size ) );

			float ratioX = RangeMapFloat( 0.f , 1.f , screenArea.mins.x , screenArea.maxs.x , pt->m_offset.z );
			float ratioY = RangeMapFloat( 0.f , 1.f , screenArea.mins.y , screenArea.maxs.y , pt->m_offset.w );
			textArea.Translate( Vec2( ratioX , ratioY ) );
			textArea.Translate( Vec2( pt->m_offset.x , pt->m_offset.y ) );

			float pivotX = ( textArea.maxs.x - textArea.mins.x ) * pt->m_pivot.x;
			float pivotY = ( textArea.maxs.y - textArea.mins.y ) * pt->m_pivot.y;
			textArea.Translate( Vec2( -pivotX , -pivotY ) );
			std::vector<Vertex_PCU>textVerts;
			g_theBitMapFont->AddVertsForTextInBox2D( textVerts , textArea , pt->m_size , pt->m_text , pt->m_color , 1.f , pt->m_pivot );
			m_context->BindTexture( g_theBitMapFont->GetTexture() );
			m_context->DrawVertexArray( textVerts );
			m_context->BindTexture( nullptr );
			m_context->EndCamera( camera );

			break;

		}

		default:
			break;
		}

	}

	m_context->EndCamera( camera );
}

void DebugRenderSystem::DebugRenderSetScreenHeight( float height )
{
	m_defaultScreenHeight = height;
}

AABB2 DebugRenderSystem::DebugGetScreenBounds()
{
	AABB2 screenBounds = AABB2( Vec2( 0.f , 0.f ) , Vec2( CLIENT_ASPECT * m_defaultScreenHeight , m_defaultScreenHeight) );
	return screenBounds;
}

void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 color , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ )
{
	DebugPoint* obj = new DebugPoint( mode , DEBUG_RENDER_WORLD , pos , size , color , duration );

	for ( int i = 0; i < DebugRenderSystem::sDebugRenderer->m_worldObjects.size(); i++ )
	{
		if ( DebugRenderSystem::sDebugRenderer->m_worldObjects[ i ] == nullptr )
		{
			DebugRenderSystem::sDebugRenderer->m_worldObjects[ i ] = obj;
			return;
		}
	}
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
	
}

void DebugAddWorldPoint( Transform transform , float size , Rgba8 color , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ )
{
	DebugPoint* obj = new DebugPoint( mode , DEBUG_RENDER_WORLD , transform , size , color , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ )
{
	DebugPoint* obj = new DebugPoint( mode , DEBUG_RENDER_WORLD , pos , size , startColor , endColor , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldLine( Vec3 start , Vec3 end , Rgba8 color , float lineThickness , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugLine* obj = new DebugLine( mode , DEBUG_RENDER_WORLD , start , end , color , lineThickness , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}


void DebugAddWorldLine( Vec3 p0 , Vec3 p1 , Rgba8 p0_start_color , Rgba8 p0_end_color , Rgba8 p1_start_color , Rgba8 p1_end_color ,float lineThickness, float duration , eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ )
{
	DebugLine* obj = new DebugLine( mode , DEBUG_RENDER_WORLD , p0 , p1 , p0_start_color , p0_end_color , p1_start_color , p1_end_color , lineThickness , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldArrow( Vec3 start , Vec3 end , Rgba8 color , float duration , float thickness , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugArrow* obj = new DebugArrow( mode , DEBUG_RENDER_WORLD , start , end ,color, thickness , thickness , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}


void DebugAddWorldArrow( Vec3 p0 , Vec3 p1 , Rgba8 p0_start_color , Rgba8 p0_end_color , Rgba8 p1_start_color , Rgba8 p1_end_color , float duration , float thickness, eDebugRenderMode mode/*=DEBUG_RENDER_USE_DEPTH */ )
{
	DebugArrow* obj = new DebugArrow( mode , DEBUG_RENDER_WORLD , p0 , p1 , p0_start_color,p0_end_color,p1_start_color,p1_end_color , thickness , thickness , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldQuad( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugQuad* obj = new DebugQuad( mode , DEBUG_RENDER_WORLD , p0 , p1 , p2 , p3 , color , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldQuad( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color , Texture* tex , float duration ,AABB2 uvs, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugQuad* obj = new DebugQuad( mode , DEBUG_RENDER_WORLD , p0 , p1 , p2 , p3 , color, tex, uvs , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldQuad( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugQuad* obj = new DebugQuad( mode , DEBUG_RENDER_WORLD , p0 , p1 , p2 , p3 , start_color, end_color , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldWireBounds( AABB3 bounds , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugWorldWireBox* obj = new DebugWorldWireBox( mode , DEBUG_RENDER_WORLD , bounds , color , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugWireSphere* obj = new DebugWireSphere( mode , DEBUG_RENDER_WORLD , pos , radius , color , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugWireSphere* obj = new DebugWireSphere( mode , DEBUG_RENDER_WORLD , pos , radius , start_color,end_color , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldBasis( Mat44 basis , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugBasis* obj = new DebugBasis( mode , DEBUG_RENDER_WORLD , basis , color , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldBasis( Mat44 basis , Rgba8 start_tint , Rgba8 end_tint , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugBasis* obj = new DebugBasis( mode , DEBUG_RENDER_WORLD , basis , start_tint,end_tint , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldText( Mat44 basis , Vec2 pivot , float size , Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode , char const* text )
{
	DebugWorldText* obj = new DebugWorldText( mode , DEBUG_RENDER_WORLD , size , start_color , end_color , basis ,duration, pivot , text );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldTextf( Mat44 basis , Vec2 pivot , float size , Rgba8 color , float duration , eDebugRenderMode mode , char const* text , ... )
{
	va_list args;
	va_start( args , text );
	std::string tex = Stringv( text , args );

	DebugWorldText* obj = new DebugWorldText( mode , DEBUG_RENDER_WORLD , size ,color, basis , duration , pivot , tex );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldTextf( Mat44 basis , Vec2 pivot , float size , Rgba8 color , char const* text , ... )
{
	va_list args;
	va_start( args , text );
	std::string tex = Stringv( text , args );

	DebugWorldText* obj = new DebugWorldText( DEBUG_RENDER_USE_DEPTH , DEBUG_RENDER_WORLD , size , color , basis , -1.f , pivot , tex );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}


void DebugAddWorldBillboardText( Vec3 origin , Vec2 pivot ,float size, Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode , char const* text )
{
	Mat44 basis = Mat44();
	basis.SetTranslation3D( origin );

	DebugWorldText* obj = new DebugWorldText( mode , DEBUG_RENDER_WORLD , size , start_color , end_color , basis , duration , pivot , text, true );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldBillboardTextf( Vec3 origin , Vec2 pivot , float size , Rgba8 color , float duration , eDebugRenderMode mode , char const* format , ... )
{

	va_list args;
	va_start( args , format );
	std::string text = Stringv( format , args );

	Mat44 basis = Mat44();
	basis.SetTranslation3D( origin );

	DebugWorldText* obj = new DebugWorldText( mode , DEBUG_RENDER_WORLD , size , color , basis , duration , pivot , text , true );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldBillboardTextf( Vec3 origin , Vec2 pivot , float size , Rgba8 color , char const* format , ... )
{
	va_list args;
	va_start( args , format );
	std::string text = Stringv( format , args );

	Mat44 basis = Mat44();
	basis.SetTranslation3D( origin );

	DebugWorldText* obj = new DebugWorldText( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_WORLD , size , color , basis , -1.f , pivot , text , true );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddScreenPoint( Vec2 pos , Rgba8 color )
{
	DebugPoint* obj = new DebugPoint( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN , Vec3( pos , 0.f ) ,10.f, color , -1.f );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 color , float duration /*= 0.0f */ )
{
	DebugPoint* obj = new DebugPoint( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN , Vec3( pos , 0.f ) , size , color , duration );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 start_color , Rgba8 end_color , float duration )
{
	DebugPoint* obj = new DebugPoint( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN , Vec3( pos , 0.f ) , size , start_color,end_color , duration );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenLine( Vec2 p0 , Vec2 p1 , Rgba8 color ,float lineThickness, float duration /*= 0.0f */ )
{
	DebugLine* obj = new DebugLine( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN , Vec3(p0,0.f) , Vec3(p1,0.f) , color , lineThickness , duration );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenLine( Vec2 p0 , Rgba8 p0_start_color , Rgba8 p0_end_color , Vec2 p1 , Rgba8 p1_start_color , Rgba8 p1_end_color , float lineThickness , float duration )
{
	DebugLine* obj = new DebugLine( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN , Vec3(p0,0.f) , Vec3(p1,0.f) , p0_start_color , p0_end_color , p1_start_color , p1_end_color , lineThickness , duration );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenArrow( Vec2 p0 , Vec2 p1 , Rgba8 color , float lineThickness ,float tipHeight, float duration /*= 0.0f */ )
{
	DebugArrow* obj = new DebugArrow( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN , Vec3(p0,0.f) , Vec3(p1,0.f) , color , lineThickness , tipHeight , duration );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenArrow( Vec2 p0 , Rgba8 p0_start_color , Rgba8 p0_end_color , Vec2 p1 , Rgba8 p1_start_color , Rgba8 p1_end_color , float lineThickness , float tipHeight , float duration )
{
	DebugArrow* obj = new DebugArrow( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN , Vec3(p0,0.f) , Vec3(p1,0.f) , p0_start_color , p0_end_color , p1_start_color , p1_end_color , lineThickness , tipHeight , duration );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenQuad( AABB2 bounds , Rgba8 color , float duration /*= 0.0f */ )
{
	DebugQuad* obj = new DebugQuad( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN , Vec3(bounds.mins,0.f) , Vec3(bounds.maxs.x,bounds.mins.y,0.f) , Vec3(bounds.maxs,0.f) , Vec3( bounds.mins.x , bounds.maxs.y,0.f ) , color , duration );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenQuad( AABB2 bounds , Rgba8 start_color , Rgba8 end_color , float duration )
{
	DebugQuad* obj = new DebugQuad( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN , Vec3( bounds.mins , 0.f ) , Vec3( bounds.maxs.x , bounds.mins.y , 0.f ) , Vec3( bounds.maxs , 0.f ) , Vec3( bounds.mins.x , bounds.maxs.y , 0.f ) , start_color , end_color , duration );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenTexturedQuad( AABB2 bounds , Texture* tex , AABB2 uvs , Rgba8 tint , float duration /*= 0.0f */ )
{
	DebugQuad* obj = new DebugQuad( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_WORLD , Vec3( bounds.mins , 0.f ) , Vec3( bounds.maxs.x , bounds.mins.y , 0.f ) , Vec3( bounds.maxs , 0.f ) , Vec3( bounds.mins.x , bounds.maxs.y , 0.f ) , tint , tex , uvs , duration );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenText( Vec4 ratioOffset , Vec2 pivot , float size , Rgba8 start_color , Rgba8 end_color , float duration , char const* text )
{
	DebugScreenText* obj = new DebugScreenText( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN ,size, start_color , end_color , duration , pivot , ratioOffset , text );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenText( Vec4 ratioOffset , Vec2 pivot , float size , Rgba8 color , float duration , char const* text )
{
	DebugScreenText* obj = new DebugScreenText( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN ,size, color , duration , pivot , ratioOffset , text );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenTextf( Vec4 ratioOffset , Vec2 pivot , float size , Rgba8 start_color , Rgba8 end_color , float duration , char const* format , ... )
{
	std::string text = Stringfv2( format );

	DebugScreenText* obj = new DebugScreenText( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN ,size, start_color , end_color , duration , pivot , ratioOffset , text );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}

void DebugAddScreenTextf( Vec4 ratioOffset , Vec2 pivot , float size , Rgba8 color , float duration , char const* format,... )
{
	
	va_list args;
	va_start( args , format );
	std::string text = Stringv( format,args );

	DebugScreenText* obj = new DebugScreenText( DEBUG_RENDER_ALWAYS , DEBUG_RENDER_SCREEN ,size, color , duration , pivot , ratioOffset , text );
	DebugRenderSystem::sDebugRenderer->m_screenObjects.push_back( obj );
}




