#include"Vertex_PCU.hpp"
#include <vector>
#include "Engine/Core/AABB2.hpp"

Vertex_PCU::Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords )
{
m_position=position;
m_color=tint;
m_uvTexCoords=uvTexCoords;
}


 

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

 
