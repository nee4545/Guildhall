#include "Game/GameObject.hpp"
#include "Engine/Physics/RigidBody2D.hpp"

GameObject::~GameObject()
{
	m_rigidbody->Destroy();
	m_rigidbody = nullptr;
}

void GameObject::UpdateColliderColors( Rgba8 borderColor , Rgba8 fillColor )
{
	m_borderColor = borderColor;
	m_fillColor = fillColor;
}

void GameObject::UpdateColorsBasedOnStatus()
{
	if ( isMouseInside )
	{
		m_borderColor = Rgba8( 247 , 202 , 25 , 255 );
	}

	if ( isBeingDragged )
	{
		m_borderColor = Rgba8( 0 , 255 , 0 , 255 );
	}

	if ( isColliding )
	{
		m_fillColor = Rgba8( 255 , 0 , 0 , 128 );
	}

	if ( !isMouseInside && !isBeingDragged )
	{
		m_borderColor = Rgba8( 0 , 0 , 255 , 255 );
	}

	if ( !isColliding )
	{
		m_fillColor = Rgba8( 255 , 255 , 255 , 128 );
	}
}
