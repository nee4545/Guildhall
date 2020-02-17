
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/ThirdParty/stb_image.h"
#include "Engine/Core/Texture.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Sampler.hpp"


#define UNUSED(x) (void)(x);

#include "Engine/Core/D3D11Common.hpp"

#define RENDER_DEBUG


BitmapFont* g_theBitMapFont = nullptr;


void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	//RenderBuffer* m_immediateVBO;

	//Update a vertex buffer
	size_t bufferByteSize = numVertexes * sizeof( Vertex_PCU );
	size_t elementSize = sizeof( Vertex_PCU );
	m_immediateVBO->Update( vertexes , bufferByteSize , elementSize );

	//Bind 

		BindVertexInput( m_immediateVBO );


		Draw( numVertexes , 0 );


}

void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU> &verts )
{

	DrawVertexArray( (int)verts.size() , &verts[0] );

}

void RenderContext::DrawAABB2D( const AABB2& aabb, const Rgba8& color )
{
	Vertex_PCU vert[6]
	{
		Vertex_PCU(Vec3(aabb.mins.x,aabb.mins.y,0.f),color,Vec2(0.f,0.f)),
		Vertex_PCU(Vec3(aabb.maxs.x,aabb.mins.y,0.f),color,Vec2(1.f,0.f)),
		Vertex_PCU(Vec3(aabb.mins.x,aabb.maxs.y,0.f),color,Vec2(0.f,1.f)),

		Vertex_PCU(Vec3(aabb.maxs.x,aabb.mins.y,0.f),color,Vec2(1.f,0.f)),
		Vertex_PCU(Vec3(aabb.maxs.x,aabb.maxs.y,0.f),color,Vec2(1.f,1.f)),
		Vertex_PCU(Vec3(aabb.mins.x,aabb.maxs.y,0.f),color,Vec2(0.f,1.f))
	
	};

	DrawVertexArray(6,vert);

}

void RenderContext::DrawXFlippedAABB2D( const AABB2& aabb , const Rgba8& color )
{
	Vertex_PCU vert[ 6 ]
	{
		Vertex_PCU( Vec3( aabb.mins.x,aabb.mins.y,0.f ),color,Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,0.f ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),color,Vec2( 1.f,1.f ) ),

		Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,0.f ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),color,Vec2( 0.f,1.f ) ),
		Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),color,Vec2( 1.f,1.f ) )

	};

	DrawVertexArray( 6 , vert );
}

