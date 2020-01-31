#pragma once

#include "Engine/Math/Vec2.hpp"
#include "string"

class RenderContext;
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11RasterizerState;
struct ID3D10Blob;

enum eShaderType
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT,
};

class ShaderStage
{

public:
	bool Complie( RenderContext* ctx ,
		std::string const& filename ,
		void const* source ,
		size_t const sourceByteLen ,
		eShaderType stage );

	bool IsValid();

public:
	eShaderType m_type = SHADER_TYPE_VERTEX;
	ID3D10Blob* m_byteCode = nullptr;

	union
	{
		ID3D11Resource* m_handle;
		ID3D11VertexShader* m_vs;
		ID3D11PixelShader* m_fs;

	};

	~ShaderStage();

};

class Shader
{
public:
	Shader( RenderContext* context );
	~Shader();
	bool CreateFromFile( std::string const& filename );
	void CreateRasterState();

public:
	RenderContext* m_owner;
	ShaderStage m_vertexStage;
	ShaderStage m_fragmentStage;

	ID3D11RasterizerState* m_rasterState = nullptr;
};