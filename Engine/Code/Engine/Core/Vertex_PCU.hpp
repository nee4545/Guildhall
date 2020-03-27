#pragma once
#include <Engine\Math\Vec3.hpp>
#include <Engine\Core\Rgba8.hpp>
#include <Engine\Math\Vec2.hpp>
#include <vector>

struct AABB2;
struct AABB3;
class Polygon2D;


enum eBufferFormatType
{
	// add types as you need them - for now, we can get by with just this
	BUFFER_FORMAT_VEC3 =6 ,
	BUFFER_FORMAT_VEC2 = 16 ,
	BUFFER_FORMAT_R8G8B8A8_UNORM =28 ,
};

struct buffer_attribute_t
{
	std::string name; 			// used to link to a D3D11 shader
	// uint location; 			// used to link to a GL/Vulkan shader 
	eBufferFormatType type;		// what data are we describing
	unsigned int offset; 		// where is it relative to the start of a vertex

	buffer_attribute_t( char const* n , eBufferFormatType t , unsigned int offset );
	buffer_attribute_t();
};

struct Vertex_PCU
{
	Vec3 m_position;
    Rgba8 m_color;
	Vec2 m_uvTexCoords;

	Vertex_PCU() = default;
	~Vertex_PCU(){};

	 explicit Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords );

	 static buffer_attribute_t LAYOUT[];
	

};

void AppendAABB2(std::vector<Vertex_PCU>& verts,AABB2& aabb, const Rgba8& tint);
void AppendAABB3( std::vector<Vertex_PCU>& verts , AABB3& aabb , const Rgba8& tint );
void AppendAABB2(std::vector<Vertex_PCU>& verts, AABB2& aabb, const Rgba8& tint, Vec2 minUVs, Vec2 maxUVs);
void AppendDisc2( std::vector<Vertex_PCU>& verts , float radius , const Rgba8& tint );
void AppendCyinder( std::vector<Vertex_PCU>& cylinderVerts, Vec3 start, Vec3 end, float startRadius, float endRadius, Rgba8 startColor, Rgba8 endColor );
void AppendArrow( std::vector<Vertex_PCU>& arrowVerts , Vec3 start , Vec3 end ,float coneHeight, float lineRadius , float arrowRadius , Rgba8 lineColor , Rgba8 arrowColor );
void AppendPolygon2(std::vector<Vertex_PCU>& verts,const Polygon2D* poly);
void TransformVertexArray( std::vector<Vertex_PCU>& vertices , float scale , float rotationDegrees , const Vec2& translation );