void RenderContext::DrawLine( const Vec2& start, const Vec2& end, const Rgba8& color, float thickness )
{
	Vec2 fwd=end-start;
	fwd.SetLength( thickness*0.5f );
	Vec2 lef=fwd.GetRotated90Degrees();

	Vec2 endRight= end+fwd-lef;
	Vec2 startRight= start-fwd-lef;
	Vec2 endLeft= end+fwd+lef;
	Vec2 startLeft= start-fwd+lef;

	Vec3 er=Vec3( endRight.x, endRight.y, 0.f );
	Vec3 el=Vec3( endLeft.x, endLeft.y, 0.f );
	Vec3 sr=Vec3( startRight.x, startRight.y, 0.f );
	Vec3 sl=Vec3( startLeft.x, startLeft.y, 0.f );

	Vertex_PCU vert1[3] =
	{
		Vertex_PCU( sr,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( er,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( el,color,Vec2( 0.f,0.f ) )
	};

	Vertex_PCU vert2[3] =
	{
		Vertex_PCU( sr,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( el,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( sl,color,Vec2( 0.f,0.f ) )
	};

	DrawVertexArray( 3, vert1 );
	DrawVertexArray( 3, vert2 );
}

void RenderContext::DrawRing( const Vec2 centre, float radius, Rgba8 color, float thickness )
{
	constexpr float theta= 360.f/16.f;

	for( int index=0; index<16; index++ )
	{
		float degree1=theta*float( index );
		float degree2=theta*float( index+1 );

		DrawLine( centre+Vec2( radius*CosDegrees( degree1 ), radius*SinDegrees( degree1 ) ), centre+Vec2( radius*CosDegrees( degree2 ), radius*SinDegrees( degree2 ) ), color, thickness );
	}
}

void RenderContext::DrawDisc( const Vec2 centre , float radius , Rgba8 color )
{
	constexpr int  NUMBER_OF_DISC_VERTS = 120;
	Vertex_PCU discVerts[ NUMBER_OF_DISC_VERTS ];
	const Vec2 UVCoordsAtCenter = Vec2( 0.5f , 0.5f );
	const float UVRadius = 0.5f;
	float angleInDegreesBetweenDiscTriangles = 0.f;
	discVerts[ 0 ] = Vertex_PCU( ( Vec3( 0.f , 0.f , 0.f ) ) , color , Vec2( 0.5f , 0.5f ) );
	discVerts[ 1 ] = Vertex_PCU( ( Vec3( radius , 0.f , 0.f ) ) , color , Vec2( 1.f , 0.5f ) );
	angleInDegreesBetweenDiscTriangles = ( 360.f * 3.f ) / static_cast< float >( NUMBER_OF_DISC_VERTS );

	float costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
	float intialXVertex = radius * costheta;
	float sintheha = SinDegrees( angleInDegreesBetweenDiscTriangles );
	float initialYVertex = radius * sintheha;
	discVerts[ 2 ] = Vertex_PCU( ( Vec3( intialXVertex , initialYVertex , 0.f ) ) , color , Vec2( UVRadius + UVRadius * costheta , UVRadius + UVRadius * sintheha ) );

	int discVertIndex = 3;
	for ( discVertIndex = 3; discVertIndex < NUMBER_OF_DISC_VERTS; discVertIndex += 3 )
	{
		angleInDegreesBetweenDiscTriangles = angleInDegreesBetweenDiscTriangles + ( ( 360.f * 3.f ) / ( NUMBER_OF_DISC_VERTS ) );
		discVerts[ discVertIndex ] = discVerts[ discVertIndex - 3 ];
		discVerts[ discVertIndex + 1 ] = discVerts[ discVertIndex - 1 ];
		discVerts[ discVertIndex + 2 ].m_position = Vec3( radius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			radius * SinDegrees( angleInDegreesBetweenDiscTriangles ) , 0.f );
		discVerts[ discVertIndex + 2 ].m_color = color;
		discVerts[ discVertIndex + 2 ].m_uvTexCoords = Vec2( UVRadius + UVRadius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			UVRadius + UVRadius * SinDegrees( angleInDegreesBetweenDiscTriangles ) );
	}
	discVerts[ NUMBER_OF_DISC_VERTS - 1 ] = discVerts[ 1 ];
	TransformVertexArray( NUMBER_OF_DISC_VERTS , discVerts , 1 , 0.f , centre );
	DrawVertexArray( NUMBER_OF_DISC_VERTS , discVerts );
}

void RenderContext::BindShader( Shader* shader )
{
	ASSERT_OR_DIE( m_isDrawing == true , "Camera has not been set" );

	m_currentShader = shader;

	if ( m_currentShader == nullptr )
	{
		m_currentShader = m_defaultShader;
	}

	
}

void RenderContext::BindShader( std::string filename )
{
	Shader* temp = nullptr;
	if ( filename == "" )
	{
		BindShader( temp );
		return;
	}

	m_currentShader=GetOrCreateShader( filename.c_str() );
}

void RenderContext::BindVertexInput( VertexBuffer* vbo )
{
	ID3D11Buffer* vboHandle = vbo->m_handle;
	unsigned int stride = sizeof( Vertex_PCU );
	unsigned int offset = 0;
	
	if ( m_lastBoundVBO != vboHandle )
	{
		m_context->IASetVertexBuffers( 0 , 1 , &vboHandle , &stride , &offset );
		m_lastBoundVBO = vboHandle;
	}
}

Shader* RenderContext::GetOrCreateShader( char const* filename )
{
	Shader* temp = m_loadedShaders[ filename ];
	if ( temp == nullptr )
	{
		temp = new Shader( this );
		temp->CreateFromFile( filename );
		m_loadedShaders[ filename ] = temp;
	}

	return m_loadedShaders[ filename ];
}

void RenderContext::BindUniformBuffer( unsigned int slot , RenderBuffer* ubo )
{
	ID3D11Buffer* uboHandle = ubo->m_handle;

	m_context->VSSetConstantBuffers( slot , 1 , &uboHandle );
	m_context->PSSetConstantBuffers( slot , 1 , &uboHandle );
}

Texture* RenderContext::CreateTextureFromFile(  const char* imageFilePath )
{
	
	//const char* imageFilePath = "Data/Images/Test_StbiFlippedAndOpenGL.png";
	//unsigned int textureID = 0;
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );
	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );
	GUARANTEE_OR_DIE( numComponents == 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );
	

	//DirectX Creation
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = imageTexelSizeX;
	desc.Height = imageTexelSizeY;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1; //MSAA
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA  initialData;
	initialData.pSysMem = imageData;
	initialData.SysMemPitch = imageTexelSizeX * 4;
	initialData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc , &initialData , &texHandle );

	stbi_image_free( imageData );
	Texture* temp=new Texture(imageFilePath,this,texHandle);
	m_LoadedTextures[imageFilePath]=temp;
	return temp;

}


Texture* RenderContext::GetOrCreateTextureFromFile( const char* imageFilePath )
{
	Texture* temp=m_LoadedTextures[imageFilePath];
	if( temp==nullptr )
	{
		temp=CreateTextureFromFile(imageFilePath);
	}

	return temp;
}



Texture* RenderContext::CreateTextureFromColor( Rgba8 color )
{
	UNUSED( color );
	return nullptr;
}

void RenderContext::Startup( Window* window )
{

	/*#if defined(RENDER_DEBUG)
	  CreateDebugModule();
	#endif*/
	//Instance - singleton
	IDXGISwapChain* swapchain = nullptr; 

	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

	 #if defined(RENDER_DEBUG)
	 flags |= D3D11_CREATE_DEVICE_DEBUG;
	 #endif

	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	memset( &swapchainDesc , 0 , sizeof( swapchainDesc ) );
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.Flags = 0;
	
	HWND hwnd = ( HWND ) window->m_hwnd;
	swapchainDesc.OutputWindow = hwnd;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.Windowed = TRUE;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferDesc.Width = window->GetClientWidth();
	swapchainDesc.BufferDesc.Height = window->GetClientHeight();


	D3D11CreateDeviceAndSwapChain( nullptr , D3D_DRIVER_TYPE_HARDWARE, nullptr ,
		flags , nullptr , 0 , D3D11_SDK_VERSION , &swapchainDesc , &swapchain , &m_device , nullptr , &m_context );

	m_swapChain = new SwapChain( this , swapchain );

	//m_defaultShader = new Shader(this);
	//m_defaultShader->CreateFromFile( "Data/Shaders/Default.hlsl" );

	m_defaultShader = GetOrCreateShader( "Data/Shaders/Default.hlsl" );

	m_immediateVBO = new VertexBuffer( this , MEMORY_HINT_DYNAMIC );
	//swapchain->Release();
	m_frameUBO = new RenderBuffer( this , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );

	m_defaultSampler = new Sampler( this , SAMPLER_POINT );
	m_defaultColor = CreateTextureFromFile( "Data/Images/white.png" );

	CreateBlendStates();

	g_theBitMapFont = CreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );

}

