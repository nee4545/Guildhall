#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/VertexCommon.hpp"

struct AABB2;

struct VertexLit
{
public:
	VertexLit() {};
	explicit VertexLit( Vec3 position , Rgba8 color , Vec2 uv , Vec3 normal = Vec3( 1.f , 0.f , 0.f ) );
	explicit VertexLit( Vec3 position , Vec4 color , Vec2 uv , Vec3 normal = Vec3( 1.f , 0.f , 0.f ) );
	explicit VertexLit( Vec3 position , Vec4 color , Vec2 uv , Vec3 normal = Vec3( 1.f , 0.f , 0.f ) , Vec4 tangent = Vec4( 1.f , 0.f , 0.f , 1.f ) );

public:
	Vec3 m_position;
	Vec4 m_color;
	Vec2 m_uv;

	//Vec3 m_bitangent;
	Vec3 m_normal;
	Vec4 m_tangent;
	//Vec2 padding;

	static buffer_attribute_t LAYOUT[];
};


void AppendIndexedQuad( std::vector<VertexLit> &vertices , std::vector<unsigned int> &indices, AABB2& quad, Rgba8 color );
void AppendIndexedCuboid( std::vector<VertexLit>& vertices , std::vector<unsigned int>& indices , Vec3 mins , Vec3 maxs , Rgba8 color );
void AppendIndexedUVSPhere( unsigned int hCuts , unsigned int vCuts , std::vector<VertexLit>& sphereVerts , std::vector<unsigned int>& sphereIndices , float radius , Vec3 center , const Rgba8 color );