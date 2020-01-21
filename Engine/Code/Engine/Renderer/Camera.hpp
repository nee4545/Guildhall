#pragma once
#include "Engine/Math/Vec2.hpp"

class Camera
{
private:
	Vec2 bottom_Left;
	Vec2 top_Right;
public:
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;

	void Translate2D(Vec2 translation2D);

	Camera()
	{
		bottom_Left=Vec2(0.0f,0.0f);
		top_Right=Vec2(0.0f,0.0f);
	}

};