void RenderContext::Shutdown()
{
	
	delete g_theBitMapFont;

	for ( auto& x : m_loadedShaders )
	{
		if ( x.second != nullptr )
		{
			delete x.second;
			x.second = nullptr;
		}
	}

	if ( m_defaultColor != nullptr )
	{
		delete m_defaultColor;
		m_defaultColor = nullptr;
	}

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	/*if ( m_defaultShader != nullptr )
	{
		delete m_defaultShader;
		m_defaultShader = nullptr;
	}*/

	if ( m_frameUBO != nullptr )
	{
		delete m_frameUBO;
		m_frameUBO = nullptr;
	}

	if ( m_defaultSampler != nullptr )
	{
		delete m_defaultSampler;
		m_defaultSampler = nullptr;
	}

	delete m_swapChain;
	m_swapChain = nullptr;

	DX_SAFE_RELEASE( m_alphaBlendState );
	DX_SAFE_RELEASE( m_additiveBlendState );


	DX_SAFE_RELEASE( m_context );
	DX_SAFE_RELEASE( m_device );

	

	/*ReportLiveObjects();
	DestroyDebugModule();*/

}

void RenderContext::BeginFrame()
{
	


}

void RenderContext::EndFrame()
{
	m_swapChain->Present();
}

void RenderContext::Draw( int numVertexes , int vertexOffset )
{
	m_context->VSSetShader( m_currentShader->m_vertexStage.m_vs , nullptr , 0 );
	m_context->RSSetState( m_currentShader->m_rasterState );
	m_context->PSSetShader( m_currentShader->m_fragmentStage.m_fs , nullptr , 0 );

	/*TextureView* view = m_texture->GetRenderTargetView();
	ID3D11RenderTargetView* rtv = view->GetRTVHandle();*/
	
	//This is temporary
	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	
	
	//Describe vertex format to shader

	ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout( /*Vertex_PCU::LAYOUT*/ );
	m_context->IASetInputLayout( inputLayout );

	m_context->Draw( numVertexes , vertexOffset );
}

