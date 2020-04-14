#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include <stdio.h>
#include <d3dcompiler.h>


void* FileReadToNewBuffer( std::string const& filename, size_t *outSize )
{
	FILE* fp = nullptr;

	fopen_s(&fp, filename.c_str() , "r" );
	if ( fp == nullptr )
	{
		return nullptr;
	}

	fseek( fp , 0 , SEEK_END );
	long file_size = ftell( fp );

	unsigned char* buffer = new unsigned char[ file_size+1 ];
	if ( buffer != nullptr )
	{
		fseek( fp , 0 , SEEK_SET );
		size_t bytes_read=fread( buffer , 1 , (size_t) file_size+1 , fp );
		buffer[ bytes_read ] = NULL;
	}

	if ( outSize != nullptr )
	{
		*outSize = ( size_t ) file_size;
	}

	fclose( fp );

	return buffer;
}


Shader::Shader( RenderContext* context ) :m_owner( context )
{
	//CreateRasterState();
}

Shader::~Shader()
{
	DX_SAFE_RELEASE( m_rasterState );
	DX_SAFE_RELEASE( m_inputLayout );
}

bool Shader::CreateFromFile( std::string const& filename )
{
	size_t file_size = 0;

	void* source = FileReadToNewBuffer( filename,&file_size );
	
	if ( source == nullptr )
	{
		return false;
	}

	m_vertexStage.Complie(m_owner,filename,source,file_size,SHADER_TYPE_VERTEX );
	m_fragmentStage.Complie(m_owner,filename, source,file_size,SHADER_TYPE_FRAGMENT);


	return m_vertexStage.IsValid() && m_fragmentStage.IsValid();
}

void Shader::CreateRasterState()
{
	D3D11_RASTERIZER_DESC desc;

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = TRUE; // the only reason we're doing this; 
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	ID3D11Device* device = m_owner->m_device;
	device->CreateRasterizerState( &desc , &m_rasterState );
}

ID3D11InputLayout* Shader::GetOrCreateInputLayout( buffer_attribute_t* attribute/*=Vertex_PCU::LAYOUT*/ )
{
	if ( m_inputLayout!=nullptr )
	{
		return m_inputLayout;
	}

	buffer_attribute_t const* attribsCopy = attribute;
	int count = 0;
	while ( attribsCopy->type != BUFFER_FORMAT_INVALID )
	{
		count++;
		attribsCopy++;
	}

	//int numAttributes = sizeof(attribute)/sizeof(buffer_attribute_t);
	D3D11_INPUT_ELEMENT_DESC* vertexDescription= new D3D11_INPUT_ELEMENT_DESC[ count ];

	buffer_attribute_t *current_ele = &attribute[ 0 ];
	int i = 0;
	while ( i < count )
	{
		vertexDescription[ i ].SemanticName = ( LPSTR ) current_ele->name.c_str();
		vertexDescription[ i ].SemanticIndex = 0;
		vertexDescription[ i ].Format = ( DXGI_FORMAT ) current_ele->type;
		vertexDescription[ i ].InputSlot = 0;
		vertexDescription[ i ].AlignedByteOffset = current_ele->offset;
		vertexDescription[ i ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexDescription[ i ].InstanceDataStepRate = 0;

		i++;
		current_ele = &attribute[ i ];
	}


	ID3D11Device* device = m_owner->m_device;
	device->CreateInputLayout( vertexDescription , count , m_vertexStage.GetByteCode() , m_vertexStage.GetByteCodeLength() , &m_inputLayout );

	return m_inputLayout;

}



static char const* GetDefaultEntryPointForStage( eShaderType type )
{
	switch ( type )
	{
	case SHADER_TYPE_VERTEX:
		return "VertexFunction";
		break;
	case SHADER_TYPE_FRAGMENT:
		return "FragmentFunction";
		break;
	default: ERROR_AND_DIE( "invalid Format" );
		//return "";
	}
}


static char const* GetDefaultEntryModelForStage( eShaderType stage )
{
	switch ( stage )
	{
	case SHADER_TYPE_VERTEX:
		return "vs_5_0";
		break;
	case SHADER_TYPE_FRAGMENT:
		return "ps_5_0";
		break;
	default: ERROR_AND_DIE( "invalid Format" );
		//return "";
	}
}

bool ShaderStage::Complie( RenderContext* ctx , std::string const& filename , void const* source , size_t const sourceByteLen , eShaderType stage )
{
	//HLSL gets complied to byte code
	//Link ByteCode -> Device assembly

	char const* entryPoint = GetDefaultEntryPointForStage( stage );
	char const* shaderModel = GetDefaultEntryModelForStage( stage );

	DWORD compileFlags = 0U;
#if defined(DEBUG_SHADERS)
	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors = nullptr;

	HRESULT hr = ::D3DCompile( source ,
		sourceByteLen ,
		filename.c_str() ,
		nullptr ,
		D3D_COMPILE_STANDARD_FILE_INCLUDE ,
		entryPoint ,
		shaderModel ,
		compileFlags ,
		0 ,
		&byteCode ,
		&errors );

	if ( FAILED( hr ) || ( errors != nullptr ) )
	{
		if ( errors != nullptr )
		{
			char* error_string = ( char* ) errors->GetBufferPointer();
			DebuggerPrintf( "Failed to compile [%s].  Compiler gave the following output;\n%s" ,
				filename.c_str() ,
				error_string );
			DEBUGBREAK();
		}
	}
	else
	{
		ID3D11Device* device = ctx->m_device;
		void const* byteCodePtr = byteCode->GetBufferPointer();
		size_t byteCodeSize = byteCode->GetBufferSize();

		switch ( stage )
		{
		case SHADER_TYPE_VERTEX:
			hr =device->CreateVertexShader( byteCodePtr , byteCodeSize , nullptr , &m_vs );
			GUARANTEE_OR_DIE( SUCCEEDED( hr ),"Failed to link" );
			break;
		case SHADER_TYPE_FRAGMENT:
			hr = device->CreatePixelShader( byteCodePtr , byteCodeSize , nullptr , &m_fs );
			GUARANTEE_OR_DIE( SUCCEEDED( hr ) , "Failed to link" );
			break;
		default:
			break;
		}
	}



	DX_SAFE_RELEASE( errors );

	if ( stage == SHADER_TYPE_VERTEX )
	{
		m_byteCode = byteCode;
	}
	else
	{
		DX_SAFE_RELEASE( byteCode );
		m_byteCode = nullptr;
	}

	return IsValid();

}

bool ShaderStage::IsValid()
{
	if ( m_handle != nullptr )
	{
		return true;
	}
	else
		return false;
}

void const* ShaderStage::GetByteCode() const
{
	return m_byteCode->GetBufferPointer();
}

size_t ShaderStage::GetByteCodeLength() const
{
	return m_byteCode->GetBufferSize();
}

ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE( m_byteCode );
	DX_SAFE_RELEASE( m_handle );
}
