#include"Vertex_PCU.hpp"
#include <vector>
#include "Engine/Core/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Polygon2D.hpp"

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

 
 void AppendDisc2( std::vector<Vertex_PCU>& verts , float radius , const Rgba8& color )
 {
	 Vertex_PCU vertices[ 48 ];

	 vertices[ 0 ] = Vertex_PCU( Vec3( 0.f , 0.f , 0.f ) , color , Vec2( 0.f , 0.f ) );
	 vertices[ 1 ] = Vertex_PCU( Vec3( radius , 0.f , 0.f ) , color , Vec2( 0.f , 0.f ) );

	 float angleDegrees = ( float ) 360 / 16;

	 float initialX = CosDegrees( angleDegrees ) * radius;
	 float initialY = SinDegrees( angleDegrees ) * radius;

	 vertices[ 2 ] = Vertex_PCU( Vec3( initialX , initialY , 0.f ) , color , Vec2( 0.f , 0.f ) );

	 for ( int index = 3; index < 48; index += 3 )
	 {

		 vertices[ index ] = vertices[ index - 3 ];
		 vertices[ index + 1 ] = vertices[ index - 1 ];

		 angleDegrees += 360.f / 16.f;

		 vertices[ index + 2 ] = Vertex_PCU( Vec3( radius * CosDegrees( angleDegrees ) , radius * SinDegrees( angleDegrees ) , 0.f ) , color , Vec2( 0.f , 0.f ) );

	 }

	 vertices[ 47 ] = vertices[ 1 ];

	 for ( int i = 0; i < 48; i++ )
	 {
		 verts.push_back( vertices[ i ] );
	 }
 }

 void AppendPolygon2( std::vector<Vertex_PCU>& verts , const Polygon2D* poly )
 {
	 Rgba8 color = Rgba8( 255 , 255 , 255 , 255 );

	 Vertex_PCU commonVert = Vertex_PCU( Vec3( poly->m_points[ 0 ] , 0.f ) , color , Vec2( 0.f , 1.f ) );

	 int totalTriangles = (int)poly->m_points.size() - 2;
	 int counter = 0;
	 int i = 1;
	 int j = 2;

	 while ( counter<totalTriangles )
	 {
		 Vertex_PCU nextVert1 = Vertex_PCU( Vec3( poly->m_points[ i ] , 0.f ) , color , Vec2( 0.f , 1.f ) );
		 Vertex_PCU nextVert2 = Vertex_PCU( Vec3( poly->m_points[ j ] , 0.f ) , color , Vec2( 0.f , 1.f ) );

		 verts.push_back( commonVert );
		 verts.push_back( nextVert1 );
		 verts.push_back( nextVert2 );

		 i++;
		 j++;
		 counter++;
	 }

 }

 void TransformVertexArray( std::vector<Vertex_PCU>& vertices , float scale , float rotationDegrees , const Vec2& translation )
 {
	 for ( int i = 0; i < vertices.size(); i++ )
	 {
		 vertices[ i ].m_position = TransformPosition3DXY( vertices[ i ].m_position , scale , rotationDegrees , translation );
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
