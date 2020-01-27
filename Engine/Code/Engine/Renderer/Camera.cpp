#include "Camera.hpp"
#include "Engine/Core/Rgba8.hpp"

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	this->bottom_Left.x=bottomLeft.x;
	this->bottom_Left.y=bottomLeft.y;
	this->top_Right.x=topRight.x;
	this->top_Right.y=topRight.y;
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return bottom_Left;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return top_Right;
}

void Camera::Translate2D( Vec2 translation2D )
{
	bottom_Left+=translation2D;
	top_Right+=translation2D;
}

void Camera::SetClearMode( unsigned int clearFlags , Rgba8 color , float depth/*=0.f */ , unsigned int stencil/*=0.f */ )
{
	m_clearColor = color;
	m_clearMode = clearFlags;
}
