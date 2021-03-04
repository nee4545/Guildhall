#pragma once
#include "Engine/Math/Mat44.hpp"
#include <string>
#include <vector>

class GPUMesh;

struct MeshBuilderOptions
{
	Mat44 transform;
	bool invertV = false;
	bool calculateNoramls = false;
	bool generateTangents = false;
	bool inverWindindOrder = false;
};

class OBJLoader
{
public:
	OBJLoader();
	~OBJLoader() {};
	void ParseObjFile( std::string filePath, std::vector<std::string>& out );
	static GPUMesh* LoadObjFileIntoGpuMesh( MeshBuilderOptions options , std::string objFilePath );

private:

};