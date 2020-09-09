#pragma once
#include <Engine\Math\Vec3.hpp>
#include <Engine\Core\Rgba8.hpp>
#include <Engine\Math\Vec2.hpp>
#include "Engine\Core\VertexCommon.hpp"
#include <vector>

struct AABB2;
struct AABB3;
class Polygon2D;


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
void AppendUVSPhere( unsigned int hCuts , unsigned int vCuts , std::vector<Vertex_PCU>& sphereVerts, std::vector<unsigned int>& sphereIndices, float radius , Vec3 center , const Rgba8 color );
void AppendCyinder( std::vector<Vertex_PCU>& cylinderVerts, Vec3 start, Vec3 end, float startRadius, float endRadius, Rgba8 startColor, Rgba8 endColor );
void AppendArrow( std::vector<Vertex_PCU>& arrowVerts , Vec3 start , Vec3 end ,float coneHeight, float lineRadius , float arrowRadius , Rgba8 lineColor , Rgba8 arrowColor );
void AppendPolygon2(std::vector<Vertex_PCU>& verts,const Polygon2D* poly);
void AppendCuboid( std::vector< Vertex_PCU >& cubeMeshVerts , std::vector< unsigned int >& cubeIndices , const AABB3 box , const Rgba8& tint );
void TransformVertexArray( std::vector<Vertex_PCU>& vertices , float scale , float rotationDegrees , const Vec2& translation );
void TransformVertexArray( int numVertices , Vertex_PCU* vertices , float scale , float rotationDegrees , const Vec2& translation );
void AppendCuboidV2(std::vector<Vertex_PCU>& verts,std::vector<unsigned int>& indices, const AABB3 box, const Rgba8& tint);
