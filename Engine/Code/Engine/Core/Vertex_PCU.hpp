#pragma once
#include <Engine\Math\Vec3.hpp>
#include <Engine\Core\Rgba8.hpp>
#include <Engine\Math\Vec2.hpp>
#include <vector>

struct AABB2;

struct Vertex_PCU
{
	Vec3 m_position;
    Rgba8 m_color;
	Vec2 m_uvTexCoords;

	Vertex_PCU() = default;
	~Vertex_PCU(){};

	 explicit Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords );
	

};

void AppendAABB2(std::vector<Vertex_PCU>& verts,AABB2& aabb, const Rgba8& tint);
void AppendAABB2(std::vector<Vertex_PCU>& verts, AABB2& aabb, const Rgba8& tint, Vec2 minUVs, Vec2 maxUVs);
