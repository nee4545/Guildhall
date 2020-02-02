#include "Camera.hpp"
#define UNUSED(x) (void)(x);

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

void Camera::SetOutputsize( Vec2 size )
{
	m_outpitsize = size;
}



void Camera::SetPosition( Vec2 position )
{
	m_pos = position;
	
}

void Camera::SetOrthoViewForCameraPosition()
{
	bottom_Left = m_pos - m_outpitsize / 2.f;
	top_Right = m_pos + m_outpitsize / 2.f;
}

void Camera::SetProjectionOrthographic( float height , float nearZ /*= -1.f */ , float farZ /*= 1.f */ )
{
	m_outpitsize.x = height * GetAspectRatio();
	m_outpitsize.y = height;
	UNUSED( nearZ );
	UNUSED( farZ );
}

float Camera::GetAspectRatio() const
{
	return m_outpitsize.x / m_outpitsize.y;
}

void Camera::Translate2D( Vec2 translation2D )
{
	bottom_Left+=translation2D;
	top_Right+=translation2D;
}
