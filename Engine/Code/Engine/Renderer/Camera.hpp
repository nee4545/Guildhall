#pragma once
#include "Engine/Math/Vec2.hpp"

class Camera
{
private:
	Vec2 bottom_Left;
	Vec2 top_Right;
	Vec2 m_outpitsize;
	Vec2 m_pos;


public:
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	void SetOutputsize( Vec2 size );
	float GetCameraHeight();
	void SetPosition( Vec2 position );
	Vec2 GetPosition() { return m_pos; }
	Vec2 GetOutputSize() { return m_outpitsize; }
	void SetOrthoViewForCameraPosition();
	void SetProjectionOrthographic( float height , float nearZ = -1.f , float farZ = 1.f );
	float GetAspectRatio() const;
	Vec2 ClientToWorldPosition( Vec2 clientPos );

	void Translate2D(Vec2 translation2D);

	Camera()
	{
		bottom_Left=Vec2(0.0f,0.0f);
		top_Right=Vec2(0.0f,0.0f);
	}

};