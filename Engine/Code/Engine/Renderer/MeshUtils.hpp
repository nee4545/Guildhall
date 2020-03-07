#pragma once
#include <vector>
struct Vertex_PCU;
struct Rgba8;
struct Vec3;

void AddUVSphereToIndexedVertexArray( std::vector<Vertex_PCU>& verts , std::vector<unsigned int>& indices , Vec3& center , float radius , unsigned int horizintalCuts , unsigned int verticalCuts , Rgba8& color );