#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/MathUtils.hpp"


void AddUVSphereToIndexedVertexArray( std::vector<Vertex_PCU>& verts , std::vector<unsigned int>& indices , Vec3& center , float radius , unsigned int horizintalCuts , unsigned int verticalCuts , Rgba8& color )
{
	float deltaTheta = 360.f / ( float ) horizintalCuts;
	float deltaPhi = 180.f / ( float ) verticalCuts;

	for ( float theta = 0.f; theta <= 360.0; theta += deltaTheta )
	{
		for ( float phi = -90.f; phi <= 90.0; phi += deltaPhi )
		{
			Vec3 currentCoords = GetSphericalCoordinates( theta , phi , radius );
			float u = RangeMapFloat( 0.f , 360.f , 0.f , 1.f , theta );
			float v = RangeMapFloat( -90.f , 90.f , 0.f , 1.f , phi );
			Vertex_PCU currentCoordVerts = Vertex_PCU( center + currentCoords , color , Vec2( u , v ) );
			verts.push_back( currentCoordVerts );
		}
	}
	for ( unsigned int hcutIndex = 0; hcutIndex < horizintalCuts; hcutIndex++ )
	{
		for ( unsigned int vCutIndex = 0; vCutIndex < verticalCuts; vCutIndex++ )
		{
			unsigned int index0 = hcutIndex + ( ( horizintalCuts + 1 ) * vCutIndex );
			unsigned int index1 = index0 + 1;
			unsigned int index2 = index0 + horizintalCuts + 1;
			unsigned int index3 = index2 + 1;
			indices.push_back( index0 );
			indices.push_back( index1 );
			indices.push_back( index3 );
			indices.push_back( index0 );
			indices.push_back( index3 );
			indices.push_back( index2 );
		}
	}
}
