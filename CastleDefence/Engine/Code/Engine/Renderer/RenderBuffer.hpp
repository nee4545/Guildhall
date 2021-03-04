#pragma once

//A GPU allocator
#include "Engine/Core/VertexCommon.hpp"
#include <vector>

struct ID3D11Buffer;
class RenderContext;
#define BIT_FLAG(b) (1<<(b))

// The "What" are we using for
enum eRenderBufferUsage: unsigned int
{
	VERTEX_BUFFER_BIT = BIT_FLAG(0),
	INDEX_BUFFER_BIT  = BIT_FLAG(1),
	UNIFORM_BUFFER_BIT = BIT_FLAG(2),
};

//typedef unsigned int eRenderBufferUsage;

//How we are going to access it
enum eRenderMemoryHint : unsigned int
{
	MEMORY_HINT_GPU,      // GPU can read/write , cup cant touch it. If we change it, rarely changes from CPU
	MEMORY_HINT_DYNAMIC,  // GPU memory(read/write), that changes OFTEN from the CPU - it allows to 'Map' the memory
	MEMORY_HINT_STAGING,  // CPU read/ write and can copy 
};



class RenderBuffer
{
public:
	RenderBuffer( RenderContext* owner , eRenderBufferUsage usage , eRenderMemoryHint memHint );
	~RenderBuffer();

	bool Update( void const* data , size_t dataByteSize , size_t elementByteSize );
	bool IsCompatible( size_t dataByteSize , size_t elementByteSize ) const;
	void CleanUp();


private:
	bool Create( size_t dataByteSize , size_t elementByteSize );
	


public:
	RenderContext* m_owner = nullptr;
	ID3D11Buffer* m_handle = nullptr;

	eRenderBufferUsage m_usage;
	eRenderMemoryHint m_memHint;

	size_t m_bufferByteSize;
	size_t m_elementByteSize;
};



class VertexBuffer :public RenderBuffer
{
public:
	VertexBuffer( RenderContext* ctx , eRenderMemoryHint hint ) :RenderBuffer( ctx,VERTEX_BUFFER_BIT , hint ) {};
	buffer_attribute_t* m_attribute;
	unsigned int m_stride;
};


class IndexBuffer : public RenderBuffer
{
public:
	IndexBuffer( RenderContext* ctx , eRenderMemoryHint hint ) :RenderBuffer( ctx , INDEX_BUFFER_BIT , hint ) {};

	void Update( unsigned int icount , unsigned	int const* indices );
	void Update( std::vector<unsigned int> const& indices ); // helper, calls one above
};