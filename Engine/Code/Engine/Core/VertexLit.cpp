#include "Engine/Core/VertexLit.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

VertexLit::VertexLit( Vec3 position , Rgba8 color , Vec2 uv , Vec3 normal /*= Vec3( 1.f , 0.f , 0.f ) */ )
{
	m_position = position;
	m_color = color.GetAsNormalizedVec4();
	m_normal = normal;
	m_uv = uv;
}

VertexLit::VertexLit( Vec3 position , Vec4 color , Vec2 uv , Vec3 normal /*= Vec3( 1.f , 0.f , 0.f ) */ )
{
	m_position = position;
	m_color = color;
	m_uv = uv;
	m_normal = normal;
}

//  [4/12/2020 Neeraj Jairam]

VertexLit::VertexLit( Vec3 position , Vec4 color , Vec2 uv , Vec3 normal /*= Vec3( 1.f , 0.f , 0.f ) */ , Vec4 tangent /*= Vec4( 1.f , 0.f , 0.f , 1.f ) */ )
{
	m_position = position;
	m_color = color;
	m_uv = uv;
	m_normal = normal;
	m_tangent = tangent;
}

buffer_attribute_t  VertexLit::LAYOUT[] =
{
	buffer_attribute_t( "POSITION",  BUFFER_FORMAT_VEC3,      		offsetof( VertexLit, m_position ) ),
	buffer_attribute_t( "COLOR",     BUFFER_FORMAT_VEC4, 			offsetof( VertexLit, m_color ) ),
	buffer_attribute_t( "TEXCOORD",  BUFFER_FORMAT_VEC2,      		offsetof( VertexLit, m_uv ) ),
	buffer_attribute_t( "NORMAL",	 BUFFER_FORMAT_VEC3,			offsetof( VertexLit,m_normal ) ),
	buffer_attribute_t( "TANGENT",	 BUFFER_FORMAT_VEC4,			offsetof( VertexLit,m_tangent ) ),
	buffer_attribute_t()// end - terminator element; 
};



void AppendIndexedQuad( std::vector<VertexLit> &vertices , std::vector<unsigned int> &indices , AABB2& quad, Rgba8 color )
{
	VertexLit v1 = VertexLit( Vec3( quad.mins , 0.f ) , color , Vec2( 0.f , 0.f ) );
	VertexLit v2 = VertexLit( Vec3( Vec2(quad.maxs.x,quad.mins.y) , 0.f ) , color , Vec2( 1.f , 0.f ) );
	VertexLit v3 = VertexLit( Vec3( quad.maxs , 0.f ) , color , Vec2( 1.f , 1.f ) );
	VertexLit v4 = VertexLit( Vec3( Vec2(quad.mins.x,quad.maxs.y) , 0.f ) , color , Vec2( 0.f , 1.f ) );

	Vec3 quadNormal = CrossProduct3D( v2.m_position - v1.m_position , v4.m_position - v1.m_position );
	quadNormal = quadNormal.GetNormalized();

	Vec3 tangent = ( v2.m_position - v1.m_position ).GetNormalized();
	Vec4 quadTangent = Vec4( tangent.x , tangent.y , tangent.z , 1.f );

	v1.m_normal = quadNormal;
	v2.m_normal = quadNormal;
	v3.m_normal = quadNormal;
	v4.m_normal = quadNormal;

	v1.m_tangent = quadTangent;
	v2.m_tangent = quadTangent;
	v3.m_tangent = quadTangent;
	v4.m_tangent = quadTangent;
	
	vertices.push_back( v1 );
	vertices.push_back( v2 );
	vertices.push_back( v3 );
	vertices.push_back( v4 );

	indices.push_back( 0 );
	indices.push_back( 1 );
	indices.push_back( 2 );
	indices.push_back( 2 );
	indices.push_back( 3 );
	indices.push_back( 0 );

}

