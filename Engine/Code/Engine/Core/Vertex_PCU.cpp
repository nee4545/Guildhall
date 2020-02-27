#include"Vertex_PCU.hpp"
#include <vector>
#include "Engine/Core/AABB2.hpp"

Vertex_PCU::Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords )
{
m_position=position;
m_color=tint;
m_uvTexCoords=uvTexCoords;
}


	 buffer_attribute_t  Vertex_PCU::LAYOUT[] = 
	{
		buffer_attribute_t( "POSITION",  BUFFER_FORMAT_VEC3,      		offsetof( Vertex_PCU, m_position ) ),
		buffer_attribute_t( "COLOR",     BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof( Vertex_PCU, m_color ) ),
		buffer_attribute_t( "TEXCOORD",        BUFFER_FORMAT_VEC2,      		offsetof( Vertex_PCU, m_uvTexCoords ) ),
		buffer_attribute_t() // end - terminator element; 
	};

 

 void AppendAABB2( std::vector<Vertex_PCU>& verts, AABB2& aabb, const Rgba8& tint )
 {
	 Vertex_PCU vert[6]
	 {
		 Vertex_PCU( Vec3( aabb.mins.x,aabb.mins.y,0.f ),tint,Vec2( 0.f,0.f ) ),
		 Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,0.f ),tint,Vec2( 1.f,0.f ) ),
		 Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),tint,Vec2( 0.f,1.f ) ),
														 
		 Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,0.f ),tint,Vec2( 1.f,0.f ) ),
		 Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),tint,Vec2( 1.f,1.f ) ),
		 Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),tint,Vec2( 0.f,1.f ) )

	 };

	 for( int index=0; index<6; index++ )
	 {
		 verts.push_back(vert[index]);
	 }
 }

 void AppendAABB2( std::vector<Vertex_PCU>& verts, AABB2& aabb, const Rgba8& tint, Vec2 minUVs, Vec2 maxUVs )
 {
	 Vertex_PCU vert[6]
	 {
		 Vertex_PCU( Vec3( aabb.mins.x,aabb.mins.y,0.f ),tint, minUVs ),
		 Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,0.f ),tint,Vec2( maxUVs.x, minUVs.y ) ),
		 Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),tint,Vec2( minUVs.x, maxUVs.y ) ),

		 Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,0.f ),tint,Vec2( maxUVs.x, minUVs.y ) ),
		 Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),tint, maxUVs ),
		 Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),tint,Vec2( minUVs.x, maxUVs.y ) )

	 };

	 for( int index=0; index<6; index++ )
	 {
		 verts.push_back( vert[index] );
	 }
 }

 
 buffer_attribute_t::buffer_attribute_t( char const* n , eBufferFormatType t , unsigned int offset )
 {
	 name = n;
	 type = t;
	 this->offset = offset;
 }

 buffer_attribute_t::buffer_attribute_t()
 {
	 name = "";
	 type = BUFFER_FORMAT_VEC2;
	 offset = 0;
 }
