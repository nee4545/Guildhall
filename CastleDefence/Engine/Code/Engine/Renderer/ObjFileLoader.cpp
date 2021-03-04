#include "Engine/Renderer/ObjFileLoader.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Math\Vec3.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Renderer\GPUMesh.hpp"
#include "Engine\Renderer\RenderContext.hpp"
#include "Engine\Core\VertexLit.hpp"
#include "Engine\ThirdParty\MikktSpace.h"
#include "Engine\Math\MathUtils.hpp"
#include <fstream>
#define UNUSED(x) (void)(x);

extern RenderContext* g_theRenderer;

Strings GetTrimmedStrings( std::string& s )
{
	Strings temp = SplitStringOnDelimiter( s , ' ' );
	Strings toReturn;

	for ( int i = 0; i < temp.size(); i++ )
	{
		if ( temp[ i ] != "" )
		{
			toReturn.push_back( temp[i] );
		}
	}

	return toReturn;
}

static int GetNumFaces( SMikkTSpaceContext const* context )
{
	std::vector<VertexLit>& vertices = *( std::vector<VertexLit>* )( context->m_pUserData );
	return (int)(vertices.size() / 3);
}

static int GetNumVerticesOfFace( SMikkTSpaceContext const* context, int iFace )
{
	UNUSED( context );
	UNUSED( iFace );
	return 3;
}

static void GetPositionForFaceVert( const SMikkTSpaceContext* pContext , float fvPosOut[] , const int iFace , const int iVert )
{
	std::vector<VertexLit>& vertices = *( std::vector<VertexLit>* )( pContext->m_pUserData );
	int indexInVertexArray = iFace * 3 + iVert;

	// get our posistion
	Vec3 ourPos = vertices[ indexInVertexArray ].m_position;

	fvPosOut[ 0 ] = ourPos.x;
	fvPosOut[ 1 ] = ourPos.y;
	fvPosOut[ 2 ] = ourPos.z;
}

static void GetNormalForFaceVert( const SMikkTSpaceContext* pContext , float fvNormOut[] , const int iFace , const int iVert )
{
	std::vector<VertexLit>& vertices = *( std::vector<VertexLit>* )( pContext->m_pUserData );
	int indexInVertexArray = iFace * 3 + iVert;

	Vec3 ourPos = vertices[ indexInVertexArray ].m_normal;

	fvNormOut[ 0 ] = ourPos.x;
	fvNormOut[ 1 ] = ourPos.y;
	fvNormOut[ 2 ] = ourPos.z;
}

static void GetUVForFaceVert( const SMikkTSpaceContext* pContext , float fvTexcOut[] , const int iFace , const int iVert )
{
	std::vector<VertexLit>& vertices = *( std::vector<VertexLit>* )( pContext->m_pUserData );
	int indexInVertexArray = iFace * 3 + iVert;
	
	Vec2 ourPos = vertices[ indexInVertexArray ].m_uv;

	fvTexcOut[ 0 ] = ourPos.x;
	fvTexcOut[ 1 ] = ourPos.y;

}

static void SetTangent( const SMikkTSpaceContext* pContext ,
	const float fvTangent[] ,
	const float fSign ,
	const int iFace , const int iVert )
{
	std::vector<VertexLit>& vertices = *( std::vector<VertexLit>* )( pContext->m_pUserData );
	int indexInVertexArray = iFace * 3 + iVert;

	vertices[ indexInVertexArray ].m_tangent = Vec4( fvTangent[ 0 ] , fvTangent[ 1 ] , fvTangent[ 2 ] , fSign );
}