void AppendIndexedCuboid( std::vector<VertexLit>& vertices , std::vector<unsigned int>& indices , Vec3 mins , Vec3 maxs , Rgba8 color )
{

	Vec3 quadNormal;

	//Back Face
	VertexLit v1 = VertexLit( Vec3( mins.x , mins.y , mins.z ) , color , Vec2( 0.f , 0.f ) );
	VertexLit v2 = VertexLit( Vec3( maxs.x , mins.y , mins.z ) , color , Vec2( 1.f , 0.f ) );
	VertexLit v3 = VertexLit( Vec3( maxs.x , maxs.y , mins.z ) , color , Vec2( 1.f , 1.f ) );
	VertexLit v4 = VertexLit( Vec3( mins.x , maxs.y , mins.z ) , color , Vec2( 0.f , 1.f ) );

	quadNormal = CrossProduct3D( v2.m_position - v1.m_position , v4.m_position - v1.m_position );
	quadNormal = quadNormal.GetNormalized();
	quadNormal = -quadNormal;

	Vec3 tangent = ( v2.m_position - v1.m_position ).GetNormalized();
	Vec4 quadTangent = Vec4( tangent.x , tangent.y , tangent.z , 1.f );

	v1.m_normal = quadNormal;
	v2.m_normal = quadNormal;
	v3.m_normal = quadNormal;
	v4.m_normal = quadNormal;

	v1.m_tangent = -quadTangent;
	v2.m_tangent = -quadTangent;
	v3.m_tangent = -quadTangent;
	v4.m_tangent = -quadTangent;

	vertices.push_back( v1 );
	vertices.push_back( v2 );
	vertices.push_back( v3 );
	vertices.push_back( v4 );

	indices.push_back( 0 );
	indices.push_back( 1 );
	indices.push_back( 2 );
	indices.push_back( 2 );
	indices.push_back( 3 );
	indices.push_back( 0 );

	//Front Face
	VertexLit v5 = VertexLit( Vec3( mins.x , mins.y , maxs.z ) , color , Vec2( 0.f , 0.f ) );
	VertexLit v6 = VertexLit( Vec3( maxs.x , mins.y , maxs.z ) , color , Vec2( 1.f , 0.f ) );
	VertexLit v7 = VertexLit( Vec3( maxs.x , maxs.y , maxs.z ) , color , Vec2( 1.f , 1.f ) );
	VertexLit v8 = VertexLit( Vec3( mins.x , maxs.y , maxs.z ) , color , Vec2( 0.f , 1.f ) );

	quadNormal = -quadNormal;

	v5.m_normal = quadNormal;
	v6.m_normal = quadNormal;
	v7.m_normal = quadNormal;
	v8.m_normal = quadNormal;

	v5.m_tangent = quadTangent;
	v6.m_tangent = quadTangent;
	v7.m_tangent = quadTangent;
	v8.m_tangent = quadTangent;

	vertices.push_back( v5 );
	vertices.push_back( v6 );
	vertices.push_back( v7 );
	vertices.push_back( v8 );

	indices.push_back( 4 );
	indices.push_back( 5 );
	indices.push_back( 6 );
	indices.push_back( 6 );
	indices.push_back( 7 );
	indices.push_back( 4 );

	//Left Face
	VertexLit v9 = VertexLit ( Vec3( mins.x , mins.y , maxs.z ) , color , Vec2( 0.f , 0.f ) );
	VertexLit v10 = VertexLit( Vec3( mins.x , mins.y , mins.z ) , color , Vec2( 1.f , 0.f ) );
	VertexLit v11 = VertexLit( Vec3( mins.x , maxs.y , mins.z ) , color , Vec2( 1.f , 1.f ) );
	VertexLit v12 = VertexLit( Vec3( mins.x , maxs.y , maxs.z ) , color , Vec2( 0.f , 1.f ) );								

	quadNormal = CrossProduct3D( v10.m_position - v9.m_position , v12.m_position - v9.m_position );
	quadNormal = quadNormal.GetNormalized();

	tangent = (v10.m_position - v9.m_position).GetNormalized();
	quadTangent = Vec4( tangent.x , tangent.y , tangent.z , 1.f );

	v9.m_normal = -quadNormal;
	v10.m_normal = -quadNormal;
	v11.m_normal = -quadNormal;
	v12.m_normal = -quadNormal;

	v9.m_tangent = -quadTangent;
	v10.m_tangent = -quadTangent;
	v11.m_tangent = -quadTangent;
	v12.m_tangent = -quadTangent;

	vertices.push_back( v9 );
	vertices.push_back( v10 );
	vertices.push_back( v11 );
	vertices.push_back( v12 );

	indices.push_back( 8 );
	indices.push_back( 9 );
	indices.push_back( 10 );
	indices.push_back( 10 );
	indices.push_back( 11 );
	indices.push_back( 8 );

	//right face
	VertexLit v13 = VertexLit( Vec3( maxs.x , mins.y , mins.z ) , color , Vec2( 0.f , 0.f ) );
	VertexLit v14 = VertexLit( Vec3( maxs.x , mins.y , maxs.z ) , color , Vec2( 1.f , 0.f ) );
	VertexLit v15 = VertexLit( Vec3( maxs.x , maxs.y , maxs.z ) , color , Vec2( 1.f , 1.f ) );
	VertexLit v16 = VertexLit( Vec3( maxs.x , maxs.y , mins.z ) , color , Vec2( 0.f , 1.f ) );

	quadNormal = -quadNormal;

	v13.m_normal = -quadNormal;
	v14.m_normal = -quadNormal;
	v15.m_normal = -quadNormal;
	v16.m_normal = -quadNormal;

	v13.m_tangent = quadTangent;
	v14.m_tangent = quadTangent;
	v15.m_tangent = quadTangent;
	v16.m_tangent = quadTangent;

	vertices.push_back( v13 );
	vertices.push_back( v14 );
	vertices.push_back( v15 );
	vertices.push_back( v16 );

	indices.push_back( 12 );
	indices.push_back( 13 );
	indices.push_back( 14 );
	indices.push_back( 14 );
	indices.push_back( 15 );
	indices.push_back( 12 );

	//top face
	VertexLit v17 = VertexLit( Vec3( mins.x , maxs.y , maxs.z ) , color , Vec2( 0.f , 0.f ) );
	VertexLit v18 = VertexLit( Vec3( maxs.x , maxs.y , maxs.z ) , color , Vec2( 1.f , 0.f ) );
	VertexLit v19 = VertexLit( Vec3( maxs.x , maxs.y , mins.z ) , color , Vec2( 1.f , 1.f ) );
	VertexLit v20 = VertexLit( Vec3( mins.x , maxs.y , mins.z ) , color , Vec2( 0.f , 1.f ) );

	quadNormal = CrossProduct3D( v18.m_position - v17.m_position , v20.m_position - v17.m_position );
	quadNormal = quadNormal.GetNormalized();

	tangent = (v18.m_position - v17.m_position).GetNormalized();
	quadTangent = Vec4( tangent.x , tangent.y , tangent.z , 1.f );

	v17.m_normal = quadNormal;
	v18.m_normal = quadNormal;
	v19.m_normal = quadNormal;
	v20.m_normal = quadNormal;

	v17.m_tangent = quadTangent;
	v18.m_tangent = quadTangent;
	v19.m_tangent = quadTangent;
	v20.m_tangent = quadTangent;

	vertices.push_back( v17 );
	vertices.push_back( v18 );
	vertices.push_back( v19 );
	vertices.push_back( v20 );

	indices.push_back( 16 );
	indices.push_back( 17 );
	indices.push_back( 18 );
	indices.push_back( 18 );
	indices.push_back( 19 );
	indices.push_back( 16 );

	// bottom face

	VertexLit v21 = VertexLit( Vec3( mins.x , mins.y , maxs.z ) , color , Vec2( 0.f , 0.f ) );
	VertexLit v22 = VertexLit( Vec3( maxs.x , mins.y , maxs.z ) , color , Vec2( 1.f , 0.f ) );
	VertexLit v23 = VertexLit( Vec3( maxs.x , mins.y , mins.z ) , color , Vec2( 1.f , 1.f ) );
	VertexLit v24 = VertexLit( Vec3( mins.x , mins.y , mins.z ) , color , Vec2( 0.f , 1.f ) );

	quadNormal = -quadNormal;

	v21.m_normal = quadNormal;
	v22.m_normal = quadNormal;
	v23.m_normal = quadNormal;
	v24.m_normal = quadNormal;

	v21.m_tangent = quadTangent;
	v22.m_tangent = quadTangent;
	v23.m_tangent = quadTangent;
	v24.m_tangent = quadTangent;

	vertices.push_back( v21 );
	vertices.push_back( v22 );
	vertices.push_back( v23 );
	vertices.push_back( v24 );

	indices.push_back( 20 );
	indices.push_back( 21 );
	indices.push_back( 22 );
	indices.push_back( 22 );
	indices.push_back( 23 );
	indices.push_back( 20 );

}

