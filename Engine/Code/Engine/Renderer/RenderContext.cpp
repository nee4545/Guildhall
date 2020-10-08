#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/ThirdParty/stb_image.h"
#include "Engine/Core/Texture.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Polygon2D.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Material.hpp"


#define UNUSED(x) (void)(x);

#include "Engine/Core/D3D11Common.hpp"

#define RENDER_DEBUG


BitmapFont* g_theBitMapFont = nullptr;


void RenderContext::DrawVertexArray( int numVertexes , const Vertex_PCU* vertexes )
{
	//Update a vertex buffer
	size_t bufferByteSize = numVertexes * sizeof( Vertex_PCU );
	size_t elementSize = sizeof( Vertex_PCU );
	m_immediateVBO->Update( vertexes , bufferByteSize , elementSize );
	m_immediateVBO->m_attribute = Vertex_PCU::LAYOUT;
	m_immediateVBO->m_stride = sizeof( Vertex_PCU );
	//Bind 
	BindVertexBuffer( m_immediateVBO );
	Draw( numVertexes , 0 );
}

void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& verts )
{
	if ( verts.size() > 0 )
	{
		DrawVertexArray( ( int ) verts.size() , &verts[ 0 ] );
	}
}

void RenderContext::DrawVertexArray( int numVertexes , VertexBuffer* vertices )
{
	BindVertexBuffer( vertices );
	Draw( numVertexes , 0 );
}

void RenderContext::DrawIndexed( unsigned int indexCount , unsigned int startIndex , unsigned int indexStride , buffer_attribute_t* layout )
{
	m_context->VSSetShader( m_currentShader->m_vertexStage.m_vs , nullptr , 0 );
	m_context->RSSetState( m_rasterState );
	m_context->PSSetShader( m_currentShader->m_fragmentStage.m_fs , nullptr , 0 );

	ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout( layout );
	m_context->IASetInputLayout( inputLayout );

	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	m_context->DrawIndexed( indexCount , startIndex , indexStride );
}

void RenderContext::DrawAABB2D( const AABB2& aabb , const Rgba8& color )
{
	Vertex_PCU vert[ 6 ]
	{
		Vertex_PCU( Vec3( aabb.mins.x,aabb.mins.y,0.f ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,0.f ),color,Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),color,Vec2( 0.f,1.f ) ),

		Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,0.f ),color,Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),color,Vec2( 1.f,1.f ) ),
		Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),color,Vec2( 0.f,1.f ) )

	};

	DrawVertexArray( 6 , vert );

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

