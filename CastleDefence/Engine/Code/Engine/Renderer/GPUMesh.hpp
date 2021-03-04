#pragma once

class RenderContext;
class VertexBuffer;
class IndexBuffer;
struct buffer_attribute_t;
#include "Engine/Renderer/RenderBuffer.hpp"

class GPUMesh // A04
{
public:
	GPUMesh( RenderContext* owner );
	~GPUMesh();

	void UpdateVertices( unsigned int vcount , void const* vertexData , unsigned int vertexStride , buffer_attribute_t* layout );
	void UpdateIndices( unsigned int icount , unsigned int const* indices );
	void UpdateIndices( unsigned int icount,void const* vertexData );

	int GetIndexCount() const;

	template <typename VERTEX_TYPE>
	void UpdateVertices( unsigned int vcount , VERTEX_TYPE const* vertices )
	{
		if ( vcount == 0 )
		{

		}

		m_vertices->m_attribute = VERTEX_TYPE::LAYOUT;
		m_vertices->m_stride = sizeof( VERTEX_TYPE );
		UpdateVertices( vcount , vertices , sizeof( VERTEX_TYPE ) , VERTEX_TYPE::LAYOUT );
	}

	int m_vertexCount = 0;
	int m_indexCount = 0;

public:
	VertexBuffer* m_vertices=nullptr;  // like a std::vector<VertexPCU> -> but on GPU
	IndexBuffer* m_indices=nullptr; 	// like a std::vector<uint> -> but on the GPU

	// optionally
	// std::vector<uint8_t> m_cpuVertexData; 
	// std::Vector<uint> m_cpuIndexData; 
};