void AppendIndexedUVSPhere( unsigned int hCuts , unsigned int vCuts , std::vector<VertexLit>& sphereVerts , std::vector<unsigned int>& sphereIndices , float radius , Vec3 center , const Rgba8 color )
{
	float deltaTheta = 360.f / ( float ) hCuts;
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

			VertexLit verts = VertexLit(
				 center + ( normal * radius)  , color ,
					Vec2(1.f,1.f) - Vec2( u , v ) );

				verts.m_normal = normal.GetNormalized();
				verts.m_tangent = -tangent;

			sphereVerts.push_back( verts );
		}
	}

	for ( unsigned int hcutIndex = 0; hcutIndex < hCuts; hcutIndex++ )
	{
		for ( unsigned int vCutIndex = 0; vCutIndex < vCuts; vCutIndex++ )
		{
			unsigned int index0 = hcutIndex + ( ( hCuts + 1 ) * vCutIndex );
			unsigned int index1 = index0 + 1;
			unsigned int index2 = index0 + hCuts + 1;
			unsigned int index3 = index2 + 1;
			sphereIndices.push_back( index0 );
			sphereIndices.push_back( index1 );
			sphereIndices.push_back( index3 );
			sphereIndices.push_back( index0 );
			sphereIndices.push_back( index3 );
			sphereIndices.push_back( index2 );
		}
	}
}

