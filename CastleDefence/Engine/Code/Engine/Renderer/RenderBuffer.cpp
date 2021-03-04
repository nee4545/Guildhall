#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/D3D11Common.hpp"

RenderBuffer::RenderBuffer( RenderContext* owner , eRenderBufferUsage usage , eRenderMemoryHint memHint )
{
	m_owner = owner;
	m_usage = usage;
	m_memHint = memHint;

	m_handle = nullptr;
	m_bufferByteSize = 0U;
	m_elementByteSize = 0U;
}


RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE( m_handle );
}


D3D11_USAGE ToDXMemoryUsage( eRenderMemoryHint hint )
{
	switch ( hint )
	{
	case MEMORY_HINT_GPU:
		return D3D11_USAGE_DEFAULT;
		break;
	case MEMORY_HINT_DYNAMIC:
		return D3D11_USAGE_DYNAMIC;
		break;
	case MEMORY_HINT_STAGING:
		return D3D11_USAGE_STAGING;
		break;
	default:
		ERROR_AND_DIE( "undefined hint" );
		break;
	}
}

unsigned int ToDXUsage( eRenderBufferUsage usage )
{
	unsigned int ret = 0;

	if ( usage & VERTEX_BUFFER_BIT )
	{
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if ( usage & INDEX_BUFFER_BIT )
	{
		ret |= D3D11_BIND_INDEX_BUFFER;
	}

	if ( usage & UNIFORM_BUFFER_BIT )
	{
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	return ret;
}

bool RenderBuffer::Update( void const* data , size_t dataByteSize , size_t elementByteSize )
{
	//If not compatiable - destroy old buffer
		//1.our elementSIze matches the passed in
		// for GPU buffersizes must match
	    // for dynamic passed in passsize< bugffersize

	if ( !IsCompatible(dataByteSize,elementByteSize) )
	{
		CleanUp(); // destroy buffer
	}

	// if no buffer create 
	if ( m_handle == nullptr )
	{
		Create( dataByteSize , elementByteSize );
	}

	ID3D11DeviceContext* ctx = m_owner->m_context;
	if ( m_memHint == MEMORY_HINT_DYNAMIC )
	{
		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT result = ctx->Map( m_handle , 0 , D3D11_MAP_WRITE_DISCARD , 0 , &mapped );

		if ( SUCCEEDED( result ) )
		{
			memcpy( mapped.pData , data , dataByteSize );
			ctx->Unmap( m_handle , 0 );
		}
		else
		{
			return false;
		}
	}
	else
	{
		ctx->UpdateSubresource( m_handle , 0 , nullptr , data , 0 , 0 );
	}


	//Mapping
	//Only available to DYNAMIC buffer


	//CopySubresource(direct copy)
	//This is only available to GPU buffers that have exactly

	return true;
}

bool RenderBuffer::IsCompatible( size_t dataByteSize , size_t elementByteSize ) const
{
	if ( m_handle == nullptr )
	{
		return false;
	}

	if ( m_elementByteSize != elementByteSize )
	{
		return false;
	}

	if ( m_memHint == MEMORY_HINT_DYNAMIC )
	{
		return dataByteSize <= m_bufferByteSize;
	}
	else
	{
		return dataByteSize == m_bufferByteSize;
	}
}

void RenderBuffer::CleanUp()
{
	DX_SAFE_RELEASE( m_handle );
	m_bufferByteSize = 0;
	m_elementByteSize = 0;
}

bool RenderBuffer::Create( size_t dataByteSize , size_t elementByteSize )
{
	ID3D11Device* device = m_owner->m_device;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (unsigned int)dataByteSize;
	desc.Usage = ToDXMemoryUsage( m_memHint );
	desc.BindFlags = ToDXUsage( m_usage );

	if ( m_memHint == MEMORY_HINT_DYNAMIC )
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if(m_memHint == MEMORY_HINT_STAGING )
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	desc.MiscFlags =0;
	desc.StructureByteStride = (unsigned int)elementByteSize;

	m_elementByteSize = elementByteSize;
	m_bufferByteSize = dataByteSize;

	device->CreateBuffer( &desc , nullptr , &m_handle );

	return( m_handle != nullptr );
}

void IndexBuffer::Update( unsigned int icount , unsigned int const* indices )
{
	RenderBuffer::Update( indices , icount * sizeof( unsigned int ) , sizeof( unsigned int ) );
}

void IndexBuffer::Update( std::vector<unsigned int> const& indices )
{
	Update( (unsigned int)indices.size() , &indices[ 0 ] );
}
