#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

GPUMesh::GPUMesh( RenderContext* owner )
{
	m_vertices = new VertexBuffer( owner , MEMORY_HINT_DYNAMIC );
	m_indices = new IndexBuffer( owner , MEMORY_HINT_DYNAMIC );
}

void GPUMesh::UpdateVertices( unsigned int vcount , void const* vertexData , unsigned int vertexStride , buffer_attribute_t const* layout )
{
	m_vertexCount = vcount;
	size_t bufferByteSize = vcount * vertexStride;
	m_vertices->Update( vertexData , bufferByteSize, sizeof(layout) );
}

void GPUMesh::UpdateIndices( unsigned int icount , unsigned int const* indices )
{
	m_indexCount = icount;
	m_indices->Update( icount,indices );
}

void GPUMesh::UpdateIndices( unsigned int icount , void const* indexData )
{
	m_indexCount = icount;
	m_indices->Update( icount , (unsigned int*)indexData );
}

int GPUMesh::GetIndexCount() const
{
	return m_indexCount;
}

GPUMesh::~GPUMesh()
{
	if ( m_vertices != nullptr )
	{
		delete m_vertices;
		m_vertices = nullptr;
	}

	if ( m_indices != nullptr )
	{
		delete m_indices;
		m_indices = nullptr;
	}
}
