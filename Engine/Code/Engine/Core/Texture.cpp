#include "Engine/Core/Texture.hpp"



Texture::Texture( const char* filePath, unsigned int texID, IntVec2 size ):imageFilePath(filePath),textureID(texID),dimensions(size)
{
	
}

unsigned int Texture::GetTextureID() const
{
	return textureID;
}