void RenderContext::DrawLine( const Vec2& start , const Vec2& end , const Rgba8& color , float thickness )
{
	Vec2 fwd = end - start;
	fwd.SetLength( thickness * 0.5f );
	Vec2 lef = fwd.GetRotated90Degrees();

	Vec2 endRight = end + fwd - lef;
	Vec2 startRight = start - fwd - lef;
	Vec2 endLeft = end + fwd + lef;
	Vec2 startLeft = start - fwd + lef;

	Vec3 er = Vec3( endRight.x , endRight.y , 0.f );
	Vec3 el = Vec3( endLeft.x , endLeft.y , 0.f );
	Vec3 sr = Vec3( startRight.x , startRight.y , 0.f );
	Vec3 sl = Vec3( startLeft.x , startLeft.y , 0.f );

	Vertex_PCU vert1[ 3 ] =
	{
		Vertex_PCU( sr,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( er,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( el,color,Vec2( 0.f,0.f ) )
	};

	Vertex_PCU vert2[ 3 ] =
	{
		Vertex_PCU( sr,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( el,color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( sl,color,Vec2( 0.f,0.f ) )
	};

	DrawVertexArray( 3 , vert1 );
	DrawVertexArray( 3 , vert2 );
}

void RenderContext::DrawArrow2D( Vec2& start , Vec2& end , Rgba8 lineColor , Rgba8 arrowColor , float lineThickness )
{
	DrawLine( start , end , lineColor , lineThickness );

	Vec2 dir = ( end - start ).GetNormalized();

	Vec2 pt1 = end + ( dir * 3.f );
	Vec2 pt2 = end + ( dir.GetRotated90Degrees() * 3.f );
	Vec2 pt3 = end + ( dir.GetRotatedMinus90Degrees() * 3.f );

	Vertex_PCU verts[ 3 ] =
	{
		Vertex_PCU( Vec3( pt1,0.f ),arrowColor,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( pt2,0.f ),arrowColor,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( pt3,0.f ),arrowColor,Vec2( 0.f,0.f ) )
	};

	DrawVertexArray( 3 , verts );
}

void RenderContext::DrawRing( const Vec2 centre , float radius , Rgba8 color , float thickness )
{
	constexpr float theta = 360.f / 16.f;

	for ( int index = 0; index < 16; index++ )
	{
		float degree1 = theta * float( index );
		float degree2 = theta * float( index + 1 );

		DrawLine( centre + Vec2( radius * CosDegrees( degree1 ) , radius * SinDegrees( degree1 ) ) , centre + Vec2( radius * CosDegrees( degree2 ) , radius * SinDegrees( degree2 ) ) , color , thickness );
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

void RenderContext::DrawSector( const Vec2 centre , const Vec2 forwardVector ,float radius, float angle , Rgba8 color )
{
	std::vector<Vertex_PCU> verts;


	float deltaDegrees = 1.f;
	float currentDegrees = -angle * 0.5f;



	while ( currentDegrees < angle * 0.5f )
	{
		Vertex_PCU v1 = Vertex_PCU( Vec3(centre,0.f) , color , Vec2( 0.f , 0.f ) );
		Vec2 vec1 = centre + forwardVector.GetRotatedDegrees( currentDegrees ) * radius;
		Vec2 vec2 = centre + forwardVector.GetRotatedDegrees( currentDegrees+deltaDegrees ) * radius;
		Vertex_PCU v2 = Vertex_PCU( Vec3( vec1 , 0.f ) , color , Vec2( 0.f , 0.f ) );
		Vertex_PCU v3 = Vertex_PCU( Vec3( vec2 , 0.f ) , color , Vec2( 0.f , 0.f ) );

		verts.push_back( v1 );
		verts.push_back( v2 );
		verts.push_back( v3 );

		currentDegrees += deltaDegrees;
	}

	
	DrawVertexArray( verts );
}

void RenderContext::DrawMesh( GPUMesh* mesh )
{
	BindVertexBuffer( mesh->m_vertices );


	bool hasIndices = mesh->GetIndexCount() > 0;

	if ( hasIndices )
	{
		BindIndexBuffer( mesh->m_indices );
		DrawIndexed( mesh->GetIndexCount() , 0 , 0 , mesh->m_vertices->m_attribute );
	}
	else
	{
		Draw( mesh->m_vertexCount , 0 , mesh->m_vertices->m_attribute );
	}
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

	m_currentShader = GetOrCreateShader( filename.c_str() );
}

void RenderContext::BindShaderState( ShaderState* state )
{
	if ( state == nullptr )
	{
		BindShader( nullptr );
		SetBlendMode( BlendMode::ALPHA );
		return;
	}

	BindShader( state->m_shader );
	CreateRasterState( state->m_fillmode , state->m_culling , state->m_windingOrderCounterClockwise );
	SetDepthTest( state->m_depthTest , state->m_writeDepth );
	SetBlendMode( state->m_blendMode );

}

void RenderContext::BindMaterial( Material* material )
{
	if ( material == nullptr )
	{
		BindShaderState( nullptr );
		BindTexture( nullptr );
		BindSampler( m_defaultSampler );
		m_materialUBO = nullptr;
		m_materialUBO = new RenderBuffer( this , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
		BindUniformBuffer( UBO_MATERIAL_SLOT , m_materialUBO );
	}
	BindShaderState( material->GetShaderState() );


	if ( material->m_ubo != nullptr )
	{
		BindUniformBuffer( UBO_MATERIAL_SLOT , material->m_ubo );
	}
	for ( auto index : material->m_texturePerSlot )
	{
		BindTexture( index.second , ( eTextureSlot ) index.first );
	}
	if ( material->m_samplersPerSlot.size() > 0 )
	{
		BindSampler( material->m_samplersPerSlot[ 0 ] );
	}
	else
	{
		BindSampler( m_defaultSampler );
	}

	m_lights.specularFactor = material->m_specularFactor;
	m_lights.specularPower = material->m_specularPower;
}

void RenderContext::BindVertexBuffer( VertexBuffer* vbo )
{
	ID3D11Buffer* vboHandle = vbo->m_handle;
	unsigned int stride = vbo->m_stride;/*sizeof( Vertex_PCU );*/
	unsigned int offset = 0;

	if ( m_lastBoundVBO != vboHandle )
	{
		m_context->IASetVertexBuffers( 0 , 1 , &vboHandle , &stride , &offset );
		m_lastBoundVBO = vboHandle;
	}
}

void RenderContext::BindIndexBuffer( IndexBuffer* ibo )
{
	ID3D11Buffer* iboHandle = ibo->m_handle;
	unsigned int offset = 0;

	m_context->IASetIndexBuffer( iboHandle , DXGI_FORMAT_R32_UINT , offset );

	// 	if ( m_lastBoundIBO != iboHandle )
	// 	{
	// 		m_lastBoundIBO = iboHandle;
	// 	}
	// 	else
	// 	{
	// 		m_context->IASetIndexBuffer( m_lastBoundIBO , DXGI_FORMAT_R32_UINT , offset );
	// 	}
}

void RenderContext::BindDepthStencil( Texture* dsv )
{
	ID3D11RenderTargetView* rtvCopy = m_texture->GetRenderTargetView()->GetRTVHandle();
	ID3D11RenderTargetView* const* rtv = &rtvCopy;

	if ( dsv == nullptr )
	{
		m_context->OMSetRenderTargets( 1 , rtv , nullptr );
		return;
	}

	TextureView* tempDsv = dsv->GetOrCreateDepthBuffer( m_texture->GetTexelSizeCoords() , this );
	m_context->OMSetRenderTargets( 1 , rtv , tempDsv->m_dsv );
}

void RenderContext::SetDepthTest( eCompareOp comparision , bool writePass )
{
	D3D11_COMPARISON_FUNC cmp;
	D3D11_DEPTH_WRITE_MASK mask;
	switch ( comparision )
	{
	case COMPARE_NEVER:
		cmp = D3D11_COMPARISON_NEVER;
		break;
	case COMPARE_ALWAYS:
		cmp = D3D11_COMPARISON_ALWAYS;
		break;
	case COMPARE_EQUAL:
		cmp = D3D11_COMPARISON_EQUAL;
		break;
	case COMPARE_NOTEQUAL:
		cmp = D3D11_COMPARISON_NOT_EQUAL;
		break;
	case COMPARE_LESS:
		cmp = D3D11_COMPARISON_LESS;
		break;
	case COMPARE_LEQUAL:
		cmp = D3D11_COMPARISON_LESS_EQUAL;
		break;
	case COMPARE_GREATER:
		cmp = D3D11_COMPARISON_GREATER;
		break;
	case COMPARE_GEQUAL:
		cmp = D3D11_COMPARISON_GREATER_EQUAL;
		break;
	default:
		cmp = D3D11_COMPARISON_NEVER;
		break;
	}

	if ( writePass )
	{
		mask = D3D11_DEPTH_WRITE_MASK_ALL;
	}
	else
	{
		mask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	if ( m_depthStencilState )
	{
		DX_SAFE_RELEASE( m_depthStencilState );
	}
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = mask;
	dsDesc.DepthFunc = cmp;
	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Create depth stencil state
	m_device->CreateDepthStencilState( &dsDesc , &m_depthStencilState );
	m_context->OMSetDepthStencilState( m_depthStencilState , 1 );
}

void RenderContext::ClearDepth( Texture* dsTarget , float depth )
{
	TextureView* view = dsTarget->GetOrCreateDepthBuffer( m_texture->GetTexelSizeCoords() , this );
	ID3D11DepthStencilView* dsv = view->GetDsvHandle();
	m_context->ClearDepthStencilView( dsv , D3D11_CLEAR_DEPTH , depth , 0 );
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

void RenderContext::SetModalMatrix( Mat44 mat )
{
	if ( m_modelUBO == nullptr )
	{
		m_modelUBO = new RenderBuffer( this , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}

	modelData_t data;
	data.modal = mat;

	m_modelUBO->Update( &data , sizeof( data ) , sizeof( data ) );

	BindUniformBuffer( 2 , m_modelUBO );

}

void RenderContext::BindUniformBuffer( unsigned int slot , RenderBuffer* ubo )
{
	ID3D11Buffer* uboHandle = ubo->m_handle;

	m_context->VSSetConstantBuffers( slot , 1 , &uboHandle );
	m_context->PSSetConstantBuffers( slot , 1 , &uboHandle );
}


void RenderContext::CreateRasterState( D3D11_FILL_MODE fillmode , D3D11_CULL_MODE cullmode , bool frontCounterClockWise /*= true */ )
{
	D3D11_RASTERIZER_DESC desc;

	desc.FillMode = fillmode;
	desc.CullMode = cullmode;
	desc.FrontCounterClockwise = frontCounterClockWise;
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	ID3D11Device* device = m_device;

	if ( m_rasterState != nullptr )
	{
		DX_SAFE_RELEASE( m_rasterState );
	}

	device->CreateRasterizerState( &desc , &m_rasterState );
}

void RenderContext::CreateRasterState( D3D11_RASTERIZER_DESC desc )
{
	ID3D11Device* device = m_device;

	if ( m_rasterState != nullptr )
	{
		DX_SAFE_RELEASE( m_rasterState );
	}

	device->CreateRasterizerState( &desc , &m_rasterState );
}

void RenderContext::CreateRasterState( eFillMode fillmode , eCullMode cullmode , bool frontCounterClockWise /*= true */ )
{
	D3D11_RASTERIZER_DESC desc;

	switch ( fillmode )
	{
	case SOLID:
		desc.FillMode = D3D11_FILL_SOLID;
		break;
	case WIRE_FRAME:
		desc.FillMode = D3D11_FILL_WIREFRAME;
		break;
	default:
		break;
	}

	switch ( cullmode )
	{
	case CULL_NONE:
		desc.CullMode = D3D11_CULL_NONE;
		break;
	case CULL_FRONT:
		desc.CullMode = D3D11_CULL_FRONT;
		break;
	case CULL_BACK:
		desc.CullMode = D3D11_CULL_BACK;
		break;
	default:
		break;
	}

	desc.FrontCounterClockwise = frontCounterClockWise;
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	ID3D11Device* device = m_device;

	if ( m_rasterState != nullptr )
	{
		DX_SAFE_RELEASE( m_rasterState );
	}

	device->CreateRasterizerState( &desc , &m_rasterState );

}

void RenderContext::DrawPolygonUnfilled( const Polygon2D& polygon , const Rgba8& color , float thickness )
{
	int numPoints = polygon.GetVertexCount();
	/*Vec2* points;
	points = new Vec2[ numPoints ];
	polygon.GetPoints( points );*/

	int start = 0;
	int end = 1;

	int counter = 0;

	while ( counter < numPoints )
	{
		DrawLine( polygon.m_points[ start ] , polygon.m_points[ end ] , color , thickness );
		start = ( start + 1 ) % numPoints;
		end = ( end + 1 ) % numPoints;
		counter++;
	}
}

void RenderContext::DrawPolygonFilled( const Polygon2D& polygon , const Rgba8& color )
{
	int numPoints = polygon.GetVertexCount();
	//Vec2* points;
	//points = new Vec2[ numPoints ];
	//polygon.GetPoints( points );

	int vert1 = 1;
	int vert2 = 2;

	int counter = 0;

	while ( counter < numPoints - 2 )
	{
		Vertex_PCU vertexs[ 3 ];
		vertexs[ 0 ] = Vertex_PCU( Vec3( polygon.m_points[ 0 ].x , polygon.m_points[ 0 ].y , 0.f ) , color , Vec2( 0.f , 0.f ) );
		vertexs[ 1 ] = Vertex_PCU( Vec3( polygon.m_points[ vert1 ].x , polygon.m_points[ vert1 ].y , 0.f ) , color , Vec2( 0.f , 0.f ) );
		vertexs[ 2 ] = Vertex_PCU( Vec3( polygon.m_points[ vert2 ].x , polygon.m_points[ vert2 ].y , 0.f ) , color , Vec2( 0.f , 0.f ) );

		DrawVertexArray( 3 , vertexs );

		vert1++;
		vert2++;
		counter++;

	}
}

Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath )
{

	//const char* imageFilePath = "Data/Images/Test_StbiFlippedAndOpenGL.png";
	//unsigned int textureID = 0;
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath , &imageTexelSizeX , &imageTexelSizeY , &numComponents , numComponentsRequested );
	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData , Stringf( "Failed to load image \"%s\"" , imageFilePath ) );
	//GUARANTEE_OR_DIE( numComponents == 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );


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
	Texture* temp = new Texture( imageFilePath , this , texHandle );
	m_LoadedTextures[ imageFilePath ] = temp;
	return temp;

}


Texture* RenderContext::GetOrCreateTextureFromFile( const char* imageFilePath )
{
	Texture* temp = m_LoadedTextures[ imageFilePath ];
	if ( temp == nullptr )
	{
		temp = CreateTextureFromFile( imageFilePath );
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


	D3D11CreateDeviceAndSwapChain( nullptr , D3D_DRIVER_TYPE_HARDWARE , nullptr ,
		flags , nullptr , 0 , D3D11_SDK_VERSION , &swapchainDesc , &swapchain , &m_device , nullptr , &m_context );

	m_swapChain = new SwapChain( this , swapchain );

	m_defaultShader = GetOrCreateShader( "Data/Shaders/DefaultVertexPCU.hlsl" );
	CreateRasterState( D3D11_FILL_SOLID , D3D11_CULL_NONE );

	m_immediateVBO = new VertexBuffer( this , MEMORY_HINT_DYNAMIC );
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

	for ( auto& x : m_LoadedTextures )
	{
		if ( x.second != nullptr )
		{
			delete x.second;
			x.second = nullptr;
		}
	}

	m_defaultColor = nullptr;

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

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

	if ( m_modelUBO != nullptr )
	{
		delete m_modelUBO;
		m_modelUBO = nullptr;
	}

	if ( m_lightUBO != nullptr )
	{
		delete m_lightUBO;
		m_lightUBO = nullptr;
	}

	if ( m_materialUBO != nullptr )
	{
		delete m_materialUBO;
		m_materialUBO = nullptr;
	}

	if ( m_fogUBO != nullptr )
	{
		delete m_fogUBO;
		m_fogUBO = nullptr;
	}

	delete m_swapChain;
	m_swapChain = nullptr;

	DX_SAFE_RELEASE( m_depthStencilState );
	DX_SAFE_RELEASE( m_rasterState );

	DX_SAFE_RELEASE( m_alphaBlendState );
	DX_SAFE_RELEASE( m_additiveBlendState );
	DX_SAFE_RELEASE( m_opaqueBlendState );

	//DX_SAFE_RELEASE( m_lastBoundIBO );
	//DX_SAFE_RELEASE( m_lastBoundVBO );

	DX_SAFE_RELEASE( m_context );
	DX_SAFE_RELEASE( m_device );

	ReportLiveObjects();
	DestroyDebugModule();

}

void RenderContext::BeginFrame()
{


}

void RenderContext::EndFrame()
{
	m_swapChain->Present();
}

void RenderContext::Draw( int numVertexes , int vertexOffset , buffer_attribute_t* layout /*= Vertex_PCU::LAYOUT */ )
{
	m_context->VSSetShader( m_currentShader->m_vertexStage.m_vs , nullptr , 0 );
	m_context->RSSetState( m_rasterState );
	m_context->PSSetShader( m_currentShader->m_fragmentStage.m_fs , nullptr , 0 );

	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//Describe vertex format to shader
	ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout( layout );
	m_context->IASetInputLayout( inputLayout );

	m_context->Draw( numVertexes , vertexOffset );
}

void RenderContext::BeginCamera( Camera& camera )
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

	if ( camera.m_backBuffer && ( camera.m_clearMode && CLEAR_DEPTH_BIT ) )
	{
		ClearDepth( camera.m_backBuffer , 1.f );
	}

	TextureView* view = colorTarget->GetRenderTargetView();
	ID3D11RenderTargetView* rtv = view->GetRTVHandle();

	m_context->OMSetRenderTargets( 1 , &rtv , nullptr );


	if ( camera.m_texture != nullptr )
	{
		m_texture = camera.m_texture;
	}
	else
	{
		m_texture = m_swapChain->GetBackBuffer();
		camera.m_texture = m_swapChain->GetBackBuffer();
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

	if ( camera.m_clearMode & CLEAR_COLOR_BIT )
	{
		ClaerScreen( camera.GetClearColor() );
	}
	BindShader( "" );

	m_lastBoundVBO = nullptr;

	SetModalMatrix( Mat44() );


	BindUniformBuffer( 0 , m_frameUBO );
	BindUniformBuffer( 1 , camera.UpdateAndGetUBO( this ) );
	BindUniformBuffer( 2 , m_modelUBO );

	BindTexture( m_defaultColor );
	BindSampler( m_defaultSampler );

	SetBlendMode( BlendMode::ALPHA );

}

void RenderContext::EndCamera( const Camera& camera )
{
	UNUSED( camera );
	m_isDrawing = false;
}

void RenderContext::UpdateFrameTime( float deltaTime )
{
	frameData_t frameData;
	frameData.systemTime = ( float ) GetCurrentTimeSeconds();
	frameData.systemDeltaTime = deltaTime;

	m_frameUBO->Update( &frameData , sizeof( frameData ) , sizeof( frameData ) );
}

void RenderContext::TransformVertexArray( int numVertices , Vertex_PCU* vertices , float scale , float rotationDegrees , const Vec2& translation )
{
	for ( int index = 0; index < numVertices; index++ )
	{
		vertices[ index ].m_position = TransformPosition3DXY( vertices[ index ].m_position , scale , rotationDegrees , translation );
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
	case BlendMode::OPAQE:
		m_context->OMSetBlendState( m_opaqueBlendState , zeros , ~0U );
		break;
	default:
		break;
	}


}

BitmapFont* RenderContext::CreateBitMapFontFromFile( std::string filePath )
{
	filePath.append( ".png" );
	Texture* texture = GetOrCreateTextureFromFile( filePath.c_str() );

	BitmapFont* bitmap = new BitmapFont( "BitMapFont" , texture );
	m_LoadedFonts[ filePath ] = bitmap;
	return bitmap;
}

BitmapFont* RenderContext::GetOrCreateBitMapFontFromFile( std::string filePath )
{
	BitmapFont* temp = m_LoadedFonts[ filePath ];
	if ( temp == nullptr )
	{
		temp = CreateBitMapFontFromFile( filePath );
	}

	return temp;
}

void RenderContext::SetAmbientColor( Rgba8 color )
{
	Vec4 c = color.GetAsNormalizedVec4();

	m_lights.ambientLight.x = c.x;
	m_lights.ambientLight.y = c.y;
	m_lights.ambientLight.z = c.z;
}

void RenderContext::SetAmbientIntensity( float intensity )
{
	m_lights.ambientLight.w = intensity;
}

void RenderContext::SetAmbientLight( Rgba8 color , float intensity )
{
	SetAmbientColor( color );
	m_lights.ambientLight.w = intensity;
}

void RenderContext::SetSpecularConstants( float specularPower , float specularFactor )
{
	m_lights.specularFactor = specularFactor;
	m_lights.specularPower = specularPower;
}

void RenderContext::EnableLight( unsigned int idx , light_t const& lightInfo )
{
	if ( m_lightUBO == nullptr )
	{
		m_lightUBO = new RenderBuffer( this , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}

	m_lights.light[ idx ] = lightInfo;

	m_lightUBO->Update( &m_lights , sizeof( m_lights ) , sizeof( m_lights ) );

	BindUniformBuffer( 3 , m_lightUBO );

}

void RenderContext::SetAttenuationFactors( eAttenuations factor , unsigned int lightId )
{
	Vec3 att;
	switch ( factor )
	{
	case ATTENUATION_LINEAR:
		att = Vec3( 0.f , 1.f , 0.f );
		break;
	case ATTENUATION_QUADRATIC:
		att = Vec3( 0.f , 0.f , 1.f );
		break;
	case ATTENUATION_CONSTANT:
		att = Vec3( 1.f , 0.f , 0.f );
		break;
	default:
		break;
	}

	m_lights.light[ lightId ].attenuation = att;
}

void RenderContext::SetSpecularFactor( float factor )
{
	m_lights.specularFactor = factor;
}

void RenderContext::SetSpecularPower( float power )
{
	m_lights.specularPower = power;
}

void RenderContext::SetSpecularAttenuation( Vec3 attenuation , unsigned int lightId )
{
	m_lights.light[ lightId ].specularAttunation = attenuation;
}

void RenderContext::SetDiffuseAttenuation( Vec3 attenuation , unsigned int lightId )
{
	m_lights.light[ lightId ].attenuation = attenuation;
}

void RenderContext::SetLightType( eLightTypes type , unsigned int lightId )
{
	switch ( type )
	{
	case POINT_LIGHT:
	{
		m_lights.light[ lightId ].directionFactor = 0.f;
		break;
	}
	case DIRECTIONAL_LIGHT:
	{
		m_lights.light[ lightId ].directionFactor = 1.f;
		break;
	}
	default:
		break;
	}
}

void RenderContext::BindMaterialData( void* data , unsigned int dataSize )
{
	if ( m_materialUBO == nullptr )
	{
		m_materialUBO = new RenderBuffer( this , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}

	m_materialUBO->Update( data , dataSize , dataSize );

	BindUniformBuffer( 5 , m_materialUBO );
}

void RenderContext::EnableFog( fog_t fogData )
{
	if ( m_fogUBO == nullptr )
	{
		m_fogUBO = new RenderBuffer( this , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}

	m_fogUBO->Update( &fogData , sizeof( fogData ) , sizeof( fogData ) );

	BindUniformBuffer( 4 , m_fogUBO );
}

void RenderContext::DisableFog()
{
	if ( m_fogUBO == nullptr )
	{
		m_fogUBO = new RenderBuffer( this , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}

	fog_t  temp;
	temp.nearFog = INFINITY;
	temp.farFog = INFINITY;

	m_fogUBO->Update( &temp , sizeof( temp ) , sizeof( temp ) );

	BindUniformBuffer( 4 , m_fogUBO );
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

	D3D11_BLEND_DESC opaqueDesc;
	opaqueDesc.AlphaToCoverageEnable = false;
	opaqueDesc.IndependentBlendEnable = false;
	opaqueDesc.RenderTarget[ 0 ].BlendEnable = false;
	opaqueDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	opaqueDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	opaqueDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	opaqueDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	opaqueDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	opaqueDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
	// render all output
	opaqueDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState( &opaqueDesc , &m_opaqueBlendState );

}

void RenderContext::ClaerScreen( const Rgba8 clearColor )
{
	float clearFloats[ 4 ];
	clearFloats[ 0 ] = ( float ) clearColor.r / 255.0f;
	clearFloats[ 1 ] = ( float ) clearColor.g / 255.0f;
	clearFloats[ 2 ] = ( float ) clearColor.b / 255.0f;
	clearFloats[ 3 ] = ( float ) clearColor.a / 255.0f;

	Texture* backbuffer = m_swapChain->GetBackBuffer();
	TextureView* backbuffer_rtv = backbuffer->GetRenderTargetView();
	ID3D11RenderTargetView* rtv = backbuffer_rtv->GetRTVHandle();

	m_context->ClearRenderTargetView( rtv , clearFloats );

}


void RenderContext::BindTexture( const Texture* texture , eTextureSlot textureType , unsigned int index )
{
	Texture* tex;

	if ( texture != nullptr )
	{
		tex = const_cast< Texture* >( texture );
	}
	else
	{
		tex = m_defaultColor;
	}

	TextureView* shaderResourceView = tex->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetSRV();
	m_context->PSSetShaderResources( ( UINT ) textureType + index , 1 , &srvHandle );
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
		typedef HRESULT( WINAPI* GetDebugModuleCB )( REFIID , void** );
		GetDebugModuleCB cb = ( GetDebugModuleCB ) ::GetProcAddress( ( HMODULE ) m_debugModule , "DXGIGetDebugInterface" );

		// create our debug object
		HRESULT hr = cb( __uuidof( IDXGIDebug ) , ( void** ) &m_debug );
		ASSERT_OR_DIE( SUCCEEDED( hr ) , "Failed debug" );
	}
}

// cleanup after ourselves
void RenderContext::DestroyDebugModule()
{
	if ( nullptr != m_debug )
	{
		DX_SAFE_RELEASE( m_debug );   // release our debug object
		FreeLibrary( ( HMODULE ) m_debugModule ); // unload the dll

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