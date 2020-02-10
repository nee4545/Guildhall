#pragma once
#include "Engine/Core/Rgba8.hpp"
class Rigidbody2D;

class GameObject
{
public:
	
	Rigidbody2D* m_rigidbody = nullptr;
	GameObject() {};
	~GameObject();

	Rgba8 m_borderColor = Rgba8( 0 , 0 , 255 , 255 );
	Rgba8 m_fillColor = Rgba8( 255 , 255 , 255 , 128 );

	bool isBeingDragged = false;
	bool isMouseInside = false;
	bool isColliding = false;

	void UpdateColliderColors( Rgba8 borderColor , Rgba8 fillColor );
	void UpdateColorsBasedOnStatus();
	
};