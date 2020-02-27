#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

GPUMesh::GPUMesh( RenderContext* owner )
{
	m_vertices = new VertexBuffer( owner , MEMORY_HINT_DYNAMIC );
}

void GPUMesh::UpdateVertices( unsigned int vcount , void const* vertexData , unsigned int vertexStride , buffer_attribute_t const* layout )
{
	//m_vertices->Update()
}
