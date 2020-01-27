#pragma once
#include "Engine/Math/IntVec2.hpp"

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
	IntVec2 dimensions;
	

public:
	Texture(){};
	~Texture();
	Texture(const char* filePath, unsigned int textureID, IntVec2 size);
	Texture( RenderContext* ctx , ID3D11Texture2D* handle );
	TextureView* GetRenderTargetView();
	unsigned int GetTextureID()const;
	const IntVec2 GetDimensions() const { return dimensions;}
};