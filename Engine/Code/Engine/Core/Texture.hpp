#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <string>
struct ID3D11Texture2D;
class RenderContext;
class TextureView;

class Texture
{
	

private:
    const char* imageFilePath =nullptr;
	unsigned int textureID = 0;
	RenderContext* m_owner = nullptr;
	ID3D11Texture2D* m_handle = nullptr;
	TextureView* m_renderTargetView = nullptr;
	TextureView* m_shaderResourceView = nullptr;
	IntVec2 m_texelSizeCoords;
	IntVec2 m_dimensions;
	

public:
	Texture(){};
	~Texture();
	Texture(const char* filePath, unsigned int textureID, IntVec2 size);
	explicit Texture( const char* filePath , RenderContext* ctx , ID3D11Texture2D* handle );
	Texture( RenderContext* ctx , ID3D11Texture2D* handle );
	TextureView* GetRenderTargetView();
	TextureView* GetOrCreateShaderResourceView();
	unsigned int GetTextureID()const;
	const IntVec2 GetDimensions() const { return m_dimensions;}

	IntVec2 GetTexelSizeCoords() const;
};