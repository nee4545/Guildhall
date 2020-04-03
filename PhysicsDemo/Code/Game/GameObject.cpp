#include "Game/GameObject.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/Collider2D.hpp"

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

	//float value = RangeMapFloat( 0.f , 1.f , 25.f , 255.f , this->m_rigidbody->m_collider->m_material.bounciness );
	//m_fillColor = Rgba8( ( unsigned char ) value , ( unsigned char ) value , ( unsigned char ) value , 128 );
}
