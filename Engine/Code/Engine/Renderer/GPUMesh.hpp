#pragma once

class RenderContext;
class VertexBuffer;
struct buffer_attribute_t;

class GPUMesh // A04
{
public:
	GPUMesh( RenderContext* owner );

	void UpdateVertices( unsigned int vcount , void const* vertexData , unsigned int vertexStride , buffer_attribute_t const* layout );
	void UpdateIndices( unsigned int icount , unsigned int const* indices );

	int GetIndexCount() const;

	template <typename VERTEX_TYPE>
	void UpdateVertices( unsigned int vcount , VERTEX_TYPE const* vertices )
	{
		UpdateVertices( vcount , vertices , sizeof( VERTEX_TYPE ) , VERTEX_TYPE::LAYOUT );
	}

public:
	VertexBuffer* m_vertices;  // like a std::vector<VertexPCU> -> but on GPU
	//IndexBuffer* m_indices; 	// like a std::vector<uint> -> but on the GPU

	// optionally
	// std::vector<uint8_t> m_cpuVertexData; 
	// std::Vector<uint> m_cpuIndexData; 
};