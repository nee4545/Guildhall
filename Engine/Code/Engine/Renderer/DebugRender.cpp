#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

extern RenderContext* g_theRenderer;
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
	m_transform.m_scale *= size;

}

DebugPoint::DebugPoint( eDebugRenderMode mode , eDebugRenderSpace space , Transform transform , float size , Rgba8 color , float duration , bool isBillboard /*= false */ ) :
	DebugRenderObject( OBJECT_POINT , mode , space , duration , isBillboard )
{
	m_transform = transform;
	m_size = size;
	m_color = color;

	m_transform.m_scale *= size;
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

void DebugRenderSystem::DebugRenderWorldToCamera( Camera* cam )
{
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
					AddUVSphereToIndexedVertexArray( sphereVerts , sphereIndices , centre , 1.f , 64 , 32 , pt->m_color );
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

			default:
				break;
		}
	}

	m_camera->m_backBuffer = nullptr;
}


void DebugRenderSystem::DebugRenderToScreen( Texture* output )
{
	Camera camera;	
	if ( output != nullptr )
	{
		camera.m_texture = output;
		Vec2 min = Vec2( 0.f , 0.f );
		Vec2 max = Vec2( output->GetTexelSizeCoords() );
		camera.SetOrthoView(min,max);
	}
	else
	{
		Vec2 min = Vec2( 0.f , 0.f );
		Vec2 max = Vec2( ( 16.f / 9.f ) * m_defaultScreenHeight ,m_defaultScreenHeight );
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

void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 color , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ )
{
	DebugPoint* obj = new DebugPoint( mode , DEBUG_RENDER_WORLD , pos , size , color , duration );
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