void GenereateTangentsForVertexArray( std::vector<VertexLit>& vertices )
{
	SMikkTSpaceInterface interface;

	interface.m_getNumFaces = GetNumFaces;
	interface.m_getNumVerticesOfFace = GetNumVerticesOfFace;

	interface.m_getPosition = GetPositionForFaceVert;
	interface.m_getNormal = GetNormalForFaceVert;
	interface.m_getTexCoord = GetUVForFaceVert;

	interface.m_setTSpaceBasic = SetTangent;
	interface.m_setTSpace = nullptr;

	SMikkTSpaceContext context;
	context.m_pInterface = &interface;
	context.m_pUserData = &vertices;

	genTangSpaceDefault( &context );
}

OBJLoader::OBJLoader()
{

}

void OBJLoader::ParseObjFile( std::string filePath, std::vector<std::string>& out )
{
	std::ifstream file;
	std::string line;

	file.open( filePath );

	if ( file.is_open() )
	{
		while ( getline( file , line ) )
		{
			out.push_back( line );
		}

		file.close();
	}
	else
	{
		DebuggerPrintf( "Error in reading file" );
		DEBUGBREAK();
	}

}

GPUMesh* OBJLoader::LoadObjFileIntoGpuMesh( MeshBuilderOptions options , std::string objFilePath )
{
	OBJLoader temp = OBJLoader();
	std::vector<std::string> linesOfObjFile;

	temp.ParseObjFile( "Data/Objs/test_models/scifi_fighter/mesh.obj" , linesOfObjFile );

	std::vector<Vec3> postions;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvs;

	GPUMesh* mesh = new GPUMesh( g_theRenderer );

	std::vector<VertexLit> finalVerts;

	std::vector<std::vector<int>> iForVerts;
	std::vector<std::vector<int>> iForNormals;
	std::vector<std::vector<int>> iForTex;



	for ( long i = 0; i < linesOfObjFile.size(); i++ )
	{
		if ( linesOfObjFile[ i ] == "")
		{
			continue;
		}

	
		Strings s = GetTrimmedStrings( linesOfObjFile[ i ]);
		
		if ( s[ 0 ] == "#" )
		{
			continue;
		}

		if ( s[ 0 ] == "v" )
		{
			Vec3 vec;
			vec.x = (float)atof( s[ 1 ].c_str() );
			vec.y = (float)atof( s[ 2 ].c_str() );
			vec.z = (float)atof( s[ 3 ].c_str() );
			postions.push_back( vec );
		}

		if ( s[ 0 ] == "vn" )
		{
			Vec3 vec;
			vec.x = (float)atof( s[ 1 ].c_str() );
			vec.y = (float)atof( s[ 2 ].c_str() );
			vec.z = (float)atof( s[ 3 ].c_str() );
			normals.push_back( vec );
		}

		if ( s[ 0 ] == "vt" )
		{
			Vec2 vec;
			vec.x = ( float ) atof( s[ 1 ].c_str() );
			vec.y = ( float ) atof( s[ 2 ].c_str() );
			uvs.push_back( vec );
		}

		if ( s[ 0 ] == "f" )
		{
			if ( s.size() == 5 )
			{
				Strings t1 = SplitStringOnDelimiter( s[ 1 ] , '/' );

				Strings t2 = SplitStringOnDelimiter( s[ 2 ] , '/' );

				Strings t3 = SplitStringOnDelimiter( s[ 3 ] , '/' );

				Strings t4 = SplitStringOnDelimiter( s[ 4 ] , '/' );

				std::vector<int> x1;
				std::vector<int> x2;
				std::vector<int> x3;

				x1.push_back( atoi( t1[ 0 ].c_str() ) );
				x1.push_back( atoi( t2[ 0 ].c_str() ) );
				x1.push_back( atoi( t3[ 0 ].c_str() ) );
				x1.push_back( atoi( t4[ 0 ].c_str() ) );
			
				x2.push_back( atoi( t1[ 1 ].c_str() ) );
				x2.push_back( atoi( t2[ 1 ].c_str() ) );
				x2.push_back( atoi( t3[ 1 ].c_str() ) );
				x2.push_back( atoi( t4[ 1 ].c_str() ) );

				x3.push_back( atoi( t1[ 2 ].c_str() ) );
				x3.push_back( atoi( t2[ 2 ].c_str() ) );
				x3.push_back( atoi( t3[ 2 ].c_str() ) );
				x3.push_back( atoi( t4[ 2 ].c_str() ) );

				iForVerts.push_back( x1 );
				iForTex.push_back( x2 );
				iForNormals.push_back( x3 );


			}
			else if ( s.size() == 4 )
			{
				Strings t1 = SplitStringOnDelimiter( s[ 1 ] , '/' );

				Strings t2 = SplitStringOnDelimiter( s[ 2 ] , '/' );

				Strings t3 = SplitStringOnDelimiter( s[ 3 ] , '/' );

				std::vector<int> x1;
				std::vector<int> x2;
				std::vector<int> x3;

				x1.push_back( atoi( t1[ 0 ].c_str() ) );
				x1.push_back( atoi( t2[ 0 ].c_str() ) );
				x1.push_back( atoi( t3[ 0 ].c_str() ) );
			
				x2.push_back( atoi( t1[ 1 ].c_str() ) );
				x2.push_back( atoi( t2[ 1 ].c_str() ) );
				x2.push_back( atoi( t3[ 1 ].c_str() ) );
				
				x3.push_back( atoi( t1[ 2 ].c_str() ) );
				x3.push_back( atoi( t2[ 2 ].c_str() ) );
				x3.push_back( atoi( t3[ 2 ].c_str() ) );
			

				iForVerts.push_back( x1 );
				iForTex.push_back( x2 );
				iForNormals.push_back( x3 );
				
			}
			else
			{
				DebuggerPrintf( "Error in obj definition" );
				DEBUGBREAK();
			}

		}

	}

	for ( int i = 0; i < iForVerts.size(); i++ )
	{
		if ( iForVerts[ i ].size() == 4 )
		{
			VertexLit v1;
			VertexLit v2;
			VertexLit v3;
			VertexLit v4;

			v1.m_color = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedVec4();
			v2.m_color = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedVec4();
			v3.m_color = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedVec4();
			v4.m_color = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedVec4();

			v1.m_position = postions[ (iForVerts[ i ][ 0 ])-1 ];
			v2.m_position = postions[ (iForVerts[ i ][ 1 ])-1 ];
			v3.m_position = postions[ (iForVerts[ i ][ 2 ])-1 ];
			v4.m_position = postions[ (iForVerts[ i ][ 3 ])-1 ];

			v1.m_position = options.transform.TransformPosition3D( v1.m_position );
			v2.m_position = options.transform.TransformPosition3D( v2.m_position );
			v3.m_position = options.transform.TransformPosition3D( v3.m_position );
			v4.m_position = options.transform.TransformPosition3D( v4.m_position );

			v1.m_uv = uvs[ ( iForTex[ i ][ 0 ] ) - 1 ];
			v2.m_uv = uvs[ ( iForTex[ i ][ 1 ] ) - 1 ];
			v3.m_uv = uvs[ ( iForTex[ i ][ 2 ] ) - 1 ];
			v4.m_uv = uvs[ ( iForTex[ i ][ 3 ] ) - 1 ];

			if ( options.invertV )
			{
				v1.m_uv.y = 1.f - v1.m_uv.y;
				v2.m_uv.y = 1.f - v2.m_uv.y;
				v3.m_uv.y = 1.f - v3.m_uv.y;
				v4.m_uv.y = 1.f - v4.m_uv.y;
			}

			if ( iForNormals.size() > 0 )
			{
				v1.m_normal = normals[ ( iForNormals[ i ][ 0 ] ) - 1 ];
				v2.m_normal = normals[ ( iForNormals[ i ][ 1 ] ) - 1 ];
				v3.m_normal = normals[ ( iForNormals[ i ][ 2 ] ) - 1 ];
				v4.m_normal = normals[ ( iForNormals[ i ][ 3 ] ) - 1 ];

				v1.m_normal = options.transform.TransformPosition3D( v1.m_normal );
				v2.m_normal = options.transform.TransformPosition3D( v2.m_normal );
				v3.m_normal = options.transform.TransformPosition3D( v3.m_normal );
				v4.m_normal = options.transform.TransformPosition3D( v4.m_normal );

			}

			
			if ( options.inverWindindOrder )
			{
				finalVerts.push_back( v1 );
				finalVerts.push_back( v4 );
				finalVerts.push_back( v3 );

				finalVerts.push_back( v4 );
				finalVerts.push_back( v2 );
				finalVerts.push_back( v1 );
			}
			else
			{
				finalVerts.push_back( v1 );
				finalVerts.push_back( v2 );
				finalVerts.push_back( v3 );

				finalVerts.push_back( v3 );
				finalVerts.push_back( v4 );
				finalVerts.push_back( v1 );
			}

			

		}
		else if ( iForVerts[ i ].size() == 3 )
		{
			VertexLit v1;
			VertexLit v2;
			VertexLit v3;

			v1.m_color = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedVec4();
			v2.m_color = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedVec4();
			v3.m_color = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedVec4();

			v1.m_position = postions[ (iForVerts[ i ][ 0 ]) - 1 ];
			v2.m_position = postions[ (iForVerts[ i ][ 1 ]) - 1 ];
			v3.m_position = postions[ (iForVerts[ i ][ 2 ]) - 1 ];

			v1.m_position = options.transform.TransformPosition3D( v1.m_position );
			v2.m_position = options.transform.TransformPosition3D( v2.m_position );
			v3.m_position = options.transform.TransformPosition3D( v3.m_position );

			if ( iForNormals.size() > 0 )
			{
				v1.m_normal = normals[ ( iForNormals[ i ][ 0 ] ) - 1 ];
				v2.m_normal = normals[ ( iForNormals[ i ][ 1 ] ) - 1 ];
				v3.m_normal = normals[ ( iForNormals[ i ][ 2 ] ) - 1 ];

				v1.m_normal = options.transform.TransformPosition3D( v1.m_normal );
				v2.m_normal = options.transform.TransformPosition3D( v2.m_normal );
				v3.m_normal = options.transform.TransformPosition3D( v3.m_normal );

			}

			v1.m_uv = uvs[ ( iForTex[ i ][ 0 ] ) - 1 ];
			v2.m_uv = uvs[ ( iForTex[ i ][ 1 ] ) - 1 ];
			v3.m_uv = uvs[ ( iForTex[ i ][ 2 ] ) - 1 ];

			if ( options.invertV )
			{
				v1.m_uv.y = 1.f - v1.m_uv.y;
				v2.m_uv.y = 1.f - v2.m_uv.y;
				v3.m_uv.y = 1.f - v3.m_uv.y;
			}

			
			if ( options.inverWindindOrder )
			{
				finalVerts.push_back( v1 );
				finalVerts.push_back( v3 );
				finalVerts.push_back( v2 );
			}
			else
			{
				finalVerts.push_back( v1 );
				finalVerts.push_back( v2 );
				finalVerts.push_back( v3 );
			}
		}

	}

	if ( options.calculateNoramls )
	{
		for ( int i = 0; i < finalVerts.size(); i+=3 )
		{
			Vec3 normal = CrossProduct3D( finalVerts[ i + 1 ].m_position - finalVerts[ i ].m_position , finalVerts[ i + 2 ].m_position - finalVerts[ i ].m_position );
			finalVerts[ i ].m_normal = normal;
			finalVerts[ i+1 ].m_normal = normal;
			finalVerts[ i+2 ].m_normal = normal;
		}
	}

	if ( options.generateTangents )
	{
		GenereateTangentsForVertexArray( finalVerts );
	}

	finalVerts;

	mesh->UpdateVertices( ( unsigned int ) finalVerts.size() , &finalVerts[ 0 ] );

	return mesh;
}
