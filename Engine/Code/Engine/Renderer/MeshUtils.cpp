#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/MathUtils.hpp"


void AddUVSphereToIndexedVertexArray( std::vector<Vertex_PCU>& verts , std::vector<unsigned int>& indices , Vec3& center , float radius , unsigned int horizintalCuts , unsigned int verticalCuts , Rgba8& color )
{
	float deltaTheta = 360.f / ( float ) horizintalCuts;
	float deltaPhi = 180.f / ( float ) verticalCuts;

	float x , y , z , xy = 0.f;
	float u , v = 0.f;

	float hAngle = 0.f;
	float vAngle = 0.f;

	for ( int i = 0; i <= (int)verticalCuts; i++ )
	{
		vAngle = 90.f - i * deltaPhi;
		xy = radius * CosDegrees( vAngle );
		z = radius * SinDegrees( vAngle );

		for ( int j = 0; j <= (int)horizintalCuts; j++ )
		{
			hAngle = j * deltaTheta;

			x = xy * CosDegrees( hAngle );
			y = xy * SinDegrees( hAngle );

			u = ( float ) j / horizintalCuts;
			v = ( float ) i / verticalCuts;

			verts.push_back( Vertex_PCU( Vec3( x , y , z ) + center , color , Vec2( u , v ) ) );
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
