#pragma once
#include "Engine/Core/AABB2.hpp"
class Texture;

class Object
{
public:
	Object();
	~Object();

	void Update( float deltaSeconds );
	void Render();
	
	Texture* m_texture = nullptr;
	AABB2 m_objectBox;
};