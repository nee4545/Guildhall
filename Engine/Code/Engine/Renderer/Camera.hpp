#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

enum  eCameraClearBitFlag : unsigned int
{
	CLEAR_COLOR_BIT =(1 << 0),
	CLEAR_DEPTH_BIT =(1 << 1 ),
	CLEAR_STENCIL_BIT =(1 << 2 )

};

class Camera
{
private:
	Vec2 bottom_Left;
	Vec2 top_Right;
	unsigned int m_clearMode = 0;
	Rgba8 m_clearColor;
public:
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;

	void Translate2D(Vec2 translation2D);
	void SetClearMode( unsigned int clearFlags , Rgba8 color , float depth=0.f , unsigned int stencil=0.f );
	Rgba8 GetClearColor() const;

	Camera()
	{
		bottom_Left=Vec2(0.0f,0.0f);
		top_Right=Vec2(0.0f,0.0f);
	}

};