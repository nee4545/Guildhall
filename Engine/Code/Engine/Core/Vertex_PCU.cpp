#include"Vertex_PCU.hpp"
#include <vector>
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Polygon2D.hpp"
#include "Engine/Math/Mat44.hpp"

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

 
 /*void AppendAABB3( std::vector<Vertex_PCU>& verts , AABB3& aabb , const Rgba8& tint )
 {
	 Vertex_PCU vert[ 6 ]
	 {
		 Vertex_PCU( Vec3( aabb.mins.x,aabb.mins.y,aabb.mins.z ),tint,Vec2( 0.f,0.f ) ),
		 Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,aabb.mins.z ),tint,Vec2( 1.f,0.f ) ),
		 Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,aabb.maxs.z ),tint,Vec2( 0.f,1.f ) ),

		 Vertex_PCU( Vec3( aabb.maxs.x,aabb.mins.y,aabb.mins.z ),tint,Vec2( 1.f,0.f ) ),
		 Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,aabb.maxs.z ),tint,Vec2( 1.f,1.f ) ),
		 Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,aabb.maxs.z ),tint,Vec2( 0.f,1.f ) )

	 };

	 for ( int index = 0; index < 6; index++ )
	 {
		 verts.push_back( vert[ index ] );
	 }
 }*/

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

 void AppendUVSPhere( unsigned int hCuts , unsigned int vCuts , std::vector<Vertex_PCU>& sphereVerts ,std::vector<unsigned int>& sphereIndices, float radius , Vec3 center , const Rgba8 color )
 {

	 float deltaTheta = 360.f / ( float ) hCuts;
	 float deltaPhi = 180.f / ( float ) vCuts;
	 for ( float phi = -90.f; phi <= 90.0; phi += deltaPhi )
	 {
		 for ( float theta = -360.f; theta <= 0.0; theta += deltaTheta )
		 {
			 Vec3 currentCoordsNormal = GetSphericalCoordinates( theta , phi , radius );
			 float u = RangeMapFloat( -360.f , 0.f , 0.f , 1.f , theta );
			 float v = RangeMapFloat( -90.f , 90.f , 0.f , 1.f , phi );

			 Vertex_PCU currentCoordVerts = Vertex_PCU( center + currentCoordsNormal , color , Vec2( u , v ) );
			 sphereVerts.push_back( currentCoordVerts );
		 }
	 }

	 for ( unsigned int hcutIndex = 0; hcutIndex < hCuts; hcutIndex++ )
	 {
		 for ( unsigned int vCutIndex = 0; vCutIndex < vCuts; vCutIndex++ )
		 {
			 int index0 = hcutIndex + ( ( hCuts + 1 ) * vCutIndex );
			 int index1 = index0 + 1;
			 int index2 = index0 + hCuts + 1;
			 int index3 = index2 + 1;
			 sphereIndices.push_back( index0 );
			 sphereIndices.push_back( index1 );
			 sphereIndices.push_back( index3 );
			 sphereIndices.push_back( index0 );
			 sphereIndices.push_back( index3 );
			 sphereIndices.push_back( index2 );
		 }
	 }
 }

	 /*float deltaTheta = 360.f / ( float ) hCuts;
	 float deltaPhi = 180.f / ( float ) vCuts;

	 for ( float i = -90.f; i <= 90.f; i += deltaPhi )
	 {
		 for ( float j = -360.f; j <= 0.0; j += deltaTheta )
		 {
			 float cosTheta = CosDegrees( j );
			 float sinTheta = SinDegrees( j );
			 float cosPhi = CosDegrees( i );
			 float sinPhi = SinDegrees( i );
			 Vec3 normal;
			 normal.x = cosPhi * cosTheta;
			 normal.y = sinPhi;
			 normal.z = -cosPhi * sinTheta;
			 normal = normal.GetNormalized();
			 Vec4 tangent;
			 tangent.x = cosPhi * -sinTheta;
			 tangent.y = 0.f;
			 tangent.z = cosPhi * -cosTheta;
			 tangent.w = 1.f;

			 float u = RangeMapFloat( -360.f , 0.f , 0.f , 1.f , j );
			 float v = RangeMapFloat( -90.f , 90.f , 0.f , 1.f , i );

			 Vertex_PCU verts = Vertex_PCU(
				 center + ( normal * radius ) , color ,
				 Vec2( 1.f , 1.f ) - Vec2( u , v ) );

			 sphereVerts.push_back( verts );
		 }
	 }*/
 

 void AppendCyinder( std::vector<Vertex_PCU>& cylinderVerts , Vec3 start , Vec3 end , float startRadius , float endRadius , Rgba8 startColor , Rgba8 endColor )
 {
	 Mat44 transformation = Mat44::LookAt( start , end );
	 Vec3 ibasis = transformation.GetIBasis3D();
	 Vec3 jbasis = transformation.GetJBasis3D();
	 const Vec3& i = ibasis;
	 const Vec3& j = jbasis;
	 const Vec3& s = start;
	 const Vec3& e = end;

	 //Start Disc

	 const int  numDiscVerts = 120;
	 Vertex_PCU startDiscVerts[ numDiscVerts ];

	 float angleDegrees = 0.f;
	 Vec3 secondVertStartDisc = start + ( i * startRadius ); 
	 startDiscVerts[ 0 ] = Vertex_PCU( start , startColor , Vec2( 0.f , 0.f ) );
	 startDiscVerts[ 1 ] = Vertex_PCU( ( secondVertStartDisc ) , startColor , Vec2( 0.f , 0.f ) );
	 angleDegrees = ( 360.f * 3.f ) / (float)( numDiscVerts );

	 float costheta = CosDegrees( angleDegrees );
	 Vec3 iComponent = i * costheta * startRadius;
	 float sintheta = SinDegrees( angleDegrees );
	 Vec3 jComponent = j * sintheta * startRadius;
	 startDiscVerts[ 2 ] = Vertex_PCU( ( s + iComponent + jComponent ) , startColor , Vec2(0.f,0.f) );
	
	 int discVertIndex = 3;
	 for ( discVertIndex = 3; discVertIndex < numDiscVerts; discVertIndex += 3 )
	 {
		 angleDegrees = angleDegrees + ( ( 360.f * 3.f ) / ( numDiscVerts ) );
		 startDiscVerts[ discVertIndex ] = startDiscVerts[ discVertIndex - 3 ];
		 startDiscVerts[ discVertIndex + 1 ] = startDiscVerts[ discVertIndex - 1 ];
		 costheta = CosDegrees( angleDegrees );
		 sintheta = SinDegrees( angleDegrees );
		 iComponent = i * costheta * startRadius;
		 jComponent = j * sintheta * startRadius;
		 startDiscVerts[ discVertIndex + 2 ].m_position = s + iComponent + jComponent;
		 startDiscVerts[ discVertIndex + 2 ].m_color = startColor;
		 startDiscVerts[ discVertIndex + 2 ].m_uvTexCoords = Vec2(0.f,0.f);
	 }
	 startDiscVerts[ numDiscVerts - 1 ] = startDiscVerts[ 1 ];
	 for ( int index = 0; index < numDiscVerts; index++ )
	 {
		 cylinderVerts.push_back( startDiscVerts[ index ] );
	 }
		
	 //End Disc
	 Vertex_PCU endDiscVerts[ numDiscVerts ];
	 Vec3 secondVertEndDisc = end + ( i * endRadius ); 
	 angleDegrees = ( 360.f * 3.f ) /  (float)( numDiscVerts );
	 costheta = CosDegrees( angleDegrees );
	 iComponent = i * costheta * endRadius;
	 sintheta = SinDegrees( angleDegrees );
	 jComponent = j * sintheta * endRadius;
	 endDiscVerts[ 0 ] = Vertex_PCU( end , endColor , Vec2( 0.f , 0.f ) );
	 endDiscVerts[ 1 ] = Vertex_PCU( ( secondVertEndDisc ) , endColor , Vec2( 1.f , 0.f ) );
	 endDiscVerts[ 2 ] = Vertex_PCU( ( end + iComponent + jComponent ) , endColor , Vec2(0.f,0.f) );
	
	 discVertIndex = 3;
	 for ( discVertIndex = 3; discVertIndex < numDiscVerts; discVertIndex += 3 )
	 {
		 angleDegrees = angleDegrees + ( ( 360.f * 3.f ) / ( numDiscVerts ) );
		 endDiscVerts[ discVertIndex ] = endDiscVerts[ discVertIndex - 3 ];
		 endDiscVerts[ discVertIndex + 1 ] = endDiscVerts[ discVertIndex - 1 ];
		 costheta = CosDegrees( angleDegrees );
		 sintheta = SinDegrees( angleDegrees );
		 iComponent = i * costheta * endRadius;
		 jComponent = j * sintheta * endRadius;
		 endDiscVerts[ discVertIndex + 2 ].m_position = e + iComponent + jComponent;
		 endDiscVerts[ discVertIndex + 2 ].m_color = endColor;
		 endDiscVerts[ discVertIndex + 2 ].m_uvTexCoords = Vec2(0.f,0.f);
	 }
	 endDiscVerts[ numDiscVerts - 1 ] = endDiscVerts[ 1 ];

	 for ( int index = 0; index < numDiscVerts; index++ )
	 {
		 cylinderVerts.push_back( endDiscVerts[ index ] );
	 }
	 
	// Box

	 std::vector<Vertex_PCU> boxVerts;



	 for ( int index = 1; index < numDiscVerts; index +=3 )
	 {
		 boxVerts.push_back( startDiscVerts[ index ] );
		 boxVerts.push_back( startDiscVerts[ index + 1 ] );
		 boxVerts.push_back( endDiscVerts[ index + 1 ] );

		 boxVerts.push_back( endDiscVerts[ index+1] );
		 boxVerts.push_back( endDiscVerts[ index] );
		 boxVerts.push_back( startDiscVerts[ index ] );
	 }

	 for ( int index = 0; index < boxVerts.size(); index++ )
	 {
		 cylinderVerts.push_back( boxVerts[ index ] );
	 }
 }

 void AppendArrow( std::vector<Vertex_PCU>& arrowVerts , Vec3 start , Vec3 end ,float coneHeight, float lineRadius , float arrowRadius , Rgba8 lineColor , Rgba8 arrowColor )
 {
	 AppendCyinder( arrowVerts , start , end , lineRadius , lineRadius , lineColor , arrowColor );

	 Mat44 transformation = Mat44::LookAt( start , end );
	 Vec3 ibasis = transformation.GetIBasis3D();
	 Vec3 jbasis = transformation.GetJBasis3D();
	 const Vec3& i = ibasis;
	 const Vec3& j = jbasis;
	 const Vec3& e = end;

	 const int  numDiscVerts = 120;
	 Vertex_PCU startDiscVerts[ numDiscVerts ];

	 float angleDegrees = 0.f;
	 Vec3 secondVertStartDisc = end + ( i * arrowRadius );
	 startDiscVerts[ 0 ] = Vertex_PCU( end , arrowColor , Vec2( 0.f , 0.f ) );
	 startDiscVerts[ 1 ] = Vertex_PCU( ( secondVertStartDisc ) , arrowColor , Vec2( 1.f , 0.f ) );
	 angleDegrees = ( 360.f * 3.f ) / ( float ) ( numDiscVerts );

	 float costheta = CosDegrees( angleDegrees );
	 Vec3 iComponent = i * costheta * arrowRadius;
	 float sintheta = SinDegrees( angleDegrees );
	 Vec3 jComponent = j * sintheta * arrowRadius;
	 startDiscVerts[ 2 ] = Vertex_PCU( ( e + iComponent + jComponent ) , arrowColor , Vec2( 0.f , 0.f ) );

	 int discVertIndex = 3;
	 for ( discVertIndex = 3; discVertIndex < numDiscVerts; discVertIndex += 3 )
	 {
		 angleDegrees = angleDegrees + ( ( 360.f * 3.f ) / ( numDiscVerts ) );
		 startDiscVerts[ discVertIndex ] = startDiscVerts[ discVertIndex - 3 ];
		 startDiscVerts[ discVertIndex + 1 ] = startDiscVerts[ discVertIndex - 1 ];
		 costheta = CosDegrees( angleDegrees );
		 sintheta = SinDegrees( angleDegrees );
		 iComponent = i * costheta * arrowRadius;
		 jComponent = j * sintheta * arrowRadius;
		 startDiscVerts[ discVertIndex + 2 ].m_position = e + iComponent + jComponent;
		 startDiscVerts[ discVertIndex + 2 ].m_color = arrowColor;
		 startDiscVerts[ discVertIndex + 2 ].m_uvTexCoords = Vec2( 0.f , 0.f );
	 }
	 startDiscVerts[ numDiscVerts - 1 ] = startDiscVerts[ 1 ];
	 for ( int index = 0; index < numDiscVerts; index++ )
	 {
		 arrowVerts.push_back( startDiscVerts[ index ] );
	 }

	 Vec3 arrowDir = ( end - start ).GetNormalized();
	 Vec3 arrowEnd = end + arrowDir * coneHeight;

	 Vertex_PCU arrowEndVertex = Vertex_PCU( arrowEnd , arrowColor , Vec2( 0.f , 0.f ) );


	 // Cone

	 std::vector<Vertex_PCU> boxVerts;
	 for ( int index = 1; index < numDiscVerts - 1; index++ )
	 {
		 boxVerts.push_back( startDiscVerts[ index ] );
		 boxVerts.push_back( startDiscVerts[ index + 1 ] );
		 boxVerts.push_back( arrowEndVertex );
	 }

	 for ( int index = 0; index < boxVerts.size(); index++ )
	 {
		 arrowVerts.push_back( boxVerts[ index ] );
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

 void TransformVertexArray( int numVertices , Vertex_PCU* vertices , float scale , float rotationDegrees , const Vec2& translation )
 {
	 for ( int index = 0; index < numVertices; index++ )
	 {
		 vertices[ index ].m_position = TransformPosition3DXY( vertices[ index ].m_position , scale , rotationDegrees , translation );
	 }
 }

 void AppendCuboidV2( std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices , const AABB3 box , const Rgba8& tint )
 {
	 Vertex_PCU cuboidVerts[ 24 ] = {
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, Vec2(0.f,0.f) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, Vec2(1.f,0.f ) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, Vec2(1.f,1.f) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, Vec2(0.f,1.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, Vec2(0.f,0.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, Vec2(1.f,0.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, Vec2(1.f,1.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, Vec2(0.f,1.f) ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, Vec2(0.f,0.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, Vec2(1.f,0.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, Vec2(1.f,1.f) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, Vec2(0.f,1.f) ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, Vec2(0.f,0.f) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, Vec2(1.f,0.f) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, Vec2(1.f,1.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, Vec2(0.f,1.f) ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, Vec2(0.f,0.f) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, Vec2(1.f,0.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, Vec2(1.f,1.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, Vec2(0.f,1.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, Vec2(0.f,0.f) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, Vec2(1.f,0.f) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, Vec2(1.f,1.f) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, Vec2(0.f,1.f) ),
	};


	 for ( int index = 0; index < 24; index++ )
	 {
		 verts.push_back( cuboidVerts[ index ] );
	 }

	 unsigned int cuboidIndices[ 36 ] = {
			0,1,2,
			2,3,0,
			4,5,6,
			6,7,4,
			8,9,10,
			10,11,8,
			12,13,14,
			14,15,12,
			16,17,18,
			18,19,16,
			20,21,22,
			22,23,20,
	 };

	 for ( int i = 0; i < 36; i++ )
	 {
		 indices.push_back( cuboidIndices[ i ] );
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
	 type = BUFFER_FORMAT_INVALID;
	 offset = 0;
 }


 void AppendCuboid( std::vector< Vertex_PCU >& cubeMeshVerts , std::vector< unsigned int >& cubeIndices , const AABB3 box , const Rgba8& tint )
 {
	 Vertex_PCU CubeVerts[ 24 ] = {
		 
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z,box.m_maxs.y ) , tint, Vec2( 0.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_maxs.y ) , tint, Vec2( 1.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_maxs.y ) , tint, Vec2( 1.f, 1.f ) ),
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z,box.m_maxs.y ) , tint, Vec2( 1.f, 0.f ) ),
						 
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z,box.m_mins.y ) , tint, Vec2( 0.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_mins.y ) , tint, Vec2( 1.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_mins.y )  , tint, Vec2( 1.f, 1.f ) ),
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z,box.m_mins.y ) , tint, Vec2( 1.f, 0.f ) ),
										 
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_maxs.y ) ,tint, Vec2( 0.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_mins.y ) ,tint, Vec2( 1.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_mins.y ) , tint, Vec2( 1.f, 1.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_maxs.y ) ,tint, Vec2( 1.f, 0.f ) ),
														
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z,box.m_maxs.y ) ,tint, Vec2( 0.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z,box.m_mins.y ) ,tint, Vec2( 1.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z,box.m_mins.y ) , tint, Vec2( 1.f, 1.f ) ),
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z,box.m_maxs.y ) ,tint, Vec2( 1.f, 0.f ) ),
																		 
	 Vertex_PCU( Vec3( box.m_mins.x, box.m_maxs.z, box.m_maxs.y ) ,tint, Vec2( 0.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x, box.m_maxs.z, box.m_maxs.y ) ,tint, Vec2( 1.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_mins.y ) , tint, Vec2( 1.f, 1.f ) ),
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z, box.m_mins.y ) ,tint, Vec2( 1.f, 0.f ) ),
																						 
	 Vertex_PCU( Vec3( box.m_mins.x, box.m_mins.z, box.m_maxs.y ) ,tint, Vec2( 0.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x, box.m_mins.z, box.m_maxs.y ) ,tint, Vec2( 1.f, 0.f ) ),
	 Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_mins.y ) , tint, Vec2( 1.f, 1.f ) ),
	 Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z, box.m_mins.y ) ,tint, Vec2( 1.f, 0.f ) ),
	 };

	 for ( int index = 0; index < 24; index++ )
	 {
		 cubeMeshVerts.push_back( CubeVerts[ index ] );
	 }

	 int CubeIndices[ 36 ] = 
	 {
		0,1,2,
		2,3,0,
		4,5,6,
		6,7,4,
		8,9,10,
		10,11,8,
		12,13,14,
		14,15,12,
		16,17,18,
		18,19,16,
		20,21,22,
		22,23,20,
	 };

	 for ( int index = 0; index < 36; index++ )
	 {
		 cubeIndices.push_back( CubeIndices[ index ] );
	 }
 }