void RenderContext::BeginCamera( Camera &camera)
{
	#if defined(RENDER_DEBUG)
		m_context->ClearState();
	#endif

	m_isDrawing = true;
	Texture* colorTarget = camera.GetColorTarget();
	{
		if ( colorTarget == nullptr )
		{
			colorTarget = m_swapChain->GetBackBuffer();
		}
	}

	TextureView* view = colorTarget->GetRenderTargetView();
	ID3D11RenderTargetView* rtv = view->GetRTVHandle();

	m_context->OMSetRenderTargets( 1 , &rtv , nullptr );


	if ( camera.m_texture != nullptr )
	{
		m_texture = camera.m_texture;
	} else
	{
		m_texture = m_swapChain->GetBackBuffer();
	}

	IntVec2 textureDimensions = m_texture->GetTexelSizeCoords();

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = ( float ) textureDimensions.x;
	viewport.Height = ( float ) textureDimensions.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_context->RSSetViewports( 1 , &viewport );

	ClaerScreen(camera.GetClearColor());
	BindShader( "" );

	m_lastBoundVBO = nullptr;

	BindUniformBuffer( 0 , m_frameUBO );
	BindUniformBuffer( 1 , camera.UpdateAndGetUBO( this ) );

	BindTexture( m_defaultColor );
	BindSampler( m_defaultSampler );

	SetBlendMode( BlendMode::ALPHA );

}

void RenderContext::EndCamera( const Camera& camera )
{
	 UNUSED(camera);
	 m_isDrawing = false;
}

void RenderContext::UpdateFrameTime( float deltaTime )
{
	frameData_t frameData;
	frameData.systemTime = (float)GetCurrentTimeSeconds();
	frameData.systemDeltaTime = deltaTime;

	m_frameUBO->Update( &frameData , sizeof( frameData ) , sizeof( frameData ) );
}

void RenderContext::TransformVertexArray( int numVertices, Vertex_PCU* vertices, float scale, float rotationDegrees, const Vec2& translation )
{
	for( int index=0; index<numVertices; index++ )
	{
		vertices[index].m_position=TransformPosition3DXY(vertices[index].m_position,scale,rotationDegrees,translation);
	}
}

void RenderContext::SetBlendMode( BlendMode blendMode )
{
	float const zeros[] = { 0,0,0,0 };

	switch ( blendMode )
	{
	case BlendMode::ALPHA:
		m_context->OMSetBlendState( m_alphaBlendState , zeros , ~0U );
		break;
	case BlendMode::ADDITIVE:
		m_context->OMSetBlendState( m_additiveBlendState , zeros , ~0U );
		break;
	default:
		break;
	}


}

BitmapFont* RenderContext::CreateBitMapFontFromFile( std::string filePath )
{
	filePath.append(".png");
	Texture* texture = GetOrCreateTextureFromFile(filePath.c_str());

	BitmapFont* bitmap = new BitmapFont("BitMapFont",texture);
	m_LoadedFonts[filePath] = bitmap;
	return bitmap;
}

