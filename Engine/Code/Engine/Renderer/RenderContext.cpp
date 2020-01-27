
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/ThirdParty/stb_image.h"
#include "Engine/Core/Texture.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#define UNUSED(x) (void)(x);

#include "Engine/Core/D3D11Common.hpp"

#define RENDER_DEBUG


void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	/*glBegin(GL_TRIANGLES);
	{
		for( int vertIndex=0; vertIndex<numVertexes; vertIndex++ )
		{
			const Vertex_PCU& vert = vertexes[ vertIndex ];
			glTexCoord2f(vert.m_uvTexCoords.x,vert.m_uvTexCoords.y);
			glColor4ub(vert.m_color.r,vert.m_color.g,vert.m_color.b,vert.m_color.a);
			glVertex3f(vert.m_position.x,vert.m_position.y,vert.m_position.z);
		}

	}
	glEnd();*/
	UNUSED( vertexes );
	UNUSED( numVertexes );
	GUARANTEE_OR_DIE( false , "DrawVertexArray" );
}

void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU> &verts )
{
	/*glBegin( GL_TRIANGLES );
	{
		for( int vertIndex=0; vertIndex<verts.size(); vertIndex++ )
		{
			glTexCoord2f( verts[vertIndex].m_uvTexCoords.x, verts[vertIndex].m_uvTexCoords.y );
			glColor4ub( verts[vertIndex].m_color.r, verts[vertIndex].m_color.g, verts[vertIndex].m_color.b,verts[vertIndex].m_color.a );
			glVertex3f( verts[vertIndex].m_position.x, verts[vertIndex].m_position.y, verts[vertIndex].m_position.z );
		}

	}
	glEnd();*/
	UNUSED( verts );
	GUARANTEE_OR_DIE( false , "Draw function" );
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

Texture* RenderContext::CreateTextureFromFile(  const char* imageFilePath )
{
	UNUSED( imageFilePath );
	////const char* imageFilePath = "Data/Images/Test_StbiFlippedAndOpenGL.png";
	//unsigned int textureID = 0;
	//int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	//int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	//int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	//int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
	//stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	//unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );
	//// Check if the load was successful
	//GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );
	//GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );
	//glEnable( GL_TEXTURE_2D );
	//glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	//glGenTextures( 1, (GLuint*)&textureID );
	//glBindTexture( GL_TEXTURE_2D, textureID );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	////These 2 lines below are compulsory Please don't remove them.
	//// Set magnification (texel > pixel) and minification (texel < pixel) filters
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR
	//GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	//if( numComponents == 3 )
	//{
	//	bufferFormat = GL_RGB;
	//}
	//GLenum internalFormat = bufferFormat;
	//glTexImage2D(            // Upload this pixel data to our new OpenGL texture
	//	GL_TEXTURE_2D,        // Creating this as a 2d texture
	//	0,                    // Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
	//	internalFormat,        // Type of texel format we want OpenGL to use for this texture internally on the video card
	//	imageTexelSizeX,    // Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
	//	imageTexelSizeY,    // Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
	//	0,                    // Border size, in texels (must be 0 or 1, recommend 0)
	//	bufferFormat,        // Pixel format describing the composition of the pixel data in buffer
	//	GL_UNSIGNED_BYTE,    // Pixel color components are unsigned bytes (one byte per color channel/component)
	//	imageData );        // Address of the actual pixel data bytes/buffer in system memory
	//						// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory

	//stbi_image_free( imageData );
	//Texture* temp=new Texture(imageFilePath,textureID,IntVec2(imageTexelSizeX,imageTexelSizeY));
	//m_LoadedTextures[imageFilePath]=temp;
	//return temp;

	GUARANTEE_OR_DIE( false , "Creating texture" );
	return nullptr;
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

void RenderContext::Startup( Window* window )
{
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

	//swapchain->Release();
}

void RenderContext::Shutdown()
{
	if ( m_device != nullptr )
	{
		m_device->Release();
		m_device = nullptr;
		m_context->Release();
		m_context = nullptr;
	}

	delete m_swapChain;
	m_swapChain = nullptr;
}

void RenderContext::BeginFrame()
{
	

}

void RenderContext::EndFrame()
{
	m_swapChain->Present();
}

void RenderContext::BeginCamera(const Camera &camera)
{
	//glLoadIdentity();
	//glOrtho(camera.GetOrthoBottomLeft().x,camera.GetOrthoTopRight().x,camera.GetOrthoBottomLeft().y,camera.GetOrthoTopRight().y,0.f,1.f);
	//
	UNUSED( camera );
	GUARANTEE_OR_DIE( false , "begin camera" );
}

void RenderContext::EndCamera( const Camera& camera )
{
	 UNUSED(camera);
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
	/*if( blendMode == BlendMode::ALPHA )
	{
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else if( blendMode == BlendMode::ADDITIVE )
	{
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	}
	else
	{
		ERROR_AND_DIE( Stringf( "Unknown / unsupported blend mode #%i", blendMode ) );
	}*/
	UNUSED( blendMode );
	GUARANTEE_OR_DIE( false , "Blend mode" );

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

void RenderContext::ClaerScreen( const Rgba8 clearColor )
{
	/*glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(float(clearColor.r),float(clearColor.g),float(clearColor.b),float(clearColor.a));*/

	GUARANTEE_OR_DIE( false , "clrscr" );

}


void RenderContext::BindTexture( const Texture* texture )
{
	/*if( texture )
	{
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, texture->GetTextureID() );
	}
	else
	{
		glDisable( GL_TEXTURE_2D );
	}*/
	UNUSED( texture );
	GUARANTEE_OR_DIE( false , "BindTexture" );
}

