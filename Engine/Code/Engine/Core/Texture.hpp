#pragma once
#include "Engine/Math/IntVec2.hpp"


class Texture
{
	friend class RenderContext;

private:
    const char* imageFilePath =nullptr;
	unsigned int textureID = 0;
	IntVec2 dimensions;
	

public:
	Texture(){};
	~Texture(){};
	Texture(const char* filePath, unsigned int textureID, IntVec2 size);
	unsigned int GetTextureID()const;
	const IntVec2 GetDimensions() const { return dimensions;}
};