BitmapFont* RenderContext::GetOrCreateBitMapFontFromFile( std::string filePath )
{
	BitmapFont* temp=m_LoadedFonts[filePath];
	if( temp==nullptr )
	{
		temp=CreateBitMapFontFromFile( filePath );
	}

	return temp;
}

void RenderContext::CreateBlendStates()
{
	D3D11_BLEND_DESC alphaDesc;
	alphaDesc.AlphaToCoverageEnable = false;
	alphaDesc.IndependentBlendEnable = false;
	alphaDesc.RenderTarget[ 0 ].BlendEnable = true;
	alphaDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
	// render all output
	alphaDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState( &alphaDesc , &m_alphaBlendState );
	D3D11_BLEND_DESC additiveDesc;
	additiveDesc.AlphaToCoverageEnable = false;
	additiveDesc.IndependentBlendEnable = false;
	additiveDesc.RenderTarget[ 0 ].BlendEnable = true;
	additiveDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
	// render all output
	additiveDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState( &additiveDesc , &m_additiveBlendState );

}

void RenderContext::ClaerScreen( const Rgba8 clearColor )
{
	/*glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(float(clearColor.r),float(clearColor.g),float(clearColor.b),float(clearColor.a));*/
	float clearFloats[ 4 ];
	clearFloats[ 0 ] = ( float ) clearColor.r / 255.0f;
	clearFloats[ 1 ] = ( float ) clearColor.g / 255.0f;
	clearFloats[ 2 ] = ( float ) clearColor.b / 255.0f;
	clearFloats[ 3 ] = ( float ) clearColor.a / 255.0f;

	Texture* backbuffer = m_swapChain->GetBackBuffer();
	TextureView* backbuffer_rtv = backbuffer->GetRenderTargetView();
	ID3D11RenderTargetView* rtv = backbuffer_rtv->GetRTVHandle();

	m_context->ClearRenderTargetView( rtv , clearFloats );

	//GUARANTEE_OR_DIE( false , "clrscr" );

}


void RenderContext::BindTexture(const Texture* texture )
{
	Texture* tex;
	
	if(texture!=nullptr )
	{
		tex = const_cast< Texture* >( texture );
	}
	else
	{
		tex = m_defaultColor;
	}


	TextureView* shaderResourceView= tex->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetSRV();
	m_context->PSSetShaderResources( 0 , 1 , &srvHandle );
}

void RenderContext::BindSampler( const Sampler* sampler )
{
	ID3D11SamplerState* samplerHandle = sampler->m_handle;
	m_context->PSSetSamplers( 0 , 1 , &samplerHandle );
}



void RenderContext::CreateDebugModule()
{
	// load the dll
	m_debugModule = ::LoadLibraryA( "Dxgidebug.dll" );
	if ( m_debugModule == nullptr )
	{
		DebuggerPrintf( "gfx" , "Failed to find dxgidebug.dll.  No debug features enabled." );
	}
	else
	{
		// find a function in the loaded dll
		typedef HRESULT( WINAPI * GetDebugModuleCB )( REFIID , void** );
		GetDebugModuleCB cb = ( GetDebugModuleCB ) ::GetProcAddress( (HMODULE)m_debugModule , "DXGIGetDebugInterface" );

		// create our debug object
		HRESULT hr = cb( __uuidof( IDXGIDebug ) , ( void** ) & m_debug );
		ASSERT_OR_DIE( SUCCEEDED( hr ),"Failed debug" );
	}
}

// cleanup after ourselves
void RenderContext::DestroyDebugModule()
{
	if ( nullptr != m_debug )
	{
		DX_SAFE_RELEASE( m_debug );   // release our debug object
		FreeLibrary((HMODULE) m_debugModule ); // unload the dll

		m_debug = nullptr;
		m_debugModule = nullptr;
	}
}

// This method will list all current live D3D objects, types, and reference counts
void RenderContext::ReportLiveObjects()
{
	if ( nullptr != m_debug )
	{
		m_debug->ReportLiveObjects( DXGI_DEBUG_ALL , DXGI_DEBUG_RLO_DETAIL );
	}
}

