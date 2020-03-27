#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"

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

DebugLine::DebugLine( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end ,Rgba8 color, float lineThickness , float duration , bool isBillboard /*= false */ ) :
	DebugRenderObject( OBJECT_LINE , mode , space , duration , isBillboard )
{
	m_startPos = start;
	m_endPos = end;
	m_color = color;
	m_lineThickness = lineThickness;
}

DebugArrow::DebugArrow( eDebugRenderMode mode , eDebugRenderSpace space , Vec3 start , Vec3 end ,Rgba8 color, float lineThickness , float tipHeight , float duration/*=-1.f */ , bool isBillboard /*= false */ ):
	DebugRenderObject( OBJECT_ARROW , mode , space , duration , isBillboard )
{
	m_startPos = start;
	m_endPos = end;
	m_color = color;
	m_lineThickness = lineThickness;
	m_tipHeight = tipHeight;
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

void DebugRenderObject::Update()
{
	//m_timer->CheckAndDecrement();

	if ( m_timer->m_durationSeconds < 0.f )
	{
		return;
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
						m_context->BindShader( nullptr );
						
						std::vector<Vertex_PCU> lineVerts;
						AppendCyinder( lineVerts , pt->m_startPos , pt->m_endPos , pt->m_lineThickness , pt->m_lineThickness , pt->m_color , pt->m_color );

						m_context->DrawVertexArray( lineVerts );
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

						std::vector<Vertex_PCU> lineVerts;
						AppendCyinder( lineVerts , pt->m_startPos , pt->m_endPos , pt->m_lineThickness , pt->m_lineThickness , pt->m_color , pt->m_color );

						m_context->DrawVertexArray( lineVerts );
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
						AppendCyinder( lineVerts , pt->m_startPos , pt->m_endPos , pt->m_lineThickness , pt->m_lineThickness , pt->m_color , pt->m_color );

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
						AppendArrow( arrowVerts , pt->m_startPos , pt->m_endPos , pt->m_tipHeight , pt->m_lineThickness , pt->m_lineThickness , pt->m_color , pt->m_color );

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
						AppendArrow( arrowVerts , pt->m_startPos , pt->m_endPos , pt->m_tipHeight , pt->m_lineThickness , pt->m_lineThickness , pt->m_color , pt->m_color );

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
						AppendArrow( arrowVerts , pt->m_startPos , pt->m_endPos , pt->m_tipHeight , pt->m_lineThickness , pt->m_lineThickness , pt->m_color , pt->m_color );

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
						quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , pt->m_uvs.maxs ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p0 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );

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
						quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , pt->m_uvs.maxs ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p0 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );

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
						quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p2 , pt->m_color , Vec2( pt->m_uvs.maxs.x , pt->m_uvs.mins.y ) ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p3 , pt->m_color , pt->m_uvs.maxs ) );
						quadVerts.push_back( Vertex_PCU( pt->m_p0 , pt->m_color , Vec2( pt->m_uvs.mins.x , pt->m_uvs.maxs.y ) ) );

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

	camera.SetClearMode(0|CLEAR_DEPTH_BIT|CLEAR_STENCIL_BIT , Rgba8( 0 , 0 , 0 , 255 ) , 0.f , 0 );

	m_context->BeginCamera( camera );

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

			m_context->BindShader( nullptr );
			m_context->SetModalMatrix( pt->m_transform.ToMatrix() );
			m_context->DrawDisc( Vec2( 0.f , 0.f ) , 1.f , pt->m_color );
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

void DebugAddWorldLine( Vec3 start , Vec3 end , Rgba8 color , float lineThickness , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugLine* obj = new DebugLine( mode , DEBUG_RENDER_WORLD , start , end , color , lineThickness , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}

void DebugAddWorldArrow( Vec3 start , Vec3 end , Rgba8 color , float duration , float thickness , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugArrow* obj = new DebugArrow( mode , DEBUG_RENDER_WORLD , start , end ,color, thickness , thickness , duration );
	DebugRenderSystem::sDebugRenderer->m_worldObjects.push_back( obj );
}


void DebugAddWorldQuad( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , Rgba8 color , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugQuad* obj = new DebugQuad( mode , DEBUG_RENDER_WORLD , p0 , p1 , p2 , p3 , color , duration );
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




